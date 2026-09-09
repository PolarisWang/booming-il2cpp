"""resume — build a resume worklist from a prior run's state files.

Used on a re-run with `--resume <run_id>` to skip chunks that already have a
final outcome (passed OR failed/stalled) and only re-run chunks that were still
in-flight (running/retrying) or never started (unknown/absent).
"""

from __future__ import annotations

from .worklist import WorkItem
from . import state as nstate


def build_resume_worklist(config, full_worklist: list[WorkItem]) -> list[WorkItem]:
    """Return only WorkItems that still need a final result.

    Items whose prior result (under resume_run_id) has a terminal status
    ("passed", "failed", "stalled") are SKIPPED — they reached a conclusion.
    Items with status "running", "retrying", "unknown" or NO prior result at
    all are included (they were in-flight or never started when the run died).
    """
    if not config.resume_run_id:
        return list(full_worklist)

    prior = nstate.read_all_results(config, for_run_id=config.resume_run_id)
    terminal = {"passed", "failed", "stalled"}
    remaining: list[WorkItem] = []
    skipped = 0
    for w in full_worklist:
        prior_res = prior.get(w.key, {})
        status = prior_res.get("status", "unknown")
        if status in terminal:
            skipped += 1
            continue
        remaining.append(w)

    print(f"  [resume] {skipped} already-terminated chunks skipped, "
          f"{len(remaining)} in-flight / orphaned to re-run",
          flush=True)
    return remaining
