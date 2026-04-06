from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any


def _fingerprint_id(fingerprint_inputs: dict[str, Any]) -> str:
    payload = json.dumps(fingerprint_inputs, sort_keys=True, ensure_ascii=False, separators=(",", ":"))
    return hashlib.sha256(payload.encode("utf-8")).hexdigest()[:12]


def materialize_fingerprint(
    *,
    repo_root: Path,
    host_platform: str,
    family: str,
    suite: str,
    fingerprint_inputs: dict[str, Any],
) -> dict[str, str]:
    fingerprint = _fingerprint_id(fingerprint_inputs)
    suite_root = repo_root / "artifacts" / "tests" / "build" / host_platform / family / suite
    fingerprint_dir = suite_root / "fingerprints" / fingerprint
    current_path = suite_root / "current.json"
    fingerprint_dir_text = fingerprint_dir.as_posix()

    fingerprint_dir.mkdir(parents=True, exist_ok=True)
    current_payload = {
        "fingerprint": fingerprint,
        "fingerprintDir": fingerprint_dir_text,
    }
    current_path.write_text(json.dumps(current_payload, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")

    return {
        "fingerprint": fingerprint,
        "fingerprintDir": fingerprint_dir_text,
        "currentPath": str(current_path.relative_to(repo_root).as_posix()),
    }
