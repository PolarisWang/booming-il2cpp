// ── T4 SEH Handler Unit Tests ──────────────────────────────────────────────
//
// P3-A of the EH industrialization roadmap.  Validates the T4 code registry,
// demotion queue, lookup cache, SEH clause table parsing, and finally unwind
// state machine from t4_seh_handler.cpp.
//
// Strategy:
//   The source file is included directly so static functions
//   (BuildSehExceptionDispatch, FindSehHandlerForOffset, etc.) are
//   accessible.  To avoid duplicate symbols, this test does NOT link against
//   chaos_codegen.lib.  All platform-specific paths (VEH, personality
//   routine, ChaosT4RaiseException) are compiled but never invoked.
//
// Global state (g_t4_code_entries, g_t4_code_count, g_pending_free,
// g_t4_lookup_generation, g_t4_unwind) is reset in SetUp() / TearDown().

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <tuple>
#include <vector>

// ── Include the source under test for access to static functions ──────────
// This brings in the entire translation unit: registry, demotion queue,
// VEH handler, personality routine, etc.  The VEH / personality code is
// compiled but never called — tests focus on the logic-level functions.

// clang-format off
#include "t4_seh_handler.cpp"
// clang-format on

// Stub destructor: required because the test includes t4_seh_handler.cpp directly
// without linking chaos_codegen.lib (avoids duplicate symbols from the direct
// #include).  ~JitMethod() is normally defined in code_generator.cpp.
// In tests, all pointer fields are nullptr (default-initialized), so the stub
// is safe — no real resources are freed.
namespace chaos::il2cpp::jit {
JitMethod::~JitMethod() noexcept {}
}

namespace chaos::il2cpp::jit {
namespace {

// ══════════════════════════════════════════════════════════════════════════════
// Helpers
// ══════════════════════════════════════════════════════════════════════════════

/// Build a byte buffer containing fake code followed by an SEH clause table.
///
/// Layout (appended after code_size bytes of 0xCC INT3 padding):
///   uint32_t  count
///   for each clause:
///     uint32_t  flags            (0=catch, 1=filter, 2=finally, 4=fault)
///     uint32_t  try_start_offset
///     uint32_t  try_end_offset
///     uint32_t  handler_start_offset
///     uint32_t  class_token      (0=catch-all, non-zero=typed catch)
///
/// Returns a JitMethod whose `code` and `seh_table_offset` reference
/// the returned buffer.  The caller must keep the buffer alive for the
/// lifetime of any test using the returned JitMethod.
struct FakeCodeWithSeh {
    std::vector<uint8_t> buffer;
    JitMethod         nm;

