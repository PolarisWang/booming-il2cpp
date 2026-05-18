"""stress.py — CLI command for the Pressure Test Management System.

Sub-commands:
    run stress <test-name> [options]       Run a stress test
    run stress list                        List available tests
    run stress status [<test-name>]        Show latest results
    run stress history <test-name>         Show recent runs
    run stress compare <test-name> <id1> <id2>  Side-by-side comparison
    run stress export <test-name> --output <path>
    run stress import <path>
    run stress regressions                 List tests with recent regressions

Exit codes:
    0 = all passed
    1 = failures detected / regressions found
    2 = error (CLI usage, binary not found, etc.)
"""
from __future__ import annotations

import json
import shutil
import sys
import zipfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

try:
    from ..testing import stress_records as records_module
    from ..testing import stress_runners as runners_module
    from ..testing import stress_comparison as comparison_module
except ImportError:
    run_root = Path(__file__).resolve().parents[1]
    if str(run_root) not in sys.path:
        sys.path.insert(0, str(run_root))
    from testing import stress_records as records_module
    from testing import stress_runners as runners_module
    from testing import stress_comparison as comparison_module

AVAILABLE_TESTS = {
    "gc-stress": runners_module.run_gc_stress,
    "capacity-stress": lambda repo, **kw: [runners_module.run_capacity_stress(repo, **kw)],
    "threading-stress": lambda repo, **kw: [runners_module.run_threading_stress(repo, **kw)],
    "gc-stress-mode": lambda repo, **kw: [runners_module.run_gc_stress_mode(repo, **kw)],
    "delegate-stress": runners_module.run_delegate_stress,
}


# ---------------------------------------------------------------------------
# Help
# ---------------------------------------------------------------------------

def _print_help() -> None:
    print("Usage: run stress <sub-command> [options]")
    print()
    print("Sub-commands:")
    print("  run stress <test-name> [options]       Run a stress test")
    print("  run stress list                        List available stress tests")
    print("  run stress status [<test-name>]         Show latest results summary")
    print("  run stress history <test-name>          Show recent runs")
    print("  run stress compare <test-name> <id1> <id2>  Side-by-side comparison")
    print("  run stress export <test-name> --output <path>  Export results")
    print("  run stress import <path>                Import archived results")
    print("  run stress regressions                  List all tests with regressions")
    print()
    print("Test names:", ", ".join(sorted(AVAILABLE_TESTS)))
    print()
    print("Options for run stress <test-name>:")
    print("  --record            Store results in JSONL")
    print("  --json              Output results as JSON (AI-friendly)")
    print("  --quick / -q        Reduce scale for fast verification")
    print("  --build             Build the test binary before running")
    print("  --workers N         Override worker/thread count")
    print("  --duration N        Override duration in ms")
    print("  --output <path>     Write full output to file")


# ---------------------------------------------------------------------------
# Run
# ---------------------------------------------------------------------------

def _run_test(
    test_name: str,
    repo_root: Path,
    options: dict[str, Any],
) -> list[dict[str, Any]]:
    """Run a stress test and return list of result dicts (one per scenario)."""
    runner_fn = AVAILABLE_TESTS.get(test_name)
    if runner_fn is None:
        print(f"Unknown test: {test_name}")
        sys.exit(2)

    kw: dict[str, Any] = {}
    if test_name == "gc-stress":
        kw["build"] = options.get("build", False)
        kw["quick"] = options.get("quick", False)
        if options.get("workers"):
            kw["workers"] = int(options["workers"])
        if options.get("allocations"):
            kw["allocations"] = int(options["allocations"])
    elif test_name == "capacity-stress":
        kw["build"] = options.get("build", False)
        kw["quick"] = options.get("quick", False)
    elif test_name == "threading-stress":
        kw["build"] = options.get("build", False)
        if options.get("workers"):
            kw["workers"] = int(options["workers"])
        if options.get("duration"):
            kw["duration_ms"] = int(options["duration"])
    elif test_name == "gc-stress-mode":
        kw["build"] = options.get("build", False)
        if options.get("mode"):
            kw["mode"] = int(options["mode"])
    elif test_name == "delegate-stress":
        kw["build"] = options.get("build", False)
        kw["quick"] = options.get("quick", False)
        kw["scenario"] = options.get("scenario")

    print(f"Running {test_name}...")
    results = runner_fn(repo_root, **kw)

    # Convert to dicts
    result_dicts = [r.to_dict() if hasattr(r, "to_dict") else r for r in results]
    return result_dicts


