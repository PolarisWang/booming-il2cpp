from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH, SOLUTION_CORE_PACK_PROOFS_ROOT


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeFeatures.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/GenericInstantiationProofEntry::Run:System.Int32()"
GENERIC_CALLEE_SUBJECT_ID = (
    "CoreRuntimeFeatures/GenericInstantiationProofEntry::CreatePair<System.Int32,System.String>"
    ":System.ValueTuple<System.Int32,System.String>(System.Int32,System.String)"
)
GENERIC_DEFINITION_SUBJECT_ID = "CoreRuntimeFeatures/GenericInstantiationProofEntry::CreatePair`2:System.ValueTuple<!!0,!!1>(!!0,!!1)"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "loader-generic-method-reference"


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


class LoaderGenericMethodReferenceDecodeTests(unittest.TestCase):
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

        generic_demand_graph = load_json(self.output_root / "generic-instantiation-demand-graph.json")
        aot_core_ir = load_json(self.output_root / "aot-core-ir.json")
        method = next(method for method in aot_core_ir["methods"] if method["subjectId"] == ENTRY_SUBJECT_ID)
        call = next(
            instruction
            for instruction in method["instructions"]
            if instruction["op"] == "call" and instruction.get("callee") == GENERIC_CALLEE_SUBJECT_ID
        )
        graph_demand = next(
            demand
            for demand in generic_demand_graph["demands"]
            if demand["subjectKind"] == "method" and demand["subjectId"] == GENERIC_CALLEE_SUBJECT_ID
        )

        self.assertEqual(
            {
                "assemblyName": "CoreRuntimeFeatures",
                "subjectKind": "method",
                "subjectId": GENERIC_CALLEE_SUBJECT_ID,
            },
            call["reference"],
        )
        runtime_generic_context = call["targetReference"]["runtimeGenericContext"]
        self.assertEqual(
            {
                "contextKind": 2,
                "definitionSubjectId": GENERIC_DEFINITION_SUBJECT_ID,
                "typeArguments": [],
                "methodArguments": ["System.Int32", "System.String"],
            },
            runtime_generic_context["instantiationKey"],
        )
        self.assertEqual(3, runtime_generic_context["supportKindCode"])
        self.assertEqual(2, runtime_generic_context["specializationKindCode"])
        self.assertTrue(runtime_generic_context["sharedGenericBodyId"]["value"])
        self.assertTrue(runtime_generic_context["instantiationStubId"]["value"])
        self.assertEqual(graph_demand["instantiationKey"], runtime_generic_context["instantiationKey"])
        self.assertEqual(graph_demand["supportKindCode"], runtime_generic_context["supportKindCode"])
        self.assertEqual(graph_demand["specializationKindCode"], runtime_generic_context["specializationKindCode"])


if __name__ == "__main__":
    unittest.main()