    /// Each clause is (flags, try_start, try_end, handler_start, class_token).
    explicit FakeCodeWithSeh(
        const std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t,
                                       uint32_t>>& clauses,
        uint32_t code_size_hint = 64) {
        // Fake code bytes (INT3 padding).
        buffer.resize(code_size_hint, 0xCC);

        // SEH table starts right after the code.
        const uint32_t seh_offset = static_cast<uint32_t>(buffer.size());

        // Write clause count.
        const uint32_t count = static_cast<uint32_t>(clauses.size());
        AppendBytes(reinterpret_cast<const uint8_t*>(&count), sizeof(count));

        // Write each clause.
        for (const auto& [flags, ts, te, hs, ct] : clauses) {
            const uint32_t fields[5] = {flags, ts, te, hs, ct};
            AppendBytes(reinterpret_cast<const uint8_t*>(fields),
                        sizeof(fields));
        }

        nm.code             = buffer.data();
        nm.code_size        = static_cast<uint32_t>(buffer.size());
        nm.seh_table_offset = seh_offset;
    }

private:
    void AppendBytes(const uint8_t* src, size_t len) {
        buffer.insert(buffer.end(), src, src + len);
    }
};

/// RAII helper to reset the thread-local unwind state on scope exit.
/// Useful when a test modifies g_t4_unwind and wants a clean slate afterward.
struct UnwindStateGuard {
    UnwindStateGuard() {
        saved = g_t4_unwind;
    }
    ~UnwindStateGuard() {
        g_t4_unwind = saved;
    }
    T4UnwindState saved;
};

// ══════════════════════════════════════════════════════════════════════════════
// Test Fixture
// ══════════════════════════════════════════════════════════════════════════════

class T4SehHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // ── Clear code registry ──────────────────────────────────────────
        g_t4_code_count = 0;
        for (uint32_t i = 0; i < kMaxT4CodeEntries; ++i) {
            g_t4_code_entries[i] = T4CodeEntry{};
        }
        // ── Clear pending-free table ─────────────────────────────────────
        g_pending_free_count = 0;
        for (uint32_t i = 0; i < kMaxPendingFreeRegions; ++i) {
            g_pending_free[i] = PendingFreeRegion{};
        }
        // ── Reset lookup cache (generation bump forces cold miss) ────────
        InvalidateLookupCache();
        g_t4_lookup_cache = {};
        // ── Reset thread-local exception/unwind TLS ──────────────────────
        g_t4_throw_ret_addr   = nullptr;
        g_t4_frame_rsp        = nullptr;
        g_t4_exception_obj    = nullptr;
        ResetUnwindState();
        // ── Release spinlock just in case a prior test crashed mid-lock ──
        InterlockedExchange(&g_t4_code_lock, 0);
    }

    void TearDown() override {
        // Same cleanup as SetUp, ensuring no state leaks between tests.
        g_t4_code_count = 0;
        for (uint32_t i = 0; i < kMaxT4CodeEntries; ++i) {
            g_t4_code_entries[i] = T4CodeEntry{};
        }
        g_pending_free_count = 0;
        for (uint32_t i = 0; i < kMaxPendingFreeRegions; ++i) {
            g_pending_free[i] = PendingFreeRegion{};
        }
        InvalidateLookupCache();
        g_t4_lookup_cache = {};
        g_t4_throw_ret_addr = nullptr;
        g_t4_frame_rsp      = nullptr;
        g_t4_exception_obj  = nullptr;
        ResetUnwindState();
        InterlockedExchange(&g_t4_code_lock, 0);
    }

    /// Register a minimal T4 code entry and return the registered code
    /// pointer (for use with FindT4CodeByAddress) and JitMethod pointer.
    std::pair<void*, const JitMethod*>
    RegisterFakeEntry(uint32_t code_size = 64) {
        auto* buf = new std::vector<uint8_t>(code_size, 0xCC);
        auto* nm  = new JitMethod();
        nm->code      = buf->data();
        nm->code_size = code_size;
        // Store on heap so pointers remain valid for the test duration.
        // Leaks are acceptable in short-lived unit tests.
        static std::vector<std::unique_ptr<std::vector<uint8_t>>> s_bufs;
        static std::vector<std::unique_ptr<JitMethod>> s_nms;
        s_bufs.emplace_back(buf);
        s_nms.emplace_back(nm);

        RegisterT4Code(nm->code, nm->code_size, nm, /*token=*/0);
        return {buf->data(), nm};
    }
};

// ══════════════════════════════════════════════════════════════════════════════
// FindT4CodeByAddress
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest, FindT4CodeByAddress_ReturnsNullptrWhenRegistryEmpty) {
    uint8_t fake_addr;
    EXPECT_EQ(FindT4CodeByAddress(&fake_addr), nullptr);
}

TEST_F(T4SehHandlerTest,
       FindT4CodeByAddress_ReturnsNativeMethodWhenAddressInRange) {
    auto [code, nm] = RegisterFakeEntry(64);
    const auto* c = static_cast<const uint8_t*>(code);
    // Address at the start of the registered range.
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
    // Address in the middle of the range.
    EXPECT_EQ(FindT4CodeByAddress(c + 32), nm);
    // Address at the last byte of the range.
    EXPECT_EQ(FindT4CodeByAddress(c + 63), nm);
}