def _handle_run(args: list[str], repo_root: Path) -> int:
    """Handle: run stress <test-name> [options]"""
    if not args:
        _print_help()
        return 2

    test_name = args[0]
    rest = args[1:]

    # Parse options
    options: dict[str, Any] = {}
    i = 0
    while i < len(rest):
        if rest[i] in ("--record",):
            options["record"] = True
        elif rest[i] in ("--json",):
            options["json"] = True
        elif rest[i] in ("--quick", "-q"):
            options["quick"] = True
        elif rest[i] in ("--build",):
            options["build"] = True
        elif rest[i] == "--workers" and i + 1 < len(rest):
            options["workers"] = rest[i + 1]
            i += 1
        elif rest[i] == "--duration" and i + 1 < len(rest):
            options["duration"] = rest[i + 1]
            i += 1
        elif rest[i] == "--output" and i + 1 < len(rest):
            options["output"] = rest[i + 1]
            i += 1
        elif rest[i] == "--mode" and i + 1 < len(rest):
            options["mode"] = rest[i + 1]
            i += 1
        elif rest[i] == "--allocations" and i + 1 < len(rest):
            options["allocations"] = rest[i + 1]
            i += 1
        elif rest[i] == "--scenario" and i + 1 < len(rest):
            options["scenario"] = rest[i + 1]
            i += 1
        i += 1

    # Run the test
    if test_name == "all":
        result_dicts = []
        for name in sorted(AVAILABLE_TESTS):
            r = _run_test(name, repo_root, options)
            result_dicts.extend(r)
    else:
        result_dicts = _run_test(test_name, repo_root, options)

    # Determine overall exit code
    any_failed = any(r.get("status") in ("failed", "error") for r in result_dicts)

    # Record results
    if options.get("record"):
        _record_results(repo_root, test_name if test_name != "all" else "all", result_dicts, options)

    # Write full output file if requested
    if options.get("output"):
        out_path = Path(options["output"])
        out_path.write_text(json.dumps(result_dicts, indent=2, default=str), encoding="utf-8")
        print(f"Output written to {out_path}")

    # JSON output (AI-friendly)
    if options.get("json"):
        output = {
            "testName": test_name,
            "results": result_dicts,
            "exitCode": 1 if any_failed else 0,
        }
        print(json.dumps(output, indent=2, default=str))
    else:
        # Human-readable summary
        for r in result_dicts:
            sn = f" [{r.get('scenarioName', '')}]" if r.get("scenarioName") else ""
            status_icon = "PASS" if r.get("status") == "passed" else "FAIL"
            print(f"  {r['test_name']}{sn}: {status_icon}")
            if r.get("errors"):
                for e in r["errors"]:
                    print(f"    error: {e}")

        summary = {"passed": 0, "failed": 0, "error": 0}
        for r in result_dicts:
            s = r.get("status", "")
            if s in summary:
                summary[s] += 1
        total = sum(summary.values())
        print(f"\nResults: {total} scenarios, {summary['passed']} passed, {summary['failed']} failed, {summary['error']} errors")

    return 1 if any_failed else 0


# ---------------------------------------------------------------------------
# Record results
# ---------------------------------------------------------------------------

def _record_results(
    repo_root: Path,
    test_name: str,
    result_dicts: list[dict[str, Any]],
    options: dict[str, Any],
) -> None:
    """Append results to JSONL and backup baselines."""
    git_commit = _get_git_commit(repo_root)
    git_branch = _get_git_branch(repo_root)
    git_branch = _get_git_branch(repo_root)
    timestamp = datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    run_id_base = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S")

    for i, r in enumerate(result_dicts):
        run_id = f"{run_id_base}-{test_name}-{r.get('scenario_name', str(i))}"
        record = {
            "schemaVersion": 1,
            "runId": run_id,
            "testName": r.get("test_name", test_name),
            "scenarioName": r.get("scenario_name"),
            "status": r.get("status", "unknown"),
            "recordedAt": timestamp,
            "gitCommit": git_commit,
            "gitBranch": git_branch,
            "platform": sys.platform,
            "config": r.get("config", {}),
            "metrics": r.get("metrics", {}),
            "output": r.get("output", {}),
            "errors": r.get("errors", []),
        }

        # Set quick flag in config if applicable
        if options.get("quick"):
            record["config"]["quick"] = True

        records_module.append_record(repo_root, record)

    # Backup baseline if this is a non-quick full run
    if not options.get("quick"):
        _backup_baseline(repo_root, test_name)

    print(f"  Recorded {len(result_dicts)} result(s) to stress-records/{test_name}/")


