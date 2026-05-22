"""Parallel batch verify-family using subprocess.Popen for concurrent execution.
Runs 2 families concurrently. Each family in its own process.
"""
import subprocess, sys, time, json, os, shutil
from pathlib import Path

REPO = Path(__file__).resolve().parents[5]
TESTING_BASE = REPO / "testing" / "foundation-dll"
RESULTS_PATH = REPO / "batch_verify_family_results.json"
TMP_DIR = REPO / "batch_results_tmp"
WORKER = REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "verify_one_family.py"
MAX_CONCURRENT = 2
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

def has_passed(assembly, slug):
    rp = TESTING_BASE / assembly / slug / "unified-verification-report.json"
    if not rp.exists():
        return False
    try:
        return json.loads(rp.read_text(encoding="utf-8")).get("overall_status") == "passed"
    except:
        return False

def read_result(assembly, slug):
    """Read result from temp file, return dict or None."""
    rp = TMP_DIR / f"{assembly}_{slug}.json"
    if rp.exists():
        try:
            return json.loads(rp.read_text(encoding="utf-8"))
        except:
            return None
    return None

def main():
    # Clean temp dir
    if TMP_DIR.exists():
        shutil.rmtree(TMP_DIR)
    TMP_DIR.mkdir(parents=True)

    all_families = get_all_families()
    todo = [(a, s) for a, s in all_families if not has_passed(a, s)]
    already = len(all_families) - len(todo)

    print(f"Total families: {len(all_families)}", flush=True)
    print(f"Already passed: {already}", flush=True)
    print(f"Need verification: {len(todo)}", flush=True)

    if not todo:
        print("All families already verified!", flush=True)
        return

    results = {}
    passed = failed = errors = 0
    active = []  # list of (assembly, slug, process, start_time)
    remaining = list(todo)
    total = len(remaining)
    done = 0

    def save():
        RESULTS_PATH.write_text(json.dumps({
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
            "total": total,
            "passed": passed, "failed": failed, "errors": errors,
            "skipped_passed": already, "completed": done,
            "results": results,
        }, indent=2, ensure_ascii=False), encoding="utf-8")

    save()

    while remaining or active:
        # Launch new workers if we have capacity
        while len(active) < MAX_CONCURRENT and remaining:
            a, s = remaining.pop(0)
            start = time.time()
            proc = subprocess.Popen(
                [sys.executable, str(WORKER), a, s],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                text=True, errors='replace',
            )
            active.append((a, s, proc, start))
            print(f"  START [{a}/{s}] ({len(remaining)} remaining)", flush=True)

        # Check active processes
        new_active = []
        for a, s, proc, start in active:
            ret = proc.poll()
            if ret is not None:
                done += 1
                elapsed = time.time() - start
                result = read_result(a, s)
                if result is None:
                    stdout, stderr = proc.communicate()
                    result = {
                        "status": f"no-result-file: {stdout[-200:]}; {stderr[-200:]}",
                        "elapsed_s": round(elapsed, 1),
                    }

                key = f"{a}/{s}"
                results[key] = result
                result["elapsed_s"] = round(elapsed, 1)

                if result.get("status") == "passed":
                    passed += 1
                    print(f"  [{done}/{total}] PASS [{a}/{s}] ({elapsed:.1f}s)", flush=True)
                elif result.get("status") == "failed":
                    failed += 1
                    fs = [s for s, st in result.get("stages", {}).items() if st not in ("passed", "skipped")]
                    print(f"  [{done}/{total}] FAIL [{a}/{s}] ({elapsed:.1f}s) stages: {fs}", flush=True)
                else:
                    errors += 1
                    print(f"  [{done}/{total}] ERROR [{a}/{s}]: {result.get('status','?')}", flush=True)

                save()
            else:
                new_active.append((a, s, proc, start))

        active = new_active

        if remaining or active:
            time.sleep(5)  # poll every 5 seconds

    # Cleanup temp
    shutil.rmtree(TMP_DIR, ignore_errors=True)

    # Summary
    print(f"\n{'='*60}", flush=True)
    print(f"FINAL: P:{passed} F:{failed} E:{errors} | Skipped:{already}", flush=True)
    if failed:
        print(f"\n--- FAILED ---", flush=True)
        for k, r in sorted(results.items()):
            if r.get("status") == "failed":
                fs = [s for s, st in r.get("stages", {}).items() if st not in ("passed", "skipped")]
                print(f"  {k}: {fs}", flush=True)
    if errors:
        print(f"\n--- ERRORS ---", flush=True)
        for k, r in sorted(results.items()):
            if r.get("status") not in ("passed", "failed"):
                print(f"  {k}: {r.get('status','?')}", flush=True)

if __name__ == "__main__":
    main()