TEST_F(T4SehHandlerTest,
       FindT4CodeByAddress_ReturnsNullptrWhenAddressOutOfRange) {
    auto [code, nm] = RegisterFakeEntry(64);
    (void)nm;
    const auto* c = static_cast<const uint8_t*>(code);
    // Just before the range.
    EXPECT_EQ(FindT4CodeByAddress(c - 1), nullptr);
    // Just after the range (code_start + code_size).
    EXPECT_EQ(FindT4CodeByAddress(c + 64), nullptr);
    // Far outside the range.
    EXPECT_EQ(FindT4CodeByAddress(reinterpret_cast<void*>(0xDEADBEEF)),
              nullptr);
}

TEST_F(T4SehHandlerTest,
       FindT4CodeByAddress_CacheWorksForRepeatedLookupsInSamePage) {
    auto [code, nm] = RegisterFakeEntry(4096);  // 4 KiB = 1 page
    const auto* c = static_cast<const uint8_t*>(code);
    // First lookup populates the TLS cache (cold miss).
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
    // Second lookup within the same page should hit the cache.
    // The page is based on address >> 12, so any address within the same
    // 4 KiB page (offset < 4096) hits the cache.
    EXPECT_EQ(FindT4CodeByAddress(c + 2000), nm);
    // Third lookup at a different offset within the same page.
    EXPECT_EQ(FindT4CodeByAddress(c + 3999), nm);
}

TEST_F(T4SehHandlerTest,
       FindT4CodeByAddress_CacheInvalidatedWhenGenerationChanges) {
    auto [code, nm] = RegisterFakeEntry(64);
    // Populate cache.
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
    // Verify cache is populated.
    EXPECT_NE(g_t4_lookup_cache.nm, nullptr);
    EXPECT_EQ(g_t4_lookup_cache.generation, g_t4_lookup_generation);

    // Bump generation — simulates Unregister/Demote on another thread.
    InvalidateLookupCache();

    // Generation mismatch forces slow-path linear scan, which still succeeds
    // because the entry is still in the registry.
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
    // Cache is repopulated with new generation.
    EXPECT_EQ(g_t4_lookup_cache.generation, g_t4_lookup_generation);
}

// ══════════════════════════════════════════════════════════════════════════════
// RegisterT4Code / UnregisterT4Code
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest, RegisterT4Code_SetsUpValidEntry) {
    uint8_t fake_code[128] = {};
    JitMethod nm;
    nm.code      = fake_code;
    nm.code_size = sizeof(fake_code);

    RegisterT4Code(nm.code, nm.code_size, &nm, /*token=*/42);

    // Verify the entry is findable.
    EXPECT_EQ(FindT4CodeByAddress(fake_code), &nm);
    EXPECT_EQ(FindT4CodeByAddress(fake_code + 64), &nm);
}

TEST_F(T4SehHandlerTest, RegisterT4Code_RejectsNullParameters) {
    uint8_t fake_code[64] = {};
    JitMethod nm;
    nm.code      = fake_code;
    nm.code_size = 64;

    // nullptr code_start.
    RegisterT4Code(nullptr, 64, &nm);
    EXPECT_EQ(g_t4_code_count, 0u);

    // zero code_size.
    RegisterT4Code(fake_code, 0, &nm);
    EXPECT_EQ(g_t4_code_count, 0u);

    // nullptr JitMethod.
    RegisterT4Code(fake_code, 64, nullptr);
    EXPECT_EQ(g_t4_code_count, 0u);
}

