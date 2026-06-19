"""Report collector — aggregate all build outputs into a unified directory.

After a nightly build completes, the collector:

  1. Creates a timestamped run directory: {report_dir}/YYYY-MM-DD_HHMMSS-{run_id}/
  2. Copies per-assembly _dll/reports/latest/ → reports/<assembly>/
  3. Copies per-chunk results → per-chunk/<assembly>/<slug>/
  4. Copies benchmark history → benchmark-history/
  5. Runs nightly delta + summary → summary/
  6. Writes git/jenkins/config metadata → metadata/
  7. Creates/updates the `latest` symlink (or copy on Windows)
"""
from __future__ import annotations

import json
import os
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_FOUNDATION_DLL = _HERE.parent
if str(_FOUNDATION_DLL) not in sys.path:
    sys.path.insert(0, str(_FOUNDATION_DLL))

from verification.nightly_runner.config import NightlyConfig
from verification.nightly_runner.orchestrator import NightlyResult, discover_assemblies


class ReportCollector:
    """Aggregate nightly build outputs into a unified report directory."""

    def __init__(self, config: NightlyConfig, result: NightlyResult):
        self.config = config
        self.result = result
        self.report_dir = config.report_dir

        # Compute run timestamp
        now = datetime.now(timezone.utc)
        self.timestamp = now.strftime("%Y%m%d_%H%M%S")
        run_id = self._compute_run_id()
        self.run_dir = self.report_dir / f"{self.timestamp}-{run_id}"

    def collect(self) -> Path:
        """Run all collection phases, returning the report directory path."""
        print(f"\n{'='*60}")
        print(f"  Collecting reports → {self.run_dir}")
        print(f"{'='*60}")

        # 1. Copy per-assembly _dll/reports/
        self._copy_assembly_reports()

        # 2. Copy per-chunk results
        self._copy_chunk_results()

        # 3. Copy benchmark history
        if self.config.keep_per_chunk_results:
            self._copy_benchmark_history()

        # 4. Generate nightly delta + summary
        if not self.config.skip_nightly_report:
            self._generate_nightly_report()

        # 5. Write metadata
        self._write_metadata()

        # 6. Update latest symlink
        self._update_latest()

        print(f"\n  Report ready: {self.run_dir}")
        return self.run_dir

    # ── Run ID ──────────────────────────────────────────────────────────

    def _compute_run_id(self) -> str:
        """Short run identifier: first 8 chars of git commit or 'noid'."""
        try:
            commit = subprocess.run(
                ["git", "rev-parse", "--short=8", "HEAD"],
                capture_output=True, text=True, timeout=5,
                cwd=self.config.repo_root,
            ).stdout.strip()
            return commit or "noid"
        except Exception:
            return "noid"

    # ── Assembly reports ────────────────────────────────────────────────

    def _copy_assembly_reports(self):
        """Copy each assembly's _dll/reports/latest/ → reports/<assembly>/."""
        target = self.run_dir / "reports"
        assemblies = self.config.assemblies or discover_assemblies(self.config.foundation_dir)

        copied = 0
        for asm in assemblies:
            src = self.config.foundation_dir / asm / "_dll" / "reports" / "latest"
            if src.exists():
                dst = target / asm
                dst.mkdir(parents=True, exist_ok=True)
                try:
                    shutil.copytree(
                        str(src), str(dst),
                        dirs_exist_ok=True,
                        ignore=shutil.ignore_patterns("history"),
                    )
                    copied += 1
                except OSError as e:
                    print(f"  [collector] WARNING: failed to copy reports for {asm}: {e}")

        print(f"  [collector] Copied reports for {copied}/{len(assemblies)} assemblies")

    # ── Chunk results ───────────────────────────────────────────────────

    def _copy_chunk_results(self):
        """Copy each chunk's results/ → per-chunk/<assembly>/<slug>/."""
        target = self.run_dir / "per-chunk"
        assemblies = self.config.assemblies or discover_assemblies(self.config.foundation_dir)

        copied = 0
        for asm in assemblies:
            chunks_dir = self.config.foundation_dir / asm / "chunks"
            if not chunks_dir.exists():
                continue
            for slug_dir in chunks_dir.iterdir():
                if not slug_dir.is_dir():
                    continue
                src = slug_dir / "results"
                if src.exists():
                    dst = target / asm / slug_dir.name
                    dst.mkdir(parents=True, exist_ok=True)
                    try:
                        shutil.copytree(str(src), str(dst), dirs_exist_ok=True)
                        copied += 1
                    except OSError:
                        pass

        print(f"  [collector] Copied results for {copied} chunks")

    # ── Benchmark history ───────────────────────────────────────────────

    def _copy_benchmark_history(self):
        """Copy benchmark-history.jsonl → benchmark-history/<asm>/<slug>.jsonl."""
        target = self.run_dir / "benchmark-history"
        results_base = Path(__file__).resolve().parent.parent / "results" / "foundation-dll"

        copied = 0
        if results_base.exists():
            assemblies = self.config.assemblies or discover_assemblies(self.config.foundation_dir)
            for asm in assemblies:
                asm_dir = results_base / asm
                if not asm_dir.exists():
                    continue
                for slug_dir in asm_dir.iterdir():
                    if not slug_dir.is_dir():
                        continue
                    src = slug_dir / "perf" / "benchmark-history.jsonl"
                    if src.exists():
                        dst_dir = target / asm
                        dst_dir.mkdir(parents=True, exist_ok=True)
                        try:
                            shutil.copy2(str(src), str(dst_dir / f"{slug_dir.name}.jsonl"))
                            copied += 1
                        except OSError:
                            pass

        print(f"  [collector] Copied {copied} benchmark history files")

    # ── Nightly report (delta + summary) ────────────────────────────────

    def _generate_nightly_report(self):
        """Generate nightly-delta.json and nightly-summary.md."""
        summary_dir = self.run_dir / "summary"
        summary_dir.mkdir(parents=True, exist_ok=True)

        try:
            # Compute delta
            from verification.nightly_delta import compute_nightly_delta

            today = datetime.now(timezone.utc).strftime("%Y-%m-%d")
            delta = compute_nightly_delta(self.config.foundation_dir, today=today)

            delta_path = summary_dir / "nightly-delta.json"
            delta_path.write_text(json.dumps(delta, indent=2, ensure_ascii=False), encoding="utf-8")

            # Generate summary
            from verification.nightly_summary import generate_summary

            summary = generate_summary(delta)
            summary_path = summary_dir / "nightly-summary.md"
            summary_path.write_text(summary, encoding="utf-8")

            # Pipeline status
            status = self._build_pipeline_status(delta)
            status_path = summary_dir / "pipeline-status.json"
            status_path.write_text(json.dumps(status, indent=2), encoding="utf-8")

            print(f"  [collector] Nightly report generated: "
                  f"{delta['overall']['totalAssemblies']} assemblies, "
                  f"{len(delta['regressions'])} regressions, "
                  f"{len(delta['improvements'])} improvements")

        except Exception as e:
            print(f"  [collector] WARNING: nightly report generation failed: {e}")

    def _build_pipeline_status(self, delta: dict) -> dict[str, Any]:
        """Build a pipeline status summary from delta + runtime results."""
        overall = delta.get("overall", {})

        chunk_statuses: dict[str, str] = {}
        for key, cd in delta.get("chunks", {}).items():
            chunk_statuses[key] = cd.get("buildStatus", "unknown")

        return {
            "runId": self._compute_run_id(),
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "totalAssemblies": overall.get("totalAssemblies", 0),
            "totalChunks": overall.get("totalChunks", 0),
            "chunksVerified": overall.get("chunksVerified", 0),
            "buildPassed": self.result.build_passed,
            "buildFailed": self.result.build_failed,
            "regressions": len(delta.get("regressions", [])),
            "improvements": len(delta.get("improvements", [])),
            "chunkStatuses": chunk_statuses,
        }

    # ── Metadata ────────────────────────────────────────────────────────

    def _write_metadata(self):
        """Write git, jenkins, and config metadata files."""
        meta_dir = self.run_dir / "metadata"
        meta_dir.mkdir(parents=True, exist_ok=True)

        # Git info
        git_info = self._get_git_info()
        (meta_dir / "git-info.json").write_text(
            json.dumps(git_info, indent=2, ensure_ascii=False), encoding="utf-8")

        # Jenkins info
        jenkins_info = {
            "job": self.config.jenkins_job,
            "buildId": self.config.jenkins_build_id,
            "buildUrl": self.config.jenkins_build_url,
            "trigger": self.config.jenkins_trigger,
        }
        (meta_dir / "jenkins-info.json").write_text(
            json.dumps(jenkins_info, indent=2, ensure_ascii=False), encoding="utf-8")

        # Pipeline config
        (meta_dir / "config.json").write_text(
            json.dumps(self.config.to_dict(), indent=2, ensure_ascii=False), encoding="utf-8")

        # Duration info
        duration = {
            "startedAt": datetime.fromtimestamp(self.result.started_at, tz=timezone.utc).isoformat(),
            "endedAt": datetime.fromtimestamp(self.result.ended_at, tz=timezone.utc).isoformat(),
            "totalSeconds": round(self.result.ended_at - self.result.started_at, 1),
            "chunkCount": self.result.chunk_count,
        }
        (meta_dir / "duration.json").write_text(
            json.dumps(duration, indent=2, ensure_ascii=False), encoding="utf-8")

        print(f"  [collector] Metadata written to {meta_dir}")

    def _get_git_info(self) -> dict[str, str]:
        """Get git commit information."""
        cwd = self.config.repo_root
        try:
            return {
                "commit": subprocess.run(
                    ["git", "rev-parse", "HEAD"], capture_output=True, text=True, timeout=5,
                    cwd=cwd).stdout.strip(),
                "branch": subprocess.run(
                    ["git", "rev-parse", "--abbrev-ref", "HEAD"], capture_output=True,
                    text=True, timeout=5, cwd=cwd).stdout.strip(),
                "message": subprocess.run(
                    ["git", "log", "-1", "--format=%s"], capture_output=True,
                    text=True, timeout=5, cwd=cwd).stdout.strip(),
                "date": subprocess.run(
                    ["git", "log", "-1", "--format=%ai"], capture_output=True,
                    text=True, timeout=5, cwd=cwd).stdout.strip(),
            }
        except Exception:
            return {"commit": "unknown", "branch": "unknown",
                    "message": "", "date": ""}

    # ── Latest symlink ──────────────────────────────────────────────────

    def _update_latest(self):
        """Create or update the `latest` symlink (copy on Windows)."""
        latest_path = self.report_dir / "latest"

        # Remove existing
        if latest_path.exists() or latest_path.is_symlink():
            try:
                latest_path.unlink()
            except OSError:
                try:
                    shutil.rmtree(str(latest_path), ignore_errors=True)
                except OSError:
                    pass

        # Create symlink (Unix) or directory copy (Windows fallback)
        if os.name != "nt":
            try:
                # Relative symlink for readability
                rel_path = os.path.relpath(str(self.run_dir), str(self.report_dir))
                latest_path.symlink_to(rel_path, target_is_directory=True)
                print(f"  [collector] latest → {rel_path}")
                return
            except OSError:
                pass

        # Windows fallback: write a marker file pointing to the latest run
        try:
            (self.report_dir / "latest.txt").write_text(
                str(self.run_dir.name), encoding="utf-8")
            print(f"  [collector] latest.txt → {self.run_dir.name}")
        except OSError:
            print(f"  [collector] WARNING: could not create latest pointer")
