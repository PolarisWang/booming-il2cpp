"""Robust batch verification runner — saves results reliably, skips slow stages."""
from __future__ import annotations

import json, os, sys, time, traceback
from pathlib import Path

REPO = Path(r'D:/agent/booming-il2cpp')
os.chdir(str(REPO))

for p in [
    str(REPO / "testing" / "foundation-dll"),
    str(REPO / "testing" / "foundation-dll" / "verification"),
    str(REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll"),
    str(REPO / "build" / "toolchains" / "run"),
    str(REPO / "testing" / "project-test" / "_core" / "python"),
]:
    if p not in sys.path:
        sys.path.insert(0, p)

sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 1)
sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 1)

from verification.orchestration.context import FamilyContext
from verification.orchestration.engine import VerificationPipeline

SKIP_STAGES = {
    "benchmark", "hotupdate", "hotupdate_aot_benchmark",
    "hotupdate_jit_fact", "hotupdate_jit_benchmark",
    "asm_compare", "microbench",
}

ASSEMBLY = "System.Private.CoreLib"
TESTING_ROOT = REPO / "testing" / "foundation-dll"

# Discover families
families_dir = TESTING_ROOT / ASSEMBLY
slugs = sorted([
    d.name for d in families_dir.iterdir()
    if d.is_dir() and (
        (d / "capability-family-contract.json").exists() or
        (d / "contract.json").exists()
    )
])
# Filter skip slugs
SKIP_SLUGS = {
    "aggregation", "filtering", "immutable-array", "immutable-list", "reports",
    "interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core",
    "generic-supplement",  # takes 30+ min for jit_codegen cmake build
}
slugs = [s for s in slugs if s not in SKIP_SLUGS]

print(f"Discovered {len(slugs)} families")
print(f"Skipping stages: {SKIP_STAGES}")

OUTPUT = REPO / "testing" / "foundation-dll" / "results" / "robust-batch-results.json"
OUTPUT.parent.mkdir(parents=True, exist_ok=True)

results = []
total_start = time.perf_counter()

for i, slug in enumerate(slugs):
    family_dir = families_dir / slug
    ctx = FamilyContext(
        slug=slug,
        assembly=ASSEMBLY,
        family_dir=family_dir,
        skip_stages=SKIP_STAGES,
    )

    print(f"\n{'='*60}")
    print(f"[{i+1}/{len(slugs)}] {slug}")
    print(f"{'='*60}")

    start = time.perf_counter()
    try:
        pipeline = VerificationPipeline(ctx)
        report = pipeline.run()
        duration = time.perf_counter() - start

        # Also save per-family report
        perf_dir = TESTING_ROOT / "results" / "foundation-dll" / ASSEMBLY / slug / "perf"
        try:
            from verification.analysis.perf_store import auto_save_perf_data
            auto_save_perf_data(ctx, report.stages)
        except Exception:
            pass

        result = {
            "slug": slug,
            "status": report.overall_status,
            "duration_seconds": round(duration, 1),
            "stages": {
                name: {"status": s.get("status", "?"), "summary": s.get("summary", "")[:200]}
                for name, s in report.stages.items()
            },
        }
        passed_stages = sum(1 for s in report.stages.values() if s.get("status") == "passed")
        total_stages = len(report.stages)
        result["stage_summary"] = f"{passed_stages}/{total_stages} passed"
        print(f"  => {report.overall_status} ({duration:.0f}s, {passed_stages}/{total_stages})")

    except Exception as e:
        duration = time.perf_counter() - start
        traceback.print_exc()
        result = {
            "slug": slug,
            "status": "crashed",
            "duration_seconds": round(duration, 1),
            "stages": {},
            "error": str(e)[:300],
            "stage_summary": "CRASHED",
        }
        print(f"  => CRASHED ({duration:.0f}s): {e}")

    results.append(result)

    # Save progress
    summary = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "assembly": ASSEMBLY,
        "total": len(slugs),
        "completed": i + 1,
        "passed": sum(1 for r in results if r["status"] == "passed"),
        "failed": sum(1 for r in results if r["status"] in ("failed", "crashed")),
        "elapsed_seconds": round(time.perf_counter() - total_start, 1),
        "results": results,
    }
    OUTPUT.write_text(json.dumps(summary, indent=2, ensure_ascii=False), encoding="utf-8")

total = time.perf_counter() - total_start
passed = sum(1 for r in results if r["status"] == "passed")
failed = sum(1 for r in results if r["status"] in ("failed", "crashed"))
print(f"\n{'='*60}")
print(f"BATCH COMPLETE: {len(results)} families in {total:.0f}s")
print(f"  Passed: {passed}")
print(f"  Failed: {failed}")
print(f"  Report: {OUTPUT}")
print(f"{'='*60}")
