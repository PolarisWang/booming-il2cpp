from __future__ import annotations

import json
import subprocess
from pathlib import Path
from typing import Any


def read_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8-sig"))


def write_json(path: Path, payload: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        json.dumps(payload, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )


def join_relative(base: Path, relative_path: str) -> Path:
    result = base
    for segment in Path(relative_path).parts:
        result = result / segment
    return result


def run_process(arguments: list[str], cwd: Path | None = None) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        arguments,
        cwd=str(cwd) if cwd else None,
        capture_output=True,
        text=True,
        check=False,
    )


def combine_process_output(completed: subprocess.CompletedProcess[str]) -> str:
    parts = []
    if completed.stdout:
        parts.append(completed.stdout.strip())
    if completed.stderr:
        parts.append(completed.stderr.strip())
    return "\n".join(part for part in parts if part).strip()
