"""Worker: verify one family. Called by parallel_runner.sh or manually.
Usage: python verify_one_family.py <assembly> <slug>
"""
import sys, json, time
from pathlib import Path

REPO = Path(__file__).resolve().parents[5]
sys.path.insert(0, str(REPO / "build" / "toolchains" / "run" / "testing" / "foundation_dll"))
sys.path.insert(0, str(REPO / "build" / "toolchains" / "run"))

assembly = sys.argv[1]
slug = sys.argv[2]

t0 = time.time()
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

elapsed = time.time() - t0
output = json.dumps({"status": status, "stages": stages, "coverage": coverage, "elapsed_s": round(elapsed, 1)})
# Write result to a temp file for the parent process
result_dir = REPO / "batch_results_tmp"
result_dir.mkdir(exist_ok=True)
(result_dir / f"{assembly}_{slug}.json").write_text(output, encoding="utf-8")
print(f"[{assembly}/{slug}] {status} ({elapsed:.1f}s)")
