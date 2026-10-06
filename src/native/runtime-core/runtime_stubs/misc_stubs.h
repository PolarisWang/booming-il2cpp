// ── Misc stub declarations ─────────────────────────────────────
// Assorted stubs without a clear single domain: Array, Buffer,
// Type marshalling, Culture, GC, Environment, Console, Delegate.
#pragma once

#include <chaos/compiler_hints.h>
#include <cstring>

// Array
void    ChaosArrayClear(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INT32 count) noexcept;
CHAOS_IL2CPP_INT32 ChaosArrayGetLength(CHAOS_IL2CPP_INTPTR array, CHAOS_IL2CPP_INT32 dimension) noexcept;

// Type marshalling — force-inline to eliminate call overhead for trivial casts.
// On x64, CHAOS_IL2CPP_INTPTR ≡ int64_t, so ChaosStoreInt64 is a no-op identity.
// Without forceinline, each call generates function prologue/epilogue (~10-20
// cycles) for what should be zero instructions.
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosStoreInt64(CHAOS_IL2CPP_INT64 value) noexcept {
    return static_cast<CHAOS_IL2CPP_INTPTR>(value);
}
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosStoreFloat32(CHAOS_IL2CPP_FLOAT32 value) noexcept {
    CHAOS_IL2CPP_INT32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INT64  ChaosLoadInt64(CHAOS_IL2CPP_INTPTR value) noexcept {
    return static_cast<CHAOS_IL2CPP_INT64>(value);
}

// ── Float64 bit-cast helpers (force-inline for AOT hot path) ──
// These eliminate 2 out-of-line function calls per double operation
// in Convert::ToInt32(Double), Convert::ToDecimal(Double), etc.
// JIT inlines these to 0 instructions (same-register reuse); AOT
// codegen stores double bits in GPR via memcpy, requiring explicit
// store/load.  Inline avoids call/ret overhead (~10-20 cycles each).
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_INTPTR ChaosStoreFloat64(CHAOS_IL2CPP_FLOAT64 value) noexcept {
    CHAOS_IL2CPP_INT64 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return static_cast<CHAOS_IL2CPP_INTPTR>(bits);
}
CHAOS_IL2CPP_FORCEINLINE CHAOS_IL2CPP_FLOAT64 ChaosLoadFloat64(CHAOS_IL2CPP_INTPTR value) noexcept {
    CHAOS_IL2CPP_INT64 bits = static_cast<CHAOS_IL2CPP_INT64>(value);
    CHAOS_IL2CPP_FLOAT64 result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}

// Buffer
CHAOS_IL2CPP_INT32  ChaosBufferByteLength(CHAOS_IL2CPP_INTPTR array) noexcept;
void ChaosBufferMemmove(CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INTPTR src, CHAOS_IL2CPP_SIZE count) noexcept;
void ChaosBufferMemoryCopy(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR dest, CHAOS_IL2CPP_INT64 dest_size, CHAOS_IL2CPP_INT64 src_bytes) noexcept;
void ChaosBufferBlockCopy(CHAOS_IL2CPP_INTPTR src, CHAOS_IL2CPP_INT32 src_offset, CHAOS_IL2CPP_INTPTR dst, CHAOS_IL2CPP_INT32 dst_offset, CHAOS_IL2CPP_INT32 count) noexcept;

// Culture
CHAOS_IL2CPP_INTPTR ChaosCultureGetCurrent(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetInvariant(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCompareInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDateTimeFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetDisplayName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetName(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetNumberFormat(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INT32 ChaosCompareInfoCompare(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;
CHAOS_IL2CPP_INT32 ChaosCompareInfoIndexOf(CHAOS_IL2CPP_INTPTR compare_info, CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetTextInfo(CHAOS_IL2CPP_INTPTR culture) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoToLower(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoToUpper(CHAOS_IL2CPP_INTPTR text_info, CHAOS_IL2CPP_INT32 c) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextInfoGetCultureName(CHAOS_IL2CPP_INTPTR text_info) noexcept;

// ── Globalization stubs ──
CHAOS_IL2CPP_FLOAT64 ChaosCharUnicodeInfoGetNumericValue(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCharUnicodeInfoGetDigitValue(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCharUnicodeInfoGetDecimalDigitValue(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCharUnicodeInfoGetUnicodeCategory(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INT32  ChaosCompareInfoIsSortableString(CHAOS_IL2CPP_INTPTR str) noexcept;
CHAOS_IL2CPP_INT32  ChaosCompareInfoIsSortableInt(CHAOS_IL2CPP_INT32 ch) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDateTimeFormatInfoGetInstance(CHAOS_IL2CPP_INTPTR provider) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfo(CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfoBool(CHAOS_IL2CPP_INTPTR name, CHAOS_IL2CPP_INT32 tryFirst) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureGetCultureInfoByIetfLanguageTag(CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCultureCreateSpecificCulture(CHAOS_IL2CPP_INTPTR name) noexcept;
CHAOS_IL2CPP_INTPTR ChaosCompareInfoGetCompareInfo(CHAOS_IL2CPP_INTPTR name) noexcept;

// GC
extern "C" {
void    ChaosGcCollect(CHAOS_IL2CPP_INT32 generation) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetGeneration(CHAOS_IL2CPP_INTPTR obj) noexcept;
CHAOS_IL2CPP_INT32  ChaosGcGetMaxGeneration(void) noexcept;
}

// Environment / Console
CHAOS_IL2CPP_INTPTR ChaosEnvironmentGetStackTrace(void) noexcept;
CHAOS_IL2CPP_INT32 chaos_current_managed_thread_id(void) noexcept;
CHAOS_IL2CPP_INTPTR ChaosConsoleGetError(void) noexcept;
void    ChaosConsoleWriteLine(CHAOS_IL2CPP_INTPTR value) noexcept;

// Delegate
void    ChaosDelegateInitialize(CHAOS_IL2CPP_INTPTR delegate_obj, CHAOS_IL2CPP_INTPTR target, CHAOS_IL2CPP_INTPTR method_ptr) noexcept;
CHAOS_IL2CPP_INTPTR ChaosDelegateGetTarget(CHAOS_IL2CPP_INTPTR delegate_obj) noexcept;
CHAOS_IL2CPP_INTPTR chaos_delegate_combine(CHAOS_IL2CPP_INTPTR a, CHAOS_IL2CPP_INTPTR b) noexcept;
CHAOS_IL2CPP_INTPTR chaos_delegate_remove(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 ChaosDbDataReaderExtensionsCanGetColumnSchema(CHAOS_IL2CPP_INTPTR reader) noexcept;

// S31 — System.Net.Sockets.Socket::Send NativeBody wrappers (NT-2; see misc_stubs.cpp).
// Placeholder semantics: return 0 without touching the socket (real winsock2 impl
// lands in NT-3/NT-4 chaos_net).  The wrapper unpacks the managed byte[] via
// get_managed_array before calling these, so `bytes` is a plain pointer + length.
CHAOS_IL2CPP_INT32 ChaosSocketSendBytes(
    CHAOS_IL2CPP_INTPTR socket,
    const CHAOS_IL2CPP_UINT8* bytes,
    CHAOS_IL2CPP_INTPTR byteLength,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INT32 size,
    CHAOS_IL2CPP_INT32 flags) noexcept;

CHAOS_IL2CPP_INT32 ChaosSocketSendBytesError(
    CHAOS_IL2CPP_INTPTR socket,
    const CHAOS_IL2CPP_UINT8* bytes,
    CHAOS_IL2CPP_INTPTR byteLength,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INT32 size,
    CHAOS_IL2CPP_INT32 flags,
    CHAOS_IL2CPP_INT32* error) noexcept;

// S33 - System.Net.Sockets.Socket::Receive NativeBody wrappers (NT-4; see misc_stubs.cpp).
// Same contract as Send: receive into the caller-owned buffer, report SocketError
// via *error, return 0 on failure / bytes received on success.
CHAOS_IL2CPP_INT32 ChaosSocketReceiveBytes(
    CHAOS_IL2CPP_INTPTR socket,
    CHAOS_IL2CPP_UINT8* bytes,
    CHAOS_IL2CPP_INTPTR byteLength,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INT32 size,
    CHAOS_IL2CPP_INT32 flags) noexcept;

CHAOS_IL2CPP_INT32 ChaosSocketReceiveBytesError(
    CHAOS_IL2CPP_INTPTR socket,
    CHAOS_IL2CPP_UINT8* bytes,
    CHAOS_IL2CPP_INTPTR byteLength,
    CHAOS_IL2CPP_INT32 offset,
    CHAOS_IL2CPP_INT32 size,
    CHAOS_IL2CPP_INT32 flags,
    CHAOS_IL2CPP_INT32* error) noexcept;

// S34 - System.Net.Sockets.UdpClient::SendAsync(byte[], int, IPEndPoint) NativeBody
// wrapper target (NT-8).  Returns an ALREADY-COMPLETED Task<int> handle (result 0)
// for the wire-free fact path; `bytes` is a plain pointer + length unpacked
// from the managed array by the generated wrapper.
CHAOS_IL2CPP_INTPTR ChaosUdpClientSendAsync(
    CHAOS_IL2CPP_INTPTR client,
    const CHAOS_IL2CPP_UINT8* bytes,
    CHAOS_IL2CPP_INTPTR byteLength,
    CHAOS_IL2CPP_INT32 count,
    CHAOS_IL2CPP_INTPTR endpoint) noexcept;
// S34 (NT-8): IPEndPoint::.ctor(IPAddress, Int32) — no-op (subject never reads endpoint fields).
void ChaosIPEndPointCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INTPTR port) noexcept;

