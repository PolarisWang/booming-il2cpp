"""stress_runners.py — Adapter runners for each stress test type.

Each runner locates the test binary, executes it, parses output, and returns
a unified StressRunResult dict.
"""
from __future__ import annotations

import json
import os
import re
import subprocess
import sys
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any


# ---------------------------------------------------------------------------
# StressRunResult — unified return type for all runners
# ---------------------------------------------------------------------------

@dataclass
class StressRunResult:
    status: str                     # "passed" | "failed" | "error"
    test_name: str                  # e.g. "gc-stress"
    scenario_name: str | None = None
    metrics: dict[str, Any] = field(default_factory=dict)
    output: dict[str, Any] = field(default_factory=lambda: {"stdout": "", "stderr": "", "reportPath": ""})
    errors: list[str] = field(default_factory=list)
    config: dict[str, Any] = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    def passed(self) -> bool:
        return self.status == "passed"


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _find_binary(repo_root: Path, binary_name: str, search_dirs: list[Path] | None = None) -> Path | None:
    """Locate a test binary under known artifact directories."""
    candidates: list[Path] = search_dirs or [
        repo_root / "artifacts" / "native-runtime-core-test",
        repo_root / "artifacts" / "presets" / "debug" / "bin",
        repo_root / "artifacts" / "presets" / "release" / "bin",
    ]
    for d in candidates:
        # Check both Release and Debug subdirs
        for sub in (d, d / "Release", d / "Debug", d / "RelWithDebInfo"):
            exe = sub / f"{binary_name}.exe"
            if exe.exists():
                return exe
    return None


def _run(cmd: list[str], timeout: int = 300, env: dict[str, str] | None = None) -> subprocess.CompletedProcess:
    """Run a command and return the result."""
    return subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, env=env,
                          encoding="utf-8", errors="replace")


# ---------------------------------------------------------------------------
# GC Stress runner
# ---------------------------------------------------------------------------

def run_gc_stress(
    repo_root: Path,
    *,
    build: bool = False,
    quick: bool = False,
    workers: int | None = None,
    allocations: int | None = None,
) -> list[StressRunResult]:
    """Run the chaos_gc_stress_test binary.

    Returns a list of per-scenario StressRunResult (one per scenario A-K),
    plus an aggregate result as the last entry.
    """
    binary = _find_binary(repo_root, "chaos_gc_stress_test")
    if binary is None:
        return [StressRunResult(
            status="error",
            test_name="gc-stress",
            errors=["chaos_gc_stress_test binary not found. Build with --build flag first."],
        )]

    if build:
        # Build the target first
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", "chaos_gc_stress_test", "--config", "Debug"])
        if b.returncode != 0:
            return [StressRunResult(
                status="error",
                test_name="gc-stress",
                errors=[f"Build failed: {b.stderr[:500]}"],
                output={"stdout": b.stdout[:1000], "stderr": b.stderr[:1000]},
            )]

    # Quick mode: pass CHAOS_IL2CPP_STRESS_SCALE for reduced workload
    env = None
    if quick:
        env = {**os.environ, "CHAOS_IL2CPP_STRESS_SCALE": "50"}

    result = _run([str(binary)], timeout=600, env=env)
    if result.returncode != 0 and result.returncode != 1:
        return [StressRunResult(
            status="error",
            test_name="gc-stress",
            errors=[f"Binary crashed (RC={result.returncode})"],
            output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        )]

    # Try to parse the structured JSON report
    report_dir = repo_root / "artifacts" / "native-runtime-core-test" / "reports"
    report_files = sorted(report_dir.glob("gc_stress_report_*.json")) if report_dir.exists() else []
    report_path = report_files[-1] if report_files else None

    if report_path:
        try:
            report = json.loads(report_path.read_text(encoding="utf-8"))
            scenarios = report.get("scenarios", [])
            results: list[StressRunResult] = []
            for s in scenarios:
                config = {"workers": s.get("workers"), "allocations_per_worker": s.get("allocations_per_worker")}
                if quick:
                    config["quick"] = True
                results.append(StressRunResult(
                    status="passed" if s.get("passed", False) else "failed",
                    test_name="gc-stress",
                    scenario_name=s.get("name", "unknown"),
                    metrics={
                        "totalAllocations": s.get("total_allocations", 0),
                        "totalBytesEstimate": s.get("total_bytes_estimate", 0),
                        "patternVerificationFailures": s.get("pattern_verification_failures", 0),
                        "youngCollections": s.get("gc_stats", {}).get("young_collections", 0),
                        "youngPromotedBytes": s.get("gc_stats", {}).get("young_promoted_bytes", 0),
                        "youngReclaimedBytes": s.get("gc_stats", {}).get("young_reclaimed_bytes", 0),
                        "youngAvgPauseNs": s.get("gc_stats", {}).get("young_avg_pause_ns", 0),
                        "fullCollections": s.get("gc_stats", {}).get("full_collections", 0),
                        "fullReclaimedBytes": s.get("gc_stats", {}).get("full_reclaimed_bytes", 0),
                        "fullAvgPauseNs": s.get("gc_stats", {}).get("full_avg_pause_ns", 0),
                        "activeRegionsAfter": s.get("active_regions_after", 0),
                    },
                    output={"reportPath": str(report_path)},
                    config=config,
                ))

            # Aggregate summary
            total = len(scenarios)
            passed = sum(1 for s in scenarios if s.get("passed", False))
            results.append(StressRunResult(
                status="passed" if passed == total else "failed",
                test_name="gc-stress",
                scenario_name="__summary__",
                metrics={"totalScenarios": total, "passed": passed, "failed": total - passed},
                config={"quick": quick},
            ))
            return results
        except (json.JSONDecodeError, KeyError) as e:
            pass  # fall through to console parsing

    # Fallback: parse console output
    return [StressRunResult(
        status="passed" if result.returncode == 0 else "failed",
        test_name="gc-stress",
        scenario_name="console",
        output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        config={"quick": quick},
    )]


