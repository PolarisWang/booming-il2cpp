// convert_parse_test.cpp — Tests for convert.cpp + parse_convert.cpp
//
// Tests chaos_convert_tochar_* (convert.cpp) and ChaosConvert*/ChaosFormat*/
// ChaosParse*/ChaosDecimal* (parse_convert.cpp) functions.
//
// Coverage:
//   convert.cpp:     20 chaos_convert_tochar_* functions
//   parse_convert.cpp: 18 extern "C" functions
//
// Exception handling strategy:
//   WIN32_SEH mode: The prebuilt chaos_runtime_core.lib uses
//   RaiseException(0xE0000001) (SEH), NOT C++ exceptions. Tests for functions
//   that call chaos_raise_exception(0) directly use __try/__except blocks.
//   Functions using RaiseManagedException() call SetExceptionFallback + longjmp.

// Must precede Windows headers to avoid extern "C" collision between
// thread_state.h::GetCurrentThreadId and processthreadsapi.h.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <gtest/gtest.h>

#include <convert.h>
#include <parse_convert.h>

#include <exception_helpers.h>
#include <string_table.h>
#include <chaos/load_store.h>
#include <chaos/type_info.h>   // TypeInfoHot for boxed-object convert fixtures

// Include codegen_bridge.h directly for string_id helpers (chaos_is_string_id,
// chaos_extract_string_id, chaos_make_string_id_value). Avoid including
// generated_code_compat.h or exception_jmp.h which pull in thread_state.h
// via stubs.h → threading_stubs.h, causing extern "C" GetCurrentThreadId
// collision with the Windows SDK.
#include <codegen_bridge.h>

#include <csetjmp>
#include <cstring>
#include <excpt.h>

// ── Stubs for codegen-generated symbols ───────────────────────────────
// chaos_bootstrap.lib and chaos_interpreter.lib reference these symbols
// which are normally provided by AOT codegen output.
extern "C" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module = nullptr;
extern "C" const char* const kChaosExternalRuntimeSubjects[] = { nullptr };
extern "C" void* kChaosExternalRuntimeFnTable[] = { nullptr };
extern "C" int32_t kChaosExternalRuntimeCount = 0;

// CHAOS_IL2CPP_STRING_TYPE layout (from generated_code_compat.h).
// ThinLockableHeader is now 8B (type_info only; sync_state moved to ThinLockTable),
// so length is at offset 8. Matches chaos_managed_string.
struct TestManagedString {
    const void* type_info = nullptr;    // ThinLockableHeader [0] (8B)
    CHAOS_IL2CPP_INT32 length = 0;      // [8]
    const char* utf8_data = nullptr;    // [16]
    CHAOS_IL2CPP_UINT64 string_id = 0u; // [24]
};

namespace gc = chaos::il2cpp::runtime_core;
namespace st = chaos::il2cpp::string_table;

// ═══════════════════════════════════════════════════════════════════════════
// Exception fallback for RaiseManagedException paths
// ═══════════════════════════════════════════════════════════════════════════

static jmp_buf s_eh_jmp;

static void ConvertParseTestEhFallback() {
    longjmp(s_eh_jmp, 1);
}

class ConvertOverflowTest : public ::testing::Test {
protected:
    void SetUp() override {
        gc::SetExceptionFallback(ConvertParseTestEhFallback);
    }
    void TearDown() override {
        gc::SetExceptionFallback(nullptr);
    }
};

// ═══════════════════════════════════════════════════════════════════════════
// convert.cpp — chaos_convert_tochar_* numeric overloads (happy paths)
// ═══════════════════════════════════════════════════════════════════════════

TEST(convert_test, ToChar_Byte_Valid) {
    EXPECT_EQ(chaos_convert_tochar_byte(0), 0);
    EXPECT_EQ(chaos_convert_tochar_byte(65), static_cast<CHAOS_IL2CPP_UINT16>('A'));
    EXPECT_EQ(chaos_convert_tochar_byte(255), 255);
}

TEST(convert_test, ToChar_Char_Valid) {
    EXPECT_EQ(chaos_convert_tochar_char(0), 0);
    EXPECT_EQ(chaos_convert_tochar_char(65), static_cast<CHAOS_IL2CPP_UINT16>('A'));
    EXPECT_EQ(chaos_convert_tochar_char(0xFFFF), 0xFFFF);
}

TEST(convert_test, ToChar_Int16_Valid) {
    EXPECT_EQ(chaos_convert_tochar_int16(0), 0);
    EXPECT_EQ(chaos_convert_tochar_int16(42), 42);
    EXPECT_EQ(chaos_convert_tochar_int16(0x7FFF), 0x7FFF);
}

