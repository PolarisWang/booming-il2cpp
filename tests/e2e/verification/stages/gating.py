"""gating — unified 'is this fact real enough to let downstream stages run / count?'

Shared judgment used by BOTH the report/aggregate layer (S1) and the stage head
gates in benchmark_chunk.py / hotupdate_chunk.py (S2), so a run-time stage gate
and an aggregate counting gate never drift apart.

Semantics — a chunk's `fact.json` (already carrying realTotal/realPassed, the
smoke-42 UNVERIFIED tail subtracted) is judged:
  * 'pass' — real_pass / real_total >= GATE_REAL_RATIO AND real_total > 0
  * 'fail' — nominal pass looks green but real verification is a tiny fraction
             of dispatched methods (smoke-dominated / not really verified).  So
             downstream benchmark/hotupdate counting should NOT treat this chunk
             as a verified green source.
  * 'skip' — chunk has no fact data (build produced 0 subjects, or not run), so
             there is no gate to apply; downstream keeps its historical behavior.

Threshold default 0.10 (">=10% really verified to be gated through") is a config
drawn from the honest snapshot: most currently-green chunks sit either near 100%
(CoreLib) or below 5% (fully smoke).  Overridable via GATE_REAL_RATIO import so
an operator can loosen/tighten per run without touching stage code.
"""

GATE_REAL_RATIO = 0.10  # min real-passed / real-total to clear the fact gate


def classify_gate(fact: dict | None, *, min_ratio: float = GATE_REAL_RATIO) -> str:
    """Return 'pass' | 'fail' | 'skip' for one chunk's fact.json dict.

    Args:
        fact: per-chunk fact summary dict (total, passed, realTotal, realPassed,
              unverifiedSmoke...).  None or empty -> 'skip'.
        min_ratio: minimum realPassed/realTotal to be 'pass'.
    """
    if not fact:
        return "skip"
    total = fact.get("total") or 0
    if total == 0:
        return "skip"  # no dispatched methods -> nothing to gate

    # real columns: if a stage didn't persist them, fall back to nominal (old
    # producers) — but an empty realTotal after smoke still means all-smoke.
    real_t = fact.get("realTotal")
    real_t = real_t if real_t is not None else fact.get("total") or 0
    real_p = fact.get("realPassed")
    real_p = real_p if real_p is not None else (fact.get("passed") or 0)

    if real_t <= 0:
        # nominal count>0 but real total 0 → everything was smoke → not verified.
        return "fail"

    ratio = real_p / real_t
    if ratio >= min_ratio:
        return "pass"
    return "fail"


def fact_real_ratio(fact: dict | None) -> float:
    """Helper: the real-passed / real-total ratio for a fact dict (0..1)."""
    if not fact:
        return 0.0
    real_t = fact.get("realTotal")
    real_t = real_t if real_t is not None else (fact.get("total") or 0)
    real_p = fact.get("realPassed")
    real_p = real_p if real_p is not None else (fact.get("passed") or 0)
    return real_p / real_t if real_t > 0 else 0.0
