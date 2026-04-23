from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
MANAGED_CLOSURE_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureModels.cs"
LOADED_WORLD_MODEL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "LoadedWorldModel.cs"
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
LOADER_STAGE_ASSEMBLY_LOADING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.AssemblyLoading.cs"
LOADER_STAGE_CROSS_ASSEMBLY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.CrossAssemblyInstantiation.cs"
LOADER_STAGE_GENERIC_INSTANTIATION_PROJECTION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericInstantiationProjection.cs"
LOADER_STAGE_GENERIC_DEMAND_GRAPH_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.GenericDemandGraph.cs"
LOADER_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "Models" / "LoaderModels.cs"


class LoaderInstantiationDemandGraphContractTests(unittest.TestCase):
    def test_loader_contracts_expose_generic_instantiation_demand_graph_models(self) -> None:
        contracts_source = MANAGED_CLOSURE_MODELS_PATH.read_text(encoding="utf-8")
        loaded_world_source = LOADED_WORLD_MODEL_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "public sealed record GenericInstantiationDemandModel",
            "public sealed record GenericInstantiationDemandGraphModel",
            "public required string RequestingAssemblyName { get; init; }",
            "public required string OwningAssemblyName { get; init; }",
            "public required string SubjectKind { get; init; }",
            "public required string SubjectId { get; init; }",
            "public required string DefinitionSubjectId { get; init; }",
            "public required string DemandSourceKind { get; init; }",
            "public required GenericInstantiationKey InstantiationKey { get; init; }",
            "public required GenericSupportKind SupportKindCode { get; init; }",
            "public required GenericSpecializationKind SpecializationKindCode { get; init; }",
            "public GenericInstantiationDemandGraphModel? GenericInstantiationDemandGraph { get; init; }",
        ]:
            self.assertIn(required_fragment, contracts_source + loaded_world_source)

    def test_loader_stage_splits_generic_demand_graph_collection_from_materialization(self) -> None:
        loader_stage_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")
        assembly_loading_source = LOADER_STAGE_ASSEMBLY_LOADING_PATH.read_text(encoding="utf-8")
        cross_assembly_source = LOADER_STAGE_CROSS_ASSEMBLY_PATH.read_text(encoding="utf-8")
        generic_instantiation_projection_source = LOADER_STAGE_GENERIC_INSTANTIATION_PROJECTION_PATH.read_text(encoding="utf-8")
        loader_models_source = LOADER_MODELS_PATH.read_text(encoding="utf-8")

        self.assertTrue(
            LOADER_STAGE_GENERIC_DEMAND_GRAPH_PATH.is_file(),
            msg=f"missing loader generic demand graph helper split: {LOADER_STAGE_GENERIC_DEMAND_GRAPH_PATH}",
        )

        for required_fragment in [
            "private static GenericInstantiationDemandGraphModel BuildGenericInstantiationDemandGraph(",
            "private static GenericInstantiationDemandGraphModel MergeGenericInstantiationDemandGraphs(",
            "private static void TrackTypeInstantiationDemand(",
            "private static void TrackMethodInstantiationDemand(",
        ]:
            self.assertIn(required_fragment, LOADER_STAGE_GENERIC_DEMAND_GRAPH_PATH.read_text(encoding="utf-8"))

        for required_fragment in [
            "var genericInstantiationDemandEntries = new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);",
            "var projectedTypes = new Dictionary<string, ManagedTypeModel>(StringComparer.Ordinal);",
            "var projectedMethods = new Dictionary<string, ManagedMethodModel>(StringComparer.Ordinal);",
            "TrackTypeInstantiationDemand(",
            "TrackMethodInstantiationDemand(",
            "DemandGraph = BuildGenericInstantiationDemandGraph(genericInstantiationDemandEntries)",
            "public required GenericInstantiationDemandGraphModel DemandGraph { get; init; }",
        ]:
            self.assertIn(required_fragment, generic_instantiation_projection_source + loader_models_source)

        for required_fragment in [
            "loadedAssemblies = ProjectCrossAssemblyMethodInstantiations(assemblyPaths, loadedAssemblies);",
            "GenericInstantiationDemandGraph = MergeGenericInstantiationDemandGraphs(",
        ]:
            self.assertIn(required_fragment, loader_stage_source)

        for required_fragment in [
            "var projectedMethodsByAssembly = loadedAssemblies.ToDictionary(",
            "TrackMethodInstantiationDemand(",
            "requestingAssemblyName: sourceAssemblyName",
            "assembly.GenericInstantiationDemandGraph",
        ]:
            self.assertIn(required_fragment, cross_assembly_source)

        self.assertIn("GenericInstantiationDemandGraph = genericInstantiationProjection.DemandGraph,", assembly_loading_source)

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
                loader_stage_source + cross_assembly_source + generic_instantiation_projection_source,
            )

    def test_loader_derives_runtime_generic_contexts_from_demand_graph(self) -> None:
        assembly_loading_source = LOADER_STAGE_ASSEMBLY_LOADING_PATH.read_text(encoding="utf-8")
        cross_assembly_source = LOADER_STAGE_CROSS_ASSEMBLY_PATH.read_text(encoding="utf-8")
        generic_instantiation_projection_source = LOADER_STAGE_GENERIC_INSTANTIATION_PROJECTION_PATH.read_text(encoding="utf-8")
        demand_graph_source = LOADER_STAGE_GENERIC_DEMAND_GRAPH_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(",
            "private static IReadOnlyList<ManagedMethodModel> ApplyDemandDerivedRuntimeGenericContexts(",
            "private static RuntimeGenericContextArtifact? ResolveRuntimeGenericContext(",
            'StatusReasonCode = $"loader-demand:{demand.DemandSourceKind}",',
        ]:
            self.assertIn(required_fragment, demand_graph_source)

        self.assertIn(
            "Methods = ApplyDemandDerivedRuntimeGenericContexts(allMethods, genericInstantiationProjection.DemandGraph),",
            assembly_loading_source,
        )
        self.assertIn(
            "Methods = ApplyDemandDerivedRuntimeGenericContexts(",
            cross_assembly_source,
        )

        self.assertNotIn("ManagedNaming.TryCreateRuntimeGenericContext(", assembly_loading_source)
        self.assertNotIn("ManagedNaming.TryCreateRuntimeGenericContext(", generic_instantiation_projection_source)
        self.assertNotIn("ManagedNaming.TryCreateRuntimeGenericContext(", cross_assembly_source)


if __name__ == "__main__":
    unittest.main()