# ---------------------------------------------------------------------------
# Capacity Stress runner
# ---------------------------------------------------------------------------

def run_capacity_stress(
    repo_root: Path,
    *,
    build: bool = False,
    quick: bool = False,
) -> StressRunResult:
    """Run the chaos_capacity_stress_test binary."""
    binary = _find_binary(repo_root, "chaos_capacity_stress_test")
    if binary is None:
        return StressRunResult(
            status="error",
            test_name="capacity-stress",
            errors=["chaos_capacity_stress_test binary not found. Build with --build flag first."],
        )

    if build:
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", "chaos_capacity_stress_test", "--config", "Debug"])
        if b.returncode != 0:
            return StressRunResult(
                status="error", test_name="capacity-stress",
                errors=[f"Build failed: {b.stderr[:500]}"],
                output={"stdout": b.stdout[:1000], "stderr": b.stderr[:1000]},
            )

    result = _run([str(binary)], timeout=300)

    # Parse console output for test results
    # Expected format: "Results: N tests, M failures"
    summary_match = re.search(r"Results:\s*(\d+)\s*tests,\s*(\d+)\s*failures", result.stdout)
    total_tests = int(summary_match.group(1)) if summary_match else 0
    failures = int(summary_match.group(2)) if summary_match else -1

    status = "passed"
    errors: list[str] = []
    if result.returncode != 0:
        status = "error"
        errors.append(f"Binary crashed (RC={result.returncode})")
    elif failures > 0:
        status = "failed"
        errors.append(f"{failures} test(s) failed")

    return StressRunResult(
        status=status,
        test_name="capacity-stress",
        metrics={
            "totalTests": total_tests,
            "failures": failures,
            "passed": total_tests - max(failures, 0),
        },
        output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        errors=errors,
        config={"quick": quick},
    )


# ---------------------------------------------------------------------------
# Threading Stress runner
# ---------------------------------------------------------------------------

