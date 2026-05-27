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
    const auto* payload = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        static_cast<const char*>(obj) + sizeof(PureTypeHeader));

    switch (ti->stable_id)
    {
        case chaos_compute_type_stable_id("System.Byte"):
            return chaos_convert_tochar_byte(*payload);
        case chaos_compute_type_stable_id("System.Char"):
        case chaos_compute_type_stable_id("System.UInt16"):
            return static_cast<CHAOS_IL2CPP_UINT16>(*payload);
        case chaos_compute_type_stable_id("System.Int16"):
            return chaos_convert_tochar_int16(*payload);
        case chaos_compute_type_stable_id("System.Int32"):
            return chaos_convert_tochar_int32(*payload);
        case chaos_compute_type_stable_id("System.Int64"):
            return chaos_convert_tochar_int64(*payload);
        case chaos_compute_type_stable_id("System.SByte"):
            return chaos_convert_tochar_sbyte(*payload);
        case chaos_compute_type_stable_id("System.UInt32"):
            return chaos_convert_tochar_uint32(*payload);
        case chaos_compute_type_stable_id("System.UInt64"):
            return chaos_convert_tochar_uint64(*payload);
        case chaos_compute_type_stable_id("System.Single"):
        {
            // Float payload: low 32 bits of *payload are the float bits
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
        case chaos_compute_type_stable_id("System.Double"):
        {
            // Double payload at offset 8, read as double (same 8-byte slot)
            auto d = chaos::il2cpp::common::load_float64(*payload);
            auto intVal = static_cast<int>(d);
            if (intVal < 0 || intVal > 0xFFFF)
            {
                chaos::il2cpp::runtime_core::chaos_raise_exception(0);
            }
            return static_cast<CHAOS_IL2CPP_UINT16>(intVal);
        }
        case chaos_compute_type_stable_id("System.Decimal"):
        {
            // Decimal payload at offset 8 is a DecimalCarrier (16B).
            // chaos_convert_tochar_decimal expects a pointer to the carrier.
            auto dec_addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
                static_cast<const char*>(obj) + sizeof(PureTypeHeader));
            return chaos_convert_tochar_decimal(dec_addr);
        }
        case chaos_compute_type_stable_id("System.Boolean"):
            return chaos_convert_tochar_boolean(*payload);
        case chaos_compute_type_stable_id("System.DateTime"):
            return chaos_convert_tochar_datetime(*payload);
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
    if (chaos_is_string_id(value))
    {
        const auto chaos_view = chaos::il2cpp::string_table::Resolve(
            chaos_extract_string_id(value));
        if (chaos_view.byte_count == 0)
        {
            chaos::il2cpp::runtime_core::chaos_raise_exception(0);
        }
        return static_cast<CHAOS_IL2CPP_UINT16>(
            static_cast<unsigned char>(chaos_view.utf8_data[0]));
    }
    chaos::il2cpp::runtime_core::chaos_raise_exception(0);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_string_provider(
    CHAOS_IL2CPP_INTPTR value, CHAOS_IL2CPP_INTPTR provider)
{
    (void)provider;
    return chaos_convert_tochar_string(value);
}
