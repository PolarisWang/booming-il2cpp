"""benchmark.py 鈥?CLI command for running benchmarks and managing benchmark records.

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
import time
from pathlib import Path
from typing import Any


def _load(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)  # type: ignore[union-attr]
    return mod


def _normalize_host_platform(host_platform: str) -> str:
    value = str(host_platform or "").lower()
    if value.startswith("windows"):
        return "windows"
    if value.startswith("macos"):
        return "macos"
    if value.startswith("linux"):
        return "linux"
    return value


def _select_benchmark_matrix_id(
    manifest: dict[str, Any],
    *,
    mode: str,
    host_platform: str,
) -> str:
    platform_key = _normalize_host_platform(host_platform)
    desired_terms = {
        "managed": ("managed-benchmark", "managed-perf", "managed"),
        "native": ("native-benchmark", "native-perf", "native"),
        "interpreter": ("interpreter-benchmark", "interpreter-perf", "interpreter"),
    }.get(mode, (mode,))

    best_matrix_id: str | None = None
    best_score = -1

    for matrix in list(manifest.get("environmentMatrices") or []):
        matrix_payload = dict(matrix)
        matrix_id = str(matrix_payload.get("matrixId") or "")
        pipeline_id = str(matrix_payload.get("pipelineId") or "")
        execution_context = dict(matrix_payload.get("executionContext") or {})
        host_value = str(execution_context.get("hostPlatform") or "")
        runtime_profile = str(execution_context.get("runtimeProfile") or "")
        haystack = " ".join((matrix_id, pipeline_id, runtime_profile)).lower()

        if not any(term in haystack for term in desired_terms):
            continue

        score = 0
        if platform_key and matrix_id.lower().startswith(platform_key):
            score += 4
        if platform_key and host_value.lower().startswith(platform_key):
            score += 3
        if any(term == pipeline_id.lower() for term in desired_terms):
            score += 3
        if any(term in runtime_profile.lower() for term in desired_terms):
            score += 2
        if "benchmark" in pipeline_id.lower():
            score += 1

        if score > best_score:
            best_score = score
            best_matrix_id = matrix_id

    if best_matrix_id:
        return best_matrix_id

    raise ValueError(
        f"no {mode} benchmark matrix configured for subject '{manifest.get('subjectId') or '?'}'"
    )


def _extract_runtime_performance(
    stage_results: list[dict[str, Any]],
    *,
    preferred_kind: str,
) -> dict[str, Any] | None:
    for stage_result in reversed(stage_results):
        if str(stage_result.get("kind") or "") != preferred_kind:
            continue
        performance = dict(dict(stage_result.get("details") or {}).get("performance") or {})
        metrics = dict(performance.get("metrics") or {})
        if metrics:
            return {
                "metrics": metrics,
                "regressionStatus": str(performance.get("regressionStatus") or "no-baseline"),
            }

    for stage_result in reversed(stage_results):
        if str(stage_result.get("bucket") or "") != "runtime":
            continue
        performance = dict(dict(stage_result.get("details") or {}).get("performance") or {})
        metrics = dict(performance.get("metrics") or {})
        if metrics:
            return {
                "metrics": metrics,
                "regressionStatus": str(performance.get("regressionStatus") or "no-baseline"),
            }

    return None


def _preferred_runtime_stage_kind(mode: str) -> str:
    return {
        "managed": "runtime-perf-collect",
        "native": "native-runtime-perf",
        "interpreter": "interpreter-runtime-perf",
    }.get(mode, "runtime")


def _run_subject_benchmark_pipeline(
    *,
    repo_root: Path,
    subject_id: str,
    mode: str,
    host_platform: str,
    subjects_module: Any | None = None,
    executor_module: Any | None = None,
) -> dict[str, Any]:
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    subjects_mod = subjects_module or _load("subjects", testing_root / "subjects.py")
    executor_mod = executor_module or _load("subject_executor", testing_root / "subject_executor.py")

    try:
        manifest = subjects_mod.load_subject_manifest(repo_root, subject_id)
        matrix_id = _select_benchmark_matrix_id(manifest, mode=mode, host_platform=host_platform)
        execution_result = executor_mod.execute_subject_matrix(
            repo_root,
            subject_id,
            goal_id="perf.release",
            matrix_id=matrix_id,
            run_id=f"benchmark-{subject_id}-{mode}-{int(time.time())}",
        )
    except Exception as error:
        return {"error": str(error)}

    performance = _extract_runtime_performance(
        list(execution_result.get("stageResults") or []),
        preferred_kind=_preferred_runtime_stage_kind(mode),
    )
    if performance is None:
        errors = [str(item) for item in list(execution_result.get("errors") or []) if str(item)]
        error_text = "; ".join(errors) or f"{mode} benchmark did not produce runtime metrics for {subject_id}"
        return {"error": error_text}

    return {
        "metrics": dict(performance["metrics"]),
        "regressionFound": performance["regressionStatus"] == "regressed",
    }


def _run_native_benchmark_pipeline(
    *,
    repo_root: Path,
    subject_id: str,
    host_platform: str,
    subjects_module: Any | None = None,
    executor_module: Any | None = None,
) -> dict[str, Any]:
    return _run_subject_benchmark_pipeline(
        repo_root=repo_root,
        subject_id=subject_id,
        mode="native",
        host_platform=host_platform,
        subjects_module=subjects_module,
        executor_module=executor_module,
    )


def dispatch(args: list[str], repo_root: Path, host_platform: str) -> int:
    """Dispatch benchmark sub-commands.

    Returns exit code: 0 = success, 1 = regression, 2 = error.
    """
    testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
    records_mod = _load("benchmark_records", testing_root / "benchmark_records.py")
    detector_mod = _load("device_detector", testing_root / "device_detector.py")
    dash_mod = _load("benchmark_dashboard_generator", testing_root / "benchmark_dashboard_generator.py")

    # 鈹€鈹€ Parse args 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
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

    # 鈹€鈹€ status sub-command 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    if do_status:
        return _cmd_status(repo_root, records_mod, subject_id, do_all)

    # 鈹€鈹€ dashboard sub-command 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
    if do_dashboard and not do_record:
        output_path = Path(output) if output else repo_root / "docs" / "benchmark" / "dashboard.html"
        print(f"Generating benchmark dashboard 鈫?{output_path}")
        dash_mod.generate(repo_root, output_path)
        print("鉁?Dashboard generated")
        if do_open:
            import webbrowser
            webbrowser.open(output_path.as_uri())
        return 0

    # 鈹€鈹€ record sub-command 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
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
                print(f"  鈫?docs/benchmark/ updated")
            except Exception as e:
                print(f"  鈿?dashboard update failed: {e}")

        return 1 if regression_found else 0

    # 鈹€鈹€ Default: show help 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€
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

    All modes flow through the subject planner/executor pipeline so the same
    workload contract is consumed by managed, native and interpreter benchmarks.
    """
    import subprocess
    import datetime

    manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
    if not manifest_path.exists():
        return {"error": f"subject not found: {subject_id}"}

    if mode == "native":
        pipeline_result = _run_native_benchmark_pipeline(
            repo_root=repo_root,
            subject_id=subject_id,
            host_platform=host_platform,
        )
    else:
        pipeline_result = _run_subject_benchmark_pipeline(
            repo_root=repo_root,
            subject_id=subject_id,
            mode=mode,
            host_platform=host_platform,
        )
    if "error" in pipeline_result:
        return pipeline_result

    metrics = dict(pipeline_result.get("metrics") or {})
    regression_found = bool(pipeline_result.get("regressionFound"))
    if not metrics:
        return {"error": f"{mode} benchmark returned no metrics for {subject_id}"}

    try:
        git_out = subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"],
            text=True, cwd=repo_root, timeout=5,
        ).strip()
    except Exception:
        git_out = "unknown"

    plat_name = str(host_platform)
    record: dict[str, Any] = {
        "runId": f"{datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%d-%H%M%S')}-{subject_id}-{mode}",
        "subject": subject_id,
        "mode": mode,
        "platform": plat_name,
        "device": device,
        "recordedAt": datetime.datetime.now(datetime.timezone.utc).isoformat().replace("+00:00", "Z"),
        "gitCommit": git_out,
        "gitBranch": "main",
        "metrics": metrics,
    }
    records_mod.append_record(repo_root, record)

    return {"record": record, "regressionFound": regression_found}

def _print_result(subject_id: str, mode: str, device: dict[str, Any], result: dict[str, Any]) -> None:
    if "error" in result:
        print(f"  鉁?{subject_id} / {mode}: {result['error']}")
        return
    rec = result.get("record") or {}
    m = rec.get("metrics") or {}
    latency = m.get("meanDurationMs", m.get("elapsedMilliseconds", "?"))
    ops = m.get("opsPerSecond")
    dev_name = device.get("name", device.get("id", "?"))
    print(f"  鉁?{subject_id} / {mode} / {dev_name}")
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
  --subject <id>               Target subject (e.g. PerformanceFeaturePack)
  --all                        All Bench* subjects
  --mode <managed|native|interpreter>  Execution mode (default: all modes)
  --record                     Run benchmark and record result to records.jsonl
  --dashboard [--open]         Generate HTML dashboard (optionally open in browser)
  --output <path>              Dashboard output path
  status [--subject <id>]      Show latest benchmark records

Examples:
  run benchmark --subject PerformanceFeaturePack --mode native --record
  run benchmark --all --record
  run benchmark --dashboard --open
  run benchmark status --all
""")

