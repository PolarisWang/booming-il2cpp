from tests.unit.compatibility.solution_core_pack_subject_test_support import *


class TestSolutionCorePackSubjectScenarios(SolutionCorePackSubjectTestSupport):
    def test_solution_core_pack_registers_foundation_dll_translation_scenario(self) -> None:
        features_path = SUBJECT_ROOT / "owner.features.json"
        payload = json.loads(features_path.read_text(encoding="utf-8"))
        scenarios = {item["id"]: item for item in payload["engineeringScenarios"]}

        self.assertIn("solution-foundation-dll-translation", scenarios)
        self.assertEqual(5, scenarios["solution-foundation-dll-translation"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/FoundationDllTranslationSolution",
            scenarios["solution-foundation-dll-translation"]["sourcePath"],
        )
        self.assertIn("solution-static-call-ctor-getter", scenarios)
        self.assertEqual(6, scenarios["solution-static-call-ctor-getter"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/StaticCallCtorGetterSolution",
            scenarios["solution-static-call-ctor-getter"]["sourcePath"],
        )
        self.assertIn("solution-constructor-then-instance-call", scenarios)
        self.assertEqual(7, scenarios["solution-constructor-then-instance-call"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/ConstructorThenInstanceCallSolution",
            scenarios["solution-constructor-then-instance-call"]["sourcePath"],
        )
        self.assertIn("solution-delegate-closed-target-relay", scenarios)
        self.assertEqual(8, scenarios["solution-delegate-closed-target-relay"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/DelegateClosedTargetRelaySolution",
            scenarios["solution-delegate-closed-target-relay"]["sourcePath"],
        )
        self.assertIn("solution-exception-throw-catch-finally", scenarios)
        self.assertEqual(9, scenarios["solution-exception-throw-catch-finally"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/ExceptionThrowCatchFinallySolution",
            scenarios["solution-exception-throw-catch-finally"]["sourcePath"],
        )
        self.assertIn("solution-nested-exception-throw-catch-finally", scenarios)
        self.assertEqual(10, scenarios["solution-nested-exception-throw-catch-finally"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/NestedExceptionThrowCatchFinallySolution",
            scenarios["solution-nested-exception-throw-catch-finally"]["sourcePath"],
        )
        self.assertIn("solution-marshaling-utf8-export", scenarios)
        self.assertEqual(11, scenarios["solution-marshaling-utf8-export"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/MarshalingUtf8ExportSolution",
            scenarios["solution-marshaling-utf8-export"]["sourcePath"],
        )
        self.assertIn("solution-reflection-interop-closure", scenarios)
        self.assertEqual(12, scenarios["solution-reflection-interop-closure"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/ReflectionInteropClosureSolution",
            scenarios["solution-reflection-interop-closure"]["sourcePath"],
        )
        self.assertIn("solution-pinvoke-direct-call", scenarios)
        self.assertEqual(13, scenarios["solution-pinvoke-direct-call"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/PInvokeDirectCallSolution",
            scenarios["solution-pinvoke-direct-call"]["sourcePath"],
        )
        self.assertIn("solution-array-boxing-reference-array", scenarios)
        self.assertEqual(14, scenarios["solution-array-boxing-reference-array"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/ArrayBoxingReferenceArraySolution",
            scenarios["solution-array-boxing-reference-array"]["sourcePath"],
        )
        self.assertIn("solution-interface-dispatch-message", scenarios)
        self.assertEqual(15, scenarios["solution-interface-dispatch-message"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/InterfaceDispatchMessageSolution",
            scenarios["solution-interface-dispatch-message"]["sourcePath"],
        )
        self.assertIn("solution-threading-threadstatic-monitor", scenarios)
        self.assertEqual(16, scenarios["solution-threading-threadstatic-monitor"]["code"])
        self.assertEqual(
            "verification/catalog/scenarios/SolutionCorePack/ThreadingThreadStaticMonitorSolution",
            scenarios["solution-threading-threadstatic-monitor"]["sourcePath"],
        )
