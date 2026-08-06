from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
COLLECTION_SOURCE = (
    REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Runtime" / "Collection" / "ChaosTestCollection.cs"
)
COLLECTION_LOADER_SOURCE = (
    REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Runtime" / "Collection" / "ChaosTestCollectionLoader.cs"
)


class TestFrameworkCollectionContractTests(unittest.TestCase):
    def test_runtime_collection_exposes_version_and_subject_metadata(self) -> None:
        collection_source = COLLECTION_SOURCE.read_text(encoding="utf-8")

        self.assertIn("public int SchemaVersion { get; set; } = 1;", collection_source)
        self.assertIn('public string SubjectId { get; set; } = string.Empty;', collection_source)
        self.assertIn("public bool FrameworkReferenced { get; set; }", collection_source)
        self.assertIn('public string SubjectKind { get; set; } = string.Empty;', collection_source)
        self.assertIn("public List<string> WarningCodes { get; set; } = [];", collection_source)

    def test_runtime_collection_loader_validates_schema_version(self) -> None:
        loader_source = COLLECTION_LOADER_SOURCE.read_text(encoding="utf-8")

        self.assertIn("SchemaVersion", loader_source)
        self.assertIn("unsupported collection schemaVersion", loader_source)


if __name__ == "__main__":
    unittest.main()
