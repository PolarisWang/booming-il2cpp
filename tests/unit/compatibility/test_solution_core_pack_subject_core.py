from tests.unit.compatibility.solution_core_pack_subject_test_support import *


class TestSolutionCorePackSubjectCore(SolutionCorePackSubjectTestSupport):
    def test_solution_core_pack_manifest_declares_solution_mode_subject(self) -> None:
        self.assertTrue(MANIFEST_PATH.is_file(), msg=f"missing manifest: {MANIFEST_PATH}")

        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))

        self.assertEqual("SolutionCorePack", manifest["subjectId"])
        self.assertEqual("canonical", manifest["category"])
        self.assertEqual("dotnet-solution", manifest["sourceModel"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", manifest["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            manifest["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", manifest["source"]["entry"])
        self.assertEqual({"entryKind": 1, "entrySlice": 7}, manifest["source"]["entrySelection"])
        self.assertEqual(
            "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            manifest["workloadEntry"],
        )

    def test_solution_core_pack_solution_includes_internal_mainline_performance_and_archetype_projects(self) -> None:
        self.assertTrue(SOLUTION_PATH.is_file(), msg=f"missing solution: {SOLUTION_PATH}")
        self.assertTrue(PRIMARY_PROJECT_PATH.is_file(), msg=f"missing primary project: {PRIMARY_PROJECT_PATH}")

        solution_text = SOLUTION_PATH.read_text(encoding="utf-8")
        project_text = PRIMARY_PROJECT_PATH.read_text(encoding="utf-8")

        self.assertIn(r"Host\SolutionCorePack.csproj", solution_text)
        self.assertIn(r"Proofs\CoreRuntimeFeatures\CoreRuntimeFeatures.csproj", solution_text)
        self.assertIn(r"Benchmarks\CoreRuntimeBenchmarks\CoreRuntimeBenchmarks.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\SimpleLibrarySolution\App\GoldenSimpleLib.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\MultiProjectSolution\App\GoldenMultiProject.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\PackageReferenceSolution\App\GoldenWithPackage.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\ReferenceAssemblySolution\App\GoldenReferenceAssembly.App.csproj", solution_text)
        self.assertIn(r"EngineeringScenarios\CoreLibReferenceSolution\App\GoldenCoreLibReference.App.csproj", solution_text)
        self.assertIn(
            r"EngineeringScenarios\CoreLibReferenceSolution\NativeProofApp\GoldenCoreLibReference.NativeProofApp.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\CoreLibReferenceSolution\HotUpdatePatch\GoldenCoreLibReference.HotUpdatePatch.csproj",
            solution_text,
        )
        self.assertIn(r"EngineeringScenarios\MixedReferenceClosureSolution\App\GoldenMixedReference.App.csproj", solution_text)
        self.assertIn(
            r"EngineeringScenarios\FoundationDllTranslationSolution\App\FoundationDllTranslation.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\StaticCallCtorGetterSolution\App\StaticCallCtorGetter.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\ConstructorThenInstanceCallSolution\App\ConstructorThenInstanceCall.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\DelegateClosedTargetRelaySolution\App\DelegateClosedTargetRelay.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\ExceptionThrowCatchFinallySolution\App\ExceptionThrowCatchFinally.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\NestedExceptionThrowCatchFinallySolution\App\NestedExceptionThrowCatchFinally.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\MarshalingUtf8ExportSolution\App\MarshalingUtf8Export.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\ReflectionInteropClosureSolution\App\ReflectionInteropClosure.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\PInvokeDirectCallSolution\App\PInvokeDirectCall.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\ArrayBoxingReferenceArraySolution\App\ArrayBoxingReferenceArray.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\InterfaceDispatchMessageSolution\App\InterfaceDispatchMessage.App.csproj",
            solution_text,
        )
        self.assertIn(
            r"EngineeringScenarios\ThreadingThreadStaticMonitorSolution\App\ThreadingThreadStaticMonitor.App.csproj",
            solution_text,
        )
        self.assertNotIn(r"Slices\HelloWorld\HelloWorld.csproj", solution_text)
        self.assertNotIn(r"Slices\GenericEcho\GenericEcho.csproj", solution_text)
        self.assertNotIn(r"Slices\HelloWorldObject\HelloWorldObject.csproj", solution_text)
        self.assertNotIn(r"Slices\ReflectionLite\ReflectionLite.csproj", solution_text)
        self.assertNotIn(r"Slices\PInvokeLite\PInvokeLite.csproj", solution_text)
        self.assertNotIn(r"Slices\HostEmbeddingLite\HostEmbeddingLite.csproj", solution_text)
        self.assertIn(r"..\Proofs\CoreRuntimeFeatures\CoreRuntimeFeatures.csproj", project_text)
        self.assertIn(r"..\Benchmarks\CoreRuntimeBenchmarks\CoreRuntimeBenchmarks.csproj", project_text)
        self.assertIn(r"..\EngineeringScenarios\ReferenceAssemblySolution\App\GoldenReferenceAssembly.App.csproj", project_text)
        self.assertIn(r"..\EngineeringScenarios\CoreLibReferenceSolution\App\GoldenCoreLibReference.App.csproj", project_text)
        self.assertIn(
            r"..\EngineeringScenarios\CoreLibReferenceSolution\NativeProofApp\GoldenCoreLibReference.NativeProofApp.csproj",
            project_text,
        )
        self.assertIn(r"..\EngineeringScenarios\MixedReferenceClosureSolution\App\GoldenMixedReference.App.csproj", project_text)
        self.assertIn(
            r"..\EngineeringScenarios\FoundationDllTranslationSolution\App\FoundationDllTranslation.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\StaticCallCtorGetterSolution\App\StaticCallCtorGetter.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\ConstructorThenInstanceCallSolution\App\ConstructorThenInstanceCall.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\DelegateClosedTargetRelaySolution\App\DelegateClosedTargetRelay.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\ExceptionThrowCatchFinallySolution\App\ExceptionThrowCatchFinally.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\NestedExceptionThrowCatchFinallySolution\App\NestedExceptionThrowCatchFinally.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\MarshalingUtf8ExportSolution\App\MarshalingUtf8Export.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\ReflectionInteropClosureSolution\App\ReflectionInteropClosure.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\PInvokeDirectCallSolution\App\PInvokeDirectCall.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\ArrayBoxingReferenceArraySolution\App\ArrayBoxingReferenceArray.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\InterfaceDispatchMessageSolution\App\InterfaceDispatchMessage.App.csproj",
            project_text,
        )
        self.assertIn(
            r"..\EngineeringScenarios\ThreadingThreadStaticMonitorSolution\App\ThreadingThreadStaticMonitor.App.csproj",
            project_text,
        )
        self.assertNotIn(r"..\..\..\MainlineFeaturePack\source\MainlineFeaturePack.csproj", project_text)
        self.assertNotIn(r"..\..\..\PerformanceFeaturePack\source\PerformanceFeaturePack.csproj", project_text)
