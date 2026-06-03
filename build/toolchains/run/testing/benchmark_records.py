"""benchmark_records.py — Append-Only JSON Lines store for benchmark run data.

Each benchmark run appends a single JSON record (one line) to:
    artifact/verification/benchmark-records/{subject_id}/records.jsonl

Queries scan the file from the end to retrieve the most-recent record(s).
"""
from __future__ import annotations

import json
from pathlib import Path
from typing import Any

try:
    from . import verification_layout as verification_layout_module
except ImportError:
    import sys

    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from testing import verification_layout as verification_layout_module


# ---------------------------------------------------------------------------
# Path resolution
# ---------------------------------------------------------------------------

def _records_path(repo_root: Path, subject_id: str) -> Path:
    return verification_layout_module.raw_benchmark_records_path(repo_root, subject_id)


# ---------------------------------------------------------------------------
# Write
# ---------------------------------------------------------------------------

def append_record(repo_root: Path, record: dict[str, Any]) -> None:
    """Append a single benchmark record to the subject's JSONL file."""
    subject_id = str(record.get("subject") or "")
    if not subject_id:
        raise ValueError("record must have a 'subject' field")

    path = _records_path(repo_root, subject_id)
    path.parent.mkdir(parents=True, exist_ok=True)

    line = json.dumps(record, ensure_ascii=False, separators=(",", ":")) + "\n"

    with open(path, "a", encoding="utf-8") as f:
        f.write(line)


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
        # Support nested device.id via "device_id" → record["device"]["id"]
        if key == "device_id":
            if record.get("device", {}).get("id") != value:
                return False
        elif record.get(key) != value:
            return False
    return True


def query_latest(
    repo_root: Path,
    subject_id: str,
    mode: str,
    device_id: str,
) -> dict[str, Any] | None:
    """Return the most-recent record matching subject+mode+device, or None."""
    path = _records_path(repo_root, subject_id)
    for record in _iter_records_reverse(path):
        if _matches(record, subject=subject_id, mode=mode, device_id=device_id):
            return record
    return None


def query_latest_all_modes(
    repo_root: Path,
    subject_id: str,
    device_id: str,
) -> dict[str, dict[str, Any] | None]:
    """Return {mode: latest_record} for all known modes for a subject+device."""
    modes = {"managed", "native", "interpreter"}
    result: dict[str, dict[str, Any] | None] = {m: None for m in modes}
    path = _records_path(repo_root, subject_id)
    found: set[str] = set()
    for record in _iter_records_reverse(path):
        if not _matches(record, subject=subject_id, device_id=device_id):
            continue
        mode = str(record.get("mode") or "")
        if mode in modes and mode not in found:
            result[mode] = record
            found.add(mode)
        if found == modes:
            break
    return result


def query_history(
    repo_root: Path,
    subject_id: str,
    mode: str,
    device_id: str,
    limit: int = 20,
) -> list[dict[str, Any]]:
    """Return the N most-recent records (newest first) for subject+mode+device."""
    path = _records_path(repo_root, subject_id)
    results: list[dict[str, Any]] = []
    for record in _iter_records_reverse(path):
        if _matches(record, subject=subject_id, mode=mode, device_id=device_id):
            results.append(record)
            if len(results) >= limit:
                break
    return results


def query_all_devices(
    repo_root: Path,
    subject_id: str,
    mode: str,
) -> dict[str, dict[str, Any]]:
    """Return {device_id: latest_record} for a subject+mode across all devices."""
    path = _records_path(repo_root, subject_id)
    result: dict[str, dict[str, Any]] = {}
    for record in _iter_records_reverse(path):
        if not _matches(record, subject=subject_id, mode=mode):
            continue
        dev_id = record.get("device", {}).get("id") or "unknown"
        if dev_id not in result:
            result[dev_id] = record
    return result


def list_subjects_with_records(repo_root: Path) -> list[str]:
    """Return sorted list of subject IDs that have at least one benchmark record."""
    subjects_root = verification_layout_module.raw_benchmark_records_root(repo_root)
    if not subjects_root.is_dir():
        return []
    results: list[str] = []
    for p in sorted(subjects_root.iterdir()):
        if (p / "records.jsonl").exists():
            results.append(p.name)
    return results
