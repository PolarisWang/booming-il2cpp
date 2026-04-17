from __future__ import annotations

import importlib.util
import json
import sys
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_WORKSPACE_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "subject" / "project_workspace.py"


def load_module(path: Path, module_name: str):
    if not path.is_file():
        raise FileNotFoundError(f"module missing: {path}")

    spec = importlib.util.spec_from_file_location(module_name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"unable to load module: {path}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class CanonicalSolutionProjectPathsTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.workspace_module = load_module(
            PROJECT_WORKSPACE_MODULE_PATH,
            "chaos_canonical_solution_project_paths_workspace",
        )

    def test_canonical_solution_managed_projects_follow_current_engineering_scenario_layout(self) -> None:
        subject_ids = (
            "SolutionCorePack",
            "MixedExecutionFeaturePack",
            "HotUpdateHostPack",
        )

        for subject_id in subject_ids:
            manifest = json.loads(
                (REPO_ROOT / "subjects" / subject_id / "subject.manifest.json").read_text(encoding="utf-8")
            )
            managed_projects = self.workspace_module._subject_managed_projects(REPO_ROOT, manifest)
            project_paths = [str(item.get("projectPath") or "") for item in managed_projects]

            with self.subTest(subject_id=subject_id):
                self.assertTrue(project_paths)
                self.assertFalse(any("Archetypes/" in path for path in project_paths))
                self.assertTrue(any("EngineeringScenarios/" in path for path in project_paths))


if __name__ == "__main__":
    unittest.main()
