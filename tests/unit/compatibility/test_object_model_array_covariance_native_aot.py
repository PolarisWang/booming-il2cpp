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
SOURCE_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "ObjectModelAndDispatch" / "ArrayCovarianceProof.cs"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/ArrayCovarianceProofEntry::Run()"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "object-model-array-covariance-native-aot"


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


class ObjectModelArrayCovarianceNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.native_aot_output_root = TEST_OUTPUT_ROOT / f"native-aot-{uuid.uuid4().hex}"
        cls.bundle_generated = False
        cls.native_aot_generated = False

    def _ensure_native_aot_generated(self) -> None:
        if self.__class__.native_aot_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)
        if self.native_aot_output_root.exists():
            shutil.rmtree(self.native_aot_output_root)

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

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.output_root),
                str(self.native_aot_output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.bundle_generated = True
        self.__class__.native_aot_generated = True

    def test_array_covariance_proof_source_is_aot_friendly(self) -> None:
        self.assertTrue(SOURCE_PATH.is_file(), msg=f"missing proof source: {SOURCE_PATH}")

        source_text = SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "internal class ArrayCovarianceBaseBox",
            "internal sealed class ArrayCovarianceDerivedBox : ArrayCovarianceBaseBox",
            "object boxedArray = new ArrayCovarianceDerivedBox[1];",
            "var matched = boxedArray as ArrayCovarianceBaseBox[];",
            "var casted = (ArrayCovarianceBaseBox[])boxedArray;",
            "var objectArray = (object[])boxedArray;",
            "casted[0] = new ArrayCovarianceDerivedBox { Value = 42 };",
            "Assert.Equal(42, ((ArrayCovarianceDerivedBox)matched![0]).Value);",
            "Assert.Equal(42, ((ArrayCovarianceDerivedBox)casted[0]).Value);",
            "Assert.Equal(42, ((ArrayCovarianceDerivedBox)objectArray[0]).Value);",
        ]:
            self.assertIn(required_fragment, source_text)

    def test_driver_emits_array_covariance_carrier_into_aot_core_ir(self) -> None:
        self._ensure_native_aot_generated()

        artifact_path = self.output_root / "aot-core-ir.json"
        artifact = json.loads(artifact_path.read_text(encoding="utf-8"))
        method = find_method_by_subject_id(artifact["methods"], ENTRY_SUBJECT_ID)

        array_casts = [
            instruction
            for instruction in method["instructions"]
            if instruction["op"] in ("castclass", "isinst")
            and instruction.get("targetReference", {}).get("subjectId", "").endswith("[]")
        ]
        self.assertGreaterEqual(len(array_casts), 3)

        for instruction in array_casts:
            target_reference = instruction["targetReference"]
            self.assertIn("arrayElementSubjectId", target_reference)
            self.assertIn("arrayElementTypeShape", target_reference)

    def test_driver_emits_native_aot_cpp_for_array_covariance_proof(self) -> None:
        self._ensure_native_aot_generated()

        generated_cpp = (
            self.native_aot_output_root
            / "generated"
            / "native-aot.generated.cpp"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "std::CHAOS_IL2CPP_UINT8 element_type_shape = 0;",
            "CHAOS_IL2CPP_INTPTR element_type_id = 0;",
            "chaos_array->element_type_shape =",
            "chaos_array->element_type_id =",
            "chaos_is_array_type_compatible(",
            "chaos_is_array_store_compatible(",
            "chaos_header->type_id == chaos_type_id_managed_array",
            "auto* chaos_array = reinterpret_cast<chaos_managed_array*>(chaos_value);",
        ]:
            self.assertIn(required_fragment, generated_cpp)


if __name__ == "__main__":
    unittest.main()

