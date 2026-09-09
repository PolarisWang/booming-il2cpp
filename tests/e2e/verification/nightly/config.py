"""NightlyConfig — single dataclass for all nightly build parameters.

Risk mitigations baked in:
  - P1-4: retry_on_error dict (retry policy per error_class, not blind retry)
  - P2-1: oom_auto_downgrade (exit 137 detection → max_workers //= 2)
  - P1-1: provenance guard (git_commit check, fallback to full rebuild)
"""

from __future__ import annotations

import os
import json
import subprocess
import time
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


def _detect_repo_root() -> Path:
    """Walk up from this file to find the repo root (.git marker)."""
    p = Path(__file__).resolve()
    for _ in range(10):
        if (p / ".git").is_dir():
            return p
        p = p.parent
    return Path.cwd()


def _detect_foundation_dir() -> Path:
    """CHAOS_FOUNDATION_DLL env override, else auto-detect from repo root."""
    override = os.environ.get("CHAOS_FOUNDATION_DLL")
    if override:
        return Path(override)
    repo = _detect_repo_root()
    # Typical layout: tests/e2e/translation  (foundation)
    candidate = repo / "tests" / "e2e" / "translation"
    if candidate.is_dir():
        return candidate
    # Fallback: testing/foundation-dll (legacy)
    return repo / "testing" / "foundation-dll"


def _detect_bench_workers() -> int:
    """Auto-detect bench workers: one per physical core, capped at 4."""
    try:
        import os
        count = os.cpu_count() or 4
        return min(count, 4)
    except Exception:
        return 2


def _generate_run_id() -> str:
    """YYYYMMDD_HHMMSS-{short_git_hash}"""
    ts = datetime.now(timezone.utc).strftime("%Y%m%d_%H%M%S")
    try:
        short = subprocess.run(
            ["git", "rev-parse", "--short", "HEAD"],
            capture_output=True, text=True, timeout=5,
        ).stdout.strip()
    except Exception:
        short = "unknown"
    return f"{ts}-{short}"


@dataclass
class NightlyConfig:
    # ── Paths ──
    foundation_dir: Path = field(default_factory=_detect_foundation_dir)
    report_dir: Path = field(default_factory=lambda: Path("nightly-build-report").resolve())
    run_id: str = field(default_factory=_generate_run_id)

    # ── Execution ──
    max_workers: int = 4
    native_config: str = "check"
    stages: list[str] = field(default_factory=lambda: [
        "build", "fact", "benchmark", "managed_benchmark",
        "hotupdate", "coverage-audit", "aggregate",
    ])
    max_idle_seconds: int = 1200  # watchdog threshold
    # ⚡ D.1-fix V1: per-stage idle budget (build/fact can be silent for long,
    #    benchmark/hotupdate output more frequently).  If None, all stages
    #    use max_idle_seconds.  Overridable via env CHAOS_IDLE_BY_STAGE as a
    #    JSON dict like '{"benchmark":900,"build":3600}'.
    idle_by_stage: dict[str, int] | None = None
    verbose: bool = False
    profile_pass: bool = False

    # ⚡ P1-4: retry policy per error_class (not blind retry)
    retry_on_error: dict[str, bool] = field(default_factory=lambda: {
        "timeout": True,
        "native-crash": True,
        "env-missing": True,
        "atg-combined-cs": False,
        "native-codegen-missing-symbol": False,
        "native-codegen-syntax": False,
        "platform-diagnostic": False,
        "csharp-error": False,
        "hotupdate-patch-arm": False,
        "unknown": True,
    })
    max_retries_per_chunk: int = 1

    # ⚡ P2-1: OOM auto-downgrade
    oom_auto_downgrade: bool = True

    # ── Resume ──
    resume_run_id: str | None = None
    assembly_filter: list[str] | None = None

    # ── Jenkins metadata ──
    jenkins_job: str = ""
    jenkins_build_id: str = ""
    jenkins_build_url: str = ""
    jenkins_trigger: str = "manual"

    def __post_init__(self):
        self.foundation_dir = Path(self.foundation_dir).resolve()
        self.report_dir = Path(self.report_dir).resolve()
        # D.1-fix V1: parse idle_by_stage from env if not set on object.
        if self.idle_by_stage is None:
            raw = os.environ.get("CHAOS_IDLE_BY_STAGE", "")
            if raw:
                try:
                    parsed = json.loads(raw)
                    if isinstance(parsed, dict):
                        self.idle_by_stage = {str(k): int(v) for k, v in parsed.items()}
                except (ValueError, TypeError):
                    print("  [config] WARN: CHAOS_IDLE_BY_STAGE not valid JSON; "
                          "using max_idle_seconds for all stages")

    def idle_for(self, stage: str) -> int:
        """Per-stage idle budget (D.1-fix V1). Falls back to max_idle_seconds."""
        if self.idle_by_stage:
            got = self.idle_by_stage.get(stage)
            if got is not None:
                return got
        return self.max_idle_seconds

    @classmethod
    def from_env(cls) -> dict[str, str]:
        """Read Jenkins CI metadata from env vars."""
        meta = {}
        for env_key, conf_key in [
            ("JOB_NAME", "jenkins_job"),
            ("BUILD_ID", "jenkins_build_id"),
            ("BUILD_URL", "jenkins_build_url"),
            ("JENKINS_TRIGGER", "jenkins_trigger"),
        ]:
            val = os.environ.get(env_key)
            if val:
                meta[conf_key] = val
        return meta

    def to_dict(self) -> dict[str, Any]:
        return {
            "foundationDir": str(self.foundation_dir),
            "reportDir": str(self.report_dir),
            "runId": self.run_id,
            "maxWorkers": self.max_workers,
            "nativeConfig": self.native_config,
            "stages": self.stages,
            "maxIdleSeconds": self.max_idle_seconds,
            "retryOnError": self.retry_on_error,
            "oomAutoDowngrade": self.oom_auto_downgrade,
            "resumeRunId": self.resume_run_id,
            "assemblyFilter": self.assembly_filter,
            "profilePass": self.profile_pass,
            "verbose": self.verbose,
            **{k: getattr(self, k) for k in ["jenkins_job", "jenkins_build_id",
                                              "jenkins_build_url", "jenkins_trigger"]},
        }