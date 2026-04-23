from __future__ import annotations

import json
import shutil
import subprocess
import time
import unittest
import uuid
from pathlib import Path
from typing import Any

from tests.support import load_module
from unittest.mock import patch


REPO_ROOT = Path(__file__).resolve().parents[3]
BENCHMARK_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "commands" / "benchmark.py"
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "benchmark-command"

class FakeRecordsModule:
    def __init__(self) -> None:
        self.records: list[dict[str, object]] = []

    def append_record(self, repo_root: Path, record: dict[str, object]) -> None:
        self.records.append(record)


class BenchmarkCommandTestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self, prefix: str) -> Path:
        repo_root = TEST_TMP_ROOT / f"{prefix}-{uuid.uuid4().hex}"
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_subject_fixture(
        self,
        repo_root: Path,
        subject_id: str,
        *,
        perf_matrix_ids: list[str] | None = None,
    ) -> None:
        manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        environment_matrices = []
        for matrix_id in perf_matrix_ids or []:
            environment_matrices.append(
                {
                    "matrixId": matrix_id,
                    "pipelineId": "managed-benchmark",
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": "windows-x64",
                        "runtimeProfile": "managed-perf-release",
                    },
                }
            )
        manifest_path.write_text(
            json.dumps(
                {
                    "subjectId": subject_id,
                    "displayName": subject_id,
                    "environmentMatrices": environment_matrices,
                }
            ),
            encoding="utf-8",
        )

        csproj_path = repo_root / "subjects" / subject_id / "source" / f"{subject_id}.csproj"
        csproj_path.parent.mkdir(parents=True, exist_ok=True)
        csproj_path.write_text("<Project Sdk=\"Microsoft.NET.Sdk\"></Project>", encoding="utf-8")

    def _write_workspace_benchmark_fixture(
        self,
        repo_root: Path,
        *,
        subject_id: str,
        stable_id: str,
        alias: str,
        entry_index: int,
        assembly_name: str,
        declaring_type: str,
        method_name: str,
        method_signature: str,
        matrix_id: str = "workspace-benchmark-matrix",
        native_host_assembly_name: str | None = None,
    ) -> None:
        workspace_root = repo_root / "verification" / "workspaces" / "subjects" / subject_id
        managed_tests_root = workspace_root / "managed-tests"
        generated_root = managed_tests_root / "Generated"
        project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
        generated_source_path = generated_root / "ChaosGeneratedDeclaredBenchmarks.g.cs"
        collection_path = generated_root / "declared-tests.collection.json"
        manifest_path = workspace_root / "workspace.manifest.json"
        native_host_project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkNativeHost.csproj"
        native_host_generated_source_path = generated_root / "ChaosGeneratedDeclaredNativeBenchmarks.g.cs"

        materialized_paths = [project_path, generated_source_path]
        if native_host_assembly_name:
            materialized_paths.extend([native_host_project_path, native_host_generated_source_path])

        for path in materialized_paths:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("<Project />\n" if path.suffix == ".csproj" else "// fixture\n", encoding="utf-8")

        collection_payload = {
            "subjectId": subject_id,
            "frameworkReferenced": True,
            "subjectKind": "declared-test",
            "warningCodes": [],
            "declaredUnitTests": [],
            "declaredBenchmarks": [
                {
                    "stableId": stable_id,
                    "entryIndex": entry_index,
                    "alias": alias,
                    "assemblyName": assembly_name,
                    "declaringType": declaring_type,
                    "methodName": method_name,
                    "methodSignature": method_signature,
                    "category": 1,
                    "capabilityFamily": 1,
                    "capabilityItem": 1,
                    "archetype": 1,
                    "hotUpdateCapability": 0,
                    "requires": 0,
                    "metrics": 1,
                    "modes": 3,
                    "warmupCount": 2,
                    "iterationCount": 5,
                    "invocationCount": 10,
                }
            ],
        }
        collection_path.write_text(json.dumps(collection_payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

        managed_test_projects = [
            {
                "projectId": f"managed-test/{subject_id}/benchmark-host",
                "projectPath": project_path.relative_to(repo_root).as_posix(),
                "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                "hostKind": "benchmark-host",
                "collectionPath": collection_path.relative_to(repo_root).as_posix(),
                "generatedSourcePath": generated_source_path.relative_to(repo_root).as_posix(),
            }
        ]
        native_test_projects: list[dict[str, Any]] = []
        if native_host_assembly_name:
            managed_test_projects.append(
                {
                    "projectId": f"managed-test/{subject_id}/benchmark-host-native",
                    "projectPath": native_host_project_path.relative_to(repo_root).as_posix(),
                    "assemblyName": native_host_assembly_name,
                    "hostKind": "benchmark-host",
                    "collectionPath": collection_path.relative_to(repo_root).as_posix(),
                    "generatedSourcePath": native_host_generated_source_path.relative_to(repo_root).as_posix(),
                }
            )
            native_test_projects.append(
                {
                    "projectId": f"native-test/{subject_id}/{matrix_id}/benchmark-host",
                    "matrixId": matrix_id,
                    "projectPath": (
                        workspace_root / "native" / matrix_id / "benchmark" / "chaos_subject_native_aot.vcxproj"
                    ).relative_to(repo_root).as_posix(),
                    "configureRoot": (workspace_root / "native" / matrix_id).relative_to(repo_root).as_posix(),
                    "hostKind": "benchmark-host",
                    "managedTestProjectId": f"managed-test/{subject_id}/benchmark-host-native",
                }
            )

        manifest_payload: dict[str, Any] = {
            "workspaceVersion": 2,
            "kind": "subject-workspace",
            "subjectId": subject_id,
            "defaultMatrixId": matrix_id,
            "managedProjects": [],
            "managedTestProjects": managed_test_projects,
            "nativeProjects": [],
            "nativeTestProjects": native_test_projects,
            "matrices": [
                {
                    "matrixId": matrix_id,
                    "goalIds": ["perf.release"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "managedTestProjectIds": [str(item["projectId"]) for item in managed_test_projects],
                    "nativeTestProjectIds": [str(item["projectId"]) for item in native_test_projects],
                }
            ],
        }
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        manifest_path.write_text(json.dumps(manifest_payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

