from __future__ import annotations

import json
import uuid
from pathlib import Path
from typing import Any


REPO_ROOT = Path(__file__).resolve().parents[2]
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests"


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def make_temp_repo_root(area: str, prefix: str) -> Path:
    repo_root = TEST_TMP_ROOT / area / f"{prefix}-{uuid.uuid4().hex}"
    repo_root.mkdir(parents=True, exist_ok=False)
    return repo_root
