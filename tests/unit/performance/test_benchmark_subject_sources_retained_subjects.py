from tests.unit.performance.benchmark_subject_sources_test_support import *


class TestBenchmarkSubjectSourcesRetainedSubjects(BenchmarkSubjectSourceTestSupport):
    def test_hot_update_host_pack_declares_hot_update_unit_and_benchmark_slices(self) -> None:
        manifest_path = HOT_UPDATE_HOST_PACK_OWNER_MANIFEST_PATH
        project_path = HOT_UPDATE_HOST_PACK_ROOT / "support" / "host" / "HotUpdateHostPack.csproj"
        program_path = HOT_UPDATE_HOST_PACK_ROOT / "support" / "host" / "Program.cs"
        skeleton_path = HOT_UPDATE_HOST_PACK_ROOT / "proofs" / "HotUpdateSkeletonProofEntry.cs"
        patch_callback_path = HOT_UPDATE_HOST_PACK_ROOT / "proofs" / "PatchCallbackFlowProofEntry.cs"
        replacement_path = HOT_UPDATE_HOST_PACK_ROOT / "proofs" / "MethodReplacementProofEntry.cs"
        shared_proof_path = HOT_UPDATE_HOST_PACK_ROOT / "proofs" / "SharedContractProofEntry.cs"
        shared_contract_path = HOT_UPDATE_HOST_PACK_ROOT / "support" / "shared-contracts" / "ContractIdentityWitness.cs"
        rollback_path = HOT_UPDATE_HOST_PACK_ROOT / "proofs" / "VersionRollbackProofEntry.cs"
        dispatch_path = HOT_UPDATE_HOST_PACK_ROOT / "benchmarks" / "HotUpdateDispatchBenchmark.cs"
        load_path = HOT_UPDATE_HOST_PACK_ROOT / "benchmarks" / "HotUpdateLoadBenchmark.cs"
        method_replacement_benchmark_path = HOT_UPDATE_HOST_PACK_ROOT / "benchmarks" / "MethodReplacementBenchmark.cs"
        roundtrip_path = HOT_UPDATE_HOST_PACK_ROOT / "benchmarks" / "HotUpdateRoundtripBenchmark.cs"
        package_support_path = HOT_UPDATE_HOST_PACK_ROOT / "support" / "patch" / "HotUpdatePackageSupport.cs"
        full_project_solution_path = (
            REPO_ROOT / "verification" / "catalog" / "scenarios" / "HotUpdateHostPack" / "FullProjectHotUpdateSolution" / "FullProjectHotUpdateSolution.sln"
        )
        full_project_host_path = (
            REPO_ROOT / "verification" / "catalog" / "scenarios" / "HotUpdateHostPack" / "FullProjectHotUpdateSolution" / "HostApp" / "Program.cs"
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
        project_source = project_path.read_text(encoding="utf-8")
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
        self.assertEqual("verification/catalog/owners/HotUpdateHostPack/support/host/HotUpdateHostPack.sln", manifest["source"]["path"])
        self.assertEqual("verification/catalog/owners/HotUpdateHostPack/support/host/HotUpdateHostPack.csproj", manifest["source"]["primaryProjectPath"])
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
            "verification/catalog/scenarios/HotUpdateHostPack/FullProjectHotUpdateSolution/HostApp/GoldenHotUpdateHost.App.csproj",
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
        manifest_path = MIXED_EXECUTION_FEATURE_PACK_OWNER_MANIFEST_PATH
        project_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "support" / "host" / "MixedExecutionFeaturePack.csproj"
        program_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "support" / "host" / "Program.cs"
        proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "proofs" / "MixedExecutionProofEntry.cs"
        arithmetic_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "proofs" / "InterpreterArithmeticProofEntry.cs"
        lowering_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "proofs" / "InterpreterLoweringProofEntry.cs"
        generic_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "proofs" / "MixedGenericFlowProofEntry.cs"
        exception_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "proofs" / "MixedExceptionFlowProofEntry.cs"
        delegate_proof_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "proofs" / "MixedDelegateFlowProofEntry.cs"
        benchmark_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "benchmarks" / "MixedExecutionBenchmark.cs"
        native_benchmark_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "benchmarks" / "MixedExecutionNativeBenchmark.cs"
        support_path = MIXED_EXECUTION_FEATURE_PACK_ROOT / "support" / "interpreter" / "InterpreterArithmeticSupport.cs"
        support_solution_path = (
            REPO_ROOT / "verification" / "catalog" / "scenarios" / "MixedExecutionFeaturePack" / "MixedBridgeSolution" / "MixedBridgeSolution.sln"
        )
        arithmetic_support_project_path = (
            REPO_ROOT
            / "verification"
            / "catalog"
            / "scenarios"
            / "MixedExecutionFeaturePack"
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
        project_source = project_path.read_text(encoding="utf-8")
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
        self.assertEqual("verification/catalog/owners/MixedExecutionFeaturePack/support/host/MixedExecutionFeaturePack.sln", manifest["source"]["path"])
        self.assertEqual("verification/catalog/owners/MixedExecutionFeaturePack/support/host/MixedExecutionFeaturePack.csproj", manifest["source"]["primaryProjectPath"])
        self.assertEqual("require", manifest["testDeclarationMode"])
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionNativeBenchmarkEntry::RunWorkload()", manifest["workloadEntry"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertNotIn('<InternalsVisibleTo Include="MixedExecutionFeaturePack.DeclaredBenchmarkHost" />', project_source)
        self.assertIn('<InternalsVisibleTo Include="MixedExecutionFeaturePack.DeclaredBenchmarkNativeHost" />', project_source)
        archetype_matrix = next(
            matrix for matrix in manifest["environmentMatrices"] if matrix["matrixId"] == "windows-archetype-mixed-bridge-managed-output"
        )
        self.assertEqual(
            "verification/catalog/scenarios/MixedExecutionFeaturePack/MixedBridgeSolution/InterpreterArithmeticProof/InterpreterArithmeticProof.csproj",
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
