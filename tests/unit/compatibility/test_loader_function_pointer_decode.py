from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import (
    SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH,
    SOLUTION_CORE_PACK_BENCHMARKS_ROOT,
    SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH,
    SOLUTION_CORE_PACK_PROOFS_ROOT,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
PROOF_PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
PROOF_DLL_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeFeatures.dll"
BENCHMARK_PROJECT_PATH = SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH
BENCHMARK_DLL_PATH = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeBenchmarks.dll"
PROOF_ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/FunctionPointerProofEntry::Run:System.Int32()"
BENCHMARK_ENTRY_SUBJECT_ID = "CoreRuntimeBenchmarks/FunctionPointerBenchmarkEntry::RunWorkload:System.Int32()"
TEST_OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "loader-function-pointer-decode"


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


def iter_typed_instructions(method_artifact: dict[str, object]) -> list[dict[str, object]]:
    instructions: list[dict[str, object]] = []
    for block in method_artifact["blocks"]:
        instructions.extend(block["instructions"])
    return instructions


class LoaderFunctionPointerDecodeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.proof_output_root = TEST_OUTPUT_ROOT / f"proof-{uuid.uuid4().hex}"
        cls.benchmark_output_root = TEST_OUTPUT_ROOT / f"benchmark-{uuid.uuid4().hex}"
        cls.proof_bundle_generated = False
        cls.benchmark_bundle_generated = False

    def _ensure_driver_and_framework_built(self) -> None:
        run_checked(["dotnet", "build", str(TEST_FRAMEWORK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(TEST_FRAMEWORK_DLL_PATH.is_file(), msg=f"missing test framework dll: {TEST_FRAMEWORK_DLL_PATH}")
        run_checked(["dotnet", "build", str(DRIVER_PROJECT_PATH), "-c", "Release", "-m:1"], cwd=REPO_ROOT)
        self.assertTrue(DRIVER_DLL_PATH.is_file(), msg=f"missing driver dll: {DRIVER_DLL_PATH}")

    def _ensure_proof_bundle_generated(self) -> None:
        if self.__class__.proof_bundle_generated:
            return

        self._ensure_driver_and_framework_built()
        if self.proof_output_root.exists():
            shutil.rmtree(self.proof_output_root)

        run_checked(["dotnet", "build", str(PROOF_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(PROOF_DLL_PATH.is_file(), msg=f"missing proof dll: {PROOF_DLL_PATH}")
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(PROOF_DLL_PATH),
                str(self.proof_output_root),
                "--entry-point-subject-id",
                PROOF_ENTRY_SUBJECT_ID,
                "--additional-assembly",
                str(TEST_FRAMEWORK_DLL_PATH),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.proof_bundle_generated = True

    def _ensure_benchmark_bundle_generated(self) -> None:
        if self.__class__.benchmark_bundle_generated:
            return

        self._ensure_driver_and_framework_built()
        if self.benchmark_output_root.exists():
            shutil.rmtree(self.benchmark_output_root)

        run_checked(["dotnet", "build", str(BENCHMARK_PROJECT_PATH), "-c", "Release"], cwd=REPO_ROOT)
        self.assertTrue(BENCHMARK_DLL_PATH.is_file(), msg=f"missing benchmark dll: {BENCHMARK_DLL_PATH}")
        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                str(BENCHMARK_DLL_PATH),
                str(self.benchmark_output_root),
                "--entry-point-subject-id",
                BENCHMARK_ENTRY_SUBJECT_ID,
                "--additional-assembly",
                str(TEST_FRAMEWORK_DLL_PATH),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.benchmark_bundle_generated = True

    def test_typed_il_carries_function_pointer_call_site_signature_for_proof_entry(self) -> None:
        self._ensure_proof_bundle_generated()

        typed_il = load_json(self.proof_output_root / "typed-il-ir.json")
        method = next(method for method in typed_il["methods"] if method["subjectId"] == PROOF_ENTRY_SUBJECT_ID)
        calli = next(instruction for instruction in iter_typed_instructions(method) if instruction["op"] == "calli")

        self.assertEqual("System.Int32", calli["resultType"])
        self.assertEqual(
            {
                "kindCode": 1,
                "returnType": "System.Int32",
                "parameterTypes": ["System.Int32", "System.Int32"],
            },
            calli["callSiteSignature"],
        )

    def test_aot_core_ir_preserves_function_pointer_call_site_signature_for_benchmark_entry(self) -> None:
        self._ensure_benchmark_bundle_generated()

        aot_core_ir = load_json(self.benchmark_output_root / "aot-core-ir.json")
        method = next(method for method in aot_core_ir["methods"] if method["subjectId"] == BENCHMARK_ENTRY_SUBJECT_ID)
        calli = next(instruction for instruction in method["instructions"] if instruction["op"] == "calli")

        self.assertEqual("System.Int32", calli["resultType"])
        self.assertEqual(
            {
                "kindCode": 1,
                "returnType": "System.Int32",
                "parameterTypes": ["System.Int32", "System.Int32"],
            },
            calli["callSiteSignature"],
        )


if __name__ == "__main__":
    unittest.main()

