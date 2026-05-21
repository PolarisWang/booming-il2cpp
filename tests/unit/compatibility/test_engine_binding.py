from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import read_contracts_source, read_native_reference_planner_source


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
LOWERING_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceLoweringPlanner.cs"
EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Generator" / "NativeReferenceProofEmitter.cs"
CATALOG_PATH = (
    REPO_ROOT
    / "src"
    / "managed"
    / "Chaos.IL2CPP.Generator"
    / "ReferenceProof"
    / "NativeReferenceProofCatalog.cs"
)
SUBJECT_WORKERS_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "subject_workers.py"
RUNTIME_CORE_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.cpp"
RUNTIME_CORE_HEADER_PATH = REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_core.h"
ENGINE_FIXTURE_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects"
ENGINE_SUBJECT_PROGRAM_PATHS = [
    ENGINE_FIXTURE_ROOT / "EngineLogWriteLite" / "source" / "Program.cs",
    ENGINE_FIXTURE_ROOT / "EngineObjectHandleLite" / "source" / "Program.cs",
    ENGINE_FIXTURE_ROOT / "EngineLifecycleCallbackLite" / "source" / "Program.cs",
    ENGINE_FIXTURE_ROOT / "EngineHostProof" / "source" / "Program.cs",
]


class EngineBindingTests(unittest.TestCase):
    def test_engine_subject_tree_realizes_four_minimal_engine_subjects(self) -> None:
        subject_ids = [
            "EngineLogWriteLite",
            "EngineObjectHandleLite",
            "EngineLifecycleCallbackLite",
            "EngineHostProof",
        ]

        for subject_id in subject_ids:
            subject_root = ENGINE_FIXTURE_ROOT / subject_id
            manifest_path = subject_root / "subject.manifest.json"
            project_path = subject_root / "source" / f"{subject_id}.csproj"
            program_path = subject_root / "source" / "Program.cs"
            proof_main_path = subject_root / "validation" / "proof" / "native-reference" / "main.cpp"

            self.assertTrue(manifest_path.is_file(), msg=f"missing subject manifest: {manifest_path}")
            self.assertTrue(project_path.is_file(), msg=f"missing subject project: {project_path}")
            self.assertTrue(program_path.is_file(), msg=f"missing subject program: {program_path}")
            self.assertTrue(proof_main_path.is_file(), msg=f"missing proof host main.cpp: {proof_main_path}")

    def test_contract_codegen_runtime_and_workers_lock_engine_binding_surface(self) -> None:
        contracts_source = read_contracts_source(REPO_ROOT)
        planner_source = read_native_reference_planner_source(REPO_ROOT)
        emitter_source = EMITTER_PATH.read_text(encoding="utf-8")
        catalog_source = CATALOG_PATH.read_text(encoding="utf-8")
        workers_source = SUBJECT_WORKERS_PATH.read_text(encoding="utf-8")
        runtime_core_source = RUNTIME_CORE_PATH.read_text(encoding="utf-8")
        runtime_core_header_source = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        self.assertIn("EngineBindings", contracts_source)
        self.assertIn("HostBindings", contracts_source)

        for constant_name, lowering_family in [
            ("EngineLogWriteMinimal", "engine.log-write.minimal"),
            ("EngineObjectHandleRoundtripMinimal", "engine.object-handle.roundtrip.minimal"),
            ("EngineLifecycleCallbackMinimal", "engine.lifecycle-callback.minimal"),
            ("EngineHostProofMinimal", "engine.host-proof.minimal"),
        ]:
            self.assertIn(lowering_family, catalog_source)
            self.assertIn(f"NativeReferenceProofCatalog.{constant_name}", planner_source)
            self.assertIn(f"NativeReferenceProofCatalog.{constant_name}", emitter_source)

        for stage_kind in [
            "generated-engine-proof",
            "runtime-engine-observe",
            "runtime-engine-trace-compare",
        ]:
            self.assertIn(stage_kind, workers_source)

        for required_fragment in [
            "engineContractSummary",
            "engineEmissionSummary",
            "engineObservationSummary",
            "engineTraceCompareReportPaths",
        ]:
            self.assertIn(required_fragment, workers_source)

        for helper_name in [
            "EngineLogWrite",
            "CreateEngineObjectHandle",
            "ResolveEngineObjectHandle",
            "RegisterEngineLifecycleCallback",
            "DispatchEngineLifecycleCallbacks",
            "IsMainThreadLane",
        ]:
            self.assertIn(helper_name, runtime_core_source)
            self.assertIn(helper_name, runtime_core_header_source)

    def test_engine_subject_sources_stay_within_minimal_engine_slice(self) -> None:
        for program_path in ENGINE_SUBJECT_PROGRAM_PATHS:
            program_source = program_path.read_text(encoding="utf-8")
            self.assertNotIn(
                ".Length",
                program_source,
                msg=f"engine minimal subject should avoid String.Length dependency: {program_path}",
            )


if __name__ == "__main__":
    unittest.main()
