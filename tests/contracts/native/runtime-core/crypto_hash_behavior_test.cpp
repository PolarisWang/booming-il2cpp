// ── Crypto behavior test — verifies the CNG hash return-value + length fix ──
//
// Regression test for the digest-return + over-read fix in the CNG hash family
// (ChaosCngHash): the function now returns the computed digest in a managed
// byte[] (instead of discarding it with `return 0;`) and hashes EXACTLY
// inArr->length bytes (instead of length * sizeof(INTPTR), which over-read past
// the buffer).  On the Windows/BCrypt path, the one-shot wrappers
// ChaosSha1Hash/ChaosSha256Hash/ChaosSha3_*Hash all route through ChaosCngHash,
// so hashing "abc" with the fixed buffer semantics is covered by test_sha256_abc
// (an over-read would produce a different digest).
//
// NOTE (coverage): the fixtures below drive the SHA1/SHA256/MD5 one-shot wrappers
// (ChaosMd5Hash uses the OpenSSL sibling on Linux).  They do NOT directly call
// ChaosCngHmac or ChaosCngGetBytes — a dedicated test is a separate TODO if those
// need their own digest-return coverage.
//
// These stubs take/return a *managed object handle* (CHAOS_IL2CPP_INTPTR). We
// fabricate a minimal ManagedArrayAccessor on the heap (32-byte header +
// contiguous element bytes) — get_managed_array is a pure cast and does NOT
// validate against the GC heap, so a native test can drive the stub directly.

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/crypto_stubs.h"

// The hash stubs are declared inside chaos::il2cpp::runtime_core (extern "C") in
// crypto_stubs.h; this test calls them unqualified.  Without this using-directive
// the unqualified ChaosSha*/ChaosMd5 calls do not resolve (pre-existing test defect).
using namespace chaos::il2cpp::runtime_core;

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST(name) do { ++g_tests_run; std::printf("  TEST: %s ... ", name); } while (0)
#define PASS() do { ++g_tests_passed; std::puts("PASS"); } while (0)
#define FAIL(msg) do { std::puts("FAIL"); std::printf("    %s\n", msg); } while (0)

// ── Helpers ────────────────────────────────────────────────────────────

// Fabricate a managed byte[] of `data` (a plain struct: 32B header + bytes).
static CHAOS_IL2CPP_INTPTR make_byte_array(const uint8_t* data, int n) {
    auto* storage = static_cast<uint8_t*>(std::calloc(1, sizeof(ManagedArrayAccessor) + static_cast<size_t>(n)));
    if (!storage) return 0;
    auto* arr = reinterpret_cast<ManagedArrayAccessor*>(storage);
    arr->element_type_shape = 2;  // value type (byte[])
    arr->length = static_cast<CHAOS_IL2CPP_INTPTR>(n);
    if (n > 0 && data) {
        std::memcpy(storage + sizeof(ManagedArrayAccessor), data, static_cast<size_t>(n));
    }
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(arr);
}

// Read a hash result (managed byte[]) into a hex string for comparison.
static std::string result_hex(CHAOS_IL2CPP_INTPTR handle) {
    const auto* result = get_managed_array(handle);
    if (!result) return "<null>";
    const auto len = static_cast<size_t>(result->length);
    const auto* bytes = reinterpret_cast<const uint8_t*>(
        reinterpret_cast<const uint8_t*>(result) + sizeof(ManagedArrayAccessor));
    static const char* hex = "0123456789abcdef";
    std::string out;
    out.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        out += hex[bytes[i] >> 4];
        out += hex[bytes[i] & 0x0F];
    }
    return out;
}

// ── Tests ──────────────────────────────────────────────────────────────

// SHA-1 of "abc" = a9993e364706816aba3e25717850c26c9cd0d89d
static void test_sha1_abc() {
    TEST("ChaosSha1Hash(\"abc\") == a999...89d");
    uint8_t abc[] = {'a', 'b', 'c'};
    auto in = make_byte_array(abc, 3);
    auto out = ChaosSha1Hash(in);
    std::string got = result_hex(out);
    if (got == "a9993e364706816aba3e25717850c26c9cd0d89d") { PASS(); }
    else { FAIL(got.c_str()); }
}

// SHA-256 of "" = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
static void test_sha256_empty() {
    TEST("ChaosSha256Hash(\"\") == e3b0...855");
    auto in = make_byte_array(nullptr, 0);
    auto out = ChaosSha256Hash(in);
    std::string got = result_hex(out);
    if (got == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855") { PASS(); }
    else { FAIL(got.c_str()); }
}

// SHA-256 of "abc" = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
static void test_sha256_abc() {
    TEST("ChaosSha256Hash(\"abc\") == ba78...15ad");
    uint8_t abc[] = {'a', 'b', 'c'};
    auto in = make_byte_array(abc, 3);
    auto out = ChaosSha256Hash(in);
    std::string got = result_hex(out);
    if (got == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") { PASS(); }
    else { FAIL(got.c_str()); }
}

// MD5 of "abc" = 900150983cd24fb0d6963f7d28e17f72
static void test_md5_abc() {
    TEST("ChaosMd5Hash(\"abc\") == 9001...7f72");
    uint8_t abc[] = {'a', 'b', 'c'};
    auto in = make_byte_array(abc, 3);
    auto out = ChaosMd5Hash(in);
    std::string got = result_hex(out);
    if (got == "900150983cd24fb0d6963f7d28e17f72") { PASS(); }
    else { FAIL(got.c_str()); }
}

// The "over-read" regression: hashing "abc" must hash EXACTLY 3 bytes.
// If the old `length * sizeof(INTPTR)` bug were present it would read 24 bytes
// and SHA-256 would be wrong (not ba78...15ad) — so test_sha256_abc already
// covers it. This is documented as the guard.

int main() {
    std::printf("=== crypto_hash_behavior_test ===\n");
    test_sha1_abc();
    test_sha256_empty();
    test_sha256_abc();
    test_md5_abc();

    std::printf("\n  Passed %d/%d\n", g_tests_passed, g_tests_run);
    if (g_tests_passed != g_tests_run) { return 1; }
    return 0;
}
