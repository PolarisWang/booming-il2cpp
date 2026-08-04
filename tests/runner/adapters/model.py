"""Shared result model for the test driver.

Every layer adapter returns a SuiteResult. The driver collects these and
reconciles them against known-failure baselines to label "known" failures.
There is NO "skip" state by project rule — every test PASSES or FAILS.
"""
from __future__ import annotations

from dataclasses import dataclass, field
from typing import Optional


@dataclass
class CaseResult:
    name: str
    passed: bool                      # True = passed, False = failed
    duration_s: float = 0.0
    message: Optional[str] = None     # failure/error detail (may be truncated)


@dataclass
class SuiteResult:
    """Outcome of one test group (one project / one ctest / one script)."""
    layer: str
    group: str                        # group name from contract
    passed: int = 0
    failed: int = 0
    total: int = 0
    cases: list[CaseResult] = field(default_factory=list)
    error: Optional[str] = None       # set if the adapter could not run at all
    duration_s: float = 0.0

    @property
    def ok(self) -> bool:
        """A group is OK only if it ran without infra error and had zero failures.
        There is no 'skipped' state: passed/failed/total fully describe a run."""
        if self.error is not None:
            return False
        # total>0 means we actually ran something; a group that ran nothing is a failure
        if self.total == 0:
            return False
        return self.failed == 0
