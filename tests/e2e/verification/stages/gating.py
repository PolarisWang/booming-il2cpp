"""gating — unified 'is this fact real enough to let downstream stages run / count?'

Shared judgment used by BOTH the report/aggregate layer (S1) and the stage head
gates in benchmark_chunk.py / hotupdate_chunk.py (S2), so a run-time stage gate
and an aggregate counting gate never drift apart.

Semantics — a chunk's `fact.json` carries runtime-derived real-vs-smoke counts
(stamped by fact_chunk.py from the per-record `resultKind`):
  * realVerified — value != 42: a genuine assertion produced a real value.
  * unassertable — void / async-void: the 42 is structural, not a gap.
  * smokeUnknown — has a return type but returned 42: the honest coverage gap.
  * failed       — passed == False.

Judgment:
  * 'pass' — realVerified / total >= GATE_REAL_RATIO
             (unassertable records stay in the denominator: an all-void chunk
              must not claim a free 100%).
  * 'fail' — real verification is a tiny fraction of dispatched methods
             (smoke-dominated / not really verified).  Downstream benchmark /
             hotupdate counting must NOT treat this chunk as a verified source.
  * 'skip' — chunk has no fact data (build produced 0 subjects, or not run).

Threshold default 0.05 (">=5% really verified to be gated through").  The long
term lever is driving smokeUnknown down, not lowering this further.  Overridable
via CHAOS_GATE_REAL_RATIO so an operator can loosen/tighten without code change.
"""

import os

GATE_REAL_RATIO = float(os.environ.get("CHAOS_GATE_REAL_RATIO", "0.05"))


def classify_gate(fact: dict | None, *, min_ratio: float = GATE_REAL_RATIO) -> str:
    """Return 'pass' | 'fail' | 'skip' for one chunk's fact.json dict.

    Args:
        fact: per-chunk fact summary dict.  New producers carry the runtime
              triple (realVerified / unassertable / smokeUnknown); legacy
              producers only carry realTotal/realPassed — handled below.
        min_ratio: minimum realVerified/assertable-ratio to be 'pass'.

    Judgement model
    ---------------
    A chunk's records fall into four buckets (see fact_chunk.classify_fact_record):

        real          value != 42 — a genuine assertion produced a real value
        unassertable  void / async-void — the 42 is structural, there is
                      nothing to assert, so this is NOT a coverage gap
        smoke         has a return type but returned 42 — the honest gap
        failed        passed == False — a real failure

    `unassertable` records must not sink a chunk: an assembly that is entirely
    `Task`-returning (e.g. System.Net.ServerSentEvents, whose whole fact surface
    is `SseFormatter.WriteAsync`) has no assertable method at all — grading it
    'fail' would report a coverage gap that does not exist.

    So the denominator excludes unassertable records.  A chunk passes when
    either (a) there is nothing to assert, or (b) the assertable records have a
    real-verified ratio >= min_ratio.
    """
    if not fact:
        return "skip"
    total = fact.get("total") or 0
    if total == 0:
        return "skip"  # no dispatched methods -> nothing to gate

    # ── Preferred: runtime-derived buckets (fact_chunk.py stamps these) ──
    real_v = fact.get("realVerified")
    if real_v is not None:
        unassertable = fact.get("unassertable") or 0
        assertable = total - unassertable
        # Nothing to assert anywhere -> the chunk is as verified as it can be.
        if assertable <= 0:
            return "pass"
        if real_v <= 0:
            return "fail"
        return "pass" if (real_v / assertable) >= min_ratio else "fail"

    # ── Legacy fallback: realTotal/realPassed (pre-runtime-enrichment runs) ──
    real_t = fact.get("realTotal")
    real_t = real_t if real_t is not None else total
    real_p = fact.get("realPassed")
    real_p = real_p if real_p is not None else (fact.get("passed") or 0)
    if real_t <= 0:
        return "fail"
    return "pass" if (real_p / real_t) >= min_ratio else "fail"


def fact_real_ratio(fact: dict | None) -> float:
    """Real-verified fraction of the *assertable* records (0..1).

    Prefers the runtime-derived realVerified/total; falls back to the legacy
    realPassed/realTotal for older fact.json files.
    """
    if not fact:
        return 0.0
    total = fact.get("total") or 0
    real_v = fact.get("realVerified")
    if real_v is not None and total > 0:
        return real_v / total
    real_t = fact.get("realTotal")
    real_t = real_t if real_t is not None else total
    real_p = fact.get("realPassed")
    real_p = real_p if real_p is not None else (fact.get("passed") or 0)
    return real_p / real_t if real_t > 0 else 0.0


def fact_gap_breakdown(fact: dict | None) -> dict:
    """Return the honest four-way split for reporting.

    ``{realVerified, unassertable, smokeUnknown, failed, total}`` — zero-filled
    when a legacy fact.json lacks the runtime fields.
    """
    fact = fact or {}
    return {
        "realVerified": fact.get("realVerified", 0),
        "unassertable": fact.get("unassertable", 0),
        "smokeUnknown": fact.get("smokeUnknown", 0),
        "failed": fact.get("failed", 0),
        "total": fact.get("total", 0),
    }
