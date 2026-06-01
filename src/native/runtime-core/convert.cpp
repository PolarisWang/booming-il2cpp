// ABI exports: extern "C" linkage for managed/NativeAOT callability.

#include "convert.h"
#include "exception_helpers.h"
#include "exception_jmp.h"
#include "string_table.h"
#include "codegen_bridge.h"
#include "generated_code_compat.h"
#include "runtime_core.h"
#include "runtime_stubs/misc_stubs.h"

#include <chaos/load_store.h>

#include <cstdint>
#include <cstring>
#include <limits>

// ── Numeric overloads: checked cast to uint16 ──────────────────────────

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_byte(CHAOS_IL2CPP_INTPTR value)
{
    // Byte range 0-255 is always valid for char
    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<CHAOS_IL2CPP_UINT8>(value & 0xFF));
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_char(CHAOS_IL2CPP_INTPTR value)
{
    // Char is already a 16-bit value, just mask and cast
    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<uint16_t>(value));
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_int16(CHAOS_IL2CPP_INTPTR value)
{
    if (value < 0)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for a character.");
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_int32(CHAOS_IL2CPP_INTPTR value)
{
    if (value < 0 || value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for a character.");
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_int64(CHAOS_IL2CPP_INTPTR value)
{
    if (value < 0 || value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for a character.");
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_sbyte(CHAOS_IL2CPP_INTPTR value)
{
    if (value < 0)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for a character.");
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_uint16(CHAOS_IL2CPP_INTPTR value)
{
    // UInt16 range 0-65535 maps exactly to char
    return static_cast<CHAOS_IL2CPP_UINT16>(static_cast<uint16_t>(value));
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_uint32(CHAOS_IL2CPP_INTPTR value)
{
    if (value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for a character.");
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_uint64(CHAOS_IL2CPP_INTPTR value)
{
    if (value > static_cast<CHAOS_IL2CPP_INTPTR>(0xFFFF))
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.OverflowException",
            "Value was either too large or too small for a character.");
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(value);
}

// ── Overloads that throw (BCL semantics: InvalidCastException) ────────

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_boolean(CHAOS_IL2CPP_INTPTR value)
{
    (void)value;
    chaos::il2cpp::runtime_core::chaos_raise_exception(0);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_datetime(CHAOS_IL2CPP_INTPTR value)
{
    (void)value;
    chaos::il2cpp::runtime_core::chaos_raise_exception(0);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_decimal(CHAOS_IL2CPP_INTPTR value)
{
    auto* carrier = reinterpret_cast<chaos::il2cpp::runtime_core::DecimalCarrier*>(value);
    uint32_t scale = (carrier->flags >> 16) & 0xFF;
    CHAOS_IL2CPP_INT64 intVal = static_cast<CHAOS_IL2CPP_INT64>(carrier->lo64);
    for (uint32_t i = 0; i < scale; i++) {
        intVal /= 10;
    }
    if (carrier->flags & 0x80000000u) {
        intVal = -intVal;
    }
    if (intVal < 0 || intVal > 0xFFFF)
    {
        chaos::il2cpp::runtime_core::chaos_raise_exception(0);
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_double(CHAOS_IL2CPP_INTPTR value)
{
    auto d = chaos::il2cpp::common::load_float64(value);
    auto intVal = static_cast<int>(d);
    if (intVal < 0 || intVal > 0xFFFF)
    {
        chaos::il2cpp::runtime_core::chaos_raise_exception(0);
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_single(CHAOS_IL2CPP_INTPTR value)
{
    CHAOS_IL2CPP_INT32 bits = static_cast<CHAOS_IL2CPP_INT32>(static_cast<int32_t>(value));
    float f;
    std::memcpy(&f, &bits, sizeof(f));
    auto intVal = static_cast<int>(f);
    if (intVal < 0 || intVal > 0xFFFF)
    {
        chaos::il2cpp::runtime_core::chaos_raise_exception(0);
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
}

// ── Object overloads (unbox then convert) ──────────────────────────────
// Boxed value types use PureTypeHeader (8B: type_info) + payload (8B) → 16B.
// Payload is at offset 8. The type is determined by TypeInfoHot::stable_id
// (64-bit FNV-1a hash of the type name).
//
// Dispatch strategy: constexpr perfect hash table (shift=14, mask=0x3F,
// 64 slots, zero collisions among all 14 eligible value types).  Replaces
// the 64-bit switch/ binary-search-tree with a single O(1) table lookup.
// Handler indices are assigned alphabetically by type name.
//
// Index mapping (handler_idx → type):
//   0=Boolean, 1=Byte, 2=Char, 3=DateTime, 4=Decimal, 5=Double,
//   6=Int16, 7=Int32, 8=Int64, 9=SByte, 10=Single, 11=UInt16,
//   12=UInt32, 13=UInt64

// Perfect hash: (stable_id >> 14) & 0x3F → handler index 0-13 (or 0xFF = not found)
static constexpr uint8_t kToCharHandlerIndex[64] = {
    255, 255, 255, 255,  10,   1,   3, 255, 255, 255,   6,  13,   5, 255, 255, 255,
    255, 255, 255,   7, 255, 255, 255,   9, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255,  12, 255,   2, 255,  11,   4, 255,
    255, 255, 255, 255,   0, 255, 255,   8, 255, 255, 255, 255, 255, 255, 255, 255,
};

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_object(CHAOS_IL2CPP_INTPTR value)
{
    if (value == 0)
    {
        chaos::il2cpp::runtime_core::RaiseManagedException(
            "System.ArgumentNullException",
            "Value cannot be null.");
    }

    const auto* obj = reinterpret_cast<const void*>(value);
    const auto* ti = chaos_object_get_type_info(obj);

    // Fast rejection: only value types (type_shape==2) can be unboxed.
    // Non-value types (reference, interface) reach here only on invalid IL.
    if (ti->type_shape != 2)
    {
        chaos::il2cpp::runtime_core::chaos_raise_exception(0);
    }

    uint32_t hash_slot = (ti->stable_id >> 14) & 0x3F;
    uint8_t handler_idx = kToCharHandlerIndex[hash_slot];
    if (handler_idx >= 14)
    {
        chaos::il2cpp::runtime_core::chaos_raise_exception(0);
    }

    const auto* payload = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        static_cast<const char*>(obj) + sizeof(PureTypeHeader));

    switch (handler_idx)
    {
        case 0:  // System.Boolean → InvalidCastException
            return chaos_convert_tochar_boolean(*payload);
        case 1:  // System.Byte → direct cast (range always valid)
            return chaos_convert_tochar_byte(*payload);
        case 2:  // System.Char → direct mask
            return static_cast<CHAOS_IL2CPP_UINT16>(*payload);
        case 3:  // System.DateTime → InvalidCastException
            return chaos_convert_tochar_datetime(*payload);
        case 4:  // System.Decimal → DecimalCarrier parse at payload offset
        {
            auto dec_addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
                static_cast<const char*>(obj) + sizeof(PureTypeHeader));
            return chaos_convert_tochar_decimal(dec_addr);
        }
        case 5:  // System.Double → IEEE 754 → int truncation
        {
            auto d = chaos::il2cpp::common::load_float64(*payload);
            auto intVal = static_cast<int>(d);
            if (intVal < 0 || intVal > 0xFFFF)
            {
                chaos::il2cpp::runtime_core::chaos_raise_exception(0);
            }
            return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
        }
        case 6:  // System.Int16 → signed range check
            return chaos_convert_tochar_int16(*payload);
        case 7:  // System.Int32 → signed range check
            return chaos_convert_tochar_int32(*payload);
        case 8:  // System.Int64 → signed range check
            return chaos_convert_tochar_int64(*payload);
        case 9:  // System.SByte → signed range check
            return chaos_convert_tochar_sbyte(*payload);
        case 10: // System.Single → IEEE 754 → int truncation
        {
            CHAOS_IL2CPP_INT32 bits = static_cast<CHAOS_IL2CPP_INT32>(
                static_cast<int32_t>(*payload));
            float f;
            std::memcpy(&f, &bits, sizeof(f));
            auto intVal = static_cast<int>(f);
            if (intVal < 0 || intVal > 0xFFFF)
            {
                chaos::il2cpp::runtime_core::chaos_raise_exception(0);
            }
            return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
        }
        case 11: // System.UInt16 → direct cast (range always valid)
            return static_cast<CHAOS_IL2CPP_UINT16>(*payload);
        case 12: // System.UInt32 → unsigned range check
            return chaos_convert_tochar_uint32(*payload);
        case 13: // System.UInt64 → unsigned range check
            return chaos_convert_tochar_uint64(*payload);
        default:
            chaos::il2cpp::runtime_core::chaos_raise_exception(0);
    }
}

namespace chaos::il2cpp::runtime_core {

// ── ConvertToChar kernel entry points for ToChar AOT bridge ──────────
// These are called from generated kernel stubs (MathKernel template).
// Each delegates to the corresponding extern "C" chaos_convert_tochar_* function.

CHAOS_IL2CPP_UINT16 ConvertToCharFromByte(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_byte(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromInt16(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_int16(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromInt32(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_int32(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromInt64(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_int64(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromSByte(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_sbyte(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromUInt32(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_uint32(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromUInt64(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_uint64(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromSingle(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_single(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromDouble(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_double(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromBoolean(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_boolean(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromDateTime(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_datetime(value); }
CHAOS_IL2CPP_UINT16 ConvertToCharFromDecimal(CHAOS_IL2CPP_INTPTR value) noexcept
    { return chaos_convert_tochar_decimal(value); }

/// Kernel entry point for ToChar(Object) AOT bridge.
/// Dispatches on the boxed value's type via TypeInfoHot::stable_id.
CHAOS_IL2CPP_UINT16 ConvertToCharFromObject(CHAOS_IL2CPP_INTPTR value) noexcept
{
    return chaos_convert_tochar_object(value);
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_object_provider(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR provider)
{
    (void)provider;
    auto* chaos_slots = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(value);
    CHAOS_IL2CPP_INTPTR payload = chaos_slots[1];
    return chaos_convert_tochar_int32(payload);
}

// ── String overloads (first UTF-8 byte → char) ─────────────────────────

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_string(CHAOS_IL2CPP_INTPTR value)
{
    // In Fact Static verification mode, ldstr pushes a tagged StringId.
    // Read the first UTF-8 byte and return it as char.
    // Uses ResolveWithGlobalCache (direct-mapped global cache, no TLS) —
    // matches the inline shape in RuntimeHelperShapeRegistry.
    const auto chaos_view = chaos::il2cpp::string_table::ResolveWithGlobalCache(
        chaos_extract_string_id(value));
    if (chaos_view.byte_count == 0)
    {
        chaos::il2cpp::runtime_core::chaos_raise_exception(0);
        return 0;
    }
    return static_cast<CHAOS_IL2CPP_UINT16>(
        static_cast<unsigned char>(chaos_view.utf8_data[0]));
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_string_provider(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR provider)
{
    (void)provider;
    return chaos_convert_tochar_string(value);
}
