from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SHARED_PERF_PROJECT_PATH = "src/validation/perf/Benchmark.WorkloadEntry.PerfHarness/Benchmark.WorkloadEntry.PerfHarness.csproj"
SOLUTION_CORE_PACK_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
HOT_UPDATE_HOST_PACK_ROOT = REPO_ROOT / "subjects" / "HotUpdateHostPack"
MIXED_EXECUTION_FEATURE_PACK_ROOT = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack"
MANAGED_PERF_SUBJECTS = {
    "HotUpdateHostPack": {
        "manifest_path": HOT_UPDATE_HOST_PACK_ROOT / "subject.manifest.json",
        "source_path": "subjects/HotUpdateHostPack/source/HotUpdateHostPack.sln",
        "primary_project_path": "subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj",
        "workload_entry": "HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()",
        "harness_iterations": 4,
    },
    "MixedExecutionFeaturePack": {
        "manifest_path": MIXED_EXECUTION_FEATURE_PACK_ROOT / "subject.manifest.json",
        "source_path": "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln",
        "primary_project_path": "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
        "workload_entry": "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
        "native_workload_entry": "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
        "harness_iterations": 6,
    },
}


class BenchmarkSubjectSourceTests(unittest.TestCase):
    def test_solution_core_pack_declares_internal_performance_slice_benchmarks(self) -> None:
        manifest_path = SOLUTION_CORE_PACK_ROOT / "subject.manifest.json"
        project_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj"
        arithmetic_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "PrimitivesAndOps" / "ArithmeticBenchmark.cs"
        allocation_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "MemoryAndAllocation" / "AllocationBenchmark.cs"
        dispatch_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "ObjectModelAndDispatch" / "DispatchBenchmark.cs"
        delegate_callback_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "ObjectModelAndDispatch"
            / "DelegateCallbackInteropBenchmark.cs"
        )
        generic_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "GenericsAndCollections" / "GenericBenchmark.cs"
        generic_interface_dispatch_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "ObjectModelAndDispatch"
            / "GenericInterfaceDispatchBenchmark.cs"
        )
        generic_sharing_boundary_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "GenericsAndCollections"
            / "GenericSharingBoundaryBenchmark.cs"
        )
        required_instantiation_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "GenericsAndCollections"
            / "RequiredInstantiationClosureBenchmark.cs"
        )
        native_call_interop_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "InteropAndMarshaling"
            / "NativeCallInteropBenchmark.cs"
        )
        string_utf8_marshaling_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "InteropAndMarshaling"
            / "StringAndUtf8MarshalingBenchmark.cs"
        )
        struct_marshaling_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "InteropAndMarshaling"
            / "StructMarshalingBenchmark.cs"
        )
        task_flow_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "AsyncAndIterators" / "TaskAndValueTaskFlowBenchmark.cs"
        task_scheduling_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "ThreadingAndSynchronization" / "TaskSchedulingBenchmark.cs"
        monitor_locking_path = SOLUTION_CORE_PACK_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "ThreadingAndSynchronization" / "MonitorAndLockingBenchmark.cs"
        member_metadata_lookup_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "ReflectionAndMetadata"
            / "MemberMetadataLookupBenchmark.cs"
        )
        function_pointer_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "UnsafeAndLowLevel"
            / "FunctionPointerBenchmark.cs"
        )
        span_memory_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "UnsafeAndLowLevel"
            / "SpanAndMemoryBenchmark.cs"
        )
        unsafe_pointer_path = (
            SOLUTION_CORE_PACK_ROOT
            / "source"
            / "Benchmarks"
            / "CoreRuntimeBenchmarks"
            / "UnsafeAndLowLevel"
            / "UnsafePointerBenchmark.cs"
        )

        for path in [
            manifest_path,
            project_path,
            arithmetic_path,
            allocation_path,
            dispatch_path,
            delegate_callback_path,
            generic_path,
            generic_interface_dispatch_path,
            generic_sharing_boundary_path,
            required_instantiation_path,
            native_call_interop_path,
            string_utf8_marshaling_path,
            struct_marshaling_path,
            task_flow_path,
            task_scheduling_path,
            monitor_locking_path,
            member_metadata_lookup_path,
            function_pointer_path,
            span_memory_path,
            unsafe_pointer_path,
        ]:
            self.assertTrue(path.is_file(), msg=f"missing solution core perf asset: {path}")

        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        arithmetic_source = arithmetic_path.read_text(encoding="utf-8")
        allocation_source = allocation_path.read_text(encoding="utf-8")
        dispatch_source = dispatch_path.read_text(encoding="utf-8")
        delegate_callback_source = delegate_callback_path.read_text(encoding="utf-8")
        generic_source = generic_path.read_text(encoding="utf-8")
        generic_interface_dispatch_source = generic_interface_dispatch_path.read_text(encoding="utf-8")
        generic_sharing_boundary_source = generic_sharing_boundary_path.read_text(encoding="utf-8")
        required_instantiation_source = required_instantiation_path.read_text(encoding="utf-8")
        native_call_interop_source = native_call_interop_path.read_text(encoding="utf-8")
        string_utf8_marshaling_source = string_utf8_marshaling_path.read_text(encoding="utf-8")
        struct_marshaling_source = struct_marshaling_path.read_text(encoding="utf-8")
        task_flow_source = task_flow_path.read_text(encoding="utf-8")
        task_scheduling_source = task_scheduling_path.read_text(encoding="utf-8")
        monitor_locking_source = monitor_locking_path.read_text(encoding="utf-8")
        member_metadata_lookup_source = member_metadata_lookup_path.read_text(encoding="utf-8")
        function_pointer_source = function_pointer_path.read_text(encoding="utf-8")
        span_memory_source = span_memory_path.read_text(encoding="utf-8")
        unsafe_pointer_source = unsafe_pointer_path.read_text(encoding="utf-8")

        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        benchmark_project_path = (
            "subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/CoreRuntimeBenchmarks.csproj"
        )
        managed_perf_matrix = next(matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-managed-perf")
        native_perf_matrix = next(matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-native-perf")
        self.assertEqual(benchmark_project_path, managed_perf_matrix["source"]["primaryProjectPath"])
        self.assertEqual(benchmark_project_path, native_perf_matrix["source"]["primaryProjectPath"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", managed_perf_matrix["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", native_perf_matrix["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", native_perf_matrix["workloadEntry"])

        self.assertIn("ChaosBenchmark(", arithmetic_source)
        self.assertIn('Alias = "arithmetic-bench"', arithmetic_source)
        self.assertIn("ChaosBenchmarkCategory.RuntimeDispatch", arithmetic_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", arithmetic_source)
        self.assertIn("ChaosMetric.WallClockUs", arithmetic_source)

        self.assertIn("ChaosBenchmark(", allocation_source)
        self.assertIn('Alias = "allocation-bench"', allocation_source)
        self.assertIn("ChaosBenchmarkCategory.Allocation", allocation_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", allocation_source)
        self.assertIn("ChaosMetric.ManagedAllocBytes", allocation_source)
        self.assertIn("public static int RunWorkload()", allocation_source)
        self.assertNotIn("public static long RunWorkload()", allocation_source)

        self.assertIn("ChaosBenchmark(", dispatch_source)
        self.assertIn('Alias = "dispatch-bench"', dispatch_source)
        self.assertIn("ChaosBenchmarkCategory.RuntimeDispatch", dispatch_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", dispatch_source)
        self.assertIn("DispatchShape first = new DispatchCircle(3.0);", dispatch_source)
        self.assertIn("(i & 3) switch", dispatch_source)
        self.assertNotIn("DispatchShape[] shapes", dispatch_source)
        self.assertIn("public static int RunWorkload()", dispatch_source)
        self.assertNotIn("public static long RunWorkload()", dispatch_source)

        self.assertIn("ChaosBenchmark(", generic_source)
        self.assertIn('Alias = "generic-bench"', generic_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", generic_source)
        self.assertIn("ChaosRuntimeFeature.GenericSharing", generic_source)
        self.assertIn("Dictionary<string, int>", generic_source)
        self.assertIn("public static int RunWorkload()", generic_source)
        self.assertNotIn("public static long RunWorkload()", generic_source)

        self.assertIn("ChaosBenchmark(", generic_interface_dispatch_source)
        self.assertIn('Alias = "generic-interface-dispatch-bench"', generic_interface_dispatch_source)
        self.assertIn("ChaosCapabilityItem.GenericInterfaceDispatch", generic_interface_dispatch_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", generic_interface_dispatch_source)
        self.assertIn("IGenericDispatchValue<int>", generic_interface_dispatch_source)

        for source_text, alias in [
            (delegate_callback_source, "delegate-callback-interop-bench"),
            (generic_sharing_boundary_source, "generic-sharing-boundary-bench"),
            (required_instantiation_source, "required-instantiation-closure-bench"),
            (native_call_interop_source, "native-call-interop-bench"),
            (string_utf8_marshaling_source, "string-utf8-marshaling-bench"),
            (struct_marshaling_source, "struct-marshaling-bench"),
            (task_flow_source, "task-valuetask-flow-bench"),
            (member_metadata_lookup_source, "member-metadata-lookup-bench"),
            (function_pointer_source, "function-pointer-bench"),
            (span_memory_source, "span-memory-bench"),
            (unsafe_pointer_source, "unsafe-pointer-bench"),
        ]:
            self.assertIn("ChaosBenchmark(", source_text)
            self.assertIn(f'Alias = "{alias}"', source_text)
            self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", source_text)

        self.assertIn("ChaosBenchmark(", task_flow_source)
        self.assertIn('Alias = "task-valuetask-flow-bench"', task_flow_source)
        self.assertIn("ChaosCapabilityItem.TaskAndValueTaskFlow", task_flow_source)
        self.assertIn("ChaosRuntimeFeature.AsyncStateMachine", task_flow_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", task_flow_source)

        self.assertIn("ChaosBenchmark(", task_scheduling_source)
        self.assertIn('Alias = "task-scheduling-bench"', task_scheduling_source)
        self.assertIn("ChaosCapabilityItem.TaskScheduling", task_scheduling_source)
        self.assertIn("ChaosRuntimeFeature.Threading | ChaosRuntimeFeature.Synchronization", task_scheduling_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", task_scheduling_source)

        self.assertIn("ChaosBenchmark(", monitor_locking_source)
        self.assertIn('Alias = "monitor-locking-bench"', monitor_locking_source)
        self.assertIn("ChaosCapabilityItem.MonitorAndLocking", monitor_locking_source)
        self.assertIn("ChaosRuntimeFeature.Synchronization", monitor_locking_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", monitor_locking_source)

    def test_managed_benchmark_subjects_keep_perf_harness_external_and_solution_core_pack_adds_managed_baseline(self) -> None:
        self.assertTrue((REPO_ROOT / SHARED_PERF_PROJECT_PATH).is_file())

        for subject_id, spec in MANAGED_PERF_SUBJECTS.items():
            manifest_path = spec["manifest_path"]
            self.assertTrue(manifest_path.is_file(), msg=f"missing managed perf manifest: {manifest_path}")
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            workload_entry = str(spec["workload_entry"])

            self.assertEqual(str(spec["source_path"]), manifest["source"]["path"])
            self.assertEqual(str(spec["primary_project_path"]), manifest["source"]["primaryProjectPath"])
            self.assertEqual(workload_entry, manifest["workloadEntry"])
            self.assertEqual("require", manifest["testDeclarationMode"])
            self.assertEqual(spec["harness_iterations"], manifest["validation"]["perf"]["harnessIterations"])
            self.assertEqual(SHARED_PERF_PROJECT_PATH, manifest["validation"]["perf"]["project"])
            self.assertNotIn("Program::Main(System.String[])", json.dumps(manifest, ensure_ascii=False))

            for pipeline in manifest["executionPipelines"]:
                stage_kinds = {stage["kind"] for stage in pipeline["stages"]}
                if pipeline["pipelineId"] == "native-benchmark":
                    self.assertIn("generated-native-aot", stage_kinds, msg=subject_id)
                    self.assertNotIn("generated-native-proof", stage_kinds, msg=subject_id)

            for matrix in manifest["environmentMatrices"]:
                if matrix["pipelineId"] == "native-benchmark":
                    native_workload_entry = str(spec.get("native_workload_entry") or workload_entry)
                    self.assertEqual(native_workload_entry, matrix["source"]["entry"], msg=subject_id)

        solution_core_manifest = json.loads((SOLUTION_CORE_PACK_ROOT / "subject.manifest.json").read_text(encoding="utf-8"))
        self.assertEqual(SHARED_PERF_PROJECT_PATH, solution_core_manifest["validation"]["perf"]["project"])
        self.assertEqual(4, solution_core_manifest["validation"]["perf"]["harnessIterations"])
        managed_pipeline = next(
            pipeline for pipeline in solution_core_manifest["executionPipelines"] if pipeline["pipelineId"] == "managed-benchmark"
        )
        managed_stage_kinds = {stage["kind"] for stage in managed_pipeline["stages"]}
        self.assertIn("runtime-perf-collect", managed_stage_kinds)
        native_pipeline = next(
            pipeline for pipeline in solution_core_manifest["executionPipelines"] if pipeline["pipelineId"] == "native-benchmark"
        )
        native_stage_kinds = {stage["kind"] for stage in native_pipeline["stages"]}
        self.assertIn("generated-native-aot", native_stage_kinds)
        self.assertNotIn("generated-native-proof", native_stage_kinds)
        managed_perf_matrix = next(
            matrix for matrix in solution_core_manifest["environmentMatrices"] if matrix["matrixId"] == "windows-managed-perf"
        )
        self.assertEqual(
            "subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/CoreRuntimeBenchmarks.csproj",
            managed_perf_matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", managed_perf_matrix["source"]["entry"])
        self.assertEqual("managed-benchmark", managed_perf_matrix["pipelineId"])

    def test_hot_update_host_pack_declares_hot_update_unit_and_benchmark_slices(self) -> None:
        manifest_path = HOT_UPDATE_HOST_PACK_ROOT / "subject.manifest.json"
        project_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "HotUpdateHostPack.csproj"
        program_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Program.cs"
        skeleton_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Proofs" / "HotUpdateSkeletonProofEntry.cs"
        patch_callback_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Proofs" / "PatchCallbackFlowProofEntry.cs"
        replacement_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Proofs" / "MethodReplacementProofEntry.cs"
        shared_proof_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Proofs" / "SharedContractProofEntry.cs"
        shared_contract_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "SharedContracts" / "ContractIdentityWitness.cs"
        rollback_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Proofs" / "VersionRollbackProofEntry.cs"
        dispatch_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Benchmarks" / "HotUpdateDispatchBenchmark.cs"
        load_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Benchmarks" / "HotUpdateLoadBenchmark.cs"
        method_replacement_benchmark_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Benchmarks" / "MethodReplacementBenchmark.cs"
        roundtrip_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Host" / "Benchmarks" / "HotUpdateRoundtripBenchmark.cs"
        package_support_path = HOT_UPDATE_HOST_PACK_ROOT / "source" / "Patch" / "HotUpdatePackageSupport.cs"
        full_project_solution_path = (
            HOT_UPDATE_HOST_PACK_ROOT / "source" / "EngineeringScenarios" / "FullProjectHotUpdateSolution" / "FullProjectHotUpdateSolution.sln"
        )
        full_project_host_path = (
            HOT_UPDATE_HOST_PACK_ROOT / "source" / "EngineeringScenarios" / "FullProjectHotUpdateSolution" / "HostApp" / "Program.cs"
        )

        for path in [
            manifest_path,
            project_path,
            program_path,
            skeleton_path,
            patch_callback_path,
            replacement_path,
            shared_proof_path,
            shared_contract_path,
            rollback_path,
            dispatch_path,
            load_path,
            method_replacement_benchmark_path,
            roundtrip_path,
            package_support_path,
            full_project_solution_path,
            full_project_host_path,
        ]:
            self.assertTrue(path.is_file(), msg=f"missing hot-update host pack asset: {path}")

        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        program_source = program_path.read_text(encoding="utf-8")
        skeleton_source = skeleton_path.read_text(encoding="utf-8")
        patch_callback_source = patch_callback_path.read_text(encoding="utf-8")
        replacement_source = replacement_path.read_text(encoding="utf-8")
        shared_proof_source = shared_proof_path.read_text(encoding="utf-8")
        shared_contract_source = shared_contract_path.read_text(encoding="utf-8")
        rollback_source = rollback_path.read_text(encoding="utf-8")
        dispatch_source = dispatch_path.read_text(encoding="utf-8")
        load_source = load_path.read_text(encoding="utf-8")
        method_replacement_benchmark_source = method_replacement_benchmark_path.read_text(encoding="utf-8")
        roundtrip_source = roundtrip_path.read_text(encoding="utf-8")
        package_support_source = package_support_path.read_text(encoding="utf-8")
        full_project_host_source = full_project_host_path.read_text(encoding="utf-8")

        self.assertEqual("HotUpdateHostPack", manifest["subjectId"])
        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.sln", manifest["source"]["path"])
        self.assertEqual("subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj", manifest["source"]["primaryProjectPath"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("HotUpdateHostPack/Program::Main()", manifest["source"]["entry"])
        self.assertEqual("HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertEqual(["proof"], manifest["validationProfiles"]["managed-output"])
        self.assertEqual(["perf"], manifest["validationProfiles"]["perf-profile"])
        self.assertEqual(4, manifest["validation"]["perf"]["harnessIterations"])
        self.assertEqual(SHARED_PERF_PROJECT_PATH, manifest["validation"]["perf"]["project"])
        perf_matrix = next(matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-managed-perf")
        self.assertEqual("HotUpdateHostPack/HotUpdateLoadBenchmarkEntry::RunWorkload()", perf_matrix["source"]["entry"])
        archetype_matrix = next(
            matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-archetype-full-project-managed-output"
        )
        self.assertEqual(
            "subjects/HotUpdateHostPack/source/EngineeringScenarios/FullProjectHotUpdateSolution/HostApp/GoldenHotUpdateHost.App.csproj",
            archetype_matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenHotUpdateHost.App/Program::Main()", archetype_matrix["source"]["entry"])

        self.assertIn("HotUpdateSkeletonProofEntry.Run", program_source)
        self.assertIn("PatchCallbackFlowProofEntry.Run", program_source)
        self.assertIn("MethodReplacementProofEntry.Run", program_source)
        self.assertIn("SharedContractProofEntry.Run", program_source)
        self.assertIn("VersionRollbackProofEntry.Run", program_source)

        self.assertIn("ChaosUnitTest(", skeleton_source)
        self.assertIn('Alias = "hot-update-skeleton-proof"', skeleton_source)
        self.assertIn("ChaosUnitCategory.HotUpdateContract", skeleton_source)
        self.assertIn("ChaosRuntimeFeature.HotUpdate", skeleton_source)
        self.assertIn("ChaosSolutionArchetype.SkeletonPatchSolution", skeleton_source)
        self.assertIn("ChaosHotUpdateCapability.PackageLoad", skeleton_source)
        self.assertNotIn("Console.WriteLine", skeleton_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", skeleton_source)

        self.assertIn("ChaosUnitTest(", replacement_source)
        self.assertIn('Alias = "method-replacement-proof"', replacement_source)
        self.assertIn("ChaosUnitCategory.HotUpdateContract", replacement_source)
        self.assertIn("ChaosHotUpdateCapability.MethodReplacement", replacement_source)
        self.assertNotIn("Console.WriteLine", replacement_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", replacement_source)

        self.assertIn("ChaosUnitTest(", patch_callback_source)
        self.assertIn('Alias = "patch-callback-flow-proof"', patch_callback_source)
        self.assertIn("ChaosCapabilityItem.PatchCallbackFlow", patch_callback_source)
        self.assertIn("ChaosHotUpdateCapability.PatchCallbackFlow", patch_callback_source)
        self.assertNotIn("Console.WriteLine", patch_callback_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", patch_callback_source)

        self.assertIn("ChaosUnitTest(", shared_proof_source)
        self.assertIn('Alias = "shared-contract-proof"', shared_proof_source)
        self.assertIn("ChaosSolutionArchetype.FullProjectHotUpdateSolution", shared_proof_source)
        self.assertIn("ChaosHotUpdateCapability.SharedContractBinding", shared_proof_source)
        self.assertIn("ContractIdentityWitness", shared_contract_source)
        self.assertNotIn("Console.WriteLine", shared_proof_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", shared_proof_source)

        self.assertIn("ChaosUnitTest(", rollback_source)
        self.assertIn('Alias = "version-rollback-proof"', rollback_source)
        self.assertIn("ChaosUnitCategory.HotUpdateContract", rollback_source)
        self.assertIn("ChaosHotUpdateCapability.VersionRollback", rollback_source)
        self.assertNotIn("Console.WriteLine", rollback_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", rollback_source)

        self.assertIn("ChaosBenchmark(", dispatch_source)
        self.assertIn('Alias = "hot-update-dispatch-bench"', dispatch_source)
        self.assertIn("ChaosBenchmarkCategory.HotUpdate", dispatch_source)
        self.assertIn("ChaosRuntimeFeature.HotUpdate", dispatch_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", dispatch_source)
        self.assertIn("ChaosHotUpdateCapability.PatchCallbackFlow", dispatch_source)

        self.assertIn("ChaosBenchmark(", load_source)
        self.assertIn('Alias = "hot-update-load-bench"', load_source)
        self.assertIn("ChaosBenchmarkCategory.HotUpdate", load_source)
        self.assertIn("LoadPackage", load_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", load_source)
        self.assertIn("ChaosHotUpdateCapability.PackageLoad", load_source)

        self.assertIn("ChaosBenchmark(", method_replacement_benchmark_source)
        self.assertIn('Alias = "method-replacement-bench"', method_replacement_benchmark_source)
        self.assertIn("ChaosCapabilityItem.MethodReplacement", method_replacement_benchmark_source)
        self.assertIn("ChaosHotUpdateCapability.MethodReplacement", method_replacement_benchmark_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", method_replacement_benchmark_source)

        self.assertIn("ChaosBenchmark(", roundtrip_source)
        self.assertIn('Alias = "hot-update-roundtrip-bench"', roundtrip_source)
        self.assertIn("ChaosBenchmarkCategory.HotUpdate", roundtrip_source)
        self.assertIn("BridgeDispatcher", roundtrip_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", roundtrip_source)
        self.assertIn("ChaosHotUpdateCapability.PatchCallbackFlow", roundtrip_source)
        self.assertIn("CurrentAotVersion", package_support_source)
        self.assertIn("CreatePackageRoot", package_support_source)
        self.assertIn("Assembly.LoadFrom", full_project_host_source)
        self.assertIn("IPatchFeature", full_project_host_source)
        self.assertIn("Assert.Throws<InvalidOperationException>", full_project_host_source)

    def test_mixed_execution_feature_pack_declares_mixed_interpreter_unit_and_benchmark_slices(self) -> None:
        manifest_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "subject.manifest.json"
        project_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "MixedExecutionFeaturePack.csproj"
        program_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Host" / "Program.cs"
        proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Proofs" / "MixedExecutionProofEntry.cs"
        arithmetic_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Proofs" / "InterpreterArithmeticProofEntry.cs"
        lowering_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Proofs" / "InterpreterLoweringProofEntry.cs"
        generic_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Proofs" / "MixedGenericFlowProofEntry.cs"
        exception_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Proofs" / "MixedExceptionFlowProofEntry.cs"
        delegate_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Proofs" / "MixedDelegateFlowProofEntry.cs"
        benchmark_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Benchmarks" / "MixedExecutionBenchmark.cs"
        native_benchmark_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Benchmarks" / "MixedExecutionNativeBenchmark.cs"
        support_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "Interpreter" / "InterpreterArithmeticSupport.cs"
        support_solution_path = (
            MIXED_EXECUTION_FEATURE_PACK_ROOT / "source" / "EngineeringScenarios" / "MixedBridgeSolution" / "MixedBridgeSolution.sln"
        )
        arithmetic_support_project_path = (
            MIXED_EXECUTION_FEATURE_PACK_ROOT
            / "source"
            / "EngineeringScenarios"
            / "MixedBridgeSolution"
            / "InterpreterArithmeticProof"
            / "InterpreterArithmeticProof.csproj"
        )

        for path in [
            manifest_path,
            project_path,
            program_path,
            proof_path,
            arithmetic_proof_path,
            lowering_path,
            generic_proof_path,
            exception_proof_path,
            delegate_proof_path,
            benchmark_path,
            native_benchmark_path,
            support_path,
            support_solution_path,
            arithmetic_support_project_path,
        ]:
            self.assertTrue(path.is_file(), msg=f"missing mixed execution feature pack asset: {path}")

        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        program_source = program_path.read_text(encoding="utf-8")
        proof_source = proof_path.read_text(encoding="utf-8")
        arithmetic_proof_source = arithmetic_proof_path.read_text(encoding="utf-8")
        lowering_source = lowering_path.read_text(encoding="utf-8")
        generic_proof_source = generic_proof_path.read_text(encoding="utf-8")
        exception_proof_source = exception_proof_path.read_text(encoding="utf-8")
        delegate_proof_source = delegate_proof_path.read_text(encoding="utf-8")
        benchmark_source = benchmark_path.read_text(encoding="utf-8")
        native_benchmark_source = native_benchmark_path.read_text(encoding="utf-8")
        support_source = support_path.read_text(encoding="utf-8")

        self.assertEqual("MixedExecutionFeaturePack", manifest["subjectId"])
        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln", manifest["source"]["path"])
        self.assertEqual("subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj", manifest["source"]["primaryProjectPath"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        archetype_matrix = next(
            matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-archetype-mixed-bridge-managed-output"
        )
        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/EngineeringScenarios/MixedBridgeSolution/InterpreterArithmeticProof/InterpreterArithmeticProof.csproj",
            archetype_matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("InterpreterArithmeticProof/Program::Main()", archetype_matrix["source"]["entry"])

        self.assertIn("MixedExecutionProofEntry.Run", program_source)
        self.assertIn("InterpreterArithmeticProofEntry.Run", program_source)
        self.assertIn("InterpreterLoweringProofEntry.Run", program_source)
        self.assertIn("MixedGenericFlowProofEntry.Run", program_source)
        self.assertIn("MixedExceptionFlowProofEntry.Run", program_source)
        self.assertIn("MixedDelegateFlowProofEntry.Run", program_source)

        self.assertIn("ChaosUnitTest(", proof_source)
        self.assertIn('Alias = "mixed-execution-proof"', proof_source)
        self.assertIn("ChaosUnitCategory.RuntimeContract", proof_source)
        self.assertIn("ManagedInterpreterExecutor", proof_source)
        self.assertIn("ChaosSolutionArchetype.MixedBridgeSolution", proof_source)
        self.assertNotIn("Console.WriteLine", proof_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", proof_source)

        self.assertIn("ChaosUnitTest(", arithmetic_proof_source)
        self.assertIn('Alias = "interpreter-arithmetic-proof"', arithmetic_proof_source)
        self.assertIn("ChaosCapabilityItem.InterpreterArithmetic", arithmetic_proof_source)
        self.assertNotIn("Console.WriteLine", arithmetic_proof_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", arithmetic_proof_source)

        self.assertIn("ChaosUnitTest(", lowering_source)
        self.assertIn('Alias = "interpreter-lowering-proof"', lowering_source)
        self.assertIn("ChaosUnitCategory.RuntimeContract", lowering_source)
        self.assertIn("ILToIRLowering", lowering_source)
        self.assertIn("ChaosSolutionArchetype.MixedBridgeSolution", lowering_source)
        self.assertNotIn("Console.WriteLine", lowering_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", lowering_source)

        self.assertIn("ChaosUnitTest(", generic_proof_source)
        self.assertIn('Alias = "mixed-generic-flow-proof"', generic_proof_source)
        self.assertIn("ChaosCapabilityItem.MixedGenericFlow", generic_proof_source)
        self.assertNotIn("Console.WriteLine", generic_proof_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", generic_proof_source)

        self.assertIn("ChaosUnitTest(", exception_proof_source)
        self.assertIn('Alias = "mixed-exception-flow-proof"', exception_proof_source)
        self.assertIn("ChaosCapabilityItem.MixedExceptionFlow", exception_proof_source)
        self.assertNotIn("Console.WriteLine", exception_proof_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", exception_proof_source)

        self.assertIn("ChaosUnitTest(", delegate_proof_source)
        self.assertIn('Alias = "mixed-delegate-flow-proof"', delegate_proof_source)
        self.assertIn("ChaosCapabilityItem.MixedDelegateFlow", delegate_proof_source)
        self.assertNotIn("Console.WriteLine", delegate_proof_source)
        self.assertNotIn("ChaosEvidenceKind.Stdout", delegate_proof_source)

        self.assertIn("ChaosBenchmark(", benchmark_source)
        self.assertIn('Alias = "mixed-execution-bench"', benchmark_source)
        self.assertIn("ChaosBenchmarkCategory.RuntimeDispatch", benchmark_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Interpreter", benchmark_source)
        self.assertIn("ChaosSolutionArchetype.MixedBridgeSolution", benchmark_source)
        self.assertIn("public static int RunWorkload()", benchmark_source)
        self.assertIn("AotCompute", benchmark_source)
        self.assertIn("InterpreterCompute", benchmark_source)

        managed_perf_matrix = next(
            matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-managed-perf"
        )
        native_perf_matrix = next(
            matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-native-perf"
        )
        self.assertEqual(
            "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
            managed_perf_matrix["source"]["entry"],
        )
        self.assertEqual(
            "MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()",
            native_perf_matrix["source"]["entry"],
        )
        self.assertIn("ChaosBenchmark(", native_benchmark_source)
        self.assertIn('Alias = "mixed-execution-native-bench"', native_benchmark_source)
        self.assertIn("ChaosSolutionArchetype.MixedBridgeSolution", native_benchmark_source)
        self.assertIn("public static int RunWorkload()", native_benchmark_source)
        self.assertIn("private const int IterationCount =", native_benchmark_source)
        self.assertIn("for (int i = 0; i < IterationCount; i++)", native_benchmark_source)
        self.assertIn("ResolveAssemblyPath", support_source)
        self.assertIn("InterpreterArithmeticProof.dll", support_source)


if __name__ == "__main__":
    unittest.main()