def run_threading_stress(
    repo_root: Path,
    *,
    build: bool = False,
    workers: int = 16,
    duration_ms: int = 30000,
) -> StressRunResult:
    """Run threading stress via entry.exe --stress."""
    # Locate entry.exe for the threading-monitor-interlocked family
    entry_candidates = [
        repo_root / "verification" / "foundation-dll" / "System.Private.CoreLib"
        / "threading-monitor-interlocked" / "native" / "entry.exe",
    ]
    # Also check common build output locations
    for p in (repo_root / "artifacts").rglob("entry.exe"):
        entry_candidates.append(p)
    for p in (repo_root / "verification").rglob("entry.exe"):
        entry_candidates.append(p)

    entry_exe: Path | None = None
    for p in entry_candidates:
        if p.exists():
            entry_exe = p
            break

    if entry_exe is None:
        return StressRunResult(
            status="error",
            test_name="threading-stress",
            errors=["entry.exe not found. Build a foundation-dll family first."],
        )

    if build:
        b = _run([sys.executable, str(repo_root / "build" / "toolchains" / "run" / "testing" / "foundation_dll"
                  / "family_entrypoint_generator.py"),
                  "--family", "threading-monitor-interlocked", "--build"])
        if b.returncode != 0:
            return StressRunResult(
                status="error", test_name="threading-stress",
                errors=[f"Build failed: {b.stderr[:500]}"],
                output={"stdout": b.stdout[:1000], "stderr": b.stderr[:1000]},
            )

    result = _run([str(entry_exe), "--stress", str(workers), str(duration_ms)], timeout=duration_ms + 30000)

    # Parse output for ops/sec and passed/failed counts
    ops_match = re.search(r"([\d.]+)\s*ops/sec", result.stdout, re.IGNORECASE)
    ops_per_sec = float(ops_match.group(1)) if ops_match else 0.0

    passed_match = re.search(r"Passed:\s*(\d+)/(\d+)", result.stdout)
    passed_tests = int(passed_match.group(1)) if passed_match else 0
    total_tests = int(passed_match.group(2)) if passed_match else 0

    status = "passed"
    errors: list[str] = []
    if result.returncode != 0:
        status = "error"
        errors.append(f"entry.exe crashed (RC={result.returncode})")

    return StressRunResult(
        status=status,
        test_name="threading-stress",
        metrics={
            "workers": workers,
            "durationMs": duration_ms,
            "passed": passed_tests,
            "failures": total_tests - passed_tests,
            "totalTests": total_tests,
            "opsPerSecond": ops_per_sec,
        },
        output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        errors=errors,
        config={"workers": workers, "durationMs": duration_ms},
    )


# ---------------------------------------------------------------------------
# GC Stress Mode runner (deferred — currently a no-op with instructions)
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# GC Stress Mode runner (env CHAOS_GC_STRESS_MODE)
# ---------------------------------------------------------------------------

def run_gc_stress_mode(
    repo_root: Path,
    *,
    mode: int = 1,
    build: bool = False,
    subject_binary: str = "chaos_gc_sanity_test",
) -> StressRunResult:
    """Run GC stress mode by executing a test subject with CHAOS_GC_STRESS_MODE set.

    GC stress modes (defined in gc_stress.cpp):
      mode=1 — GcStressCheckAlloc: full GC after every allocation (re-entrancy test)
      mode=2 — GcStressCheckVerify: heap verification after every safepoint
      mode=3 — both modes combined

    The subject binary should exercise GC allocation paths (e.g., chaos_gc_sanity_test).
    Under GC stress mode, each allocation triggers a full GC — this stresses the
    collector's re-entrancy safety and heap integrity.
    """
    binary = _find_binary(repo_root, subject_binary)
    if binary is None:
        return StressRunResult(
            status="error",
            test_name="gc-stress-mode",
            errors=[
                f"'{subject_binary}' binary not found. Build with --build flag first.",
                "Suggested: cmake --build artifacts/presets/debug --target chaos_gc_sanity_test --config Debug",
            ],
        )

    if build:
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", subject_binary, "--config", "Debug"])
        if b.returncode != 0:
            return StressRunResult(
                status="error", test_name="gc-stress-mode",
                errors=[f"Build failed: {b.stderr[:500]}"],
                output={"stdout": b.stdout[:1000], "stderr": b.stderr[:1000]},
            )

    # Determine which stress mode to activate.
    mode_names = {1: "alloc-stress (full-GC after every alloc)",
                  2: "verify-stress (heap verify after every safepoint)",
                  3: "combined-stress (both modes)"}
    mode_name = mode_names.get(mode, f"unknown-mode-{mode}")

    env = {**os.environ, "CHAOS_GC_STRESS_MODE": str(mode)}

    result = _run([str(binary)], timeout=120, env=env)

    status = "passed"
    errors: list[str] = []
    metrics: dict[str, Any] = {"mode": mode}

    if result.returncode != 0:
        if result.returncode == 1:
            # Test failures reported by the test harness itself.
            status = "failed"
            errors.append(f"Test subject reported failures (RC=1)")
        elif result.returncode == 3:
            # CHAOS_IL2CPP_ASSERT triggered via _assert_abort
            status = "failed"
            errors.append("Assertion failure detected under GC stress mode")
        elif result.returncode < 0 or result.returncode > 3:
            # Likely a crash (access violation, etc.)
            status = "error"
            errors.append(f"Test subject crashed under GC stress mode (RC={result.returncode})")
        else:
            status = "failed"
            errors.append(f"Test subject exited with RC={result.returncode}")

    # Parse any "PASS"/"FAIL" patterns in stdout to count results.
    pass_count = len(re.findall(r'\bPASS\b', result.stdout))
    fail_count = len(re.findall(r'\bFAIL\b', result.stdout))
    if pass_count > 0 or fail_count > 0:
        metrics["passCount"] = pass_count
        metrics["failCount"] = fail_count
        metrics["passed"] = pass_count
        metrics["failures"] = fail_count
        metrics["totalTests"] = pass_count + fail_count

    # Check for assertion failure output.
    if "assert" in result.stdout.lower() or "assert" in result.stderr.lower():
        if status == "passed":
            status = "failed"
        errors.append("Assertion message detected in output")
        metrics["assertionDetected"] = True

    return StressRunResult(
        status=status,
        test_name="gc-stress-mode",
        scenario_name=mode_name,
        metrics=metrics,
        output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        errors=errors,
        config={"mode": mode, "subjectBinary": subject_binary},
    )


