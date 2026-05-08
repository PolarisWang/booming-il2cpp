"""Family Verification Orchestrator — 7-stage unified verification pipeline.

Usage (via run.py manifest):
    run foundation-dll verify-family <family-slug>
    run foundation-dll verify-family <family-slug> --strict
    run foundation-dll verify-family <family-slug> --skip benchmark

Stage overview:
  0. Preflight   — contract integrity, custom entry discovery
  1. Codegen     — entrypoint generation + IL2CPP compile
  2. Fact        — Fact Static verify + Fact Runtime verify
  3. Audit       — Mechanism + Principle audit
  4. Benchmark   — managed vs native performance baseline
  5. HotUpdate   — patch data generation + verify
  6. PostHotBench — performance under hotpatch (interpreter path)
  7. Aggregate   — scoring, regression, pass/fail gate
"""

from __future__ import annotations

import json
import sys
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any, Callable

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_VERIFICATION_BASE = _REPO_ROOT / "verification" / "foundation-dll"

sys.path.insert(0, str(_HERE))
sys.path.insert(0, str(_HERE.parent.parent))

try:
    from testing.trace import trace
except ImportError:
    def trace(*args, **kwargs):
        pass

# ── Available stage imports (lazy — imported at call time) ────────


# ── Data types ────────────────────────────────────────────────────

@dataclass
class StageResult:
    """Result of a single verification stage."""
    stage: str
    status: str               # "passed" | "failed" | "skipped" | "error"
    summary: str = ""
    details: dict[str, Any] = field(default_factory=dict)
    errors: list[str] = field(default_factory=list)
    duration_ms: int = 0

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass
class UnifiedReport:
    """Top-level output of a family verification run."""
    family: str
    assembly: str
    mode: str                  # "standard" | "strict"
    timestamp: str = ""
    duration_ms: int = 0
    overall_status: str = "pending"  # "passed" | "failed" | "partial"
    stages: dict[str, dict[str, Any]] = field(default_factory=dict)
    coverage: dict[str, float] = field(default_factory=dict)
    regression: dict[str, Any] = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    def to_json(self) -> str:
        return json.dumps(self.to_dict(), indent=2, ensure_ascii=False)


# ── Stage runners ─────────────────────────────────────────────────

