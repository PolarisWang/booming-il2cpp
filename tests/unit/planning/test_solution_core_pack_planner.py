from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"


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
            "subjects/SolutionCorePack/source/Launcher/SolutionCorePack.csproj",
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
            "subjects/SolutionCorePack/source/Archetypes/SimpleLibrarySolution/App/GoldenSimpleLib.App.csproj",
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
            "subjects/SolutionCorePack/source/Archetypes/MultiProjectSolution/App/GoldenMultiProject.App.csproj",
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
            "subjects/SolutionCorePack/source/Archetypes/PackageReferenceSolution/App/GoldenWithPackage.App.csproj",
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
            "subjects/SolutionCorePack/source/Archetypes/ReferenceAssemblySolution/App/GoldenReferenceAssembly.App.csproj",
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
            "subjects/SolutionCorePack/source/Archetypes/CoreLibReferenceSolution/App/GoldenCoreLibReference.App.csproj",
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
            "subjects/SolutionCorePack/source/Archetypes/MixedReferenceClosureSolution/App/GoldenMixedReference.App.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenMixedReference.App/Program::Main()", plan["selection"]["source"]["entry"])


if __name__ == "__main__":
    unittest.main()
