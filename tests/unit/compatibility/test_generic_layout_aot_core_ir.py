from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import (
    SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH,
    SOLUTION_CORE_PACK_PROOFS_ROOT,
    find_method_by_subject_id,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeFeatures.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/GenericLayoutProofEntry::Run()"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "generic-layout-aot-core-ir"


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


class GenericLayoutAotCoreIrTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.bundle_generated = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        run_checked(["dotnet", "build", str(PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(DLL_PATH.is_file(), msg=f"missing proof assembly: {DLL_PATH}")
        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework assembly: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")

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

    def test_aot_core_ir_carries_generic_context_for_closed_generic_methods_and_types(self) -> None:
        self._ensure_bundle_generated()

        artifact_path = self.output_root / "aot-core-ir.json"
        artifact = json.loads(artifact_path.read_text(encoding="utf-8"))

        generic_echo = find_method_by_subject_id(
            artifact["methods"],
            "CoreRuntimeFeatures/GenericEcho::Echo<System.Int32>(System.Int32)",
        )
        self.assertEqual(2, generic_echo["runtimeGenericContext"]["instantiationKey"]["contextKind"])
        self.assertEqual(
            "CoreRuntimeFeatures/GenericEcho::Echo`1:!!0(!!0)",
            generic_echo["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
        )
        self.assertEqual([], generic_echo["runtimeGenericContext"]["instantiationKey"]["typeArguments"])
        self.assertEqual(
            ["System.Int32"],
            generic_echo["runtimeGenericContext"]["instantiationKey"]["methodArguments"],
        )
        self.assertEqual(3, generic_echo["runtimeGenericContext"]["supportKindCode"])
        self.assertEqual(2, generic_echo["runtimeGenericContext"]["specializationKindCode"])
        self.assertTrue(generic_echo["runtimeGenericContext"]["statusReasonCode"].startswith("loader-demand:"))
        self.assertTrue(generic_echo["runtimeGenericContext"]["sharedGenericBodyId"]["value"])
        self.assertTrue(generic_echo["runtimeGenericContext"]["instantiationStubId"]["value"])
        self.assertEqual(
            generic_echo["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
            generic_echo["openDefinitionSubjectId"],
        )
        self.assertEqual(
            generic_echo["runtimeGenericContext"]["sharedGenericBodyId"],
            generic_echo["sharedGenericBodyId"],
        )
        self.assertEqual(
            generic_echo["runtimeGenericContext"]["instantiationStubId"],
            generic_echo["instantiationStubId"],
        )
        self.assertEqual(
            "CoreRuntimeFeatures/GenericEcho::Echo<System.Int32>:System.Int32(System.Int32)",
            generic_echo["genericDiagnostic"]["subjectId"],
        )
        self.assertEqual(
            "CoreRuntimeFeatures/GenericEcho::Echo<System.Int32>(System.Int32)",
            generic_echo["genericDiagnostic"]["displaySubjectId"],
        )

        entry_method = find_method_by_subject_id(artifact["methods"], ENTRY_SUBJECT_ID)

        generic_echo_call = next(
            instruction
            for instruction in entry_method["instructions"]
            if instruction.get("targetReference", {}).get("genericDiagnostic", {}).get("displaySubjectId")
            == "CoreRuntimeFeatures/GenericEcho::Echo<System.Int32>(System.Int32)"
        )
        self.assertEqual(
            2,
            generic_echo_call["targetReference"]["runtimeGenericContext"]["instantiationKey"]["contextKind"],
        )
        self.assertEqual(
            "CoreRuntimeFeatures/GenericEcho::Echo`1:!!0(!!0)",
            generic_echo_call["targetReference"]["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
        )
        self.assertEqual(
            generic_echo_call["targetReference"]["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
            generic_echo_call["targetReference"]["openDefinitionSubjectId"],
        )
        self.assertEqual(
            generic_echo_call["targetReference"]["runtimeGenericContext"]["sharedGenericBodyId"],
            generic_echo_call["targetReference"]["sharedGenericBodyId"],
        )
        self.assertEqual(
            generic_echo_call["targetReference"]["runtimeGenericContext"]["instantiationStubId"],
            generic_echo_call["targetReference"]["instantiationStubId"],
        )
        self.assertEqual(
            ["System.Int32"],
            generic_echo_call["targetReference"]["runtimeGenericContext"]["instantiationKey"]["methodArguments"],
        )
        self.assertTrue(
            generic_echo_call["targetReference"]["runtimeGenericContext"]["statusReasonCode"].startswith("loader-demand:")
        )

        generic_box_newobj = next(
            instruction
            for instruction in entry_method["instructions"]
            if instruction.get("targetReference", {}).get("subjectId") == "CoreRuntimeFeatures/GenericBox<System.Int32>"
        )
        self.assertEqual(
            1,
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["instantiationKey"]["contextKind"],
        )
        self.assertEqual(
            "CoreRuntimeFeatures/GenericBox`1",
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
        )
        self.assertEqual(
            ["System.Int32"],
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["instantiationKey"]["typeArguments"],
        )
        self.assertEqual(
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
            generic_box_newobj["targetReference"]["openDefinitionSubjectId"],
        )
        self.assertEqual(
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["sharedGenericBodyId"],
            generic_box_newobj["targetReference"]["sharedGenericBodyId"],
        )
        self.assertEqual(
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["instantiationStubId"],
            generic_box_newobj["targetReference"]["instantiationStubId"],
        )
        self.assertTrue(
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["statusReasonCode"].startswith("loader-demand:")
        )
        self.assertEqual(
            [],
            generic_box_newobj["targetReference"]["runtimeGenericContext"]["instantiationKey"]["methodArguments"],
        )

        generic_box_field = next(
            instruction
            for instruction in entry_method["instructions"]
            if instruction.get("targetReference", {}).get("subjectId") == "CoreRuntimeFeatures/GenericBox<System.Int32>::Value"
        )
        self.assertEqual(
            1,
            generic_box_field["targetReference"]["runtimeGenericContext"]["instantiationKey"]["contextKind"],
        )
        self.assertEqual(
            "CoreRuntimeFeatures/GenericBox`1::Value",
            generic_box_field["targetReference"]["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
        )
        self.assertEqual(
            ["System.Int32"],
            generic_box_field["targetReference"]["runtimeGenericContext"]["instantiationKey"]["typeArguments"],
        )
        self.assertEqual(
            generic_box_field["targetReference"]["runtimeGenericContext"]["instantiationKey"]["definitionSubjectId"],
            generic_box_field["targetReference"]["openDefinitionSubjectId"],
        )
        self.assertEqual(
            generic_box_field["targetReference"]["runtimeGenericContext"]["sharedGenericBodyId"],
            generic_box_field["targetReference"]["sharedGenericBodyId"],
        )
        self.assertEqual(
            generic_box_field["targetReference"]["runtimeGenericContext"]["instantiationStubId"],
            generic_box_field["targetReference"]["instantiationStubId"],
        )
        self.assertTrue(
            generic_box_field["targetReference"]["runtimeGenericContext"]["statusReasonCode"].startswith("loader-demand:")
        )
        self.assertEqual(
            [],
            generic_box_field["targetReference"]["runtimeGenericContext"]["instantiationKey"]["methodArguments"],
        )


if __name__ == "__main__":
    unittest.main()