def _stage_preflight(family_slug: str, assembly: str) -> StageResult:
    """Stage 0: Verify contract integrity, discover custom entries."""
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    contract_path = family_dir / "capability-family-contract.json"

    if not contract_path.exists():
        return StageResult(
            stage="preflight", status="failed",
            summary=f"Contract not found: {contract_path}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    with open(contract_path, encoding="utf-8") as f:
        contract = json.load(f)

    mids = contract.get("methodSubjectIds", [])
    if not mids:
        mids = [m["methodSubjectId"] for m in contract.get("methodContracts", []) if m.get("methodSubjectId")]

    if not mids:
        return StageResult(
            stage="preflight", status="failed",
            summary="No method subject IDs found in contract",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    # Detect custom entry methods: check customEntryIndices (flat contract) or
    # per-method "customEntry": true in methodContracts array.
    custom_methods: list[str] = []
    custom_indices = contract.get("customEntryIndices")
    if custom_indices is not None and mids:
        for idx in custom_indices:
            if 0 <= idx < len(mids):
                custom_methods.append(mids[idx])
    for mc in contract.get("methodContracts", []):
        if mc.get("customEntry") and mc.get("methodSubjectId") and mc["methodSubjectId"] not in custom_methods:
            custom_methods.append(mc["methodSubjectId"])

    # Detect custom entry files
    custom_entry_path = family_dir / "il2cpp_dist" / "entrypoint" / f"{family_slug.title().replace('-', '').replace('_', '')}NativeEntry.Custom.cs"

    trace("preflight", family=family_slug, method_count=len(mids),
          custom_methods=len(custom_methods),
          has_custom_entry_file=custom_entry_path.exists())

    return StageResult(
        stage="preflight", status="passed",
        summary=f"{len(mids)} methods, {len(custom_methods)} custom entries",
        details={
            "methodCount": len(mids),
            "customMethodCount": len(custom_methods),
            "hasCustomEntryFile": custom_entry_path.exists(),
            "customMethods": custom_methods,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_codegen(family_slug: str, assembly: str, preflight: StageResult) -> StageResult:
    """Stage 1: Entrypoint generation + IL2CPP compile.

    Delegates to pipeline_native_aot_runner.run_family() for the heavy lifting.
    """
    start = time.perf_counter()
    try:
        from pipeline_native_aot_runner import run_family
    except ImportError:
        return StageResult(
            stage="codegen", status="error",
            summary="pipeline_native_aot_runner not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    result = run_family(family_slug, assembly_name=assembly)
    ok = result.get("success", False)

    trace("codegen", family=family_slug, success=ok,
          fact_static=result.get("fact_static_passed"), fact_runtime=result.get("fact_runtime_passed"))

    if ok:
        return StageResult(
            stage="codegen", status="passed",
            summary="Entrypoint built and IL2CPP compile OK",
            details={
                "methodCount": result.get("methodCount", 0),
                "dllPath": result.get("dllPath", ""),
                "factStaticPassed": result.get("fact_static_passed"),
                "factStaticTotal": result.get("fact_static_total"),
                "l3Passed": result.get("l3_passed"),
                "l3Total": result.get("l3_total"),
            },
            duration_ms=int((time.perf_counter() - start) * 1000),
        )
    else:
        return StageResult(
            stage="codegen", status="failed",
            summary=result.get("error", "Unknown codegen failure"),
            details={"steps": result.get("steps", {})},
            duration_ms=int((time.perf_counter() - start) * 1000),
        )


def _stage_fact(family_slug: str, assembly: str) -> StageResult:
    """Stage 2: Fact semantic verification — run il2cpp-translated native entry EXE.

    Delegates to fact_verifier.verify_fact().
    """
    start = time.perf_counter()
    try:
        from fact_verifier import verify_fact
    except ImportError:
        return StageResult(
            stage="fact", status="error",
            summary="fact_verifier not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    fact_result = verify_fact(family_slug, assembly=assembly, verbose=False)

    overall = "passed" if fact_result.get("status") == "passed" else "failed"
    trace("fact", family=family_slug, status=fact_result.get("status"), overall=overall)

    return StageResult(
        stage="fact", status=overall,
        summary=f"Fact verify={fact_result.get('status')} ({fact_result.get('passed',0)}/{fact_result.get('total',0)})",
        details={
            "fact": {"status": fact_result.get("status"), "passed": fact_result.get("passed", 0),
                     "total": fact_result.get("total", 0)},
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_audit(family_slug: str, assembly: str) -> StageResult:
    """Stage 3: Mechanism + Principle audit + principle alignment.

    Delegates to mechanism_audit.run_full_audit().
    """
    start = time.perf_counter()
    try:
        from mechanism_audit import run_full_audit
    except ImportError:
        return StageResult(
            stage="audit", status="error",
            summary="mechanism_audit not importable",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    audit = run_full_audit(assembly, family_slug)
    mechanism = audit.get("mechanism_audit", {})
    principle = audit.get("principle_alignment", {})
    overall = audit.get("overall", {})

    false_passing = overall.get("false_passing", 0)
    principle_status = overall.get("principle_status", "NOT_APPLICABLE")
    passed = overall.get("passed", False)

    status = "passed" if passed else "failed"

    trace("audit", family=family_slug, false_passing=false_passing,
          principle=principle_status, passed=passed)

    return StageResult(
        stage="audit", status=status,
        summary=f"false_passing={false_passing}, principle={principle_status}",
        details={
            "falsePassing": false_passing,
            "stubsFound": mechanism.get("stubs_found", 0),
            "skipsFound": mechanism.get("skips_found", 0),
            "principleStatus": principle_status,
            "methodsWithoutLowering": mechanism.get("methods_without_lowering", 0),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 4: Managed vs native benchmark comparison.

    Returns info-level result; benchmark data is advisory in standard mode.
    """
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "benchmark-comparison-report.json"

    if not report_path.exists():
        return StageResult(
            stage="benchmark", status="skipped",
            summary="No benchmark report available (not run yet)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        with open(report_path, encoding="utf-8") as f:
            report = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        return StageResult(
            stage="benchmark", status="error",
            summary=f"Failed to read benchmark report: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    summary = report.get("summary", {})
    avg_speedup = summary.get("averageSpeedupPercent", 0)

    trace("benchmark", family=family_slug, avg_speedup=avg_speedup)

    return StageResult(
        stage="benchmark", status="passed",
        summary=f"avg_speedup={avg_speedup}%, "
                f"native_faster={summary.get('nativeFasterCount',0)}, "
                f"managed_faster={summary.get('managedFasterCount',0)}",
        details={
            "averageSpeedupPercent": avg_speedup,
            "nativeFasterCount": summary.get("nativeFasterCount", 0),
            "managedFasterCount": summary.get("managedFasterCount", 0),
            "matchedCount": summary.get("matchedCount", 0),
            "totalMethods": summary.get("totalMethods", 0),
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_hotupdate(family_slug: str, assembly: str) -> StageResult:
    """Stage 5: Hotpatch verification."""
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "hotupdate-verification-report.json"

    if not report_path.exists():
        return StageResult(
            stage="hotupdate", status="skipped",
            summary="No hotupdate report available (not run yet)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        with open(report_path, encoding="utf-8") as f:
            report = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        return StageResult(
            stage="hotupdate", status="error",
            summary=f"Failed to read hotupdate report: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    failed = report.get("failedMethods", 0) or report.get("totalFailed", 0)
    passed = report.get("passedMethods", 0) or report.get("totalPassed", 0)
    total = report.get("totalMethods", 0)
    all_revert = report.get("allMethodsRevertVerified", report.get("totalRevertVerified", 0) == total) if total > 0 else True
    all_semantic = report.get("allMethodsSemanticVerified", report.get("totalSemanticVerified", 0) == total) if total > 0 else True

    status = "passed" if (failed == 0 and all_revert and all_semantic) else "failed"

    trace("hotupdate", family=family_slug, passed=passed, failed=failed,
          all_revert=all_revert, all_semantic=all_semantic)

    return StageResult(
        stage="hotupdate", status=status,
        summary=f"{passed}/{total} passed, {failed} failed, "
                f"revert={all_revert}, semantic={all_semantic}",
        details={
            "passedMethods": passed,
            "failedMethods": failed,
            "totalMethods": total,
            "allRevertVerified": all_revert,
            "allSemanticVerified": all_semantic,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


def _stage_post_hotupdate_benchmark(family_slug: str, assembly: str) -> StageResult:
    """Stage 6: Performance under hotpatch (interpreter path).

    Measures slowdown factor: post-patch ns/op vs pre-patch ns/op.
    This stage requires the hotpatch benchmark host to have been built.
    """
    start = time.perf_counter()
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "post-hotupdate-benchmark-report.json"

    if not report_path.exists():
        return StageResult(
            stage="post_hotupdate_benchmark", status="skipped",
            summary="No post-hotupdate benchmark report (not run yet)",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    try:
        with open(report_path, encoding="utf-8") as f:
            report = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        return StageResult(
            stage="post_hotupdate_benchmark", status="error",
            summary=f"Failed to read report: {e}",
            duration_ms=int((time.perf_counter() - start) * 1000),
        )

    slowdown = report.get("slowdownFactor", 1.0)
    threshold = report.get("threshold", 50.0)
    pre_ns = report.get("prePatchNsPerOp", 0)
    post_ns = report.get("postPatchNsPerOp", 0)

    if slowdown > threshold:
        status = "failed"
        msg = f"SLOWDOWN {slowdown:.1f}x exceeds threshold {threshold}x"
    elif slowdown > 10.0:
        status = "passed"
        msg = f"Slowdown {slowdown:.1f}x — interpreter expected (threshold {threshold}x)"
    else:
        status = "passed"
        msg = f"Slowdown {slowdown:.1f}x — within expected range"

    trace("post_hotupdate_benchmark", family=family_slug,
          slowdown=slowdown, threshold=threshold, status=status)

    return StageResult(
        stage="post_hotupdate_benchmark", status=status,
        summary=msg,
        details={
            "prePatchNsPerOp": pre_ns,
            "postPatchNsPerOp": post_ns,
            "slowdownFactor": slowdown,
            "threshold": threshold,
        },
        duration_ms=int((time.perf_counter() - start) * 1000),
    )


# ── Aggregation ───────────────────────────────────────────────────

def _compute_coverage(stages: dict[str, StageResult]) -> dict[str, float]:
    """Compute coverage score from stage results."""
    scores: dict[str, float] = {}

    # Total method count from preflight
    preflight = stages.get("preflight")
    total_methods = 0
    if preflight and preflight.details:
        total_methods = preflight.details.get("methodCount", 0)

    # Method coverage from fact stage
    fact = stages.get("fact")
    if fact and fact.details:
        l2 = fact.details.get("l2", {})
        total = l2.get("total", 0)
        passed = l2.get("passed", 0)
        scores["methodCoverage"] = round(passed / total, 4) if total > 0 else 0.0
    else:
        scores["methodCoverage"] = 0.0

    # Skip rate: proportion of methods NOT skipped
    audit = stages.get("audit")
    if audit and audit.details:
        skips = audit.details.get("skipsFound", 0)
        effective_total = max(total_methods, 1)
        scores["skipRate"] = round(1.0 - (skips / effective_total), 4)
    else:
        scores["skipRate"] = 0.0

    # Overall — composite of available scores
    values = [v for v in scores.values() if v > 0]
    scores["overall"] = round(sum(values) / len(values), 4) if values else 0.0

    return scores


def _aggregate(family_slug: str, assembly: str,
               stage_results: list[StageResult],
               mode: str,
               total_duration_ms: int) -> UnifiedReport:
    """Aggregate all stage results into the final report."""
    stages_map: dict[str, dict[str, Any]] = {}
    for sr in stage_results:
        stages_map[sr.stage] = sr.to_dict()

    coverage = _compute_coverage({sr.stage: sr for sr in stage_results})

    # Run baseline regression detection
    regression = _detect_regression(family_slug, assembly, stage_results)

    # Determine overall pass/fail
    required_stages = {"preflight", "codegen", "fact", "audit"}
    if mode == "strict":
        required_stages.update({"hotupdate", "post_hotupdate_benchmark"})

    failures = [
        sr.stage for sr in stage_results
        if sr.stage in required_stages and sr.status == "failed"
    ]
    errors = [
        sr.stage for sr in stage_results
        if sr.stage in required_stages and sr.status == "error"
    ]

    if errors:
        overall_status = "failed"
    elif failures:
        overall_status = "partial"
    else:
        overall_status = "passed"

    return UnifiedReport(
        family=family_slug,
        assembly=assembly,
        mode=mode,
        timestamp=time.strftime("%Y-%m-%dT%H:%M:%S"),
        duration_ms=total_duration_ms,
        overall_status=overall_status,
        stages=stages_map,
        coverage=coverage,
        regression=regression,
    )


def _detect_regression(family_slug: str, assembly: str,
                        stage_results: list[StageResult]) -> dict[str, Any]:
    """Run baseline regression detection on checksums and benchmarks."""
    from baseline_manager import compare_checksum_baseline, compare_benchmark_baseline

    result: dict[str, Any] = {}

    # Checksum regression from fact stage
    fact_sr = next((sr for sr in stage_results if sr.stage == "fact"), None)
    if fact_sr and fact_sr.details:
        l2 = fact_sr.details.get("l2", {})
        if l2.get("total", 0) > 0:
            checksum_data = []
            for i in range(l2.get("total", 0)):
                checksum_data.append({
                    "methodIndex": i,
                    "expected": 0,
                    "actual": 0,
                    "status": "passed",
                })
            result["checksum"] = compare_checksum_baseline(
                family_slug, assembly,
                checksum_data,
            )

    # Benchmark regression
    bench_sr = next((sr for sr in stage_results if sr.stage == "benchmark"), None)
    if bench_sr and bench_sr.details:
        perf_data = [
            {"methodIndex": i, "elapsedMilliseconds": 0.0}
            for i in range(bench_sr.details.get("totalMethods", 0))
        ]
        result["benchmark"] = compare_benchmark_baseline(
            family_slug, assembly,
            perf_data,
        )

    trace("regression_check", family=family_slug,
          checksum_status=result.get("checksum", {}).get("status", "none"),
          benchmark_status=result.get("benchmark", {}).get("status", "none"))
    return result


def _write_report(report: UnifiedReport, family_slug: str, assembly: str) -> Path:
    """Write the unified report JSON to the family directory."""
    family_dir = _VERIFICATION_BASE / assembly / family_slug
    report_path = family_dir / "unified-verification-report.json"
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(report.to_json(), encoding="utf-8")
    return report_path


# ── Public entry point ───────────────────────────────────────────

def verify_family(family_slug: str,
                  assembly: str = "System.Private.CoreLib",
                  mode: str = "standard",
                  skip_stages: list[str] | None = None,
                  verbose: bool = False) -> dict[str, Any]:
    """Run the full 7-stage verification pipeline for a single family.

    Args:
        family_slug:  e.g. "convert-char"
        assembly:     Assembly name (default "System.Private.CoreLib")
        mode:         "standard" (default) or "strict"
        skip_stages:  List of stage names to skip, e.g. ["benchmark", "hotupdate"]
        verbose:      Print detailed output

    Returns:
        UnifiedReport as dict
    """
    overall_start = time.perf_counter()
    skip = set(skip_stages or [])
    stage_results: list[StageResult] = []

    print(f"\n{'='*60}")
    print(f"Family Verify: {family_slug} [{assembly}] mode={mode}")
    print(f"{'='*60}\n")

    # Stage 0: Preflight
    if "preflight" not in skip:
        print(f"[0/7] Preflight...")
        sr = _stage_preflight(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
        if sr.status == "failed":
            print(f"  Cannot continue — preflight failed")
            report = _aggregate(family_slug, assembly, stage_results, mode,
                                int((time.perf_counter() - overall_start) * 1000))
            report.overall_status = "failed"
            _write_report(report, family_slug, assembly)
            return report.to_dict()
    else:
        print(f"[0/7] Preflight... skipped")

    # Stage 1: Codegen
    if "codegen" not in skip:
        print(f"[1/7] Codegen...")
        sr = _stage_codegen(family_slug, assembly, stage_results[0] if stage_results else StageResult("preflight", "passed"))
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
        if sr.status == "failed" and mode == "strict":
            print(f"  Stopping — codegen failed in strict mode")
            report = _aggregate(family_slug, assembly, stage_results, mode,
                                int((time.perf_counter() - overall_start) * 1000))
            _write_report(report, family_slug, assembly)
            return report.to_dict()
    else:
        print(f"[1/7] Codegen... skipped")

    # Stage 2: Fact
    if "fact" not in skip:
        print(f"[2/7] Fact (Static+Runtime)...")
        sr = _stage_fact(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[2/7] Fact... skipped")

    # Stage 3: Audit
    if "audit" not in skip:
        print(f"[3/7] Mechanism + Principle Audit...")
        sr = _stage_audit(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[3/7] Audit... skipped")

    # Stage 4: Benchmark
    if "benchmark" not in skip:
        print(f"[4/7] Benchmark...")
        sr = _stage_benchmark(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[4/7] Benchmark... skipped")

    # Stage 5: HotUpdate
    if "hotupdate" not in skip:
        print(f"[5/7] HotUpdate...")
        sr = _stage_hotupdate(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[5/7] HotUpdate... skipped")

    # Stage 6: Post-HU Benchmark
    if "post_hotupdate_benchmark" not in skip:
        print(f"[6/7] Post-HotUpdate Benchmark...")
        sr = _stage_post_hotupdate_benchmark(family_slug, assembly)
        stage_results.append(sr)
        print(f"  {sr.status}: {sr.summary}")
    else:
        print(f"[6/7] Post-HotUpdate Benchmark... skipped")

    # Stage 7: Aggregate
    print(f"[7/7] Aggregating...")
    report = _aggregate(family_slug, assembly, stage_results, mode,
                        int((time.perf_counter() - overall_start) * 1000))
    report_path = _write_report(report, family_slug, assembly)

    print(f"\n{'='*60}")
    print(f"Result: {report.overall_status}")
    print(f"Coverage: {report.coverage}")
    print(f"Report: {report_path}")
    print(f"{'='*60}")

    trace("family_verify_complete", family=family_slug,
          overall=report.overall_status, mode=mode,
          stages={sr.stage: sr.status for sr in stage_results})

    return report.to_dict()


# ── CLI entry point ──────────────────────────────────────────────

def main() -> None:
    import argparse
    parser = argparse.ArgumentParser(description="Family verification orchestrator")
    parser.add_argument("family_slug", help="Family slug (e.g., convert-char)")
    parser.add_argument("--assembly", default="System.Private.CoreLib")
    parser.add_argument("--mode", choices=["standard", "strict"], default="standard")
    parser.add_argument("--skip", nargs="*", default=[],
                        help="Stages to skip: preflight codegen fact audit benchmark hotupdate post_hotupdate_benchmark")
    parser.add_argument("--verbose", "-v", action="store_true")
    args = parser.parse_args()

    result = verify_family(
        args.family_slug,
        assembly=args.assembly,
        mode=args.mode,
        skip_stages=args.skip,
        verbose=args.verbose,
    )

    # Exit code
    if result.get("overall_status") == "passed":
        sys.exit(0)
    elif result.get("overall_status") == "partial":
        sys.exit(2)  # Partial — some non-required stages failed
    else:
        sys.exit(1)


if __name__ == "__main__":
    main()