TEST_F(T4SehHandlerTest,
       UnregisterT4Code_ClearsEntryAndEnqueuesDemotedCode) {
    uint8_t fake_code[64] = {};
    JitMethod nm;
    nm.code      = fake_code;
    nm.code_size = sizeof(fake_code);

    RegisterT4Code(nm.code, nm.code_size, &nm);
    ASSERT_EQ(FindT4CodeByAddress(fake_code), &nm);

    UnregisterT4Code(fake_code);

    // Entry should no longer be findable.
    EXPECT_EQ(FindT4CodeByAddress(fake_code), nullptr);

    // Verify the code was enqueued in the pending-free table.
    bool found = false;
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; ++i) {
        if (g_pending_free[i].active &&
            g_pending_free[i].code_start == fake_code) {
            found = true;
            EXPECT_EQ(g_pending_free[i].code_size, sizeof(fake_code));
            break;
        }
    }
    EXPECT_TRUE(found) << "Demoted code should be in pending-free table";
}

TEST_F(T4SehHandlerTest, UnregisterT4Code_NullCodeStartReturnsSafely) {
    // Should not crash or modify state.
    uint32_t count_before = g_t4_code_count;
    UnregisterT4Code(nullptr);
    EXPECT_EQ(g_t4_code_count, count_before);
}

TEST_F(T4SehHandlerTest, UnregisterT4Code_UnknownAddressReturnsSafely) {
    auto [code, nm] = RegisterFakeEntry(64);
    (void)nm;
    uint32_t count_before = g_t4_code_count;

    uint8_t unknown[16] = {};
    UnregisterT4Code(unknown);

    // Registry should be unchanged.
    EXPECT_EQ(g_t4_code_count, count_before);
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
}

TEST_F(T4SehHandlerTest, DoubleRegisterSameAddressDoesNotCrash) {
    uint8_t fake_code[64] = {};
    JitMethod nm1, nm2;
    nm1.code = nm2.code = fake_code;
    nm1.code_size = nm2.code_size = 64;

    RegisterT4Code(fake_code, 64, &nm1);
    RegisterT4Code(fake_code, 64, &nm2);

    // Both entries exist; FindT4CodeByAddress returns first match.
    EXPECT_NE(FindT4CodeByAddress(fake_code), nullptr);
    EXPECT_EQ(g_t4_code_count, 2u);
}

// ══════════════════════════════════════════════════════════════════════════════
// EnqueueDemotedCode / ReclaimDemotedCode
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest,
       EnqueueDemotedCode_DeduplicatesSameAddressEnqueuedTwice) {
    uint8_t fake_code[64] = {};
    JitMethod nm;
    nm.code      = fake_code;
    nm.code_size = sizeof(fake_code);

    RegisterT4Code(nm.code, nm.code_size, &nm);
    UnregisterT4Code(fake_code);
    uint32_t count_after_first = g_pending_free_count;

    // Unregister again (second time through a different test path).
    // Since the entry's nm is already nullptr, UnregisterT4Code won't
    // find it in the code registry.  We test deduplication by calling
    // EnqueueDemotedCode directly.
    EnqueueDemotedCode(fake_code, sizeof(fake_code));

    // Count should not increase because the address is already tracked.
    EXPECT_EQ(g_pending_free_count, count_after_first);
}

TEST_F(T4SehHandlerTest, ReclaimDemotedCode_ClearsAllEntries) {
    // Enqueue several fake regions (addresses won't be valid for VirtualFree,
    // but ReclaimDemotedCode handles that gracefully — the entries are still
    // cleared).
    uint8_t a[16], b[16], c[16];
    EnqueueDemotedCode(a, sizeof(a));
    EnqueueDemotedCode(b, sizeof(b));
    EnqueueDemotedCode(c, sizeof(c));
    ASSERT_EQ(g_pending_free_count, 3u);

    ReclaimDemotedCode();

    // All entries should be inactive.
    EXPECT_EQ(g_pending_free_count, 0u);
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; ++i) {
        EXPECT_FALSE(g_pending_free[i].active);
        EXPECT_EQ(g_pending_free[i].code_start, nullptr);
        EXPECT_EQ(g_pending_free[i].code_size, 0u);
    }
}

