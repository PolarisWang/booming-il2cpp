"""Performance baseline tracker — compilation time + code size metrics.

Collects and persists codegen performance baselines per family:
  - Generated C++ file size (total, per method)
  - Compiled native binary section sizes (.text, .data, .rdata)
  - Codegen stage wall-clock duration

Stored under testing/results/verification-history/<assembly>/<slug>/baseline-perf.json
Regression detection compares current vs baseline with configurable thresholds.
"""

from __future__ import annotations

import json
import time
from pathlib import Path
from typing import Any

from _core.python.perf_store import _common_fields, _get_git_commit, _detect_device

# Default regression thresholds
DEFAULT_THRESHOLDS = {
    "cpp_size_bytes": {"warning": 1.15, "critical": 1.30},     # 15% / 30% increase
    "binary_text_section_bytes": {"warning": 1.10, "critical": 1.25},
    "codegen_duration_ms": {"warning": 1.20, "critical": 1.50},
    "num_gc_slots": {"warning": 0.80, "critical": 0.50},       # 20% / 50% decrease
    "num_methods": {"warning": 0.90, "critical": 0.80},        # 10% / 20% decrease
}


def collect_perf_baseline(
    family_dir: Path,
    codegen_duration_ms: int = 0,
) -> dict[str, Any]:
    """Collect performance baseline data for a family.

    Args:
        family_dir: Family directory under testing/foundation-dll/<assembly>/<slug>.
        codegen_duration_ms: Codegen stage duration (from pipeline stage result).

    Returns:
        Dict with performance metrics.
    """
    metrics: dict[str, Any] = {
        "cpp_size_bytes": 0,
        "binary_text_section_bytes": 0,
        "binary_total_bytes": 0,
        "codegen_duration_ms": codegen_duration_ms,
        "num_methods": 0,
        "num_gc_slot_entries": 0,
        "num_gc_slots_total": 0,
        "files_measured": [],
    }

    # ── Generated C++ size ────────────────────────────────────────────
    cpp_files = list(family_dir.rglob("native-aot.generated.cpp"))
    for cpp in cpp_files:
        size = cpp.stat().st_size
        metrics["cpp_size_bytes"] += size
        metrics["files_measured"].append({
            "path": str(cpp.relative_to(family_dir)),
            "size_bytes": size,
        })

    # Count methods: estimate from function definitions in generated C++
    if cpp_files:
        import re
        main_cpp = cpp_files[0]
        content = main_cpp.read_bytes()
        func_count = len(re.findall(rb"^[a-zA-Z_]\w*\s+[a-zA-Z_]\w*\([^)]*\)\s*\{", content, re.MULTILINE))
        metrics["num_methods"] = func_count

        # GC slot count
        num_gc = re.findall(rb"num_gc_slots\s*=\s*(\d+)", content)
        if num_gc:
            slot_counts = [int(m) for m in num_gc]
            metrics["num_gc_slot_entries"] = len(slot_counts)
            metrics["num_gc_slots_total"] = sum(slot_counts)

    # ── Compiled binary size ──────────────────────────────────────────
    binary = family_dir / "native" / "entry.exe"
    if binary.exists():
        metrics["binary_total_bytes"] = binary.stat().st_size

    # Try to get section sizes (Windows: dumpbin, Linux: size/objdump)
    metrics["binary_text_section_bytes"] = _measure_text_section(binary)

    return metrics


def _measure_text_section(binary: Path) -> int:
    """Measure .text section size using dumpbin or size utility."""
    if not binary.exists():
        return 0

    try:
        import subprocess
        # Windows: dumpbin /HEADERS
        r = subprocess.run(
            ["dumpbin", "/HEADERS", str(binary)],
            capture_output=True, text=True, timeout=15,
        )
        for line in r.stdout.splitlines():
            if ".text" in line and "size" in line.lower():
                # Parse: "  .text   name    XXXXXX   size"
                parts = line.strip().split()
                for i, p in enumerate(parts):
                    if p == "size" and i + 1 < len(parts):
                        try:
                            return int(parts[i + 1])
                        except ValueError:
                            pass
        return 0
    except Exception:
        return 0


