"""stress_records.py — Append-Only JSON Lines store for stress test run data.

Each stress test run appends a single JSON record (one line) to:
    artifact/verification/stress-records/{test_name}/records.jsonl

Queries scan the file from the end to retrieve the most-recent record(s).
File locking prevents concurrent-write corruption.

Mirrors benchmark_records.py pattern exactly.
"""
from __future__ import annotations

import json
import os
import time
from pathlib import Path
from typing import Any

try:
    from . import verification_layout as verification_layout_module
except ImportError:
    import sys

    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import verification_layout as verification_layout_module

# fcntl is Unix-only; on Windows we use a lock-file strategy instead
try:
    import fcntl as _fcntl
    _HAS_FCNTL = True
except ImportError:
    _fcntl = None  # type: ignore[assignment]
    _HAS_FCNTL = False


# ---------------------------------------------------------------------------
# Path resolution
# ---------------------------------------------------------------------------

def _records_path(repo_root: Path, test_name: str) -> Path:
    return verification_layout_module.raw_stress_records_path(repo_root, test_name)


# ---------------------------------------------------------------------------
# Write
# ---------------------------------------------------------------------------

def append_record(repo_root: Path, record: dict[str, Any]) -> None:
    """Append a single stress test record to the test's JSONL file.

    Uses a file lock to prevent corruption from concurrent writers.
    """
    test_name = str(record.get("testName") or "")
    if not test_name:
        raise ValueError("record must have a 'testName' field")

    path = _records_path(repo_root, test_name)
    path.parent.mkdir(parents=True, exist_ok=True)

    line = json.dumps(record, ensure_ascii=False, separators=(",", ":")) + "\n"

    # Platform-aware locking: use fcntl on Unix, fall back to a .lock file on Windows.
    if os.name == "nt" or not _HAS_FCNTL:
        _append_windows(path, line)
    else:
        _append_unix(path, line)


def _append_windows(path: Path, line: str) -> None:
    """Append with a simple retry+rename strategy on Windows (no fcntl)."""
    lock_path = path.with_suffix(".lock")
    for _ in range(20):
        try:
            # Exclusive creation of lock file
            fd = os.open(str(lock_path), os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            os.close(fd)
            break
        except FileExistsError:
            time.sleep(0.05)
    try:
        with open(path, "a", encoding="utf-8") as f:
            f.write(line)
    finally:
        try:
            lock_path.unlink(missing_ok=True)
        except Exception:
            pass


def _append_unix(path: Path, line: str) -> None:
    with open(path, "a", encoding="utf-8") as f:
        _fcntl.flock(f, _fcntl.LOCK_EX)  # type: ignore[union-attr]
        try:
            f.write(line)
        finally:
            _fcntl.flock(f, _fcntl.LOCK_UN)  # type: ignore[union-attr]


# ---------------------------------------------------------------------------
# Read helpers
# ---------------------------------------------------------------------------

def _iter_records_reverse(path: Path):
    """Yield records from the JSONL file in reverse order (newest first)."""
    if not path.exists():
        return
    lines = path.read_text(encoding="utf-8").splitlines()
    for raw in reversed(lines):
        raw = raw.strip()
        if not raw:
            continue
        try:
            yield json.loads(raw)
        except json.JSONDecodeError:
            continue


def _matches(record: dict[str, Any], **criteria) -> bool:
    for key, value in criteria.items():
        if record.get(key) != value:
            return False
    return True


# ---------------------------------------------------------------------------
# Query functions
# ---------------------------------------------------------------------------

def query_latest(
    repo_root: Path,
    test_name: str,
    scenario_name: str | None = None,
) -> dict[str, Any] | None:
    """Return the most-recent record for a test (optionally filtered by scenario), or None."""
    path = _records_path(repo_root, test_name)
    for record in _iter_records_reverse(path):
        if scenario_name is not None and record.get("scenarioName") != scenario_name:
            continue
        return record
    return None


def query_history(
    repo_root: Path,
    test_name: str,
    scenario_name: str | None = None,
    limit: int = 20,
) -> list[dict[str, Any]]:
    """Return the N most-recent records (newest first) for a test+scenario."""
    path = _records_path(repo_root, test_name)
    results: list[dict[str, Any]] = []
    for record in _iter_records_reverse(path):
        if scenario_name is not None and record.get("scenarioName") != scenario_name:
            continue
        results.append(record)
        if len(results) >= limit:
            break
    return results


def query_by_run_id(repo_root: Path, test_name: str, run_id: str) -> dict[str, Any] | None:
    """Return a specific record by runId, or None."""
    path = _records_path(repo_root, test_name)
    for record in _iter_records_reverse(path):
        if record.get("runId") == run_id:
            return record
    return None


def list_tests_with_records(repo_root: Path) -> list[str]:
    """Return sorted list of test names that have at least one stress record."""
    tests_root = verification_layout_module.raw_stress_records_root(repo_root)
    if not tests_root.is_dir():
        return []
    results: list[str] = []
    for p in sorted(tests_root.iterdir()):
        if (p / "records.jsonl").exists():
            results.append(p.name)
    return results


def list_scenarios(repo_root: Path, test_name: str) -> list[str]:
    """Return sorted list of unique scenario names seen in a test's records."""
    seen: set[str] = set()
    path = _records_path(repo_root, test_name)
    for record in _iter_records_reverse(path):
        sn = record.get("scenarioName")
        if sn:
            seen.add(str(sn))
    return sorted(seen)
