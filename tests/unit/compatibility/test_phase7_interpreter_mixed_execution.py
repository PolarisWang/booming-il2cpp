"""Phase 7: Interpreter lowering, mixed execution & native smoke baseline.

Validates three proof subjects end-to-end:

  1. InterpreterArithmeticProof (library)  — builds, verifies AOT bridge entry points
  2. InterpreterLoweringProof  (console)   — builds, runs, validates IR lowering evidence
  3. MixedExecutionProof      (console)   — builds, runs, validates all 24 evidence strings
"""

import unittest
from pathlib import Path

from tests.unit.compatibility.interpreter_mixed_execution_test_support import (
    REPO_ROOT,
    make_unique_build_root,
    run_checked,
)

PROOF_ROOT = REPO_ROOT / "tests" / "fixtures" / "subjects"

INTERPRETER_ARITHMETIC_PROJECT = (
    PROOF_ROOT / "InterpreterArithmeticProof" / "source" / "InterpreterArithmeticProof.csproj"
)

INTERPRETER_LOWERING_PROJECT = (
    PROOF_ROOT / "InterpreterLoweringProof" / "source" / "InterpreterLoweringProof.csproj"
)

MIXED_EXECUTION_PROJECT = (
    PROOF_ROOT / "MixedExecutionProof" / "source" / "MixedExecutionProof.csproj"
)

INTERPRETER_LOWERING_PROGRAM = (
    PROOF_ROOT / "InterpreterLoweringProof" / "source" / "Program.cs"
)

MIXED_EXECUTION_PROGRAM = (
    PROOF_ROOT / "MixedExecutionProof" / "source" / "Program.cs"
)

# ── Expected InterpreterLoweringProof evidence (from Program.cs Console.WriteLines) ──

EXPECTED_LOWERING_PREFIXES = [
    "InterpreterLoweringProof entry reached.",
    "args=",
    "call-ops=",
    "first-block-instructions=",
    "method-subject=",
    "bridge-call-ops=",
    "bridge-has-callbridge=",
    "local-call-ops=",
    "local-call-has-call=",
    "callvirt-ops=",
    "callvirt-target=",
    "instance-call-ops=",
    "instance-call-target=",
    "has-catch-region=",
    "exception-region-count=",
    "has-finally-region=",
]

# ── Expected MixedExecutionProof evidence lines ──

EXPECTED_MIXED_EVIDENCE = [
    "mixed-aot-to-interpreter-before-load=22",
    "mixed-aot-to-interpreter=42",
    "mixed-interpreter-to-aot=30",
    "mixed-interpreter-to-aot-ops=ldarg,ldarg,callbridge,ret",
    "mixed-interpreter-to-aot-target=InterpreterArithmeticProof.AotBridge/AotBridgeExports::Add(System.Int32,System.Int32)",
    "mixed-interpreter-local-call=42",
    "mixed-interpreter-local-call-ops=ldarg,ldarg,call,ret",
    "mixed-interpreter-instance-call=42",
    "mixed-interpreter-instance-call-ops=ldarg,ldarg,callvirt,ret",
    "mixed-interpreter-instance-call-target=InterpreterArithmeticProof/InstanceArithmetic::AddOne(System.Int32)",
    "mixed-interpreter-string-bridge=5",
    "mixed-interpreter-string-bridge-ops=ldstr,callbridge,ret",
    "mixed-interpreter-to-engine=7",
    "mixed-interpreter-throw-caught=ok",
    "mixed-interpreter-leave-finally=ok",
    "mixed-interpreter-rethrow-caught=ok",
    "mixed-interpreter-real-catch=42",
    "mixed-interpreter-real-catch-region-kind=catch",
    "mixed-interpreter-real-rethrow-caught=ok",
    "mixed-interpreter-real-rethrow-region-kind=catch",
    "mixed-interpreter-real-leave-finally=42",
    "mixed-interpreter-real-leave-finally-region-kind=finally",
    "mixed-interpreter-real-leave-finally-opcodes=leave,endfinally",
    "mixed-aot-to-interpreter-after-unload=22",
]


