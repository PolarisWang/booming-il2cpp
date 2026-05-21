"""Re-run all failed families and generate consolidated batch report + dashboard."""
import json
import subprocess
import sys
import time
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parent
_CORE_PACKAGE = _REPO_ROOT / "testing" / "foundation-dll"
_CLI_SCRIPT = _CORE_PACKAGE / "_core" / "python" / "cli.py"
_DASHBOARD_SCRIPT = _CORE_PACKAGE / "_core" / "python" / "deep_dashboard.py"
_RESULTS_DIR = _REPO_ROOT / "testing" / "results"
_BATCH_REPORT = _RESULTS_DIR / "batch-report.json"

# Families to re-run
FAMILIES = [
    "array-indexing-copy", "collections-generic-core", "delegate-core-invocation",
    "enum-parsing", "math-numerics", "reflection-assembly", "reflection-field-property",
    "reflection-member-complete", "reflection-activation", "reflection-member-basics",
    "reflection-type", "type-runtime-handles", "threading-monitor-interlocked", "rcw-basic",
]

# Setup sys.path for imports
sys.path.insert(0, str(_CORE_PACKAGE))
sys.path.insert(0, str(_REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll"))

from _core.python.models import UnifiedReport

report = UnifiedReport(assembly="System.Private.CoreLib")

for i, slug in enumerate(FAMILIES):
    print(f"\n[{i+1}/{len(FAMILIES)}] {slug}...", flush=True)
    t0 = time.time()
    result = subprocess.run(
        [sys.executable, str(_CLI_SCRIPT), slug],
        capture_output=True, text=True, timeout=7200,
        cwd=str(_REPO_ROOT),
    )
    elapsed = time.time() - t0

    found = False
    for line in result.stdout.splitlines():
        if line.startswith("FINAL_REPORT:"):
            try:
                r = json.loads(line[len("FINAL_REPORT:"):])
                report.add_result(r)
                status = r.get("overall_status", "?")
                cov = r.get("coverage", {})
                print(f"  -> {status} ({cov.get('stagesPassed', 0)}/{cov.get('stagesTotal', 0)}) [{elapsed:.0f}s]", flush=True)
                found = True
            except json.JSONDecodeError:
                pass
            break

    if not found:
        passed = result.returncode == 0
        status = "passed" if passed else "failed"
        print(f"  -> {status} (exit={result.returncode}) [{elapsed:.0f}s]", flush=True)

# Write batch report
report_path = _RESULTS_DIR / "batch-report-final.json"
report_dict = report.to_dict()
report_path.write_text(json.dumps(report_dict, ensure_ascii=False, indent=2), encoding="utf-8")
total = len(report_dict.get("results", []))
npassed = report_dict.get("passed", 0)
nfailed = report_dict.get("failed", 0)
print(f"\n=== Final Report ===")
print(f"Total: {total}, Passed: {npassed}, Failed: {nfailed}")

print(f"\nReport: {report_path}")