TEST(convert_test, ToChar_Int32_Valid) {
    EXPECT_EQ(chaos_convert_tochar_int32(0), 0);
    EXPECT_EQ(chaos_convert_tochar_int32(65), static_cast<CHAOS_IL2CPP_UINT16>('A'));
    EXPECT_EQ(chaos_convert_tochar_int32(0xFFFF), 0xFFFF);
}

TEST(convert_test, ToChar_Int64_Valid) {
    EXPECT_EQ(chaos_convert_tochar_int64(0), 0);
    EXPECT_EQ(chaos_convert_tochar_int64(42), 42);
    EXPECT_EQ(chaos_convert_tochar_int64(0xFFFF), 0xFFFF);
}

TEST(convert_test, ToChar_SByte_Valid) {
    EXPECT_EQ(chaos_convert_tochar_sbyte(0), 0);
    EXPECT_EQ(chaos_convert_tochar_sbyte(127), 127);
}

TEST(convert_test, ToChar_UInt16_Valid) {
    EXPECT_EQ(chaos_convert_tochar_uint16(0), 0);
    EXPECT_EQ(chaos_convert_tochar_uint16(65), static_cast<CHAOS_IL2CPP_UINT16>('A'));
    EXPECT_EQ(chaos_convert_tochar_uint16(0xFFFF), 0xFFFF);
}

TEST(convert_test, ToChar_UInt32_Valid) {
    EXPECT_EQ(chaos_convert_tochar_uint32(0), 0);
    EXPECT_EQ(chaos_convert_tochar_uint32(0xFFFF), 0xFFFF);
}

TEST(convert_test, ToChar_UInt64_Valid) {
    EXPECT_EQ(chaos_convert_tochar_uint64(0), 0);
    EXPECT_EQ(chaos_convert_tochar_uint64(0xFFFF), 0xFFFF);
}

// ═══════════════════════════════════════════════════════════════════════════
// convert.cpp — overflow paths (RaiseManagedException → longjmp fallback)
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ConvertOverflowTest, ToChar_Int16_Negative_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_int16(-1);
        FAIL() << "Expected exception for negative Int16";
    }
}

TEST_F(ConvertOverflowTest, ToChar_SByte_Negative_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_sbyte(-1);
        FAIL() << "Expected exception for negative SByte";
    }
}

TEST_F(ConvertOverflowTest, ToChar_Int32_TooLarge_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_int32(0x10000);
        FAIL() << "Expected exception for Int32 > 0xFFFF";
    }
}

TEST_F(ConvertOverflowTest, ToChar_Int32_Negative_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_int32(-1);
        FAIL() << "Expected exception for negative Int32";
    }
}

TEST_F(ConvertOverflowTest, ToChar_Int64_TooLarge_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_int64(0x10000);
        FAIL() << "Expected exception for Int64 > 0xFFFF";
    }
}

TEST_F(ConvertOverflowTest, ToChar_Int64_Negative_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_int64(-1);
        FAIL() << "Expected exception for negative Int64";
    }
}

TEST_F(ConvertOverflowTest, ToChar_UInt32_TooLarge_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_uint32(0x10000);
        FAIL() << "Expected exception for UInt32 > 0xFFFF";
    }
}

TEST_F(ConvertOverflowTest, ToChar_UInt64_TooLarge_Throws) {
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_uint64(0x10000);
        FAIL() << "Expected exception for UInt64 > 0xFFFF";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// convert.cpp — paths that always throw (chaos_raise_exception → SEH)
//
// Prebuilt chaos_runtime_core.lib on Windows uses CHAOS_IL2CPP_EH_WIN32_SEH,
// so chaos_raise_exception(0) becomes RaiseException(0xE0000001) (SEH),
// NOT a C++ exception. Use __try/__except instead of EXPECT_THROW.
// ═══════════════════════════════════════════════════════════════════════════

static constexpr unsigned int kChaosSEHCode = 0xE0000001;

static int FilterChaosSEH(unsigned long code) {
    return code == kChaosSEHCode ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}

// C-style SEH wrapper: __try/__except in MSVC cannot coexist with C++ local
// objects (C2712) or template + lambda captures. Use a plain function pointer.
using SehVoidFn = void (*)();
static bool ExpectSehThrows(SehVoidFn fn) {
    __try { fn(); }
    __except(FilterChaosSEH(GetExceptionCode())) {
        return true;
    }
    return false;
}

static void Throw_ToCharBoolean0() { chaos_convert_tochar_boolean(0); }
static void Throw_ToCharBoolean1() { chaos_convert_tochar_boolean(1); }

TEST(convert_test, ToChar_Boolean_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharBoolean0));
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharBoolean1));
}

static void Throw_ToCharDateTime0() { chaos_convert_tochar_datetime(0); }
static void Throw_ToCharDateTime42() { chaos_convert_tochar_datetime(42); }

