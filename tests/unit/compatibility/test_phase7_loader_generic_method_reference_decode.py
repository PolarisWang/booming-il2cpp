from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework" / "Chaos.TestFramework.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.dll"
PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "FeatureSlices"
    / "CoreRuntimeFeatures"
    / "CoreRuntimeFeatures.csproj"
)
DLL_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "FeatureSlices"
    / "CoreRuntimeFeatures"
    / "bin"
    / "Release"
    / "net8.0"
    / "CoreRuntimeFeatures.dll"
)
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/GenericInstantiationProofEntry::Run()"
GENERIC_CALLEE_SUBJECT_ID = (
    "CoreRuntimeFeatures/GenericInstantiationProofEntry::CreatePair<System.Int32,System.String>"
    "(System.Int32,System.String)"
)
GENERIC_DEFINITION_SUBJECT_ID = "CoreRuntimeFeatures/GenericInstantiationProofEntry::CreatePair(!!0,!!1)"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase7-loader-generic-method-reference"


def run_checked(arguments: list[str], *, cwd: Path) -> subprocess.CompletedProcess[str]:
    completed = subprocess.run(
        arguments,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        combined_output = "\n".join(part for part in [completed.stdout, completed.stderr] if part)
        raise AssertionError(f"command failed ({completed.returncode}): {' '.join(arguments)}\n{combined_output}")
    return completed


def load_json(path: Path) -> object:
    return json.loads(path.read_text(encoding="utf-8"))


def iter_typed_instructions(method_artifact: dict[str, object]) -> list[dict[str, object]]:
    instructions: list[dict[str, object]] = []
    for block in method_artifact["blocks"]:
        instructions.extend(block["instructions"])
    return instructions


class Phase7LoaderGenericMethodReferenceDecodeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.bundle_generated = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework dll: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(DLL_PATH.is_file(), msg=f"missing proof assembly: {DLL_PATH}")

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(DLL_PATH),
                str(self.output_root),
                "--entry-point-subject-id",
                ENTRY_SUBJECT_ID,
                "--additional-assembly",
                str(TEST_FRAMEWORK_DLL_PATH),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.bundle_generated = True

    def test_typed_il_carries_reference_for_closed_generic_call(self) -> None:
        self._ensure_bundle_generated()

        typed_il = load_json(self.output_root / "typed-il-ir.json")
        method = next(method for method in typed_il["methods"] if method["subjectId"] == ENTRY_SUBJECT_ID)
        call = next(
            instruction
            for instruction in iter_typed_instructions(method)
            if instruction["op"] == "call" and instruction.get("callee") == GENERIC_CALLEE_SUBJECT_ID
        )

        self.assertEqual(
            {
                "assemblyName": "CoreRuntimeFeatures",
                "subjectKind": "method",
                "subjectId": GENERIC_CALLEE_SUBJECT_ID,
            },
            call["reference"],
        )

    def test_aot_core_ir_keeps_generic_context_for_closed_generic_call(self) -> None:
        self._ensure_bundle_generated()

        aot_core_ir = load_json(self.output_root / "aot-core-ir.json")
        method = next(method for method in aot_core_ir["methods"] if method["subjectId"] == ENTRY_SUBJECT_ID)
        call = next(
            instruction
            for instruction in method["instructions"]
            if instruction["op"] == "call" and instruction.get("callee") == GENERIC_CALLEE_SUBJECT_ID
        )

        self.assertEqual(
            {
                "assemblyName": "CoreRuntimeFeatures",
                "subjectKind": "method",
                "subjectId": GENERIC_CALLEE_SUBJECT_ID,
            },
            call["reference"],
        )
        self.assertEqual(
            {
                "contextKind": 2,
                "definitionSubjectId": GENERIC_DEFINITION_SUBJECT_ID,
                "typeArguments": [],
                "methodArguments": ["System.Int32", "System.String"],
            },
            call["targetReference"]["genericContext"],
        )


if __name__ == "__main__":
    unittest.main()
