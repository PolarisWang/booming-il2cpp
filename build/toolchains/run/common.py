from __future__ import annotations

import json
import locale
import os
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


def _decode_output(data: bytes | None) -> str:
    if not data:
        return ""

    def normalize(text: str) -> str:
        return text.replace("\r\n", "\n").replace("\r", "\n")

    candidates = ["utf-8-sig", "utf-8", locale.getpreferredencoding(False)]
    if os.name == "nt":
        candidates.append("mbcs")

    seen: set[str] = set()
    for encoding in candidates:
        normalized = (encoding or "").lower()
        if not normalized or normalized in seen:
            continue
        seen.add(normalized)
        try:
            return normalize(data.decode(encoding))
        except UnicodeDecodeError:
            continue

    fallback = locale.getpreferredencoding(False) or "utf-8"
    return normalize(data.decode(fallback, errors="replace"))


def _merge_environment(overrides: dict[str, str] | None) -> dict[str, str] | None:
    if not overrides:
        return None

    merged = dict(os.environ)
    if os.name == "nt":
        existing_keys = {key.lower(): key for key in merged}
        for key, value in overrides.items():
            duplicate = existing_keys.get(key.lower())
            if duplicate is not None and duplicate != key:
                merged.pop(duplicate, None)
            merged[key] = value
        return merged

    return {**merged, **overrides}


def run_process(
    arguments: list[str],
    cwd: Path | None = None,
    env: dict[str, str] | None = None,
) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=str(cwd) if cwd else None,
        env=_merge_environment(env),
        capture_output=True,
        text=False,
        check=False,
    )
    return subprocess.CompletedProcess(
        completed.args,
        completed.returncode,
        _decode_output(completed.stdout),
        _decode_output(completed.stderr),
    )


def combine_process_output(completed: subprocess.CompletedProcess[str]) -> str:
    parts = []
    if completed.stdout:
        parts.append(completed.stdout.strip())
    if completed.stderr:
        parts.append(completed.stderr.strip())
    return "\n".join(part for part in parts if part).strip()