def _get_git_commit(repo_root: Path) -> str:
    try:
        import subprocess
        r = subprocess.run(["git", "rev-parse", "--short", "HEAD"], capture_output=True, text=True, cwd=repo_root, timeout=10)
        return r.stdout.strip() if r.returncode == 0 else "unknown"
    except Exception:
        return "unknown"


def _get_git_branch(repo_root: Path) -> str:
    try:
        import subprocess
        r = subprocess.run(["git", "rev-parse", "--abbrev-ref", "HEAD"], capture_output=True, text=True, cwd=repo_root, timeout=10)
        return r.stdout.strip() if r.returncode == 0 else "unknown"
    except Exception:
        return "unknown"


def _backup_baseline(repo_root: Path, test_name: str) -> None:
    """Backup existing baseline before a new full run."""
    history_root = repo_root / "verification-history" / "stress" / test_name
    latest_dir = history_root / "latest"
    baseline_file = latest_dir / "baseline.json"

    if not baseline_file.exists():
        return

    timestamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S")
    backup_dir = history_root / timestamp
    backup_dir.mkdir(parents=True, exist_ok=True)
    shutil.copy2(str(baseline_file), str(backup_dir / "baseline.json"))
    print(f"  Baseline backed up to verification-history/stress/{test_name}/{timestamp}/")


# ---------------------------------------------------------------------------
# List
# ---------------------------------------------------------------------------

def _handle_list(args: list[str], repo_root: Path) -> int:
    """List available and recorded stress tests."""
    print("Available stress tests:")
    for name in sorted(AVAILABLE_TESTS):
        print(f"  {name}")

    recorded = records_module.list_tests_with_records(repo_root)
    if recorded:
        print(f"\nTests with recorded history: {', '.join(recorded)}")
    return 0


# ---------------------------------------------------------------------------
# Status
# ---------------------------------------------------------------------------

def _handle_status(args: list[str], repo_root: Path) -> int:
    """Show latest result summary for one or all tests."""
    test_filter = args[0] if args else None

    print(f"{'Test Name':<25} {'Last Run':<22} {'Status':<10} Key Metric")
    print("-" * 80)

    for test_name in sorted(AVAILABLE_TESTS):
        if test_filter and test_name != test_filter:
            continue
        latest = records_module.query_latest(repo_root, test_name, scenario_name="__summary__")
        if latest is None:
            # Try without summary scenario
            latest = records_module.query_latest(repo_root, test_name)

        if latest is None:
            print(f"{test_name:<25} {'--':<22} {'--':<10} --")
            continue

        recorded_at = latest.get("recordedAt", "")[:19] if latest.get("recordedAt") else "--"
        status = latest.get("status", "unknown")
        metrics = latest.get("metrics", {})

        # Pick a key metric for display
        key = ""
        if "patternVerificationFailures" in metrics:
            key = f"pattern_fails={metrics['patternVerificationFailures']}"
        elif "passCount" in metrics:
            key = f"{metrics.get('passCount', 0)}/{metrics.get('passCount', 0) + metrics.get('failCount', 0)} passed"
        elif "failures" in metrics:
            key = f"{metrics.get('passed', 0)}/{metrics.get('totalTests', 0)} passed"
        elif "opsPerSecond" in metrics:
            key = f"{metrics['opsPerSecond']:.0f} ops/sec"
        else:
            key = str(metrics)[:40]

        print(f"{test_name:<25} {recorded_at:<22} {status:<10} {key}")

    return 0


# ---------------------------------------------------------------------------
# History
# ---------------------------------------------------------------------------

