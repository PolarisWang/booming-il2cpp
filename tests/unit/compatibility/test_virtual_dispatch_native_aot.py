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
    get_method_subject_display_string,
)


REPO_ROOT = Path(__file__).resolve().parents[3]
DRIVER_PROJECT_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "Chaos.IL2CPP.Driver.csproj"
DRIVER_DLL_PATH = REPO_ROOT / "src" / "managed" / "Chaos.IL2CPP.Driver" / "bin" / "Release" / "net8.0" / "Chaos.IL2CPP.Driver.dll"
TEST_FRAMEWORK_PROJECT_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "Chaos.TestFramework.Sdk.csproj"
TEST_FRAMEWORK_DLL_PATH = REPO_ROOT / "src" / "reference" / "Chaos.TestFramework.Sdk" / "bin" / "Release" / "net8.0" / "Chaos.TestFramework.Sdk.dll"
PROJECT_PATH = SOLUTION_CORE_PACK_BENCHMARKS_PROJECT_PATH
DLL_PATH = SOLUTION_CORE_PACK_BENCHMARKS_ROOT / "bin" / "Release" / "net8.0" / "CoreRuntimeBenchmarks.dll"
OUTPUT_ROOT = REPO_ROOT / "artifacts" / ".tmp-tests" / "virtual-dispatch-native-aot"
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


