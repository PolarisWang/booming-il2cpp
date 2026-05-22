"""Serial verify-family for ALL families. Each family in its own Python process.
More robust than threading — no import conflicts, output is unbuffered.
"""
import subprocess, sys, time, json, os
from pathlib import Path

REPO = Path(__file__).resolve().parents[5]
TESTING_BASE = REPO / "testing" / "foundation-dll"
RESULTS_PATH = REPO / "batch_verify_family_results.json"

SKIP_FAMILIES = {  # assemblies to skip DLL-level directories
    "reports", "_contracts", "_core", "CMakeLists.txt"
}

SCRIPT = r"""
import sys, json, time
sys.path.insert(0, r'{run_dir}')
sys.path.insert(0, r'{run_dir2}')
from family_verification_orchestrator import verify_family

t0 = time.time()
try:
    result = verify_family('{slug}', assembly='{assembly}', mode='standard')
    report = result.get('unifiedReport', result)
    status = report.get('overall_status', 'error')
    stages = {{k: v.get('status', '?') for k, v in report.get('stages', {{}}).items()}}
    coverage = report.get('coverage', {{}})
except Exception as e:
    status = f'exception: {{e}}'
    stages = {{}}
    coverage = {{}}
elapsed = time.time() - t0
print(json.dumps({{'status': status, 'stages': stages, 'coverage': coverage, 'elapsed_s': round(elapsed, 1)}}))
"""

def get_all_families():
    families = []
    for asm_dir in sorted(TESTING_BASE.iterdir()):
        if not asm_dir.is_dir() or asm_dir.name in SKIP_FAMILIES:
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

def has_passed(assembly, slug):
    rp = TESTING_BASE / assembly / slug / "unified-verification-report.json"
    if not rp.exists():
        return False
    try:
        return json.loads(rp.read_text(encoding="utf-8")).get("overall_status") == "passed"
    except:
        return False

def main():
    all_families = get_all_families()
    todo = [(a, s) for a, s in all_families if not has_passed(a, s)]
    already = len(all_families) - len(todo)

    print(f"Total families: {len(all_families)}", flush=True)
    print(f"Already passed: {already}", flush=True)
    print(f"Need verification: {len(todo)}", flush=True)

    if not todo:
        print("All families already verified!", flush=True)
        return

    run_dir = str(REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll")
    run_dir2 = str(REPO / "build" / "toolchains" / "run")

    results = {}
    passed = failed = errors = 0
    total_elapsed = 0

    for i, (assembly, slug) in enumerate(todo, 1):
        start = time.time()
        print(f"\n[{i}/{len(todo)}] [{assembly}] {slug} ...", flush=True)

        code = SCRIPT.format(run_dir=run_dir, run_dir2=run_dir2, slug=slug, assembly=assembly)
        proc = subprocess.run(
            [sys.executable, "-c", code],
            capture_output=True, text=True, errors='replace', timeout=900,  # 15 min per family
        )

        elapsed = time.time() - start
        key = f"{assembly}/{slug}"

        if proc.returncode != 0:
            status = f"subprocess-error: {proc.stderr[-200:]}"
            results[key] = {"status": status, "elapsed_s": round(elapsed, 1)}
            errors += 1
            print(f"  ERROR: {status}", flush=True)
            continue

        # Find JSON result in stdout (last {...} line)
        result = None
        stdout = proc.stdout.strip()
        for line in reversed(stdout.splitlines()):
            line = line.strip()
            if line.startswith("{"):
                try:
                    parsed = json.loads(line)
                    if "status" in parsed and "elapsed_s" in parsed:
                        result = parsed
                        break
                except json.JSONDecodeError:
                    continue
        if result is None:
            result = {"status": f"json-not-found", "stdout_last": stdout[-200:], "elapsed_s": round(elapsed, 1)}
            errors += 1
            print(f"  JSON NOT FOUND — exit code {proc.returncode}, stderr: {proc.stderr[-200:]}", flush=True)
            results[key] = result
            continue

        results[key] = result
        total_elapsed += result.get("elapsed_s", elapsed)

        if result["status"] == "passed":
            passed += 1
            print(f"  PASS ({result.get('elapsed_s',0)}s)", flush=True)
        elif result["status"] == "failed":
            failed += 1
            fail_stages = [s for s, st in result.get("stages", {}).items() if st not in ("passed", "skipped")]
            print(f"  FAIL ({result.get('elapsed_s',0)}s) stages: {fail_stages}", flush=True)
        else:
            errors += 1
            print(f"  ERROR: {result['status']}", flush=True)

        # Save incremental results
        RESULTS_PATH.write_text(json.dumps({
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
            "total": len(todo),
            "passed": passed,
            "failed": failed,
            "errors": errors,
            "skipped_passed": already,
            "completed": i,
            "results": results,
        }, indent=2, ensure_ascii=False), encoding="utf-8")

    # Summary
    print(f"\n{'='*70}", flush=True)
    print(f"FINAL SUMMARY", flush=True)
    print(f"{'='*70}", flush=True)
    print(f"Total: {len(todo)}, Passed: {passed}, Failed: {failed}, Errors: {errors}", flush=True)
    print(f"Skipped (already passed): {already}", flush=True)
    print(f"Avg time: {total_elapsed/max(len(todo),1):.1f}s", flush=True)

    if failed:
        print(f"\n--- FAILED ---", flush=True)
        for k, r in sorted(results.items()):
            if r.get("status") == "failed":
                fs = [s for s, st in r.get("stages", {}).items() if st not in ("passed", "skipped")]
                print(f"  {k}: {fs}", flush=True)

if __name__ == "__main__":
    main()
