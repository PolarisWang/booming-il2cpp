from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]


class DeclaredHostFriendAccessContractTests(unittest.TestCase):
    def test_declared_entry_projects_grant_friend_access_to_generated_hosts(self) -> None:
        project_expectations = {
            REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj": (
                "SolutionCorePack.DeclaredProofHost",
            ),
            REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj": (
                "SolutionCorePack.DeclaredBenchmarkHost",
            ),
            REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "MixedExecutionFeaturePack.csproj": (
                "MixedExecutionFeaturePack.DeclaredProofHost",
                "MixedExecutionFeaturePack.DeclaredBenchmarkHost",
            ),
            REPO_ROOT / "subjects" / "HotUpdateHostPack" / "source" / "HotUpdateHostPack.csproj": (
                "HotUpdateHostPack.DeclaredProofHost",
                "HotUpdateHostPack.DeclaredBenchmarkHost",
            ),
        }

        for project_path, friend_assemblies in project_expectations.items():
            project_text = project_path.read_text(encoding="utf-8")
            for friend_assembly in friend_assemblies:
                with self.subTest(project=project_path.as_posix(), friend=friend_assembly):
                    self.assertIn(friend_assembly, project_text)


if __name__ == "__main__":
    unittest.main()
