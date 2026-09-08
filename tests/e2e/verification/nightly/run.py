"""run — subprocess-per-chunk execution farm with phase batch orchestration.

Routes each (assembly, chunk) through an isolated `chunk_pipeline` subprocess
(Route 3), never importing the stage functions.  Provides:
  - Phase-batch ordering (a): Phase A = build+fact for all; Phase B =
    benchmark/managed_benchmark/hotupdate/coverage for only Phase-A-passed.
  - `--provided-stages build,fact` on Phase B + provenance guard in chunk_pipeline.
  - error_class-based retry policy (P1-4), watchdog integration, resume.
"""

from __future__ import annotations

import os
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path

from .worklist import WorkItem
from . import state as nstate
from .resume import build_resume_worklist


@dataclass
class ItemRuntime:
    item: WorkItem
    retry_count: int = 0
    status: str = "pending"      # pending|running|passed|failed|retrying|stalled
    error_class: str = "none"
    exit_code: int | None = None
    duration: float = 0.0


@dataclass
class NightlyResult:
    start_wall: float = field(default_factory=time.time)
    end_wall: float = 0.0
    chunk_results: dict[str, dict] = field(default_factory=dict)

    @property
    def passed_count(self) -> int:
        return sum(1 for r in self.chunk_results.values() if r.get("status") == "passed")

    @property
    def failed_count(self) -> int:
        return sum(1 for r in self.chunk_results.values()
                   if r.get("status") in ("failed", "stalled"))

    def summary(self) -> str:
        total = len(self.chunk_results)
        dur = self.end_wall - self.start_wall
        return (f"{self.passed_count}/{total} chunks passed, "
                f"{self.failed_count} failed ({dur:.0f}s)")


def _build_cmd(config, item: WorkItem, stages: list[str],
               provided_stages: str | None) -> list[str]:
    cmd = [
        sys.executable, "-u", "-m", "verification.chunk_pipeline",
        "--assembly", item.assembly,
        "--chunk", item.slug,
        "--stages", ",".join(stages),
        "--native-config", config.native_config,
    ]
    if provided_stages:
        cmd += ["--provided-stages", provided_stages]
    if config.verbose:
        cmd.append("--verbose")
    return cmd


