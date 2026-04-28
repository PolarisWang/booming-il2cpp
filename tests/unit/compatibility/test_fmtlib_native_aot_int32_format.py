from __future__ import annotations

import shutil
import subprocess
import unittest
import uuid
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "fmtlib-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"
SYNTHETIC_ASSEMBLY_NAME = "FmtlibFormatInt32Harness"
SYNTHETIC_ENTRY_SUBJECT_ID = f"{SYNTHETIC_ASSEMBLY_NAME}/Program::Main()"


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


class FmtlibNativeAotFormatInt32Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.synthetic_workspace_root = OUTPUT_ROOT / f"synthetic-workspace-{uuid.uuid4().hex}"
        cls.synthetic_bundle_root = OUTPUT_ROOT / f"synthetic-bundle-{uuid.uuid4().hex}"
        cls.synthetic_emit_root = OUTPUT_ROOT / f"synthetic-emit-{uuid.uuid4().hex}"
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
        # Exercise Int32.ToString() to trigger the format_int32 external helper.
        (project_root / "Program.cs").write_text(
            """
internal static class Program
{
    private static int Main()
    {
        int value = 42;
        string text = value.ToString();
        return text is null ? 1 : 0;
    }
}
""".strip()
            + "\n",
            encoding="utf-8",
        )
        return project_root

    def _ensure_synthetic_native_emitted(self) -> None:
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
                SYNTHETIC_ENTRY_SUBJECT_ID,
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

    def test_emit_native_aot_generated_code_contains_format_int32(self) -> None:
        self._ensure_synthetic_native_emitted()

        generated_cpp_path = self.synthetic_emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: FmtlibFormatInt32Harness/Program::Main()", generated_cpp)
        self.assertIn(
            "ChaosIl2cpp::Common::format_int32",
            generated_cpp,
            "generated code should use ChaosIl2cpp::Common::format_int32 instead of snprintf",
        )

    def test_generated_code_has_no_snprintf_for_int_formatting(self) -> None:
        """Verify that snprintf is not used in the Int32.ToString hotpath."""
        self._ensure_synthetic_native_emitted()

        generated_cpp_path = self.synthetic_emit_root / GENERATED_CPP_RELATIVE_PATH
        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")

        # The Int32.ToString helper should use format_int32, not snprintf.
        # Find the Int32.ToString section.
        int32_to_string_idx = generated_cpp.find("Int32__ToString")
        self.assertGreater(
            int32_to_string_idx, -1,
            "generated code should reference Int32.ToString helper",
        )

        # Snprintf may still appear elsewhere (e.g. RuntimeSkeletonPage),
        # but NOT in the Int32.ToString section.
        section_start = max(0, int32_to_string_idx - 200)
        section_end = min(len(generated_cpp), int32_to_string_idx + 800)
        section = generated_cpp[section_start:section_end]

        self.assertNotIn(
            "std::snprintf",
            section,
            "Int32.ToString helper must not use snprintf",
        )
        self.assertIn(
            "format_int32",
            section,
            "Int32.ToString helper must use format_int32",
        )


if __name__ == "__main__":
    unittest.main()
