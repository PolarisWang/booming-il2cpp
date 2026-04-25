from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import (
    SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH,
    SOLUTION_CORE_PACK_BENCHMARKS_ROOT,
    find_method_by_subject_id,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
PROJECT_PATH = SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeBenchmarks.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeBenchmarks/IntegerCarrierBenchmarkEntry::RunWorkload()"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "small-integer-carrier-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"


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


class SmallIntegerCarrierNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.emit_output_root = OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"
        cls.bundle_generated = False
        cls.native_emitted = False

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

    def _ensure_native_emitted(self) -> None:
        if self.__class__.native_emitted:
            return

        self._ensure_bundle_generated()
        if self.emit_output_root.exists():
            shutil.rmtree(self.emit_output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.output_root),
                str(self.emit_output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.native_emitted = True

    def test_aot_core_ir_uses_precise_small_integer_abi_carriers(self) -> None:
        self._ensure_bundle_generated()

        aot_core_ir = load_json(self.output_root / "aot-core-ir.json")
        methods = {method["subjectId"]: method for method in aot_core_ir["methods"]}

        uint16_method = find_method_by_subject_id(
            methods,
            "CoreRuntimeBenchmarks/IntegerCarrierBenchmarkMath::RoundTripUInt16(System.UInt16)",
        )
        self.assertEqual(7, uint16_method["returnAbi"]["carrierKindCode"])
        self.assertEqual([7], [slot["carrierKindCode"] for slot in uint16_method["parameterAbis"]])

        int16_method = find_method_by_subject_id(
            methods,
            "CoreRuntimeBenchmarks/IntegerCarrierBenchmarkMath::IncrementInt16(System.Int16)",
        )
        self.assertEqual(6, int16_method["returnAbi"]["carrierKindCode"])
        self.assertEqual([6], [slot["carrierKindCode"] for slot in int16_method["parameterAbis"]])

        byte_method = find_method_by_subject_id(
            methods,
            "CoreRuntimeBenchmarks/IntegerCarrierBenchmarkMath::TruncateToByte(System.Int32)",
        )
        self.assertEqual(5, byte_method["returnAbi"]["carrierKindCode"])
        self.assertEqual([1], [slot["carrierKindCode"] for slot in byte_method["parameterAbis"]])

        sbyte_method = find_method_by_subject_id(
            methods,
            "CoreRuntimeBenchmarks/IntegerCarrierBenchmarkMath::TruncateToSByte(System.Int32)",
        )
        self.assertEqual(4, sbyte_method["returnAbi"]["carrierKindCode"])
        self.assertEqual([1], [slot["carrierKindCode"] for slot in sbyte_method["parameterAbis"]])

    def test_emit_native_aot_succeeds_for_small_integer_carrier_proof(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_output_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("std::uint16_t", generated_cpp)
        self.assertIn("std::int16_t", generated_cpp)
        self.assertIn("std::uint8_t", generated_cpp)
        self.assertIn("std::int8_t", generated_cpp)


if __name__ == "__main__":
    unittest.main()

