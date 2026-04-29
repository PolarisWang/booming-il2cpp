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
            "TypeQueryCapability",
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

    def test_runtime_core_exposes_type_capability_authority_seam(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")
        runtime_core_header_text = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "RuntimeTypeCapabilityInfoV0",
            "TypeQueryCapability(",
            "value_size_bytes",
            "vector_lane_kind",
            "vector_lane_count",
            "vector_width_bytes",
            "TypeCapabilityVectorLaneKind",
            "GenericContextGetClassArgCount(",
            "GenericContextGetClassArg(",
            "GenericContextGetMethodArgCount(",
            "GenericContextGetMethodArg(",
        ]:
            self.assertIn(required_fragment, runtime_core_text + "\n" + runtime_core_header_text)

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
            "bool CharIsAscii(std::CHAOS_IL2CPP_UINT16 value)",
            "bool CharIsAsciiDigit(std::CHAOS_IL2CPP_UINT16 value)",
            "bool CharIsAsciiHexDigit(std::CHAOS_IL2CPP_UINT16 value)",
            "bool CharIsAsciiLetter(std::CHAOS_IL2CPP_UINT16 value)",
            "bool CharIsSurrogatePair(std::CHAOS_IL2CPP_UINT16 high_surrogate, std::CHAOS_IL2CPP_UINT16 low_surrogate)",
            "bool CharIsWhiteSpaceLatin1(std::CHAOS_IL2CPP_UINT16 value)",
            "bool CharIsLatin1(std::CHAOS_IL2CPP_UINT16 value)",
            "bool HalfIsFinite(std::CHAOS_IL2CPP_UINT16 value)",
            "bool HalfIsNegative(std::CHAOS_IL2CPP_UINT16 value)",
            "bool HalfIsNormal(std::CHAOS_IL2CPP_UINT16 value)",
            "bool HalfOperatorEquals(std::CHAOS_IL2CPP_UINT16 left_value, std::CHAOS_IL2CPP_UINT16 right_value)",
            "bool HalfOperatorLessThan(std::CHAOS_IL2CPP_UINT16 left_value, std::CHAOS_IL2CPP_UINT16 right_value)",
            "bool HalfIsNaN(std::CHAOS_IL2CPP_UINT16 value)",
            "bool HalfIsInfinity(std::CHAOS_IL2CPP_UINT16 value)",
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
            "DEFINE_NFLOAT_BOOL_DELEGATE(IsNegative);",
            "DEFINE_NFLOAT_BOOL_DELEGATE(IsNormal);",
            "DEFINE_NFLOAT_BOOL_DELEGATE(IsFinite);",
            "DEFINE_NFLOAT_BOOL_DELEGATE(IsNaN);",
            "DEFINE_NFLOAT_BOOL_DELEGATE(IsInfinity);",
            "int32_t Int128Compare(const void* left_value, const void* right_value)",
            "bool Int128Equals(const void* left_value, const void* right_value)",
            "int32_t UInt128Compare(const void* left_value, const void* right_value)",
            "bool UInt128Equals(const void* left_value, const void* right_value)",
            "int32_t DateTimeCompareTicks(const void* left_value, const void* right_value)",
            "int32_t TimeSpanCompareTicks(const void* left_value, const void* right_value)",
            "int32_t DateOnlyCompareDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value)",
            "bool DateOnlyEqualsDayNumber(CHAOS_IL2CPP_INT32 left_value, CHAOS_IL2CPP_INT32 right_value)",
            "int32_t TimeOnlyCompareTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value)",
            "bool TimeOnlyEqualsTicksValue(CHAOS_IL2CPP_INT64 left_value, CHAOS_IL2CPP_INT64 right_value)",
            "std::isfinite(",
            "std::isnan(",
            "std::isinf(",
            "kDateTimeTicksMask",
            "ValueTypeKernelBackendKind::Intrinsic",
            "ValueTypeKernelBackendKind::Generic",
        ]:
            self.assertIn(required_fragment, runtime_core_text)

    def test_runtime_core_exposes_marshal_platform_root_and_memory_block_helpers(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")
        runtime_core_header_text = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "struct MarshalPlatformAbiRootV1",
            "struct MarshalMemoryBlockAbiV1",
            "struct MarshalStringMarshalingAbiV1",
            "struct MarshalStructureLayoutAbiV1",
            "enum class MarshalAllocationKind",
            "enum class MarshalStructureMarshallingClass",
            "GetMarshalPlatformAbiRootV1()",
            "MarshalAllocHGlobal(",
            "MarshalAllocCoTaskMem(",
            "MarshalReAllocHGlobal(",
            "MarshalReAllocCoTaskMem(",
            "MarshalFreeHGlobal(",
            "MarshalFreeCoTaskMem(",
            "MarshalStringToCoTaskMemUtf8(",
            "MarshalPtrToStringUtf8(",
            "MarshalReadInt32(",
            "MarshalWriteIntPtr(",
            "kMarshalPlatformAbiRootV1",
            "MarshalPlatformCapabilityMemoryBlock",
            "MarshalPlatformCapabilityStringMarshaling",
            "MarshalPlatformCapabilityStructureLayout",
            "MarshalAllocationHeader",
            "kMarshalAllocationMagic",
        ]:
            self.assertIn(required_fragment, runtime_core_text + "\n" + runtime_core_header_text)

    def test_runtime_core_exposes_task_runtime_kernel_root(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")
        runtime_core_header_text = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "enum TaskRuntimeKernelCapabilityBits",
            "struct TaskRuntimeKernelV1",
            "GetTaskRuntimeKernelV1()",
            "kTaskRuntimeKernelV1",
            "TaskRuntimeKernelCapabilityStateCore",
            "TaskRuntimeKernelCapabilityContinuation",
            "TaskRuntimeKernelCapabilityFactory",
            "TaskRuntimeKernelCapabilityIntrospection",
            "TaskKernelNewId()",
            "g_next_task_id",
        ]:
            self.assertIn(required_fragment, runtime_core_text + "\n" + runtime_core_header_text)

    def test_runtime_core_exposes_interop_kernel32_platform_helpers(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")
        runtime_core_header_text = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "InteropKernel32GetLastError()",
            "InteropKernel32GetCurrentProcessId()",
            "InteropKernel32GetCurrentThreadId()",
            "InteropKernel32GetCurrentProcess()",
            "InteropKernel32GetCurrentThread()",
            "InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle)",
            "InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle)",
            "::GetLastError()",
            "::GetCurrentProcessId()",
            "::CloseHandle(",
            "::FreeLibrary(",
        ]:
            self.assertIn(required_fragment, runtime_core_text + "\n" + runtime_core_header_text)

    def test_runtime_core_exposes_vector_kernel_foundation_helpers(self) -> None:
        runtime_core_text = RUNTIME_CORE_SOURCE_PATH.read_text(encoding="utf-8")
        runtime_core_header_text = RUNTIME_CORE_HEADER_PATH.read_text(encoding="utf-8")

        for required_fragment in [
            "enum class VectorKernelBackendKind",
            "GetVectorKernelBackend()",
            "struct RuntimeNumericsVector2Carrier",
            "struct RuntimeNumericsVector3Carrier",
            "struct RuntimeNumericsVector4Carrier",
            "struct RuntimeNumericsMatrix3x2Carrier",
            "struct RuntimeNumericsMatrix4x4Carrier",
            "struct RuntimeNumericsQuaternionCarrier",
            "struct RuntimeIntrinsicVector64Carrier",
            "struct RuntimeIntrinsicVector128Carrier",
            "struct RuntimeIntrinsicVector256Carrier",
            "struct RuntimeIntrinsicVector512Carrier",
            "inline TCarrier VectorFixedApplyUnary(",
            "RuntimeNumericsVector2Carrier Vector2Zero()",
            "RuntimeNumericsVector2Carrier Vector2One()",
            "RuntimeNumericsVector2Carrier Vector2UnitX()",
            "RuntimeNumericsVector2Carrier Vector2UnitY()",
            "RuntimeNumericsVector2Carrier Vector2Add(",
            "RuntimeNumericsVector2Carrier Vector2Subtract(",
            "RuntimeNumericsVector2Carrier Vector2Multiply(",
            "RuntimeNumericsVector2Carrier Vector2MultiplyScalar(",
            "RuntimeNumericsVector2Carrier Vector2Divide(",
            "RuntimeNumericsVector2Carrier Vector2DivideScalar(",
            "RuntimeNumericsVector2Carrier Vector2Abs(",
            "RuntimeNumericsVector2Carrier Vector2Min(",
            "RuntimeNumericsVector2Carrier Vector2Max(",
            "RuntimeNumericsVector2Carrier Vector2Clamp(",
            "RuntimeNumericsVector2Carrier Vector2SquareRoot(",
            "RuntimeNumericsVector2Carrier Vector2Normalize(",
            "RuntimeNumericsVector2Carrier Vector2Lerp(",
            "RuntimeNumericsVector2Carrier Vector2Reflect(",
            "bool Vector2Equals(",
            "int32_t Vector2GetHashCode(",
            "float Vector2Length(",
            "float Vector2GetElement(",
            "RuntimeNumericsVector2Carrier Vector2Negate(",
            "float Vector2Dot(",
            "float Vector2LengthSquared(",
            "float Vector2DistanceSquared(",
            "RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix)",
            "RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix)",
            "RuntimeNumericsVector2Carrier Vector2Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation)",
            "RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix3x2Carrier matrix)",
            "RuntimeNumericsVector2Carrier Vector2TransformNormal(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix)",
            "bool Vector2TryCopyTo(",
            "inline TCarrier VectorFixedCompareEqual(",
            "inline TCarrier VectorFixedCompareNotEqual(",
            "inline TCarrier VectorFixedCompareGreaterThan(",
            "inline TCarrier VectorFixedCompareGreaterThanOrEqual(",
            "inline TCarrier VectorFixedCompareLessThan(",
            "inline TCarrier VectorFixedCompareLessThanOrEqual(",
            "inline TCarrier VectorFixedMin(",
            "inline TCarrier VectorFixedMax(",
            "inline TCarrier VectorFixedAbs(",
            "inline TCarrier VectorFixedNegate(",
            "inline TCarrier VectorFixedBitwiseAndNot(",
            "inline TCarrier VectorFixedBitwiseSelect(",
            "RuntimeNumericsVector3Carrier Vector3Zero()",
            "RuntimeNumericsVector3Carrier Vector3One()",
            "RuntimeNumericsVector3Carrier Vector3UnitZ()",
            "RuntimeNumericsVector3Carrier Vector3Add(",
            "RuntimeNumericsVector3Carrier Vector3Subtract(",
            "RuntimeNumericsVector3Carrier Vector3Multiply(",
            "RuntimeNumericsVector3Carrier Vector3MultiplyScalar(",
            "RuntimeNumericsVector3Carrier Vector3Divide(",
            "RuntimeNumericsVector3Carrier Vector3DivideScalar(",
            "RuntimeNumericsVector3Carrier Vector3Abs(",
            "RuntimeNumericsVector3Carrier Vector3Min(",
            "RuntimeNumericsVector3Carrier Vector3Max(",
            "RuntimeNumericsVector3Carrier Vector3Clamp(",
            "RuntimeNumericsVector3Carrier Vector3SquareRoot(",
            "RuntimeNumericsVector3Carrier Vector3Normalize(",
            "RuntimeNumericsVector3Carrier Vector3Lerp(",
            "RuntimeNumericsVector3Carrier Vector3Reflect(",
            "RuntimeNumericsVector3Carrier Vector3Cross(",
            "bool Vector3Equals(",
            "int32_t Vector3GetHashCode(",
            "float Vector3Length(",
            "float Vector3GetElement(",
            "RuntimeNumericsVector3Carrier Vector3Negate(",
            "float Vector3Dot(",
            "float Vector3LengthSquared(",
            "float Vector3DistanceSquared(",
            "RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix)",
            "RuntimeNumericsVector3Carrier Vector3Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation)",
            "RuntimeNumericsVector3Carrier Vector3TransformNormal(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix)",
            "bool Vector3TryCopyTo(",
            "RuntimeNumericsVector4Carrier Vector4Zero()",
            "RuntimeNumericsVector4Carrier Vector4One()",
            "RuntimeNumericsVector4Carrier Vector4UnitW()",
            "RuntimeNumericsVector4Carrier Vector4Add(",
            "RuntimeNumericsVector4Carrier Vector4Subtract(",
            "RuntimeNumericsVector4Carrier Vector4Multiply(",
            "RuntimeNumericsVector4Carrier Vector4MultiplyScalar(",
            "RuntimeNumericsVector4Carrier Vector4Divide(",
            "RuntimeNumericsVector4Carrier Vector4DivideScalar(",
            "RuntimeNumericsVector4Carrier Vector4Abs(",
            "RuntimeNumericsVector4Carrier Vector4Min(",
            "RuntimeNumericsVector4Carrier Vector4Max(",
            "RuntimeNumericsVector4Carrier Vector4Clamp(",
            "RuntimeNumericsVector4Carrier Vector4SquareRoot(",
            "RuntimeNumericsVector4Carrier Vector4Normalize(",
            "RuntimeNumericsVector4Carrier Vector4Lerp(",
            "bool Vector4Equals(",
            "int32_t Vector4GetHashCode(",
            "float Vector4Length(",
            "float Vector4GetElement(",
            "RuntimeNumericsVector4Carrier Vector4Negate(",
            "float Vector4Dot(",
            "float Vector4LengthSquared(",
            "float Vector4DistanceSquared(",
            "RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsMatrix4x4Carrier matrix)",
            "RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector2Carrier value, RuntimeNumericsQuaternionCarrier rotation)",
            "RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsMatrix4x4Carrier matrix)",
            "RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector3Carrier value, RuntimeNumericsQuaternionCarrier rotation)",
            "RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsMatrix4x4Carrier matrix)",
            "RuntimeNumericsVector4Carrier Vector4Transform(RuntimeNumericsVector4Carrier value, RuntimeNumericsQuaternionCarrier rotation)",
            "bool Vector4TryCopyTo(",
            "RuntimeIntrinsicVector64Carrier Vector64Reinterpret(",
            "RuntimeIntrinsicVector128Carrier Vector128Reinterpret(",
            "RuntimeIntrinsicVector256Carrier Vector256Reinterpret(",
            "RuntimeIntrinsicVector512Carrier Vector512Reinterpret(",
            "VectorPlatformByteWidth()",
            'display_name.rfind("System.Numerics.Vector<", 0) == 0',
            "VectorFixedAdd(",
            "VectorFixedBroadcast(",
            "VectorFixedCreateScalar(",
            "VectorFixedCreateFromLanes(",
            "VectorFixedCreateFromArray(",
            "VectorFixedCreateFromHalves(",
            "VectorFixedExtractShiftCount(",
            "VectorFixedSubtract(",
            "VectorFixedMultiply(",
            "VectorFixedMultiplyScalar(",
            "VectorFixedDivide(",
            "VectorFixedDivideScalar(",
            "VectorFixedEquals(",
            "VectorFixedBitwiseAnd(",
            "VectorFixedBitwiseOr(",
            "VectorFixedBitwiseXor(",
            "VectorFixedTernaryLogic(",
            "VectorFixedRotateLeft(",
            "VectorFixedRotateRight(",
            "VectorFixedRotateLeftVariable(",
            "VectorFixedRotateRightVariable(",
            "VectorFixedAlignRight32(",
            "VectorFixedAlignRight64(",
            "VectorFixedShuffle2x128(",
            "VectorFixedConvertToVector128(",
            "VectorFixedConvertToVector128Saturating(",
            "VectorFixedPermuteVar(",
            "VectorFixedPermuteVarX2(",
            "VectorFixedGetExponent(",
            "VectorFixedGetMantissa(",
            "VectorFixedReciprocal14(",
            "VectorFixedReciprocalSqrt14(",
            "VectorFixedRoundScale(",
            "VectorFixedScale(",
            "VectorFixedFixup(",
            "VectorFixedOnesComplement(",
            "VectorFixedOneFromCapability(",
            "VectorKernelBackendKind::Generic",
        ]:
            self.assertIn(required_fragment, runtime_core_text + "\n" + runtime_core_header_text)


if __name__ == "__main__":
    unittest.main()
