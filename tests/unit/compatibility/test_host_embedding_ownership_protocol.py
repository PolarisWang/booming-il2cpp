from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
HOST_EMBEDDING_PROGRAM_PATH = REPO_ROOT / "subjects" / "HostEmbeddingLite" / "source" / "Program.cs"


class Phase3HostEmbeddingOwnershipProtocolTests(unittest.TestCase):
    def test_host_embedding_lite_exercises_gchandle_based_ownership_roundtrip(self) -> None:
        source = HOST_EMBEDDING_PROGRAM_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "GCHandle.Alloc",
            "GCHandleType.Normal",
            "GCHandleType.Weak",
            ".Free()",
            "ExerciseOwnershipProtocol",
            'Console.WriteLine(ownershipSummary);',
            "ownership=",
            "managed->engine-host",
        ]:
            self.assertIn(required_fragment, source)


if __name__ == "__main__":
    unittest.main()