# ---------------------------------------------------------------------------
# Delegate Stress runner
# ---------------------------------------------------------------------------

def run_delegate_stress(
    repo_root: Path,
    *,
    build: bool = False,
    quick: bool = False,
    scenario: str | None = None,
) -> list[StressRunResult]:
    """Run the chaos_delegate_stress_test binary.

    Returns a list of per-scenario StressRunResult (one per scenario),
    plus an aggregate result as the last entry.
    """
    binary = _find_binary(repo_root, "chaos_delegate_stress_test")
    if binary is None:
        return [StressRunResult(
            status="error",
            test_name="delegate-stress",
            errors=["chaos_delegate_stress_test binary not found. Build with --build flag first."],
        )]

    if build:
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", "chaos_delegate_stress_test", "--config", "Debug"])
        if b.returncode != 0:
            return [StressRunResult(
                status="error",
                test_name="delegate-stress",
                errors=[f"Build failed: {b.stderr[:500]}"],
                output={"stdout": b.stdout[:1000], "stderr": b.stderr[:1000]},
            )]

    # Assemble the command
    cmd = [str(binary)]
    if scenario:
        cmd.append(scenario)

    env = None
    if quick:
        env = {**os.environ, "CHAOS_IL2CPP_STRESS_SCALE": "50"}

    result = _run(cmd, timeout=600, env=env)
    if result.returncode != 0 and result.returncode != 1:
        return [StressRunResult(
            status="error",
            test_name="delegate-stress",
            errors=[f"Binary crashed (RC={result.returncode})"],
            output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        )]

    # Try to parse the structured JSON report
    report_dir = repo_root / "artifacts" / "native-runtime-core-test" / "reports"
    report_files = sorted(report_dir.glob("delegate_stress_report_*.json")) if report_dir.exists() else []
    report_path = report_files[-1] if report_files else None

    if report_path:
        try:
            report = json.loads(report_path.read_text(encoding="utf-8"))
            scenarios = report.get("scenarios", [])
            results: list[StressRunResult] = []
            for s in scenarios:
                config: dict[str, Any] = {}
                if quick:
                    config["quick"] = True
                if scenario:
                    config["scenario"] = scenario
                results.append(StressRunResult(
                    status="passed" if s.get("passed", False) else "failed",
                    test_name="delegate-stress",
                    scenario_name=s.get("name", "unknown"),
                    metrics={
                        "allocCount": s.get("alloc_count", 0),
                        "bytesAllocated": s.get("bytes_allocated", 0),
                        "patternVerificationFailures": s.get("pattern_verification_failures", 0),
                        "combineCount": s.get("combine_count", 0),
                        "removeCount": s.get("remove_count", 0),
                        "invokeCount": s.get("invoke_count", 0),
                        "youngCollections": s.get("gc_stats", {}).get("young_collections", 0),
                        "youngAvgPauseNs": s.get("gc_stats", {}).get("young_avg_pause_ns", 0),
                        "fullCollections": s.get("gc_stats", {}).get("full_collections", 0),
                        "fullAvgPauseNs": s.get("gc_stats", {}).get("full_avg_pause_ns", 0),
                        "activeRegionsAfter": s.get("active_regions_after", 0),
                    },
                    output={"reportPath": str(report_path)},
                    config=config,
                ))

            # Aggregate summary
            total = len(scenarios)
            passed = sum(1 for s in scenarios if s.get("passed", False))
            results.append(StressRunResult(
                status="passed" if passed == total else "failed",
                test_name="delegate-stress",
                scenario_name="__summary__",
                metrics={"totalScenarios": total, "passed": passed, "failed": total - passed},
                config={"quick": quick},
            ))
            return results
        except (json.JSONDecodeError, KeyError) as e:
            pass  # fall through to console parsing

    # Fallback: parse console output
    return [StressRunResult(
        status="passed" if result.returncode == 0 else "failed",
        test_name="delegate-stress",
        scenario_name="console",
        output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        config={"quick": quick, "scenario": scenario},
    )]