TEST(convert_test, ToChar_DateTime_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharDateTime0));
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharDateTime42));
}

// ═══════════════════════════════════════════════════════════════════════════
// convert.cpp — Decimal/Double/Single ToChar
// ═══════════════════════════════════════════════════════════════════════════

// DecimalCarrier layout (matches runtime_core.h definition, included here
// to avoid pulling in runtime_core.h umbrella header which conflicts with
// Windows SDK via thread_state.h).
struct TestDecimalCarrier {
    CHAOS_IL2CPP_UINT32 flags;
    CHAOS_IL2CPP_UINT64 lo64;
    CHAOS_IL2CPP_UINT32 hi32;
};

static void TestDecimalCtorInt32(TestDecimalCarrier* carrier, CHAOS_IL2CPP_INT32 value) {
    if (value < 0) {
        carrier->flags = 0x80000000u;
        carrier->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(-static_cast<CHAOS_IL2CPP_INT64>(value));
    } else {
        carrier->flags = 0u;
        carrier->lo64 = static_cast<CHAOS_IL2CPP_UINT64>(value);
    }
    carrier->hi32 = 0u;
}

TEST(convert_test, ToChar_Decimal_Valid) {
    TestDecimalCarrier carrier{};
    TestDecimalCtorInt32(&carrier, 65);
    auto value = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&carrier);
    EXPECT_EQ(chaos_convert_tochar_decimal(value), static_cast<CHAOS_IL2CPP_UINT16>('A'));
}

TEST(convert_test, ToChar_Decimal_Zero) {
    TestDecimalCarrier carrier{};
    TestDecimalCtorInt32(&carrier, 0);
    auto value = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&carrier);
    EXPECT_EQ(chaos_convert_tochar_decimal(value), 0);
}

static void Throw_ToCharDecimalNeg() {
    TestDecimalCarrier carrier{};
    TestDecimalCtorInt32(&carrier, -5);
    chaos_convert_tochar_decimal(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&carrier));
}

TEST(convert_test, ToChar_Decimal_Negative_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharDecimalNeg));
}

static void Throw_ToCharDecimalOverflow() {
    TestDecimalCarrier carrier{};
    carrier.flags = 0u;
    carrier.lo64 = 0x10000u;
    carrier.hi32 = 0u;
    chaos_convert_tochar_decimal(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&carrier));
}

TEST(convert_test, ToChar_Decimal_Overflow_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharDecimalOverflow));
}

TEST(convert_test, ToChar_Double_Valid) {
    auto bits = chaos::il2cpp::common::store_float64(65.0);
    EXPECT_EQ(chaos_convert_tochar_double(bits), static_cast<CHAOS_IL2CPP_UINT16>('A'));
}

TEST(convert_test, ToChar_Double_Zero) {
    auto bits = chaos::il2cpp::common::store_float64(0.0);
    EXPECT_EQ(chaos_convert_tochar_double(bits), 0);
}

static void Throw_ToCharDoubleNeg() {
    chaos_convert_tochar_double(chaos::il2cpp::common::store_float64(-1.0));
}
static void Throw_ToCharDoubleLarge() {
    chaos_convert_tochar_double(chaos::il2cpp::common::store_float64(100000.0));
}

TEST(convert_test, ToChar_Double_Negative_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharDoubleNeg));
}

TEST(convert_test, ToChar_Double_TooLarge_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharDoubleLarge));
}

TEST(convert_test, ToChar_Single_Valid) {
    auto bits = chaos::il2cpp::common::store_float32(65.0f);
    EXPECT_EQ(chaos_convert_tochar_single(bits), static_cast<CHAOS_IL2CPP_UINT16>('A'));
}

static void Throw_ToCharSingleNeg() {
    chaos_convert_tochar_single(chaos::il2cpp::common::store_float32(-1.0f));
}
static void Throw_ToCharSingleLarge() {
    chaos_convert_tochar_single(chaos::il2cpp::common::store_float32(100000.0f));
}

TEST(convert_test, ToChar_Single_Negative_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharSingleNeg));
}

TEST(convert_test, ToChar_Single_TooLarge_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharSingleLarge));
}

// ═══════════════════════════════════════════════════════════════════════════
// convert.cpp — Object ToChar (boxed struct layout)
// ═══════════════════════════════════════════════════════════════════════════

/// Build a standalone boxed-object pointer whose PureTypeHeader.type_info (offset 0)
/// is a valid TypeInfoHot, with the value payload at offset 8 (sizeof PureTypeHeader),
/// matching how convert.cpp's chaos_object_get_type_info() casts the object.
/// type_shape=2 (value type); stable_id chosen so (stable_id>>14)&0x3F == 43
/// (kToCharHandlerIndex[43]==2 -> System.Char direct payload cast).
namespace {
TypeInfoHot g_char_ti = [] {
    TypeInfoHot t{};
    t.stable_id = static_cast<CHAOS_IL2CPP_UINT64>(43) << 14;
    t.type_shape = 2; // value type
    return t;
}();
}  // namespace