TEST_F(T4SehHandlerTest, EnqueueDemotedCode_FullTableDoesNotCrash) {
    // Fill the table.
    std::vector<uint8_t> buf(kMaxPendingFreeRegions * 2);
    for (uint32_t i = 0; i < kMaxPendingFreeRegions; ++i) {
        EnqueueDemotedCode(&buf[i], 1);
    }
    EXPECT_EQ(g_pending_free_count, kMaxPendingFreeRegions);

    // The next enqueue should log a warning and drop the entry (no crash).
    uint8_t extra = 0;
    EnqueueDemotedCode(&extra, 1);
    // Count should not increase.
    EXPECT_EQ(g_pending_free_count, kMaxPendingFreeRegions);
}

TEST_F(T4SehHandlerTest, EnqueueDemotedCode_RejectsNullCodeStart) {
    EnqueueDemotedCode(nullptr, 64);
    EXPECT_EQ(g_pending_free_count, 0u);
}

TEST_F(T4SehHandlerTest, EnqueueDemotedCode_RejectsZeroSize) {
    uint8_t addr = 0;
    EnqueueDemotedCode(&addr, 0);
    EXPECT_EQ(g_pending_free_count, 0u);
}

// ══════════════════════════════════════════════════════════════════════════════
// DemoteT4ByToken
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest, DemoteT4ByToken_DemotesMatchingEntries) {
    uint8_t fake_a[64] = {};
    uint8_t fake_b[64] = {};
    JitMethod nm_a, nm_b;
    nm_a.code = fake_a; nm_a.code_size = 64;
    nm_b.code = fake_b; nm_b.code_size = 64;

    RegisterT4Code(fake_a, 64, &nm_a, /*token=*/100);
    RegisterT4Code(fake_b, 64, &nm_b, /*token=*/200);

    // Demote by token 100 — only entry A should be cleared.
    uint32_t demoted = DemoteT4ByToken(100);
    EXPECT_EQ(demoted, 1u);
    EXPECT_EQ(FindT4CodeByAddress(fake_a), nullptr);
    EXPECT_EQ(FindT4CodeByAddress(fake_b), &nm_b);
}

TEST_F(T4SehHandlerTest, DemoteT4ByToken_ZeroTokenReturnsZero) {
    auto [code, nm] = RegisterFakeEntry(64);
    (void)nm;
    uint32_t demoted = DemoteT4ByToken(0);
    EXPECT_EQ(demoted, 0u);
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
}

TEST_F(T4SehHandlerTest, DemoteT4ByToken_NoMatchReturnsZero) {
    auto [code, nm] = RegisterFakeEntry(64);
    (void)nm;
    uint32_t demoted = DemoteT4ByToken(999);
    EXPECT_EQ(demoted, 0u);
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
}

