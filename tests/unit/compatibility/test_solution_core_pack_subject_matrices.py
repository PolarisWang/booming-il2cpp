from tests.unit.compatibility.solution_core_pack_subject_test_support import *


class TestSolutionCorePackSubjectMatrices(SolutionCorePackSubjectTestSupport):
    def test_solution_core_pack_manifest_declares_foundation_dll_translation_native_proof_matrix(self) -> None:
        manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
        matrices = {item["matrixId"]: item for item in manifest["environmentMatrices"]}

        self.assertIn("windows-foundation-dll-translation-native-proof", matrices)
        matrix = matrices["windows-foundation-dll-translation-native-proof"]
        self.assertEqual("proof-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/FoundationDllTranslationSolution/App/FoundationDllTranslation.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/NestedExceptionThrowCatchFinallySolution/App/NestedExceptionThrowCatchFinally.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/ConstructorThenInstanceCallSolution/App/ConstructorThenInstanceCall.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/StaticCallCtorGetterSolution/App/StaticCallCtorGetter.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/DelegateClosedTargetRelaySolution/App/DelegateClosedTargetRelay.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/ExceptionThrowCatchFinallySolution/App/ExceptionThrowCatchFinally.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/MarshalingUtf8ExportSolution/App/MarshalingUtf8Export.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/ReflectionInteropClosureSolution/App/ReflectionInteropClosure.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/PInvokeDirectCallSolution/App/PInvokeDirectCall.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/ArrayBoxingReferenceArraySolution/App/ArrayBoxingReferenceArray.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/InterfaceDispatchMessageSolution/App/InterfaceDispatchMessage.App.csproj",
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
            "verification/catalog/scenarios/SolutionCorePack/ThreadingThreadStaticMonitorSolution/App/ThreadingThreadStaticMonitor.App.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", matrix["source"])

        self.assertEqual(
            [
                "verification/catalog/scenarios/SolutionCorePack/CoreLibReferenceSolution/HotUpdatePatch/GoldenCoreLibReference.HotUpdatePatch.csproj"
            ],
            manifest["hotUpdate"]["patchProjectPaths"],
        )
        self.assertIn("windows-corelib-reference-hotupdate-proof", matrices)
        matrix = matrices["windows-corelib-reference-hotupdate-proof"]
        self.assertEqual("managed-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual("dotnet-managed", matrix["executionContext"]["toolchainProfile"])
        self.assertEqual("hotupdate-proof-output", matrix["executionContext"]["runtimeProfile"])

        self.assertIn("windows-managed-proof", matrices)
        matrix = matrices["windows-managed-proof"]
        self.assertEqual("managed-runtime-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "verification/catalog/owners/SolutionCorePack/proofs/CoreRuntimeFeatures/CoreRuntimeFeatures.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/ProofEntry::Run()", matrix["source"]["entry"])
        self.assertEqual("dotnet-managed", matrix["executionContext"]["toolchainProfile"])
        self.assertEqual("managed-proof-output", matrix["executionContext"]["runtimeProfile"])

        self.assertIn("windows-corelib-reference-native-hotupdate-proof", matrices)
        matrix = matrices["windows-corelib-reference-native-hotupdate-proof"]
        self.assertEqual("native-hotupdate-proof-output", matrix["pipelineId"])
        self.assertEqual(["correctness.dev"], matrix["supportedGoals"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
            matrix["source"]["primaryProjectPath"],
        )
        self.assertEqual("", matrix["source"]["entry"])
        self.assertTrue(matrix["source"]["fullAssemblyClosure"])
        self.assertEqual("msvc-reference", matrix["executionContext"]["toolchainProfile"])
        self.assertEqual("native-hotupdate-proof-output", matrix["executionContext"]["runtimeProfile"])
        self.assertEqual(
            [
                {
                    "assemblyName": "System.Private.CoreLib",
                    "inputAssemblyPath": "src/dll/dotnet-foundation/net8.0/runtime/System.Private.CoreLib.dll",
                }
            ],
            matrix["source"]["supplementalFullClosureAssemblies"],
        )
        self.assertEqual(
            {
                "family": "declared-unit-test",
                "stableId": "SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()",
                "alias": "corelib-reference-hotupdate-proof",
                "entryIndex": 60,
            },
            matrix["entrySelection"],
        )