def save_baseline(
    results_dir: Path,
    assembly: str,
    slug: str,
    metrics: dict[str, Any],
) -> Path:
    """Save performance baseline to verification-history.

    Args:
        results_dir: results directory (e.g., testing/results).
        assembly: Assembly name.
        slug: Family slug.
        metrics: Performance metrics dict.

    Returns:
        Path to saved baseline file.
    """
    baseline_dir = results_dir / "verification-history" / assembly / slug
    baseline_dir.mkdir(parents=True, exist_ok=True)
    baseline_path = baseline_dir / "baseline-perf.json"

    baseline = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "gitCommit": _get_git_commit(),
        "device": _detect_device(),
        "metrics": metrics,
    }

    baseline_path.write_text(
        json.dumps(baseline, indent=2, ensure_ascii=False),
        encoding="utf-8",
    )
    return baseline_path


def load_baseline(
    results_dir: Path,
    assembly: str,
    slug: str,
) -> dict[str, Any] | None:
    """Load existing performance baseline if available."""
    baseline_path = results_dir / "verification-history" / assembly / slug / "baseline-perf.json"
    if not baseline_path.exists():
        return None
    try:
        return json.loads(baseline_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return None


def detect_regression(
    current: dict[str, Any],
    baseline: dict[str, Any],
    thresholds: dict[str, dict[str, float]] | None = None,
) -> dict[str, Any]:
    """Compare current metrics against baseline and detect regressions.

    Args:
        current: Current performance metrics.
        baseline: Baseline performance metrics (from load_baseline).
        thresholds: Per-metric threshold overrides. Uses DEFAULT_THRESHOLDS.

    Returns:
        Dict with has_regression (bool), regressions list, and warnings list.
    """
    if thresholds is None:
        thresholds = DEFAULT_THRESHOLDS

    result: dict[str, Any] = {
        "has_regression": False,
        "has_warning": False,
        "regressions": [],
        "warnings": [],
        "improvements": [],
        "ratios": {},
    }

    current_metrics = current.get("metrics", current)
    baseline_metrics = baseline.get("metrics", baseline)

    all_keys = set(current_metrics.keys()) & set(baseline_metrics.keys())

    for key in all_keys:
        cv = current_metrics[key]
        bv = baseline_metrics[key]

        # Skip non-numeric or zero-baseline
        if not isinstance(cv, (int, float)) or not isinstance(bv, (int, float)):
            continue
        if bv == 0:
            continue

        ratio = cv / bv
        result["ratios"][key] = round(ratio, 3)

        threshold = thresholds.get(key)
        if not threshold:
            continue

        critical = threshold.get("critical", 2.0)
        warning = threshold.get("warning", 1.5)

        if ratio >= critical:
            result["has_regression"] = True
            result["regressions"].append({
                "metric": key,
                "baseline": bv,
                "current": cv,
                "ratio": round(ratio, 3),
                "severity": "critical",
                "message": f"{key}: {cv} vs baseline {bv} ({ratio:.1f}x) — CRITICAL",
            })
        elif ratio >= warning:
            result["has_warning"] = True
            result["warnings"].append({
                "metric": key,
                "baseline": bv,
                "current": cv,
                "ratio": round(ratio, 3),
                "severity": "warning",
                "message": f"{key}: {cv} vs baseline {bv} ({ratio:.1f}x) — WARNING",
            })
        # Improvement (ratio below the inverse of warning)
        inv_warning = 1.0 / warning if warning > 0 else 0.5
        if ratio <= inv_warning:
            result["improvements"].append({
                "metric": key,
                "baseline": bv,
                "current": cv,
                "ratio": round(ratio, 3),
                "message": f"{key}: {cv} vs baseline {bv} ({ratio:.1f}x) — improved",
            })

    return result
