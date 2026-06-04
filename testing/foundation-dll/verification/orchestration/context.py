"""Core data models for the foundation DLL chunk verification pipeline."""

from __future__ import annotations

import json
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any


@dataclass
class StageResult:
    """Result of a single verification stage.

    Fields:
        stage: Stage key (e.g. "build", "fact", "coverage-audit").
        status: One of "passed", "failed", "skipped", or "error".
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
class ChunkContext:
    """Context for a single chunk verification run.

    A "chunk" is a group of methods from the same DLL, collected by namespace
    partitioning (see namespace-partition.json).  Each chunk lives under
    <assembly_dir>/chunks/<slug>/ and contains managed subjects, native entry.exe,
    and results/.

    Fields:
        slug: Chunk identifier, typically kebab-case (e.g. "system", "collections-generic").
        assembly: Assembly name (e.g. "System.Private.CoreLib").
        chunk_dir: Root directory for this chunk's files (managed/, native/, results/).
        foundation_dir: Root directory for the assembly (parent of _dll/, chunks/).
        mode: Verification mode — "standard" (default) or "strict".
        skip_stages: Set of stage keys to skip during pipeline execution.
        verbose: Enable verbose logging output.
        native_config: Native build config — "check", "profile", or "ship".
        stage_timeout_seconds: Per-stage max wall-clock time (0 = no timeout).
        resume: Skip already-passed stages from previous run.
    """
    slug: str
    assembly: str
    chunk_dir: Path
    foundation_dir: Path
    mode: str = "standard"
    skip_stages: set[str] = field(default_factory=set)
    verbose: bool = False
    native_config: str = "check"
    stage_timeout_seconds: int = 0
    resume: bool = False
    skip_probe: bool = False

    @property
    def managed_dir(self) -> Path:
        return self.chunk_dir / "managed" / "subjects"

    @property
    def native_dir(self) -> Path:
        return self.chunk_dir / "native"

    @property
    def results_dir(self) -> Path:
        return self.chunk_dir / "results"

    @property
    def subjects_dll_path(self) -> Path:
        return self.managed_dir / "CombinedSubjects.dll"

    @property
    def subjects_metadata_path(self) -> Path:
        return self.managed_dir / "subjects.metadata.json"

    @property
    def entry_exe_path(self) -> Path:
        return self.native_dir / "entry.exe"

    @property
    def entry_jit_exe_path(self) -> Path:
        return self.native_dir / "entry-jit.exe"

    @property
    def patch_data_path(self) -> Path:
        return self.native_dir / "patch.patchdata"

    @property
    def patch_subjects_dll_path(self) -> Path:
        return self.chunk_dir / "managed" / "subjects" / "PatchSubjects.dll"

    @property
    def dll_manifest_dir(self) -> Path:
        return self.foundation_dir / "_dll"

    @property
    def namespace_partition_path(self) -> Path:
        return self.dll_manifest_dir / "namespace-partition.json"


@dataclass
class UnifiedReport:
    """Top-level output of a chunk verification run.

    Fields:
        chunk: Chunk slug (matches ChunkContext.slug).
        assembly: Assembly name.
        mode: Verification mode used ("standard" or "strict").
        timestamp: ISO-8601 timestamp of when the report was generated.
        duration_ms: Total pipeline wall-clock time in milliseconds.
        overall_status: One of "pending" (not yet completed), "passed" (all
                        required stages passed), "failed" (required stage
                        failed or errored), or "skipped" (no stages ran).
        stages: Dict of stage_key -> StageResult.to_dict() for every stage
                that was executed or skipped.
        coverage: Aggregated coverage metrics.
        dashboard: Benchmark key ratios.
        regression: Regression detection results.
    """
    chunk: str
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

    @staticmethod
    def load_from_file(path: Path) -> UnifiedReport | None:
        """Load a unified report from a JSON file.  Returns None on failure."""
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
            stages_raw = data.pop("stages", {})
            report = UnifiedReport(**data)
            report.stages = stages_raw
            return report
        except (OSError, json.JSONDecodeError, TypeError):
            return None
