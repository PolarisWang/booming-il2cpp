"""resume — build a resume worklist from a prior run's state files.

Used on a re-run with `--resume <run_id>` to skip chunks that already passed,
so an interrupted nightly can continue without redoing completed work.

Risk mitigation: P3-1 (tolerant reads — unknown/missing result files are
treated as "not passed" → included in the resume set).
"""

from __future__ import annotations

from .worklist import WorkItem
from . import state as nstate


def build_resume_worklist(config, full_worklist: list[WorkItem]) -> list[WorkItem]:
    """Return only WorkItems whose prior result (under resume_run_id) is not passed.

    Items with no result file at all (not present in prior run) are included
    (they simply didn't run before).  Items whose prior status == "passed" are
    skipped.  Anything else (failed/stalled/retrying/unknown) is re-run.
    """
    if not config.resume_run_id:
        # Nothing to resume against; caller should have passed a run id.
        return list(full_worklist)

    # Read prior state from <report_dir>/run-state/<resume_run_id> — the PRIOR
    # run's results, not the current run's (config.run_id differs).
    prior = nstate.read_all_results(config, for_run_id=config.resume_run_id)
    remaining = []
    skipped = 0
    for w in full_worklist:
        prior_res = prior.get(w.key, {})
        status = prior_res.get("status", "unknown")
        if status == "passed":
            skipped += 1
            continue
        remaining.append(w)

    print(f"  [resume] {skipped} already-passed chunks skipped, {len(remaining)} to run",
          flush=True)
    return remaining
