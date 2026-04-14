from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
LOADER_STAGE_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Loader" / "LoaderStage.cs"
MIXED_EXECUTION_PROOF_PATH = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "ManagedBridge" / "Proofs" / "MixedExecutionProofEntry.cs"
INTERPRETER_LOWERING_PROOF_PATH = REPO_ROOT / "subjects" / "MixedExecutionFeaturePack" / "source" / "Lowering" / "InterpreterLoweringProofEntry.cs"
SOLUTION_CORE_LAUNCHER_PATH = REPO_ROOT / "subjects" / "SolutionCorePack" / "source" / "Launcher" / "Program.cs"


class LoaderSwitchOpcodeTests(unittest.TestCase):
    def test_retained_solution_subjects_still_contain_real_switch_usage(self) -> None:
        mixed_execution_source = MIXED_EXECUTION_PROOF_PATH.read_text(encoding="utf-8")
        lowering_source = INTERPRETER_LOWERING_PROOF_PATH.read_text(encoding="utf-8")
        solution_core_launcher_source = SOLUTION_CORE_LAUNCHER_PATH.read_text(encoding="utf-8")

        self.assertIn("instruction.OpCode switch", mixed_execution_source)
        self.assertIn("return opCode switch", lowering_source)
        self.assertIn("return selection switch", solution_core_launcher_source)

    def test_loader_stage_decodes_current_retained_subject_branch_and_prefix_il(self) -> None:
        loader_source = LOADER_STAGE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "ILOpCode.Switch",
            "ILOpCode.Constrained",
            "ILOpCode.Localloc",
            "ILOpCode.Unaligned",
            "ILOpCode.Cpblk",
            "ILOpCode.Ldelem",
            "ILOpCode.Stelem",
            "ILOpCode.Unbox",
            "ILOpCode.Unbox_any",
            'Op = "cpblk"',
            'Op = "localloc"',
            'Op = "switch"',
            "DecodeSwitchInstruction(",
            "ReadSwitchTargets(",
            "SkipConstrainedInstruction(",
            "SkipUnalignedInstruction(",
            "DecodeArrayElementInstruction(",
            "DecodeUnboxAnyInstruction(",
        ]:
            self.assertIn(required_fragment, loader_source)


if __name__ == "__main__":
    unittest.main()
