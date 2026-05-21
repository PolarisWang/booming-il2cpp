"""Merge re-run results into full batch report and generate dashboard."""
from __future__ import annotations

import json
import sys
from pathlib import Path

_REPO_ROOT = Path(__file__).resolve().parent
_DASHBOARD = _REPO_ROOT / "testing" / "foundation-dll" / "_core" / "python" / "deep_dashboard.py"
_RESULTS_DIR = _REPO_ROOT / "testing" / "results"
_FULL_REPORT = _RESULTS_DIR / "batch-report-full.json"
_RERUN_RESULTS = _RESULTS_DIR / "rerun-results.json"
_OUTPUT_REPORT = _RESULTS_DIR / "batch-report.json"
_OUTPUT_DASHBOARD = _RESULTS_DIR / "deep-dashboard.html"

# Load full 46-family report
full = json.loads(_FULL_REPORT.read_text(encoding="utf-8"))
rerun = json.loads(_RERUN_RESULTS.read_text(encoding="utf-8"))

# Update results with re-run data
updated = set()
for res in full["results"]:
    slug = res["slug"]
    if slug in rerun:
        new = rerun[slug]
        if "overall_status" in new:
            old_status = res.get("status", "?")
            new_status = new.get("overall_status", "?")
            res["status"] = new_status
            res["overall_status"] = new_status
        if "stages" in new:
            res["stages"] = new["stages"]
        if "coverage" in new:
            res["coverage"] = new["coverage"]
        res["duration_seconds"] = new.get("duration_seconds", res.get("duration_seconds", 0))
        updated.add(slug)

# Recompute totals
passed = sum(1 for r in full["results"] if r.get("status") == "passed")
failed = sum(1 for r in full["results"] if r.get("status") == "failed")
full["passed"] = passed
full["failed"] = failed

# Write merged report
_OUTPUT_REPORT.write_text(json.dumps(full, indent=2, ensure_ascii=False), encoding="utf-8")
total = len(full["results"])
print(f"Merged report: {total} families, {passed} passed, {failed} failed")
print(f"Updated {len(updated)} families: {sorted(updated)}")
print(f"Report: {_OUTPUT_REPORT}")

# Generate dashboard
sys.path.insert(0, str(_DASHBOARD.parent))
import importlib.util
spec = importlib.util.spec_from_file_location("deep_dashboard", _DASHBOARD)
dd = importlib.util.module_from_spec(spec)
spec.loader.exec_module(dd)

report = dd.load_batch_report(_OUTPUT_REPORT)
html = dd.generate_html(report)
_OUTPUT_DASHBOARD.write_text(html, encoding="utf-8")
print(f"Dashboard: {_OUTPUT_DASHBOARD} ({len(html)} bytes)")