class VirtualDispatchNativeAotTests(unittest.TestCase):
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

    def _build_bundle(self, entry_subject_id: str) -> Path:
        bundle_root = self.output_root / uuid.uuid4().hex
        if bundle_root.exists():
            shutil.rmtree(bundle_root)

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

        return bundle_root

    def _emit(self, entry_subject_id: str) -> str:
        bundle_root = self._build_bundle(entry_subject_id)
        emit_root = self.emit_root / uuid.uuid4().hex
        if emit_root.exists():
            shutil.rmtree(emit_root)

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

    def test_aot_core_ir_retains_virtual_dispatch_override_targets_for_virtual_dispatch_benchmark(self) -> None:
        bundle_root = self._build_bundle("CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()")

        artifact = json.loads((bundle_root / "aot-core-ir.json").read_text(encoding="utf-8"))
        reachable_methods = {
            get_method_subject_display_string(str(method["subjectId"]))
            for method in artifact["methods"]
        }

        self.assertIn("CoreRuntimeBenchmarks/DispatchShape::Area()", reachable_methods)
        self.assertIn("CoreRuntimeBenchmarks/DispatchCircle::Area()", reachable_methods)
        self.assertIn("CoreRuntimeBenchmarks/DispatchRectangle::Area()", reachable_methods)

    def test_generic_matrix_surfaces_dispatch_boundary_case_for_virtual_dispatch_benchmark(self) -> None:
        bundle_root = self._build_bundle("CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()")

        artifact = json.loads((bundle_root / "generic-capability-matrix.json").read_text(encoding="utf-8"))
        boundary_cases = artifact["boundaryCases"]
        dispatch_cases = [
            boundary_case
            for boundary_case in boundary_cases
            if boundary_case["boundaryKind"] == "DispatchBoundary"
            and get_method_subject_display_string(str(boundary_case["sourceMethodSubjectId"]))
            == "CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()"
            and get_method_subject_display_string(str(boundary_case["targetSubjectId"]))
            == "CoreRuntimeBenchmarks/DispatchShape::Area()"
        ]

        self.assertEqual(1, len(dispatch_cases))
        self.assertEqual("virtual-callvirt", dispatch_cases[0]["evidenceKind"])

        family_budgets = {budget["familyId"]: budget for budget in artifact["familyBudgets"]}
        self.assertGreaterEqual(
            family_budgets["DispatchBoundary"]["observedCount"],
            1,
            msg="dispatch boundary budget must be backed by explicit boundary cases instead of entry-name guessing",
        )

    def test_emit_native_aot_succeeds_for_virtual_dispatch_benchmark(self) -> None:
        generated_cpp = self._emit("CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()")

        self.assertIn("// Managed method: CoreRuntimeBenchmarks/DispatchBenchmarkEntry::RunWorkload()", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeBenchmarks/DispatchCircle::Area()", generated_cpp)
        self.assertIn("// Managed method: CoreRuntimeBenchmarks/DispatchRectangle::Area()", generated_cpp)

    def test_emit_native_aot_succeeds_for_string_length_benchmark(self) -> None:
        generated_cpp = self._emit("CoreRuntimeBenchmarks/RequiredInstantiationClosureBenchmarkEntry::RunWorkload()")

        self.assertIn("// Managed method: CoreRuntimeBenchmarks/RequiredInstantiationClosureBenchmarkEntry::RunWorkload()", generated_cpp)
        self.assertIn("chaos_external_runtime_System_Private_CoreLib_System_String__get_Length_System_Int32__", generated_cpp)
        self.assertIn(
            "constexpr CHAOS_IL2CPP_INTPTR chaos_type_id_System_Private_CoreLib_System_Int32 =",
            generated_cpp,
            msg="value-type array/list runtime helpers must have a stable int32 type id available",
        )
        self.assertIn(
            "chaos_array->element_type_id = chaos_type_id_System_Private_CoreLib_System_Int32;",
            generated_cpp,
            msg="int32 arrays must carry the emitted int32 type id so IReadOnlyList<int> helpers can match them at runtime",
        )

    def test_emit_native_aot_succeeds_for_span_and_memory_benchmark(self) -> None:
        generated_cpp = self._emit("CoreRuntimeBenchmarks/SpanAndMemoryBenchmarkEntry::RunWorkload()")

        self.assertIn("// Managed method: CoreRuntimeBenchmarks/SpanAndMemoryBenchmarkEntry::RunWorkload()", generated_cpp)
        self.assertIn("chaos_create_array_span_int32", generated_cpp)
        self.assertIn("chaos_create_array_memory_int32", generated_cpp)
        self.assertLess(
            generated_cpp.index("struct chaos_managed_array"),
            generated_cpp.index("CHAOS_IL2CPP_INTPTR chaos_create_array_span_int32("),
            msg="span runtime helpers must be emitted after the managed array object model",
        )
        self.assertLess(
            generated_cpp.index("CHAOS_IL2CPP_INTPTR chaos_create_array_memory_int32("),
            generated_cpp.index("CHAOS_IL2CPP_INTPTR chaos_create_memory_int32("),
            msg="array memory helper must be declared before the wrapper helper uses it",
        )
        self.assertIn(
            "\n".join(
                [
                    "        auto* chaos_array = new chaos_managed_array{};",
                    "        chaos_array->header.type_id = chaos_type_id_managed_array;",
                    "        chaos_array->element_type_shape = 2;",
                ]
            ),
            generated_cpp,
            msg="int32 arrays must be emitted as value-type arrays so span/memory helpers do not abort at runtime",
        )

    def test_emit_native_aot_succeeds_for_string_and_utf8_marshaling_benchmark(self) -> None:
        generated_cpp = self._emit("CoreRuntimeBenchmarks/StringAndUtf8MarshalingBenchmarkEntry::RunWorkload()")

        self.assertIn("// Managed method: CoreRuntimeBenchmarks/StringAndUtf8MarshalingBenchmarkEntry::RunWorkload()", generated_cpp)
        self.assertIn("chaos_external_runtime_System_Runtime_InteropServices_Marshal__StringToCoTaskMemUTF8_System_IntPtr_System_String_", generated_cpp)
        self.assertIn("chaos_external_runtime_System_Runtime_InteropServices_Marshal__FreeCoTaskMem_System_Void_System_IntPtr_", generated_cpp)

    def test_emit_native_aot_succeeds_for_struct_marshaling_benchmark(self) -> None:
        generated_cpp = self._emit("CoreRuntimeBenchmarks/StructMarshalingBenchmarkEntry::RunWorkload()")

        self.assertIn("// Managed method: CoreRuntimeBenchmarks/StructMarshalingBenchmarkEntry::RunWorkload()", generated_cpp)
        self.assertIn("chaos_external_runtime_System_Runtime_InteropServices_Marshal__SizeOf_", generated_cpp)
        self.assertIn("chaos_external_runtime_System_Runtime_InteropServices_Marshal__StructureToPtr_", generated_cpp)
        self.assertIn("chaos_external_runtime_System_Runtime_InteropServices_Marshal__PtrToStructure_", generated_cpp)

    def test_emit_native_aot_succeeds_for_task_and_valuetask_benchmark(self) -> None:
        generated_cpp = self._emit("CoreRuntimeBenchmarks/TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()")

        self.assertIn("// Managed method: CoreRuntimeBenchmarks/TaskAndValueTaskFlowBenchmarkEntry::RunWorkload()", generated_cpp)
        self.assertIn(
            "CoreRuntimeBenchmarks_TaskAndValueTaskFlowBenchmarkEntry__ComputeValueTaskAsync_d__2_MoveNext",
            generated_cpp,
            msg="value-task state-machine continuation must still be emitted into the generated native AOT source",
        )
        self.assertIn(
            "chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_AsyncValueTaskMethodBuilder_System_Int32___Create_System_Runtime_CompilerServices_AsyncValueTaskMethodBuilder_System_Int32___",
            generated_cpp,
            msg="value-task async state machines must emit a Create helper instead of leaving the builder unresolved",
        )
        self.assertIn(
            "chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_AsyncValueTaskMethodBuilder_System_Int32___get_Task_System_Threading_Tasks_ValueTask_System_Int32___",
            generated_cpp,
            msg="value-task async state machines must materialize the builder get_Task helper for the returned ValueTask<int>",
        )
        self.assertIn(
            "chaos_external_runtime_System_Private_CoreLib_System_Threading_Tasks_ValueTask_System_Int32___GetAwaiter_System_Runtime_CompilerServices_ValueTaskAwaiter_System_Int32___",
            generated_cpp,
            msg="ValueTask<int>.GetAwaiter() must be lowered through a native helper so the benchmark can consume the returned ValueTask",
        )
        self.assertIn(
            "chaos_external_runtime_System_Private_CoreLib_System_Runtime_CompilerServices_ValueTaskAwaiter_System_Int32___GetResult_System_Int32__",
            generated_cpp,
            msg="ValueTaskAwaiter<int>.GetResult() must be emitted so the benchmark can synchronously observe the async result",
        )


if __name__ == "__main__":
    unittest.main()
