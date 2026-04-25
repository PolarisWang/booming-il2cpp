from tests.unit.planning.solution_core_pack_planner_test_support import *


class TestSolutionCorePackPlannerArchetypes(SolutionCorePackPlannerTestSupport):
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
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("SimpleLibrarySolution", "GoldenSimpleLib.App"),
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
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("MultiProjectSolution", "GoldenMultiProject.App"),
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
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("PackageReferenceSolution", "GoldenWithPackage.App"),
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
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("ReferenceAssemblySolution", "GoldenReferenceAssembly.App"),
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
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("CoreLibReferenceSolution", "GoldenCoreLibReference.App"),
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
        self.assertEqual(SOLUTION_CORE_PACK_HOST_SOLUTION, plan["selection"]["source"]["path"])
        self.assertEqual(
            solution_core_pack_scenario_project_path("MixedReferenceClosureSolution", "GoldenMixedReference.App"),
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("GoldenMixedReference.App/Program::Main()", plan["selection"]["source"]["entry"])
