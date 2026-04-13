from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import uuid
from pathlib import Path
from typing import Any


REPO_ROOT = Path(__file__).resolve().parents[1]
PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"
FIXTURE_SUBJECTS_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects"
REGISTRY_FIXTURES_ROOT = REPO_ROOT / "tests" / "fixtures" / "registry"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module

def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def make_temp_repo_root(area: str, prefix: str) -> Path:
    repo_root = TEST_TMP_ROOT / area / f"{prefix}-{uuid.uuid4().hex}"
    repo_root.mkdir(parents=True, exist_ok=False)
    return repo_root


def _rewrite_path_prefixes(payload: object, *, prefix: str, replacement: str) -> object:
    if isinstance(payload, dict):
        return {
            key: _rewrite_path_prefixes(value, prefix=prefix, replacement=replacement)
            for key, value in payload.items()
        }
    if isinstance(payload, list):
        return [_rewrite_path_prefixes(value, prefix=prefix, replacement=replacement) for value in payload]
    if isinstance(payload, str) and payload.startswith(prefix):
        return replacement + payload.removeprefix(prefix)
    return payload


def rewrite_fixture_manifest_paths(payload: object, fixture_subject_id: str) -> object:
    return _rewrite_path_prefixes(
        payload,
        prefix=f"tests/fixtures/subjects/{fixture_subject_id}/",
        replacement=f"subjects/{fixture_subject_id}/",
    )


def clone_fixture_subject_repo(
    fixture_subject_id: str,
    *,
    area: str = "fixture-subjects",
) -> tuple[Path, dict[str, Any]]:
    repo_root = make_temp_repo_root(area, fixture_subject_id.lower())
    subject_root = repo_root / "subjects" / fixture_subject_id
    shutil.copytree(FIXTURE_SUBJECTS_ROOT / fixture_subject_id, subject_root)
    manifest_path = subject_root / "subject.manifest.json"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    manifest = rewrite_fixture_manifest_paths(manifest, fixture_subject_id)
    write_json(manifest_path, manifest)
    return repo_root, manifest


def clone_registry_fixture_tree(repo_root: Path) -> Path:
    target_root = repo_root / "tests" / "fixtures" / "registry"
    target_root.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(REGISTRY_FIXTURES_ROOT, target_root)
    return target_root


def materialize_subject_manifest(repo_root: Path, manifest: dict[str, Any]) -> Path:
    subject_id = str(manifest["subjectId"])
    source = dict(manifest.get("source") or {})
    relative_paths = [
        str(source.get("path") or ""),
        str(source.get("primaryProjectPath") or ""),
    ]
    validation = dict(manifest.get("validation") or {})
    for validation_spec in validation.values():
        relative_paths.append(str(dict(validation_spec).get("project") or ""))

    for relative_path in relative_paths:
        if not relative_path:
            continue
        materialized_path = repo_root / relative_path
        materialized_path.parent.mkdir(parents=True, exist_ok=True)
        if materialized_path.suffix:
            materialized_path.write_text("<Project />\n", encoding="utf-8")
        else:
            materialized_path.mkdir(parents=True, exist_ok=True)

    for expected_path in dict(manifest.get("expected") or {}).values():
        (repo_root / str(expected_path)).mkdir(parents=True, exist_ok=True)
    for baseline_path in dict(manifest.get("baselines") or {}).values():
        target_path = repo_root / str(baseline_path)
        if target_path.suffix:
            target_path.parent.mkdir(parents=True, exist_ok=True)
        else:
            target_path.mkdir(parents=True, exist_ok=True)

    manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
    write_json(manifest_path, manifest)
    return manifest_path

def load_public_specs_module(module_name: str):
    return load_module(PUBLIC_SPECS_MODULE_PATH, module_name)


def select_public_suite_spec(
    module_name: str,
    *,
    host_platform: str | None = None,
    family: str | None = None,
    required_stages: list[str] | None = None,
    required_module_ids: list[str] | None = None,
    required_subsystem_ids: list[str] | None = None,
) -> dict[str, Any]:
    public_specs_module = load_public_specs_module(module_name)
    matches: list[dict[str, Any]] = []
    for spec in list(public_specs_module.PUBLIC_TEST_SPECS):
        if host_platform is not None and host_platform not in list(spec.get("supported_hosts") or []):
            continue
        if family is not None and str(spec.get("family") or "") != family:
            continue
        if required_stages and not set(required_stages).issubset(set(spec.get("stages") or [])):
            continue
        if required_module_ids and not set(required_module_ids).issubset(set(spec.get("moduleIds") or [])):
            continue
        if required_subsystem_ids and not set(required_subsystem_ids).issubset(set(spec.get("subsystemIds") or [])):
            continue
        matches.append(dict(spec))
    if not matches:
        raise AssertionError(
            "no public suite spec matched filters: "
            f"host_platform={host_platform}, family={family}, required_stages={required_stages}, "
            f"required_module_ids={required_module_ids}, required_subsystem_ids={required_subsystem_ids}"
        )
    return sorted(matches, key=lambda item: str(item["id"]))[0]


def select_registry_item(
    items: list[dict[str, Any]],
    *,
    object_type: str | None = None,
    required_family: str | None = None,
    required_level: str | None = None,
    required_module_ids: list[str] | None = None,
    required_subsystem_ids: list[str] | None = None,
    required_supported_hosts: list[str] | None = None,
) -> dict[str, Any]:
    matches: list[dict[str, Any]] = []
    for item in items:
        if object_type is not None and str(item.get("type") or "") != object_type:
            continue
        if required_family is not None and str(item.get("family") or "") != required_family:
            continue
        if required_level is not None and str(item.get("level") or "") != required_level:
            continue
        if required_module_ids and not set(required_module_ids).issubset(set(item.get("moduleIds") or [])):
            continue
        if required_subsystem_ids and not set(required_subsystem_ids).issubset(set(item.get("subsystemIds") or [])):
            continue
        if required_supported_hosts and not set(required_supported_hosts).issubset(set(item.get("supportedHosts") or [])):
            continue
        matches.append(item)
    if not matches:
        raise AssertionError(
            "no registry item matched filters: "
            f"object_type={object_type}, required_family={required_family}, required_level={required_level}, "
            f"required_module_ids={required_module_ids}, required_subsystem_ids={required_subsystem_ids}, "
            f"required_supported_hosts={required_supported_hosts}"
        )
    return sorted(matches, key=lambda item: str(item["id"]))[0]
