"""Universal stage result — shared across all verification pipelines."""

from __future__ import annotations

from dataclasses import dataclass, field, asdict
from typing import Any


@dataclass
class StageResult:
    """Result of a single verification stage."""
    stage: str
    status: str  # "passed" | "failed" | "skipped" | "error"
    summary: str = ""
    details: dict[str, Any] = field(default_factory=dict)
    errors: list[str] = field(default_factory=list)
    duration_ms: int = 0

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)
