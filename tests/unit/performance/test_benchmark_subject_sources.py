from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
BENCH_SUBJECT_ROOT = REPO_ROOT / "subjects"
SHARED_PERF_PROJECT_PATH = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
BENCHMARK_FEATURE_PACK_ROOT = BENCH_SUBJECT_ROOT / "PerformanceFeaturePack"
HOT_UPDATE_HOST_PACK_ROOT = BENCH_SUBJECT_ROOT / "HotUpdateHostPack"
MIXED_EXECUTION_FEATURE_PACK_ROOT = BENCH_SUBJECT_ROOT / "MixedExecutionFeaturePack"
EXPECTED_HARNESS_ITERATIONS = {
    "PerformanceFeaturePack": 2,
    "MixedExecutionFeaturePack": 6,
}
CANONICAL_PERF_SUBJECTS = {
    "PerformanceFeaturePack": {
        "manifest_path": BENCHMARK_FEATURE_PACK_ROOT / "subject.manifest.json",
        "source_path": "subjects/PerformanceFeaturePack/source/PerformanceFeaturePack.csproj",
        "workload_entry": "PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()",
    },
    "MixedExecutionFeaturePack": {
        "manifest_path": MIXED_EXECUTION_FEATURE_PACK_ROOT / "subject.manifest.json",
        "source_path": "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
        "workload_entry": "MixedExecutionFeaturePack/MixedExecutionBenchmarkEntry::RunWorkload()",
    },
}


