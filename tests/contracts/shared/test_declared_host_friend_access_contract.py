from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]


class DeclaredHostFriendAccessContractTests(unittest.TestCase):
    def test_declared_entry_projects_do_not_reference_removed_generated_managed_hosts(self) -> None:
        project_paths = (
            REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj",
            REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj",
            REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "MixedExecutionFeaturePack.csproj",
            REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "HotUpdateHostPack.csproj",
        )

        for project_path in project_paths:
            project_text = project_path.read_text(encoding="utf-8")
            with self.subTest(project=project_path.as_posix()):
                self.assertNotIn("DeclaredProofHost", project_text)
                self.assertNotIn("DeclaredBenchmarkHost", project_text)


if __name__ == "__main__":
    unittest.main()
