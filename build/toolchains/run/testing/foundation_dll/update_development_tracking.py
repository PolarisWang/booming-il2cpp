"""Update development-tracking.json with current phase per family.

Scans verification artifacts for each family and determines which
development phase (skeleton → codegen → hotupdate → closure) the
family is currently in. Phase promotion is conservative: never
downgrade a phase that was previously achieved.
"""

from __future__ import annotations

import json
import os
import sys
import time
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]  # D:/agent/booming-il2cpp
_VERIFICATION = _REPO_ROOT / "verification" / "foundation-dll"
_TRACKING_PATH = _REPO_ROOT / "docs" / "verification" / "foundation-dll-audit" / "development-tracking.json"

sys.path.insert(0, str(_HERE))

from native_codegen_generator import _slug_from_family_id

# CoreLib — 31 families tracked currently
ASSEMBLY_NAME = "System.Private.CoreLib"
FAMILIES_DIR = _VERIFICATION / ASSEMBLY_NAME


def _string(value: Any) -> str:
    return str(value or "").strip()


def _load_contract(family_slug: str) -> dict[str, Any] | None:
    """Load capability-family-contract.json for a family."""
    contract_path = FAMILIES_DIR / family_slug / "capability-family-contract.json"
    if not contract_path.exists():
        return None
    try:
        return json.loads(contract_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _resolve_path(*segments: str) -> Path:
    """Resolve a path, preferring il2cpp_dist over native for codegen artifacts."""
    p = FAMILIES_DIR / segments[0] / "native" / Path(*segments[1:])
    if p.exists():
        return p
    if len(segments) >= 2 and segments[1] == "native":
        alt = FAMILIES_DIR / segments[0] / "il2cpp_dist" / Path(*segments[2:])
        if alt.exists():
            return alt
    return p


def _load_codegen_metrics(family_slug: str) -> dict[str, Any] | None:
    """Load native-aot.codegen-metrics.json for genuine output."""
    path = _resolve_path(family_slug, "native", "genuine", "native-aot.codegen-metrics.json")
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _load_coverage(family_slug: str) -> dict[str, Any] | None:
    """Load skeleton coverage JSON."""
    path = FAMILIES_DIR / family_slug / "native" / "native-reference.runtime-skeleton.coverage.json"
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def _load_previous_tracking() -> dict[str, Any]:
    """Load existing development-tracking.json, if any."""
    if _TRACKING_PATH.exists():
        try:
            return json.loads(_TRACKING_PATH.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            pass
    return {}


def _load_review_verdict(family_slug: str, review_type: str) -> str:
    """Read AI review verdict from review/<family>/<type>-review-v*.md.

    Returns 'passed', 'failed', or 'pending'.
    """
    review_dir = FAMILIES_DIR / family_slug / "review"
    if not review_dir.is_dir():
        return "pending"
    # Match files like codegen-review-v1.md, hotupdate-review-v2.md
    pattern = f"{review_type}-review-v*.md"
    matching = list(review_dir.glob(pattern))
    if not matching:
        return "pending"
    # Use the latest version
    latest = sorted(matching)[-1]
    content = latest.read_text(encoding="utf-8")
    if "# Verdict: PASS" in content or "## Verdict: PASS" in content:
        return "passed"
    if "# Verdict: FAIL" in content or "## Verdict: FAIL" in content:
        return "failed"
    return "pending"


def scan_family_phase(family_slug: str) -> dict[str, Any]:
    """Evaluate the current development phase for one family.

    Returns a dict with 'phase' and per-phase gate status.
    """
    contract = _load_contract(family_slug)
    if contract is None:
        return {"phase": "skeleton", "gates": {}, "methodCount": 0}

    method_ids = contract.get("methodSubjectIds", [])
    method_count = len(method_ids)

    # Check artifact existence
    genuine_cpp = _resolve_path(family_slug, "native", "genuine", "generated", "native-aot.generated.cpp")
    genuine_metrics = _load_codegen_metrics(family_slug)
    patch_dir = FAMILIES_DIR / family_slug / "native" / "patch" / "generated"
    hotupdate_dir = FAMILIES_DIR / family_slug / "native" / "hotupdate"
    review_dir = FAMILIES_DIR / family_slug / "review"
    test_dir = FAMILIES_DIR / family_slug / "test"

    has_genuine_cpp = genuine_cpp.exists() and genuine_cpp.stat().st_size > 0
    has_patch = patch_dir.is_dir() and any(patch_dir.iterdir())
    has_hotupdate = hotupdate_dir.is_dir() and any(hotupdate_dir.iterdir())
    has_tests = test_dir.is_dir() and any(test_dir.iterdir())

    # Gate hints from codegen-metrics
    generated_symbol_count = 0
    has_real_methods = False
    if genuine_metrics:
        generated_symbol_count = int(genuine_metrics.get("generatedSymbolCount", 0))
        has_real_methods = generated_symbol_count > method_count  # more symbols than claim count = real codegen

    # Previous phase (for conservative promotion)
    prev_tracking = _load_previous_tracking()
    prev_phase = "skeleton"
    prev_family = prev_tracking.get("families", {}).get(family_slug, {})
    if prev_family:
        prev_phase = prev_family.get("phase", "skeleton")

    phase_order = {"skeleton": 0, "codegen": 1, "hotupdate": 2, "closure": 3}
    current_phase_idx = phase_order.get(prev_phase, 0)

    # Evaluate Phase 1 (Skeleton):
    skeleton_passed = has_genuine_cpp or genuine_metrics is not None

    # Evaluate Phase 2 (CodeGen):
    codegen_review = _load_review_verdict(family_slug, "codegen")
    codegen_gate = codegen_review if codegen_review != "pending" else ("passed" if has_real_methods and has_tests else "pending")
    codegen_passed = (
        skeleton_passed
        and has_real_methods
        and has_tests
        and codegen_review != "failed"
    )

    # Evaluate Phase 3 (HotUpdate):
    hotupdate_review = _load_review_verdict(family_slug, "hotupdate")
    hotupdate_passed = (
        codegen_passed
        and has_patch
        and has_hotupdate
        and hotupdate_review != "failed"
    )

    # Evaluate Phase 4 (Closure):
    closure_passed = (
        hotupdate_passed
        and codegen_review == "passed"
        and hotupdate_review == "passed"
    )

    # Determine target phase (conservative: never downgrade)
    target_phase = prev_phase
    if closure_passed and current_phase_idx < 4:
        target_phase = "closure"
    elif hotupdate_passed and current_phase_idx < 3:
        target_phase = "hotupdate"
    elif codegen_passed and current_phase_idx < 2:
        target_phase = "codegen"
    elif skeleton_passed and current_phase_idx < 1:
        target_phase = "skeleton"

    gates = {
        "skeleton": {
            "nativeProof": "passed" if skeleton_passed else "pending",
            "hasGenuineCpp": has_genuine_cpp,
            "placeholderCount": method_count,
        },
        "codegen": {
            "nativeProof": "passed" if has_real_methods else ("passed" if skeleton_passed else "pending"),
            "managedProof": "passed" if has_tests else "pending",
            "aiCodegenReview": codegen_review,
            "generatedSymbolCount": generated_symbol_count,
            "hasTests": has_tests,
        },
        "hotupdate": {
            "hotupdateProof": "passed" if has_patch else "pending",
            "hotupdateAssertion": "passed" if has_hotupdate else "pending",
            "aiHotupdateReview": hotupdate_review,
            "hasPatch": has_patch,
        },
        "closure": {
            "completionCertification": "passed" if closure_passed else "pending",
            "allCodegenGatesPassed": codegen_passed,
            "allHotupdateGatesPassed": hotupdate_passed,
        },
    }

    return {
        "phase": target_phase,
        "gates": gates,
        "methodCount": method_count,
        "generatedSymbolCount": generated_symbol_count,
    }


def _load_pipeline_errors() -> dict[str, dict[str, Any]]:
    """Load pipeline results and extract per-family error info.

    Returns dict of family_slug -> {error, failedStep, timestamp}.
    """
    errors: dict[str, dict[str, Any]] = {}
    pipeline_path = FAMILIES_DIR / "reports" / "batch-native-aot-pipeline-results.json"
    if pipeline_path.exists():
        try:
            data = json.loads(pipeline_path.read_text(encoding="utf-8"))
            for r in data.get("results", []):
                slug = r.get("family", "")
                if not slug:
                    continue
                if not r.get("success", True):
                    steps = r.get("steps", {})
                    failed_step = next((k for k, v in steps.items() if v == "FAILED"), None)
                    errors[slug] = {
                        "error": r.get("error", "pipeline failed"),
                        "failedStep": failed_step or "unknown",
                        "timestamp": data.get("timestamp", ""),
                    }
                elif r.get("steps", {}).get("fact_static_verify") != "OK":
                    # Fact Static didn't fully pass — record as warning, not fatal error
                    fs_passed = r.get("fact_static_passed", 0)
                    fs_total = r.get("fact_static_total", 0)
                    if fs_total and fs_passed < fs_total:
                        errors[slug] = {
                            "error": f"Fact Static: {fs_passed}/{fs_total} passed",
                            "failedStep": "fact_static_verify",
                            "timestamp": data.get("timestamp", ""),
                        }
        except (OSError, json.JSONDecodeError):
            pass

    # Also check hotupdate pipeline results
    hotupdate_path = FAMILIES_DIR / "reports" / "batch-hotupdate-pipeline-results.json"
    if hotupdate_path.exists():
        try:
            data = json.loads(hotupdate_path.read_text(encoding="utf-8"))
            for r in data.get("results", []):
                slug = r.get("family", "")
                if not slug:
                    continue
                if not r.get("success", True):
                    # Prefer native-aot errors over hotupdate errors
                    if slug not in errors:
                        errors[slug] = {
                            "error": r.get("error", "hotupdate pipeline failed"),
                            "failedStep": "hotupdate",
                            "timestamp": data.get("timestamp", ""),
                        }
        except (OSError, json.JSONDecodeError):
            pass

    return errors


def update_development_tracking() -> dict[str, Any]:
    """Scan all families and update development-tracking.json.

    Returns summary dict with phase counts.
    """
    families: dict[str, Any] = {}
    phase_counts: dict[str, int] = {"skeleton": 0, "codegen": 0, "hotupdate": 0, "closure": 0}

    pipeline_errors = _load_pipeline_errors()

    family_dirs = sorted([
        d.name for d in FAMILIES_DIR.iterdir()
        if d.is_dir() and (d / "capability-family-contract.json").exists()
    ])

    for slug in family_dirs:
        info = scan_family_phase(slug)
        entry: dict[str, Any] = {
            "phase": info["phase"],
            "gates": info["gates"],
            "methodCount": info["methodCount"],
            "generatedSymbolCount": info["generatedSymbolCount"],
            "lastUpdated": datetime.now(timezone.utc).isoformat(),
            "traceSessionIds": [],
        }
        # Inject pipeline error info if available
        if slug in pipeline_errors:
            entry["lastError"] = pipeline_errors[slug]["error"]
            entry["lastFailedStep"] = pipeline_errors[slug]["failedStep"]
            entry["lastPipelineRun"] = pipeline_errors[slug]["timestamp"]
        families[slug] = entry
        phase_counts[info["phase"]] = phase_counts.get(info["phase"], 0) + 1

    total = len(families)
    # Weighted progress: skeleton=25%, codegen=50%, hotupdate=75%, closure=100%
    phase_weight = {"skeleton": 0.25, "codegen": 0.50, "hotupdate": 0.75, "closure": 1.0}
    weighted_sum = sum(
        phase_weight.get(f["phase"], 0.0) for f in families.values()
    )
    overall_progress = round((weighted_sum / total) * 100, 1) if total > 0 else 0.0

    payload = {
        "schemaVersion": 1,
        "programId": "foundation-dll-translation-audit",
        "phases": ["skeleton", "codegen", "hotupdate", "closure"],
        "assemblyName": ASSEMBLY_NAME,
        "families": families,
        "summary": {
            "totalFamilies": total,
            "phaseCounts": phase_counts,
            "overallProgress": overall_progress,
        },
        "lastUpdated": datetime.now(timezone.utc).isoformat(),
    }

    _TRACKING_PATH.parent.mkdir(parents=True, exist_ok=True)
    _TRACKING_PATH.write_text(
        json.dumps(payload, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )

    print(f"Development tracking updated: {_TRACKING_PATH}")
    print(f"  Total families: {total}")
    print(f"  Phase distribution: {phase_counts}")
    print(f"  Overall progress: {overall_progress}%")
    return payload


def main() -> None:
    payload = update_development_tracking()
    # Print per-family phase summary
    print("\nPer-family phase:")
    for slug, info in sorted(payload["families"].items()):
        print(f"  {slug:35s} → {info['phase']:10s} ({info['methodCount']} methods)")


if __name__ == "__main__":
    main()
