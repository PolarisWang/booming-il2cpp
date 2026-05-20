"""Core data models for the foundation DLL verification pipeline."""

from __future__ import annotations

import json
import time
from dataclasses import dataclass, field, asdict
from pathlib import Path
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


@dataclass
class FamilyContext:
    """Context for a single family verification run.

    All path resolution uses family_dir as root, removing hardcoded
    _VERIFICATION_BASE dependencies that plagued the old pipeline.
    """
    slug: str                       # "convert-char"
    assembly: str                   # "System.Private.CoreLib"
    family_dir: Path                # Root directory for this family
    mode: str = "standard"          # "standard" | "strict"
    skip_stages: set[str] = field(default_factory=set)
    verbose: bool = False
    codegen_mode: str | None = None  # "aot" | "jit" | None

    @property
    def contract_path(self) -> Path:
        return self.family_dir / "contract.json"

    @property
    def native_dir(self) -> Path:
        return self.family_dir / "native"

    @property
    def managed_dir(self) -> Path:
        return self.family_dir / "managed"

    @property
    def entry_exe_path(self) -> Path:
        return self.native_dir / "entry.exe"

    @property
    def entry_aot_exe_path(self) -> Path:
        return self.native_dir / "entry-aot.exe"

    @property
    def entry_jit_exe_path(self) -> Path:
        return self.native_dir / "entry-jit.exe"

    @property
    def results_dir(self) -> Path:
        """Results output directory under testing/results/foundation-dll/."""
        return self.family_dir.parents[2] / "results" / "foundation-dll" / self.assembly / self.slug


@dataclass
class UnifiedReport:
    """Top-level output of a family verification run."""
    family: str
    assembly: str
    mode: str = "standard"
    timestamp: str = ""
    duration_ms: int = 0
    overall_status: str = "pending"  # "passed" | "failed" | "partial"
    stages: dict[str, dict[str, Any]] = field(default_factory=dict)
    coverage: dict[str, float] = field(default_factory=dict)
    dashboard: dict[str, Any] = field(default_factory=dict)
    regression: dict[str, Any] = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    def to_json(self) -> str:
        return json.dumps(self.to_dict(), indent=2, ensure_ascii=False)
