"""run — subprocess-per-chunk execution farm with phase batch orchestration.

Routes each (assembly, chunk) through an isolated `chunk_pipeline` subprocess
(Route 3), never importing the stage functions.  Provides:
  - Phase-batch ordering (a): Phase A = build+fact for all; Phase B =
    benchmark/managed_benchmark/hotupdate/coverage for only Phase-A-passed.
  - `--provided-stages build,fact` on Phase B + provenance guard in chunk_pipeline.
  - error_class-based retry policy (P1-4), watchdog integration, resume.
  - D.1-fix: per-chunk Reaper (kill hung subprocess tree by per-stage idle
    budget) so a single CoreLib chunk stalling 15-20 min no longer deadlocks
    the whole farm (V1-V5 self-reviewed).
"""

from __future__ import annotations

import os
import platform
import signal
import subprocess
import sys
import threading
import time
from dataclasses import dataclass, field
from pathlib import Path

from .worklist import WorkItem
from . import state as nstate
from .resume import build_resume_worklist


# ⚡ D.1-fix V4: chunks the Reaper has already killed.  run_phase's main
# finished-loop must not overwrite the Reaper's `stalled` write for these.
killed_keys: set[str] = set()


def _proc_alive(pid: int) -> bool:
    """Cheap liveness probe.

    POSIX uses kill -0 (signal 0, no signal sent).  Windows cannot rely on
    os.kill(pid, 0) because it returns True even for dead PIDs (PID reuse).
    On Windows use ctypes WaitForSingleObject with 0 timeout: the handle is
    signaled (WAIT_OBJECT_0) when the process has exited.
    """
    if not pid:
        return False
    if platform.system() == "Windows":
        try:
            import ctypes
            # PROCESS_QUERY_INFORMATION | SYNCHRONIZE = 0x400 | 0x00100000
            handle = ctypes.windll.kernel32.OpenProcess(0x400 | 0x00100000, 0, pid)
            if not handle:
                return False  # can't open -> not alive
            try:
                # WAIT_OBJECT_0 (0) = signaled (exited); WAIT_TIMEOUT (258) = still running
                result = ctypes.windll.kernel32.WaitForSingleObject(handle, 0)
                return result != 0
            finally:
                ctypes.windll.kernel32.CloseHandle(handle)
        except Exception:
            return True  # fallback: assume alive on error
    # POSIX
    try:
        os.kill(pid, 0)
        return True
    except (ProcessLookupError, PermissionError):
        return False
    except OSError:
        return False


def _kill_tree(pid: int) -> None:
    """Kill a process and its whole subtree, cross-platform.

    Windows uses taskkill /F /T (recursive tree).  POSIX kills the process
    group (Popen must be launched in its own session for a group to exist;
    run.py launches with default, but os.killpg on a pgid = pid works if the
    child was start_new_session — we use start_new_session=True in _launch so
    the child owns a fresh pgid equal to its pid).
    """
    if not pid:
        return
    if platform.system() == "Windows":
        subprocess.run(
            ["taskkill", "/F", "/T", "/PID", str(pid)],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            timeout=30,
        )
    else:
        try:
            os.killpg(pid, signal.SIGKILL)  # child launched start_new_session → pgid==pid
        except ProcessLookupError:
            pass
        except PermissionError:
            try:
                os.kill(pid, signal.SIGKILL)
            except Exception:
                pass


class Reaper:
    """A single per-chunk watchdog that kills a hung subprocess tree.

    Runs as a daemon thread.  Every `check_interval` seconds it stats the
    chunk's heartbeat file mtime; if the mtime is older than the per-stage
    idle budget (V1) AND the process is still alive, it kills the whole tree
    (V4 V5) and writes a 'stalled' result.

    The reaper self-exits when:
      - `signal_stop()` is called (chunk finished normally), or
      - it kills the chunk (timeout path).
    """

    def __init__(self, config, key: str, pid: int, phase: str,
                 idle_seconds: int, check_interval: int = 10):
        self._config = config
        self._key = key
        self._pid = pid
        self._phase = phase
        self._idle = idle_seconds
        self._interval = check_interval
        self._stop = threading.Event()

    def start(self) -> None:
        threading.Thread(target=self._run, daemon=True,
                         name=f"reap-{self._key[:40]}").start()

    def signal_stop(self) -> None:
        self._stop.set()

    def _run(self) -> None:
        hb = nstate.heartbeat_path(self._config, self._key)
        while not self._stop.is_set():
            if self._stop.wait(self._interval):
                return  # normal exit signalled by run loop
            if not _proc_alive(self._pid):
                # Process already gone (finished/crashed) — main loop will
                # collect it.  Nothing for the reaper to do.
                continue
            try:
                mtime = hb.stat().st_mtime
                idle_age = time.time() - mtime
            except FileNotFoundError:
                # No heartbeat file yet (launch transient).  Don't kill.
                continue
            except OSError:
                # transient FS — give more time before declaring dead
                continue
            if idle_age > self._idle:
                print(f"  [reaper] KILL {self._key} pid={self._pid} "
                      f"idle={idle_age:.0f}s > {self._idle}s", flush=True)
                _kill_tree(self._pid)
                killed_keys.add(self._key)
                nstate.write_result(
                    self._config, self._key, phase=self._phase,
                    status="stalled", error_class="timeout", exit_code=-15)
                return


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
    reapers: dict[str, Reaper] = {}   # ⚡ D.1-fix V3: separate so running stays 3-tuple
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
        #
        # start_new_session=True (POSIX only; harmless on Windows) gives the
        # child its own process group == its pid, so _kill_tree can os.killpg
        # cleanly post-kill children (D.1-fix V: tree kill).
        proc = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            cwd=_cwd,
            env=env,
            start_new_session=(platform.system() != "Windows"),
        )
        observer.attach(proc)
        running[key] = (rt, proc, observer)
        nstate.touch_heartbeat(config, key, pid=proc.pid)
        nstate.write_result(config, key, phase=phase_label,
                            status="running", stage=",".join(stages))
        # ⚡ D.1-fix V1: spawn a per-chunk Reaper with the phase's dominant stage
        #    idle budget.  If this chunk stops producing output for longer than
        #    the budget AND is still alive, the Reaper kills the whole tree and
        #    marks it stalls so the farm keeps moving (V4/V5).
        _idle = config.idle_for(stages[0]) if stages else config.max_idle_seconds
        reaper = Reaper(config, key, proc.pid, phase_label, idle_seconds=_idle)
        reapers[key] = reaper
        reaper.start()
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
            # ⚡ D.1-fix V3: clean up the Reaper for this chunk.
            reap = reapers.pop(key, None)
            if reap:
                reap.signal_stop()

            # ⚡ D.1-fix V4: if the Reaper already killed this chunk and wrote
            # a stalled result, do NOT overwrite it with pass/fail.
            if key in killed_keys:
                killed_keys.discard(key)
                rt, proc, obs = running.pop(key)
                obs.close() if hasattr(obs, "close") else None
                print(f"  [{phase_label}] STALLED {key} (reaper kill)", flush=True)
                result.chunk_results[key] = {"status": "stalled", "error_class": "timeout",
                                             "exit_code": -15, "phase": phase_label}
                continue

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

    # ⚡ D.1-fix: any reaper left (process vanished w/o finishing path) — stop.
    for _r in reapers.values():
        _r.signal_stop()
    reapers.clear()

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


