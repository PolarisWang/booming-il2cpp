// crypto_stubs_internal.h — Internal shared declarations for crypto stubs
#ifndef CHAOS_IL2CPP_CRYPTO_STUBS_INTERNAL_H_
#define CHAOS_IL2CPP_CRYPTO_STUBS_INTERNAL_H_

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "generated_code_compat.h"
#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/crypto_stubs.h"
#include "runtime_core.h"
#include <chaos/pal/pal_random.h>

#ifdef _WIN32
#include <Windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#endif

namespace chaos::il2cpp::runtime_core {
extern "C" {

// Managed byte[] allocation helper (shared: BCrypt AND OpenSSL paths)
CHAOS_IL2CPP_INTPTR alloc_byte_array(CHAOS_IL2CPP_SIZE length) noexcept;

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_CRYPTO_STUBS_INTERNAL_H_