def run_phase(config, phase_label: str, stages: list[str], items: list[ItemRuntime],
              provided_stages: str | None = None, is_last_phase: bool = False):
    """Run one phase across a list of items using a subprocess-per-chunk farm.

    Returns a populated NightlyResult for this phase (status per chunk).
    """
    pending: list[ItemRuntime] = list(items)
    running: dict[str, tuple[ItemRuntime, subprocess.Popen, object]] = {}
    result = NightlyResult()
    oom_downgraded = False

    def _launch(rt: ItemRuntime):
        from .observer import ChunkObserver
        key = rt.item.key
        observer = ChunkObserver(config, key, rt.item)
        cmd = _build_cmd(config, rt.item, stages, provided_stages)
        env = os.environ.copy()
        env["PYTHONUTF8"] = "1"   # ⚡ P1-2: force UTF-8 stdout in child
        # ⚡ Anchor the child's foundation root explicitly so chunk_pipeline
        #    (which resolves foundation_dir via _path's up-walk for a
        #    config/pipeline-config.yaml marker) doesn't pick the wrong sibling
        #    when cwd is the e2e root.  Without this it looked for the assembly
        #    under <foundation>/<asm> where foundation silently defaulted to the
        #    wrong dir → "Assembly directory not found".
        env["CHAOS_FOUNDATION_DLL"] = str(config.foundation_dir)
        # cwd must be the parent of verification/ so `-m verification.chunk_pipeline`
        # resolves.  The foundation dir is typically `<repo>/tests/e2e/translation`;
        # its parent `<repo>/tests/e2e/` is the verification root.
        _cwd = str(config.foundation_dir.parent)
        if not (Path(_cwd) / "verification").is_dir():
            _cwd = str(config.foundation_dir)  # fallback
        # ⚡ Do NOT pass encoding here: the observer reads raw bytes and decodes
        # with utf-8+replace.  Passing text mode (encoding="utf-8") would make
        # stream.readline() return str, and the observer's decode() would fail.
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=_cwd,
            env=env,
        )
        observer.attach(proc)
        running[key] = (rt, proc, observer)
        nstate.touch_heartbeat(config, key, pid=proc.pid)
        nstate.write_result(config, key, phase=phase_label,
                            status="running", stage=",".join(stages))
        if config.verbose:
            print(f"  [{phase_label}] start {key} (pid={proc.pid})", flush=True)

    # Enqueue pending items
    while pending or running:
        # Launch up to max_workers
        while len(running) < config.max_workers and pending:
            rt = pending.pop(0)
            _launch(rt)

        # Poll running
        finished_keys = []
        for key, (rt, proc, obs) in running.items():
            ret = proc.poll()
            if ret is not None:
                finished_keys.append(key)

        for key in finished_keys:
            rt, proc, obs = running.pop(key)
            ret = proc.poll()
            tail = obs.full_log_text() if hasattr(obs, "full_log_text") else (
                obs.tail_text() if hasattr(obs, "tail_text") else "")
            obs.close() if hasattr(obs, "close") else None
            error_class = nstate.classify_exit(ret, tail)
            rt.exit_code = ret
            rt.error_class = error_class
            dur = rt.duration if rt.duration > 0 else 0.0

            if error_class == "pass":
                rt.status = "passed"
                nstate.write_result(config, rt.item.key, phase=phase_label,
                                    status="passed", error_class="none", exit_code=0)
                print(f"  [{phase_label}] PASS  {rt.item.key} ({dur:.0f}s)", flush=True)
            else:
                # ⚡ P1-4: retry policy per error_class
                should_retry = config.retry_on_error.get(error_class, False)
                if should_retry and rt.retry_count < config.max_retries_per_chunk:
                    rt.retry_count += 1
                    rt.status = "retrying"
                    nstate.write_result(config, rt.item.key, phase=phase_label,
                                        status="retrying", error_class=error_class,
                                        exit_code=ret)
                    print(f"  [{phase_label}] RETRY {rt.item.key} ({error_class}, "
                          f"attempt {rt.retry_count})", flush=True)
                    pending.insert(0, rt)  # requeue at front
                else:
                    rt.status = "failed"
                    nstate.write_result(config, rt.item.key, phase=phase_label,
                                        status="failed", error_class=error_class,
                                        exit_code=ret)
                    print(f"  [{phase_label}] FAIL  {rt.item.key} ({error_class}, "
                          f"exit={ret})", flush=True)

            result.chunk_results[rt.item.key] = {
                "status": rt.status,
                "error_class": rt.error_class if rt.error_class != "none" else "none",
                "exit_code": rt.exit_code,
                "phase": phase_label,
            }

        # ⚡ P2-1: OOM auto-downgrade handled in watchdog import
        from .watchdog import watchdog_scan
        watchdog_scan(config, running)

        # Brief idle to avoid busy spin
        if not running and not pending:
            break
        if running:
            time.sleep(0.2)

    result.end_wall = time.time()
    return result


def run_phases(config, worklist: list[WorkItem]) -> NightlyResult:
    """Top-level entry: run Phase A (build+fact) then Phase B (bench+coverage)."""
    # Partition into items (each carries runtime retry count)
    items: list[ItemRuntime] = [ItemRuntime(item=w) for w in worklist]
    retained_items = []

    # ── Phase A: build + fact — for ALL items ──
    print(f"\n{'='*64}\n  Phase A — build + fact ({len(items)} chunks)\n{'='*64}", flush=True)
    phase_a = run_phase(config, "A", ["build", "fact"], items, provided_stages=None)
    results_a = phase_a.chunk_results
    for rt in items:
        k = rt.item.key
        info = results_a.get(k) or {}
        if info.get("status") == "passed":
            retained_items.append(rt)

    # ── Phase B: benchmark + coverage — only for Phase-A-passed chunks ──
    if not retained_items:
        print("  Phase A produced no passed chunks; skipping Phase B.", flush=True)
        combined = NightlyResult()
        combined.chunk_results.update(results_a)
        combined.end_wall = time.time()
        return combined

    print(f"\n{'='*64}\n  Phase B — benchmark + coverage ({len(retained_items)} chunks)\n{'='*64}", flush=True)
    # ⚡ Legacy: benchmark + managed_benchmark + hotupdate + coverage for passed items.
    phase_b_stages = ["benchmark", "managed_benchmark", "hotupdate", "coverage-audit"]
    phase_b = run_phase(config, "B", phase_b_stages, retained_items,
                        provided_stages="build,fact")

    # Combine
    combined = NightlyResult()
    combined.chunk_results.update(results_a)
    combined.chunk_results.update(phase_b.chunk_results)
    combined.end_wall = time.time()
    return combined


