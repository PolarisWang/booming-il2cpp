from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
PROJECT_PATH = REPO_ROOT / "src" / "tools" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis" / "Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj"
PHASE2_DIR = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-04-phase-2-system-private-corelib-certification-substrate"
)
PHASE3_DIR = (
    REPO_ROOT
    / "docs"
    / "dev"
    / "completed"
    / "20260418-05-phase-3-system-private-corelib-semantic-family-nativeization"
)
TEST_TMP_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "corelib-dll-boundary-inventory"


class CoreLibDllBoundaryInventoryTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        TEST_TMP_ROOT.mkdir(parents=True, exist_ok=True)

    def setUp(self) -> None:
        self.output_dir = TEST_TMP_ROOT / f"run-{uuid.uuid4().hex}"
        self.output_dir.mkdir(parents=True, exist_ok=False)

    def tearDown(self) -> None:
        shutil.rmtree(self.output_dir, ignore_errors=True)

    def test_generator_emits_corelib_dll_boundary_inventory(self) -> None:
        completed = subprocess.run(
            [
                "dotnet",
                "run",
                "--project",
                str(PROJECT_PATH),
                "--",
                "corelib-dll-inventory",
                "--phase2-dir",
                str(PHASE2_DIR),
                "--phase3-dir",
                str(PHASE3_DIR),
                "--output-dir",
                str(self.output_dir),
                "--task-id",
                "corelib-dll-boundary-inventory-test",
            ],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
            check=False,
        )

        self.assertEqual(
            0,
            completed.returncode,
            msg=f"stdout:\n{completed.stdout}\n\nstderr:\n{completed.stderr}",
        )

        inventory_path = self.output_dir / "corelib-dll-boundary-inventory-v1-01.json"
        self.assertTrue(inventory_path.is_file(), msg=f"missing inventory: {inventory_path}")
        payload = json.loads(inventory_path.read_text(encoding="utf-8"))

        self.assertEqual(1, payload["schemaVersion"])
        self.assertEqual("corelib-dll-boundary-inventory-test", payload["taskId"])
        self.assertEqual("System.Private.CoreLib", payload["assemblyName"])
        self.assertEqual("blocked", payload["finalStatus"])
        self.assertIn("corelib-dll-boundary-generated-artifacts-missing", payload["blockers"])

        for tfm in ("net8.0", "net10.0"):
            framework = payload["targetFrameworks"][tfm]
            self.assertEqual(6, framework["familyCount"])
            self.assertGreater(framework["publicMemberCount"], 0)
            self.assertEqual(
                {
                    "abstract",
                    "field-metadata",
                    "internal-call",
                    "managed-cil",
                    "pinvoke",
                    "runtime-special",
                },
                set(framework["families"]),
            )
            self.assertEqual([], framework["generatedArtifacts"])


if __name__ == "__main__":
    unittest.main()
