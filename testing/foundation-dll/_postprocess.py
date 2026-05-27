"""Post-process: rebuild batch report, regenerate dashboard after hotupdate runs."""
from __future__ import annotations

import json
import time
from pathlib import Path

REPORTS_ROOT = Path("System.Private.CoreLib")
RESULTS_DIR = Path("results/foundation-dll/System.Private.CoreLib")
BATCH_PATH = Path("results/batch-report.json")

# Step 1: Rebuild batch report including new HU data
# Unified reports are written by run_pipeline.py to results/foundation-dll/<assembly>/<slug>/
results = []
results_base = (Path.cwd().parent if Path.cwd().name == "System.Private.CoreLib" else Path.cwd()) / RESULTS_DIR
if not results_base.exists():
    results_base = Path.cwd() / RESULTS_DIR

for family_dir in sorted(REPORTS_ROOT.iterdir()):
    if not family_dir.is_dir():
        continue
    # Prefer the results-dir report (fresh from this batch run), fall back
    # to the family-dir report (legacy location).
    results_report = results_base / family_dir.name / "unified-verification-report.json"
    family_report = family_dir / "unified-verification-report.json"
    report_file = results_report if results_report.exists() else family_report
    if not report_file.exists():
        continue
    try:
        raw = json.loads(report_file.read_text(encoding="utf-8"))
        results.append({
            "slug": raw.get("family", family_dir.name),
            "status": raw.get("overall_status", "unknown"),
            "duration_seconds": round(raw.get("duration_ms", 0) / 1000, 1),
            "stages": raw.get("stages", {}),
            "coverage": raw.get("coverage"),
            "dashboard": raw.get("dashboard"),
            "regression": raw.get("regression"),
        })
    except Exception as e:
        print(f"  WARN: {report_file}: {e}")

report = {
    "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
    "assembly": "System.Private.CoreLib",
    "total_families": len(results),
    "passed": sum(1 for r in results if r["status"] == "passed"),
    "failed": sum(1 for r in results if r["status"] == "failed"),
    "crashed": sum(1 for r in results if r["status"] == "crashed"),
    "skipped": sum(1 for r in results if r["status"] == "skipped"),
    "elapsed_seconds": round(sum(r.get("duration_seconds", 0) for r in results), 1),
    "results": results,
}

BATCH_PATH.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")
print(f"Batch report: {len(results)} families -> {BATCH_PATH}")

# Step 2: Regenerate dashboard
from verification.reporting.dashboard import generate_html, parse_family

report["parsed"] = [parse_family(r) for r in results]
html = generate_html(report)

dashboard_path = BATCH_PATH.with_name("deep-dashboard.html")
dashboard_path.write_text(html, encoding="utf-8")
print(f"Dashboard: {len(html)/1024:.0f} KB -> {dashboard_path}")

# Step 3: Final summary
from verification.reporting.dashboard import _compute_benchmark_comparisons

families = sorted([d.name for d in REPORTS_ROOT.iterdir() if d.is_dir() and d.name != "System.Private.CoreLib"])
hu_count = net8_ok = aot_ok = hu_net8_valid = hu_net8_within_3x = 0
violations = []

for slug in families:
    bc = _compute_benchmark_comparisons(slug)
    techs = bc.get("technologies", {})
    comps = bc.get("comparisons", {})

    if techs.get("chaos-hu-aot"): hu_count += 1
    if techs.get("net8-jit"): net8_ok += 1
    if techs.get("chaos-aot"): aot_ok += 1

    hvn = comps.get("hu_aot_vs_net8", {})
    if hvn.get("status") == "completed":
        ratio = hvn.get("ratio")
        hu_net8_valid += 1
        if ratio and ratio >= 0.333:
            hu_net8_within_3x += 1
        else:
            violations.append((slug, ratio))

print(f"\n=== FINAL SUMMARY ===")
print(f"Total families: {len(families)}")
print(f"HU-AOT data:    {hu_count}/{len(families)}")
print(f"AOT data:       {aot_ok}/{len(families)}")
print(f"NET8 data:      {net8_ok}/{len(families)}")
print(f"HU+NET8 both:   {hu_net8_valid}")
print(f"Within 3x:      {hu_net8_within_3x}")
for slug, ratio in violations:
    slower = f"{1/ratio:.1f}x" if ratio and ratio > 0 else "N/A"
    print(f"  VIOLATION: {slug}: {ratio:.2f}x ({slower} slower)")
