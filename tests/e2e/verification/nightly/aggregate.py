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

    # persist summary JSON
    #
    # TWO copies, deliberately.
    #
    # `nightly-result.json` is the shared, well-known name and stays for
    # backward compatibility — but it is a SINGLE file that every run
    # overwrites, and the agents accumulate many runs (run-state/ held 13
    # entries spanning hours). A publisher reading it can therefore report a
    # different run's numbers: build 277 published 0/45 while its own
    # run-state recorded 21 chunks passed, because by the time it read the
    # summary another run had replaced it.
    #
    # `run-<run_id>.json` is this run's own, never overwritten. The publish step
    # prefers it (the run id is in provenance) and falls back to the shared
    # name only when there is no per-run copy, which covers older payloads.
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
        "runId": config.run_id,
        "nativeConfig": config.native_config,
    }
    blob = json.dumps(payload, indent=2)
    for name in ("nightly-result.json", f"run-{config.run_id}.json"):
        try:
            (out_dir / name).write_text(blob, encoding="utf-8")
        except OSError as e:
            # Loud: a summary that silently fails to land is how the published
            # numbers drift from what actually ran.
            print(f"  [summary] FAILED to write {name}: {e}", flush=True)

    # Also emit an old-compatible human-readable summary markdown (what consumers
    # watching nightly-reports read).  This closes the reviewer's L2 gap: nightly
    # report markdown is produced even though the lean runner no longer shells out
    # to an aggregate/reporting subprocess per family.
    try:
        md_lines = [
            "# Nightly Build Summary",
            "",
            f"- Run: `{config.run_id}`",
            f"- Config: `{config.native_config}`",
            f"- Result: **{summ.passed}/{summ.total_chunks} passed**, "
            f"{summ.failed} failed, {summ.stalled} stalled",
            "",
            "## Error classes",
            "",
        ]
        if summ.by_error_class:
            md_lines.append("| class | count |")
            md_lines.append("|-------|-------|")
            for k, v in sorted(summ.by_error_class.items()):
                md_lines.append(f"| {k} | {v} |")
        md_lines += ["", "## Failed chunks", ""]
        if summ.translation_defect_fail_keys:
            md_lines += ["### translation-defect (real codegen issues)", ""]
            md_lines += [f"- `{k}`" if "`" not in k else f"- {k}"
                         for k in summ.translation_defect_fail_keys]
        if summ.infra_fail_keys:
            md_lines += ["### infra / timeout", ""]
            md_lines += [f"- {k}" for k in summ.infra_fail_keys]
        if summ.code_defect_fail_keys:
            md_lines += ["### other", ""]
            md_lines += [f"- {k}" for k in summ.code_defect_fail_keys]
        (out_dir / "nightly-summary.md").write_text(
            "\n".join(md_lines) + "\n", encoding="utf-8")
    except OSError:
        pass

    # ── Persist baseline snapshot for triage ──
    # Best-effort: never let a failed baseline write interrupt the nightly.
    #
    # But NOT silent. This used to be a bare `except Exception: pass`, and run
    # 20260912_045412-c3d475811 is missing from baseline/index.json while its
    # neighbours (044157, 060525, 064426) are present — the snapshot write
    # failed and nothing said so. That run is exactly the one whose published
    # result (0/45) disagreed with its own run-state (21 chunks passed), so the
    # one piece of evidence that would have explained the discrepancy was lost
    # to a swallowed exception. Record the failure loudly; still never raise.
    try:
        from verification._path import build_root
        from verification.tools.baseline_store import record_from_run
        _snap = record_from_run(
            run_id=config.run_id,
            report_dir=config.report_dir,
            results_base=build_root(),
            native_config=config.native_config,
        )
        if _snap is None:
            # record_from_run returns None when it found nothing to snapshot —
            # the chunk results tree was empty or unreadable. That is a real
            # signal, not a no-op: it means the baseline for this run is absent.
            print(f"  [baseline] NOT RECORDED for {config.run_id}: no chunk results "
                  f"found under {build_root()} — trends will have a gap here",
                  flush=True)
        else:
            print(f"  [baseline] recorded {_snap.name}", flush=True)
    except Exception as e:
        import traceback
        print(f"  [baseline] FAILED for {config.run_id}: "
              f"{type(e).__name__}: {e}", flush=True)
        print("  [baseline] " + traceback.format_exc().replace("\n", "\n  [baseline] "),
              flush=True)

    return summ