class BenchmarkSubjectSourceTests(unittest.TestCase):
    def test_benchmark_feature_pack_declares_first_batch_runtime_benchmarks(self) -> None:
        manifest_path = BENCHMARK_FEATURE_PACK_ROOT / "subject.manifest.json"
        project_path = BENCHMARK_FEATURE_PACK_ROOT / "source" / "PerformanceFeaturePack.csproj"
        arithmetic_path = BENCHMARK_FEATURE_PACK_ROOT / "source" / "ArithmeticBenchmark.cs"
        allocation_path = BENCHMARK_FEATURE_PACK_ROOT / "source" / "AllocationBenchmark.cs"
        dispatch_path = BENCHMARK_FEATURE_PACK_ROOT / "source" / "DispatchBenchmark.cs"
        generic_path = BENCHMARK_FEATURE_PACK_ROOT / "source" / "GenericBenchmark.cs"

        self.assertTrue(manifest_path.is_file(), msg=f"missing manifest: {manifest_path}")
        self.assertTrue(project_path.is_file(), msg=f"missing project: {project_path}")
        self.assertTrue(arithmetic_path.is_file(), msg=f"missing arithmetic slice: {arithmetic_path}")
        self.assertTrue(allocation_path.is_file(), msg=f"missing allocation slice: {allocation_path}")
        self.assertTrue(dispatch_path.is_file(), msg=f"missing dispatch slice: {dispatch_path}")
        self.assertTrue(generic_path.is_file(), msg=f"missing generic slice: {generic_path}")

        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        arithmetic_source = arithmetic_path.read_text(encoding="utf-8")
        allocation_source = allocation_path.read_text(encoding="utf-8")
        dispatch_source = dispatch_path.read_text(encoding="utf-8")
        generic_source = generic_path.read_text(encoding="utf-8")

        self.assertEqual("PerformanceFeaturePack", manifest["subjectId"])
        self.assertEqual("subjects/PerformanceFeaturePack/source/PerformanceFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("PerformanceFeaturePack", manifest["displayName"])
        self.assertEqual("PerformanceFeaturePack/ArithmeticBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])

        self.assertIn("ChaosBenchmark(", arithmetic_source)
        self.assertIn('Alias = "arithmetic-bench"', arithmetic_source)
        self.assertIn("ChaosBenchmarkCategory.RuntimeDispatch", arithmetic_source)
        self.assertIn("ChaosMetric.WallClockUs", arithmetic_source)

        self.assertIn("ChaosBenchmark(", allocation_source)
        self.assertIn('Alias = "allocation-bench"', allocation_source)
        self.assertIn("ChaosBenchmarkCategory.Allocation", allocation_source)
        self.assertIn("ChaosMetric.ManagedAllocBytes", allocation_source)

        self.assertIn("ChaosBenchmark(", dispatch_source)
        self.assertIn('Alias = "dispatch-bench"', dispatch_source)
        self.assertIn("ChaosBenchmarkCategory.RuntimeDispatch", dispatch_source)
        self.assertIn("DispatchShape[] shapes", dispatch_source)

        self.assertIn("ChaosBenchmark(", generic_source)
        self.assertIn('Alias = "generic-bench"', generic_source)
        self.assertIn("ChaosRuntimeFeature.GenericSharing", generic_source)
        self.assertIn("Dictionary<string, int>", generic_source)

    def test_all_bench_subjects_keep_perf_analysis_outside_subject_source(self) -> None:
        self.assertTrue((REPO_ROOT / SHARED_PERF_PROJECT_PATH).is_file())

        for subject_id, spec in CANONICAL_PERF_SUBJECTS.items():
            manifest_path = spec["manifest_path"]
            self.assertTrue(manifest_path.is_file(), msg=f"missing canonical perf manifest: {manifest_path}")
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            workload_entry = str(spec["workload_entry"])

            self.assertEqual(str(spec["source_path"]), manifest["source"]["path"])
            self.assertEqual(workload_entry, manifest["workloadEntry"])
            self.assertEqual("require", manifest["testDeclarationMode"])
            self.assertEqual(EXPECTED_HARNESS_ITERATIONS[subject_id], manifest["validation"]["perf"]["harnessIterations"])
            self.assertEqual(SHARED_PERF_PROJECT_PATH, manifest["validation"]["perf"]["project"])
            self.assertNotIn("Program::Main(System.String[])", json.dumps(manifest, ensure_ascii=False))

            for pipeline in manifest["executionPipelines"]:
                pipeline_id = pipeline["pipelineId"]
                stage_kinds = {stage["kind"] for stage in pipeline["stages"]}
                if pipeline_id == "native-benchmark":
                    self.assertIn("generated-native-aot", stage_kinds, msg=subject_id)
                    self.assertNotIn("generated-native-proof", stage_kinds, msg=subject_id)

            for matrix in manifest["environmentMatrices"]:
                if matrix["pipelineId"] == "native-benchmark":
                    self.assertEqual(workload_entry, matrix["source"]["entry"], msg=subject_id)

    def test_hot_update_host_pack_declares_hot_update_unit_and_benchmark_slices(self) -> None:
        manifest_path = HOT_UPDATE_HOST_PACK_ROOT / "subject.manifest.json"
        project_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "HotUpdateHostPack.csproj"
        skeleton_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "HotUpdateSkeletonProofEntry.cs"
        replacement_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "MethodReplacementProofEntry.cs"
        rollback_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "VersionRollbackProofEntry.cs"
        dispatch_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "HotUpdateDispatchBenchmark.cs"
        load_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "HotUpdateLoadBenchmark.cs"
        roundtrip_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "HotUpdateRoundtripBenchmark.cs"

        for path in [
            manifest_path,
            project_path,
            skeleton_path,
            replacement_path,
            rollback_path,
            dispatch_path,
            load_path,
            roundtrip_path,
        ]:
            self.assertTrue(path.is_file(), msg=f"missing hot-update host pack asset: {path}")

        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        skeleton_source = skeleton_path.read_text(encoding="utf-8")
        replacement_source = replacement_path.read_text(encoding="utf-8")
        rollback_source = rollback_path.read_text(encoding="utf-8")
        dispatch_source = dispatch_path.read_text(encoding="utf-8")
        load_source = load_path.read_text(encoding="utf-8")
        roundtrip_source = roundtrip_path.read_text(encoding="utf-8")

        self.assertEqual("HotUpdateHostPack", manifest["subjectId"])
        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj", manifest["source"]["path"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("HotUpdateHostPack/Program::Main()", manifest["source"]["entry"])

        self.assertIn("ChaosUnitTest(", skeleton_source)
        self.assertIn('Alias = "hot-update-skeleton-proof"', skeleton_source)
        self.assertIn("ChaosUnitCategory.HotUpdateContract", skeleton_source)
        self.assertIn("ChaosRuntimeFeature.HotUpdate", skeleton_source)

        self.assertIn("ChaosUnitTest(", replacement_source)
        self.assertIn('Alias = "method-replacement-proof"', replacement_source)
        self.assertIn("ChaosUnitCategory.HotUpdateContract", replacement_source)

        self.assertIn("ChaosUnitTest(", rollback_source)
        self.assertIn('Alias = "version-rollback-proof"', rollback_source)
        self.assertIn("ChaosUnitCategory.HotUpdateContract", rollback_source)

        self.assertIn("ChaosBenchmark(", dispatch_source)
        self.assertIn('Alias = "hot-update-dispatch-bench"', dispatch_source)
        self.assertIn("ChaosBenchmarkCategory.HotUpdate", dispatch_source)
        self.assertIn("ChaosRuntimeFeature.HotUpdate", dispatch_source)

        self.assertIn("ChaosBenchmark(", load_source)
        self.assertIn('Alias = "hot-update-load-bench"', load_source)
        self.assertIn("ChaosBenchmarkCategory.HotUpdate", load_source)
        self.assertIn("LoadPackage", load_source)

        self.assertIn("ChaosBenchmark(", roundtrip_source)
        self.assertIn('Alias = "hot-update-roundtrip-bench"', roundtrip_source)
        self.assertIn("ChaosBenchmarkCategory.HotUpdate", roundtrip_source)
        self.assertIn("BridgeDispatcher", roundtrip_source)

    def test_mixed_execution_feature_pack_declares_mixed_interpreter_unit_and_benchmark_slices(self) -> None:
        manifest_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "subject.manifest.json"
        project_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "MixedExecutionFeaturePack.csproj"
        proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "MixedExecutionProofEntry.cs"
        lowering_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "InterpreterLoweringProofEntry.cs"
        benchmark_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "MixedExecutionBenchmark.cs"
        arithmetic_support_project_path = (
            MIXED_EXECUTION_FEATURE_PACK_ROOT
            / "source"
            / "InterpreterArithmeticProof"
            / "InterpreterArithmeticProof.csproj"
        )

        for path in [
            manifest_path,
            project_path,
            proof_path,
            lowering_path,
            benchmark_path,
            arithmetic_support_project_path,
        ]:
            self.assertTrue(path.is_file(), msg=f"missing mixed execution feature pack asset: {path}")

        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        proof_source = proof_path.read_text(encoding="utf-8")
        lowering_source = lowering_path.read_text(encoding="utf-8")
        benchmark_source = benchmark_path.read_text(encoding="utf-8")

        self.assertEqual("MixedExecutionFeaturePack", manifest["subjectId"])
        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", manifest["source"]["path"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])

        self.assertIn("ChaosUnitTest(", proof_source)
        self.assertIn('Alias = "mixed-execution-proof"', proof_source)
        self.assertIn("ChaosUnitCategory.RuntimeContract", proof_source)
        self.assertIn("ManagedInterpreterExecutor", proof_source)

        self.assertIn("ChaosUnitTest(", lowering_source)
        self.assertIn('Alias = "interpreter-lowering-proof"', lowering_source)
        self.assertIn("ChaosUnitCategory.RuntimeContract", lowering_source)
        self.assertIn("ILToIRLowering", lowering_source)

        self.assertIn("ChaosBenchmark(", benchmark_source)
        self.assertIn('Alias = "mixed-execution-bench"', benchmark_source)
        self.assertIn("ChaosBenchmarkCategory.RuntimeDispatch", benchmark_source)
        self.assertIn("AotCompute", benchmark_source)
        self.assertIn("InterpreterCompute", benchmark_source)


if __name__ == "__main__":
    unittest.main()