def _handle_history(args: list[str], repo_root: Path) -> int:
    """Show recent runs for a test."""
    if not args:
        print("Usage: run stress history <test-name> [--limit N] [--scenario <name>]")
        return 2

    test_name = args[0]
    rest = args[1:]

    limit = 10
    scenario = None
    i = 0
    while i < len(rest):
        if rest[i] == "--limit" and i + 1 < len(rest):
            limit = int(rest[i + 1])
            i += 1
        elif rest[i] == "--scenario" and i + 1 < len(rest):
            scenario = rest[i + 1]
            i += 1
        i += 1

    records = records_module.query_history(repo_root, test_name, scenario_name=scenario, limit=limit)
    if not records:
        print(f"No records found for '{test_name}'" + (f" scenario '{scenario}'" if scenario else ""))
        return 0

    print(f"History for '{test_name}'" + (f" (scenario: {scenario})" if scenario else "") + ":")
    print(f"{'Run ID':<35} {'Time':<20} {'Status':<10} {'Git Commit':<10} Key Metric")
    print("-" * 90)

    for r in records:
        run_id = r.get("runId", "?")[:34]
        ts = r.get("recordedAt", "")[:19] if r.get("recordedAt") else "?"
        status = r.get("status", "?")
        commit = r.get("gitCommit", "?")[:9]
        metrics = r.get("metrics", {})
        key = str(metrics)[:50]
        print(f"{run_id:<35} {ts:<20} {status:<10} {commit:<10} {key}")

    return 0


# ---------------------------------------------------------------------------
# Compare
# ---------------------------------------------------------------------------

def _handle_compare(args: list[str], repo_root: Path) -> int:
    """Side-by-side comparison of two runs."""
    if len(args) < 2:
        print("Usage: run stress compare <test-name> <run-id-1> [run-id-2]")
        print("  If run-id-2 is omitted, compares run-id-1 with the latest run.")
        return 2

    test_name = args[0]
    run_id_1 = args[1]
    run_id_2 = args[2] if len(args) > 2 else None

    r1 = records_module.query_by_run_id(repo_root, test_name, run_id_1)
    if r1 is None:
        print(f"Run '{run_id_1}' not found for '{test_name}'")
        return 2

    if run_id_2:
        r2 = records_module.query_by_run_id(repo_root, test_name, run_id_2)
    else:
        r2 = records_module.query_latest(repo_root, test_name)

    if r2 is None:
        print("No second run to compare against.")
        return 2

    print(f"Comparing '{test_name}':")
    print(f"  Run 1: {r1.get('runId', '?')}  ({r1.get('recordedAt', '?')})")
    print(f"  Run 2: {r2.get('runId', '?')}  ({r2.get('recordedAt', '?')})")
    print()

    # Compare metrics
    result = comparison_module.detect_regression(
        r2.get("metrics", {}),
        r1.get("metrics", {}),
    )

    m1 = r1.get("metrics", {})
    m2 = r2.get("metrics", {})
    all_metrics = set(m1.keys()) | set(m2.keys())

    if all_metrics:
        print(f"{'Metric':<30} {'Run 1':<15} {'Run 2':<15} {'Delta':<15}")
        print("-" * 75)
        for metric in sorted(all_metrics):
            v1 = m1.get(metric, "-")
            v2 = m2.get(metric, "-")
            v1s = f"{v1:.4f}" if isinstance(v1, float) else str(v1)
            v2s = f"{v2:.4f}" if isinstance(v2, float) else str(v2)
            delta = ""
            for reg in result["regressions"]:
                if reg["metric"] == metric:
                    delta = f"↓ {reg['delta']}"
            for imp in result["improvements"]:
                if imp["metric"] == metric:
                    delta = f"↑ {imp['delta']}"
            print(f"{metric:<30} {v1s:<15} {v2s:<15} {delta:<15}")

    print(f"\nOverall: {result['overall']}")
    if result["regressions"]:
        print("Regressions detected:")
        for r in result["regressions"]:
            print(f"  {r['metric']}: {r['before']} -> {r['after']} ({r['delta']})")

    return 1 if result["hasRegression"] else 0


# ---------------------------------------------------------------------------
# Export / Import
# ---------------------------------------------------------------------------

