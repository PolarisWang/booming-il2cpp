from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
MANAGED_NAMING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedNaming.cs"
MANAGED_CLOSURE_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureModels.cs"
LOADER_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "Models" / "LoaderModels.cs"
LOADER_METADATA_RESOLUTION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.MetadataResolution.cs"
LOADER_GENERIC_MATERIALIZATION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericMaterialization.cs"


class LoaderMethodIdentityGenericArityTests(unittest.TestCase):
    def test_method_models_expose_generic_parameter_count(self) -> None:
        contracts_source = MANAGED_CLOSURE_MODELS_PATH.read_text(encoding="utf-8")
        loader_models_source = LOADER_MODELS_PATH.read_text(encoding="utf-8")

        self.assertIn("public int GenericParameterCount { get; init; }", contracts_source)
        self.assertIn("public int GenericParameterCount { get; init; }", loader_models_source)

    def test_managed_naming_embeds_generic_arity_into_method_identity(self) -> None:
        naming_source = MANAGED_NAMING_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "int genericParameterCount = 0)",
            "CreateMethodIdentityName(methodName, genericParameterCount)",
            "public static string CreateMethodIdentityName(string methodName, int genericParameterCount = 0)",
            "var returnTypeSeparatorIndex = subjectId.LastIndexOf(':', parameterListIndex >= 0 ? parameterListIndex : subjectId.Length - 1);",
            "return subjectId[(separatorIndex + 2)..returnTypeSeparatorIndex];",
        ]:
            self.assertIn(required_fragment, naming_source)

    def test_loader_propagates_generic_arity_into_definition_and_materialized_subject_ids(self) -> None:
        metadata_resolution_source = LOADER_METADATA_RESOLUTION_PATH.read_text(encoding="utf-8")
        generic_materialization_source = LOADER_GENERIC_MATERIALIZATION_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "var genericParameterCount = methodDefinition.GetGenericParameters().Count;",
            "GenericParameterCount = genericParameterCount,",
            "GetMemberReferenceGenericParameterCount(memberReference)",
            "GetDefinitionGenericParameterCount(metadataReader, memberReference.Parent)",
            "new GenericArityTypeProvider()",
            "var hasClosedMethodArguments = methodArguments.Any(argument => !argument.StartsWith(\"!\", StringComparison.Ordinal));",
            "hasClosedMethodArguments ? 0 : baseReference.GenericParameterCount",
        ]:
            self.assertIn(required_fragment, metadata_resolution_source)

        self.assertIn("definitionMethod.GenericParameterCount", generic_materialization_source)


if __name__ == "__main__":
    unittest.main()
