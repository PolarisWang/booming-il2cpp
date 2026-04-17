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
PROJECT_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "FeatureSlices"
    / "CoreRuntimeFeatures"
    / "CoreRuntimeFeatures.csproj"
)
DLL_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "FeatureSlices"
    / "CoreRuntimeFeatures"
    / "bin"
    / "Release"
    / "net8.0"
    / "CoreRuntimeFeatures.dll"
)
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase3-string-join-enumerable-native-aot"
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


class Phase3StringJoinEnumerableNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.synthetic_workspace_root = OUTPUT_ROOT / f"synthetic-workspace-{uuid.uuid4().hex}"

    def _emit_entry(self, entry_subject_id: str) -> Path:
        bundle_root = self.output_root / f"bundle-{uuid.uuid4().hex}"
        emit_root = self.output_root / f"emit-{uuid.uuid4().hex}"

        if bundle_root.exists():
            shutil.rmtree(bundle_root)
        if emit_root.exists():
            shutil.rmtree(emit_root)

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
                str(bundle_root),
                "--entry-point-subject-id",
                entry_subject_id,
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
                str(bundle_root),
                str(emit_root),
            ],
            cwd=REPO_ROOT,
        )
        return emit_root / GENERATED_CPP_RELATIVE_PATH

    def _write_string_join_string_harness(self) -> Path:
        assembly_name = f"StringJoinStringEnumerableHarness_{uuid.uuid4().hex}"
        project_root = self.synthetic_workspace_root / assembly_name
        project_root.mkdir(parents=True, exist_ok=True)
        (project_root / f"{assembly_name}.csproj").write_text(
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
using System.Collections.Generic;

internal static class Program
{
    private static int Main()
    {
        return 0;
    }

    private static int RunStringJoin()
    {
        IEnumerable<string> values = new[]
        {
            "alpha",
            "beta",
            "gamma",
        };

        return string.Join(",", values) == "alpha,beta,gamma" ? 0 : 1;
    }
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _emit_synthetic_string_join_string_entry(self) -> Path:
        bundle_root = self.output_root / f"synthetic-bundle-{uuid.uuid4().hex}"
        emit_root = self.output_root / f"synthetic-emit-{uuid.uuid4().hex}"

        if bundle_root.exists():
            shutil.rmtree(bundle_root)
        if emit_root.exists():
            shutil.rmtree(emit_root)

        project_root = self._write_string_join_string_harness()
        project_path = project_root / f"{project_root.name}.csproj"
        dll_path = project_root / "bin" / "Release" / "net8.0" / f"{project_root.name}.dll"
        entry_subject_id = f"{project_root.name}/Program::RunStringJoin()"

        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")
        run_checked(["dotnet", "build", str(project_path), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(dll_path.is_file(), msg=f"missing synthetic harness dll: {dll_path}")
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(dll_path),
                str(bundle_root),
                "--entry-point-subject-id",
                entry_subject_id,
            ],
            cwd=REPO_ROOT,
        )
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(bundle_root),
                str(emit_root),
            ],
            cwd=REPO_ROOT,
        )
        return emit_root / GENERATED_CPP_RELATIVE_PATH

    def test_emit_native_aot_succeeds_for_iterator_string_join_int_enumerable(self) -> None:
        generated_cpp_path = self._emit_entry("CoreRuntimeFeatures/IteratorStateMachineProofEntry::Run()")
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: CoreRuntimeFeatures/IteratorStateMachineProofEntry::Run()", generated_cpp)

    def test_emit_native_aot_succeeds_for_resource_lifecycle_string_join_string_enumerable(self) -> None:
        generated_cpp_path = self._emit_synthetic_string_join_string_entry()
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("RunStringJoin", generated_cpp)


if __name__ == "__main__":
    unittest.main()

