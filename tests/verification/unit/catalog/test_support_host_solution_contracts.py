from __future__ import annotations

import re
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[4]
PROJECT_LINE_PATTERN = re.compile(r'^Project\(".*?"\) = ".*?", "(?P<path>[^"]+\.csproj)"')


class TestSupportHostSolutionContracts(unittest.TestCase):
    def test_support_host_solutions_reference_existing_projects(self) -> None:
        solution_paths = sorted(
            REPO_ROOT.glob("verification/catalog/owners/*/support/host/*.sln")
        )
        self.assertTrue(solution_paths, "expected support host solutions under verification/catalog/owners/*/support/host")

        for solution_path in solution_paths:
            with self.subTest(solution=str(solution_path.relative_to(REPO_ROOT))):
                text = solution_path.read_text(encoding="utf-8")
                project_paths = [
                    match.group("path")
                    for line in text.splitlines()
                    for match in [PROJECT_LINE_PATTERN.match(line)]
                    if match is not None
                ]
                self.assertTrue(project_paths, "expected at least one project reference in support host solution")

                for relative_project_path in project_paths:
                    resolved_path = (solution_path.parent / relative_project_path).resolve()
                    self.assertTrue(
                        resolved_path.is_file(),
                        f"missing project reference {relative_project_path} from {solution_path.relative_to(REPO_ROOT)}",
                    )
                    self.assertNotIn(
                        "/owners/scenarios/",
                        resolved_path.as_posix(),
                        "support host solution should resolve scenarios from verification/catalog/scenarios",
                    )
