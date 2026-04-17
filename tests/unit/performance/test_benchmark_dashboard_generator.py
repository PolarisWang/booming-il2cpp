from __future__ import annotations

import importlib.util
import json
import shutil
import sys
import unittest
import uuid
from pathlib import Path
from typing import Any


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


class BenchmarkDashboardGeneratorTests(unittest.TestCase):
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
            "recordedAt": "2026-04-13T08:00:00+00:00",
            "gitCommit": "abc123",
        }
        if benchmark_case is not None:
            payload["benchmarkCase"] = dict(benchmark_case)
        with records_path.open("a", encoding="utf-8") as handle:
            handle.write(json.dumps(payload, ensure_ascii=False) + "\n")

    def test_update_docs_removes_stale_subject_json_and_rewrites_dashboard_payload(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_cleanup",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        subjects_doc_root = docs_root / "subjects"
        stale_subject_path = subjects_doc_root / "BenchAllocation.json"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
            )
            self._write_record(repo_root, subject_id="SolutionCorePack", mode="managed", mean_duration_ms=2.5)
            self._write_record(repo_root, subject_id="SolutionCorePack", mode="native", mean_duration_ms=1.25)

            subjects_doc_root.mkdir(parents=True, exist_ok=True)
            stale_subject_path.write_text('{"stale":true}\n', encoding="utf-8")

            generator_module.update_docs(repo_root)

            self.assertFalse(stale_subject_path.exists())
            generated_subject_paths = sorted(path.name for path in subjects_doc_root.glob("*.json"))
            self.assertEqual(["SolutionCorePack.json"], generated_subject_paths)

            subject_payload = json.loads((subjects_doc_root / "SolutionCorePack.json").read_text(encoding="utf-8"))
            self.assertIn("latestByDevice", subject_payload)
            self.assertIn("windows-x64-test-device", subject_payload["latestByDevice"])

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            self.assertEqual({"SolutionCorePack"}, set(overview_payload["subjects"]))

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("SolutionCorePack", dashboard_html)
            self.assertNotIn("BenchAllocation", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_overview_distinguishes_recorded_missing_and_unsupported_modes(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_mode_status",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="HotUpdateHostPack",
                supported_modes=["managed"],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                supported_modes=["managed", "native", "interpreter"],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["native"],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SparsePack",
                supported_modes=["managed", "native"],
            )

            self._write_record(repo_root, subject_id="HotUpdateHostPack", mode="managed", mean_duration_ms=57.5)
            self._write_record(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                mode="managed",
                mean_duration_ms=2.287,
                mean_ops_per_second=2624.106,
            )
            self._write_record(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                mode="native",
                mean_duration_ms=0.0,
                mean_ops_per_second=60000000.0,
            )
            self._write_record(
                repo_root,
                subject_id="MixedExecutionFeaturePack",
                mode="interpreter",
                mean_duration_ms=5.176,
                mean_ops_per_second=1226.199,
            )
            self._write_record(repo_root, subject_id="SolutionCorePack", mode="native", mean_duration_ms=5.846)
            self._write_record(repo_root, subject_id="SparsePack", mode="managed", mean_duration_ms=8.0)

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            summary = overview_payload["summary"]
            self.assertEqual(4, summary["subjectCount"])
            self.assertEqual(3, summary["fullyRecordedCount"])
            self.assertEqual(1, summary["subjectsWithMissingRecords"])
            self.assertEqual(6, summary["recordedModeCount"])
            self.assertEqual(1, summary["missingModeCount"])
            self.assertEqual(5, summary["unsupportedModeCount"])

            subjects_payload = overview_payload["subjects"]

            hot_update = subjects_payload["HotUpdateHostPack"]
            self.assertEqual(["managed"], hot_update["supportedModes"])
            self.assertEqual(["managed"], hot_update["recordedModes"])
            self.assertEqual([], hot_update["missingModes"])
            self.assertEqual(["native", "interpreter"], hot_update["unsupportedModes"])
            self.assertEqual("recorded", hot_update["modeStatus"]["managed"]["status"])
            self.assertEqual("unsupported", hot_update["modeStatus"]["native"]["status"])
            self.assertEqual("unsupported", hot_update["modeStatus"]["interpreter"]["status"])

            mixed = subjects_payload["MixedExecutionFeaturePack"]
            self.assertEqual(["managed", "native", "interpreter"], mixed["supportedModes"])
            self.assertEqual([], mixed["missingModes"])
            self.assertEqual("recorded", mixed["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual(2.287, mixed["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2624.106, mixed["keyMetrics"]["managedBaseline"]["opsPerSecond"])
            self.assertEqual("recorded", mixed["keyMetrics"]["relativeToManaged"]["native"]["status"])
            self.assertEqual("faster", mixed["keyMetrics"]["relativeToManaged"]["native"]["direction"])
            self.assertEqual("throughput", mixed["keyMetrics"]["relativeToManaged"]["native"]["ratio"]["basis"])
            self.assertGreater(mixed["keyMetrics"]["relativeToManaged"]["native"]["ratio"]["value"], 1000)
            self.assertEqual("recorded", mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["status"])
            self.assertEqual("slower", mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["direction"])
            self.assertEqual("latency", mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["ratio"]["basis"])
            self.assertEqual(2.26, mixed["keyMetrics"]["relativeToManaged"]["interpreter"]["ratio"]["value"])
            self.assertEqual("throughput", mixed["keyMetrics"]["nativeSpeedup"]["basis"])
            self.assertGreater(mixed["keyMetrics"]["nativeSpeedup"]["value"], 1000)
            self.assertEqual("latency", mixed["keyMetrics"]["interpreterOverhead"]["basis"])
            self.assertEqual("recorded", mixed["modeStatus"]["native"]["status"])

            solution = subjects_payload["SolutionCorePack"]
            self.assertEqual(["native"], solution["supportedModes"])
            self.assertEqual(["managed", "interpreter"], solution["unsupportedModes"])
            self.assertEqual("unsupported", solution["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual(
                "baseline-unavailable",
                solution["keyMetrics"]["relativeToManaged"]["native"]["status"],
            )

            sparse = subjects_payload["SparsePack"]
            self.assertEqual(["managed", "native"], sparse["supportedModes"])
            self.assertEqual(["native"], sparse["missingModes"])
            self.assertEqual("recorded", sparse["keyMetrics"]["managedBaseline"]["status"])
            self.assertEqual("missing", sparse["keyMetrics"]["relativeToManaged"]["native"]["status"])
            self.assertEqual("missing", sparse["modeStatus"]["native"]["status"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn('"missingModeCount": 1', dashboard_html)
            self.assertIn('"unsupportedModes": ["native", "interpreter"]', dashboard_html)
            self.assertIn("meanOpsPerSecond", dashboard_html)
            self.assertIn("Managed Baseline", dashboard_html)
            self.assertIn("Native Latency vs Managed", dashboard_html)
            self.assertIn("Interpreter Latency vs Managed", dashboard_html)
            self.assertIn("Summary Workload", dashboard_html)
            self.assertNotIn('value="opsPerSecond"', dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_keeps_subject_summary_and_exposes_case_level_benchmark_records(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_case_detail",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=8.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=4.0,
                mean_ops_per_second=2000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.0,
                mean_ops_per_second=10000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=6.0,
                mean_ops_per_second=2500.0,
                benchmark_case={
                    "stableId": "dispatch",
                    "alias": "dispatch-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()",
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=3.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "dispatch",
                    "alias": "dispatch-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()",
                },
            )

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            solution_summary = overview_payload["subjects"]["SolutionCorePack"]
            self.assertEqual(8.0, solution_summary["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, solution_summary["keyMetrics"]["nativeSpeedup"]["value"])

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({"arith", "dispatch"}, set(case_payload))
            self.assertEqual("arithmetic-bench", case_payload["arith"]["displayName"])
            self.assertEqual(2.0, case_payload["arith"]["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, case_payload["arith"]["keyMetrics"]["nativeSpeedup"]["value"])
            self.assertEqual("dispatch-bench", case_payload["dispatch"]["displayName"])
            self.assertEqual(6.0, case_payload["dispatch"]["keyMetrics"]["managedBaseline"]["durationMs"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Benchmark Case Matrix", dashboard_html)
            self.assertIn("benchmarkCasesByDevice", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_uses_workspace_catalog_when_compiled_catalog_is_unavailable(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_workspace_catalog",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        testing_root = repo_root / "build" / "toolchains" / "run" / "testing"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
            )
            self._write_workspace_benchmark_fixture(
                repo_root,
                subject_id="SolutionCorePack",
                stable_id="solution-core::arith",
                alias="arithmetic-bench",
                entry_index=9,
                assembly_name="CoreRuntimeBenchmarks",
                declaring_type="CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                method_name="RunWorkload",
                method_signature="RunWorkload()",
            )
            (testing_root / "compiled_catalog.py").write_text(
                "\n".join(
                    [
                        "from __future__ import annotations",
                        "",
                        "",
                        "def build_subject_declared_test_catalog(*, repo_root, subject_id, force_build=False):",
                        "    del repo_root, subject_id, force_build",
                        "    raise RuntimeError('compiled collection unavailable')",
                        "",
                    ]
                ),
                encoding="utf-8",
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "solution-core::arith",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.0,
                mean_ops_per_second=10000.0,
                benchmark_case={
                    "stableId": "solution-core::arith",
                    "workloadEntry": "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["solution-core::arith"]
            self.assertEqual("arithmetic-bench", case_payload["displayName"])
            self.assertEqual(["managed", "native"], case_payload["supportedModes"])
            self.assertEqual(2.0, case_payload["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, case_payload["keyMetrics"]["nativeSpeedup"]["value"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_records_subject_mode_when_data_exists_even_if_manifest_excludes_mode(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_subject_recorded_modes",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed"],
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=8.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=4.0,
                mean_ops_per_second=2000.0,
            )

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            subject_payload = overview_payload["subjects"]["SolutionCorePack"]
            self.assertEqual(["managed", "native"], subject_payload["supportedModes"])
            self.assertEqual(["managed", "native"], subject_payload["recordedModes"])
            self.assertEqual([], subject_payload["missingModes"])
            self.assertEqual(["interpreter"], subject_payload["unsupportedModes"])
            self.assertEqual("recorded", subject_payload["modeStatus"]["native"]["status"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn('"supportedModes":["managed","native"]', dashboard_html.replace(" ", ""))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_records_case_mode_when_data_exists_even_if_record_metadata_excludes_mode(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_case_supported_modes",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=8.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=4.0,
                mean_ops_per_second=2000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=6.0,
                mean_ops_per_second=2500.0,
                benchmark_case={
                    "stableId": "allocation",
                    "alias": "allocation-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/AllocationBenchmarkEntry::RunWorkload()",
                    "supportedModes": ["managed"],
                    "category": 3,
                    "metrics": 3,
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=3.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "allocation",
                    "alias": "allocation-bench",
                    "workloadEntry": "CoreRuntimeBenchmarks/AllocationBenchmarkEntry::RunWorkload()",
                    "supportedModes": ["managed"],
                    "category": 3,
                    "metrics": 3,
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["allocation"]
            self.assertEqual(["managed", "native"], case_payload["supportedModes"])
            self.assertEqual(["managed", "native"], case_payload["recordedModes"])
            self.assertEqual([], case_payload["missingModes"])
            self.assertEqual(["interpreter"], case_payload["unsupportedModes"])
            self.assertEqual("recorded", case_payload["modeStatus"]["native"]["status"])
            self.assertEqual(0, subject_payload["caseSummaryByDevice"]["windows-x64-test-device"]["managedOnlyCaseCount"])
            self.assertEqual(1, subject_payload["caseSummaryByDevice"]["windows-x64-test-device"]["crossModeCaseCount"])
            self.assertEqual(2.0, case_payload["keyMetrics"]["nativeSpeedup"]["value"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn('"crossModeCaseCount": 1', dashboard_html)
            self.assertIn('"supportedModes":["managed","native"]', dashboard_html.replace(" ", ""))
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_exposes_summary_workload_contract_for_dashboard_layout(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_summary_workload",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=8.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=4.0,
                mean_ops_per_second=2000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "CoreRuntimeBenchmarks",
                    "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "supportedModes": ["managed", "native"],
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.0,
                mean_ops_per_second=10000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "CoreRuntimeBenchmarks",
                    "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "supportedModes": ["managed", "native"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            self.assertEqual(workload_entry, subject_payload["summaryWorkloadEntry"])
            self.assertEqual("arithmetic-bench", subject_payload["summaryBenchmarkCase"]["displayName"])
            self.assertEqual("CoreRuntimeBenchmarks", subject_payload["summaryBenchmarkCase"]["assemblyName"])

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            self.assertEqual(
                "arithmetic-bench",
                overview_payload["subjects"]["SolutionCorePack"]["summaryBenchmarkCase"]["displayName"],
            )

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Summary Workload", dashboard_html)
            self.assertIn("Solution Slice Breakdown", dashboard_html)
            self.assertIn("Latency vs Managed", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_exposes_archetype_and_hot_update_case_metadata_for_dashboard_layout(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_case_contract_labels",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_subject_manifest(
                repo_root,
                subject_id="HotUpdateHostPack",
                supported_modes=["managed"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="HotUpdateHostPack",
                mode="managed",
                mean_duration_ms=42.0,
                mean_ops_per_second=250.0,
            )
            self._write_record(
                repo_root,
                subject_id="HotUpdateHostPack",
                mode="managed",
                mean_duration_ms=18.5,
                mean_ops_per_second=540.0,
                benchmark_case={
                    "stableId": "hot-update-load",
                    "alias": "hot-update-load-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "HotUpdateHostPack",
                    "declaringType": "HotUpdateHostPack.HotUpdateLoadBenchmarkEntry",
                    "category": 4,
                    "metrics": 1,
                    "requires": 32,
                    "archetype": 7,
                    "hotUpdateCapability": 17,
                    "supportedModes": ["managed"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "HotUpdateHostPack.json").read_text(encoding="utf-8"))
            self.assertEqual(
                "Skeleton Patch Solution",
                subject_payload["summaryBenchmarkCase"]["archetypeLabel"],
            )
            self.assertEqual(
                ["Package Load", "Patch Integrity"],
                subject_payload["summaryBenchmarkCase"]["hotUpdateCapabilityLabels"],
            )
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["hot-update-load"]
            self.assertEqual("Hot Update", case_payload["categoryLabel"])
            self.assertEqual("Skeleton Patch Solution", case_payload["archetypeLabel"])
            self.assertEqual(["Package Load", "Patch Integrity"], case_payload["hotUpdateCapabilityLabels"])
            self.assertEqual(["Hot Update"], case_payload["requirementLabels"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Solution Archetype", dashboard_html)
            self.assertIn("Hot-Update Capability", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_exposes_capability_contract_and_mode_reason_codes_for_declared_cases(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_capability_contracts",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "InteropBenchmarks/NativeInteropBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": "interop",
                        "alias": "native-interop-bench",
                        "assemblyName": "InteropBenchmarks",
                        "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "capabilityFamily": 7,
                        "capabilityItem": 31,
                        "archetype": 1,
                        "hotUpdateCapability": 0,
                        "requires": 0,
                        "metrics": 3,
                        "modes": 3,
                        "warmupCount": 2,
                        "iterationCount": 5,
                        "invocationCount": 10,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=9.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=3.0,
                mean_ops_per_second=3000.0,
                benchmark_case={
                    "stableId": "interop",
                    "alias": "native-interop-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "InteropBenchmarks",
                    "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "category": 1,
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                    "metrics": 3,
                    "modes": 3,
                },
            )

            generator_module.update_docs(repo_root)

            overview_payload = json.loads((docs_root / "overview.json").read_text(encoding="utf-8"))
            subject_overview = overview_payload["subjects"]["SolutionCorePack"]
            self.assertEqual("missing-record", subject_overview["modeStatus"]["native"]["reasonCode"])
            self.assertEqual(
                "unsupported-by-contract",
                subject_overview["modeStatus"]["interpreter"]["reasonCode"],
            )

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]["interop"]
            self.assertEqual(7, case_payload["capabilityFamily"])
            self.assertEqual("Interop And Marshaling", case_payload["capabilityFamilyLabel"])
            self.assertEqual(31, case_payload["capabilityItem"])
            self.assertEqual("Native Call Interop", case_payload["capabilityItemLabel"])
            self.assertEqual("SolutionCorePack", case_payload["ownerSubjectId"])
            self.assertEqual([1, 5], case_payload["supportStates"])
            self.assertEqual(["NativeGenerated", "ExternalRuntime"], case_payload["supportStateLabels"])
            self.assertTrue(case_payload["proofRequired"])
            self.assertTrue(case_payload["benchmarkRequired"])
            self.assertEqual("missing-record", case_payload["modeStatus"]["native"]["reasonCode"])
            self.assertEqual(
                "Declared by case contract, but no benchmark record was found.",
                case_payload["modeStatus"]["native"]["reasonLabel"],
            )
            self.assertEqual("unsupported-by-contract", case_payload["modeStatus"]["interpreter"]["reasonCode"])

            summary_case = subject_payload["summaryBenchmarkCase"]
            self.assertEqual("SolutionCorePack", summary_case["ownerSubjectId"])
            self.assertEqual("Native Call Interop", summary_case["capabilityItemLabel"])
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_dashboard_html_surfaces_contract_ownership_support_state_and_reason_copy(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_contract_reason_html",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "InteropBenchmarks/NativeInteropBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": "interop",
                        "alias": "native-interop-bench",
                        "assemblyName": "InteropBenchmarks",
                        "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "capabilityFamily": 7,
                        "capabilityItem": 31,
                        "archetype": 1,
                        "hotUpdateCapability": 0,
                        "requires": 0,
                        "metrics": 3,
                        "modes": 3,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=4.0,
                mean_ops_per_second=4000.0,
                benchmark_case={
                    "stableId": "interop",
                    "alias": "native-interop-bench",
                    "workloadEntry": workload_entry,
                    "assemblyName": "InteropBenchmarks",
                    "declaringType": "InteropBenchmarks.NativeInteropBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "category": 1,
                    "capabilityFamily": 7,
                    "capabilityItem": 31,
                    "metrics": 3,
                    "modes": 3,
                },
            )

            generator_module.update_docs(repo_root)

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("Owner Subject", dashboard_html)
            self.assertIn("Capability", dashboard_html)
            self.assertIn("Support State", dashboard_html)
            self.assertIn("Requirement Contract", dashboard_html)
            self.assertIn("Benchmark Required", dashboard_html)
            self.assertIn("Proof Required", dashboard_html)
            self.assertIn("unsupported-by-contract", dashboard_html)
            self.assertIn("missing-record", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_filters_out_historical_cases_not_in_current_declared_catalog(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_declared_case_filter",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": "arith",
                        "alias": "arithmetic-bench",
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "metrics": 3,
                        "modes": 3,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=8.0,
                mean_ops_per_second=1000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=4.0,
                mean_ops_per_second=2000.0,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=2.0,
                mean_ops_per_second=5000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": workload_entry,
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.0,
                mean_ops_per_second=10000.0,
                benchmark_case={
                    "stableId": "arith",
                    "alias": "arithmetic-bench",
                    "workloadEntry": workload_entry,
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=7.5,
                mean_ops_per_second=900.0,
                benchmark_case={
                    "stableId": "legacy-dispatch",
                    "alias": "legacy-dispatch-bench",
                    "workloadEntry": "PerformanceFeaturePack/LegacyDispatchBenchmarkEntry::RunWorkload()",
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({"arith"}, set(case_payload))
            self.assertEqual({"arith"}, set(subject_payload["declaredBenchmarkCases"]))

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn("arithmetic-bench", dashboard_html)
            self.assertNotIn("legacy-dispatch-bench", dashboard_html)
            self.assertNotIn("legacy-dispatch", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_rekeys_legacy_record_identity_into_declared_case_by_alias(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_declared_case_rekey",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        case_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": case_id,
                        "alias": "arithmetic-bench",
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "metrics": 3,
                        "modes": 3,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=3.0,
                mean_ops_per_second=3000.0,
                benchmark_case={
                    "stableId": "SolutionCorePack::PerformanceFeaturePack::PerformanceFeaturePack.ArithmeticBenchmarkEntry::RunWorkload()",
                    "alias": "arithmetic-bench",
                    "displayName": "arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.5,
                mean_ops_per_second=6000.0,
                benchmark_case={
                    "stableId": "SolutionCorePack::PerformanceFeaturePack::PerformanceFeaturePack.ArithmeticBenchmarkEntry::RunWorkload()",
                    "alias": "arithmetic-bench",
                    "displayName": "arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({case_id}, set(case_payload))
            self.assertEqual(["managed", "native"], case_payload[case_id]["recordedModes"])
            self.assertEqual(workload_entry, case_payload[case_id]["workloadEntry"])
            self.assertEqual("CoreRuntimeBenchmarks", case_payload[case_id]["assemblyName"])
            self.assertEqual(3.0, case_payload[case_id]["keyMetrics"]["managedBaseline"]["durationMs"])
            self.assertEqual(2.0, case_payload[case_id]["keyMetrics"]["nativeSpeedup"]["value"])

            self.assertEqual(case_id, subject_payload["summaryBenchmarkCase"]["caseId"])
            self.assertEqual(workload_entry, subject_payload["summaryBenchmarkCase"]["workloadEntry"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn(case_id, dashboard_html)
            self.assertNotIn("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)

    def test_update_docs_rekeys_legacy_record_identity_into_declared_case_by_entry_index(self) -> None:
        generator_module = load_module(
            BENCHMARK_DASHBOARD_GENERATOR_MODULE_PATH,
            "chaos_benchmark_dashboard_generator_declared_case_entry_index_rekey",
        )
        repo_root = self._make_repo_root()
        docs_root = repo_root / "docs" / "benchmark"
        workload_entry = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()"
        case_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry::RunWorkload()"

        try:
            self._write_testing_support(repo_root)
            self._write_compiled_catalog(
                repo_root,
                declared_benchmarks=[
                    {
                        "stableId": case_id,
                        "entryIndex": 11,
                        "alias": "arithmetic-bench",
                        "assemblyName": "CoreRuntimeBenchmarks",
                        "declaringType": "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                        "methodName": "RunWorkload",
                        "methodSignature": "RunWorkload()",
                        "category": 1,
                        "metrics": 3,
                        "modes": 3,
                    }
                ],
            )
            self._write_subject_manifest(
                repo_root,
                subject_id="SolutionCorePack",
                supported_modes=["managed", "native"],
                workload_entry=workload_entry,
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="managed",
                mean_duration_ms=3.0,
                mean_ops_per_second=3000.0,
                benchmark_case={
                    "stableId": "legacy/arithmetic",
                    "entryIndex": 11,
                    "alias": "legacy-arithmetic-bench",
                    "displayName": "legacy-arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )
            self._write_record(
                repo_root,
                subject_id="SolutionCorePack",
                mode="native",
                mean_duration_ms=1.5,
                mean_ops_per_second=6000.0,
                benchmark_case={
                    "stableId": "legacy/arithmetic",
                    "entryIndex": 11,
                    "alias": "legacy-arithmetic-bench",
                    "displayName": "legacy-arithmetic-bench",
                    "workloadEntry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
                    "assemblyName": "PerformanceFeaturePack",
                    "declaringType": "PerformanceFeaturePack.ArithmeticBenchmarkEntry",
                    "methodName": "RunWorkload",
                    "methodSignature": "RunWorkload()",
                    "supportedModes": ["managed", "native"],
                },
            )

            generator_module.update_docs(repo_root)

            subject_payload = json.loads((docs_root / "subjects" / "SolutionCorePack.json").read_text(encoding="utf-8"))
            case_payload = subject_payload["benchmarkCasesByDevice"]["windows-x64-test-device"]
            self.assertEqual({case_id}, set(case_payload))
            self.assertEqual(["managed", "native"], case_payload[case_id]["recordedModes"])
            self.assertEqual(11, case_payload[case_id]["entryIndex"])
            self.assertEqual(workload_entry, case_payload[case_id]["workloadEntry"])
            self.assertEqual("CoreRuntimeBenchmarks", case_payload[case_id]["assemblyName"])

            dashboard_html = (docs_root / "dashboard.html").read_text(encoding="utf-8")
            self.assertIn(case_id, dashboard_html)
            self.assertNotIn("legacy/arithmetic", dashboard_html)
            self.assertNotIn("legacy-arithmetic-bench", dashboard_html)
        finally:
            shutil.rmtree(repo_root, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
