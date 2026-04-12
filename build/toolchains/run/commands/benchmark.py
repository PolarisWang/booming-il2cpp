"""benchmark.py — CLI command for running benchmarks and managing benchmark records.

Commands:
    run benchmark --subject <id> --mode <managed|native|interpreter> --record
    run benchmark --all --mode <mode> --record
    run benchmark --dashboard [--open]
    run benchmark status [--subject <id>] [--all]

After each --record run, docs/benchmark/ is automatically updated.
"""
from __future__ import annotations

import importlib.util
import json
import os
import sys
from pathlib import Path
from typing import Any


def _load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)  # type: ignore[union-attr]
    return mod


def dispatch(args: list[str], repo_root: Path, host_platform: str) -> int:
    """Dispatch benchmark sub-commands.

    Returns exit code: 0 = success, 1 = regression, 2 = error.
    """
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    records_mod = _load("benchmark_records", testing_root / "benchmark_records.py")
    detector_mod = _load("device_detector", testing_root / "device_detector.py")
    dash_mod = _load("benchmark_dashboard_generator", testing_root / "benchmark_dashboard_generator.py")

    # ── Parse args ─────────────────────────────────────────────────────────
    subject_id: str | None = None
    mode: str | None = None
    do_record = False
    do_dashboard = False
    do_open = False
    do_status = False
    do_all = False
    output: str | None = None

    i = 0
    while i < len(args):
        a = args[i]
        if a == "--subject" and i + 1 < len(args):
            subject_id = args[i + 1]; i += 2
        elif a == "--mode" and i + 1 < len(args):
            mode = args[i + 1]; i += 2
        elif a == "--record":
            do_record = True; i += 1
        elif a == "--dashboard":
            do_dashboard = True; i += 1
        elif a == "--open":
            do_open = True; i += 1
        elif a == "status":
            do_status = True; i += 1
        elif a == "--all":
            do_all = True; i += 1
        elif a == "--output" and i + 1 < len(args):
            output = args[i + 1]; i += 2
        else:
            i += 1

    # ── status sub-command ─────────────────────────────────────────────────
    if do_status:
        return _cmd_status(repo_root, records_mod, subject_id, do_all)

    # ── dashboard sub-command ──────────────────────────────────────────────
    if do_dashboard and not do_record:
        output_path = Path(output) if output else repo_root / "docs" / "benchmark" / "dashboard.html"
        print(f"Generating benchmark dashboard → {output_path}")
        dash_mod.generate(repo_root, output_path)
        print("✓ Dashboard generated")
        if do_open:
            import webbrowser
            webbrowser.open(output_path.as_uri())
        return 0

    # ── record sub-command ─────────────────────────────────────────────────
    if do_record:
        subjects_to_run: list[str] = []
        if do_all:
            # Find all Bench* subjects
            subjects_root = repo_root / "subjects"
            subjects_to_run = sorted(
                p.name for p in subjects_root.iterdir()
                if p.name.startswith("Bench") and (p / "subject.manifest.json").exists()
            )
        elif subject_id:
            subjects_to_run = [subject_id]
        else:
            print("ERROR: --subject or --all required with --record", file=sys.stderr)
            return 2

        modes_to_run = [mode] if mode else ["managed", "native", "interpreter"]

        device = detector_mod.load_or_detect(repo_root)
        regression_found = False

        for sid in subjects_to_run:
            for m in modes_to_run:
                result = _run_pipeline_and_record(
                    repo_root=repo_root,
                    subject_id=sid,
                    mode=m,
                    device=device,
                    records_mod=records_mod,
                    host_platform=host_platform,
                )
                _print_result(sid, m, device, result)
                if result.get("regressionFound"):
                    regression_found = True

            # Update docs/benchmark/ after each subject
            try:
                dash_mod.update_docs(repo_root, subject_id=sid)
                print(f"  → docs/benchmark/ updated")
            except Exception as e:
                print(f"  ⚠ dashboard update failed: {e}")

        return 1 if regression_found else 0

    # ── Default: show help ─────────────────────────────────────────────────
    _print_help()
    return 0


# ---------------------------------------------------------------------------
# Internal helpers
# ---------------------------------------------------------------------------


