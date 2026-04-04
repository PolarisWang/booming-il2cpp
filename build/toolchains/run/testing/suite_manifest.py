from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

try:
    from ..common import read_json
except ImportError:
    root = Path(__file__).resolve().parents[1]
    sys.path.insert(0, str(root))
    from common import read_json


@dataclass(frozen=True)
class SuiteManifest:
    manifest_path: Path
    family: str
    suite_id: str
    display_name: str
    runner_type: str
    runner_config: dict[str, Any]
    supported_stages: list[str]
    supported_hosts: list[str] | None
    tags: list[str]
    coverage_policy: dict[str, Any]
    coverage_enforced: bool

    @property
    def suite_key(self) -> str:
        return f"{self.family}/{self.suite_id}"

    def to_dict(self) -> dict[str, Any]:
        return {
            "id": self.suite_key,
            "family": self.family,
            "suiteId": self.suite_id,
            "displayName": self.display_name,
            "runnerType": self.runner_type,
            "supportedStages": list(self.supported_stages),
            "supportedHosts": list(self.supported_hosts or []),
            "tags": list(self.tags),
            "coveragePolicy": dict(self.coverage_policy),
            "coverageEnforced": self.coverage_enforced,
            "manifestPath": str(self.manifest_path),
        }


def _validate_manifest_path(path: Path) -> tuple[str, str]:
    if path.name != "suite.manifest.json":
        raise ValueError(f"unexpected suite manifest path: {path}")

    try:
        suite_id = path.parent.name
        family = path.parent.parent.name
        tests_root = path.parent.parent.parent.name
    except IndexError as error:
        raise ValueError(f"unexpected suite manifest path: {path}") from error

    if tests_root != "tests" or not family or not suite_id:
        raise ValueError(f"unexpected suite manifest path: {path}")
    return family, suite_id


def _require_string(payload: dict[str, Any], field_name: str) -> str:
    value = payload.get(field_name)
    if not isinstance(value, str) or not value.strip():
        raise ValueError(f"{field_name} is required")
    return value


def _require_string_list(payload: dict[str, Any], field_name: str) -> list[str]:
    value = payload.get(field_name)
    if not isinstance(value, list) or not value or any(not isinstance(item, str) or not item.strip() for item in value):
        raise ValueError(f"{field_name} must be a non-empty string list")
    return list(value)


def load_suite_manifest(path: Path) -> SuiteManifest:
    manifest_path = path.resolve()
    family, suite_id = _validate_manifest_path(manifest_path)
    payload = read_json(manifest_path)
    if not isinstance(payload, dict):
        raise ValueError("suite manifest must be an object")

    display_name = _require_string(payload, "displayName")
    runner_type = _require_string(payload, "runnerType")
    supported_stages = _require_string_list(payload, "supportedStages")

    runner_config = payload.get("runnerConfig")
    if not isinstance(runner_config, dict):
        raise ValueError("runnerConfig must be an object")

    supported_hosts_raw = payload.get("supportedHosts")
    if supported_hosts_raw is None:
        supported_hosts = None
    else:
        if not isinstance(supported_hosts_raw, list) or any(not isinstance(item, str) or not item.strip() for item in supported_hosts_raw):
            raise ValueError("supportedHosts must be a string list")
        supported_hosts = list(supported_hosts_raw)

    tags_raw = payload.get("tags", [])
    if not isinstance(tags_raw, list) or any(not isinstance(item, str) or not item.strip() for item in tags_raw):
        raise ValueError("tags must be a string list")

    coverage_policy_raw = payload.get("coveragePolicy", {})
    if not isinstance(coverage_policy_raw, dict):
        raise ValueError("coveragePolicy must be an object")
    coverage_policy = dict(coverage_policy_raw)
    coverage_enforced = bool(coverage_policy.get("enforced", family not in {"workflow", "perf"}))

    return SuiteManifest(
        manifest_path=manifest_path,
        family=family,
        suite_id=suite_id,
        display_name=display_name,
        runner_type=runner_type,
        runner_config=dict(runner_config),
        supported_stages=supported_stages,
        supported_hosts=supported_hosts,
        tags=list(tags_raw),
        coverage_policy=coverage_policy,
        coverage_enforced=coverage_enforced,
    )
