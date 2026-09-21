// unsafe_stubs.cpp — Native execute bodies for System.Runtime.CompilerServices
//                    Unsafe pointer intrinsics
//
// A2 cheap-class of the P0-B external-runtime fallback gap. Every intrinsic
// here is pure machine-pointer reinterpretation or byte-strided address
// arithmetic; no managed type metadata, no boxing, no allocation is required.
//
// ABI: flat, machine-word (CHAOS_IL2CPP_INTPTR). All arithmetic is performed
// on the integer form of the address and rebroadcast as an IntPtr, so callers
// that round-trip short/long/void* handle widths identically.
//
// Memory: no heap allocation in any path (keeps CHAOS_IL2CPP_MALLOC/Free
// domain rules trivially satisfied). Where a caller needs a byte view it
// passes the register width; these helpers never allocate.
#include <chaos/native_types.h>
#include "runtime_stubs/unsafe_stubs.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

CHAOS_IL2CPP_INTPTR ChaosUnsafeAsRef(CHAOS_IL2CPP_INTPTR source) noexcept
{
    // Unsafe.As<TFrom,TTo>(ref TFrom) / AsRef<T>(in T): the managed ref is
    // already a machine address in AOT lowering. Reinterpretation is identity.
    return source;
}

CHAOS_IL2CPP_INTPTR ChaosUnsafeAsFromPointer(CHAOS_IL2CPP_INTPTR ptr) noexcept
{
    // Unsafe.AsRef<T>(void*): wrap the raw address as a managed reference.
    return ptr;
}

CHAOS_IL2CPP_INTPTR ChaosUnsafeAddByteOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR byteOffset) noexcept
{
    // AddByteOffset<T>(ref T, IntPtr): the IntPtr byteOffset is the byte
    // distance (System.Runtime.CompilerServices defines it as byte units).
    // Compute on unsigned address to make wraparound well-defined.
    const auto base = static_cast<CHAOS_IL2CPP_UINTPTR>(source);
    const auto off = static_cast<CHAOS_IL2CPP_UINTPTR>(byteOffset);
    return static_cast<CHAOS_IL2CPP_INTPTR>(base + off);
}

CHAOS_IL2CPP_INTPTR ChaosUnsafeSubtractByteOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR byteOffset) noexcept
{
    // SubtractByteOffset<T>(ref T, IntPtr): byte-distance back-off.
    const auto base = static_cast<CHAOS_IL2CPP_UINTPTR>(source);
    const auto off = static_cast<CHAOS_IL2CPP_UINTPTR>(byteOffset);
    return static_cast<CHAOS_IL2CPP_INTPTR>(base - off);
}

CHAOS_IL2CPP_INTPTR ChaosUnsafeAddElementOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR elementSize, CHAOS_IL2CPP_INT32 index) noexcept
{
    // Unsafe.Add<T>(ref T, int index): advance `index` elements of `elementSize`
    // bytes. For AOT the caller specializes `elementSize` (a compile-time
    // constant via GenericShapeDescriptor), so multiplying here is exact and
    // free of alignment assumptions.
    //
    // index is signed int32 (.NET allows negative element offsets). Compute the
    // stride product in signed CHAOS_IL2CPP_INTPTR so a negative index retreats
    // the address instead of wrapping to a huge unsigned add; then fold onto the
    // unsigned base so pointer wraparound below the low address is well-defined.
    const auto base = static_cast<CHAOS_IL2CPP_UINTPTR>(source);
    const auto idx = static_cast<CHAOS_IL2CPP_INTPTR>(index);
    const auto delta = elementSize * idx;
    return static_cast<CHAOS_IL2CPP_INTPTR>(base + static_cast<CHAOS_IL2CPP_UINTPTR>(delta));
}

CHAOS_IL2CPP_INTPTR ChaosUnsafeSubtractElementOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR elementSize, CHAOS_IL2CPP_INT32 index) noexcept
{
    // Unsafe.Subtract<T>(ref T, int index): mirror of AddElementOffset.
    const auto base = static_cast<CHAOS_IL2CPP_UINTPTR>(source);
    const auto idx = static_cast<CHAOS_IL2CPP_INTPTR>(index);
    const auto delta = elementSize * idx;
    return static_cast<CHAOS_IL2CPP_INTPTR>(base - static_cast<CHAOS_IL2CPP_UINTPTR>(delta));
}

CHAOS_IL2CPP_INTPTR ChaosUnsafeGetObjectValue(CHAOS_IL2CPP_INTPTR valueObj) noexcept
{
    // Unsafe.GetObjectValue(ref object) / RuntimeHelpers.GetObjectValue:
    // the argument is already the stored managed reference in AOT lowering;
    // manager returns it unchanged for a non-null reference. Identity.
    return valueObj;
}

}   // extern "C"
}   // namespace chaos::il2cpp::runtime_core
