from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


def load_subjects_module(module_name: str):
    return load_module(SUBJECTS_MODULE_PATH, module_name)


class SolutionCorePackPlannerTests(unittest.TestCase):
    def test_planner_surfaces_solution_core_pack_default_native_proof_entry(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_default")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            run_id="20260413-solution-core-pack-default-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeFeatures/InterfaceDispatchProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 11,
            },
            plan["selection"]["source"]["entrySelection"],
        )

    def test_planner_surfaces_solution_core_pack_benchmark_entry(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_benchmark")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id="20260413-solution-core-pack-benchmark-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-native-perf", plan["selection"]["matrixId"])
        self.assertEqual(
            "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            plan["selection"]["source"]["entry"],
        )
        self.assertEqual(
            "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()",
            plan["selection"]["workloadEntry"],
        )

    def test_planner_surfaces_solution_core_pack_simple_lib_archetype_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_simple_lib")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-archetype-simple-lib-managed-output",
            run_id="20260413-solution-core-pack-simple-lib-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-archetype-simple-lib-managed-output", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/SimpleLibrarySolution/App/GoldenSimpleLib.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenSimpleLib.App/Program::Main()", plan["selection"]["source"]["entry"])

    def test_planner_surfaces_solution_core_pack_multi_project_archetype_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_multi_project")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-archetype-multi-project-managed-output",
            run_id="20260413-solution-core-pack-multi-project-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-archetype-multi-project-managed-output", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/MultiProjectSolution/App/GoldenMultiProject.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenMultiProject.App/Program::Main()", plan["selection"]["source"]["entry"])

    def test_planner_surfaces_solution_core_pack_package_reference_archetype_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_package_reference")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-archetype-package-reference-managed-output",
            run_id="20260413-solution-core-pack-package-reference-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-archetype-package-reference-managed-output", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/PackageReferenceSolution/App/GoldenWithPackage.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenWithPackage.App/Program::Main()", plan["selection"]["source"]["entry"])

    def test_planner_surfaces_solution_core_pack_reference_assembly_archetype_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_reference_assembly")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-archetype-reference-assembly-managed-output",
            run_id="20260414-solution-core-pack-reference-assembly-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-archetype-reference-assembly-managed-output", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ReferenceAssemblySolution/App/GoldenReferenceAssembly.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenReferenceAssembly.App/Program::Main()", plan["selection"]["source"]["entry"])

    def test_planner_surfaces_solution_core_pack_corelib_reference_archetype_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_corelib_reference")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-archetype-corelib-reference-managed-output",
            run_id="20260414-solution-core-pack-corelib-reference-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-archetype-corelib-reference-managed-output", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/CoreLibReferenceSolution/App/GoldenCoreLibReference.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenCoreLibReference.App/Program::Main()", plan["selection"]["source"]["entry"])

    def test_planner_surfaces_solution_core_pack_mixed_reference_closure_archetype_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_mixed_reference_closure")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-archetype-mixed-reference-closure-managed-output",
            run_id="20260414-solution-core-pack-mixed-reference-closure-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-archetype-mixed-reference-closure-managed-output", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/MixedReferenceClosureSolution/App/GoldenMixedReference.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenMixedReference.App/Program::Main()", plan["selection"]["source"]["entry"])

    def test_planner_surfaces_solution_core_pack_foundation_dll_translation_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_foundation_dll_translation")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-foundation-dll-translation-native-proof",
            run_id="20260419-solution-core-pack-foundation-dll-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-foundation-dll-translation-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/App/FoundationDllTranslation.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_static_call_ctor_getter_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_static_call_ctor_getter")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-static-call-ctor-getter-native-proof",
            run_id="20260419-solution-core-pack-static-call-ctor-getter-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-static-call-ctor-getter-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/StaticCallCtorGetterSolution/App/StaticCallCtorGetter.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_constructor_then_instance_call_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_constructor_then_instance_call")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-constructor-then-instance-call-native-proof",
            run_id="20260419-solution-core-pack-constructor-then-instance-call-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-constructor-then-instance-call-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ConstructorThenInstanceCallSolution/App/ConstructorThenInstanceCall.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_delegate_closed_target_relay_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_delegate_closed_target_relay")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-delegate-closed-target-relay-native-proof",
            run_id="20260419-solution-core-pack-delegate-closed-target-relay-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-delegate-closed-target-relay-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/DelegateClosedTargetRelaySolution/App/DelegateClosedTargetRelay.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_exception_throw_catch_finally_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_exception_throw_catch_finally")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-exception-throw-catch-finally-native-proof",
            run_id="20260419-solution-core-pack-exception-throw-catch-finally-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-exception-throw-catch-finally-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ExceptionThrowCatchFinallySolution/App/ExceptionThrowCatchFinally.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_nested_exception_throw_catch_finally_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_nested_exception_throw_catch_finally")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-nested-exception-throw-catch-finally-native-proof",
            run_id="20260419-solution-core-pack-nested-exception-throw-catch-finally-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-nested-exception-throw-catch-finally-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/NestedExceptionThrowCatchFinallySolution/App/NestedExceptionThrowCatchFinally.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_corelib_reference_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_corelib_reference_native")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-corelib-reference-native-proof",
            run_id="20260419-solution-core-pack-corelib-reference-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-corelib-reference-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_marshaling_utf8_export_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_marshaling_utf8_export")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-marshaling-utf8-export-native-proof",
            run_id="20260419-solution-core-pack-marshaling-utf8-export-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-marshaling-utf8-export-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/MarshalingUtf8ExportSolution/App/MarshalingUtf8Export.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_reflection_interop_closure_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_reflection_interop_closure")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-reflection-interop-closure-native-proof",
            run_id="20260419-solution-core-pack-reflection-interop-closure-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-reflection-interop-closure-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ReflectionInteropClosureSolution/App/ReflectionInteropClosure.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_pinvoke_direct_call_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_pinvoke_direct_call")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-pinvoke-direct-call-native-proof",
            run_id="20260419-solution-core-pack-pinvoke-direct-call-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-pinvoke-direct-call-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/PInvokeDirectCallSolution/App/PInvokeDirectCall.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_array_boxing_reference_array_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_array_boxing_reference_array")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-array-boxing-reference-array-native-proof",
            run_id="20260419-solution-core-pack-array-boxing-reference-array-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-array-boxing-reference-array-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ArrayBoxingReferenceArraySolution/App/ArrayBoxingReferenceArray.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_interface_dispatch_message_native_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_interface_dispatch_message")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-interface-dispatch-message-native-proof",
            run_id="20260419-solution-core-pack-interface-dispatch-message-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-interface-dispatch-message-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/InterfaceDispatchMessageSolution/App/InterfaceDispatchMessage.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_threading_threadstatic_monitor_native_proof_matrix(self) -> None:
        planner_module = load_module(
            PLANNER_MODULE_PATH,
            "chaos_solution_core_pack_planner_threading_threadstatic_monitor",
        )

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-threading-threadstatic-monitor-native-proof",
            run_id="20260420-solution-core-pack-threading-threadstatic-monitor-native-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-threading-threadstatic-monitor-native-proof", plan["selection"]["matrixId"])
        self.assertEqual("proof-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ThreadingThreadStaticMonitorSolution/App/ThreadingThreadStaticMonitor.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertNotIn("entrySelection", plan["selection"]["source"])

    def test_planner_surfaces_solution_core_pack_corelib_reference_hotupdate_proof_matrix(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_corelib_reference_hotupdate")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-corelib-reference-hotupdate-proof",
            run_id="20260419-solution-core-pack-corelib-reference-hotupdate-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-corelib-reference-hotupdate-proof", plan["selection"]["matrixId"])
        self.assertEqual("managed-runtime-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Host/SolutionCorePack.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("dotnet-managed", plan["selection"]["executionContext"]["toolchainProfile"])
        self.assertEqual("hotupdate-proof-output", plan["selection"]["executionContext"]["runtimeProfile"])

    def test_planner_surfaces_solution_core_pack_corelib_reference_native_hotupdate_proof_matrix(self) -> None:
        planner_module = load_module(
            PLANNER_MODULE_PATH,
            "chaos_solution_core_pack_planner_corelib_reference_native_hotupdate",
        )

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            matrix_id="windows-corelib-reference-native-hotupdate-proof",
            run_id="20260420-solution-core-pack-corelib-reference-native-hotupdate-proof-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual("windows-corelib-reference-native-hotupdate-proof", plan["selection"]["matrixId"])
        self.assertEqual("native-hotupdate-proof-output", plan["selection"]["pipelineId"])
        self.assertEqual("subjects/SolutionCorePack/source/SolutionCorePack.sln", plan["selection"]["source"]["path"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/CoreLibReferenceSolution/NativeProofApp/GoldenCoreLibReference.NativeProofApp.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("", plan["selection"]["source"]["entry"])
        self.assertTrue(plan["selection"]["source"]["fullAssemblyClosure"])
        self.assertEqual("msvc-reference", plan["selection"]["executionContext"]["toolchainProfile"])
        self.assertEqual("native-hotupdate-proof-output", plan["selection"]["executionContext"]["runtimeProfile"])
        self.assertEqual(
            {
                "family": "declared-unit-test",
                "stableId": "SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()",
                "alias": "corelib-reference-hotupdate-proof",
                "entryIndex": 60,
            },
            plan["selection"]["entrySelection"],
        )

    def test_planner_can_override_declared_unit_entry_and_emit_family_specific_report_roots(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_declared_unit_selection")
        run_id = "20260413-mainline-array-ops-entry-selection-001"
        stable_id = "SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ArrayOpsProofEntry::Run()"

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="correctness.dev",
            matrix_id="windows-native-check",
            run_id=run_id,
            source_entry="CoreRuntimeFeatures/ArrayOpsProofEntry::Run()",
            entry_selection={
                "family": "declared-unit-test",
                "stableId": stable_id,
                "alias": "array-ops-proof",
            },
        )

        self.assertEqual("CoreRuntimeFeatures/ArrayOpsProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "family": "declared-unit-test",
                "stableId": stable_id,
                "alias": "array-ops-proof",
            },
            plan["selection"]["entrySelection"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/matrices/windows-native-check/declared/unit/array-ops-proof/report.json",
            plan["artifactsRoot"]["entryReportPath"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/declared/unit/array-ops-proof/summary.json",
            plan["artifactsRoot"]["entrySummaryPath"],
        )

    def test_planner_can_override_declared_benchmark_entry_and_emit_family_specific_report_roots(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_declared_benchmark_selection")
        run_id = "20260413-performance-generic-bench-entry-selection-001"
        stable_id = "SolutionCorePack::CoreRuntimeBenchmarks::CoreRuntimeBenchmarks.GenericBenchmarkEntry::RunWorkload()"

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id=run_id,
            source_entry="CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()",
            workload_entry="CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()",
            entry_selection={
                "family": "declared-benchmark",
                "stableId": stable_id,
                "alias": "generic-bench",
            },
        )

        self.assertEqual("CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/GenericBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])
        self.assertEqual(
            {
                "family": "declared-benchmark",
                "stableId": stable_id,
                "alias": "generic-bench",
            },
            plan["selection"]["entrySelection"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/matrices/windows-native-perf/declared/benchmark/generic-bench/report.json",
            plan["artifactsRoot"]["entryReportPath"],
        )
        self.assertEqual(
            f"artifacts/subjects/SolutionCorePack/runs/{run_id}/declared/benchmark/generic-bench/summary.json",
            plan["artifactsRoot"]["entrySummaryPath"],
        )

    def test_planner_surfaces_matrix_subject_entry_selection_for_solution_core_pack_managed_output(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_subject_entry_selection")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="correctness.dev",
            matrix_id="windows-archetype-simple-lib-managed-output",
            run_id="20260413-solution-core-simple-lib-entry-selection-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/SimpleLibrarySolution/App/GoldenSimpleLib.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenSimpleLib.App/Program::Main()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 4,
            },
            plan["selection"]["source"]["entrySelection"],
        )

    def test_planner_surfaces_matrix_subject_entry_selection_for_solution_core_pack_reference_assembly_output(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_reference_entry_selection")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="correctness.dev",
            matrix_id="windows-archetype-reference-assembly-managed-output",
            run_id="20260414-solution-core-reference-entry-selection-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/EngineeringScenarios/ReferenceAssemblySolution/App/GoldenReferenceAssembly.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenReferenceAssembly.App/Program::Main()", plan["selection"]["source"]["entry"])
        self.assertEqual(
            {
                "entryKind": 1,
                "entrySlice": 8,
            },
            plan["selection"]["source"]["entrySelection"],
        )

    def test_planner_selection_surfaces_thin_manifest_fields(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_thin_manifest_selection")
        subjects_module = load_subjects_module("chaos_solution_core_pack_planner_thin_manifest_subjects")
        manifest = subjects_module.load_subject_manifest(REPO_ROOT, "SolutionCorePack")

        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id="20260413-solutioncorepack-thin-selection-001",
        )

        self.assertEqual(manifest["sourceModel"], plan["selection"]["sourceModel"])
        self.assertEqual(manifest["dependencyModel"], plan["selection"]["dependencyModel"])
        self.assertEqual(manifest["executablePlan"], plan["selection"]["executablePlan"])
        self.assertEqual(manifest["engineeringProfile"], plan["selection"]["engineeringProfile"])
        self.assertEqual(manifest["availability"], plan["selection"]["availability"])
        self.assertEqual(manifest["compatibility"], plan["selection"]["compatibility"])

    def test_native_benchmark_matrix_uses_workload_entry_as_source_entry(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_native_workload_entry")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "SolutionCorePack",
            goal_id="perf.release",
            matrix_id="windows-native-perf",
            run_id="20260413-solutioncorepack-native-workload-entry-001",
        )

        self.assertEqual("SolutionCorePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/CoreRuntimeBenchmarks.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["source"]["entry"])
        self.assertEqual("CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload()", plan["selection"]["workloadEntry"])

    def test_planner_selects_native_perf_matrix_for_solution_core_pack(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_native_perf")
        subject_id = "SolutionCorePack"
        run_id = "20260413-fixture-solution-core-native-perf-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            validation_profile_id="perf-profile",
            variant="PROFILE",
            run_id=run_id,
        )

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("perf.release", plan["selection"]["goalId"])
        self.assertEqual("windows-native-perf", plan["selection"]["matrixId"])
        self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
        self.assertEqual("PROFILE", plan["selection"]["variant"])
        self.assertEqual("native-benchmark", plan["selection"]["pipelineId"])
        self.assertEqual(
            "subjects/SolutionCorePack/source/Benchmarks/CoreRuntimeBenchmarks/CoreRuntimeBenchmarks.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual(
            [
                "source-resolve",
                "host-input-build",
                "analysis-frontend",
                "generated-native-aot",
                "build-target",
                "native-runtime-perf",
                "report-assemble",
            ],
            [stage["stageId"] for stage in plan["stagePlan"]],
        )
        native_perf_stage = next(stage for stage in plan["stagePlan"] if stage["kind"] == "native-runtime-perf")
        self.assertEqual(
            f"artifacts/subjects/{subject_id}/runs/{run_id}/matrices/windows-native-perf/runtime/runtime.manifest.json",
            native_perf_stage["paths"]["manifestPath"],
        )
        self.assertEqual("report", plan["selection"]["artifactPlan"]["evidenceTerminalBucket"])

    def test_planner_uses_goal_aware_perf_defaults_for_solution_core_pack_native_perf(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_solution_core_pack_planner_native_perf_goal_defaults")
        subject_id = "SolutionCorePack"
        run_id = "20260413-fixture-solution-core-native-perf-goal-defaults-001"

        plan = planner_module.build_plan(
            REPO_ROOT,
            subject_id,
            goal_id="perf.release",
            run_id=run_id,
        )

        self.assertEqual(subject_id, plan["selection"]["subjectId"])
        self.assertEqual("perf.release", plan["selection"]["goalId"])
        self.assertEqual("windows-native-perf", plan["selection"]["matrixId"])
        self.assertEqual("perf-profile", plan["selection"]["validationProfileId"])
        self.assertEqual("perf", plan["selection"]["validationKind"])
        self.assertEqual("PROFILE", plan["selection"]["variant"])
        self.assertEqual("native-benchmark", plan["selection"]["pipelineId"])


class RetainedSubjectPlannerTests(unittest.TestCase):
    def test_planner_uses_canonical_pack_source_while_interpreter_support_project_remains_in_archetype_tree(self) -> None:
        planner_module = load_module(PLANNER_MODULE_PATH, "chaos_retained_subject_planner_interpreter_arithmetic_shell")
        plan = planner_module.build_plan(
            REPO_ROOT,
            "MixedExecutionFeaturePack",
            goal_id="correctness.dev",
            matrix_id="windows-managed-output",
            run_id="20260413-interpreterarithmeticproof-shell-entry-001",
        )

        support_project_path = (
            REPO_ROOT
            / "subjects"
            / "MixedExecutionFeaturePack"
            / "source"
            / "EngineeringScenarios"
            / "MixedBridgeSolution"
            / "InterpreterArithmeticProof"
            / "InterpreterArithmeticProof.csproj"
        )

        self.assertEqual("MixedExecutionFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.sln",
            plan["selection"]["source"]["path"],
        )
        self.assertEqual(
            "subjects/MixedExecutionFeaturePack/source/MixedExecutionFeaturePack.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertTrue(support_project_path.is_file())


if __name__ == "__main__":
    unittest.main()

