from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH, SOLUTION_CORE_PACK_BENCHMARKS_ROOT, load_module


REPO_ROOT = Path(__file__).resolve().parents[3]
TOOLING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "core" / "tooling.py"
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
SUBJECT_ID = "SolutionCorePack"
WORKLOAD_ENTRY = "CoreRuntimeBenchmarks/ArithmeticBenchmarkEntry::RunWorkload:System.Int32()"
PROJECT_PATH = SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeBenchmarks.dll"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase-b-native-aot-workload-entry"


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


class NativeAotWorkloadEntryBundleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = TEST_OUTPUT_ROOT / f"{SUBJECT_ID}-{uuid.uuid4().hex}"
        cls.native_aot_output_root = TEST_OUTPUT_ROOT / f"{SUBJECT_ID}-native-aot-{uuid.uuid4().hex}"
        cls.bundle_generated = False
        cls.native_aot_generated = False

    def _ensure_bundle_generated(self) -> None:
        if self.__class__.bundle_generated:
            return

        if self.output_root.exists():
            shutil.rmtree(self.output_root)

        tooling_module = load_module(TOOLING_MODULE_PATH, f"chaos_phase_b_workload_entry_tooling_{uuid.uuid4().hex}")
        driver_intermediate_root = tooling_module.allocate_dotnet_intermediate_dir("Chaos.IL2CPP.Driver", host_platform="windows")
        self.assertIsNotNone(driver_intermediate_root)

        run_checked(
            [
                "dotnet",
                "build",
                str(PROJECT_PATH),
                "-c",
                "Release",
            ],
            cwd=REPO_ROOT,
        )
        self.assertTrue(DLL_PATH.is_file(), msg=f"missing benchmark dll: {DLL_PATH}")

        run_checked(
            [
                "dotnet",
                "build",
                str(DRIVER_PROJECT_PATH),
                "-c",
                "Release",
                "-m:1",
                f"-p:ChaosTempIntermediateRoot={Path(driver_intermediate_root).as_posix()}/",
            ],
            cwd=REPO_ROOT,
        )

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(DLL_PATH),
                str(self.output_root),
                "--entry-point-subject-id",
                WORKLOAD_ENTRY,
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.bundle_generated = True

    def _ensure_native_aot_generated(self) -> None:
        self._ensure_bundle_generated()

        if self.__class__.native_aot_generated:
            return

        if self.native_aot_output_root.exists():
            shutil.rmtree(self.native_aot_output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.output_root),
                str(self.native_aot_output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.native_aot_generated = True

    def test_driver_generates_generic_lowering_plans_for_workload_entry(self) -> None:
        self._ensure_bundle_generated()

        closure_manifest = load_json(self.output_root / "closure.manifest.json")
        native_reference_plan = load_json(self.output_root / "native-reference.lowering-plan.json")
        native_aot_plan = load_json(self.output_root / "native-aot.lowering-plan.json")

        self.assertEqual(WORKLOAD_ENTRY, closure_manifest["entrySubjectId"])
        self.assertEqual("generic-analysis-only", native_reference_plan["planKind"])
        self.assertEqual(WORKLOAD_ENTRY, native_reference_plan["entrySubjectId"])
        self.assertEqual("generic-managed-entry", native_aot_plan["planKind"])
        self.assertEqual(WORKLOAD_ENTRY, native_aot_plan["entrySubjectId"])

    def test_native_aot_emitter_generates_real_cpp_for_bench_arithmetic_workload(self) -> None:
        self._ensure_native_aot_generated()

        generated_cpp = (
            self.native_aot_output_root
            / "generated"
            / "native-aot.generated.cpp"
        ).read_text(encoding="utf-8")

        self.assertIn("chaos_wrap_mul", generated_cpp)
        self.assertIn("chaos_ip_30", generated_cpp)
        self.assertIn("5000000", generated_cpp)
        self.assertIn("if (chaos_left < chaos_right)", generated_cpp)
        self.assertIn("return static_cast<std::int32_t>(chaos_eval_stack[--chaos_stack_top]);", generated_cpp)
        self.assertNotIn("(void)\"BenchArithmetic_Program_RunWorkload\"", generated_cpp)


if __name__ == "__main__":
    unittest.main()