# ---------------------------------------------------------------------------
# Delegate Stress GC Mode runner
# ---------------------------------------------------------------------------

def run_delegate_stress_gc_mode(
    repo_root: Path,
    *,
    mode: int = 1,
    build: bool = False,
    quick: bool = False,
    scenario: str | None = None,
) -> StressRunResult:
    """Run delegate stress with CHAOS_GC_STRESS_MODE set.

    GC stress modes:
      mode=1 — full GC after every allocation
      mode=2 — heap verification after every safepoint
      mode=3 — both modes combined
    """
    binary = _find_binary(repo_root, "chaos_delegate_stress_test")
    if binary is None:
        return StressRunResult(
            status="error",
            test_name="delegate-stress-gc-mode",
            errors=["chaos_delegate_stress_test binary not found."],
        )

    if build:
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", "chaos_delegate_stress_test", "--config", "Debug"])
        if b.returncode != 0:
            return StressRunResult(
                status="error", test_name="delegate-stress-gc-mode",
                errors=[f"Build failed: {b.stderr[:500]}"],
            )

    mode_names = {1: "alloc-stress", 2: "verify-stress", 3: "combined-stress"}
    mode_name = mode_names.get(mode, f"mode-{mode}")

    cmd = [str(binary)]
    if scenario:
        cmd.append(scenario)

    env = {**os.environ, "CHAOS_GC_STRESS_MODE": str(mode)}
    if quick:
        env["CHAOS_IL2CPP_STRESS_SCALE"] = "50"

    result = _run(cmd, timeout=300, env=env)

    status = "passed"
    errors: list[str] = []
    metrics: dict[str, Any] = {"mode": mode}

    if result.returncode != 0:
        if result.returncode == 1:
            status = "failed"
            errors.append("Delegate stress reported failures under GC stress mode")
        elif result.returncode == 3:
            status = "failed"
            errors.append("Assertion failure detected under GC stress mode")
        else:
            status = "error"
            errors.append(f"Crashed under GC stress mode (RC={result.returncode})")

    pass_count = len(re.findall(r'\bPASS\b', result.stdout))
    fail_count = len(re.findall(r'\bFAIL\b', result.stdout))
    if pass_count > 0 or fail_count > 0:
        metrics["passCount"] = pass_count
        metrics["failCount"] = fail_count

    if "assert" in result.stdout.lower() or "assert" in result.stderr.lower():
        if status == "passed":
            status = "failed"
        errors.append("Assertion message detected in output")

    return StressRunResult(
        status=status,
        test_name="delegate-stress-gc-mode",
        scenario_name=mode_name,
        metrics=metrics,
        output={"stdout": result.stdout[:2000], "stderr": result.stderr[:2000]},
        errors=errors,
        config={"mode": mode, "quick": quick, "scenario": scenario},
    )


# ---------------------------------------------------------------------------
# LOH Stress runner
# ---------------------------------------------------------------------------

