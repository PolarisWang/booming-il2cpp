from __future__ import annotations

import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import (
    SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH,
    SOLUTION_CORE_PACK_PROOFS_ROOT,
    read_native_aot_planner_source,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
PLANNER_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "NativeAotLoweringPlanner.cs"
METHOD_EMISSION_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.CodeGen" / "Emission" / "NativeAotLoweringPlanner.MethodEmission.cs"
PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeFeatures.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
SOURCE_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "GenericsAndCollections" / "GenericLayoutProof.cs"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/GenericLayoutProofEntry::Run()"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "generic-layout-native-aot"


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


def create_expected_stub_symbol(stub_value: str) -> str:
    symbol_part = "".join(character if character.isalnum() else "_" for character in stub_value).strip("_")
    return f"chaos_{symbol_part}"


class GenericLayoutNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.native_aot_output_root = TEST_OUTPUT_ROOT / f"native-aot-{uuid.uuid4().hex}"
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

        self.__class__.native_aot_generated = True

    def test_generic_layout_source_is_native_aot_friendly(self) -> None:
        self.assertTrue(SOURCE_PATH.is_file(), msg=f"missing proof source: {SOURCE_PATH}")

        source_text = SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "public readonly T Value;",
            "var box = new GenericBox<int>(GenericEcho.Echo(42));",
            "var value = box.Value;",
            "Assert.Equal(42, value);",
        ]:
            self.assertIn(required_fragment, source_text)

    def test_native_aot_planner_consumes_runtime_generic_context_when_rendering_generic_layout(self) -> None:
        planner_source = read_native_aot_planner_source(REPO_ROOT)
        method_emission_source = METHOD_EMISSION_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "method.OpenDefinitionSubjectId",
            "method.SharedGenericBodyId",
            "method.InstantiationStubId",
            "lowerableMethod.OpenDefinitionSubjectId",
            "lowerableMethod.SharedGenericBodyId",
            "lowerableMethod.InstantiationStubId",
            "instruction.TargetReference?.OpenDefinitionSubjectId",
            "instruction.TargetReference?.SharedGenericBodyId",
            "instruction.TargetReference?.InstantiationStubId",
            "FormatGenericExecutionAuthorityComment(",
            "ManagedNaming.CreateInstantiationStubSymbol(",
            "TryGetInstantiationStubSymbol(",
            "EmitGenericInstantiationStub(",
        ]:
            self.assertIn(required_fragment, planner_source)

        for required_fragment in [
            "targetReference?.OpenDefinitionSubjectId",
            "targetReference?.SharedGenericBodyId",
            "targetReference?.InstantiationStubId",
            "FormatGenericExecutionAuthorityComment(",
            "ManagedNaming.CreateInstantiationStubSymbol(",
            "EmitGenericInstantiationStub(",
        ]:
            self.assertIn(required_fragment, method_emission_source)

    def test_driver_emits_native_aot_cpp_for_generic_layout_proof(self) -> None:
        self._ensure_native_aot_generated()

        generated_cpp = (
            self.native_aot_output_root
            / "generated"
            / "native-aot.generated.cpp"
        ).read_text(encoding="utf-8")
        generic_echo_stub_symbol = create_expected_stub_symbol(
            "stub:definition=CoreRuntimeFeatures/GenericEcho::Echo`1:!!0(!!0);type=[];method=[System.Int32]"
        )

        for required_fragment in [
            "CoreRuntimeFeatures_GenericEcho_Echo_System_Int32",
            "chaos_type_CoreRuntimeFeatures_GenericBox_System_Int32",
            "field_CoreRuntimeFeatures_GenericBox_System_Int32",
            "// Generic execution authority: definition=CoreRuntimeFeatures/GenericEcho::Echo`1:!!0(!!0); type=[]; method=[System.Int32]",
            "// Generic execution authority: definition=CoreRuntimeFeatures/GenericBox`1; type=[System.Int32]; method=[]",
            f'extern "C" CHAOS_IL2CPP_INT32 {generic_echo_stub_symbol}(CHAOS_IL2CPP_INT32 chaos_arg_0);',
            f'extern "C" CHAOS_IL2CPP_INT32 {generic_echo_stub_symbol}(CHAOS_IL2CPP_INT32 chaos_arg_0)\n{{',
        ]:
            self.assertIn(required_fragment, generated_cpp)
        self.assertGreaterEqual(generated_cpp.count(generic_echo_stub_symbol), 3)


if __name__ == "__main__":
    unittest.main()

