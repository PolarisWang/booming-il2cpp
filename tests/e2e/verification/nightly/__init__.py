"""Nightly — subprocess-per-chunk nightly build orchestrator (Route 3).

Replaces `verification.nightly_runner` (process/thread pool that re-imported
`verification.stages.run_*`) with a thin dispatcher that:
  - discovers every (assembly, chunk) worklist item from namespace-partition.json
  - runs each chunk as an isolated `chunk_pipeline` subprocess (per-chunk logs,
    no spawn/module-sharing side effects, real process isolation)
  - tracks per-chunk state (heartbeat + result in separated files), watchdog,
    resume, and structured error_class categorization
  - aggregates per-chunk `_dll/reports` into a family-level nightly summary

The single execution engine is `chunk_pipeline`; this package never imports the
stage functions itself (that duplication was the root of nightly_runner's
fragility).
"""

from .config import NightlyConfig
from .worklist import WorkItem, discover_worklist
from .run import NightlyResult, run_phases
from .aggregate import aggregate_reports

__all__ = [
    "NightlyConfig",
    "WorkItem",
    "discover_worklist",
    "NightlyResult",
    "run_phases",
    "aggregate_reports",
]
