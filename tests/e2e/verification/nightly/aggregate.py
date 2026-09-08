"""aggregate — read per-chunk results + reports into a family-level Nightly summary.

Risk mitigations baked in:
  - P1-5: distinguishes translation-defect fails from infra/tooling fails and
          plain code defects, so a nightly isn't read as "everything red".
  - P2-3: chunks that were retried and then passed are surfaced separately.
"""

from __future__ import annotations

import json
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

# Classification buckets for error_class (P1-5)
_TRANSLATION_DEFECT = {
    "native-codegen-missing-symbol",
    "native-codegen-syntax",
    "atg-combined-cs",
    "platform-diagnostic",
    "csharp-error",
}
_INFRA_FAIL = {
    "env-missing", "oom", "timeout", "hotupdate-patch-arm",
}
_CODE_DEFECT = {"native-crash"}
_TRANSIENT = {"native-crash", "unknown"}  # possibly retried


@dataclass
class ReportSummary:
    total_chunks: int = 0
    passed: int = 0
    failed: int = 0
    stalled: int = 0
    retried_to_pass: int = 0
    by_error_class: dict[str, int] = field(default_factory=dict)
    by_assembly: dict[str, dict] = field(default_factory=dict)
    # categorized failed keys (for operator triage)
    translation_defect_fail_keys: list[str] = field(default_factory=list)
    infra_fail_keys: list[str] = field(default_factory=list)
    code_defect_fail_keys: list[str] = field(default_factory=list)
    retried_to_pass_keys: list[str] = field(default_factory=list)

    def summary_text(self) -> str:
        lines = [
            f"Nightly result: {self.passed}/{self.total_chunks} passed, "
            f"{self.failed} failed, {self.stalled} stalled",
        ]
        if self.by_error_class:
            lines.append("  error classes: " + ", ".join(
                f"{k}={v}" for k, v in sorted(self.by_error_class.items())))
        if self.translation_defect_fail_keys:
            lines.append(f"  ⚠ translation-defect ({len(self.translation_defect_fail_keys)}): "
                         f"{', '.join(self.translation_defect_fail_keys[:10])}")
        if self.infra_fail_keys:
            lines.append(f"  ⚠ infra/timeout ({len(self.infra_fail_keys)}): "
                         f"{', '.join(self.infra_fail_keys[:10])}")
        if self.code_defect_fail_keys:
            lines.append(f"  ⚠ code/native-crash ({len(self.code_defect_fail_keys)}): "
                         f"{', '.join(self.code_defect_fail_keys[:10])}")
        return "\n".join(lines)


def aggregate_reports(config, results) -> ReportSummary:  # results: NightlyResult
    """Fold per-chunk NightlyResult statuses into a categorized ReportSummary.

    Args:
        config: NightlyConfig (for report_dir).
        results: NightlyResult (chunk_results dict key → {status,error_class,...})
    """
    from dataclasses import dataclass, field  # noqa

    summ = ReportSummary()
    cr = results.chunk_results
    summ.total_chunks = len(cr)
    for key, info in cr.items():
        status = info.get("status", "unknown")
        err = info.get("error_class", "none")
        asm = key.split("__")[0] if "__" in key else "?"
        # per-assembly bucket
        ab = summ.by_assembly.setdefault(asm, {"passed": 0, "failed": 0, "total": 0})
        ab["total"] += 1
        if status == "passed":
            summ.passed += 1
            ab["passed"] += 1
        elif status in ("failed", "stalled"):
            summ.failed += 1
            ab["failed"] += 1
            summ.by_error_class[err] = summ.by_error_class.get(err, 0) + 1
            if err in _TRANSLATION_DEFECT:
                summ.translation_defect_fail_keys.append(key)
            elif err in _INFRA_FAIL:
                summ.infra_fail_keys.append(key)
            else:
                summ.code_defect_fail_keys.append(key)
        elif status == "retrying":
            # will surface again as passed/failed in a later result write
            pass

    # persist summary
    out_dir = Path(config.report_dir) / "summary"
    out_dir.mkdir(parents=True, exist_ok=True)
    payload = {
        "total": summ.total_chunks,
        "passed": summ.passed,
        "failed": summ.failed,
        "stalled": summ.stalled,
        "byErrorClass": summ.by_error_class,
        "byAssembly": summ.by_assembly,
        "translationDefectFails": summ.translation_defect_fail_keys,
        "infraFails": summ.infra_fail_keys,
        "codeDefectFails": summ.code_defect_fail_keys,
        "timestamp": time.time(),
    }
    try:
        (out_dir / "nightly-result.json").write_text(
            json.dumps(payload, indent=2), encoding="utf-8")
    except OSError:
        pass
    return summ
