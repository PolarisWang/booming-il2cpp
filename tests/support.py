from __future__ import annotations

import importlib.util
import sys
from pathlib import Path
from typing import Any


REPO_ROOT = Path(__file__).resolve().parents[1]
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"
PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"


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


def load_subjects_module(module_name: str):
    return load_module(SUBJECTS_MODULE_PATH, module_name)


def select_subject_record(module_name: str, **filters: object) -> dict[str, Any]:
    subjects_module = load_subjects_module(module_name)
    records = subjects_module.query_subject_records(
        subjects_module.load_subject_records(REPO_ROOT),
        **filters,
    )
    if not records:
        raise AssertionError(f"no subject record matched filters: {filters}")
    return sorted(records, key=lambda item: str(item["subjectId"]))[0]


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
