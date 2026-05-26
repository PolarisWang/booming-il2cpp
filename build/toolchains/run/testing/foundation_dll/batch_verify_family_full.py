"""Batch verify-family for ALL families across ALL foundation DLLs.
Skips families that already have a passing unified verification report.
"""
import sys, time, json, os
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

REPO = Path(__file__).resolve().parents[5]
sys.path.insert(0, str(REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll"))
sys.path.insert(0, str(REPO / "build" / "toolchains" / "run"))

TESTING_BASE = REPO / "testing" / "foundation-dll"
MAX_WORKERS = 3  # mild parallelism — cmake builds are I/O-heavy

def get_all_families():
    """Return list of (assembly_name, family_slug) for all families with entry.exe."""
    families = []
    for asm_dir in sorted(TESTING_BASE.iterdir()):
        if not asm_dir.is_dir() or asm_dir.name.startswith("_"):
            continue
        assembly = asm_dir.name
        for fam_dir in sorted(asm_dir.iterdir()):
            if not fam_dir.is_dir():
                continue
            slug = fam_dir.name
            entry_exe = fam_dir / "native" / "entry.exe"
            if entry_exe.exists():
                families.append((assembly, slug))
    return families

def has_passed_report(assembly, slug):
    """Check if unified report exists with passed status."""
    report_path = TESTING_BASE / assembly / slug / "unified-verification-report.json"
    if not report_path.exists():
        return False
    try:
        data = json.loads(report_path.read_text(encoding="utf-8"))
        return data.get("overall_status") == "passed"
    except:
        return False

def verify_one(assembly, slug):
    """Run verify_family for one family. Returns (assembly, slug, result_dict)."""
    start = time.time()
    try:
        from family_verification_orchestrator import verify_family
        result = verify_family(slug, assembly=assembly, mode="standard")
        report = result.get("unifiedReport", result)
        status = report.get("overall_status", "error")
        stages = {k: v.get("status", "?") for k, v in report.get("stages", {}).items()}
        coverage = report.get("coverage", {})
    except Exception as e:
        status = f"exception: {e}"
        stages = {}
        coverage = {}
    elapsed = time.time() - start
    return (assembly, slug, {"status": status, "stages": stages, "coverage": coverage, "elapsed_s": round(elapsed, 1)})

def main():
    all_families = get_all_families()
    print(f"Total families with entry.exe: {len(all_families)}")

    # Filter to ones that need verification
    todo = [(a, s) for a, s in all_families if not has_passed_report(a, s)]
    already_passed = len(all_families) - len(todo)
    print(f"Already passed: {already_passed}")
    print(f"Need verification: {len(todo)}")
    if already_passed > 0:
        print(f"  (Skipping: see unified-verification-report.json for passed reports)")

    if not todo:
        print("All families already verified!")
        return

    results = {}
    total_elapsed = 0
    passed_count = 0
    failed_count = 0
    error_count = 0

    # Use thread pool for parallelism
    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        fut_map = {}
        for assembly, slug in todo:
            print(f"\n--- Queuing: [{assembly}] {slug} ---")
            fut = executor.submit(verify_one, assembly, slug)
            fut_map[fut] = (assembly, slug)

        done_count = 0
        for fut in as_completed(fut_map):
            assembly, slug = fut_map[fut]
            assembly, slug, result = fut.result()
            done_count += 1
            total_elapsed += result["elapsed_s"]
            key = f"{assembly}/{slug}"
            results[key] = result

            if result["status"] == "passed":
                passed_count += 1
                marker = "PASS"
            elif result["status"] == "failed":
                failed_count += 1
                marker = "FAIL"
            else:
                error_count += 1
                marker = "ERROR"

            fail_stages = [s for s, st in result.get("stages", {}).items() if st not in ("passed", "skipped")]
            fail_info = f"  failed stages: {fail_stages}" if fail_stages else ""
            print(f"  [{done_count}/{len(todo)}] {marker} [{assembly}] {slug} ({result['elapsed_s']}s){fail_info}")

    # Summary
    print(f"\n{'='*70}")
    print(f"BATCH VERIFY SUMMARY")
    print(f"{'='*70}")
    print(f"Total: {len(todo)} families")
    print(f"Passed: {passed_count}")
    print(f"Failed: {failed_count}")
    print(f"Errors: {error_count}")
    print(f"Already passed (skipped): {already_passed}")
    print(f"Total elapsed (sum): {total_elapsed:.0f}s ({total_elapsed/60:.1f}min)")
    print(f"Average per family: {total_elapsed/max(len(todo),1):.1f}s")

    if failed_count > 0:
        print(f"\n--- FAILED ---")
        for key, r in sorted(results.items()):
            if r["status"] == "failed":
                fail_stages = [s for s, st in r.get("stages", {}).items() if st not in ("passed", "skipped")]
                print(f"  {key}: {fail_stages}")

    if error_count > 0:
        print(f"\n--- ERRORS ---")
        for key, r in sorted(results.items()):
            if r["status"] != "passed" and r["status"] != "failed":
                print(f"  {key}: {r['status']}")

    # Save results
    out_path = REPO / "batch_verify_family_results.json"
    out_path.write_text(json.dumps({
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "total": len(todo),
        "passed": passed_count,
        "failed": failed_count,
        "errors": error_count,
        "skipped_passed": already_passed,
        "results": results,
    }, indent=2, ensure_ascii=False), encoding="utf-8")
    print(f"\nResults saved to {out_path}")

if __name__ == "__main__":
    main()
