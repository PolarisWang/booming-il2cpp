from __future__ import annotations

import unittest
from pathlib import Path

from tests.support import read_native_aot_planner_source


REPO_ROOT = Path(__file__).resolve().parents[3]
CONTRACTS_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Contracts" / "ManagedClosureContracts.cs"
CODEGEN_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "CodeGenStage.cs"
NATIVE_AOT_EMITTER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotEmitter.cs"
NATIVE_AOT_PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotLoweringPlanner.cs"
AOT_CORE_IR_LOWERING_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "AotCoreIrLowering.cs"


class Phase4AotCoreIrContractTests(unittest.TestCase):
    def test_managed_closure_contracts_expose_aot_core_ir_artifact(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            'public const string AotCoreIr = "aot-core-ir.json";',
            "record AotCoreIrArtifact",
            "record AotCoreIrMethodArtifact",
            "record AotCoreIrInstructionArtifact",
            "public required AotCoreIrArtifact AotCoreIr { get; init; }",
            "public required string NativeSymbol { get; init; }",
        ]:
            self.assertIn(required_fragment, contracts_source)

    def test_codegen_and_native_aot_emitter_are_wired_through_aot_core_ir(self) -> None:
        codegen_source = CODEGEN_STAGE_PATH.read_text(encoding="utf-8")
        emitter_source = NATIVE_AOT_EMITTER_PATH.read_text(encoding="utf-8")
        lowering_source = AOT_CORE_IR_LOWERING_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "var aotCoreIr = new AotCoreIrLowering().Create(",
            'new ManagedClosureArtifactRef { Kind = "aotCoreIr", Path = ManagedClosureArtifactNames.AotCoreIr }',
            "AotCoreIr = aotCoreIr,",
        ]:
            self.assertIn(required_fragment, codegen_source)

        for required_fragment in [
            "class AotCoreIrLowering",
            "Create(",
            "TypedIlIrArtifact",
            "AotCoreIrArtifact",
        ]:
            self.assertIn(required_fragment, lowering_source)

        for required_fragment in [
            "ManagedClosureArtifactNames.AotCoreIr",
            "LoadRequiredJson<AotCoreIrArtifact>",
            "AotCoreIrMethodArtifact",
        ]:
            self.assertIn(required_fragment, emitter_source)

        self.assertNotIn("LoaderStage", emitter_source)
        self.assertNotIn("ManagedMethodModel", emitter_source)

    def test_aot_core_ir_direct_call_contract_is_exposed(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        lowering_source = AOT_CORE_IR_LOWERING_PATH.read_text(encoding="utf-8")
        planner_source = read_native_aot_planner_source(REPO_ROOT)

        for required_fragment in [
            "string? TargetSymbol",
            "int? TargetParameterCount",
            "string? TargetReturnType",
        ]:
            self.assertIn(required_fragment, contracts_source)

        for required_fragment in [
            "CodeRegistrationArtifact",
            "NativeSymbol =",
            "TargetSymbol =",
            "TargetParameterCount =",
            "TargetReturnType =",
        ]:
            self.assertIn(required_fragment, lowering_source)

        for required_fragment in [
            'case "call":',
            "EmitDirectCall(",
            "instruction.TargetSymbol",
            "instruction.TargetParameterCount",
            "instruction.TargetReturnType",
        ]:
            self.assertIn(required_fragment, planner_source)

    def test_aot_core_ir_instruction_reference_carrier_is_exposed(self) -> None:
        contracts_source = CONTRACTS_PATH.read_text(encoding="utf-8")
        lowering_source = AOT_CORE_IR_LOWERING_PATH.read_text(encoding="utf-8")

        self.assertIn("ManagedInstructionReference? Reference", contracts_source)
        self.assertIn("Reference = managedInstruction.Reference", lowering_source)


if __name__ == "__main__":
    unittest.main()