// ══════════════════════════════════════════════════════════════════════════════
// BuildSehExceptionDispatch  (static — accessible via direct include)
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_ReturnsFalseWhenSehTableOffsetIsZero) {
    JitMethod nm;
    nm.code             = reinterpret_cast<void*>(0x1000);
    nm.code_size        = 64;
    nm.seh_table_offset = 0;  // No SEH table.

    bool result = BuildSehExceptionDispatch(&nm, /*code_offset=*/10);
    EXPECT_FALSE(result);
    EXPECT_FALSE(g_t4_unwind.exception_in_flight);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_ReturnsFalseWhenNoClauseCoversOffset) {
    // Single catch clause covering offsets [20, 40).
    FakeCodeWithSeh f({{/*flags=*/0, /*try_start=*/20, /*try_end=*/40,
                          /*handler_start=*/100, /*class_token=*/0}});

    // code_offset 10 is outside the try range.
    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/10);
    EXPECT_FALSE(result);
    EXPECT_FALSE(g_t4_unwind.exception_in_flight);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_CatchAllClauseMatches) {
    // Catch-all (class_token=0) covering offsets [0, 50).
    FakeCodeWithSeh f({{/*flags=*/0, /*try_start=*/0, /*try_end=*/50,
                          /*handler_start=*/200, /*class_token=*/0}});

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/25);
    EXPECT_TRUE(result);
    EXPECT_TRUE(g_t4_unwind.has_catch);
    EXPECT_EQ(g_t4_unwind.catch_handler_offset, 200u);
    EXPECT_EQ(g_t4_unwind.unwind_count, 0u);  // No finally/fault.
    EXPECT_TRUE(g_t4_unwind.exception_in_flight);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_TypedCatchClauseMatches) {
    // Typed catch (class_token != 0) covering offsets [10, 30).
    FakeCodeWithSeh f({{/*flags=*/0, /*try_start=*/10, /*try_end=*/30,
                          /*handler_start=*/300, /*class_token=*/0x1234}});

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/15);
    EXPECT_TRUE(result);
    EXPECT_TRUE(g_t4_unwind.has_catch);
    EXPECT_EQ(g_t4_unwind.catch_handler_offset, 300u);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_InnermostCatchSelectedWithNestedClauses) {
    // Outer catch [0, 50), inner catch [10, 30).
    // At code_offset=20, the innermost catch (inner) should be selected.
    FakeCodeWithSeh f({
        {/*flags=*/0, /*try_start=*/0,  /*try_end=*/50,
         /*handler_start=*/100, /*class_token=*/0},       // outer
        {/*flags=*/0, /*try_start=*/10, /*try_end=*/30,
         /*handler_start=*/200, /*class_token=*/0x1234},  // inner
    });

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/20);
    EXPECT_TRUE(result);
    // Innermost catch (reverse search) should be selected: the inner clause.
    EXPECT_EQ(g_t4_unwind.catch_handler_offset, 200u);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_FinallyClausePopulatesUnwindList) {
    // Catch [0, 60) + finally [10, 30) nested within catch's try range.
    // At code_offset=20, both the catch and finally should be found.
    FakeCodeWithSeh f({
        {/*flags=*/0, /*try_start=*/0,  /*try_end=*/60,
         /*handler_start=*/100, /*class_token=*/0},       // catch
        {/*flags=*/2, /*try_start=*/10, /*try_end=*/30,
         /*handler_start=*/200, /*class_token=*/0},       // finally
    });

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/20);
    EXPECT_TRUE(result);
    EXPECT_TRUE(g_t4_unwind.has_catch);
    EXPECT_EQ(g_t4_unwind.catch_handler_offset, 100u);
    EXPECT_EQ(g_t4_unwind.unwind_count, 1u);
    // The finally should be in the unwind list.
    EXPECT_EQ(g_t4_unwind.unwind_list[0], 1u);  // clause index 1 = finally
    EXPECT_TRUE(g_t4_unwind.exception_in_flight);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_OnlyFinallyNoCatchAtOffset) {
    // Finally [10, 30) covering the offset, but no catch.
    // At code_offset=20, the finally should be in the unwind list but
    // has_catch should be false.
    FakeCodeWithSeh f({
        {/*flags=*/2, /*try_start=*/10, /*try_end=*/30,
         /*handler_start=*/200, /*class_token=*/0},
    });

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/20);
    EXPECT_FALSE(result);  // No catch found.
    EXPECT_FALSE(g_t4_unwind.has_catch);
    EXPECT_EQ(g_t4_unwind.unwind_count, 1u);       // Finally found.
    EXPECT_TRUE(g_t4_unwind.exception_in_flight);   // Still active (finally).
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_MultipleFinallyClausesInUnwindList) {
    // Two finally clauses nested within a catch.
    FakeCodeWithSeh f({
        {/*flags=*/0, /*try_start=*/0,  /*try_end=*/70,
         /*handler_start=*/100, /*class_token=*/0},       // catch
        {/*flags=*/2, /*try_start=*/10, /*try_end=*/25,
         /*handler_start=*/200, /*class_token=*/0},       // finally inner
        {/*flags=*/2, /*try_start=*/5,  /*try_end=*/35,
         /*handler_start=*/300, /*class_token=*/0},       // finally outer
    });

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/20);
    EXPECT_TRUE(result);
    EXPECT_EQ(g_t4_unwind.unwind_count, 2u);
    // Innermost-first order: inner finally (index 1) before outer (index 2).
    EXPECT_EQ(g_t4_unwind.unwind_list[0], 1u);
    EXPECT_EQ(g_t4_unwind.unwind_list[1], 2u);
}

