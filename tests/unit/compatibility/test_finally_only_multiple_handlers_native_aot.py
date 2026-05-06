from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import find_method_by_subject_id


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "finally-only-multiple-handlers-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"
SYNTHETIC_ASSEMBLY_NAME = "FinallyOnlyMultipleHandlersNativeAotHarness"
ENTRY_SUBJECT_ID = f"{SYNTHETIC_ASSEMBLY_NAME}/Program::RunMultipleFinally()"
TARGET_METHOD_SUBJECT_ID = f"{SYNTHETIC_ASSEMBLY_NAME}/Program::ComposeFinallyTrace(System.Boolean)"


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


class FinallyOnlyMultipleHandlersNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.synthetic_workspace_root = OUTPUT_ROOT / f"workspace-{uuid.uuid4().hex}"
        cls.synthetic_bundle_root = OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.synthetic_emit_root = OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"
        cls.synthetic_emitted = False

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

    private static string ComposeFinallyTrace(bool includeMiddleStep)
    {
        string log = string.Empty;
        try
        {
            try
            {
                log += "inner-try;";
                if (includeMiddleStep)
                {
                    log += "middle;";
                }
            }
            finally
            {
                log += "inner-finally;";
            }
        }
        finally
        {
            log += "outer-finally;";
        }

        return log;
    }

    private static int RunMultipleFinally()
    {
        return ComposeFinallyTrace(true).Length;
    }
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_native_emitted(self) -> None:
        if self.__class__.synthetic_emitted:
            return

        if self.synthetic_workspace_root.exists():
            shutil.rmtree(self.synthetic_workspace_root)
        if self.synthetic_bundle_root.exists():
            shutil.rmtree(self.synthetic_bundle_root)
        if self.synthetic_emit_root.exists():
            shutil.rmtree(self.synthetic_emit_root)

        project_root = self._write_synthetic_project()
        project_path = project_root / f"{SYNTHETIC_ASSEMBLY_NAME}.csproj"
        harness_dll_path = project_root / "bin" / "Release" / "net8.0" / f"{SYNTHETIC_ASSEMBLY_NAME}.dll"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(harness_dll_path.is_file(), msg=f"missing synthetic harness dll: {harness_dll_path}")
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(harness_dll_path),
                str(self.synthetic_bundle_root),
                "--entry-point-subject-id",
                ENTRY_SUBJECT_ID,
            ],
            cwd=REPO_ROOT,
        )
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.synthetic_bundle_root),
                str(self.synthetic_emit_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.synthetic_emitted = True

    def test_aot_core_ir_keeps_multiple_finally_only_regions(self) -> None:
        self._ensure_native_emitted()

        aot_core_ir = load_json(self.synthetic_bundle_root / "aot-core-ir.json")
        target_method = find_method_by_subject_id(aot_core_ir["methods"], TARGET_METHOD_SUBJECT_ID)

        self.assertEqual(2, target_method["exceptionRegionCount"])
        self.assertEqual([2, 2], [region["handlingKindCode"] for region in target_method["exceptionRegions"]])
        self.assertIn("endfinally", [instruction["op"] for instruction in target_method["instructions"]])
        self.assertIn("leave", [instruction["op"] for instruction in target_method["instructions"]])

    def test_emit_native_aot_uses_structured_path_for_multiple_finally_only_shape(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.synthetic_emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: FinallyOnlyMultipleHandlersNativeAotHarness/Program::ComposeFinallyTrace(System.Boolean)", generated_cpp)
        self.assertIn("inner-finally;", generated_cpp)
        self.assertIn("outer-finally;", generated_cpp)
        self.assertNotIn("chaos_dispatch_completed", generated_cpp)


if __name__ == "__main__":
    unittest.main()
