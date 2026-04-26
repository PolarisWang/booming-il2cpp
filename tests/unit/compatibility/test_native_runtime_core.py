from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ROOT_CMAKE_PATH = REPO_ROOT / "CMakeLists.txt"
RUNTIME_CORE_DIR = REPO_ROOT / "src" / "native" / "runtime-core"
RUNTIME_CORE_CMAKE_PATH = RUNTIME_CORE_DIR / "CMakeLists.txt"
RUNTIME_CORE_HEADER_PATH = RUNTIME_CORE_DIR / "runtime_core.h"
RUNTIME_CORE_SOURCE_PATH = RUNTIME_CORE_DIR / "runtime_core.cpp"


class NativeRuntimeCoreTests(unittest.TestCase):
    def test_runtime_core_is_wired_into_windows_reference_router(self) -> None:
        self.assertTrue(RUNTIME_CORE_DIR.is_dir(), msg=f"missing runtime-core dir: {RUNTIME_CORE_DIR}")
        self.assertTrue(RUNTIME_CORE_CMAKE_PATH.is_file(), msg=f"missing runtime-core CMakeLists: {RUNTIME_CORE_CMAKE_PATH}")
        self.assertTrue(RUNTIME_CORE_HEADER_PATH.is_file(), msg=f"missing runtime-core header: {RUNTIME_CORE_HEADER_PATH}")
        self.assertTrue(RUNTIME_CORE_SOURCE_PATH.is_file(), msg=f"missing runtime-core source: {RUNTIME_CORE_SOURCE_PATH}")

        root_cmake_text = ROOT_CMAKE_PATH.read_text(encoding="utf-8")
        self.assertIn("add_subdirectory(src/native/runtime-core)", root_cmake_text)

    def test_runtime_core_exports_first_proof_runtime_abi_table(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")

        required_markers = [
            "chaos_runtime_get_abi_v0",
            "GetRuntimeAbiV0",
            "kRuntimeAbiV0",
            "RuntimeInit",
            "ThreadAttach",
            "ObjectNew",
            "StringNewUtf8",
            "ClassInit",
            "FieldGetValue",
            "FieldSetValue",
            "AssemblyGetImage",
            "ImageFindType",
            "TypeFindMethod",
            "TypeFindField",
            "RaiseManagedException",
            "MethodInvoke",
            "field_storage",
            "std::memset(object->field_storage, 0, sizeof(object->field_storage));",
            "std::memcpy(out_value, object->field_storage, out_value_size);",
            "std::memcpy(object->field_storage, value, value_size);",
            "CHAOS_RUNTIME_STATUS_NOT_SUPPORTED",
            "return &kRuntimeAbiV0;",
        ]

        for marker in required_markers:
            self.assertIn(marker, runtime_core_text)

    def test_method_invoke_supports_indirect_non_pointer_return_buffers(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "if (out_return_value_size == sizeof(void*))",
            "auto* indirect_return_value = reinterpret_cast<void* const*>(out_return_value);",
            "if (indirect_return_value == nullptr || *indirect_return_value == nullptr)",
            "std::memcpy(*indirect_return_value, return_value, out_return_value_size);",
        ]:
            self.assertIn(required_fragment, runtime_core_text)

    def test_runtime_core_exposes_generic_valuetype_kernel_helpers(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "bool CharIsAscii(std::uint16_t value)",
            "bool CharIsAsciiDigit(std::uint16_t value)",
            "bool CharIsAsciiHexDigit(std::uint16_t value)",
            "bool CharIsAsciiLetter(std::uint16_t value)",
            "bool CharIsSurrogatePair(std::uint16_t high_surrogate, std::uint16_t low_surrogate)",
            "bool CharIsWhiteSpaceLatin1(std::uint16_t value)",
            "bool CharIsLatin1(std::uint16_t value)",
            "bool HalfIsFinite(std::uint16_t value)",
            "bool HalfIsNegative(std::uint16_t value)",
            "bool HalfIsNormal(std::uint16_t value)",
            "bool HalfOperatorEquals(std::uint16_t left_value, std::uint16_t right_value)",
            "bool HalfOperatorLessThan(std::uint16_t left_value, std::uint16_t right_value)",
            "bool HalfIsNaN(std::uint16_t value)",
            "bool HalfIsInfinity(std::uint16_t value)",
            "int32_t SingleCompare(float left_value, float right_value)",
            "bool SingleEquals(float left_value, float right_value)",
            "bool SingleIsNegative(float value)",
            "bool SingleIsNormal(float value)",
            "bool SingleIsFinite(float value)",
            "bool SingleIsNaN(float value)",
            "bool SingleIsInfinity(float value)",
            "int32_t DoubleCompare(double left_value, double right_value)",
            "bool DoubleEquals(double left_value, double right_value)",
            "bool DoubleIsNegative(double value)",
            "bool DoubleIsNormal(double value)",
            "bool DoubleIsFinite(double value)",
            "bool DoubleIsNaN(double value)",
            "bool DoubleIsInfinity(double value)",
            "bool NFloatIsNegative(double value)",
            "bool NFloatIsNormal(double value)",
            "bool NFloatIsFinite(double value)",
            "bool NFloatIsNaN(double value)",
            "bool NFloatIsInfinity(double value)",
            "int32_t Int128Compare(const void* left_value, const void* right_value)",
            "bool Int128Equals(const void* left_value, const void* right_value)",
            "int32_t UInt128Compare(const void* left_value, const void* right_value)",
            "bool UInt128Equals(const void* left_value, const void* right_value)",
            "int32_t DateTimeCompareTicks(const void* left_value, const void* right_value)",
            "int32_t TimeSpanCompareTicks(const void* left_value, const void* right_value)",
            "int32_t DateOnlyCompareDayNumber(std::int32_t left_value, std::int32_t right_value)",
            "bool DateOnlyEqualsDayNumber(std::int32_t left_value, std::int32_t right_value)",
            "int32_t TimeOnlyCompareTicksValue(std::int64_t left_value, std::int64_t right_value)",
            "bool TimeOnlyEqualsTicksValue(std::int64_t left_value, std::int64_t right_value)",
            "std::isfinite(",
            "std::isnan(",
            "std::isinf(",
            "kDateTimeTicksMask",
            "ValueTypeKernelBackendKind::Intrinsic",
            "ValueTypeKernelBackendKind::Generic",
        ]:
            self.assertIn(required_fragment, runtime_core_text)


if __name__ == "__main__":
    unittest.main()
