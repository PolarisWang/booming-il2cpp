from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
MANAGED_CLOSURE_CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
MANAGED_CLOSURE_ARTIFACT_MODELS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureArtifactModels.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
DRIVER_ENTRY_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "DriverEntry.cs"
SUBJECT_WORKERS_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"


class LoaderInstantiationDemandGraphManagedClosureArtifactTests(unittest.TestCase):
    def test_managed_closure_contracts_expose_generic_instantiation_demand_graph_artifact(self) -> None:
        contracts_source = MANAGED_CLOSURE_CONTRACTS_PATH.read_text(encoding="utf-8")
        artifact_models_source = MANAGED_CLOSURE_ARTIFACT_MODELS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            'public const string GenericInstantiationDemandGraph = "generic-instantiation-demand-graph.json";',
            "public required GenericInstantiationDemandGraphModel GenericInstantiationDemandGraph { get; init; }",
        ]:
            self.assertIn(required_fragment, contracts_source + artifact_models_source)

    def test_codegen_driver_and_analysis_manifest_wire_generic_instantiation_demand_graph_artifact(self) -> None:
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")
        driver_source = DRIVER_ENTRY_PATH.read_text(encoding="utf-8")
        subject_workers_source = SUBJECT_WORKERS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "var genericInstantiationDemandGraph = linkedWorld.GenericInstantiationDemandGraph",
            'new ManagedClosureArtifactRef { Kind = "genericInstantiationDemandGraph", Path = ManagedClosureArtifactNames.GenericInstantiationDemandGraph }',
            "GenericInstantiationDemandGraph = genericInstantiationDemandGraph,",
        ]:
            self.assertIn(required_fragment, codegen_source)

        self.assertIn(
            "WriteJson(Path.Combine(result.OutputRootPath, ManagedClosureArtifactNames.GenericInstantiationDemandGraph), result.GenericInstantiationDemandGraph);",
            driver_source,
        )

        for required_fragment in [
            '"genericInstantiationDemandGraphPath": _relative(repo_root, output_root / "generic-instantiation-demand-graph.json"),',
            'manifest["artifacts"]["genericInstantiationDemandGraphPath"],',
        ]:
            self.assertIn(required_fragment, subject_workers_source)


if __name__ == "__main__":
    unittest.main()