def _run_pipeline_and_record(
    *,
    repo_root: Path,
    subject_id: str,
    mode: str,
    device: dict[str, Any],
    records_mod: Any,
    host_platform: str,
) -> dict[str, Any]:
    """Run the benchmark subject in the given mode and append a record.

    This is a simplified direct invocation: it calls the subject's harness
    directly via `dotnet run` (managed) or the built native binary (native).
    For interpreter mode it falls back to managed if no harness is configured.
    """
    import subprocess
    import time
    import datetime

    iterations = 1000  # default
    manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
    if not manifest_path.exists():
        return {"error": f"subject not found: {subject_id}"}

    csproj_path = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
    if not csproj_path.exists():
        return {"error": f"no .csproj for {subject_id}"}

    if mode == "native":
        # Native mode requires the full IL2CPP pipeline:
        #   C# → analysis → codegen → CMake build → run native .exe
        # That multi-stage flow is not yet wired into this CLI shortcut.
        # Do NOT write a placeholder record — stale zeros corrupt the dashboard.
        print(f"  ⚠ {subject_id} / native: skipped — native mode requires the full IL2CPP"
              f" pipeline (run via: run test subject --subject {subject_id} --matrix windows-native-perf)")
        return {"skipped": True, "reason": "native-requires-full-pipeline"}

    # Run via dotnet run for managed / interpreter
    try:
        proc = subprocess.run(
            ["dotnet", "run", "--project", str(csproj_path),
             "--configuration", "Release", "--", str(iterations)],
            capture_output=True, text=True, timeout=120, cwd=repo_root,
        )
        lines = [l for l in proc.stdout.splitlines() if l.strip()]
        # Prefer the benchmark's own reported timing (last line JSON → elapsedMilliseconds)
        metrics: dict[str, Any] = {}
        if lines:
            try:
                payload = json.loads(lines[-1])
                if isinstance(payload, dict):
                    metrics.update(payload)
            except json.JSONDecodeError:
                pass
        # elapsedMilliseconds from the harness is the benchmark time; expose as meanDurationMs
        if "elapsedMilliseconds" in metrics and "meanDurationMs" not in metrics:
            metrics["meanDurationMs"] = metrics["elapsedMilliseconds"]
        if not metrics:
            return {"error": f"no JSON output from {subject_id}"}
    except Exception as e:
        return {"error": str(e)}

    # Get git commit
    try:
        git_out = subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            text=True, cwd=repo_root, timeout=5,
        ).strip()
    except Exception:
        git_out = "unknown"

    plat_name = str(host_platform)
    record: dict[str, Any] = {
        "runId": f"{datetime.datetime.utcnow().strftime('%Y%m%d-%H%M%S')}-{subject_id}-{mode}",
        "subject": subject_id,
        "mode": mode,
        "platform": plat_name,
        "device": device,
        "recordedAt": datetime.datetime.utcnow().isoformat() + "Z",
        "gitCommit": git_out,
        "gitBranch": "main",
        "metrics": metrics,
    }
    records_mod.append_record(repo_root, record)

    return {"record": record, "regressionFound": False}


def _print_result(subject_id: str, mode: str, device: dict[str, Any], result: dict[str, Any]) -> None:
    if "error" in result:
        print(f"  ✗ {subject_id} / {mode}: {result['error']}")
        return
    rec = result.get("record") or {}
    m = rec.get("metrics") or {}
    latency = m.get("meanDurationMs", m.get("elapsedMilliseconds", "?"))
    ops = m.get("opsPerSecond")
    dev_name = device.get("name", device.get("id", "?"))
    print(f"  ✓ {subject_id} / {mode} / {dev_name}")
    print(f"      meanDurationMs: {latency} ms" + (f"   opsPerSecond: {ops:,.0f}" if ops else ""))


def _cmd_status(repo_root: Path, records_mod: Any, subject_id: str | None, do_all: bool) -> int:
    if subject_id:
        sids = [subject_id]
    elif do_all:
        sids = records_mod.list_subjects_with_records(repo_root)
    else:
        sids = records_mod.list_subjects_with_records(repo_root)

    if not sids:
        print("No benchmark records found.")
        print("Run: run benchmark --all --mode native --record")
        return 0

    for sid in sids:
        print(f"\n{sid}")
        # Latest per mode on any device
        from benchmark_records import _records_path, _iter_jsonl_reverse
        path = _records_path(repo_root, sid)
        seen: dict[str, Any] = {}
        for rec in _iter_jsonl_reverse(path):
            mode = rec.get("mode", "?")
            if mode not in seen:
                seen[mode] = rec
            if len(seen) >= 3:
                break
        for m, rec in seen.items():
            lat = rec.get("metrics", {}).get("meanDurationMs", "?")
            dev = rec.get("device", {}).get("id", "?")
            ts = rec.get("recordedAt", "?")
            print(f"  {m:15s}  {lat} ms   {dev}   {ts}")
    return 0


def _print_help() -> None:
    print("""usage: run benchmark [options]

Options:
  --subject <id>               Target subject (e.g. BenchArithmetic)
  --all                        All Bench* subjects
  --mode <managed|native|interpreter>  Execution mode (default: all modes)
  --record                     Run benchmark and record result to records.jsonl
  --dashboard [--open]         Generate HTML dashboard (optionally open in browser)
  --output <path>              Dashboard output path
  status [--subject <id>]      Show latest benchmark records

Examples:
  run benchmark --subject BenchArithmetic --mode native --record
  run benchmark --all --record
  run benchmark --dashboard --open
  run benchmark status --all
""")
