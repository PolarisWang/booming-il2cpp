from tests.unit.performance.benchmark_subject_sources_test_support import *


class TestBenchmarkSubjectSourcesSolutionCorePack(BenchmarkSubjectSourceTestSupport):
    def test_solution_core_pack_declares_internal_performance_slice_benchmarks(self) -> None:
        manifest_path = SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH
        project_path = SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH
        arithmetic_path = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "PrimitivesAndOps" / "ArithmeticBenchmark.cs"
        allocation_path = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "MemoryAndAllocation" / "AllocationBenchmark.cs"
        dispatch_path = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "ObjectModelAndDispatch" / "DispatchBenchmark.cs"
        delegate_callback_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "ObjectModelAndDispatch"
            / "DelegateCallbackInteropBenchmark.cs"
        )
        generic_path = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "GenericsAndCollections" / "GenericBenchmark.cs"
        generic_interface_dispatch_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "ObjectModelAndDispatch"
            / "GenericInterfaceDispatchBenchmark.cs"
        )
        generic_sharing_boundary_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "GenericsAndCollections"
            / "GenericSharingBoundaryBenchmark.cs"
        )
        required_instantiation_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "GenericsAndCollections"
            / "RequiredInstantiationClosureBenchmark.cs"
        )
        native_call_interop_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "InteropAndMarshaling"
            / "NativeCallInteropBenchmark.cs"
        )
        string_utf8_marshaling_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "InteropAndMarshaling"
            / "StringAndUtf8MarshalingBenchmark.cs"
        )
        struct_marshaling_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "InteropAndMarshaling"
            / "StructMarshalingBenchmark.cs"
        )
        task_flow_path = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "AsyncAndIterators" / "TaskAndValueTaskFlowBenchmark.cs"
        task_scheduling_path = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "ThreadingAndSynchronization" / "TaskSchedulingBenchmark.cs"
        monitor_locking_path = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "ThreadingAndSynchronization" / "MonitorAndLockingBenchmark.cs"
        member_metadata_lookup_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "ReflectionAndMetadata"
            / "MemberMetadataLookupBenchmark.cs"
        )
        function_pointer_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "UnsafeAndLowLevel"
            / "FunctionPointerBenchmark.cs"
        )
        span_memory_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
            / "UnsafeAndLowLevel"
            / "SpanAndMemoryBenchmark.cs"
        )
        unsafe_pointer_path = (
            SOLUTION_CORE_PACK_BENCHMARKS_ROOT
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
        project_source = project_path.read_text(encoding="utf-8")
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
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION_PATH.relative_to(REPO_ROOT).as_posix(), manifest["source"]["path"])
        self.assertEqual(SOLUTION_CORE_PACK_HOST_PROJECT_PATH.relative_to(REPO_ROOT).as_posix(), manifest["source"]["primaryProjectPath"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        benchmark_project_path = SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH.relative_to(REPO_ROOT).as_posix()
        managed_perf_matrix = next(matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-managed-perf")
        native_perf_matrix = next(matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-native-perf")
        self.assertNotIn('<InternalsVisibleTo Include="SolutionCorePack.DeclaredBenchmarkHost" />', project_source)
        self.assertIn('<InternalsVisibleTo Include="SolutionCorePack.DeclaredBenchmarkNativeHost" />', project_source)
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
        self.assertIn("Modes = ChaosExecutionMode.Managed", dispatch_source)
        self.assertIn("DispatchShape first = new DispatchCircle(3.0);", dispatch_source)
        self.assertIn("(i & 3) switch", dispatch_source)
        self.assertNotIn("DispatchShape[] shapes", dispatch_source)
        self.assertIn("public static int RunWorkload()", dispatch_source)
        self.assertNotIn("public static long RunWorkload()", dispatch_source)

        self.assertIn("ChaosBenchmark(", generic_source)
        self.assertIn('Alias = "generic-bench"', generic_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", generic_source)
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
            (string_utf8_marshaling_source, "string-utf8-marshaling-bench"),
            (struct_marshaling_source, "struct-marshaling-bench"),
            (function_pointer_source, "function-pointer-bench"),
        ]:
            self.assertIn("ChaosBenchmark(", source_text)
            self.assertIn(f'Alias = "{alias}"', source_text)
            self.assertIn("Modes = ChaosExecutionMode.Managed | ChaosExecutionMode.Native", source_text)

        self.assertIn("ChaosBenchmark(", task_flow_source)
        self.assertIn('Alias = "task-valuetask-flow-bench"', task_flow_source)
        self.assertIn("ChaosCapabilityItem.TaskAndValueTaskFlow", task_flow_source)
        self.assertIn("ChaosRuntimeFeature.AsyncStateMachine", task_flow_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", task_flow_source)

        self.assertIn("ChaosBenchmark(", task_scheduling_source)
        self.assertIn('Alias = "task-scheduling-bench"', task_scheduling_source)
        self.assertIn("ChaosCapabilityItem.TaskScheduling", task_scheduling_source)
        self.assertIn("ChaosRuntimeFeature.Threading | ChaosRuntimeFeature.Synchronization", task_scheduling_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", task_scheduling_source)
        self.assertIn("task.GetAwaiter().GetResult() + 1", task_scheduling_source)
        self.assertNotIn("task.Result + 1", task_scheduling_source)

        self.assertIn("ChaosBenchmark(", unsafe_pointer_source)
        self.assertIn('Alias = "unsafe-pointer-bench"', unsafe_pointer_source)
        self.assertIn("ChaosCapabilityItem.UnsafePointer", unsafe_pointer_source)
        self.assertIn("ChaosRuntimeFeature.UnsafePointer", unsafe_pointer_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", unsafe_pointer_source)

        self.assertIn("ChaosBenchmark(", span_memory_source)
        self.assertIn('Alias = "span-memory-bench"', span_memory_source)
        self.assertIn("ChaosCapabilityItem.SpanAndMemory", span_memory_source)
        self.assertIn("ChaosRuntimeFeature.SpanMemory", span_memory_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", span_memory_source)

        self.assertIn("ChaosBenchmark(", member_metadata_lookup_source)
        self.assertIn('Alias = "member-metadata-lookup-bench"', member_metadata_lookup_source)
        self.assertIn("ChaosCapabilityItem.MemberMetadataLookup", member_metadata_lookup_source)
        self.assertIn("ChaosRuntimeFeature.Reflection", member_metadata_lookup_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", member_metadata_lookup_source)

        self.assertIn("ChaosBenchmark(", native_call_interop_source)
        self.assertIn('Alias = "native-call-interop-bench"', native_call_interop_source)
        self.assertIn("ChaosCapabilityItem.NativeCallInterop", native_call_interop_source)
        self.assertIn("ChaosRuntimeFeature.NativeInterop", native_call_interop_source)
        self.assertIn("Modes = ChaosExecutionMode.Managed", native_call_interop_source)

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

        solution_core_manifest = json.loads(SOLUTION_CORE_PACK_OWNER_MANIFEST_PATH.read_text(encoding="utf-8"))
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
            SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH.relative_to(REPO_ROOT).as_posix(),
            managed_perf_matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", managed_perf_matrix["source"]["entry"])
        self.assertEqual("managed-benchmark", managed_perf_matrix["pipelineId"])
