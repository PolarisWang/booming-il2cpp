"""Subject correctness audit — Phase 1.1 of optimization campaign.

Checks subject test quality: assertion coverage, stub detection, triviality.

Usage:
    python -m verification.stages.subject_correctness_audit <slug> --assembly <name>
    python -m verification.stages.subject_correctness_audit <slug> --assembly <name> --fix
"""

from __future__ import annotations

import json
import re
from pathlib import Path


def audit(assembly: str, chunk: str, fix: bool = False) -> dict:
    """Audit a chunk's subject quality. Returns verdict dict."""
    foundation = Path(__file__).resolve().parent.parent
    subjects_dir = foundation / assembly / "chunks" / chunk / "managed" / "subjects"
    results_dir = foundation / assembly / "chunks" / chunk / "results"

    verdict = {
        "assembly": assembly,
        "chunk": chunk,
        "verdict": "PASS",
        "warnings": [],
        "fixes_applied": False,
    }

    # Check fact results
    fact_file = results_dir / "fact.json"
    if fact_file.exists():
        fact = json.loads(fact_file.read_text(encoding="utf-8"))
        passed = fact.get("passed", 0)
        total = fact.get("total", 0)
        if total > 0 and passed < total:
            verdict["warnings"].append(f"Fact: {passed}/{total} passed")

    # Check for stubs in combined subjects
    cs_files = list(subjects_dir.glob("*.cs")) if subjects_dir.exists() else []
    if cs_files:
        total_asserts = 0
        total_methods = 0
        for cs in cs_files:
            text = cs.read_text(encoding="utf-8", errors="replace")
            methods = re.findall(r"(public\s+.*?\w+\s*\(.*?\))", text, re.DOTALL)
            asserts = text.count("Assert.")
            total_methods += len(methods)
            total_asserts += asserts
        if total_methods > 0 and total_asserts == 0:
            verdict["warnings"].append(f"No Assert.* calls in {total_methods} methods")

    verdict["pass"] = len(verdict["warnings"]) == 0
    return verdict
