// ── Unsafe / RuntimeHelpers pointer-intrinsic stub declarations ─────────
// Native execute bodies for System.Runtime.CompilerServices.Unsafe intrinsics
// that are pure machine-pointer reinterpret / byte-strided arithmetic.
//
// These are the "cheap class" of the A2 external-runtime fallback gap (P0-B):
//   Unsafe::As / AsRef / Add / AddByteOffset / Subtract / SubtractByteOffset
//   Unsafe::GetObjectValue
// Every stub below is expressed entirely at CHAOS_IL2CPP_INTPTR granularity,
// so ABI is flat (machine-word) and no managed type metadata is required.
//
// The dispatch wiring (ShapeRegistry DirectNativeSymbol / codegen
// kChaosExternalRuntimeFnTable) is tracked on the managed side (track B);
// this file provides the native execute bodies it references.
#pragma once

#include <chaos/native_types.h>

// ── Unsafe::As<TFrom,TTo>(ref TFrom) / AsRef<T>(in T) ──────────────
// Reinterpret the source reference at the same address. A-value identity.
CHAOS_IL2CPP_INTPTR ChaosUnsafeAsRef(CHAOS_IL2CPP_INTPTR source) noexcept;

// ── Unsafe::AsRef<T>(void*) ───────────────────────────────────────
// Interpret a raw void* as a managed reference to T. Address identity.
CHAOS_IL2CPP_INTPTR ChaosUnsafeAsFromPointer(CHAOS_IL2CPP_INTPTR ptr) noexcept;

// ── Unsafe::AddByteOffset<T>(ref T, IntPtr) ───────────────────────
// Advance the reference by byteOffset bytes (machine-address arithmetic).
CHAOS_IL2CPP_INTPTR ChaosUnsafeAddByteOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR byteOffset) noexcept;

// ── Unsafe::SubtractByteOffset<T>(ref T, IntPtr) ─────────────────
// Retreat the reference by byteOffset bytes (machine-address arithmetic).
CHAOS_IL2CPP_INTPTR ChaosUnsafeSubtractByteOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR byteOffset) noexcept;

// ── Unsafe::Add<T>(ref T, int index) ─────────────────────────────
// Advance the reference by `index` elements of stride `elementSize`.
// elementSize is passed explicitly so a generic instantiation may be
// specialized without managed type metadata (GenericStaticVirtualMethod
// / GenericShapeDescriptor can emit a constant stride).
CHAOS_IL2CPP_INTPTR ChaosUnsafeAddElementOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR elementSize, CHAOS_IL2CPP_INT32 index) noexcept;

// ── Unsafe::Subtract<T>(ref T, int index) ────────────────────────
// Retreat the reference by `index` elements of stride `elementSize`.
CHAOS_IL2CPP_INTPTR ChaosUnsafeSubtractElementOffset(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR elementSize, CHAOS_IL2CPP_INT32 index) noexcept;

// ── Unsafe::GetObjectValue(ref object) ───────────────────────────
// Returns the stored managed reference. For AOT the object is already an
// unboxed reference, so this is address identity (matches RuntimeHelpers.
// Managed `GetObjectValue` which returns `this` for non-null references).
CHAOS_IL2CPP_INTPTR ChaosUnsafeGetObjectValue(CHAOS_IL2CPP_INTPTR valueObj) noexcept;