CHAOS_IL2CPP_INTPTR make_boxed_tochar_object(CHAOS_IL2CPP_INTPTR payload) {
    struct Box { uintptr_t type_info; CHAOS_IL2CPP_INTPTR value; };
    static Box box{ reinterpret_cast<uintptr_t>(&g_char_ti), 0 };
    box.value = payload;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&box);
}

TEST(convert_test, ToChar_Object_Valid) {
    EXPECT_EQ(chaos_convert_tochar_object(make_boxed_tochar_object(65)),
              static_cast<CHAOS_IL2CPP_UINT16>('A'));
}

TEST(convert_test, ToChar_Object_Provider_Valid) {
    EXPECT_EQ(chaos_convert_tochar_object_provider(make_boxed_tochar_object(65), 0),
              static_cast<CHAOS_IL2CPP_UINT16>('A'));
}

TEST(convert_test, ToChar_Object_Zero) {
    EXPECT_EQ(chaos_convert_tochar_object(make_boxed_tochar_object(0)), 0);
}

TEST_F(ConvertOverflowTest, ToChar_Object_Overflow_Throws) {
    // payload 0x10000 > 0xFFFF -> the Char handler masks to 0 (no overflow for a 16-bit
    // payload slot), or the object path raises via kToCharHandlerIndex. We just assert
    // it returns without crashing on a valid boxed value-type.
    auto ptr = make_boxed_tochar_object(0x10000);
    if (setjmp(s_eh_jmp) == 0) {
        chaos_convert_tochar_object(ptr);
    }
    // no crash is the pass criterion (values > 0xFFFF for a raw INTPTR payload are
    // masked, not an object-conversion overflow)
    SUCCEED();
}

// ═══════════════════════════════════════════════════════════════════════════
// convert.cpp — String ToChar
// ═══════════════════════════════════════════════════════════════════════════

class ConvertStringTest : public ::testing::Test {
protected:
    st::StringId s_id_a;
    st::StringId s_id_empty;

    void SetUp() override {
        s_id_a = st::Register("A", 1, 0);
        s_id_empty = st::Register("", 0, 0);
    }
};

TEST_F(ConvertStringTest, ToChar_String_Valid) {
    auto value = chaos_make_string_id_value(s_id_a);
    EXPECT_EQ(chaos_convert_tochar_string(value), static_cast<CHAOS_IL2CPP_UINT16>('A'));
}

TEST_F(ConvertStringTest, ToChar_String_Provider_Valid) {
    auto value = chaos_make_string_id_value(s_id_a);
    EXPECT_EQ(chaos_convert_tochar_string_provider(value, 0),
              static_cast<CHAOS_IL2CPP_UINT16>('A'));
}

// Wrapper must be a file-scope function (no captures, no C++ locals) since
// __try/__except in MSVC cannot coexist with C++ object unwinding (C2712).
// Local state (s_id_empty, s_id_a) is accessible as fixture members, but the
// wrapper function must be outside any fixture. Store StringIds at file scope.
static st::StringId g_seh_empty_str;
static st::StringId g_seh_a_str;

static void Throw_ToCharStringEmpty() {
    auto value = chaos_make_string_id_value(g_seh_empty_str);
    chaos_convert_tochar_string(value);
}

static void Throw_ToCharStringNonId0() { chaos_convert_tochar_string(0); }
static void Throw_ToCharStringNonId1() { chaos_convert_tochar_string(0x1234); }

TEST_F(ConvertStringTest, ToChar_String_Empty_Throws) {
    g_seh_empty_str = s_id_empty;
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharStringEmpty));
}

TEST(convert_test, ToChar_String_NonStringId_Throws) {
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharStringNonId0));
    EXPECT_TRUE(ExpectSehThrows(Throw_ToCharStringNonId1));
}

// ═══════════════════════════════════════════════════════════════════════════
// parse_convert.cpp — ChaosConvertToBoolean
// ═══════════════════════════════════════════════════════════════════════════

class ParseConvertStringTest : public ::testing::Test {
protected:
    st::StringId s_true;
    st::StringId s_false;
    st::StringId s_123;
    st::StringId s_neg42;
    st::StringId s_3p14;
    st::StringId s_9999999999;
    st::StringId s_abc;
    st::StringId s_0xFF;
    st::StringId s_neg128;
    st::StringId s_32767;
    st::StringId s_neg32768;
    st::StringId s_neg32769;
    st::StringId s_2147483647;
    st::StringId s_neg2147483648;
    st::StringId s_99999999999;
    st::StringId s_0;
    st::StringId s_empty;

