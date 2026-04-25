from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
OWNER_ROOT = REPO_ROOT / "verification" / "catalog" / "owners"


class DeclaredHostFriendAccessContractTests(unittest.TestCase):
    def test_declared_entry_projects_do_not_reference_removed_generated_managed_hosts(self) -> None:
        project_paths = (
            OWNER_ROOT / "SolutionCorePack" / "proofs" / "CoreRuntimeFeatures" / "CoreRuntimeFeatures.csproj",
            OWNER_ROOT / "SolutionCorePack" / "benchmarks" / "CoreRuntimeBenchmarks" / "CoreRuntimeBenchmarks.csproj",
            OWNER_ROOT / "MixedExecutionFeaturePack" / "support" / "host" / "MixedExecutionFeaturePack.csproj",
            OWNER_ROOT / "HotUpdateHostPack" / "support" / "host" / "HotUpdateHostPack.csproj",
        )

        for project_path in project_paths:
            project_text = project_path.read_text(encoding="utf-8")
            with self.subTest(project=project_path.as_posix()):
                self.assertNotIn("DeclaredProofHost", project_text)
                self.assertNotIn("DeclaredBenchmarkHost", project_text)


if __name__ == "__main__":
    unittest.main()
