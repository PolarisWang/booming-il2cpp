from tests.integration.registry.declared_metadata_discovery_test_support import *


class TestDeclaredMetadataDiscoveryRetainedSubjects(DeclaredMetadataDiscoveryTestSupport):
    def test_hot_update_host_pack_discovers_declared_unit_and_benchmark_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_hot_update_host_pack")

        build_root = TEST_TMP_ROOT / "hot-update-host-pack" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(HOT_UPDATE_HOST_PACK_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "HotUpdateHostPack.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="HotUpdateHostPack",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual("declared-test", catalog["subjectKind"])
            self.assertEqual([], catalog["warningCodes"])

            unit_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredUnitTests"]
            }
            benchmark_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredBenchmarks"]
            }

            self.assertEqual(
                {
                    "HotUpdateHostPack.HotUpdateSkeletonProofEntry",
                    "HotUpdateHostPack.MetadataSupplementProofEntry",
                    "HotUpdateHostPack.MethodReplacementProofEntry",
                    "HotUpdateHostPack.PatchIntegrityProofEntry",
                    "HotUpdateHostPack.PatchCallbackFlowProofEntry",
                    "HotUpdateHostPack.SharedContractProofEntry",
                    "HotUpdateHostPack.VersionRollbackProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                {
                    "HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry",
                    "HotUpdateHostPack.HotUpdateLoadBenchmarkEntry",
                    "HotUpdateHostPack.MethodReplacementBenchmarkEntry",
                    "HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("hot-update-skeleton-proof", unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["alias"])
            self.assertEqual("metadata-supplement-proof", unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["alias"])
            self.assertEqual("method-replacement-proof", unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["alias"])
            self.assertEqual("patch-callback-flow-proof", unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["alias"])
            self.assertEqual("shared-contract-proof", unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["alias"])
            self.assertEqual("version-rollback-proof", unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["alias"])
            self.assertEqual("hot-update-dispatch-bench", benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["alias"])
            self.assertEqual("hot-update-load-bench", benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["alias"])
            self.assertEqual("method-replacement-bench", benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["alias"])
            self.assertEqual("hot-update-roundtrip-bench", benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in benchmark_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in benchmark_entries.values()))
            self.assertEqual(13, unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["capabilityFamily"])
            self.assertEqual(54, unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["capabilityFamily"])
            self.assertEqual(57, unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["capabilityFamily"])
            self.assertEqual(56, unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(60, unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["capabilityFamily"])
            self.assertEqual(55, unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["capabilityItem"])
            self.assertEqual(13, unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["capabilityFamily"])
            self.assertEqual(59, unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["capabilityItem"])
            self.assertEqual(7, unit_entries["HotUpdateHostPack.HotUpdateSkeletonProofEntry"]["archetype"])
            self.assertEqual(8, unit_entries["HotUpdateHostPack.MetadataSupplementProofEntry"]["archetype"])
            self.assertEqual(7, unit_entries["HotUpdateHostPack.MethodReplacementProofEntry"]["archetype"])
            self.assertEqual(7, unit_entries["HotUpdateHostPack.PatchCallbackFlowProofEntry"]["archetype"])
            self.assertEqual(8, unit_entries["HotUpdateHostPack.SharedContractProofEntry"]["archetype"])
            self.assertEqual(9, unit_entries["HotUpdateHostPack.VersionRollbackProofEntry"]["archetype"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(60, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(54, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(56, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(13, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(60, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(7, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["archetype"])
            self.assertEqual(7, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["archetype"])
            self.assertEqual(7, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["archetype"])
            self.assertEqual(8, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["archetype"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.HotUpdateDispatchBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.HotUpdateLoadBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.MethodReplacementBenchmarkEntry"]["modes"])
            self.assertEqual(1, benchmark_entries["HotUpdateHostPack.HotUpdateRoundtripBenchmarkEntry"]["modes"])
            self.assertTrue(all(entry["evidence"] == 0 for entry in unit_entries.values()))
        finally:
            shutil.rmtree(build_root, ignore_errors=True)

    def test_mixed_execution_feature_pack_discovers_declared_unit_and_benchmark_entries_from_real_subject_output(self) -> None:
        catalog_module = load_module(COMPILED_CATALOG_MODULE_PATH, "chaos_declared_metadata_mixed_execution_feature_pack")

        build_root = TEST_TMP_ROOT / "mixed-execution-feature-pack" / uuid.uuid4().hex
        build_output_root = build_root / "build"
        intermediate_root = build_root / "obj"
        build_root.mkdir(parents=True, exist_ok=True)
        try:
            run_checked(
                [
                    "dotnet",
                    "build",
                    str(MIXED_EXECUTION_FEATURE_PACK_PROJECT_PATH),
                    "-c",
                    "Release",
                    "-o",
                    str(build_output_root),
                    "-m:1",
                    *dotnet_intermediate_args(intermediate_root),
                ],
                cwd=REPO_ROOT,
            )
            assembly_path = build_output_root / "MixedExecutionFeaturePack.dll"
            self.assertTrue(assembly_path.is_file(), msg=f"missing compiled subject dll: {assembly_path}")

            catalog = catalog_module.build_declared_test_catalog(
                repo_root=REPO_ROOT,
                subject_id="MixedExecutionFeaturePack",
                declaration_mode="require",
                assembly_paths=[assembly_path],
            )

            self.assertTrue(catalog["frameworkReferenced"])
            self.assertEqual("declared-test", catalog["subjectKind"])
            self.assertEqual([], catalog["warningCodes"])

            unit_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredUnitTests"]
            }
            benchmark_entries = {
                entry["declaringType"]: entry
                for entry in catalog["declaredBenchmarks"]
            }

            self.assertEqual(
                {
                    "MixedExecutionFeaturePack.MixedExecutionProofEntry",
                    "MixedExecutionFeaturePack.InterpreterArithmeticProofEntry",
                    "MixedExecutionFeaturePack.InterpreterLoweringProofEntry",
                    "MixedExecutionFeaturePack.MixedGenericFlowProofEntry",
                    "MixedExecutionFeaturePack.MixedExceptionFlowProofEntry",
                    "MixedExecutionFeaturePack.MixedDelegateFlowProofEntry",
                },
                set(unit_entries),
            )
            self.assertEqual(
                {
                    "MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry",
                    "MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry",
                },
                set(benchmark_entries),
            )
            self.assertEqual("mixed-execution-proof", unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["alias"])
            self.assertEqual("interpreter-arithmetic-proof", unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["alias"])
            self.assertEqual("interpreter-lowering-proof", unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["alias"])
            self.assertEqual("mixed-generic-flow-proof", unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["alias"])
            self.assertEqual("mixed-exception-flow-proof", unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["alias"])
            self.assertEqual("mixed-delegate-flow-proof", unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["alias"])
            self.assertEqual("mixed-execution-bench", benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["alias"])
            self.assertEqual("mixed-execution-native-bench", benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["alias"])
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in unit_entries.values()))
            self.assertTrue(all(entry["capabilityFamily"] > 0 for entry in benchmark_entries.values()))
            self.assertTrue(all(entry["capabilityItem"] > 0 for entry in benchmark_entries.values()))
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["capabilityFamily"])
            self.assertEqual(61, unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["capabilityFamily"])
            self.assertEqual(63, unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["capabilityFamily"])
            self.assertEqual(62, unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(64, unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(65, unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["capabilityItem"])
            self.assertEqual(14, unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["capabilityFamily"])
            self.assertEqual(66, unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["capabilityItem"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedExecutionProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.InterpreterArithmeticProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.InterpreterLoweringProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedGenericFlowProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedExceptionFlowProofEntry"]["archetype"])
            self.assertEqual(11, unit_entries["MixedExecutionFeaturePack.MixedDelegateFlowProofEntry"]["archetype"])
            self.assertEqual(14, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(61, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(14, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["capabilityFamily"])
            self.assertEqual(63, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["capabilityItem"])
            self.assertEqual(5, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionBenchmarkEntry"]["modes"])
            self.assertEqual(7, benchmark_entries["MixedExecutionFeaturePack.MixedExecutionNativeBenchmarkEntry"]["modes"])
            self.assertTrue(all(entry["evidence"] == 0 for entry in unit_entries.values()))
        finally:
            shutil.rmtree(build_root, ignore_errors=True)