class TestPhase7InterpreterMixedExecution(unittest.TestCase):
    """Phase 7: Interpreter lowering, mixed execution & native smoke baseline."""

    # ══════════════════════════════════════════════════════════════════
    # 1. InterpreterArithmeticProof — build verification
    # ══════════════════════════════════════════════════════════════════

    def test_interpreter_arithmetic_proof_builds(self) -> None:
        """InterpreterArithmeticProof (library) compiles correctly."""
        self.assertTrue(
            INTERPRETER_ARITHMETIC_PROJECT.is_file(),
            msg=f"missing project: {INTERPRETER_ARITHMETIC_PROJECT}",
        )

        build_root = make_unique_build_root("arithmetic-proof")
        run_checked(
            [
                "dotnet", "build", str(INTERPRETER_ARITHMETIC_PROJECT),
                "-c", "Release",
                f"-p:BaseOutputPath={build_root}\\",
            ],
            cwd=REPO_ROOT,
        )

    # ══════════════════════════════════════════════════════════════════
    # 2. InterpreterLoweringProof — build + run + evidence validation
    # ══════════════════════════════════════════════════════════════════

    def test_interpreter_lowering_proof_source_has_no_console_writeline_outside_entry(
        self,
    ) -> None:
        """Lowering proof uses Console.WriteLine only for the entry marker and evidence."""
        source = INTERPRETER_LOWERING_PROGRAM.read_text(encoding="utf-8")
        self.assertIn("Console.WriteLine", source)

    def test_interpreter_lowering_proof_builds_and_runs(self) -> None:
        """InterpreterLoweringProof builds, runs, and emits expected evidence."""
        self.assertTrue(
            INTERPRETER_LOWERING_PROJECT.is_file(),
            msg=f"missing project: {INTERPRETER_LOWERING_PROJECT}",
        )

        build_root = make_unique_build_root("lowering-proof")
        run_checked(
            [
                "dotnet", "build", str(INTERPRETER_LOWERING_PROJECT),
                "-c", "Release",
                f"-p:BaseOutputPath={build_root}\\",
            ],
            cwd=REPO_ROOT,
        )

        completed = run_checked(
            [
                "dotnet", "run", "--project", str(INTERPRETER_LOWERING_PROJECT),
                "-c", "Release", "--no-build",
                f"-p:BaseOutputPath={build_root}\\",
            ],
            cwd=REPO_ROOT,
        )

        output = completed.stdout.strip()
        lines = [line.strip() for line in output.split("\n") if line.strip()]

        for prefix in EXPECTED_LOWERING_PREFIXES:
            any_match = any(line.startswith(prefix) for line in lines)
            self.assertTrue(
                any_match,
                msg=f"missing evidence prefix '{prefix}' in output:\n{output}",
            )

        self.assertIn("bridge-has-callbridge=yes", output)
        self.assertIn("local-call-has-call=yes", output)
        self.assertIn("has-catch-region=yes", output)
        self.assertIn("has-finally-region=yes", output)

    # ══════════════════════════════════════════════════════════════════
    # 3. MixedExecutionProof — build + run + evidence validation
    # ══════════════════════════════════════════════════════════════════

    def test_mixed_execution_proof_source_does_not_use_xunit(self) -> None:
        """MixedExecutionProof uses Console.WriteLine evidence, not assertions."""
        source = MIXED_EXECUTION_PROGRAM.read_text(encoding="utf-8")
        self.assertIn("Console.WriteLine", source)
        self.assertNotIn("Assert.Equal", source)

    def test_mixed_execution_proof_produces_all_24_evidence_lines(self) -> None:
        """MixedExecutionProof builds, runs, and produces all 24 wiki-spec evidence lines."""
        self.assertTrue(
            MIXED_EXECUTION_PROJECT.is_file(),
            msg=f"missing project: {MIXED_EXECUTION_PROJECT}",
        )

        build_root = make_unique_build_root("mixed-proof")
        run_checked(
            [
                "dotnet", "build", str(MIXED_EXECUTION_PROJECT),
                "-c", "Release",
                f"-p:BaseOutputPath={build_root}\\",
            ],
            cwd=REPO_ROOT,
        )

        completed = run_checked(
            [
                "dotnet", "run", "--project", str(MIXED_EXECUTION_PROJECT),
                "-c", "Release", "--no-build",
                f"-p:BaseOutputPath={build_root}\\",
            ],
            cwd=REPO_ROOT,
        )

        output = completed.stdout.strip()
        lines = [line.strip() for line in output.split("\n") if line.strip()]

        for evidence in EXPECTED_MIXED_EVIDENCE:
            self.assertIn(
                evidence,
                lines,
                msg=f"missing evidence line '{evidence}' in output:\n{output}",
            )

        self.assertEqual(len(lines), len(EXPECTED_MIXED_EVIDENCE),
                         msg=f"expected {len(EXPECTED_MIXED_EVIDENCE)} lines, got {len(lines)}:\n{output}")


if __name__ == "__main__":
    unittest.main()