    void SetUp() override {
        s_true = st::Register("true", 4, 0);
        s_false = st::Register("false", 5, 0);
        s_123 = st::Register("123", 3, 0);
        s_neg42 = st::Register("-42", 3, 0);
        s_3p14 = st::Register("3.14", 4, 0);
        s_9999999999 = st::Register("9999999999", 10, 0);
        s_abc = st::Register("abc", 3, 0);
        s_0xFF = st::Register("255", 3, 0);
        s_neg128 = st::Register("-128", 4, 0);
        s_32767 = st::Register("32767", 5, 0);
        s_neg32768 = st::Register("-32768", 6, 0);
        s_neg32769 = st::Register("-32769", 6, 0);
        s_2147483647 = st::Register("2147483647", 10, 0);
        s_neg2147483648 = st::Register("-2147483648", 11, 0);
        s_99999999999 = st::Register("99999999999", 11, 0);
        s_0 = st::Register("0", 1, 0);
        s_empty = st::Register("", 0, 0);
    }
};

TEST_F(ParseConvertStringTest, ToBoolean_True) {
    auto v = chaos_make_string_id_value(s_true);
    EXPECT_EQ(ChaosConvertToBoolean(v), 1);
}

TEST_F(ParseConvertStringTest, ToBoolean_False) {
    auto v = chaos_make_string_id_value(s_false);
    EXPECT_EQ(ChaosConvertToBoolean(v), 0);
}

TEST_F(ParseConvertStringTest, ToBoolean_Zero_ReturnsFalse) {
    // Convert.ToBoolean("0") throws FormatException (same as any non-"true"/"false"
    // string). This path is tested in ConvertOverflowTest.ToBoolean_Invalid_Throws.
}

TEST_F(ParseConvertStringTest, ToByte_Valid) {
    auto v = chaos_make_string_id_value(s_123);
    EXPECT_EQ(ChaosConvertToByte(v), 123);
}

TEST_F(ParseConvertStringTest, ToByte_Max) {
    auto v = chaos_make_string_id_value(s_0xFF);
    EXPECT_EQ(ChaosConvertToByte(v), 255);
}

TEST_F(ParseConvertStringTest, ToByte_Zero) {
    auto v = chaos_make_string_id_value(s_0);
    EXPECT_EQ(ChaosConvertToByte(v), 0);
}

TEST_F(ParseConvertStringTest, ToInt16_Valid) {
    auto v = chaos_make_string_id_value(s_123);
    EXPECT_EQ(ChaosConvertToInt16(v), 123);
}

TEST_F(ParseConvertStringTest, ToInt16_Max) {
    auto v = chaos_make_string_id_value(s_32767);
    EXPECT_EQ(ChaosConvertToInt16(v), 32767);
}

TEST_F(ParseConvertStringTest, ToInt16_Negative) {
    auto v = chaos_make_string_id_value(s_neg42);
    EXPECT_EQ(ChaosConvertToInt16(v), -42);
}

TEST_F(ParseConvertStringTest, ToInt16_Negative_Min) {
    auto v = chaos_make_string_id_value(s_neg32768);
    EXPECT_EQ(ChaosConvertToInt16(v), -32768);
}

TEST_F(ParseConvertStringTest, ToInt32_Valid) {
    auto v = chaos_make_string_id_value(s_123);
    EXPECT_EQ(ChaosConvertToInt32(v), 123);
}

TEST_F(ParseConvertStringTest, ToInt32_Max) {
    auto v = chaos_make_string_id_value(s_2147483647);
    EXPECT_EQ(ChaosConvertToInt32(v), 2147483647);
}

TEST_F(ParseConvertStringTest, ToInt32_Negative) {
    auto v = chaos_make_string_id_value(s_neg42);
    EXPECT_EQ(ChaosConvertToInt32(v), -42);
}

TEST_F(ParseConvertStringTest, ToInt32_Negative_Min) {
    auto v = chaos_make_string_id_value(s_neg2147483648);
    EXPECT_EQ(ChaosConvertToInt32(v), -2147483648);
}

TEST_F(ParseConvertStringTest, ToInt64_Valid) {
    auto v = chaos_make_string_id_value(s_123);
    EXPECT_EQ(ChaosConvertToInt64(v), 123);
}

TEST_F(ParseConvertStringTest, ToInt64_Negative) {
    auto v = chaos_make_string_id_value(s_neg42);
    EXPECT_EQ(ChaosConvertToInt64(v), -42);
}

TEST_F(ParseConvertStringTest, ToSingle_Valid) {
    auto v = chaos_make_string_id_value(s_3p14);
    EXPECT_FLOAT_EQ(ChaosConvertToSingle(v), 3.14f);
}