// ══════════════════════════════════════════════════════════════════════════════
// SEH Clause Table Parsing (FindSehHandlerForOffset)
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest,
       FindSehHandlerForOffset_ReturnsHandlerOffsetWhenClauseMatches) {
    FakeCodeWithSeh f({{/*flags=*/0, /*try_start=*/0, /*try_end=*/50,
                          /*handler_start=*/0xABCD, /*class_token=*/0}});

    uint32_t handler = FindSehHandlerForOffset(&f.nm, /*code_offset=*/10);
    EXPECT_EQ(handler, 0xABCDu);
}

TEST_F(T4SehHandlerTest,
       FindSehHandlerForOffset_ReturnsFFFFFFFFWhenNoSehTable) {
    JitMethod nm;
    nm.code             = reinterpret_cast<void*>(0x1000);
    nm.code_size        = 64;
    nm.seh_table_offset = 0;

    uint32_t handler = FindSehHandlerForOffset(&nm, /*code_offset=*/0);
    EXPECT_EQ(handler, 0xFFFFFFFFu);
}

TEST_F(T4SehHandlerTest,
       FindSehHandlerForOffset_ReturnsFFFFFFFFWhenNoClauseMatches) {
    FakeCodeWithSeh f({{/*flags=*/0, /*try_start=*/10, /*try_end=*/20,
                          /*handler_start=*/100, /*class_token=*/0}});

    // code_offset 5 is before the try range.
    EXPECT_EQ(FindSehHandlerForOffset(&f.nm, 5), 0xFFFFFFFFu);
    // code_offset 20 is exactly at the end (not < try_end).
    EXPECT_EQ(FindSehHandlerForOffset(&f.nm, 20), 0xFFFFFFFFu);
    // code_offset 30 is after the try range.
    EXPECT_EQ(FindSehHandlerForOffset(&f.nm, 30), 0xFFFFFFFFu);
}

// ══════════════════════════════════════════════════════════════════════════════
// T4EndFinallyHelper
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest,
       T4EndFinallyHelper_ReturnsNullptrWhenNoUnwindState) {
    // Default-constructed g_t4_unwind: exception_in_flight=false,
    // pending_leave=false.
    ResetUnwindState();
    EXPECT_EQ(T4EndFinallyHelper(), nullptr);
}

TEST_F(T4SehHandlerTest,
       T4EndFinallyHelper_WithExceptionAndCatchButNoRegisteredCode) {
    // Set up unwind state as if a catch was found but no T4 code is
    // registered for FindT4CodeByAddress to locate.
    ResetUnwindState();
    g_t4_unwind.exception_in_flight   = true;
    g_t4_unwind.has_catch             = true;
    g_t4_unwind.catch_handler_offset  = 0x400;
    g_t4_unwind.unwind_count          = 0;

    // T4EndFinallyHelper will:
    //   1. Enter exception_in_flight branch
    //   2. unwind_index++ (0->1), 1 < 0 is false
    //   3. has_catch is true → FindT4CodeByAddress(_ReturnAddress())
    //   4. No T4 code registered → FindT4CodeByAddress returns nullptr
    //   5. ResetUnwindState, return nullptr
    void* result = T4EndFinallyHelper();
    EXPECT_EQ(result, nullptr);
    // State should be reset after the helper runs.
    EXPECT_FALSE(g_t4_unwind.exception_in_flight);
    EXPECT_FALSE(g_t4_unwind.has_catch);
}

