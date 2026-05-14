// ABI exports: extern "C" linkage for managed/NativeAOT callability.

#include "convert.h"
#include "exception_helpers.h"
#include "exception_jmp.h"
#include "string_table.h"
#include "codegen_bridge.h"
#include "generated_code_compat.h"

#include <cstdint>
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
    (void)value;
    chaos::il2cpp::runtime_core::chaos_raise_exception(0);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_double(CHAOS_IL2CPP_INTPTR value)
{
    (void)value;
    chaos::il2cpp::runtime_core::chaos_raise_exception(0);
}

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_single(CHAOS_IL2CPP_INTPTR value)
{
    (void)value;
    chaos::il2cpp::runtime_core::chaos_raise_exception(0);
}

// ── Object overloads (unbox then convert) ──────────────────────────────
// Boxed Int32 uses PureTypeHeader (8B: type_info) + value (8B) = 16B.
// Payload is at offset 8 = intptr slot [1].

extern "C" CHAOS_IL2CPP_UINT16 chaos_convert_tochar_object(CHAOS_IL2CPP_INTPTR value)
{
    // The value is a pointer to a stack-allocated boxed struct:
    //   PureTypeHeader (8B: type_info) + payload (8B) = 16B total.
    // Extract the payload at offset 8 (= slot [1] when viewed as intptr[]).
    auto* chaos_slots = reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(value);
    CHAOS_IL2CPP_INTPTR payload = chaos_slots[1];
    return chaos_convert_tochar_int32(payload);
}

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