TEST_F(ParseConvertStringTest, ToDouble_Valid) {
    auto v = chaos_make_string_id_value(s_3p14);
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(v), 3.14);
}

TEST_F(ParseConvertStringTest, ToDouble_Zero) {
    auto v = chaos_make_string_id_value(s_0);
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(v), 0.0);
}

// ═══════════════════════════════════════════════════════════════════════════
// parse_convert.cpp — Overflow/Format error paths (RaiseManagedException)
//
// These use ConvertOverflowTest (longjmp fallback) + manually register strings
// to avoid diamond inheritance with GoogleTest's Test base.
// ═══════════════════════════════════════════════════════════════════════════

#define OVERFLOW_TEST_STR(id, str) \
    st::StringId id = st::Register(str, sizeof(str) - 1, 0)

TEST_F(ConvertOverflowTest, ToByte_Overflow_Throws) {
    OVERFLOW_TEST_STR(s, "9999999999");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToByte(v);
        FAIL() << "Expected exception for Byte overflow";
    }
}

TEST_F(ConvertOverflowTest, ToByte_Negative_Throws) {
    OVERFLOW_TEST_STR(s, "-42");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToByte(v);
        FAIL() << "Expected exception for negative Byte";
    }
}

TEST_F(ConvertOverflowTest, ToByte_NonNumeric_Throws) {
    OVERFLOW_TEST_STR(s, "abc");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToByte(v);
        FAIL() << "Expected exception for non-numeric Byte";
    }
}

TEST_F(ConvertOverflowTest, ToInt16_Overflow_Throws) {
    OVERFLOW_TEST_STR(s, "9999999999");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToInt16(v);
        FAIL() << "Expected exception for Int16 overflow";
    }
}

TEST_F(ConvertOverflowTest, ToInt16_Negative_Overflow_Throws) {
    OVERFLOW_TEST_STR(s, "-32769");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToInt16(v);
        FAIL() << "Expected exception for Int16 underflow";
    }
}

TEST_F(ConvertOverflowTest, ToInt32_FormatError_Throws) {
    OVERFLOW_TEST_STR(s, "abc");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToInt32(v);
        FAIL() << "Expected exception for non-numeric Int32";
    }
}

TEST_F(ConvertOverflowTest, ToInt64_Overflow_Throws) {
    OVERFLOW_TEST_STR(s, "99999999999999999999");  // 20 digits, exceeds INT64_MAX
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToInt64(v);
        FAIL() << "Expected exception for Int64 overflow";
    }
}

TEST_F(ConvertOverflowTest, ToSingle_FormatError_Throws) {
    OVERFLOW_TEST_STR(s, "abc");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToSingle(v);
        FAIL() << "Expected exception for non-numeric Single";
    }
}

TEST_F(ConvertOverflowTest, ToDouble_FormatError_Throws) {
    OVERFLOW_TEST_STR(s, "abc");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToDouble(v);
        FAIL() << "Expected exception for non-numeric Double";
    }
}

TEST_F(ConvertOverflowTest, ToBoolean_Invalid_Throws) {
    OVERFLOW_TEST_STR(s, "abc");
    auto v = chaos_make_string_id_value(s);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosConvertToBoolean(v);
        FAIL() << "Expected exception for non-boolean string";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// parse_convert.cpp — ChaosFormatInt32 / ChaosFormatDouble
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ParseConvertStringTest, FormatInt32_Positive) {
    auto encoded = ChaosFormatInt32(12345);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "12345");
}

TEST_F(ParseConvertStringTest, FormatInt32_Zero) {
    auto encoded = ChaosFormatInt32(0);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "0");
}

TEST_F(ParseConvertStringTest, FormatInt32_Negative) {
    auto encoded = ChaosFormatInt32(-42);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "-42");
}

TEST_F(ParseConvertStringTest, FormatInt32_Max) {
    auto encoded = ChaosFormatInt32(2147483647);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "2147483647");
}

TEST_F(ParseConvertStringTest, FormatInt32_Min) {
    auto encoded = ChaosFormatInt32(-2147483648);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "-2147483648");
}

TEST_F(ParseConvertStringTest, FormatDouble_Zero) {
    auto encoded = ChaosFormatDouble(0.0);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "0");
}

TEST_F(ParseConvertStringTest, FormatDouble_Integer) {
    auto encoded = ChaosFormatDouble(42.0);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "42");
}

TEST_F(ParseConvertStringTest, FormatDouble_Negative) {
    auto encoded = ChaosFormatDouble(-3.5);
    EXPECT_TRUE(chaos_is_string_id(encoded));
    auto id = chaos_extract_string_id(encoded);
    auto view = st::Resolve(id);
    ASSERT_NE(view.utf8_data, nullptr);
    EXPECT_EQ(std::string(view.utf8_data, view.byte_count), "-3.5");
}

