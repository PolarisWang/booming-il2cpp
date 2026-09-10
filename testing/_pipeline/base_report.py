"""Abstract report base — shared result container."""

from __future__ import annotations

from dataclasses import dataclass, field, asdict
from typing import Any


@dataclass
class ReportBase:
    """Base report containing common fields across all pipeline types."""
    slug: str
    mode: str = "standard"
    timestamp: str = ""
    duration_ms: int = 0
    overall_status: str = "pending"  # "passed" | "failed" | "partial"
    stages: dict[str, dict[str, Any]] = field(default_factory=dict)
    coverage: dict[str, float] = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)
