from tests.unit.compatibility.solution_core_pack_subject_test_support import *


class TestSolutionCorePackSubjectOwnership(SolutionCorePackSubjectTestSupport):
    def test_solution_core_pack_owns_internal_slices_and_legacy_subject_roots_are_removed(self) -> None:
        self.assertTrue((ARCHETYPE_ROOT / "SimpleLibrarySolution" / "SimpleLibrarySolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "MultiProjectSolution" / "MultiProjectSolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "PackageReferenceSolution" / "PackageReferenceSolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "ReferenceAssemblySolution" / "ReferenceAssemblySolution.sln").is_file())
        self.assertTrue((ARCHETYPE_ROOT / "CoreLibReferenceSolution" / "CoreLibReferenceSolution.sln").is_file())
        self.assertTrue(
            (
                ARCHETYPE_ROOT
                / "CoreLibReferenceSolution"
                / "NativeProofApp"
                / "GoldenCoreLibReference.NativeProofApp.csproj"
            ).is_file()
        )
        self.assertTrue(
            (
                ARCHETYPE_ROOT
                / "CoreLibReferenceSolution"
                / "HotUpdatePatch"
                / "GoldenCoreLibReference.HotUpdatePatch.csproj"
            ).is_file()
        )
        self.assertTrue((ARCHETYPE_ROOT / "MixedReferenceClosureSolution" / "MixedReferenceClosureSolution.sln").is_file())
        self.assertTrue(
            (ARCHETYPE_ROOT / "FoundationDllTranslationSolution" / "FoundationDllTranslationSolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "StaticCallCtorGetterSolution" / "StaticCallCtorGetterSolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "ConstructorThenInstanceCallSolution" / "ConstructorThenInstanceCallSolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "DelegateClosedTargetRelaySolution" / "DelegateClosedTargetRelaySolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "ExceptionThrowCatchFinallySolution" / "ExceptionThrowCatchFinallySolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "NestedExceptionThrowCatchFinallySolution" / "NestedExceptionThrowCatchFinallySolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "MarshalingUtf8ExportSolution" / "MarshalingUtf8ExportSolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "ReflectionInteropClosureSolution" / "ReflectionInteropClosureSolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "PInvokeDirectCallSolution" / "PInvokeDirectCallSolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "ArrayBoxingReferenceArraySolution" / "ArrayBoxingReferenceArraySolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "InterfaceDispatchMessageSolution" / "InterfaceDispatchMessageSolution.sln").is_file()
        )
        self.assertTrue(
            (ARCHETYPE_ROOT / "ThreadingThreadStaticMonitorSolution" / "ThreadingThreadStaticMonitorSolution.sln").is_file()
        )
        self.assertTrue((SUBJECT_ROOT / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj").is_file())
        self.assertTrue((SUBJECT_ROOT / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj").is_file())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "HelloWorld").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "GenericEcho").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "HelloWorldObject").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "ReflectionLite").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "PInvokeLite").exists())
        self.assertFalse((SUBJECT_ROOT / "source" / "Slices" / "HostEmbeddingLite").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionSimpleLib").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionMultiProject").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "SolutionPackageReference").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "MainlineFeaturePack").exists())
        self.assertFalse((REPO_ROOT / "subjects" / "PerformanceFeaturePack").exists())

    def test_solution_core_pack_owns_repository_reference_bundle_and_controlled_official_dll_root(self) -> None:
        self.assertTrue(REFERENCE_BUNDLE_ROOT.is_dir(), msg=f"missing reference bundle root: {REFERENCE_BUNDLE_ROOT}")
        self.assertTrue((REFERENCE_BUNDLE_ROOT / "lib" / "ReferenceGreeter.dll").is_file())
        self.assertTrue((REFERENCE_BUNDLE_ROOT / "README.md").is_file())
        self.assertTrue(CONTROLLED_DLL_ROOT.is_dir(), msg=f"missing controlled dll root: {CONTROLLED_DLL_ROOT}")
        self.assertTrue((CONTROLLED_DLL_ROOT / "README.md").is_file())
        self.assertTrue((CONTROLLED_DLL_ROOT / "net8.0" / "ref" / "System.Runtime.dll").is_file())
        self.assertTrue((CONTROLLED_DLL_ROOT / "net8.0" / "ref" / "System.Console.dll").is_file())
        self.assertTrue((CONTROLLED_DLL_ROOT / "net8.0" / "runtime" / "System.Private.CoreLib.dll").is_file())
        self.assertTrue((CONTROLLED_DLL_ROOT / "net10.0" / "runtime" / "System.Private.CoreLib.dll").is_file())

        corelib_project_path = (
            SUBJECT_ROOT / "source" / "EngineeringScenarios" / "CoreLibReferenceSolution" / "App" / "GoldenCoreLibReference.App.csproj"
        )
        corelib_project_text = corelib_project_path.read_text(encoding="utf-8")
        self.assertIn("<DisableImplicitFrameworkReferences>true</DisableImplicitFrameworkReferences>", corelib_project_text)
        self.assertIn('<FrameworkReference Include="Microsoft.NETCore.App" />', corelib_project_text)
        for name in [
            "mscorlib.dll",
            "netstandard.dll",
            "System.Runtime.dll",
            "System.Runtime.Extensions.dll",
            "System.Console.dll",
            "System.Collections.dll",
            "System.Linq.dll",
        ]:
            self.assertIn(
                fr"..\..\..\..\..\..\src\dll\dotnet-foundation\net8.0\ref\{name}",
                corelib_project_text,
            )
        self.assertNotIn(
            r"..\..\..\..\..\..\assets\reference-bundles\dotnet-foundation\net8.0\System.Runtime.dll",
            corelib_project_text,
        )

        native_proof_project_path = (
            SUBJECT_ROOT
            / "source"
            / "EngineeringScenarios"
            / "CoreLibReferenceSolution"
            / "NativeProofApp"
            / "GoldenCoreLibReference.NativeProofApp.csproj"
        )
        native_proof_project_text = native_proof_project_path.read_text(encoding="utf-8")
        self.assertIn("<DisableImplicitFrameworkReferences>true</DisableImplicitFrameworkReferences>", native_proof_project_text)
        for name in [
            "mscorlib.dll",
            "netstandard.dll",
            "System.Runtime.dll",
            "System.Console.dll",
        ]:
            self.assertIn(
                fr"..\..\..\..\..\..\src\dll\dotnet-foundation\net8.0\ref\{name}",
                native_proof_project_text,
            )

    def test_solution_core_pack_deletes_legacy_subject_owned_native_reference_host(self) -> None:
        self.assertFalse((SUBJECT_ROOT / "validation").exists())
