from __future__ import annotations

import importlib.util
import json
import shutil
import subprocess
import sys
import unittest
from pathlib import Path
from typing import Any

from tests.support import (
    HOT_UPDATE_HOST_PACK_HOST_PROJECT_PATH,
    clone_registry_fixture_tree,
    make_temp_repo_root,
    materialize_subject_manifest,
    write_json,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "registry.py"
PUBLIC_SPECS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "public_specs.py"
HOT_UPDATE_HOST_PACK_PROJECT_PATH = HOT_UPDATE_HOST_PACK_HOST_PROJECT_PATH


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


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def make_stage(
    stage_id: str,
    kind: str,
    bucket: str,
    *,
    scope: str = "matrix",
    depends_on: list[str] | None = None,
) -> dict[str, Any]:
    return {
        "stageId": stage_id,
        "kind": kind,
        "scope": scope,
        "bucket": bucket,
        "dependsOn": list(depends_on or []),
    }


def make_pipeline(pipeline_id: str, stages: list[dict[str, Any]]) -> dict[str, Any]:
    return {
        "pipelineId": pipeline_id,
        "displayName": pipeline_id,
        "stages": list(stages),
    }


def build_registry_subject_manifest(
    subject_id: str,
    *,
    default_goal: str,
    default_matrix: str,
    execution_pipelines: list[dict[str, Any]],
    environment_matrices: list[dict[str, Any]],
    default_validation_profile: str = "proof-dev",
    validation_profiles: dict[str, list[str]] | None = None,
    validation: dict[str, dict[str, Any]] | None = None,
    source_path: str | None = None,
    primary_project_path: str | None = None,
    entry: str | None = None,
    category: str = "canonical",
    source_model: str = "dotnet-project-set",
    dependency_model: str = "project-reference",
    executable_plan: str = "generated-native",
    engineering_profile: str = "native-executable",
    availability: dict[str, str] | None = None,
    workload_entry: str | None = None,
    test_declaration_mode: str = "none",
) -> dict[str, Any]:
    resolved_source_path = source_path or f"verification/catalog/owners/{subject_id}/support/host/{subject_id}.csproj"
    resolved_primary_project_path = primary_project_path or resolved_source_path
    resolved_validation_profiles = validation_profiles or {default_validation_profile: ["proof"]}
    resolved_validation = validation or {
        "proof": {
            "kind": "proof",
            "defaultVariant": "CHECK",
        }
    }
    manifest = {
        "subjectId": subject_id,
        "displayName": subject_id,
        "category": category,
        "defaultGoal": default_goal,
        "defaultMatrix": default_matrix,
        "defaultValidationProfile": default_validation_profile,
        "sourceModel": source_model,
        "dependencyModel": dependency_model,
        "executablePlan": executable_plan,
        "engineeringProfile": engineering_profile,
        "availability": availability or {},
        "testDeclarationMode": test_declaration_mode,
        "source": {
            "type": "dotnet-project",
            "path": resolved_source_path,
            "primaryProjectPath": resolved_primary_project_path,
            "entry": entry or f"{subject_id}/Program::Main()",
        },
        "validationProfiles": resolved_validation_profiles,
        "validation": resolved_validation,
        "executionPipelines": list(execution_pipelines),
        "environmentMatrices": list(environment_matrices),
    }
    if workload_entry:
        manifest["workloadEntry"] = workload_entry
    return manifest


def create_registry_repo(
    prefix: str,
    *,
    subjects: list[dict[str, Any]],
    copy_registry_fixtures: bool = False,
) -> Path:
    repo_root = make_temp_repo_root("registry-scan", prefix)
    if copy_registry_fixtures:
        clone_registry_fixture_tree(repo_root)
    for manifest in subjects:
        materialize_subject_manifest(repo_root, manifest)
    return repo_root

class RegistryScanTestSupport(unittest.TestCase):
    pass
