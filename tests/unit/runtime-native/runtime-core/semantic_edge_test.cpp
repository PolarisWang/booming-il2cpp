// semantic_edge_test.cpp — 防线 3: 蒙过（default-input stub）检测
//
// 目标：对曾因"只测 default 输入而蒙过"的 native 方法，用**显式非-default 输入**
// 断言其真 .NET 语义。这些测试独立于 foundation-dll fact（fact 探针默认输
// 入可能掩盖 stub-0），确保即使 ATG 未生成多值探针，语义回归也能被捕获。
//
// 覆盖：
//   ChaosConvertChangeType      — Convert.ChangeType(42,Int32) → boxed 42（非 0）
//   ChaosSpanInt32ToArray       — default span → 空数组；非空 span → 正确数据数组
//
// 约定：本文件直接调用 native（非 AOT codegen），验证它们的真实行为。

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <gtest/gtest.h>

#include <parse_convert.h>
#include <runtime_stubs/array_stubs.h>
#include <runtime_stubs/stub_common.h>
#include <gc_helpers.h>
#include <exception_helpers.h>

#include <cstring>
#include <vector>

// 构造一个 boxed Int32 对象（ThinLockableHeader 16B + int32 payload）。
// 布局与 enum_stubs.cpp 的 enum_alloc_boxed_int32 一致。
static CHAOS_IL2CPP_INTPTR MakeBoxedInt32(CHAOS_IL2CPP_INT32 value) {
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(20));
    if (storage == nullptr) return 0;
    std::memset(storage, 0, 16);
    std::memcpy(storage + 16, &value, sizeof(value));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

// 构造一个 boxed Boolean 对象（ThinLockableHeader 16B + bool 1B）。
static CHAOS_IL2CPP_INTPTR MakeBoxedBool(bool value) {
    auto* storage = static_cast<unsigned char*>(GcAllocateAtomic(17));
    if (storage == nullptr) return 0;
    std::memset(storage, 0, 16);
    storage[16] = value ? 1 : 0;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(storage);
}

// 读 boxed Int32 的 payload（offset 16）。
static CHAOS_IL2CPP_INT32 ReadBoxedInt32(CHAOS_IL2CPP_INTPTR obj) {
    if (obj == 0) return -999;
    const auto* bytes = reinterpret_cast<const unsigned char*>(obj);
    CHAOS_IL2CPP_INT32 v = 0;
    std::memcpy(&v, bytes + 16, sizeof(v));
    return v;
}

// ── Convert.ChangeType(object=boxed Int32(42), TypeCode=Int32) → boxed Int32(42) ──
TEST(semantic_edge_test, ChangeType_BoxedInt32_ToInt32_Returns42) {
    auto boxed42 = MakeBoxedInt32(42);
    auto result = ChaosConvertChangeType(boxed42, /* TypeCode.Int32 */ 9);
    ASSERT_NE(result, CHAOS_IL2CPP_INTPTR(0));
    EXPECT_EQ(ReadBoxedInt32(result), 42);
}

// ── Convert.ChangeType(object=boxed Bool(true), TypeCode=Boolean) → boxed Bool(true) ──
TEST(semantic_edge_test, ChangeType_BoxedBoolTrue_ToBoolean_True) {
    auto boxedTrue = MakeBoxedBool(true);
    auto result = ChaosConvertChangeType(boxedTrue, /* TypeCode.Boolean */ 3);
    ASSERT_NE(result, CHAOS_IL2CPP_INTPTR(0));
    const auto* bytes = reinterpret_cast<const unsigned char*>(result);
    EXPECT_NE(bytes[16], 0);
}

// ── Convert.ChangeType(null, Int32) → null (default 输入) — 不蒙过但确认不 crash ──
TEST(semantic_edge_test, ChangeType_Null_ToInt32_ReturnsNull) {
    EXPECT_EQ(ChaosConvertChangeType(0, 9), CHAOS_IL2CPP_INTPTR(0));
}

// ── ReadOnlySpan<Int32>.ToArray 内存布局: [pointer, length] 两个 native int ──
// 非空 span: pointer 指向 3 个 int32 {10,20,30}, length=3 → 应返回 length=3 的数组
TEST(semantic_edge_test, SpanInt32ToArray_NonEmpty_CopiesData) {
    CHAOS_IL2CPP_INT32 data[3] = {10, 20, 30};
    // 组装 span 对（pointer 在 offset0, length 在 offset8）
    alignas(sizeof(void*)) unsigned char span_storage[16] = {};
    auto* fields = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(span_storage);
    fields[0] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(data);
    fields[1] = 3;  // length

    auto arr = reinterpret_cast<ManagedArrayAccessor*>(
        ChaosSpanInt32ToArray(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(span_storage)));
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(arr->length, 3);
    auto* elems = reinterpret_cast<const CHAOS_IL2CPP_INT32*>(accessor_get_elements(arr));
    EXPECT_EQ(elems[0], 10);
    EXPECT_EQ(elems[1], 20);
    EXPECT_EQ(elems[2], 30);
}

// ── default ReadOnlySpan<Int32> = {pointer=0, length=0} → 空数组 length=0 ──
TEST(semantic_edge_test, SpanInt32ToArray_Default_ReturnsEmptyArray) {
    alignas(sizeof(void*)) unsigned char span_storage[16] = {};
    auto* fields = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(span_storage);
    fields[0] = 0;  // null pointer
    fields[1] = 0;  // zero length

    auto arr = reinterpret_cast<ManagedArrayAccessor*>(
        ChaosSpanInt32ToArray(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(span_storage)));
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(arr->length, 0);
}

// ── null span_pair → 空数组（防 crash） ──
TEST(semantic_edge_test, SpanInt32ToArray_NullSpan_ReturnsEmptyArray) {
    auto arr = reinterpret_cast<ManagedArrayAccessor*>(ChaosSpanInt32ToArray(0));
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(arr->length, 0);
}
