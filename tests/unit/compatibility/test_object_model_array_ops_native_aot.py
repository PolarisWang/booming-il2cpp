from __future__ import annotations

import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH, SOLUTION_CORE_PACK_PROOFS_ROOT


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeFeatures.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
SOURCE_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "PrimitivesAndOps" / "ArrayOpsProof.cs"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/ArrayOpsProofEntry::Run()"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "object-model-array-ops-native-aot"


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


class ObjectModelArrayOpsNativeAotTests(unittest.TestCase):
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

    def test_array_ops_proof_source_is_aot_friendly(self) -> None:
        self.assertTrue(SOURCE_PATH.is_file(), msg=f"missing proof source: {SOURCE_PATH}")

        source_text = SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "private static void AddTo(ref int target, int delta)",
            "ChaosAssertState.Reset();",
            "int[] values = new int[3];",
            "values[0] = 7;",
            "values[1] = 11;",
            "ref var tail = ref values[2];",
            "tail = values[0] + values[1];",
            "AddTo(ref tail, 24);",
            "Assert.Equal(3, values.Length);",
            "Assert.Equal(42, values[2]);",
            "Assert.Equal(42, tail);",
            "return ChaosAssertState.Complete();",
        ]:
            self.assertIn(required_fragment, source_text)

    def test_driver_emits_native_aot_cpp_for_array_ops_proof(self) -> None:
        self._ensure_native_aot_generated()

        generated_cpp = (
            self.native_aot_output_root
            / "generated"
            / "native-aot.generated.cpp"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "struct chaos_managed_array",
            "chaos_eval_stack[chaos_stack_top++] = chaos_array->length;",
            "chaos_eval_stack[chaos_stack_top++] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)]);",
            "chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)] = chaos_value;",
            "chaos_eval_stack[chaos_stack_top++] = chaos_array->elements[static_cast<CHAOS_IL2CPP_SIZE>(chaos_index)];",
            "const auto* chaos_value_ptr = reinterpret_cast<const CHAOS_IL2CPP_INT32*>(",
            "const auto* chaos_value_ptr = chaos_resolve_native_int_slot(chaos_address);",
            "auto* chaos_value_ptr = reinterpret_cast<CHAOS_IL2CPP_INT32*>(",
            "auto* chaos_value_ptr = chaos_resolve_native_int_slot(chaos_address);",
            'extern "C" void CoreRuntimeFeatures_ArrayOpsProofEntry_AddTo(CHAOS_IL2CPP_INTPTR chaos_arg_0, CHAOS_IL2CPP_INT32 chaos_arg_1);',
            "CoreRuntimeFeatures_ArrayOpsProofEntry_AddTo(chaos_arg_0, static_cast<CHAOS_IL2CPP_INT32>(chaos_arg_1));",
        ]:
            self.assertIn(required_fragment, generated_cpp)


if __name__ == "__main__":
    unittest.main()

