from tests.integration.registry.declared_metadata_discovery_test_support import *


class TestDeclaredMetadataDiscoverySolutionCorePackBenchmarks(DeclaredMetadataDiscoveryTestSupport):
    def test_solution_core_pack_performance_slice_discovers_declared_benchmark_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_solution_core_pack_performance_slice")

        build_root = TEST_TMP_ROOT / "core-runtime-benchmarks" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(SOLUTION_CORE_PACK_PERFORMANCE_SLICE_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "CoreRuntimeBenchmarks.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="SolutionCorePack",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual("declared-test", catalog["subjectKind"])
            self.assertEqual([], catalog["warningCodes"])
            self.assertEqual([], catalog["declaredUnitTests"])

            benchmark_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredBenchmarks"]
            }
            self.assertEqual(
                {
                    "CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry",
                    "CoreRuntimeBenchmarks.AllocationBenchmarkEntry",
                    "CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry",
                    "CoreRuntimeBenchmarks.DispatchBenchmarkEntry",
                    "CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry",
                    "CoreRuntimeBenchmarks.FloatingPointCarrierBenchmarkEntry",
                    "CoreRuntimeBenchmarks.GenericBenchmarkEntry",
                    "CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry",
                    "CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry",
                    "CoreRuntimeBenchmarks.IntegerCarrierBenchmarkEntry",
                    "CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry",
                    "CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry",
                    "CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry",
                    "CoreRuntimeBenchmarks.ShiftOpsBenchmarkEntry",
                    "CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry",
                    "CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry",
                    "CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry",
                    "CoreRuntimeBenchmarks.UInt64BitwiseBenchmarkEntry",
                    "CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("arithmetic-bench", benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["alias"])
            self.assertEqual("allocation-bench", benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["alias"])
            self.assertEqual(
                "delegate-callback-interop-bench",
                benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["alias"],
            )
            self.assertEqual("dispatch-bench", benchmark_entries["CoreRuntimeBenchmarks.DispatchBenchmarkEntry"]["alias"])
            self.assertEqual(
                "function-pointer-bench",
                benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["alias"],
            )
            self.assertEqual("generic-bench", benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["alias"])
            self.assertEqual(
                "generic-interface-dispatch-bench",
                benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "generic-sharing-boundary-bench",
                benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "member-metadata-lookup-bench",
                benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "monitor-locking-bench",
                benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "native-call-interop-bench",
                benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "required-instantiation-closure-bench",
                benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["alias"],
            )
            self.assertEqual("span-memory-bench", benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["alias"])
            self.assertEqual(
                "string-utf8-marshaling-bench",
                benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "struct-marshaling-bench",
                benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "task-valuetask-flow-bench",
                benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["alias"],
            )
            self.assertEqual(
                "task-scheduling-bench",
                benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["alias"],
            )
            self.assertEqual("unsafe-pointer-bench", benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in benchmark_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in benchmark_entries.values()))
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.DispatchBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["modes"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["category"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["category"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(34, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(10, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(46, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(2, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(12, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(2, benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(11, benchmark_entries["CoreRuntimeBenchmarks.GenericInterfaceDispatchBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(12, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(52, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(6, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(22, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(8, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(37, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(31, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(12, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(51, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(10, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(43, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(32, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(33, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(15, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(8, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(36, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(10, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(45, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.ArithmeticBenchmarkEntry"]["metrics"])
            self.assertEqual(3, benchmark_entries["CoreRuntimeBenchmarks.AllocationBenchmarkEntry"]["metrics"])
            self.assertEqual(20, benchmark_entries["CoreRuntimeBenchmarks.DelegateCallbackInteropBenchmarkEntry"]["requires"])
            self.assertEqual(2048, benchmark_entries["CoreRuntimeBenchmarks.FunctionPointerBenchmarkEntry"]["requires"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.GenericBenchmarkEntry"]["requires"])
            self.assertEqual(8193, benchmark_entries["CoreRuntimeBenchmarks.GenericSharingBoundaryBenchmarkEntry"]["requires"])
            self.assertEqual(2, benchmark_entries["CoreRuntimeBenchmarks.MemberMetadataLookupBenchmarkEntry"]["requires"])
            self.assertEqual(256, benchmark_entries["CoreRuntimeBenchmarks.MonitorAndLockingBenchmarkEntry"]["requires"])
            self.assertEqual(16, benchmark_entries["CoreRuntimeBenchmarks.NativeCallInteropBenchmarkEntry"]["requires"])
            self.assertEqual(1, benchmark_entries["CoreRuntimeBenchmarks.RequiredInstantiationClosureBenchmarkEntry"]["requires"])
            self.assertEqual(512, benchmark_entries["CoreRuntimeBenchmarks.SpanAndMemoryBenchmarkEntry"]["requires"])
            self.assertEqual(16, benchmark_entries["CoreRuntimeBenchmarks.StringAndUtf8MarshalingBenchmarkEntry"]["requires"])
            self.assertEqual(16, benchmark_entries["CoreRuntimeBenchmarks.StructMarshalingBenchmarkEntry"]["requires"])
            self.assertEqual(64, benchmark_entries["CoreRuntimeBenchmarks.TaskAndValueTaskFlowBenchmarkEntry"]["requires"])
            self.assertEqual(384, benchmark_entries["CoreRuntimeBenchmarks.TaskSchedulingBenchmarkEntry"]["requires"])
            self.assertEqual(1024, benchmark_entries["CoreRuntimeBenchmarks.UnsafePointerBenchmarkEntry"]["requires"])
        finally:
            shutil.rmtree(build_root, ignore_errors=True)