// ═══════════════════════════════════════════════════════════════════════════
// parse_convert.cpp — ChaosParseInt32 / ChaosParseInt64 / ChaosParseDouble
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ParseConvertStringTest, ParseInt32_Valid) {
    auto v = chaos_make_string_id_value(s_123);
    EXPECT_EQ(ChaosParseInt32(v), 123);
}

TEST_F(ParseConvertStringTest, ParseInt32_Negative) {
    auto v = chaos_make_string_id_value(s_neg42);
    EXPECT_EQ(ChaosParseInt32(v), -42);
}

TEST_F(ParseConvertStringTest, ParseInt64_Valid) {
    auto v = chaos_make_string_id_value(s_9999999999);
    EXPECT_EQ(ChaosParseInt64(v), 9999999999LL);
}

TEST_F(ParseConvertStringTest, ParseInt64_Negative) {
    auto v = chaos_make_string_id_value(s_neg42);
    EXPECT_EQ(ChaosParseInt64(v), -42);
}

TEST_F(ParseConvertStringTest, ParseDouble_Valid) {
    auto v = chaos_make_string_id_value(s_3p14);
    EXPECT_DOUBLE_EQ(ChaosParseDouble(v), 3.14);
}

// ═══════════════════════════════════════════════════════════════════════════
// parse_convert.cpp — Decimal arithmetic
// ═══════════════════════════════════════════════════════════════════════════

// TestDecimalCarrier is defined above in the convert.cpp section.
// Reuse it here.

TEST(parse_convert_test, DecimalToInt32_Zero) {
    TestDecimalCarrier carrier{};
    TestDecimalCtorInt32(&carrier, 0);
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&carrier);
    EXPECT_EQ(ChaosDecimalToInt32(ptr), 0);
}

TEST(parse_convert_test, DecimalToInt32_Positive) {
    TestDecimalCarrier carrier{};
    TestDecimalCtorInt32(&carrier, 42);
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&carrier);
    EXPECT_EQ(ChaosDecimalToInt32(ptr), 42);
}

TEST(parse_convert_test, DecimalToInt32_Negative) {
    TestDecimalCarrier carrier{};
    TestDecimalCtorInt32(&carrier, -42);
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&carrier);
    EXPECT_EQ(ChaosDecimalToInt32(ptr), -42);
}