def _handle_export(args: list[str], repo_root: Path) -> int:
    """Export stress test results to a portable archive."""
    if len(args) < 1:
        print("Usage: run stress export <test-name> --output <path>")
        return 2

    test_name = args[0]
    rest = args[1:]

    output_path: str | None = None
    i = 0
    while i < len(rest):
        if rest[i] == "--output" and i + 1 < len(rest):
            output_path = rest[i + 1]
            i += 1
        i += 1

    if not output_path:
        print("--output <path> is required")
        return 2

    records = records_module.query_history(repo_root, test_name, limit=9999)
    if not records:
        print(f"No records found for '{test_name}'")
        return 0

    # Check baseline
    history_root = repo_root / "verification-history" / "stress" / test_name
    latest_baseline = history_root / "latest" / "baseline.json"
    baseline_data = None
    if latest_baseline.exists():
        baseline_data = json.loads(latest_baseline.read_text(encoding="utf-8"))

    archive = {
        "schemaVersion": 1,
        "exportedAt": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "testName": test_name,
        "gitCommit": _get_git_commit(repo_root),
        "recordCount": len(records),
        "records": records,
        "baseline": baseline_data,
    }

    if output_path.endswith(".zip"):
        with zipfile.ZipFile(output_path, "w", zipfile.ZIP_DEFLATED) as zf:
            zf.writestr(f"{test_name}-export.json", json.dumps(archive, indent=2, default=str))
    else:
        Path(output_path).write_text(json.dumps(archive, indent=2, default=str), encoding="utf-8")

    print(f"Exported {len(records)} records to {output_path}")
    return 0


def _handle_import(args: list[str], repo_root: Path) -> int:
    """Import previously exported stress test results."""
    if not args:
        print("Usage: run stress import <path>")
        return 2

    import_path = Path(args[0])
    if not import_path.exists():
        print(f"File not found: {import_path}")
        return 2

    # Load archive
    if import_path.suffix == ".zip":
        with zipfile.ZipFile(import_path, "r") as zf:
            names = zf.namelist()
            json_file = next((n for n in names if n.endswith(".json")), None)
            if json_file is None:
                print("No JSON found in zip archive")
                return 2
            archive = json.loads(zf.read(json_file))
    else:
        archive = json.loads(import_path.read_text(encoding="utf-8"))

    test_name = archive.get("testName", "imported")
    records = archive.get("records", [])

    if not records:
        print("No records in archive")
        return 0

    # Append records
    count = 0
    for record in records:
        try:
            records_module.append_record(repo_root, record)
            count += 1
        except Exception:
            pass

    # Restore baseline if present
    baseline = archive.get("baseline")
    if baseline:
        history_root = repo_root / "verification-history" / "stress" / test_name
        timestamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%S")
        import_dir = history_root / f"imported-{timestamp}"
        import_dir.mkdir(parents=True, exist_ok=True)
        (import_dir / "baseline.json").write_text(json.dumps(baseline, indent=2, default=str), encoding="utf-8")

    print(f"Imported {count} records to stress-records/{test_name}/")
    return 0


# ---------------------------------------------------------------------------
# Regressions
# ---------------------------------------------------------------------------

def _handle_regressions(args: list[str], repo_root: Path) -> int:
    """Scan all tests and report regressions."""
    tests = records_module.list_tests_with_records(repo_root)
    if not tests:
        print("No stress test records found.")
        return 0

    found_any = False
    for test_name in sorted(tests):
        scenario_names = records_module.list_scenarios(repo_root, test_name)
        for scenario in scenario_names:
            history = records_module.query_history(repo_root, test_name, scenario_name=scenario, limit=2)
            if len(history) < 2:
                continue

            current = history[0]
            baseline = history[1]

            result = comparison_module.detect_regression(
                current.get("metrics", {}),
                baseline.get("metrics", {}),
            )

            if result["hasRegression"]:
                if not found_any:
                    print("Regressions detected:")
                    found_any = True
                print(f"\n  {test_name} [{scenario}]:")
                for r in result["regressions"]:
                    print(f"    {r['metric']}: {r['before']} -> {r['after']} ({r['delta']})")

    if not found_any:
        print("No regressions detected.")

    return 1 if found_any else 0


# ---------------------------------------------------------------------------
# Main dispatch
# ---------------------------------------------------------------------------

def dispatch(args: list[str], repo_root: Path, host_platform: str) -> int:
    """Main entry point. Returns exit code."""
    if not args:
        _print_help()
        return 0

    sub_cmd = args[0]
    sub_args = args[1:]

    handlers = {
        "list": _handle_list,
        "status": _handle_status,
        "history": _handle_history,
        "compare": _handle_compare,
        "export": _handle_export,
        "import": _handle_import,
        "regressions": _handle_regressions,
    }

    if sub_cmd in handlers:
        return handlers[sub_cmd](sub_args, repo_root)

    # Otherwise, treat as a test name to run
    if sub_cmd in AVAILABLE_TESTS or sub_cmd == "all":
        return _handle_run(args, repo_root)

    print(f"Unknown sub-command or test name: {sub_cmd}")
    _print_help()
    return 2
