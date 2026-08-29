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
    value_suspicious: bool = False

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)


@dataclass
class ChunkContext:
    """Context for a single chunk verification run.

    A "chunk" is a group of methods from the same DLL, collected by namespace
    partitioning (see namespace-partition.json).  Each chunk has two physical
    roots, split by lifecycle:

      * `chunk_dir` is the DISCARDABLE build-output root (native/, results/,
        managed/.autogen/, managed/combined/) — it lives under the gitignored
        `artifacts/foundation-dll/<assembly>/chunks/<slug>/` tree and is the
        ~80G that may be wiped freely.
      * `source_chunk_dir` is the VERSION-CONTROLLED source root (managed/
        subjects/, chunk.json) — it stays under the foundation tree.

    Pieces live on one side or the other:

      * product outputs (entry.exe, fact.json, CombinedSubjects.dll, .autogen/,
        combined/, PatchSubjects.dll)        -> chunk_dir
      * subject sources + controlled metadata (subjects.metadata.json,
        supplemental-coverage.json, .cs/.csproj, chunk.json) -> source_chunk_dir

    Separate concrete constructors produce a legacy (single-root) context where
    `build_dir is None` (all properties fall back to chunk_dir) so callers that
    predate the split keep working unchanged.

    Fields:
        slug: Chunk identifier, typically kebab-case (e.g. "system", "collections-generic").
        assembly: Assembly name (e.g. "System.Private.CoreLib").
        chunk_dir: Build-output root for this chunk (managed/, native/, results/).
        foundation_dir: Root directory for the assembly (parent of _dll/, chunks/).
        build_dir: Discardable build root; None => chunk_dir (legacy single-root).
        mode: Verification mode — "standard" (default) or "strict".
        skip_stages: Set of stage keys to skip during pipeline execution.
        verbose: Enable verbose logging output.
        native_config: Native build config — "check", "profile", or "ship".
        stage_timeout_seconds: Per-stage max wall-clock time (0 = no timeout).
        resume: Skip already-passed stages from previous run.
        assembly_dirs: List of assembly search directories for codegen --assembly-dir.
                       Populated from pipeline-config.yaml by the pipeline orchestrator.
    """
    slug: str
    assembly: str
    chunk_dir: Path
    foundation_dir: Path
    build_dir: Path = None  # build-output root; when set, chunk_dir IS the build side (variant A)
    mode: str = "standard"
    skip_stages: set[str] = field(default_factory=set)
    verbose: bool = False
    native_config: str = "check"
    stage_timeout_seconds: int = 0
    resume: bool = False
    skip_probe: bool = False
    assembly_dirs: list[str] = field(default_factory=list)

    # ── Run identity / provenance (populated by chunk_pipeline.py at start) ──
    run_id: str = ""                  # "fdn-{YYYYMMDD}-{seq:03d}"
    platform: str = ""                # "windows-x64" | "linux-x64" | "arm64"
    device: dict[str, Any] = field(default_factory=dict)  # {id, name, cpu, ram_gb}
    git_commit: str = ""
    git_branch: str = ""
    context_fp: str = ""  # fast context fingerprint for soft cache invalidation

    # ── Roots (single source of truth) ──
    @property
    def build_root(self) -> Path:
        """Build-output root for this chunk = chunk_dir (variant A). Legacy
        single-root callers pass build_dir=None, alias to foundation chunk."""
        return self.chunk_dir

    @property
    def source_chunk_dir(self) -> Path:
        """Version-controlled chunk source root (managed/subjects, chunk.json).
        Under legacy single-root (build_dir=None) this equals chunk_dir; under
        the split it is the foundation-side chunk dir. Derived from
        foundation_dir/chunks/slug so source paths never break."""
        src = self.chunk_dir
        # If chunk_dir is the build side (build_dir is not None), the source
        # sibling lives under foundation_dir/chunks/<slug>.
        if self.build_dir is not None:
            src = self.foundation_dir / "chunks" / self.slug
        return src

    @property
    def chunks_dir(self) -> Path:
        """Chunks root on the build side — holds per-chunk discardable outputs
        (results/, native/, .autogen/, combined/). Aggregate/report/benchmark
        stages enumerate per-chunk results from here. Falls back to the source
        chunk parent when no build_dir is set (legacy)."""
        return (self.build_dir or self.chunk_dir).parent

    # ── Build-side (discardable outputs) ──
    @property
    def native_dir(self) -> Path:
        # native/ (entry.exe, entry-jit.exe, patch.patchdata) are build outputs.
        return self.build_root / "native"

    @property
    def results_dir(self) -> Path:
        # results/ (fact.json, benchmarks, reports) are discarded build outputs.
        return self.build_root / "results"

    @property
    def build_managed_dir(self) -> Path:
        """Managed/ subdir on the build side (holds build outputs like .autogen/,
        combined/, CombinedSubjects.dll)."""
        return self.build_root / "managed"

    @property
    def subjects_dll_path(self) -> Path:
        # CombinedSubjects.dll is ATG build output → build-managed side.
        return self.build_managed_dir / "CombinedSubjects.dll"

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
        # PatchSubjects.dll is a patch build output → build-managed side.
        return self.build_managed_dir / "subjects" / "PatchSubjects.dll"

    # ── Source-side (version-controlled) ──
    @property
    def source_managed_dir(self) -> Path:
        """Managed/subjects on the source side — the version-controlled test
        subject sources (.cs, .csproj) + subjects.metadata.json."""
        return self.source_chunk_dir / "managed" / "subjects"

    @property
    def managed_dir(self) -> Path:
        # Backward-compatible alias: subjects/ is the source-side managed dir.
        return self.source_managed_dir

    @property
    def subjects_metadata_path(self) -> Path:
        return self.source_managed_dir / "subjects.metadata.json"

    # ── Assembly-level source metadata (stays source-side) ──
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
