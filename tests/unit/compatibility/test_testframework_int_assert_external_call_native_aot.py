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
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "testframework-int-assert-external-call-native-aot"
ASSEMBLY_NAME = "TestFrameworkIntAssertExternalCallHarness"
ENTRY_SUBJECT_ID = f"{ASSEMBLY_NAME}/Program::Main()"
ASSERT_EQUAL_SUBJECT_ID = (
    "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Equal:System.Void(System.Int32,System.Int32,System.String)"
)
ASSERT_STATE_FIELD_SUBJECT_ID = (
    "Chaos.TestFramework.Sdk/Chaos.TestFramework.ChaosAssertState::ExitCode"
)
HELPER_SYMBOL = (
    "chaos_external_runtime_Chaos_TestFramework_Sdk_Chaos_TestFramework_Assert__Equal_"
    "System_Void_System_Int32_System_Int32_System_String_"
)
STATIC_FIELD_SYMBOL = "chaos_static_Chaos_TestFramework_Sdk_Chaos_TestFramework_ChaosAssertState__ExitCode"


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


def write_json(path: Path, payload: object) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


class TestFrameworkIntAssertExternalCallNativeAotTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        cls.workspace_root = TEST_OUTPUT_ROOT / f"workspace-{uuid.uuid4().hex}"
        cls.bundle_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.emit_root = TEST_OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"
        cls.native_emitted = False

    def _write_harness_project(self) -> Path:
        project_root = self.workspace_root / ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        project_path = project_root / f"{ASSEMBLY_NAME}.csproj"
        project_path.write_text(
            "\n".join(
                [
                    '<Project Sdk="Microsoft.NET.Sdk">',
                    "  <PropertyGroup>",
                    "    <OutputType>Exe</OutputType>",
                    "    <TargetFramework>net8.0</TargetFramework>",
                    "    <ImplicitUsings>disable</ImplicitUsings>",
                    "    <Nullable>disable</Nullable>",
                    "  </PropertyGroup>",
                    "  <ItemGroup>",
                    f'    <ProjectReference Include="{TEST_FRAMEWORK_PROJECT_PATH}" />',
                    "  </ItemGroup>",
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        (project_root / "Program.cs").write_text(
            "\n".join(
                [
                    "using Chaos.TestFramework;",
                    "",
                    "internal static class Program",
                    "{",
                    "    private static int Main()",
                    "    {",
                    "        ChaosAssertState.Reset();",
                    '        Assert.Equal(42, 42, "int assert should lower through external helper when the bundle treats it as external");',
                    "        return ChaosAssertState.Complete();",
                    "    }",
                    "}",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        return project_path

    def _rewrite_bundle_to_external_call_contract(self) -> None:
        aot_core_ir_path = self.bundle_root / "aot-core-ir.json"
        typed_il_ir_path = self.bundle_root / "typed-il-ir.json"
        code_registration_path = self.bundle_root / "code-registration.json"
        aot_manifest_path = self.bundle_root / "aot-manifest.json"

        aot_core_ir = load_json(aot_core_ir_path)
        for method in aot_core_ir["methods"]:
            for instruction in method["instructions"]:
                if instruction.get("callee") != ASSERT_EQUAL_SUBJECT_ID:
                    continue

                instruction.pop("targetSymbol", None)
                instruction.pop("targetParameterCount", None)
                instruction.pop("targetReturnType", None)
        aot_core_ir["methods"] = [
            method
            for method in aot_core_ir["methods"]
            if method["subjectId"] != ASSERT_EQUAL_SUBJECT_ID
        ]
        write_json(aot_core_ir_path, aot_core_ir)

        typed_il_ir = load_json(typed_il_ir_path)
        typed_il_ir["methods"] = [
            method
            for method in typed_il_ir["methods"]
            if method["subjectId"] != ASSERT_EQUAL_SUBJECT_ID
        ]
        write_json(typed_il_ir_path, typed_il_ir)

        code_registration = load_json(code_registration_path)
        for module in code_registration["modules"]:
            module["registrations"] = [
                registration
                for registration in module["registrations"]
                if registration["subjectId"] != ASSERT_EQUAL_SUBJECT_ID
            ]
        write_json(code_registration_path, code_registration)

        aot_manifest = load_json(aot_manifest_path)
        entries = aot_manifest["entries"]
        for entry in entries:
            if entry["subjectId"] == ASSERT_EQUAL_SUBJECT_ID:
                entry["reason"] = "external-call"
                break
        else:
            entries.append(
                {
                    "assemblyName": "Chaos.TestFramework.Sdk",
                    "subjectKind": "method",
                    "subjectId": ASSERT_EQUAL_SUBJECT_ID,
                    "reason": "external-call",
                }
            )
        write_json(aot_manifest_path, aot_manifest)

    def _ensure_native_emitted(self) -> None:
        if self.__class__.native_emitted:
            return

        if self.workspace_root.exists():
            shutil.rmtree(self.workspace_root)
        if self.bundle_root.exists():
            shutil.rmtree(self.bundle_root)
        if self.emit_root.exists():
            shutil.rmtree(self.emit_root)

        project_path = self._write_harness_project()
        harness_dll_path = self.workspace_root / ASSEMBLY_NAME / "bin" / "Release" / "net8.0" / f"{ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework dll: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing harness dll: {harness_dll_path}")

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(harness_dll_path),
                str(self.bundle_root),
                "--entry-point-subject-id",
                ENTRY_SUBJECT_ID,
                "--additional-assembly",
                str(TEST_FRAMEWORK_DLL_PATH),
            ],
            cwd=REPO_ROOT,
        )
        self._rewrite_bundle_to_external_call_contract()

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.bundle_root),
                str(self.emit_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.native_emitted = True

    def test_emit_native_aot_succeeds_when_testframework_int_assert_is_external_only(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_root / "generated" / "native-aot.generated.cpp"
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing native-aot source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn(HELPER_SYMBOL, generated_cpp)
        self.assertIn(STATIC_FIELD_SYMBOL, generated_cpp)
        self.assertIn(f"{STATIC_FIELD_SYMBOL} = static_cast<CHAOS_IL2CPP_INTPTR>(1);", generated_cpp)

        aot_core_ir = load_json(self.bundle_root / "aot-core-ir.json")
        self.assertNotIn(
            ASSERT_EQUAL_SUBJECT_ID,
            {method["subjectId"] for method in aot_core_ir["methods"]},
        )

        typed_il_ir = load_json(self.bundle_root / "typed-il-ir.json")
        self.assertNotIn(
            ASSERT_EQUAL_SUBJECT_ID,
            {method["subjectId"] for method in typed_il_ir["methods"]},
        )


if __name__ == "__main__":
    unittest.main()
