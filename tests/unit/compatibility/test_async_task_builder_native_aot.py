from __future__ import annotations

import json
import shutil
import subprocess
import unittest
import uuid
from pathlib import Path

from tests.support import SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH, SOLUTION_CORE_PACK_PROOFS_ROOT


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
PROJECT_PATH = SOLUTION_CORE_PACK_PROOFS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_PROOFS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeFeatures.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
ENTRY_SUBJECT_ID = "CoreRuntimeFeatures/AsyncAwaitProofEntry::Run:System.Int32()"
ASYNC_AWAIT_TYPE_SUBJECT_ID = "CoreRuntimeFeatures/AsyncAwaitProofEntry"
TASK_BUILDER_PREFIX = "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder"
TASK_AWAITER_PREFIX = "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter"
TASK_PREFIX = "System.Private.CoreLib/System.Threading.Tasks.Task"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "async-task-builder-native-aot"
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


def find_required_method(artifact: dict[str, object], subject_prefix: str) -> dict[str, object]:
    methods = artifact["methods"]
    return next(method for method in methods if method["subjectId"].startswith(subject_prefix))


def assert_contains_subject_prefix(
    test_case: unittest.TestCase,
    subject_ids: set[str],
    subject_prefix: str,
) -> None:
    test_case.assertTrue(
        any(subject_id.startswith(subject_prefix) for subject_id in subject_ids),
        msg=f"missing subject prefix '{subject_prefix}' in: {sorted(subject_ids)}",
    )