// ══════════════════════════════════════════════════════════════════════════════
// DemoteT4ByCallSiteToken
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest, DemoteT4ByCallSiteToken_ZeroTokenReturnsZero) {
    auto [code, nm] = RegisterFakeEntry(64);
    (void)nm;
    uint32_t demoted = DemoteT4ByCallSiteToken(0);
    EXPECT_EQ(demoted, 0u);
    EXPECT_EQ(FindT4CodeByAddress(code), nm);
}

TEST_F(T4SehHandlerTest,
       DemoteT4ByCallSiteToken_NoMatchingCallSiteReturnsZero) {
    uint8_t fake_code[64] = {};
    JitMethod nm;
    nm.code      = fake_code;
    nm.code_size = 64;
    // Set up a call site with a different token.
    CallSiteInfo cs;
    cs.method_token = 50;
    nm.call_sites      = &cs;
    nm.call_site_count = 1;

    RegisterT4Code(fake_code, 64, &nm);
    uint32_t demoted = DemoteT4ByCallSiteToken(999);
    EXPECT_EQ(demoted, 0u);
    EXPECT_EQ(FindT4CodeByAddress(fake_code), &nm);
}

// ══════════════════════════════════════════════════════════════════════════════
// Edge Cases
// ══════════════════════════════════════════════════════════════════════════════

TEST_F(T4SehHandlerTest, RegistryFull_LogsWarningAndRejectsEntry) {
    // Fill the registry to capacity.
    uint8_t dummy = 0;
    JitMethod dummy_nm;
    dummy_nm.code      = &dummy;
    dummy_nm.code_size = 1;

    for (uint32_t i = 0; i < kMaxT4CodeEntries; ++i) {
        // Each entry needs a unique address for lookup to distinguish them.
        // We reuse the same address but RegisterT4Code appends regardless.
        RegisterT4Code(&dummy, 1, &dummy_nm, i);
    }
    ASSERT_EQ(g_t4_code_count, kMaxT4CodeEntries);

    // One more should be rejected.
    uint8_t extra = 0xFF;
    JitMethod extra_nm;
    extra_nm.code      = &extra;
    extra_nm.code_size = 1;
    RegisterT4Code(&extra, 1, &extra_nm);

    // Count should not increase.
    EXPECT_EQ(g_t4_code_count, kMaxT4CodeEntries);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_CodeOffsetAtTryStartIsIncluded) {
    // try range [10, 30) — offset 10 is >= try_start and < try_end.
    FakeCodeWithSeh f({{/*flags=*/0, /*try_start=*/10, /*try_end=*/30,
                          /*handler_start=*/500, /*class_token=*/0}});

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/10);
    EXPECT_TRUE(result);
    EXPECT_EQ(g_t4_unwind.catch_handler_offset, 500u);
}

TEST_F(T4SehHandlerTest,
       BuildSehExceptionDispatch_CodeOffsetAtTryEndIsExcluded) {
    // try range [10, 30) — offset 30 is NOT < try_end (exclusive).
    FakeCodeWithSeh f({{/*flags=*/0, /*try_start=*/10, /*try_end=*/30,
                          /*handler_start=*/500, /*class_token=*/0}});

    bool result = BuildSehExceptionDispatch(&f.nm, /*code_offset=*/30);
    EXPECT_FALSE(result);
}

TEST_F(T4SehHandlerTest, UnregisterT4Code_InvalidatesLookupCache) {
    uint8_t fake_code[64] = {};
    JitMethod nm;
    nm.code      = fake_code;
    nm.code_size = sizeof(fake_code);

    RegisterT4Code(nm.code, nm.code_size, &nm);
    // Populate cache.
    FindT4CodeByAddress(fake_code);
    ASSERT_NE(g_t4_lookup_cache.nm, nullptr);
    uint32_t gen_before = g_t4_lookup_generation;

    UnregisterT4Code(fake_code);

    // Generation should be bumped.
    EXPECT_GT(g_t4_lookup_generation, gen_before);
    // Next lookup should fail (entry cleared).
    EXPECT_EQ(FindT4CodeByAddress(fake_code), nullptr);
}

}  // namespace
}  // namespace chaos::il2cpp::jit