def run_loh_stress(
    repo_root: Path,
    *,
    build: bool = False,
    quick: bool = False,
    scenario: str | None = None,
    **kw: Any,
) -> list[StressRunResult]:
    """Run the LOH (Large Object Heap) stress test."""
    binary = repo_root / "artifacts" / "native-runtime-core-test" / "Debug" / "chaos_loh_stress_test.exe"
    if not binary.exists():
        return [StressRunResult(status="error", test_name="loh-stress",
                                errors=["Binary not found; build first"])]

    if build:
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", "chaos_loh_stress_test", "--config", "Debug"])
        if b.returncode != 0:
            return [StressRunResult(status="error", test_name="loh-stress",
                                    errors=[f"Build failed: {b.stderr[:500]}"])]

    env = {**os.environ}
    if quick:
        env["CHAOS_IL2CPP_STRESS_SCALE"] = "10"

    cmd = [str(binary)]
    if scenario:
        cmd.append(scenario)
    else:
        cmd.append("all")

    result = _run(cmd, timeout=300, env=env)

    status = "passed"
    errors: list[str] = []
    metrics: dict[str, Any] = {}

    if result.returncode != 0:
        status = "failed"
        errors.append(f"Exit code {result.returncode}")

    # Parse "Results: N scenarios, M passed, F failed"
    m = re.search(r'Results:\s+(\d+)\s+scenarios,\s+(\d+)\s+passed,\s+(\d+)\s+failed', result.stdout)
    if m:
        metrics["scenarios"] = int(m.group(1))
        metrics["passed"] = int(m.group(2))
        metrics["failed"] = int(m.group(3))
        if metrics["failed"] > 0:
            status = "failed"

    return [StressRunResult(
        status=status,
        test_name="loh-stress",
        scenario_name=scenario or "__all__",
        metrics=metrics,
        output={"stdout": result.stdout[:3000], "stderr": result.stderr[:2000]},
        errors=errors,
    )]


# ---------------------------------------------------------------------------
# Finalizer Stress runner
# ---------------------------------------------------------------------------

def run_finalizer_stress(
    repo_root: Path,
    *,
    build: bool = False,
    quick: bool = False,
    scenario: str | None = None,
    **kw: Any,
) -> list[StressRunResult]:
    """Run the GC finalizer stress test."""
    binary = repo_root / "artifacts" / "native-runtime-core-test" / "Debug" / "chaos_gc_finalizer_stress_test.exe"
    if not binary.exists():
        return [StressRunResult(status="error", test_name="finalizer-stress",
                                errors=["Binary not found; build first"])]

    if build:
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", "chaos_gc_finalizer_stress_test", "--config", "Debug"])
        if b.returncode != 0:
            return [StressRunResult(status="error", test_name="finalizer-stress",
                                    errors=[f"Build failed: {b.stderr[:500]}"])]

    env = {**os.environ}
    cmd = [str(binary)]
    if scenario:
        cmd.append(scenario)
    else:
        cmd.append("all")

    result = _run(cmd, timeout=120, env=env)

    status = "passed"
    errors: list[str] = []
    metrics: dict[str, Any] = {}

    if result.returncode != 0:
        status = "failed"
        errors.append(f"Exit code {result.returncode}")

    m = re.search(r'Results:\s+(\d+)\s+scenarios,\s+(\d+)\s+passed,\s+(\d+)\s+failed', result.stdout)
    if m:
        metrics["scenarios"] = int(m.group(1))
        metrics["passed"] = int(m.group(2))
        metrics["failed"] = int(m.group(3))
        if metrics["failed"] > 0:
            status = "failed"

    return [StressRunResult(
        status=status,
        test_name="finalizer-stress",
        scenario_name=scenario or "__all__",
        metrics=metrics,
        output={"stdout": result.stdout[:3000], "stderr": result.stderr[:2000]},
        errors=errors,
    )]


# ---------------------------------------------------------------------------
# BGC Stress runner
# ---------------------------------------------------------------------------

def run_bgc_stress(
    repo_root: Path,
    *,
    build: bool = False,
    quick: bool = False,
    scenario: str | None = None,
    **kw: Any,
) -> list[StressRunResult]:
    """Run the BGC (background GC) stress test."""
    binary = repo_root / "artifacts" / "native-runtime-core-test" / "Debug" / "chaos_gc_bgc_stress_test.exe"
    if not binary.exists():
        return [StressRunResult(status="error", test_name="bgc-stress",
                                errors=["Binary not found; build first"])]

    if build:
        b = _run(["cmake", "--build", str(repo_root / "artifacts" / "presets" / "debug"),
                   "--target", "chaos_gc_bgc_stress_test", "--config", "Debug"])
        if b.returncode != 0:
            return [StressRunResult(status="error", test_name="bgc-stress",
                                    errors=[f"Build failed: {b.stderr[:500]}"])]

    env = {**os.environ}
    cmd = [str(binary)]

    result = _run(cmd, timeout=300, env=env)

    status = "passed"
    errors: list[str] = []
    metrics: dict[str, Any] = {}

    if result.returncode != 0:
        status = "failed"
        errors.append(f"Exit code {result.returncode}")

    return [StressRunResult(
        status=status,
        test_name="bgc-stress",
        metrics=metrics,
        output={"stdout": result.stdout[:3000], "stderr": result.stderr[:2000]},
        errors=errors,
    )]