class AsyncTaskBuilderNativeAotTests(unittest.TestCase):
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
        compute_async_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::ComputeAsync:",
        )
        add_async_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::AddAsync:",
        )
        bool_async_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::IsExpectedAsync:",
        )
        string_async_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::DescribeAsync:",
        )
        long_async_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::WidenAsync:",
        )
        double_async_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::MeasureAsync:",
        )
        value_task_long_async_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::ComputeValueTaskLongAsync:",
        )
        run_method = find_required_method(
            artifact,
            f"{ASYNC_AWAIT_TYPE_SUBJECT_ID}::Run:",
        )

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
        bool_async_callees = {
            instruction["callee"]
            for instruction in bool_async_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }
        string_async_callees = {
            instruction["callee"]
            for instruction in string_async_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }
        long_async_callees = {
            instruction["callee"]
            for instruction in long_async_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }
        double_async_callees = {
            instruction["callee"]
            for instruction in double_async_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }
        value_task_long_async_callees = {
            instruction["callee"]
            for instruction in value_task_long_async_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }
        run_callees = {
            instruction["callee"]
            for instruction in run_method["instructions"]
            if instruction["op"] in {"call", "callvirt"} and "callee" in instruction
        }

        assert_contains_subject_prefix(
            self,
            compute_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int32>::Create:",
        )
        assert_contains_subject_prefix(
            self,
            compute_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int32>::Start<AsyncAwaitProofEntry+<ComputeAsync>d__",
        )
        assert_contains_subject_prefix(
            self,
            compute_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int32>::get_Task:",
        )
        assert_contains_subject_prefix(
            self,
            add_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int32>::Create:",
        )
        assert_contains_subject_prefix(
            self,
            add_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int32>::Start<AsyncAwaitProofEntry+<AddAsync>d__",
        )
        assert_contains_subject_prefix(
            self,
            add_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int32>::get_Task:",
        )
        assert_contains_subject_prefix(
            self,
            bool_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Boolean>::Create:",
        )
        assert_contains_subject_prefix(
            self,
            bool_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Boolean>::SetResult:",
        )
        assert_contains_subject_prefix(
            self,
            bool_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Boolean>::get_Task:",
        )
        assert_contains_subject_prefix(
            self,
            string_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.String>::Create:",
        )
        assert_contains_subject_prefix(
            self,
            string_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.String>::SetResult:",
        )
        assert_contains_subject_prefix(
            self,
            string_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.String>::get_Task:",
        )
        assert_contains_subject_prefix(
            self,
            long_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int64>::Create:",
        )
        assert_contains_subject_prefix(
            self,
            long_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int64>::SetResult:",
        )
        assert_contains_subject_prefix(
            self,
            long_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Int64>::get_Task:",
        )
        assert_contains_subject_prefix(
            self,
            double_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Double>::Create:",
        )
        assert_contains_subject_prefix(
            self,
            double_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Double>::SetResult:",
        )
        assert_contains_subject_prefix(
            self,
            double_async_callees,
            f"{TASK_BUILDER_PREFIX}<System.Double>::get_Task:",
        )
        assert_contains_subject_prefix(
            self,
            value_task_long_async_callees,
            "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Int64>::Create:",
        )
        assert_contains_subject_prefix(
            self,
            value_task_long_async_callees,
            "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Int64>::Start<AsyncAwaitProofEntry+<ComputeValueTaskLongAsync>d__",
        )
        assert_contains_subject_prefix(
            self,
            value_task_long_async_callees,
            "System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<System.Int64>::get_Task:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_PREFIX}<System.Boolean>::GetAwaiter:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_AWAITER_PREFIX}<System.Boolean>::GetResult:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_PREFIX}<System.String>::GetAwaiter:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_AWAITER_PREFIX}<System.String>::GetResult:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_PREFIX}<System.Int64>::GetAwaiter:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_AWAITER_PREFIX}<System.Int64>::GetResult:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_PREFIX}<System.Double>::GetAwaiter:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            f"{TASK_AWAITER_PREFIX}<System.Double>::GetResult:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            "System.Private.CoreLib/System.Threading.Tasks.ValueTask<System.Int64>::GetAwaiter:",
        )
        assert_contains_subject_prefix(
            self,
            run_callees,
            "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter<System.Int64>::GetResult:",
        )

    def test_emit_native_aot_succeeds_for_async_task_builder_owner_proof(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_output_root / GENERATED_CPP_RELATIVE_PATH
        self.assertTrue(generated_cpp_path.is_file(), msg=f"missing generated source: {generated_cpp_path}")

        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::Run", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::ComputeAsync", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::AddAsync", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::IsExpectedAsync", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::DescribeAsync", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::WidenAsync", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::MeasureAsync", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeFeatures/AsyncAwaitProofEntry::ComputeValueTaskLongAsync", generated_cpp)
        self.assertIn("Task_System_Boolean___GetAwaiter", generated_cpp)
        self.assertIn("TaskAwaiter_System_Boolean___GetResult", generated_cpp)
        self.assertIn("Task_System_String___GetAwaiter", generated_cpp)
        self.assertIn("TaskAwaiter_System_String___GetResult", generated_cpp)
        self.assertIn("Task_System_Int64___GetAwaiter", generated_cpp)
        self.assertIn("TaskAwaiter_System_Int64___GetResult", generated_cpp)
        self.assertIn("Task_System_Double___GetAwaiter", generated_cpp)
        self.assertIn("TaskAwaiter_System_Double___GetResult", generated_cpp)
        self.assertIn("ValueTask_System_Int64___GetAwaiter", generated_cpp)
        self.assertIn("ValueTaskAwaiter_System_Int64___GetResult", generated_cpp)

    def test_emit_native_aot_async_runtime_prelude_uses_generic_task_carrier(self) -> None:
        self._ensure_native_emitted()

        generated_cpp_path = self.emit_output_root / GENERATED_CPP_RELATIVE_PATH
        generated_cpp = generated_cpp_path.read_text(encoding="utf-8")

        for required_fragment in [
            "struct chaos_async_task",
            "chaos_async_task_builder_get_task",
            "chaos_async_task_builder_set_result_raw",
            "chaos_async_task_awaiter_get_result_raw",
            "chaos_store_int64(chaos_arg_1)",
            "chaos_load_int64(chaos_async_task_awaiter_get_result_raw(chaos_arg_0))",
            "chaos_store_float64(chaos_arg_1)",
            "chaos_load_float64(chaos_async_task_awaiter_get_result_raw(chaos_arg_0))",
        ]:
            self.assertIn(required_fragment, generated_cpp)

        self.assertNotIn("chaos_async_task_int32", generated_cpp)


if __name__ == "__main__":
    unittest.main()

