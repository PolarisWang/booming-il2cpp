from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
PLANNER_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_planner.py"
SUBJECTS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subjects.py"


def load_subjects_module(module_name: str):
    return load_module(SUBJECTS_MODULE_PATH, module_name)

class SolutionCorePackPlannerTestSupport(unittest.TestCase):
    pass


class RetainedSubjectPlannerTestSupport(unittest.TestCase):
    pass
