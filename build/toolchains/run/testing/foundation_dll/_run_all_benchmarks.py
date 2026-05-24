"""Run native benchmarks for all CoreLib families and save JSON results."""
from __future__ import annotations

import json
import subprocess
import sys
import time
from pathlib import Path

REPO_ROOT = Path("D:/agent/booming-il2cpp")
FAMILIES_DIR = REPO_ROOT / "testing" / "foundation-dll" / "System.Private.CoreLib"
RUNNER = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "native_benchmark_runner.py"
REPORT_DIR = REPO_ROOT / "testing" / "foundation-dll" / "System.Private.CoreLib" / "reports"
REPORT_DIR.mkdir(parents=True, exist_ok=True)

families = sorted(
    d.name for d in FAMILIES_DIR.iterdir()
    if d.is_dir() and (d / "native_test" / "fact-verify" / "build" / "native-aot.generated.obj").exists()
)

print(f"Running benchmarks for {len(families)} families...\n")

results = {}
all_passed = 0
all_failed = 0
total_time = 0.0

for i, family in enumerate(families, 1):
    print(f"[{i}/{len(families)}] {family}...")
    t0 = time.time()

    r = subprocess.run(
        ["python", str(RUNNER), family, "--iterations", "100", "--no-trace"],
        capture_output=True, text=True, timeout=600,
    )

    elapsed = time.time() - t0
    total_time += elapsed

    # Always try to parse JSON — runner exits 1 on test failures but still outputs
    output = r.stdout
    json_start = output.find("{")
    if json_start >= 0:
        try:
            data = json.loads(output[json_start:])
            results[family] = data
            passed = sum(1 for m in data.get("results", []) if "elapsedMilliseconds" in m)
            failed = sum(1 for m in data.get("results", []) if "error" in m)
            total = len(data.get("results", []))
            all_passed += passed
            all_failed += failed
            status = "OK" if failed == 0 else f"{failed} FAIL"
            print(f"  {passed}/{total} passed, {failed} failed ({status}, {elapsed:.1f}s)")
        except json.JSONDecodeError:
            print(f"  JSON parse FAILED ({elapsed:.1f}s)")
            debug_file = REPORT_DIR / f"{family}_raw_output.txt"
            debug_file.write_text(r.stdout[:5000], encoding="utf-8")
            results[family] = {"error": "JSON parse failed", "stderr": r.stderr[:200]}
            all_failed += 1
    else:
        print(f"  No JSON found (rc={r.returncode}, {elapsed:.1f}s)")
        results[family] = {"error": f"No JSON output, rc={r.returncode}", "stderr": r.stderr[:200]}
        all_failed += 1

# Save comprehensive report
report = {
    "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
    "total_families": len(families),
    "summary": {"passed": all_passed, "failed": all_failed},
    "duration_seconds": round(total_time, 1),
    "families": results,
}

report_path = REPORT_DIR / "benchmark_report.json"
report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
print(f"\nReport saved to {report_path}")
print(f"Total: {all_passed} passed, {all_failed} failed across {len(families)} families ({total_time:.1f}s)")
