"""Run hotupdate_jit_fact on all families and collect results."""
import subprocess, sys, os, json, time

BASE = "D:/agent/booming-il2cpp"
ORCH = os.path.join(BASE, "build/toolchains/run/testing/foundation_dll/family_verification_orchestrator.py")
FAMILIES_DIR = os.path.join(BASE, "testing/foundation-dll/System.Private.CoreLib")
SKIP = ["preflight", "codegen", "jit_codegen", "fact", "fact_jit", "audit",
        "asm_compare", "microbench", "benchmark", "hotupdate",
        "hotupdate_aot_benchmark", "hotupdate_jit_benchmark"]

# Discover families (exclude non-family dirs)
exclude = {"reports"}
families = sorted(
    d for d in os.listdir(FAMILIES_DIR)
    if os.path.isdir(os.path.join(FAMILIES_DIR, d)) and d not in exclude
)

print(f"Found {len(families)} families.")
results = {}

for i, family in enumerate(families, 1):
    print(f"\n{'='*60}")
    print(f"[{i}/{len(families)}] {family}")
    print(f"{'='*60}")

    start = time.time()
    cmd = [sys.executable, ORCH, family, "--skip"] + SKIP
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=300)

    # Parse result
    hotupdate_jit_report_path = os.path.join(FAMILIES_DIR, family, "hotupdate-jit-verification-report.json")

    passed = None
    total = None
    if os.path.exists(hotupdate_jit_report_path):
        try:
            with open(hotupdate_jit_report_path) as f:
                rpt = json.load(f)
            passed = rpt.get("passed")
            total = rpt.get("total")
        except (json.JSONDecodeError, KeyError):
            pass

    if passed is None:
        print(f"  WARN: could not parse hotupdate_jit_fact result")
        print(f"  stdout last 5 lines: {proc.stdout.strip().splitlines()[-5:]}")
        print(f"  stderr last 5 lines: {proc.stderr.strip().splitlines()[-5:]}")
        results[family] = "UNKNOWN"
    else:
        status = "PASS" if passed == total else f"FAIL ({passed}/{total})"
        print(f"  {status}")
        results[family] = status

    elapsed = time.time() - start
    print(f"  elapsed: {elapsed:.1f}s")

# Summary
print(f"\n{'='*60}")
print("SUMMARY")
print(f"{'='*60}")
passed_all = 0
failed = 0
unknown = 0
for family, status in sorted(results.items()):
    print(f"  {family:50s} {status}")
    if status == "PASS":
        passed_all += 1
    elif status == "UNKNOWN":
        unknown += 1
    else:
        failed += 1

total = len(results)
print(f"\nPassed: {passed_all}/{total}, Failed: {failed}, Unknown: {unknown}")
sys.exit(failed)
