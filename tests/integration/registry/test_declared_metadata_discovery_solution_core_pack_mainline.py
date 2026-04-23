from tests.integration.registry.declared_metadata_discovery_test_support import *


class TestDeclaredMetadataDiscoverySolutionCorePackMainline(DeclaredMetadataDiscoveryTestSupport):
    def test_solution_core_pack_mainline_slice_discovers_declared_unit_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_solution_core_pack_mainline_slice")

        build_root = TEST_TMP_ROOT / "core-runtime-features" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(SOLUTION_CORE_PACK_MAINLINE_SLICE_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "CoreRuntimeFeatures.dll"
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
            self.assertEqual([], catalog["declaredBenchmarks"])

            unit_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredUnitTests"]
            }
            self.assertEqual(
                {
                    "CoreRuntimeFeatures.AbiWideningProofEntry",
                    "CoreRuntimeFeatures.AotClosureValidationProofEntry",
                    "CoreRuntimeFeatures.ArithmeticOpsProofEntry",
                    "CoreRuntimeFeatures.ArrayOpsProofEntry",
                    "CoreRuntimeFeatures.ArrayCovarianceProofEntry",
                    "CoreRuntimeFeatures.BitwiseOpsProofEntry",
                    "CoreRuntimeFeatures.BoxedStructFieldReadProofEntry",
                    "CoreRuntimeFeatures.BranchOpsProofEntry",
                    "CoreRuntimeFeatures.CastTypeCheckProofEntry",
                    "CoreRuntimeFeatures.ConversionOpsProofEntry",
                    "CoreRuntimeFeatures.CrossBoundaryExceptionProofEntry",
                    "CoreRuntimeFeatures.CustomAttributeLookupProofEntry",
                    "CoreRuntimeFeatures.DelegateCallbackInteropProofEntry",
                    "CoreRuntimeFeatures.DelegateChainProofEntry",
                    "CoreRuntimeFeatures.ArrayBoxingProofEntry",
                    "CoreRuntimeFeatures.AsyncAwaitProofEntry",
                    "CoreRuntimeFeatures.DelegateProofEntry",
                    "CoreRuntimeFeatures.DirectCallVirtProofEntry",
                    "CoreRuntimeFeatures.EventCallbackFlowProofEntry",
                    "CoreRuntimeFeatures.ExceptionCatchProofEntry",
                    "CoreRuntimeFeatures.ExceptionProofEntry",
                    "CoreRuntimeFeatures.FinallyAndFilterProofEntry",
                    "CoreRuntimeFeatures.FunctionPointerProofEntry",
                    "CoreRuntimeFeatures.GcSensitiveFlowProofEntry",
                    "CoreRuntimeFeatures.GenericCollectionProofEntry",
                    "CoreRuntimeFeatures.GenericConstraintProofEntry",
                    "CoreRuntimeFeatures.GenericInstantiationProofEntry",
                    "CoreRuntimeFeatures.GenericSharingBoundaryProofEntry",
                    "CoreRuntimeFeatures.GenericLayoutProofEntry",
                    "CoreRuntimeFeatures.InterfaceDispatchProofEntry",
                    "CoreRuntimeFeatures.IntegerCarrierProofEntry",
                    "CoreRuntimeFeatures.IteratorStateMachineProofEntry",
                    "CoreRuntimeFeatures.LambdaClosureCaptureProofEntry",
                    "CoreRuntimeFeatures.LinkerPreserveContractProofEntry",
                    "CoreRuntimeFeatures.LinkerStrippingProofEntry",
                    "CoreRuntimeFeatures.LoaderContractProofEntry",
                    "CoreRuntimeFeatures.MarshalingProofEntry",
                    "CoreRuntimeFeatures.MetadataSupplementProofEntry",
                    "CoreRuntimeFeatures.MemberMetadataLookupProofEntry",
                    "CoreRuntimeFeatures.MonitorAndLockingProofEntry",
                    "CoreRuntimeFeatures.NativeCallInteropProofEntry",
                    "CoreRuntimeFeatures.NestedExceptionProofEntry",
                    "CoreRuntimeFeatures.ObjectFieldProofEntry",
                    "CoreRuntimeFeatures.ObjectReturnProofEntry",
                    "CoreRuntimeFeatures.RefStructAndStackallocProofEntry",
                    "CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry",
                    "CoreRuntimeFeatures.ResourceLifecycleProofEntry",
                    "CoreRuntimeFeatures.ReflectionInteropClosureEntry",
                    "CoreRuntimeFeatures.RuntimeHelpersProofEntry",
                    "CoreRuntimeFeatures.SpanAndMemoryProofEntry",
                    "CoreRuntimeFeatures.StructMarshalingProofEntry",
                    "CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry",
                    "CoreRuntimeFeatures.TaskSchedulingProofEntry",
                    "CoreRuntimeFeatures.ThreadingProofEntry",
                    "CoreRuntimeFeatures.ThreadLocalStateProofEntry",
                    "CoreRuntimeFeatures.UInt64BitwiseProofEntry",
                    "CoreRuntimeFeatures.UnsafePointerProofEntry",
                    "CoreRuntimeFeatures.DispatchProofEntry",
                    "CoreRuntimeFeatures.OverflowOpsProofEntry",
                    "CoreRuntimeFeatures.VTableDispatchProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                "arithmetic-ops-proof",
                unit_entries["CoreRuntimeFeatures.ArithmeticOpsProofEntry"]["alias"],
            )
            self.assertEqual("array-ops-proof", unit_entries["CoreRuntimeFeatures.ArrayOpsProofEntry"]["alias"])
            self.assertEqual("bitwise-ops-proof", unit_entries["CoreRuntimeFeatures.BitwiseOpsProofEntry"]["alias"])
            self.assertEqual(
                "finally-filter-proof",
                unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["alias"],
            )
            self.assertEqual(
                "gc-sensitive-flow-proof",
                unit_entries["CoreRuntimeFeatures.GcSensitiveFlowProofEntry"]["alias"],
            )
            self.assertEqual("interface-dispatch-proof", unit_entries["CoreRuntimeFeatures.InterfaceDispatchProofEntry"]["alias"])
            self.assertEqual(
                "iterator-state-machine-proof",
                unit_entries["CoreRuntimeFeatures.IteratorStateMachineProofEntry"]["alias"],
            )
            self.assertEqual("vtable-dispatch-proof", unit_entries["CoreRuntimeFeatures.VTableDispatchProofEntry"]["alias"])
            self.assertEqual("linker-stripping-proof", unit_entries["CoreRuntimeFeatures.LinkerStrippingProofEntry"]["alias"])
            self.assertEqual("loader-contract-proof", unit_entries["CoreRuntimeFeatures.LoaderContractProofEntry"]["alias"])
            self.assertEqual("marshaling-proof", unit_entries["CoreRuntimeFeatures.MarshalingProofEntry"]["alias"])
            self.assertEqual(
                "monitor-locking-proof",
                unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["alias"],
            )
            self.assertEqual("cross-boundary-exception-proof", unit_entries["CoreRuntimeFeatures.CrossBoundaryExceptionProofEntry"]["alias"])
            self.assertEqual(
                "custom-attribute-lookup-proof",
                unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["alias"],
            )
            self.assertEqual(
                "delegate-callback-interop-proof",
                unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["alias"],
            )
            self.assertEqual("function-pointer-proof", unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["alias"])
            self.assertEqual("generic-collection-proof", unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["alias"])
            self.assertEqual(
                "generic-sharing-boundary-proof",
                unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["alias"],
            )
            self.assertEqual(
                "linker-preserve-contract-proof",
                unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["alias"],
            )
            self.assertEqual("nested-exception-proof", unit_entries["CoreRuntimeFeatures.NestedExceptionProofEntry"]["alias"])
            self.assertEqual(
                "native-call-interop-proof",
                unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["alias"],
            )
            self.assertEqual(
                "member-metadata-lookup-proof",
                unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["alias"],
            )
            self.assertEqual(
                "ref-struct-stackalloc-proof",
                unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["alias"],
            )
            self.assertEqual(
                "required-instantiation-closure-proof",
                unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["alias"],
            )
            self.assertEqual(
                "resource-lifecycle-proof",
                unit_entries["CoreRuntimeFeatures.ResourceLifecycleProofEntry"]["alias"],
            )
            self.assertEqual("runtime-helpers-proof", unit_entries["CoreRuntimeFeatures.RuntimeHelpersProofEntry"]["alias"])
            self.assertEqual("span-memory-proof", unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["alias"])
            self.assertEqual("struct-marshaling-proof", unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["alias"])
            self.assertEqual(
                "task-valuetask-flow-proof",
                unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["alias"],
            )
            self.assertEqual("task-scheduling-proof", unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["alias"])
            self.assertEqual("thread-local-state-proof", unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["alias"])
            self.assertEqual("unsafe-pointer-proof", unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["alias"])
            self.assertEqual("delegate-chain-proof", unit_entries["CoreRuntimeFeatures.DelegateChainProofEntry"]["alias"])
            self.assertEqual("dispatch-proof", unit_entries["CoreRuntimeFeatures.DispatchProofEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in unit_entries.values()))
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.ArithmeticOpsProofEntry"]["capabilityFamily"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.ArithmeticOpsProofEntry"]["capabilityItem"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.ArrayOpsProofEntry"]["capabilityFamily"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.ArrayOpsProofEntry"]["capabilityItem"])
            self.assertEqual(3, unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(15, unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["capabilityItem"])
            self.assertEqual(3, unit_entries["CoreRuntimeFeatures.IteratorStateMachineProofEntry"]["capabilityFamily"])
            self.assertEqual(16, unit_entries["CoreRuntimeFeatures.IteratorStateMachineProofEntry"]["capabilityItem"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["capabilityFamily"])
            self.assertEqual(29, unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["capabilityItem"])
            self.assertEqual(2, unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["capabilityFamily"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["capabilityItem"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["capabilityFamily"])
            self.assertEqual(21, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["capabilityFamily"])
            self.assertEqual(34, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["capabilityFamily"])
            self.assertEqual(46, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["capabilityItem"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["capabilityFamily"])
            self.assertEqual(52, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["capabilityItem"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["capabilityFamily"])
            self.assertEqual(25, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["capabilityItem"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["capabilityFamily"])
            self.assertEqual(22, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["capabilityFamily"])
            self.assertEqual(31, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["capabilityFamily"])
            self.assertEqual(44, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["capabilityItem"])
            self.assertEqual(12, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["capabilityFamily"])
            self.assertEqual(51, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["capabilityItem"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["capabilityFamily"])
            self.assertEqual(36, unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["capabilityItem"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["capabilityFamily"])
            self.assertEqual(37, unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["capabilityItem"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["capabilityFamily"])
            self.assertEqual(38, unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.RuntimeHelpersProofEntry"]["capabilityFamily"])
            self.assertEqual(39, unit_entries["CoreRuntimeFeatures.RuntimeHelpersProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.GcSensitiveFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(40, unit_entries["CoreRuntimeFeatures.GcSensitiveFlowProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.ResourceLifecycleProofEntry"]["capabilityFamily"])
            self.assertEqual(41, unit_entries["CoreRuntimeFeatures.ResourceLifecycleProofEntry"]["capabilityItem"])
            self.assertEqual(9, unit_entries["CoreRuntimeFeatures.LoaderContractProofEntry"]["capabilityFamily"])
            self.assertEqual(42, unit_entries["CoreRuntimeFeatures.LoaderContractProofEntry"]["capabilityItem"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.LinkerStrippingProofEntry"]["capabilityFamily"])
            self.assertEqual(26, unit_entries["CoreRuntimeFeatures.LinkerStrippingProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["capabilityFamily"])
            self.assertEqual(43, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["capabilityFamily"])
            self.assertEqual(33, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["capabilityItem"])
            self.assertEqual(10, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["capabilityFamily"])
            self.assertEqual(45, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["capabilityItem"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.GenericCollectionProofEntry"]["requires"])
            self.assertEqual(2, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.CrossBoundaryExceptionProofEntry"]["requires"])
            self.assertEqual(20, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.FinallyAndFilterProofEntry"]["requires"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.DelegateChainProofEntry"]["requires"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.DelegateProofEntry"]["requires"])
            self.assertEqual(8, unit_entries["CoreRuntimeFeatures.ExceptionProofEntry"]["requires"])
            self.assertEqual(2048, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["requires"])
            self.assertEqual(8193, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["requires"])
            self.assertEqual(4098, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["requires"])
            self.assertEqual(2, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["requires"])
            self.assertEqual(16, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["requires"])
            self.assertEqual(16896, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["requires"])
            self.assertEqual(18, unit_entries["CoreRuntimeFeatures.ReflectionInteropClosureEntry"]["requires"])
            self.assertEqual(1, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["requires"])
            self.assertEqual(512, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["requires"])
            self.assertEqual(16, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["requires"])
            self.assertEqual(64, unit_entries["CoreRuntimeFeatures.TaskAndValueTaskFlowProofEntry"]["requires"])
            self.assertEqual(384, unit_entries["CoreRuntimeFeatures.TaskSchedulingProofEntry"]["requires"])
            self.assertEqual(256, unit_entries["CoreRuntimeFeatures.MonitorAndLockingProofEntry"]["requires"])
            self.assertEqual(128, unit_entries["CoreRuntimeFeatures.ThreadLocalStateProofEntry"]["requires"])
            self.assertEqual(1024, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["requires"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.CustomAttributeLookupProofEntry"]["archetype"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.DelegateCallbackInteropProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.FunctionPointerProofEntry"]["archetype"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.GenericSharingBoundaryProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.LinkerPreserveContractProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.MemberMetadataLookupProofEntry"]["archetype"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.NativeCallInteropProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.RefStructAndStackallocProofEntry"]["archetype"])
            self.assertEqual(6, unit_entries["CoreRuntimeFeatures.RequiredInstantiationClosureProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.SpanAndMemoryProofEntry"]["archetype"])
            self.assertEqual(4, unit_entries["CoreRuntimeFeatures.StructMarshalingProofEntry"]["archetype"])
            self.assertEqual(5, unit_entries["CoreRuntimeFeatures.UnsafePointerProofEntry"]["archetype"])
            self.assertTrue(all(entry["evidence"] == 0 for entry in unit_entries.values()))
        finally:
            shutil.rmtree(build_root, ignore_errors=True)
