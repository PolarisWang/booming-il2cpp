from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"
SOLUTION_CORE_PACK_HOST_SOLUTION = "verification/catalog/owners/SolutionCorePack/support/host/SolutionCorePack.sln"
SOLUTION_CORE_PACK_HOST_PROJECT = "verification/catalog/owners/SolutionCorePack/support/host/SolutionCorePack.csproj"
SOLUTION_CORE_PACK_PROOFS_PROJECT = "verification/catalog/owners/SolutionCorePack/proofs/CoreRuntimeFeatures/CoreRuntimeFeatures.csproj"
SOLUTION_CORE_PACK_BENCHMARKS_PROJECT = (
    "verification/catalog/owners/SolutionCorePack/benchmarks/CoreRuntimeBenchmarks/CoreRuntimeBenchmarks.csproj"
)


def solution_core_pack_scenario_project_path(solution_name: str, project_name: str) -> str:
    return f"verification/catalog/scenarios/SolutionCorePack/{solution_name}/App/{project_name}.csproj"


def load_subjects_module(module_name: str):
    return load_module(SUBJECTS_MODULE_PATH, module_name)

class SolutionCorePackPlannerTestSupport(unittest.TestCase):
    pass


class RetainedSubjectPlannerTestSupport(unittest.TestCase):
    pass
