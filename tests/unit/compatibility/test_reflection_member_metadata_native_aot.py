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
    / "verification"
    / "catalog"
    / "owners"
    / "SolutionCorePack"
    / "Proofs"
    / "CoreRuntimeFeatures"
    / "CoreRuntimeFeatures.csproj"
)
DLL_PATH = (
    REPO_ROOT
    / "verification"
    / "catalog"
    / "owners"
    / "SolutionCorePack"
    / "Proofs"
    / "CoreRuntimeFeatures"
    / "bin"
    / "Release"
    / "net8.0"
    / "CoreRuntimeFeatures.dll"
)
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "reflection-member-metadata-native-aot"
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


class ReflectionMemberMetadataNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.emit_root = OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"

        if cls.output_root.exists():
            shutil.rmtree(cls.output_root)
        if cls.emit_root.exists():
            shutil.rmtree(cls.emit_root)

        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        run_checked(["dotnet", "build", str(PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)

        assert TEST_FRAMEWORK_DLL_PATH.is_file()
        assert DRIVER_DLL_PATH.is_file()
        assert DLL_PATH.is_file()

    def _emit(self, entry_subject_id: str) -> str:
        bundle_root = self.output_root / uuid.uuid4().hex
        emit_root = self.emit_root / uuid.uuid4().hex
        if bundle_root.exists():
            shutil.rmtree(bundle_root)
        if emit_root.exists():
            shutil.rmtree(emit_root)

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

        generated_cpp_path = emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")
        return generated_cpp_path.read_text(encoding="utf-8")

    def test_emit_native_aot_succeeds_for_member_metadata_lookup_proof(self) -> None:
        generated_cpp = self._emit("CoreRuntimeFeatures/MemberMetadataLookupProofEntry::Run()")

        self.assertIn("// Managed method: CoreRuntimeFeatures/MemberMetadataLookupProofEntry::Run()", generated_cpp)
        self.assertIn("chaos_reflection_get_field", generated_cpp)
        self.assertIn("chaos_reflection_get_parameters", generated_cpp)
        self.assertIn("chaos_reflection_get_member_name", generated_cpp)

    def test_emit_native_aot_succeeds_for_reflection_interop_closure_proof(self) -> None:
        generated_cpp = self._emit("CoreRuntimeFeatures/ReflectionInteropClosureEntry::Run()")

        self.assertIn("// Managed method: CoreRuntimeFeatures/ReflectionInteropClosureEntry::Run()", generated_cpp)
        self.assertIn("chaos_reflection_get_field", generated_cpp)
        self.assertIn("chaos_reflection_get_generic_type_definition", generated_cpp)
        self.assertIn("chaos_reflection_get_member_name", generated_cpp)


if __name__ == "__main__":
    unittest.main()

