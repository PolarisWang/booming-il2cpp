"""Parallel verify-family for ALL families across ALL foundation DLLs.
Each family runs in its own subprocess. Max 2 concurrent.
Saves incremental progress after each family completes.
"""
import subprocess, sys, time, json, os
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

REPO = Path(__file__).resolve().parents[5]
TESTING_BASE = REPO / "testing" / "foundation-dll"
RESULTS_PATH = REPO / "batch_verify_family_results.json"
MAX_WORKERS = 2

SKIP_DIRS = {"reports", "_contracts", "_core", "CMakeLists.txt"}

def get_all_families():
    families = []
    for asm_dir in sorted(TESTING_BASE.iterdir()):
        if not asm_dir.is_dir() or asm_dir.name in SKIP_DIRS:
            continue
        assembly = asm_dir.name
        for fam_dir in sorted(asm_dir.iterdir()):
            if not fam_dir.is_dir():
                continue
            slug = fam_dir.name
            if (fam_dir / "native" / "entry.exe").exists():
                families.append((assembly, slug))
    return families

def has_passed_report(assembly, slug):
    rp = TESTING_BASE / assembly / slug / "unified-verification-report.json"
    if not rp.exists():
        return False
    try:
        return json.loads(rp.read_text(encoding="utf-8")).get("overall_status") == "passed"
    except:
        return False

def verify_one(args):
    """Run verify_family for one family in a subprocess. Returns result dict."""
    assembly, slug = args
    start = time.time()
    run_dir = str(REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll")
    run_dir2 = str(REPO / "build" / "toolchains" / "run")

    code = (
        "import sys, json, time\n"
        f"sys.path.insert(0, {json.dumps(run_dir)})\n"
        f"sys.path.insert(0, {json.dumps(run_dir2)})\n"
        "from family_verification_orchestrator import verify_family\n"
        "t0 = time.time()\n"
        "try:\n"
        f"    result = verify_family({json.dumps(slug)}, assembly={json.dumps(assembly)}, mode='standard')\n"
        "    report = result.get('unifiedReport', result)\n"
        "    status = report.get('overall_status', 'error')\n"
        "    stages = {k: v.get('status', '?') for k, v in report.get('stages', {}).items()}\n"
        "    coverage = report.get('coverage', {})\n"
        "except Exception as e:\n"
        "    status = f'exception: {e}'\n"
        "    stages = {}\n"
        "    coverage = {}\n"
        "elapsed = time.time() - t0\n"
        "print('__RESULT__' + json.dumps({'status': status, 'stages': stages, 'coverage': coverage, 'elapsed_s': round(elapsed, 1)}))\n"
    )

    try:
        proc = subprocess.run(
            [sys.executable, "-c", code],
            capture_output=True, text=True, errors='replace',
            timeout=900,  # 15 min
        )
        elapsed = time.time() - start

        # Find __RESULT__ marker in stdout
        for line in proc.stdout.splitlines():
            line = line.strip()
            if line.startswith("__RESULT__"):
                try:
                    result = json.loads(line[len("__RESULT__"):])
                    result["elapsed_s"] = round(elapsed, 1)
                    return (assembly, slug, result)
                except json.JSONDecodeError:
                    pass

        # No marker found
        return (assembly, slug, {
            "status": f"no-marker: rc={proc.returncode}",
            "stdout": proc.stdout[-200:],
            "stderr": proc.stderr[-200:],
            "elapsed_s": round(elapsed, 1),
        })
    except subprocess.TimeoutExpired:
        return (assembly, slug, {"status": "timeout", "elapsed_s": round(time.time() - start, 1)})
    except Exception as e:
        return (assembly, slug, {"status": f"exception: {e}", "elapsed_s": round(time.time() - start, 1)})

def save_results(passed, failed, errors, total, already, results):
    RESULTS_PATH.write_text(json.dumps({
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "total": total,
        "passed": passed,
        "failed": failed,
        "errors": errors,
        "skipped_passed": already,
        "completed": passed + failed + errors,
        "results": results,
    }, indent=2, ensure_ascii=False), encoding="utf-8")

def main():
    all_families = get_all_families()
    todo = [(a, s) for a, s in all_families if not has_passed_report(a, s)]
    already = len(all_families) - len(todo)

    print(f"Total families: {len(all_families)}", flush=True)
    print(f"Already passed: {already}", flush=True)
    print(f"Need verification: {len(todo)}", flush=True)
    print(f"Workers: {MAX_WORKERS}", flush=True)

    if not todo:
        print("All families already verified!", flush=True)
        return

    passed = failed = errors = 0
    results = {}

    # Save initial state
    save_results(passed, failed, errors, len(todo), already, results)

    with ProcessPoolExecutor(max_workers=MAX_WORKERS) as executor:
        fut_map = {executor.submit(verify_one, (a, s)): (a, s) for a, s in todo}

        for fut in as_completed(fut_map):
            assembly, slug, result = fut.result()
            key = f"{assembly}/{slug}"
            results[key] = result

            if result["status"] == "passed":
                passed += 1
                print(f"  PASS [{assembly}/{slug}] ({result.get('elapsed_s',0)}s)", flush=True)
            elif result["status"] == "failed":
                failed += 1
                fs = [s for s, st in result.get("stages", {}).items() if st not in ("passed", "skipped")]
                print(f"  FAIL [{assembly}/{slug}] ({result.get('elapsed_s',0)}s) stages: {fs}", flush=True)
                # Print full subprocess output for debugging
                if "stdout" in result:
                    print(f"    stdout: {result['stdout']}", flush=True)
                if "stderr" in result:
                    print(f"    stderr: {result['stderr']}", flush=True)
            else:
                errors += 1
                print(f"  ERROR [{assembly}/{slug}]: {result['status']}", flush=True)
                if "stdout" in result:
                    print(f"    stdout: {result['stdout']}", flush=True)
                if "stderr" in result:
                    print(f"    stderr: {result['stderr']}", flush=True)

            # Save incremental result
            save_results(passed, failed, errors, len(todo), already, results)

    # Final summary
    total_time = sum(r.get("elapsed_s", 0) for r in results.values())
    print(f"\n{'='*60}", flush=True)
    print(f"FINAL: {total}/{len(all_families)} | P:{passed} F:{failed} E:{errors} | Skipped:{already}", flush=True)
    print(f"Total process time: {total_time:.0f}s ({total_time/60:.1f}min)", flush=True)

    if failed:
        print(f"\n--- FAILED ---", flush=True)
        for k, r in sorted(results.items()):
            if r.get("status") == "failed":
                fs = [s for s, st in r.get("stages", {}).items() if st not in ("passed", "skipped")]
                print(f"  {k}: {fs}", flush=True)

if __name__ == "__main__":
    main()
