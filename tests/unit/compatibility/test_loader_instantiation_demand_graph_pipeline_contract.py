from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
MANAGED_CLOSURE_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureModels.cs"
MANAGED_SEMANTIC_WORLD_CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedSemanticWorldContracts.cs"
LOADER_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "Models" / "LoaderModels.cs"
LOADER_STAGE_GENERIC_DEMAND_GRAPH_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericDemandGraph.cs"
LOADER_STAGE_GENERIC_INSTANTIATION_PROJECTION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericInstantiationProjection.cs"
LOADER_STAGE_GENERIC_COMPATIBILITY_SUBSTITUTION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericInstantiationProjectionSubstitution.cs"
LOADER_STAGE_ASSEMBLY_LOADING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.AssemblyLoading.cs"
SEMANTIC_WORLD_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.SemanticWorld" / "SemanticWorldStage.cs"
LINKER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Linker" / "LinkerStage.cs"


class LoaderInstantiationDemandGraphPipelineContractTests(unittest.TestCase):
    def test_generic_demand_contracts_encode_family_taxonomy_and_pipeline_passthrough(self) -> None:
        generic_contracts_source = MANAGED_CLOSURE_MODELS_PATH.read_text(encoding="utf-8")
        semantic_contracts_source = MANAGED_SEMANTIC_WORLD_CONTRACTS_PATH.read_text(encoding="utf-8")
        loader_models_source = LOADER_MODELS_PATH.read_text(encoding="utf-8")
        generic_demand_helper_source = LOADER_STAGE_GENERIC_DEMAND_GRAPH_PATH.read_text(encoding="utf-8")
        generic_instantiation_projection_source = LOADER_STAGE_GENERIC_INSTANTIATION_PROJECTION_PATH.read_text(encoding="utf-8")
        generic_compatibility_substitution_source = LOADER_STAGE_GENERIC_COMPATIBILITY_SUBSTITUTION_PATH.read_text(encoding="utf-8")
        assembly_loading_source = LOADER_STAGE_ASSEMBLY_LOADING_PATH.read_text(encoding="utf-8")
        semantic_world_stage_source = SEMANTIC_WORLD_STAGE_PATH.read_text(encoding="utf-8")
        linker_stage_source = LINKER_STAGE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "public enum GenericDemandFamilyKind : byte",
            "public required GenericDemandFamilyKind FamilyKindCode { get; init; }",
            "public GenericInstantiationDemandGraphModel? GenericInstantiationDemandGraph { get; init; }",
        ]:
            self.assertIn(required_fragment, generic_contracts_source + semantic_contracts_source)

        for required_fragment in [
            "private static GenericDemandFamilyKind ResolveGenericDemandFamilyKind(",
            "FamilyKindCode = ResolveGenericDemandFamilyKind(",
        ]:
            self.assertIn(required_fragment, generic_demand_helper_source)

        self.assertTrue(
            LOADER_STAGE_GENERIC_COMPATIBILITY_SUBSTITUTION_PATH.is_file(),
            msg=f"missing loader compatibility substitution split: {LOADER_STAGE_GENERIC_COMPATIBILITY_SUBSTITUTION_PATH}",
        )

        for required_fragment in [
            "private static ManagedMethodBodyModel ProjectInstantiationMethodBody(",
            "private static object? ProjectInstantiationOperand(",
            "private static string ProjectInstantiationText(",
            "private static ImmutableDictionary<string, string> CreateSubstitutionMap(",
        ]:
            self.assertIn(required_fragment, generic_compatibility_substitution_source)

        for required_fragment in [
            "internal sealed record GenericInstantiationProjection",
            "private static GenericInstantiationProjection BuildGenericInstantiationProjection(",
            "private static void ProjectInstantiationMethod(",
            "var genericInstantiationProjection = BuildGenericInstantiationProjection(",
            "GenericInstantiationDemandGraph = genericInstantiationProjection.DemandGraph,",
        ]:
            self.assertIn(
                required_fragment,
                loader_models_source + generic_instantiation_projection_source + assembly_loading_source,
            )

        for forbidden_fragment in [
            "MaterializedGenericModels",
            "MaterializeGenericInstantiations(",
            "TryMaterializeMethod(",
        ]:
            self.assertNotIn(
                forbidden_fragment,
                loader_models_source + generic_instantiation_projection_source + assembly_loading_source,
            )

        for forbidden_fragment in [
            "SubstituteMethodBody(",
            "SubstituteOperand(",
            "SubstituteText(",
        ]:
            self.assertNotIn(
                forbidden_fragment,
                generic_instantiation_projection_source + generic_compatibility_substitution_source,
            )

        for forbidden_fragment in [
            "GenericCompatibilityProjection",
            "BuildGenericCompatibilityProjection(",
            "ProjectCompatibilityMethod(",
            "ProjectCompatibilityMethodBody(",
            "ProjectCompatibilityOperand(",
            "ProjectCompatibilityText(",
            "genericCompatibilityProjection",
        ]:
            self.assertNotIn(
                forbidden_fragment,
                loader_models_source + generic_instantiation_projection_source + generic_compatibility_substitution_source + assembly_loading_source,
            )

        for forbidden_fragment in [
            "materializedTypes",
            "materializedMethods",
            "materializedFields",
            "materializedProperties",
            "MaterializeCrossAssemblyMethodInstantiations(",
            "TryMaterializeCrossAssemblyMethod(",
        ]:
            self.assertNotIn(
                forbidden_fragment,
                loader_models_source
                + generic_instantiation_projection_source
                + generic_compatibility_substitution_source
                + assembly_loading_source
                + semantic_world_stage_source
                + linker_stage_source,
            )

        self.assertIn(
            "GenericInstantiationDemandGraph = loadedWorld.GenericInstantiationDemandGraph,",
            semantic_world_stage_source,
        )
        self.assertIn(
            "GenericInstantiationDemandGraph = semanticWorld.GenericInstantiationDemandGraph,",
            linker_stage_source,
        )

    def test_generic_instantiation_substitution_uses_token_aware_placeholder_rewrite(self) -> None:
        substitution_source = LOADER_STAGE_GENERIC_COMPATIBILITY_SUBSTITUTION_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "return ReplaceInstantiationPlaceholders(value, substitutions);",
            "private static string ReplaceInstantiationPlaceholders(",
            "while (index < value.Length && value[index] == '!')",
            "while (index < value.Length && char.IsDigit(value[index]))",
            "substitutions.TryGetValue(value[placeholderStart..index], out var replacement)",
            "builder.Append(replacement);",
        ]:
            self.assertIn(required_fragment, substitution_source)

        self.assertNotIn(
            "value = value.Replace(placeholder, replacement, StringComparison.Ordinal);",
            substitution_source,
        )


if __name__ == "__main__":
    unittest.main()
