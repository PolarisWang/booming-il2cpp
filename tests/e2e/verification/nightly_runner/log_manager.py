"""Log manager — per-worker log capture via Tee streams.

Each ProcessPoolExecutor worker gets its own ChunkLogManager that:

  1. Replaces sys.stdout / sys.stderr with a TeeStream that writes to
     both the console AND a pipeline.log file.

  2. Sets os.environ["CHAOS_NIGHTLY_LOG_DIR"] to the chunk-specific
     log directory, so stage functions can detect it and write
     subprocess stdout/stderr to {stage_name}.log.

  3. Restores sys.stdout / sys.stderr on exit.

Usage:

    log_mgr = ChunkLogManager(log_dir, keep_console=True)
    with log_mgr:
        print("this goes to both console and pipeline.log")
        os.environ["CHAOS_NIGHTLY_LOG_DIR"]  # → str(log_mgr.log_dir)
"""
from __future__ import annotations

import os
import sys
import threading
from pathlib import Path


def _as_text(data) -> str:
    """Return str from bytes-or-str subprocess output (decodes bytes losslessly)."""
    if isinstance(data, bytes):
        return data.decode("utf-8", errors="replace")
    return data or ""


class TeeStream:
    """A write-only stream that tees to both a file and the original stream.

    Thread-safe: uses a lock for writes to avoid interleaving in the file.
    """

    def __init__(self, file_path: Path, original_stream, keep_console: bool = True):
        self._file_path = file_path
        self._file: Path | None = None  # opened lazily
        self._original = original_stream
        self._keep_console = keep_console
        self._lock = threading.Lock()
        self._closed = False

    def _ensure_open(self):
        if self._file is None and not self._closed:
            self._file_path.parent.mkdir(parents=True, exist_ok=True)
            # Use append mode so multiple TeeStreams can write to the
            # same file without losing data (defensive — each worker
            # should have its own file, but just in case).
            self._file = open(self._file_path, "a", encoding="utf-8", buffering=1)

    def write(self, text: str):
        if not text:
            return
        with self._lock:
            if self._keep_console and self._original and not self._closed:
                try:
                    self._original.write(text)
                    self._original.flush()
                except Exception:
                    pass  # best-effort
            try:
                self._ensure_open()
                if self._file:
                    self._file.write(text)
                    self._file.flush()
            except Exception:
                pass  # best-effort, don't crash the pipeline

    def flush(self):
        with self._lock:
            if self._keep_console and self._original:
                try:
                    self._original.flush()
                except Exception:
                    pass
            if self._file:
                try:
                    self._file.flush()
                except Exception:
                    pass

    def close(self):
        with self._lock:
            self._closed = True
            if self._file:
                try:
                    self._file.close()
                except Exception:
                    pass
                self._file = None

    @property
    def log_path(self) -> Path:
        return self._file_path


