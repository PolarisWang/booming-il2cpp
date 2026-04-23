from __future__ import annotations

import json
import shutil
import unittest
import uuid
from pathlib import Path
from typing import Any

from tests.support import load_module

REPO_ROOT = Path(__file__).resolve().parents[3]
BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "benchmark_dashboard_generator.py"
)
BENCHMARK_COMPARISON_MODULE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "benchmark_comparison.py"
)
BENCHMARK_DASHBOARD_TEMPLATE_PATH = (
    REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "templates" / "benchmark-dashboard.html"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "benchmark-dashboard-generator"


class BenchmarkDashboardGeneratorTestSupport(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def _make_repo_root(self) -> Path:
        repo_root = TEST_TMP_ROOT / uuid.uuid4().hex
        repo_root.mkdir(parents=True, exist_ok=False)
        return repo_root

    def _write_testing_support(self, repo_root: Path) -> None:
        testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
        testing_root.mkdir(parents=True, exist_ok=True)
        templates_root = testing_root / "templates"
        templates_root.mkdir(parents=True, exist_ok=True)
        (testing_root / "benchmark_records.py").write_text(
            "\n".join(
                [
                    "from __future__ import annotations",
                    "",
                    "",
                    "def append_record(*args, **kwargs):",
                    "    del args, kwargs",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        shutil.copyfile(BENCHMARK_COMPARISON_MODULE_PATH, testing_root / "benchmark_comparison.py")
        shutil.copyfile(BENCHMARK_DASHBOARD_TEMPLATE_PATH, templates_root / "benchmark-dashboard.html")
        (testing_root / "subjects.py").write_text(
            "\n".join(
                [
                    "from __future__ import annotations",
                    "",
                    "import json",
                    "from pathlib import Path",
                    "",
                    "",
                    "def discover_perf_subject_ids(repo_root: Path) -> list[str]:",
                    '    subjects_root = repo_root / "subjects"',
                    "    if not subjects_root.is_dir():",
                    "        return []",
                    "    return sorted(",
                    "        path.parent.name",
                    '        for path in subjects_root.glob("*/subject.manifest.json")',
                    "        if path.is_file()",
                    "    )",
                    "",
                    "",
                    "def load_subject_manifest(repo_root: Path, subject_id: str) -> dict:",
                    '    manifest_path = repo_root / "subjects" / subject_id / "subject.manifest.json"',
                    '    return json.loads(manifest_path.read_text(encoding="utf-8"))',
                    "",
                ]
            ),
            encoding="utf-8",
        )

    def _write_compiled_catalog(
        self,
        repo_root: Path,
        *,
        declared_benchmarks: list[dict[str, object]],
    ) -> None:
        testing_root = repo_root / "build" / "toolchains" / "run" / "testing"
        testing_root.mkdir(parents=True, exist_ok=True)
        payload = json.dumps({"declaredBenchmarks": declared_benchmarks}, ensure_ascii=False, indent=2)
        (testing_root / "compiled_catalog.py").write_text(
            "\n".join(
                [
                    "from __future__ import annotations",
                    "",
                    "",
                    "def build_subject_declared_test_catalog(*, repo_root, subject_id, force_build=False):",
                    "    del repo_root, subject_id, force_build",
                    f"    return {payload}",
                    "",
                ]
            ),
            encoding="utf-8",
        )

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
    ) -> None:
        workspace_root = repo_root / "solutions" / "subjects" / subject_id
        managed_tests_root = workspace_root / "managed-tests"
        generated_root = managed_tests_root / "Generated"
        project_path = managed_tests_root / f"{subject_id}.DeclaredBenchmarkHost.csproj"
        generated_source_path = generated_root / "ChaosGeneratedDeclaredBenchmarks.g.cs"
        collection_path = generated_root / "declared-tests.collection.json"
        manifest_path = workspace_root / "workspace.manifest.json"

        for path in [project_path, generated_source_path]:
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

        manifest_payload: dict[str, Any] = {
            "workspaceVersion": 2,
            "kind": "subject-workspace",
            "subjectId": subject_id,
            "defaultMatrixId": matrix_id,
            "managedProjects": [],
            "managedTestProjects": [
                {
                    "projectId": f"managed-test/{subject_id}/benchmark-host",
                    "projectPath": project_path.relative_to(repo_root).as_posix(),
                    "assemblyName": f"{subject_id}.DeclaredBenchmarkHost",
                    "hostKind": "benchmark-host",
                "collectionPath": collection_path.relative_to(repo_root).as_posix(),
                    "generatedSourcePath": generated_source_path.relative_to(repo_root).as_posix(),
                }
            ],
            "nativeProjects": [],
            "nativeTestProjects": [],
            "matrices": [
                {
                    "matrixId": matrix_id,
                    "goalIds": ["perf.release"],
                    "hostPlatform": "windows-x64",
                    "targetPlatform": "windows-x64",
                    "managedTestProjectIds": [f"managed-test/{subject_id}/benchmark-host"],
                    "nativeTestProjectIds": [],
                }
            ],
        }
        manifest_path.parent.mkdir(parents=True, exist_ok=True)
        manifest_path.write_text(json.dumps(manifest_payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    def _write_subject_manifest(
        self,
        repo_root: Path,
        *,
        subject_id: str,
        display_name: str | None = None,
        supported_modes: list[str],
        host_platform: str = "windows-x64",
        workload_entry: str | None = None,
    ) -> None:
        subject_root = repo_root / "subjects" / subject_id
        subject_root.mkdir(parents=True, exist_ok=True)

        pipelines = []
        matrices = []
        for mode in supported_modes:
            pipeline_id = f"{mode}-benchmark"
            stage_kind = {
                "managed": "runtime-perf-collect",
                "native": "native-runtime-perf",
                "interpreter": "interpreter-runtime-perf",
            }[mode]
            runtime_profile = {
                "managed": "managed-perf-release",
                "native": "native-perf-release",
                "interpreter": "interpreter-perf-release",
            }[mode]
            pipelines.append(
                {
                    "pipelineId": pipeline_id,
                    "stages": [
                        {
                            "stageId": f"{mode}-stage",
                            "kind": stage_kind,
                            "scope": "matrix",
                            "bucket": "runtime",
                            "dependsOn": [],
                        }
                    ],
                }
            )
            matrices.append(
                {
                    "matrixId": f"{host_platform.split('-')[0]}-{mode}-perf",
                    "pipelineId": pipeline_id,
                    "supportedGoals": ["perf.release"],
                    "executionContext": {
                        "hostPlatform": host_platform,
                        "targetPlatform": host_platform,
                        "runtimeProfile": runtime_profile,
                    },
                }
            )

        manifest = {
            "subjectId": subject_id,
            "displayName": display_name or subject_id,
            "executionPipelines": pipelines,
            "environmentMatrices": matrices,
        }
        if workload_entry:
            manifest["workloadEntry"] = workload_entry
        (subject_root / "subject.manifest.json").write_text(
            json.dumps(manifest, ensure_ascii=False, indent=2),
            encoding="utf-8",
        )

    def _write_record(
        self,
        repo_root: Path,
        *,
        subject_id: str,
        mode: str,
        mean_duration_ms: float,
        mean_ops_per_second: float | None = None,
        benchmark_case: dict[str, object] | None = None,
        recorded_at: str = "2026-04-13T08:00:00+00:00",
        git_commit: str = "abc123",
    ) -> None:
        records_path = repo_root / "subjects" / subject_id / "benchmark-records" / "records.jsonl"
        records_path.parent.mkdir(parents=True, exist_ok=True)
        payload = {
            "subject": subject_id,
            "mode": mode,
            "platform": "windows",
            "device": {
                "id": "windows-x64-test-device",
                "name": "Windows Test Device",
            },
            "metrics": {
                "meanDurationMs": mean_duration_ms,
                "meanOpsPerSecond": 1000 if mean_ops_per_second is None else mean_ops_per_second,
            },
            "recordedAt": recorded_at,
            "gitCommit": git_commit,
        }
        if benchmark_case is not None:
            payload["benchmarkCase"] = dict(benchmark_case)
        with records_path.open("a", encoding="utf-8") as handle:
            handle.write(json.dumps(payload, ensure_ascii=False) + "\n")