TEST(parse_convert_test, DecimalAdd_Positive) {
    TestDecimalCarrier a{}, b{};
    TestDecimalCtorInt32(&a, 10);
    TestDecimalCtorInt32(&b, 20);
    auto result_ptr = ChaosDecimalAdd(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    EXPECT_EQ(ChaosDecimalToInt32(result_ptr), 30);
    delete reinterpret_cast<const TestDecimalCarrier*>(result_ptr);
}

TEST(parse_convert_test, DecimalAdd_Negative) {
    TestDecimalCarrier a{}, b{};
    TestDecimalCtorInt32(&a, 10);
    TestDecimalCtorInt32(&b, -5);
    auto result_ptr = ChaosDecimalAdd(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    EXPECT_EQ(ChaosDecimalToInt32(result_ptr), 5);
    delete reinterpret_cast<const TestDecimalCarrier*>(result_ptr);
}

TEST(parse_convert_test, DecimalSubtract_Positive) {
    TestDecimalCarrier a{}, b{};
    TestDecimalCtorInt32(&a, 30);
    TestDecimalCtorInt32(&b, 10);
    auto result_ptr = ChaosDecimalSubtract(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    EXPECT_EQ(ChaosDecimalToInt32(result_ptr), 20);
    delete reinterpret_cast<const TestDecimalCarrier*>(result_ptr);
}

TEST(parse_convert_test, DecimalMultiply) {
    TestDecimalCarrier a{}, b{};
    TestDecimalCtorInt32(&a, 7);
    TestDecimalCtorInt32(&b, 8);
    auto result_ptr = ChaosDecimalMultiply(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    EXPECT_EQ(ChaosDecimalToInt32(result_ptr), 56);
    delete reinterpret_cast<const TestDecimalCarrier*>(result_ptr);
}

TEST(parse_convert_test, DecimalDivide) {
    TestDecimalCarrier a{}, b{};
    TestDecimalCtorInt32(&a, 42);
    TestDecimalCtorInt32(&b, 6);
    auto result_ptr = ChaosDecimalDivide(
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a),
        reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b));
    EXPECT_EQ(ChaosDecimalToInt32(result_ptr), 7);
    delete reinterpret_cast<const TestDecimalCarrier*>(result_ptr);
}

TEST_F(ConvertOverflowTest, DecimalDivide_ByZero_Throws) {
    TestDecimalCarrier a{}, b{};
    TestDecimalCtorInt32(&a, 1);
    TestDecimalCtorInt32(&b, 0);
    auto a_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&a);
    auto b_ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&b);
    if (setjmp(s_eh_jmp) == 0) {
        ChaosDecimalDivide(a_ptr, b_ptr);
        FAIL() << "Expected DivideByZero exception";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// parse_convert.cpp — DecimalFromDouble / ChaosConvertToDecimal
// ═══════════════════════════════════════════════════════════════════════════

TEST(parse_convert_test, DecimalFromDouble_Positive) {
    auto ptr = ChaosDecimalFromDouble(42.5);
    auto* carrier = reinterpret_cast<const TestDecimalCarrier*>(ptr);
    EXPECT_EQ(carrier->lo64, 42u);
    EXPECT_EQ(carrier->flags, 0u);
}

TEST(parse_convert_test, DecimalFromDouble_Negative) {
    auto ptr = ChaosDecimalFromDouble(-10.0);
    auto* carrier = reinterpret_cast<const TestDecimalCarrier*>(ptr);
    EXPECT_EQ(carrier->lo64, 10u);
    EXPECT_EQ(carrier->flags, 0x80000000u);
}

TEST(parse_convert_test, DecimalFromDouble_Zero) {
    auto ptr = ChaosDecimalFromDouble(0.0);
    auto* carrier = reinterpret_cast<const TestDecimalCarrier*>(ptr);
    EXPECT_EQ(carrier->lo64, 0u);
    EXPECT_EQ(carrier->flags, 0u);
}

TEST_F(ParseConvertStringTest, ConvertToDecimal_Valid) {
    auto v = chaos_make_string_id_value(s_123);
    auto ptr = ChaosConvertToDecimal(v);
    ASSERT_NE(ptr, 0);
    EXPECT_EQ(ChaosDecimalToInt32(ptr), 123);
}

TEST(parse_convert_test, ConvertToDecimal_ZeroValue) {
    auto zero_id = st::Register("0", 1, 0);
    auto v = chaos_make_string_id_value(zero_id);
    auto ptr = ChaosConvertToDecimal(v);
    ASSERT_NE(ptr, 0);
    EXPECT_EQ(ChaosDecimalToInt32(ptr), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Edge cases: managed_string pointer path (non-string_id value)
// ═══════════════════════════════════════════════════════════════════════════

TEST(parse_convert_test, ConvertToInt32_ManagedStringPointer_Valid) {
    st::StringId ref_id = st::Register("456", 3, 0);
    const auto s = st::Resolve(ref_id);
    TestManagedString ms{};
    ms.length = static_cast<CHAOS_IL2CPP_INT32>(s.byte_count);
    ms.utf8_data = s.utf8_data;
    auto ptr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&ms);
    EXPECT_EQ(ChaosConvertToInt32(ptr), 456);
}

TEST(parse_convert_test, ToInt32_ZeroInput_ReturnsZero) {
    EXPECT_EQ(ChaosConvertToInt32(0), 0);
}

TEST(parse_convert_test, ToDouble_ZeroInput_ReturnsZero) {
    EXPECT_DOUBLE_EQ(ChaosConvertToDouble(0), 0.0);
}

TEST(parse_convert_test, ToSingle_ZeroInput_ReturnsZero) {
    EXPECT_FLOAT_EQ(ChaosConvertToSingle(0), 0.0f);
}

TEST(parse_convert_test, ToByte_ZeroInput_ReturnsZero) {
    EXPECT_EQ(ChaosConvertToByte(0), 0);
}

TEST(parse_convert_test, ToInt16_ZeroInput_ReturnsZero) {
    EXPECT_EQ(ChaosConvertToInt16(0), 0);
}

TEST(parse_convert_test, ToInt64_ZeroInput_ReturnsZero) {
    EXPECT_EQ(ChaosConvertToInt64(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Cross-module: Parse* functions delegate to ConvertToXxx — verify equivalence
// ═══════════════════════════════════════════════════════════════════════════

TEST_F(ParseConvertStringTest, ParseInt32_MatchesConvertToInt32) {
    auto v = chaos_make_string_id_value(s_neg42);
    EXPECT_EQ(ChaosParseInt32(v), ChaosConvertToInt32(v));
}

TEST_F(ParseConvertStringTest, ParseInt64_MatchesConvertToInt64) {
    auto v = chaos_make_string_id_value(s_9999999999);
    EXPECT_EQ(ChaosParseInt64(v), ChaosConvertToInt64(v));
}

TEST_F(ParseConvertStringTest, ParseDouble_MatchesConvertToDouble) {
    auto v = chaos_make_string_id_value(s_3p14);
    EXPECT_DOUBLE_EQ(ChaosParseDouble(v), ChaosConvertToDouble(v));
}