class ChunkLogManager:
    """Context manager that captures all stdout/stderr from a chunk worker.

    Each instance manages its own log directory:

        {report_dir}/logs/{assembly}/{slug}/
        ├── pipeline.log         ← Tee'd stdout + stderr
        ├── 00_build.log         ← stage function writes here via env var
        ├── 01_fact.log
        └── ...
    """

    def __init__(self, assembly: str, slug: str, report_dir: Path,
                 keep_console: bool = True):
        self.assembly = assembly
        self.slug = slug
        self.report_dir = report_dir
        self.keep_console = keep_console

        self.log_dir = report_dir / "logs" / assembly / slug
        self._old_stdout: object = None
        self._old_stderr: object = None
        self._tee: TeeStream | None = None

    def __enter__(self) -> ChunkLogManager:
        pipeline_log = self.log_dir / "pipeline.log"

        # Capture sys.stdout
        self._old_stdout = sys.stdout
        self._tee = TeeStream(pipeline_log, sys.stdout, self.keep_console)
        sys.stdout = self._tee  # type: ignore[assignment]

        # Capture sys.stderr
        self._old_stderr = sys.stderr
        self._tee_stderr = TeeStream(pipeline_log, sys.stderr, self.keep_console)
        sys.stderr = self._tee_stderr  # type: ignore[assignment]

        # Inject env var for stage functions to detect
        os.environ["CHAOS_NIGHTLY_LOG_DIR"] = str(self.log_dir)

        # Patch subprocess.run for all subprocess calls within this worker
        self._original_subprocess_run = _subprocess.run
        _subprocess.run = self._make_logged_run()

        return self

    def _make_logged_run(self):
        """Create a subprocess.run wrapper that logs to {stage_name}.log.

        Uses the leading command argument to determine the stage name:
          - "dotnet" commands → managed build/benchmark stages
          - "cmake" commands → build stage
          - entry.exe commands → fact/benchmark/hotupdate stages
          - ATG/TPG DLL paths → build stage
        Falls back to "subprocess" for unrecognized commands.
        """
        log_dir = self.log_dir

        def _logged_run(*args, **kwargs):
            result = self._original_subprocess_run(*args, **kwargs)

            # Determine stage name from command
            cmd_args = args[0] if args else []
            stage = _guess_stage_name(cmd_args)

            log_path = log_dir / f"{stage}.log"
            try:
                log_path.parent.mkdir(parents=True, exist_ok=True)
                with open(log_path, "a", encoding="utf-8", buffering=1) as f:
                    f.write(f"--- cmd: {' '.join(str(a) for a in cmd_args)}\n")
                    if result.stdout:
                        _out = _as_text(result.stdout)
                        f.write(_out if _out.endswith("\n") else _out + "\n")
                    if result.stderr:
                        f.write("--- stderr ---\n")
                        _err = _as_text(result.stderr)
                        f.write(_err if _err.endswith("\n") else _err + "\n")
            except OSError:
                pass
            return result

        return _logged_run

    def __exit__(self, *args):
        if self._tee:
            sys.stdout = self._old_stdout  # type: ignore[assignment]
            self._tee.close()
            self._tee = None
        if self._tee_stderr:
            sys.stderr = self._old_stderr  # type: ignore[assignment]
            self._tee_stderr.close()
            self._tee_stderr = None

        # Restore subprocess.run
        if hasattr(self, "_original_subprocess_run"):
            _subprocess.run = self._original_subprocess_run

        # Clean up env var (don't leak across chunks)
        os.environ.pop("CHAOS_NIGHTLY_LOG_DIR", None)


def _guess_stage_name(cmd_args: list) -> str:
    """Guess the stage name from a command's arguments."""
    if not cmd_args:
        return "subprocess"

    cmd = str(cmd_args[0]).lower()

    # Dotnet commands
    if "dotnet" in cmd:
        full = " ".join(str(a).lower() for a in cmd_args)
        if "build" in full or "restore" in full:
            return "build"
        if "exec" in full or "run" in full:
            if "runner" in full or "test" in full:
                return "managed_benchmark"
            return "fact"
        return "build"

    # CMake commands → build stage
    if "cmake" in cmd:
        return "build"

    # entry.exe
    if "entry" in cmd or "chaos_entry" in cmd:
        full = " ".join(str(a).lower() for a in cmd_args)
        if "--fact-json" in full:
            return "fact"
        if "--benchmark" in full:
            return "benchmark"
        if "--hotupdate" in full:
            return "hotupdate"
        return "fact"

    # AutoTestGenerator
    if "autotestgenerator" in cmd or "atg" in cmd or "auto_test_generator" in cmd:
        return "build"

    # TestProjectGenerator
    if "testprojectgenerator" in cmd or "tpg" in cmd:
        return "build"

    return "subprocess"


# ── Subprocess log capture helper ───────────────────────────────────────

_STAGE_LOG_DIR: str | None = None


