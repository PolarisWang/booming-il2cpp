from __future__ import annotations

import json
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SUBJECT_ROOT = REPO_ROOT / "subjects" / "SolutionCorePack"
MANIFEST_PATH = SUBJECT_ROOT / "subject.manifest.json"
SOLUTION_PATH = SUBJECT_ROOT / "source" / "SolutionCorePack.sln"
PRIMARY_PROJECT_PATH = SUBJECT_ROOT / "source" / "Host" / "SolutionCorePack.csproj"
ARCHETYPE_ROOT = SUBJECT_ROOT / "source" / "EngineeringScenarios"
REFERENCE_BUNDLE_ROOT = REPO_ROOT / "assets" / "reference-bundles" / "dotnet-foundation"
CONTROLLED_DLL_ROOT = REPO_ROOT / "src" / "dll" / "dotnet-foundation"
LAUNCHER_PROGRAM_PATH = SUBJECT_ROOT / "source" / "Host" / "Program.cs"


class SolutionCorePackSubjectTests(unittest.TestCase):
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

    def test_solution_core_pack_registers_foundation_dll_translation_scenario(self) -> None:
        features_path = SUBJECT_ROOT / "subject.features.json"
        payload = json.loads(features_path.read_text(encoding="utf-8"))
        scenarios = {item["id"]: item for item in payload["engineeringScenarios"]}

        self.assertIn("solution-foundation-dll-translation", scenarios)
        self.assertEqual(5, scenarios["solution-foundation-dll-translation"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution",
            scenarios["solution-foundation-dll-translation"]["sourcePath"],
        )
        self.assertIn("solution-static-call-ctor-getter", scenarios)
        self.assertEqual(6, scenarios["solution-static-call-ctor-getter"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/StaticCallCtorGetterSolution",
            scenarios["solution-static-call-ctor-getter"]["sourcePath"],
        )
        self.assertIn("solution-constructor-then-instance-call", scenarios)
        self.assertEqual(7, scenarios["solution-constructor-then-instance-call"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ConstructorThenInstanceCallSolution",
            scenarios["solution-constructor-then-instance-call"]["sourcePath"],
        )
        self.assertIn("solution-delegate-closed-target-relay", scenarios)
        self.assertEqual(8, scenarios["solution-delegate-closed-target-relay"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/DelegateClosedTargetRelaySolution",
            scenarios["solution-delegate-closed-target-relay"]["sourcePath"],
        )
        self.assertIn("solution-exception-throw-catch-finally", scenarios)
        self.assertEqual(9, scenarios["solution-exception-throw-catch-finally"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ExceptionThrowCatchFinallySolution",
            scenarios["solution-exception-throw-catch-finally"]["sourcePath"],
        )
        self.assertIn("solution-nested-exception-throw-catch-finally", scenarios)
        self.assertEqual(10, scenarios["solution-nested-exception-throw-catch-finally"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/NestedExceptionThrowCatchFinallySolution",
            scenarios["solution-nested-exception-throw-catch-finally"]["sourcePath"],
        )
        self.assertIn("solution-marshaling-utf8-export", scenarios)
        self.assertEqual(11, scenarios["solution-marshaling-utf8-export"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/MarshalingUtf8ExportSolution",
            scenarios["solution-marshaling-utf8-export"]["sourcePath"],
        )
        self.assertIn("solution-reflection-interop-closure", scenarios)
        self.assertEqual(12, scenarios["solution-reflection-interop-closure"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ReflectionInteropClosureSolution",
            scenarios["solution-reflection-interop-closure"]["sourcePath"],
        )
        self.assertIn("solution-pinvoke-direct-call", scenarios)
        self.assertEqual(13, scenarios["solution-pinvoke-direct-call"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/PInvokeDirectCallSolution",
            scenarios["solution-pinvoke-direct-call"]["sourcePath"],
        )
        self.assertIn("solution-array-boxing-reference-array", scenarios)
        self.assertEqual(14, scenarios["solution-array-boxing-reference-array"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ArrayBoxingReferenceArraySolution",
            scenarios["solution-array-boxing-reference-array"]["sourcePath"],
        )
        self.assertIn("solution-interface-dispatch-message", scenarios)
        self.assertEqual(15, scenarios["solution-interface-dispatch-message"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/InterfaceDispatchMessageSolution",
            scenarios["solution-interface-dispatch-message"]["sourcePath"],
        )
        self.assertIn("solution-threading-threadstatic-monitor", scenarios)
        self.assertEqual(16, scenarios["solution-threading-threadstatic-monitor"]["code"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ThreadingThreadStaticMonitorSolution",
            scenarios["solution-threading-threadstatic-monitor"]["sourcePath"],
        )

    def test_solution_core_pack_manifest_declares_foundation_dll_translation_native_proof_matrix(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        matrices = {item["matrixId"]: item for item in manifest["environmentMatrices"]}

        self.assertIn("windows-foundation-dll-translation-native-proof", matrices)
        matrix = matrices["windows-foundation-dll-translation-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/App/FoundationDllTranslation.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-nested-exception-throw-catch-finally-native-proof", matrices)
        matrix = matrices["windows-nested-exception-throw-catch-finally-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/NestedExceptionThrowCatchFinallySolution/App/NestedExceptionThrowCatchFinally.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-constructor-then-instance-call-native-proof", matrices)
        matrix = matrices["windows-constructor-then-instance-call-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ConstructorThenInstanceCallSolution/App/ConstructorThenInstanceCall.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-static-call-ctor-getter-native-proof", matrices)
        matrix = matrices["windows-static-call-ctor-getter-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/StaticCallCtorGetterSolution/App/StaticCallCtorGetter.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-delegate-closed-target-relay-native-proof", matrices)
        matrix = matrices["windows-delegate-closed-target-relay-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/DelegateClosedTargetRelaySolution/App/DelegateClosedTargetRelay.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-exception-throw-catch-finally-native-proof", matrices)
        matrix = matrices["windows-exception-throw-catch-finally-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ExceptionThrowCatchFinallySolution/App/ExceptionThrowCatchFinally.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-corelib-reference-native-proof", matrices)
        matrix = matrices["windows-corelib-reference-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-marshaling-utf8-export-native-proof", matrices)
        matrix = matrices["windows-marshaling-utf8-export-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/MarshalingUtf8ExportSolution/App/MarshalingUtf8Export.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-reflection-interop-closure-native-proof", matrices)
        matrix = matrices["windows-reflection-interop-closure-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ReflectionInteropClosureSolution/App/ReflectionInteropClosure.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-pinvoke-direct-call-native-proof", matrices)
        matrix = matrices["windows-pinvoke-direct-call-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/PInvokeDirectCallSolution/App/PInvokeDirectCall.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-array-boxing-reference-array-native-proof", matrices)
        matrix = matrices["windows-array-boxing-reference-array-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ArrayBoxingReferenceArraySolution/App/ArrayBoxingReferenceArray.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-interface-dispatch-message-native-proof", matrices)
        matrix = matrices["windows-interface-dispatch-message-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/InterfaceDispatchMessageSolution/App/InterfaceDispatchMessage.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertIn("windows-threading-threadstatic-monitor-native-proof", matrices)
        matrix = matrices["windows-threading-threadstatic-monitor-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ThreadingThreadStaticMonitorSolution/App/ThreadingThreadStaticMonitor.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertEqual(
            [
                "subjects/SolutionCorePack/source/EngineeringScenarios/CoreLibReferenceSolution/HotUpdatePatch/GoldenCoreLibReference.HotUpdatePatch.csproj"
            ],
            manifest["hotUpdate"]["patchProjectPaths"],
        )
        self.assertIn("windows-corelib-reference-hotupdate-proof", matrices)
        matrix = matrices["windows-corelib-reference-hotupdate-proof"]
        self.assertEqual("managed-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual("dotnet-managed", matrix["executionContext"]["toolchainProfile"])
        self.assertEqual("hotupdate-proof-output", matrix["executionContext"]["runtimeProfile"])

        self.assertIn("windows-corelib-reference-native-hotupdate-proof", matrices)
        matrix = matrices["windows-corelib-reference-native-hotupdate-proof"]
        self.assertEqual("native-hotupdate-proof-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertEqual("msvc-reference", matrix["executionContext"]["toolchainProfile"])
        self.assertEqual("native-hotupdate-proof-output", matrix["executionContext"]["runtimeProfile"])
        self.assertEqual(
            {
                "family": "declared-unit-test",
                "stableId": "SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()",
                "alias": "corelib-reference-hotupdate-proof",
                "entryIndex": 60,
            },
            matrix["entrySelection"],
        )

    def test_solution_core_pack_deletes_legacy_subject_owned_native_reference_host(self) -> None:
        self.assertFalse((SUBJECT_ROOT / "validation").exists())

    def test_solution_core_launcher_uses_compact_subject_entry_selection_only(self) -> None:
        launcher_source = LAUNCHER_PROGRAM_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ChaosSubjectEntryArguments.TryParse",
            "ChaosSubjectEntryKind.Proof",
            "ChaosSubjectSlice.CoreRuntimeFeaturesProof",
            "ChaosSubjectSlice.CoreRuntimeInterfaceDispatchProof",
        ]:
            self.assertIn(required_fragment, launcher_source)
        self.assertNotIn("ChaosSourceEntryArguments.TryParse", launcher_source)
        self.assertNotIn("InvokeSourceEntry(", launcher_source)


if __name__ == "__main__":
    unittest.main()


