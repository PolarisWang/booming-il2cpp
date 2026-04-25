from __future__ import annotations

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
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "bcl-delegate-invoke-native-aot"
ASSEMBLY_NAME = "BclDelegateInvokeHarness"
ENTRY_SUBJECT_ID = f"{ASSEMBLY_NAME}/Program::Main()"
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


class BclDelegateInvokeNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.workspace_root = TEST_OUTPUT_ROOT / f"workspace-{uuid.uuid4().hex}"
        cls.output_root = TEST_OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
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
                    "using System;",
                    "using Chaos.TestFramework;",
                    "",
                    "internal static class Program",
                    "{",
                    "    private static int Main()",
                    "    {",
                    "        var owner = new DelegateOwner();",
                    "",
                    "        Action<string> recorder = owner.Record;",
                    '        recorder("native-bcl-delegate");',
                    '        Assert.Equal("native-bcl-delegate", owner.LastMessage, "Action<T>.Invoke should lower through delegate runtime");',
                    "",
                    "        Func<int, int> adder = owner.AddOne;",
                    '        Assert.Equal(5, adder(4), "Func<T,TResult>.Invoke should lower through delegate runtime");',
                    "        return 0;",
                    "    }",
                    "}",
                    "",
                    "internal sealed class DelegateOwner",
                    "{",
                    '    public string LastMessage = string.Empty;',
                    "",
                    "    public void Record(string value)",
                    "    {",
                    "        LastMessage = value;",
                    "    }",
                    "",
                    "    public int AddOne(int value)",
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

    def _ensure_native_emitted(self) -> None:
        if self.__class__.native_emitted:
            return

        if self.workspace_root.exists():
            shutil.rmtree(self.workspace_root)
        if self.output_root.exists():
            shutil.rmtree(self.output_root)
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
                str(self.emit_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.native_emitted = True

    def test_emit_native_aot_succeeds_for_bcl_delegate_invoke(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: BclDelegateInvokeHarness/Program::Main()", generated_cpp)
        self.assertIn(
            "struct chaos_type_System_Private_CoreLib_System_Action_System_String_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate",
            generated_cpp,
        )
        self.assertIn(
            "struct chaos_type_System_Private_CoreLib_System_Func_System_Int32_System_Int32_ : public chaos_type_System_Private_CoreLib_System_MulticastDelegate",
            generated_cpp,
        )
        self.assertIn(
            "struct chaos_type_System_Private_CoreLib_System_MulticastDelegate : public chaos_type_System_Private_CoreLib_System_Delegate",
            generated_cpp,
        )
        self.assertIn("std::intptr_t chaos_delegate_target = 0;", generated_cpp)
        self.assertIn("std::intptr_t chaos_delegate_method_ptr = 0;", generated_cpp)
        self.assertIn(
            "reinterpret_cast<void(*)(std::intptr_t chaos_delegate_target, std::intptr_t chaos_arg_0)>(chaos_delegate->chaos_delegate_method_ptr)",
            generated_cpp,
        )
        self.assertIn(
            "reinterpret_cast<std::int32_t(*)(std::intptr_t chaos_delegate_target, std::int32_t chaos_arg_0)>(chaos_delegate->chaos_delegate_method_ptr)",
            generated_cpp,
        )


if __name__ == "__main__":
    unittest.main()