def init_stage_log() -> str | None:
    """Initialize stage log directory from environment variable.

    Call this at the beginning of each stage function to get the log
    directory path. Returns None if CHAOS_NIGHTLY_LOG_DIR is not set
    (meaning the stage should log to console only, as before).

    Usage (add to each stage function — ≤3 lines):
        if init_stage_log():
            subprocess.run = logged_run  # type: ignore[assignment]
    """
    global _STAGE_LOG_DIR
    _STAGE_LOG_DIR = os.environ.get("CHAOS_NIGHTLY_LOG_DIR")
    return _STAGE_LOG_DIR


def get_stage_log_path(stage_name: str) -> Path | None:
    """Get the full path for a stage's log file."""
    global _STAGE_LOG_DIR
    if _STAGE_LOG_DIR is None:
        return None
    return Path(_STAGE_LOG_DIR) / f"{stage_name}.log"


def write_stage_log(stage_name: str, text: str):
    """Append text to a stage's log file.

    Thread-safe: uses append mode and flushes after each write.
    No-op if CHAOS_NIGHTLY_LOG_DIR is not set.
    """
    log_path = get_stage_log_path(stage_name)
    if log_path is None:
        return
    try:
        log_path.parent.mkdir(parents=True, exist_ok=True)
        with open(log_path, "a", encoding="utf-8") as f:
            f.write(text)
            f.flush()
    except OSError:
        pass  # best-effort


def save_subprocess_output(stage_name: str,
                           stdout: str = "",
                           stderr: str = "",
                           cmd: str = ""):
    """Save a subprocess run's stdout+stderr to the stage log file.

    Usage (add after each subprocess.run call — 1 line):
        save_subprocess_output("build", result.stdout, result.stderr, " ".join(cmd))
    """
    log_path = get_stage_log_path(stage_name)
    if log_path is None:
        return
    try:
        log_path.parent.mkdir(parents=True, exist_ok=True)
        with open(log_path, "a", encoding="utf-8") as f:
            if cmd:
                f.write(f"\n--- command: {cmd}\n")
            _stdout = _as_text(stdout)
            if _stdout:
                f.write(_stdout)
                if not _stdout.endswith("\n"):
                    f.write("\n")
            _stderr = _as_text(stderr)
            if _stderr:
                f.write("--- stderr ---\n")
                f.write(_stderr)
                if not _stderr.endswith("\n"):
                    f.write("\n")
            f.flush()
    except OSError:
        pass


# ── Context manager: wrap subprocess.run with log capture ──────────────

import subprocess as _subprocess
from contextlib import contextmanager


@contextmanager
def logged_subprocess(stage_name: str):
    """Context manager that logs all subprocess.run calls to the stage log.

    Replaces subprocess.run with a wrapper that captures stdout/stderr
    and writes them to {CHAOS_NIGHTLY_LOG_DIR}/{stage_name}.log.

    Usage (add to any stage function — 2 lines):

        from verification.nightly_runner.log_manager import logged_subprocess
        with logged_subprocess("build"):
            ...  # all subprocess.run calls are now logged
    """
    log_dir = os.environ.get("CHAOS_NIGHTLY_LOG_DIR")
    if not log_dir:
        yield
        return

    original_run = _subprocess.run

    def _logged_run(*args, **kwargs):
        result = original_run(*args, **kwargs)
        log_path = Path(log_dir) / f"{stage_name}.log"
        try:
            log_path.parent.mkdir(parents=True, exist_ok=True)
            with open(log_path, "a", encoding="utf-8", buffering=1) as f:
                if args:
                    f.write(f"--- cmd: {' '.join(str(a) for a in args[0])}\n")
                if result.stdout:
                    _out = _as_text(result.stdout)
                    f.write(_out if _out.endswith("\n") else _out + "\n")
                if result.stderr:
                    f.write("--- stderr ---\n")
                    _err = _as_text(result.stderr)
                    f.write(_err if _err.endswith("\n") else _err + "\n")
        except OSError:
            pass
        return result

    _subprocess.run = _logged_run
    try:
        yield
    finally:
        _subprocess.run = original_run
