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
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/ThreadingProofEntry::Run()"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase3-threading-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"
THREAD_CTOR_HELPER_SYMBOL = (
    "chaos_external_runtime_"
    "System_Threading_Thread_System_Threading_Thread___ctor_System_Threading_ThreadStart_"
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


class Phase3ThreadingNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.emit_root = OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"
        cls.bundle_generated = False
        cls.native_emitted = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

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
                str(self.output_root),
                "--entry-point-subject-id",
                ENTRY_SUBJECT_ID,
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
        if self.emit_root.exists():
            shutil.rmtree(self.emit_root)

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

    def test_aot_core_ir_exposes_thread_runtime_family_calls(self) -> None:
        self._ensure_bundle_generated()

        artifact = json.loads((self.output_root / "aot-core-ir.json").read_text(encoding="utf-8"))
        runtime_calls = {
            instruction["callee"]
            for method in artifact["methods"]
            for instruction in method["instructions"]
            if instruction.get("callee", "").startswith("System.Threading.Thread/System.Threading.Thread::")
        }

        self.assertTrue(
            {
                "System.Threading.Thread/System.Threading.Thread::.ctor(System.Threading.ThreadStart)",
                "System.Threading.Thread/System.Threading.Thread::Start()",
                "System.Threading.Thread/System.Threading.Thread::Join()",
                "System.Threading.Thread/System.Threading.Thread::get_CurrentThread()",
                "System.Threading.Thread/System.Threading.Thread::get_Name()",
                "System.Threading.Thread/System.Threading.Thread::set_Name(System.String)",
            }.issubset(runtime_calls),
            msg=f"missing expected thread runtime calls: {sorted(runtime_calls)}",
        )

    def test_emit_native_aot_succeeds_for_threading_owner_proof(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: CoreRuntimeFeatures/ThreadingProofEntry::Run()", generated_cpp)
        self.assertIn(
            f"{THREAD_CTOR_HELPER_SYMBOL}(chaos_arg_0, chaos_arg_1);",
            generated_cpp,
        )


if __name__ == "__main__":
    unittest.main()

