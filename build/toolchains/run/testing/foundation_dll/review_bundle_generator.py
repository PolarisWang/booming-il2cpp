from __future__ import annotations

import json
from pathlib import Path
from typing import Any


def _string(value: Any) -> str:
    return str(value or "").strip()


def _family_slug(family_id: str) -> str:
    parts = [part for part in _string(family_id).split("/") if part]
    if len(parts) >= 4:
        return "-".join(parts[2:])
    return _string(family_id).replace("/", "-")


def _write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def generate_review_bundle(
    repo_root: Path,
    *,
    assembly_name: str,
    family_id: str,
    display_name: str,
    generated_artifacts: list[str],
    solution_path: str,
) -> dict[str, Any]:
    review_root = repo_root / "testing" / "foundation-dll" / assembly_name / _family_slug(family_id) / "review"
    bundle_path = review_root / "bundle.json"
    payload = {
        "schemaVersion": 1,
        "assemblyName": assembly_name,
        "familyId": family_id,
        "displayName": display_name,
        "generatedArtifacts": list(generated_artifacts),
        "solutionPath": _string(solution_path),
        "notesPath": str((review_root / "notes.md").relative_to(repo_root)).replace("\\", "/"),
    }

    # Preserve existing review notes if present (may contain AI-generated content)
    notes_path = review_root / "notes.md"
    if not notes_path.is_file():
        notes_path.write_text("# Review Notes\n", encoding="utf-8")

    _write_json(bundle_path, payload)
    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": [
            bundle_path.relative_to(repo_root).as_posix(),
            (review_root / "notes.md").relative_to(repo_root).as_posix(),
        ],
    }
