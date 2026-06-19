"""Nightly runner — config model.

Central configuration for the nightly build pipeline, designed to be
serializable to JSON for metadata capture in the report directory.
"""
from __future__ import annotations

import os
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

# Default repo root: walk up from this file's location until we find
# the repo root (signalled by .git/ or CLAUDE.md).
_DEFAULT_REPO = Path(__file__).resolve().parent.parent.parent.parent  # .../


def _detect_repo_root() -> Path:
    """Detect repo root by walking up from the package location."""
    candidate = _DEFAULT_REPO
    for _ in range(10):
        if (candidate / ".git").exists() or (candidate / "CLAUDE.md").exists():
            return candidate
        candidate = candidate.parent
    return _DEFAULT_REPO


def _detect_bench_workers() -> int:
    """Auto-detect benchmark concurrency based on available CPU cores.

    Formula: max(1, min(4, (cpu_count - 2) // 2))
    """
    try:
        cpus = os.cpu_count() or 4
        return max(1, min(4, (cpus - 2) // 2))
    except Exception:
        return 1


_DEFAULT_STAGES = [
    "build", "fact", "benchmark", "managed_benchmark",
    "hotupdate", "coverage_audit", "aggregate", "reporting",
]


@dataclass
class NightlyConfig:
    """Immutable configuration for a nightly build run.

    All paths are resolved at construction time to absolute paths.
    """

    # ── Paths ──────────────────────────────────────────────────────────
    report_dir: Path = field(default_factory=lambda: Path("nightly-build-report").resolve())
    repo_root: Path = field(default_factory=_detect_repo_root)
    foundation_dir: Path | None = None

    # ── Execution ──────────────────────────────────────────────────────
    mode: str = "full"  # "full" | "smoke" | "extended"
    assemblies: list[str] | None = None  # None = auto-discover all
    stages: list[str] = field(default_factory=lambda: list(_DEFAULT_STAGES))
    max_workers: int = 4
    bench_workers: int | None = None
    native_config: str = "check"
    run_profile: bool = False
    stage_timeout: int = 0  # 0 = no timeout

    # ── Logging ────────────────────────────────────────────────────────
    capture_logs: bool = True
    log_level: str = "INFO"  # "INFO" | "DEBUG"
    keep_console_output: bool = True

    # ── Reporting ──────────────────────────────────────────────────────
    skip_nightly_report: bool = False
    keep_per_chunk_results: bool = True
    compress_history: bool = False

    # ── Jenkins metadata (injected from env vars) ──────────────────────
    jenkins_job: str = ""
    jenkins_build_id: str = ""
    jenkins_build_url: str = ""
    jenkins_trigger: str = "manual"

    def __post_init__(self):
        """Resolve relative paths to absolute. Convert str to Path if needed."""
        self.report_dir = Path(self.report_dir).resolve()
        self.repo_root = Path(self.repo_root).resolve()
        if self.foundation_dir is None:
            self.foundation_dir = self.repo_root / "testing" / "foundation-dll"
        self.foundation_dir = Path(self.foundation_dir).resolve()
        if self.bench_workers is None:
            self.bench_workers = _detect_bench_workers()

    def to_dict(self) -> dict[str, Any]:
        """Serializable dict for metadata capture."""
        return {
            "reportDir": str(self.report_dir),
            "repoRoot": str(self.repo_root),
            "foundationDir": str(self.foundation_dir),
            "mode": self.mode,
            "assemblies": self.assemblies,
            "stages": list(self.stages),
            "maxWorkers": self.max_workers,
            "benchWorkers": self.bench_workers,
            "nativeConfig": self.native_config,
            "runProfile": self.run_profile,
            "stageTimeout": self.stage_timeout,
            "captureLogs": self.capture_logs,
            "logLevel": self.log_level,
            "keepConsoleOutput": self.keep_console_output,
            "skipNightlyReport": self.skip_nightly_report,
            "keepPerChunkResults": self.keep_per_chunk_results,
            "compressHistory": self.compress_history,
            "jenkinsJob": self.jenkins_job,
            "jenkinsBuildId": self.jenkins_build_id,
            "jenkinsBuildUrl": self.jenkins_build_url,
            "jenkinsTrigger": self.jenkins_trigger,
        }

    @staticmethod
    def from_env() -> dict[str, str]:
        """Extract Jenkins metadata from environment variables.

        Returns a dict suitable for passing as **kwargs to NightlyConfig().
        """
        return {
            "jenkins_job": os.environ.get("JOB_NAME", ""),
            "jenkins_build_id": os.environ.get("BUILD_ID", ""),
            "jenkins_build_url": os.environ.get("BUILD_URL", ""),
            "jenkins_trigger": os.environ.get("JENKINS_TRIGGER", "manual"),
        }
