from tests.unit.planning.solution_core_pack_planner_test_support import *


class TestSolutionCorePackPlannerRetained(RetainedSubjectPlannerTestSupport):
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
            / "verification"
            / "catalog"
            / "scenarios"
            / "MixedExecutionFeaturePack"
            / "MixedBridgeSolution"
            / "InterpreterArithmeticProof"
            / "InterpreterArithmeticProof.csproj"
        )

        self.assertEqual("MixedExecutionFeaturePack", plan["selection"]["subjectId"])
        self.assertEqual(
            "verification/catalog/owners/MixedExecutionFeaturePack/support/host/MixedExecutionFeaturePack.sln",
            plan["selection"]["source"]["path"],
        )
        self.assertEqual(
            "verification/catalog/owners/MixedExecutionFeaturePack/support/host/MixedExecutionFeaturePack.csproj",
            plan["selection"]["source"]["primaryProjectPath"],
        )
        self.assertEqual("MixedExecutionFeaturePack/MixedExecutionProofEntry::Run()", plan["selection"]["source"]["entry"])
        self.assertTrue(support_project_path.is_file())
