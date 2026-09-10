// crypto_stubs.cpp — Cryptography stub implementations (dispatcher)
//
// The full implementation has been split across platform-specific files which
// are text-included here so this translation unit compiles as before WITHOUT
// any change to CMakeLists.txt (which only lists crypto_stubs.cpp for this
// directory).  Every platform file first includes crypto_stubs_internal.h,
// which carries the common includes, GcAllocateAtomic-based shared helper
// declaration, and the unchanged #ifdef _WIN32 Windows/bcrypt includes.
//
// Composition order matches the original monolith so condition-compilation is
// preserved exactly:
//   1. crypto_stubs_common.cpp — shared alloc_byte_array (defined BEFORE the
//      platform split so both BCrypt and OpenSSL paths can use it).
//   2. crypto_stubs_win32.cpp  — Windows CNG (BCrypt) implementations, guarded
//      by #ifdef _WIN32; empty on non-Windows.
//   3. crypto_stubs_posix.cpp  — OpenSSL implementations, guarded by
//      #ifndef _WIN32; empty on Windows.
//   4. ChaosAesCreate()        — cross-platform stub retained here.
//
// ABI exports: extern "C" linkage for managed/NativeAOT callability.

#include "crypto_stubs_internal.h"
#include "crypto_stubs_common.cpp"
#include "crypto_stubs_win32.cpp"
#include "crypto_stubs_posix.cpp"

namespace chaos::il2cpp::runtime_core {
extern "C" {

// ── AES Create (stub — returns null sentinel) ────────────────────
// Aes.Create() returns a new AesCryptoServiceProvider instance.
// Proper implementation requires managed object allocation.
CHAOS_IL2CPP_INTPTR ChaosAesCreate() noexcept
{
    return 0;
}

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core
