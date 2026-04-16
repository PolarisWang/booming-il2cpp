from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
AOT_CORE_IR_LOWERING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "AotCoreIrLowering.cs"
METADATA_WRITER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.MetadataWriter" / "MetadataWriterStage.cs"


class Phase4CAotCoreIrGenericsTests(unittest.TestCase):
    def test_contracts_expose_generic_context_carrier_for_aot_and_metadata_closure(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "public enum GenericContextKind : byte",
            "TypeInstantiation = 1",
            "MethodInstantiation = 2",
            "TypeAndMethodInstantiation = 3",
            "public sealed record GenericContextArtifact",
            "public required GenericContextKind ContextKind { get; init; }",
            "public required string DefinitionSubjectId { get; init; }",
            "public IReadOnlyList<string>? TypeArguments { get; init; }",
            "public IReadOnlyList<string>? MethodArguments { get; init; }",
            "public GenericContextArtifact? GenericContext { get; init; }",
            "public static GenericContextArtifact? TryCreateGenericContext(",
            "private static IReadOnlyList<string> SplitTopLevelArguments(",
        ]:
            self.assertIn(required_fragment, contracts_source)

    def test_lowering_and_metadata_writer_populate_generic_context_from_existing_subject_identity(self) -> None:
        lowering_source = AOT_CORE_IR_LOWERING_PATH.read_text(encoding="utf-8")
        metadata_writer_source = METADATA_WRITER_STAGE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "GenericContext = ManagedNaming.TryCreateGenericContext(",
            "method.SubjectId,",
            "method.DefinitionSubjectId",
        ]:
            self.assertIn(required_fragment, lowering_source)

        for required_fragment in [
            "GenericContext = ManagedNaming.TryCreateGenericContext(",
            "type.SubjectId,",
            "type.DefinitionSubjectId",
            "method.SubjectId,",
            "method.DefinitionSubjectId",
        ]:
            self.assertIn(required_fragment, metadata_writer_source)


if __name__ == "__main__":
    unittest.main()
