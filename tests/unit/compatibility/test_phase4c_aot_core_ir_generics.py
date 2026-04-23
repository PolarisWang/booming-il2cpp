from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import read_contracts_source


REPO_ROOT = Path(__file__).resolve().parents[3]
AOT_CORE_IR_LOWERING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "AotCoreIrLowering.cs"
METADATA_WRITER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.MetadataWriter" / "MetadataWriterStage.cs"


class Phase4CAotCoreIrGenericsTests(unittest.TestCase):
    def test_contracts_expose_canonical_generic_runtime_kernel_contracts(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)

        for required_fragment in [
            "public enum GenericContextKind : byte",
            "TypeInstantiation = 1",
            "MethodInstantiation = 2",
            "TypeAndMethodInstantiation = 3",
            "public enum GenericSupportKind : byte",
            "Legal = 1",
            "Shared = 2",
            "Specialized = 3",
            "Forbidden = 4",
            "public enum GenericSpecializationKind : byte",
            "SharedBody = 1",
            "SpecializedBody = 2",
            "public sealed record GenericInstantiationKey",
            "public sealed record SharedGenericBodyId",
            "public sealed record InstantiationStubId",
            "public sealed record RuntimeGenericContextArtifact",
            "public sealed record GenericDiagnosticArtifact",
            "public required GenericInstantiationKey InstantiationKey { get; init; }",
            "public required SharedGenericBodyId SharedGenericBodyId { get; init; }",
            "public required InstantiationStubId InstantiationStubId { get; init; }",
            "public required GenericSupportKind SupportKindCode { get; init; }",
            "public required GenericSpecializationKind SpecializationKindCode { get; init; }",
            "public string? OpenDefinitionSubjectId { get; init; }",
            "public SharedGenericBodyId? SharedGenericBodyId { get; init; }",
            "public InstantiationStubId? InstantiationStubId { get; init; }",
            "public RuntimeGenericContextArtifact? RuntimeGenericContext { get; init; }",
            "public GenericDiagnosticArtifact? GenericDiagnostic { get; init; }",
            "public static GenericInstantiationKey? TryCreateGenericInstantiationKey(",
            "public static GenericDiagnosticArtifact? TryCreateGenericDiagnosticArtifact(",
            "public static SharedGenericBodyId CreateSharedGenericBodyId(",
            "public static InstantiationStubId CreateInstantiationStubId(",
            "private static IReadOnlyList<string> SplitTopLevelArguments(",
        ]:
            self.assertIn(required_fragment, contracts_source)

        for forbidden_fragment in [
            "public sealed record GenericContextArtifact",
            "public static GenericContextArtifact? TryCreateGenericContext(",
            "public GenericContextArtifact? GenericContext { get; init; }",
            "public static RuntimeGenericContextArtifact? TryCreateRuntimeGenericContext(",
        ]:
            self.assertNotIn(forbidden_fragment, contracts_source)

    def test_lowering_and_metadata_writer_populate_runtime_and_diagnostic_generic_contracts(self) -> None:
        lowering_source = AOT_CORE_IR_LOWERING_PATH.read_text(encoding="utf-8")
        metadata_writer_source = METADATA_WRITER_STAGE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "var genericDemandLookup = BuildGenericDemandLookup(linkedWorld.GenericInstantiationDemandGraph);",
            "OpenDefinitionSubjectId = runtimeGenericContext?.InstantiationKey.DefinitionSubjectId,",
            "SharedGenericBodyId = runtimeGenericContext?.SharedGenericBodyId,",
            "InstantiationStubId = runtimeGenericContext?.InstantiationStubId,",
            "RuntimeGenericContext = runtimeGenericContext,",
            "GenericDiagnostic = genericDiagnostic,",
            "private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(",
            "private static RuntimeGenericContextArtifact? ResolveRuntimeGenericContext(",
            "private static GenericDiagnosticArtifact? ResolveGenericDiagnostic(",
        ]:
            self.assertIn(required_fragment, lowering_source)

        self.assertNotIn(
            "return ManagedNaming.TryCreateRuntimeGenericContext(subjectId, definitionSubjectId);",
            lowering_source,
        )

        for required_fragment in [
            "var genericDemandLookup = BuildGenericDemandLookup(linkedWorld.GenericInstantiationDemandGraph);",
            "RuntimeGenericContext = ResolveRuntimeGenericContext(",
            "GenericDiagnostic = ResolveGenericDiagnostic(",
            "private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(",
            "private static RuntimeGenericContextArtifact? ResolveRuntimeGenericContext(",
            "private static GenericDiagnosticArtifact? ResolveGenericDiagnostic(",
        ]:
            self.assertIn(required_fragment, metadata_writer_source)

        self.assertNotIn(
            "return ManagedNaming.TryCreateRuntimeGenericContext(subjectId, definitionSubjectId);",
            metadata_writer_source,
        )


if __name__ == "__main__":
    unittest.main()
