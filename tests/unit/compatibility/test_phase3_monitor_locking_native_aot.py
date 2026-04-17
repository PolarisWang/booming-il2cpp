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
    / "Proofs"
    / "CoreRuntimeFeatures"
    / "CoreRuntimeFeatures.csproj"
)
DLL_PATH = (
    REPO_ROOT
    / "subjects"
    / "SolutionCorePack"
    / "source"
    / "Proofs"
    / "CoreRuntimeFeatures"
    / "bin"
    / "Release"
    / "net8.0"
    / "CoreRuntimeFeatures.dll"
)
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/MonitorAndLockingProofEntry::Run()"
INCREMENT_SUBJECT_ID = "CoreRuntimeFeatures/MonitorAndLockingProofEntry::Increment(System.Int32)"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase3-monitor-locking-native-aot"
GENERATED_CPP_RELATIVE_PATH = Path("generated") / "native-aot.generated.cpp"
TYPE_INIT_SYMBOL = "chaos_ensure_type_initialized_CoreRuntimeFeatures_MonitorAndLockingProofEntry"
STATIC_GATE_SYMBOL = "chaos_static_CoreRuntimeFeatures_MonitorAndLockingProofEntry__Gate"


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


class Phase3MonitorLockingNativeAotTests(unittest.TestCase):
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

    def test_aot_core_ir_freezes_loop_wrapped_monitor_finally_shape(self) -> None:
        self._ensure_bundle_generated()

        artifact_path = self.output_root / "aot-core-ir.json"
        artifact = json.loads(artifact_path.read_text(encoding="utf-8"))
        increment_method = next(method for method in artifact["methods"] if method["subjectId"] == INCREMENT_SUBJECT_ID)

        self.assertEqual(1, increment_method["exceptionRegionCount"])
        self.assertEqual(
            [
                {
                    "handlingKindCode": 2,
                    "tryOffset": 12,
                    "tryLength": 22,
                    "handlerOffset": 34,
                    "handlerLength": 10,
                }
            ],
            increment_method["exceptionRegions"],
        )

        instruction_offsets = [instruction["ilOffset"] for instruction in increment_method["instructions"]]
        instruction_ops = {instruction["ilOffset"]: instruction["op"] for instruction in increment_method["instructions"]}

        self.assertEqual([0, 1, 2, 4, 9, 10, 11, 12, 13, 15, 20, 25, 26, 27, 32, 34, 35, 37, 38, 43, 44, 45, 46, 47, 48, 49, 50, 52], instruction_offsets)
        self.assertEqual("br", instruction_ops[2])
        self.assertEqual("leave", instruction_ops[32])
        self.assertEqual("endfinally", instruction_ops[43])
        self.assertEqual("blt", instruction_ops[50])

    def test_emit_native_aot_succeeds_for_monitor_locking_owner_proof(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: CoreRuntimeFeatures/MonitorAndLockingProofEntry::Run()", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/MonitorAndLockingProofEntry::Increment(System.Int32)", generated_cpp)

    def test_emit_native_aot_materializes_static_gate_initializer_before_monitor_access(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_root / GENERATED_CPP_RELATIVE_PATH
        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")

        self.assertIn(f"void {TYPE_INIT_SYMBOL}()", generated_cpp)
        self.assertIn(f"if ({STATIC_GATE_SYMBOL} == static_cast<std::intptr_t>(0))", generated_cpp)
        self.assertIn(f"{STATIC_GATE_SYMBOL} = reinterpret_cast<std::intptr_t>(", generated_cpp)
        self.assertIn(f"{TYPE_INIT_SYMBOL}();", generated_cpp)


if __name__ == "__main__":
    unittest.main()

