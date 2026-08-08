"""Data models for unified pipeline reporting and AI-ready exports.

Replaces the legacy UnifiedReport (context.py) with PipelineRunReport,
which includes run identity, platform metadata, and structured per-chunk,
per-method results designed for both human dashboards and AI agent consumption.
"""

from __future__ import annotations

import json
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Any


# ── Error pattern classification ──

# Mapping from error pattern → target expert agent.
# Used by fact_chunk._classify_failure() and ai/fact-failures.jsonl exports.
ERROR_PATTERN_ROUTES: dict[str, str] = {
    "lnk2019":         "dev-il2cpp-codegen-expert",
    "c3861":           "dev-il2cpp-codegen-expert",
    "c2556":           "dev-il2cpp-abi-expert",
    "value_suspicious": "dev-il2cpp-debug-expert",
    "shutdown_av":     "dev-il2cpp-runtime-expert",
    "assert_fail":     "dev-il2cpp-codegen-expert",
    "metadata_mismatch": "dev-il2cpp-codegen-expert",
    "timeout":         "dev-il2cpp-verification-pipeline",
    "no_entry_exe":    "dev-il2cpp-build-fixer",
}

# Bottleneck categories for benchmark/profile data.
BOTTLENECK_ROUTES: dict[str, str] = {
    "gc_pause":          "dev-il2cpp-gc-expert",
    "dispatch_overhead": "dev-il2cpp-runtime-expert",
    "translation_path":  "dev-il2cpp-translation-expert",
    "alloc_hot":         "dev-il2cpp-codegen-expert",
    "interpreter_overhead": "dev-il2cpp-runtime-expert",
    "keep_native_blocked":  "dev-il2cpp-codegen-expert",
    "patch_apply_fail":     "dev-il2cpp-build-fixer",
    "tier_deopt_loop":      "dev-il2cpp-runtime-expert",
}


def classify_failure(message: str, value: int | float | None) -> str:
    """Classify a fact failure into an error pattern.

    Args:
        message: Assertion failure message from entry.exe.
        value: Return value of the method (-1 often means not implemented).

    Returns:
        error pattern string, e.g. "value_suspicious", "assert_fail".
    """
    if value is not None:
        try:
            v = int(value)
            if v == -1 and message:
                return "value_suspicious"
        except (ValueError, TypeError):
            pass
    if message:
        msg_lower = message.lower()
        if "access violation" in msg_lower or "segfault" in msg_lower:
            return "shutdown_av"
        if "assert" in msg_lower or "expected" in msg_lower:
            return "assert_fail"
        if "lnk2019" in msg_lower or "unresolved external" in msg_lower:
            return "lnk2019"
        if "c3861" in msg_lower or "identifier not found" in msg_lower:
            return "c3861"
    return "unknown"


def route_for_error(error_pattern: str) -> str:
    """Return the target expert agent for a given error pattern."""
    return ERROR_PATTERN_ROUTES.get(error_pattern, "dev-il2cpp-debug-expert")


def route_for_bottleneck(bottleneck: str) -> str:
    """Return the target expert agent for a given bottleneck category."""
    return BOTTLENECK_ROUTES.get(bottleneck, "dev-il2cpp-runtime-expert")


# ── Core data models ──


@dataclass
class FactFailure:
    """A single fact failure with classification and routing hint."""
    methodSubjectId: str = ""
    methodIndex: int = 0
    value: int | float | None = None
    message: str = ""
    errorPattern: str = ""
    routeHint: str = ""
    knownIssueId: str | None = None


@dataclass
class FactSummary:
    """Per-chunk fact result summary."""
    totalMethods: int = 0
    passed: int = 0
    valueSuspiciousCount: int = 0
    shutdownAVCount: int = 0
    metadataMismatch: bool = False
    passRate: float = 0.0
    failures: list[FactFailure] = field(default_factory=list)


@dataclass
class BenchmarkEntry:
    """Per-method benchmark comparison data."""
    methodSubjectId: str = ""
    chaosAotMs: float | None = None
    chaosJitMs: float | None = None
    net8Ms: float | None = None
    net10Ms: float | None = None
    chaosAotVsNet8Pct: float | None = None
    previousRunPct: float | None = None
    regressionDelta: float | None = None
    highVariance: bool = False
    bottleneck: str = ""
    routeHint: str = ""
    profileData: dict[str, Any] = field(default_factory=dict)


@dataclass
class HotupdateEntry:
    """Per-method hotupdate result data."""
    methodSubjectId: str = ""
    postPatchNsPerOp: float = 0.0
    keepNative: bool = False
    patchApplied: bool = False
    semanticChanged: bool = False
    revertPassed: bool = False
    interpreterDispatchPct: float = 0.0
    bottleneck: str = ""
    routeHint: str = ""


@dataclass
class ChunkReport:
    """Aggregated results for a single chunk."""
    slug: str = ""
    assembly: str = ""
    fact: FactSummary | None = None
    benchmark: list[BenchmarkEntry] = field(default_factory=list)
    hotupdate: list[HotupdateEntry] = field(default_factory=list)
    duration_ms: int = 0


@dataclass
class PipelineRunReport:
    """Top-level output for a single pipeline run.

    Replaces the legacy UnifiedReport (context.py). Serialized to
    results/pipeline-runs.jsonl (one line per run).
    """
    runId: str = ""
    timestamp: str = ""
    platform: str = ""
    device: dict[str, Any] = field(default_factory=dict)
    gitCommit: str = ""
    gitBranch: str = ""
    nativeConfig: str = ""
    mode: str = ""
    totalDurationMs: int = 0
    overviewStatus: str = "pending"
    chunks: list[ChunkReport] = field(default_factory=list)

    def to_dict(self) -> dict[str, Any]:
        return asdict(self)

    def to_json(self) -> str:
        return json.dumps(self.to_dict(), indent=2, ensure_ascii=False)

    def to_jsonl_line(self) -> str:
        return json.dumps(self.to_dict(), ensure_ascii=False)

    @staticmethod
    def load_from_file(path: Path) -> PipelineRunReport | None:
        """Load a PipelineRunReport, reconstructing nested dataclasses.

        ``PipelineRunReport(**data)`` only rebuilds the outer dataclass; the nested
        ChunkReport/FactSummary/BenchmarkEntry/etc. came back as plain dicts, so
        attribute access like ``chunk.fact.passRate`` raised AttributeError. This
        rebuilds the full object tree.
        """
        try:
            data = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            return None

        chunks: list[ChunkReport] = []
        for cd in data.get("chunks") or []:
            fact_raw = cd.get("fact")
            fact: FactSummary | None = None
            if isinstance(fact_raw, dict):
                fact = FactSummary(
                    **{k: v for k, v in fact_raw.items() if k != "failures"},
                )
                fact.failures = [
                    FactFailure(**f) for f in (fact_raw.get("failures") or [])
                    if isinstance(f, dict)
                ]
            chunks.append(ChunkReport(
                slug=cd.get("slug", ""),
                assembly=cd.get("assembly", ""),
                fact=fact,
                benchmark=[BenchmarkEntry(**b) for b in (cd.get("benchmark") or [])
                           if isinstance(b, dict)],
                hotupdate=[HotupdateEntry(**h) for h in (cd.get("hotupdate") or [])
                           if isinstance(h, dict)],
                duration_ms=cd.get("duration_ms", 0),
            ))

        try:
            return PipelineRunReport(**{**data, "chunks": chunks})
        except TypeError:
            return None
