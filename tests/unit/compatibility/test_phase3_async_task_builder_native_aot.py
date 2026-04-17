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
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/AsyncAwaitProofEntry::Run()"
COMPUTE_ASYNC_SUBJECT_ID = "CoreRuntimeFeatures/AsyncAwaitProofEntry::ComputeAsync(System.Int32)"
ADD_ASYNC_SUBJECT_ID = "CoreRuntimeFeatures/AsyncAwaitProofEntry::AddAsync(System.Int32,System.Int32)"
COMPUTE_ASYNC_MOVE_NEXT_SUBJECT_ID = "CoreRuntimeFeatures/AsyncAwaitProofEntry+<ComputeAsync>d__0::MoveNext()"
ADD_ASYNC_MOVE_NEXT_SUBJECT_ID = "CoreRuntimeFeatures/AsyncAwaitProofEntry+<AddAsync>d__1::MoveNext()"
ASYNC_BUILDER_CREATE_SUBJECT_ID = (
    "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
    "<System.Int32>::Create()"
)
ASYNC_BUILDER_GET_TASK_SUBJECT_ID = (
    "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
    "<System.Int32>::get_Task()"
)
ASYNC_BUILDER_START_COMPUTE_SUBJECT_ID = (
    "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
    "<System.Int32>::Start<AsyncAwaitProofEntry+<ComputeAsync>d__0>"
    "(AsyncAwaitProofEntry+<ComputeAsync>d__0&)"
)
ASYNC_BUILDER_START_ADD_SUBJECT_ID = (
    "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
    "<System.Int32>::Start<AsyncAwaitProofEntry+<AddAsync>d__1>"
    "(AsyncAwaitProofEntry+<AddAsync>d__1&)"
)
ASYNC_BUILDER_AWAIT_UNSAFE_YIELD_COMPUTE_SUBJECT_ID = (
    "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
    "<System.Int32>::AwaitUnsafeOnCompleted<System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter,"
    "AsyncAwaitProofEntry+<ComputeAsync>d__0>"
    "(System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter&,AsyncAwaitProofEntry+<ComputeAsync>d__0&)"
)
ASYNC_BUILDER_AWAIT_UNSAFE_TASK_COMPUTE_SUBJECT_ID = (
    "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
    "<System.Int32>::AwaitUnsafeOnCompleted<System.Runtime.CompilerServices.TaskAwaiter<System.Int32>,"
    "AsyncAwaitProofEntry+<ComputeAsync>d__0>"
    "(System.Runtime.CompilerServices.TaskAwaiter<System.Int32>&,AsyncAwaitProofEntry+<ComputeAsync>d__0&)"
)
ASYNC_BUILDER_AWAIT_UNSAFE_YIELD_ADD_SUBJECT_ID = (
    "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
    "<System.Int32>::AwaitUnsafeOnCompleted<System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter,"
    "AsyncAwaitProofEntry+<AddAsync>d__1>"
    "(System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter&,AsyncAwaitProofEntry+<AddAsync>d__1&)"
)
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "phase3-async-task-builder-native-aot"
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


class Phase3AsyncTaskBuilderNativeAotTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.output_root = OUTPUT_ROOT / f"bundle-{uuid.uuid4().hex}"
        cls.emit_output_root = OUTPUT_ROOT / f"emit-{uuid.uuid4().hex}"
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
        if self.emit_output_root.exists():
            shutil.rmtree(self.emit_output_root)

        run_checked(
            [
                "dotnet",
                str(DRIVER_DLL_PATH),
                "emit-native-aot",
                str(self.output_root),
                str(self.emit_output_root),
            ],
            cwd=REPO_ROOT,
        )

        self.__class__.native_emitted = True

    def test_aot_core_ir_freezes_async_task_builder_helper_chain(self) -> None:
        self._ensure_bundle_generated()

        artifact_path = self.output_root / "aot-core-ir.json"
        artifact = json.loads(artifact_path.read_text(encoding="utf-8"))
        compute_async_method = next(method for method in artifact["methods"] if method["subjectId"] == COMPUTE_ASYNC_SUBJECT_ID)
        add_async_method = next(method for method in artifact["methods"] if method["subjectId"] == ADD_ASYNC_SUBJECT_ID)

        compute_async_callees = {
            instruction["callee"]
            for instruction in compute_async_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }
        add_async_callees = {
            instruction["callee"]
            for instruction in add_async_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }

        self.assertIn(ASYNC_BUILDER_CREATE_SUBJECT_ID, compute_async_callees)
        self.assertIn(ASYNC_BUILDER_START_COMPUTE_SUBJECT_ID, compute_async_callees)
        self.assertIn(ASYNC_BUILDER_GET_TASK_SUBJECT_ID, compute_async_callees)
        self.assertIn(ASYNC_BUILDER_CREATE_SUBJECT_ID, add_async_callees)
        self.assertIn(ASYNC_BUILDER_START_ADD_SUBJECT_ID, add_async_callees)
        self.assertIn(ASYNC_BUILDER_GET_TASK_SUBJECT_ID, add_async_callees)

    def test_emit_native_aot_succeeds_for_async_task_builder_owner_proof(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_output_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::Run()", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::ComputeAsync(System.Int32)", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::AddAsync(System.Int32,System.Int32)", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry+<ComputeAsync>d__0::MoveNext()", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry+<AddAsync>d__1::MoveNext()", generated_cpp)


if __name__ == "__main__":
    unittest.main()

