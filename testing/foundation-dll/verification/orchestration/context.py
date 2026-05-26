"""Core data models for the foundation DLL verification pipeline."""

from __future__ import annotations

import json
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any


@dataclass
class StageResult:
    """Result of a single verification stage.

    Fields:
        stage: Stage key matching engine.STAGES (e.g. "preflight", "codegen").
        status: One of "passed" (all checks ok), "failed" (checks failed),
                "skipped" (not executed, e.g. by skip_stages config), or
                "error" (unexpected exception during execution).
        summary: Short human-readable summary of the stage outcome.
        details: Stage-specific structured data (method counts, ratios, etc.).
        errors: List of error messages (populated when status is "failed"/"error").
        duration_ms: Wall-clock execution time in milliseconds.
    """
    stage: str
    status: str
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

    Fields:
        slug: Family identifier, typically kebab-case (e.g. "convert-char").
        assembly: Assembly name (e.g. "System.Private.CoreLib").
        family_dir: Root directory for this family's files (contract, managed/,
                    codegen/, native/).
        mode: Verification mode — "standard" (default, requires preflight/codegen/
              fact/audit to pass) or "strict" (also requires hotupdate stages).
        skip_stages: Set of stage keys to skip during pipeline execution.
        verbose: Enable verbose logging output.
        codegen_mode: Code generation mode override — "aot" or "jit" or None
                      (None uses the default from stage config).
        native_config: Native build config — "check" (default, debug-oriented),
                      "profile" (with PROFILE_SCOPE instrumentation), or "ship"
                      (optimized, no instrumentation).  When "profile", the
                      benchmark stage automatically collects PROFILE_DUMP data.
    """
    slug: str
    assembly: str
    family_dir: Path
    mode: str = "standard"
    skip_stages: set[str] = field(default_factory=set)
    verbose: bool = False
    codegen_mode: str | None = None
    native_config: str = "check"

    @property
    def contract_path(self) -> Path:
        # capability-family-contract.json is the new standard; fall back to
        # contract.json for families not yet migrated.
        cap = self.family_dir / "capability-family-contract.json"
        if cap.exists():
            return cap
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
        return self.family_dir.parents[1] / "results" / "foundation-dll" / self.assembly / self.slug


@dataclass
class UnifiedReport:
    """Top-level output of a family verification run.

    Fields:
        family: Family slug (matches FamilyContext.slug).
        assembly: Assembly name.
        mode: Verification mode used ("standard" or "strict").
        timestamp: ISO-8601 timestamp of when the report was generated.
        duration_ms: Total pipeline wall-clock time in milliseconds.
        overall_status: One of "pending" (not yet completed), "passed" (all
                        required stages passed), "failed" (required stage
                        failed or errored), or "skipped" (no stages ran).
        stages: Dict of stage_key -> StageResult.to_dict() for every stage
                that was executed or skipped.
        coverage: Aggregated coverage metrics — stagePassRate (float),
                  stagesPassed/failed/skipped/total (int).
        dashboard: Benchmark key ratios (nativeFasterRatio, managedFasterRatio,
                   irExpansionRatio, asmPassRate, averageSpeedupPercent).
        regression: Regression detection results, containing hasRegression (bool),
                    regressions (list), and benchmark (dict with status/metrics).
    """
    family: str
    assembly: str
    mode: str = "standard"
    timestamp: str = ""
    duration_ms: int = 0
    overall_status: str = "pending"
    stages: dict[str, dict[str, Any]] = field(default_factory=dict)
    coverage: dict[str, float] = field(default_factory=dict)
    dashboard: dict[str, Any] = field(default_factory=dict)
    regression: dict[str, Any] = field(default_factory=dict)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    def to_json(self) -> str:
        return json.dumps(self.to_dict(), indent=2, ensure_ascii=False)
