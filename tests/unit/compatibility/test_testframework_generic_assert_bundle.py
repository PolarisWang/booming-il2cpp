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
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "testframework-generic-assert-bundle"
ASSEMBLY_NAME = "CrossAssemblyGenericAssertHarness"
ENTRY_SUBJECT_ID_OVERRIDE = f"{ASSEMBLY_NAME}/Program::Main()"
CANONICAL_ENTRY_SUBJECT_ID = f"{ASSEMBLY_NAME}/Program::Main:System.Int32()"
GENERIC_ASSERT_SUBJECT_IDS = (
    "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Equal<System.RuntimeTypeHandle>:System.Void(System.RuntimeTypeHandle,System.RuntimeTypeHandle,System.String)",
    "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Equal<System.RuntimeMethodHandle>:System.Void(System.RuntimeMethodHandle,System.RuntimeMethodHandle,System.String)",
    "Chaos.TestFramework.Sdk/Chaos.TestFramework.Assert::Equal<System.String>:System.Void(System.String,System.String,System.String)",
)


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


class TestFrameworkGenericAssertBundleTests(unittest.TestCase):
    maxDiff = None

    @classmethod
    def setUpClass(cls) -> None:
        cls.workspace_root = TEST_OUTPUT_ROOT / f"workspace-{uuid.uuid4().hex}"
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.emit_output_root = TEST_OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"
        cls.bundle_generated = False
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
                    "using System;",
                    "using System.Reflection;",
                    "using Chaos.TestFramework;",
                    "",
                    "internal static class Program",
                    "{",
                    "    private static int Main()",
                    "    {",
                    "        RuntimeTypeHandle typeHandle = typeof(TokenCarrier).TypeHandle;",
                    '        Assert.Equal(typeHandle, typeof(TokenCarrier).TypeHandle, "type handle generic assert should stay closed");',
                    "",
                    '        MethodInfo helper = typeof(TokenCarrier).GetMethod(nameof(TokenCarrier.Helper), BindingFlags.Public | BindingFlags.Static)!;',
                    "        RuntimeMethodHandle methodHandle = helper.MethodHandle;",
                    '        Assert.Equal(methodHandle, helper.MethodHandle, "method handle generic assert should stay closed");',
                    "",
                    '        Assert.Equal(nameof(TokenCarrier), "TokenCarrier", "string generic assert should stay closed");',
                    "        return 0;",
                    "    }",
                    "}",
                    "",
                    "internal static class TokenCarrier",
                    "{",
                    "    public static int Helper(int value)",
                    "    {",
                    "        return value + 1;",
                    "    }",
                    "}",
                    "",
                ]
            ),
            encoding="utf-8",
        )
        return project_path

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.workspace_root.exists():
            shutil.rmtree(self.workspace_root)
        if self.output_root.exists():
            shutil.rmtree(self.output_root)

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
                str(self.output_root),
                "--entry-point-subject-id",
                ENTRY_SUBJECT_ID_OVERRIDE,
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

    def test_cross_assembly_generic_assert_specializations_are_retained_for_bundle_and_direct_call_targets(self) -> None:
        self._ensure_bundle_generated()

        typed_il = load_json(self.output_root / "typed-il-ir.json")
        typed_entry_method = next(
            method
            for method in typed_il["methods"]
            if method["subjectId"] == CANONICAL_ENTRY_SUBJECT_ID
        )
        typed_generic_call_instructions = [
            instruction
            for block in typed_entry_method["blocks"]
            for instruction in block["instructions"]
            if instruction.get("callee") in GENERIC_ASSERT_SUBJECT_IDS
        ]
        self.assertEqual(len(GENERIC_ASSERT_SUBJECT_IDS), len(typed_generic_call_instructions))

        aot_core_ir = load_json(self.output_root / "aot-core-ir.json")
        entry_method = next(
            method
            for method in aot_core_ir["methods"]
            if method["subjectId"] == CANONICAL_ENTRY_SUBJECT_ID
        )
        generic_call_instructions = [
            instruction
            for instruction in entry_method["instructions"]
            if instruction.get("callee") in GENERIC_ASSERT_SUBJECT_IDS
        ]
        self.assertEqual(len(GENERIC_ASSERT_SUBJECT_IDS), len(generic_call_instructions))
        for instruction in generic_call_instructions:
            self.assertEqual(instruction["callee"], instruction["targetReference"]["subjectId"])
            self.assertEqual(3, instruction.get("dispatchKindCode"))

    def test_cross_assembly_generic_assert_harness_emits_native_aot(self) -> None:
        self._ensure_native_emitted()

        generated_cpp = self.emit_output_root / "generated" / "native-aot.generated.cpp"
        self.assertTrue(generated_cpp.is_file(), msg=f"missing native-aot source: {generated_cpp}")


if __name__ == "__main__":
    unittest.main()

