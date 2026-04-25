from __future__ import annotations

import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "exception-metadata-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"
SYNTHETIC_ASSEMBLY_NAME = "ExceptionMetadataNativeAotHarness"

ENTRY_CASES = {
    "exception_message": f"{SYNTHETIC_ASSEMBLY_NAME}/Program::RunExceptionMessage()",
}


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


class ExceptionMetadataNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.synthetic_workspace_root = OUTPUT_ROOT / f"synthetic-workspace-{uuid.uuid4().hex}"
        cls.bundle_roots = {
            name: OUTPUT_ROOT / f"{name}-bundle-{uuid.uuid4().hex}"
            for name in ENTRY_CASES
        }
        cls.emit_roots = {
            name: OUTPUT_ROOT / f"{name}-emit-{uuid.uuid4().hex}"
            for name in ENTRY_CASES
        }
        cls.generated_cases: set[str] = set()
        cls.emitted_cases: set[str] = set()
        cls.inputs_built = False

    def _write_synthetic_project(self) -> Path:
        project_root = self.synthetic_workspace_root / SYNTHETIC_ASSEMBLY_NAME
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{SYNTHETIC_ASSEMBLY_NAME}.csproj").write_text(
            """
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
    <ImplicitUsings>disable</ImplicitUsings>
    <Nullable>disable</Nullable>
  </PropertyGroup>
</Project>
""".strip()
            + "\n",
            encoding="utf-8",
        )
        (project_root / "Program.cs").write_text(
            """
using System;

internal static class Program
{
    private static int Main()
    {
        return 0;
    }

    private static void ThrowKnownException(string message)
    {
        throw new InvalidOperationException(message);
    }

    private static string CatchFromCallee(string tag)
    {
        try
        {
            ThrowKnownException("from-" + tag);
            return "unreachable";
        }
        catch (InvalidOperationException ex)
        {
            return "caught:" + ex.Message;
        }
    }

    private static int RunExceptionMessage()
    {
        return CatchFromCallee("aot") is null ? 1 : 0;
    }

}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_inputs_built(self) -> Path:
        project_root = self.synthetic_workspace_root / SYNTHETIC_ASSEMBLY_NAME
        project_path = project_root / f"{SYNTHETIC_ASSEMBLY_NAME}.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / f"{SYNTHETIC_ASSEMBLY_NAME}.dll"

        if self.__class__.inputs_built:
            return harness_dll_path

        if self.synthetic_workspace_root.exists():
            shutil.rmtree(self.synthetic_workspace_root)

        project_root = self._write_synthetic_project()
        project_path = project_root / f"{SYNTHETIC_ASSEMBLY_NAME}.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / f"{SYNTHETIC_ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing synthetic harness dll: {harness_dll_path}")

        self.__class__.inputs_built = True
        return harness_dll_path

    def _ensure_bundle_generated(self, case_name: str) -> None:
        if case_name in self.__class__.generated_cases:
            return

        harness_dll_path = self._ensure_inputs_built()
        bundle_root = self.bundle_roots[case_name]
        if bundle_root.exists():
            shutil.rmtree(bundle_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(harness_dll_path),
                str(bundle_root),
                "--entry-point-subject-id",
                ENTRY_CASES[case_name],
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.generated_cases.add(case_name)

    def _ensure_native_emitted(self, case_name: str) -> None:
        if case_name in self.__class__.emitted_cases:
            return

        self._ensure_bundle_generated(case_name)
        emit_root = self.emit_roots[case_name]
        if emit_root.exists():
            shutil.rmtree(emit_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.bundle_roots[case_name]),
                str(emit_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.emitted_cases.add(case_name)

    def test_emit_native_aot_succeeds_for_exception_message_access(self) -> None:
        self._ensure_native_emitted("exception_message")

        generated_cpp_path = self.emit_roots["exception_message"] / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

if __name__ == "__main__":
    unittest.main()
