// collection_stubs_test.cpp — Unit tests for collection_stubs.cpp (T0-2)
//
// Tests all 14 public collection API functions:
//   CollectionListToArray, CollectionListAdd
//   CollectionDictionaryGetCount, CollectionDictionaryGetItem,
//   CollectionDictionarySetItem, CollectionDictionaryTryGetValue,
//   CollectionDictionaryContainsKey, CollectionDictionaryRemove,
//   CollectionDictionaryAdd
//   CollectionHashSetGetCount, CollectionHashSetAdd,
//   CollectionHashSetContains, CollectionHashSetRemove
//
// Collection objects use an embedded native storage slot at offset 16
// (kNativeStorageSlotOffset). Mock objects are allocated with std::malloc
// and must be at least 24 bytes (16B header + 8B slot).

#include <gtest/gtest.h>
#include <chaos/native_types.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>

#include "runtime_stubs/stub_common.h"
#include "runtime_stubs/collection_stubs.h"

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Helpers: create mock collection handles
// ═══════════════════════════════════════════════════════════════════════════

constexpr CHAOS_IL2CPP_SIZE kMockObjectSize = 32;  // >= 16 + sizeof(pointer)

/// Allocate a zeroed mock object large enough for the native storage slot.
static CHAOS_IL2CPP_INTPTR make_mock_handle() noexcept {
    auto* obj = std::malloc(kMockObjectSize);
    if (obj == nullptr) return 0;
    std::memset(obj, 0, kMockObjectSize);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(obj);
}

static void free_mock_handle(CHAOS_IL2CPP_INTPTR handle) noexcept {
    std::free(reinterpret_cast<void*>(handle));
}

// ═══════════════════════════════════════════════════════════════════════════
// List<T> tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(CollectionList, AddAndToArray_Empty) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    auto arr = CollectionListToArray(h);
    ASSERT_NE(arr, 0);
    // Read as StubArrayHeader (inline layout per CollectionListToArray)
    auto* hdr = reinterpret_cast<const StubArrayHeader*>(arr);
    EXPECT_EQ(hdr->length, 0u);
    free_mock_handle(h);
}

TEST(CollectionList, AddOneAndToArray) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionListAdd(h, static_cast<CHAOS_IL2CPP_INTPTR>(42));
    auto arr = CollectionListToArray(h);
    ASSERT_NE(arr, 0);
    auto* hdr = reinterpret_cast<const StubArrayHeader*>(arr);
    ASSERT_EQ(hdr->length, 1u);
    auto* elems = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(hdr + 1);
    EXPECT_EQ(elems[0], 42);
    free_mock_handle(h);
}

TEST(CollectionList, AddMultipleAndToArray) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionListAdd(h, 10);
    CollectionListAdd(h, 20);
    CollectionListAdd(h, 30);
    auto arr = CollectionListToArray(h);
    ASSERT_NE(arr, 0);
    auto* hdr = reinterpret_cast<const StubArrayHeader*>(arr);
    ASSERT_EQ(hdr->length, 3u);
    auto* elems = reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(hdr + 1);
    EXPECT_EQ(elems[0], 10);
    EXPECT_EQ(elems[1], 20);
    EXPECT_EQ(elems[2], 30);
    free_mock_handle(h);
}

TEST(CollectionList, Add_NullHandle) {
    // Should not crash
    CollectionListAdd(0, 42);
}

TEST(CollectionList, ToArray_NullHandle) {
    EXPECT_EQ(CollectionListToArray(0), 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// Dictionary<K,V> tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(CollectionDict, GetCount_Empty) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    EXPECT_EQ(CollectionDictionaryGetCount(h), 0);
    free_mock_handle(h);
}

TEST(CollectionDict, GetCount_NullHandle) {
    EXPECT_EQ(CollectionDictionaryGetCount(0), 0);
}

TEST(CollectionDict, AddAndGetItem) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionDictionaryAdd(h, 1, 100);
    CollectionDictionaryAdd(h, 2, 200);
    EXPECT_EQ(CollectionDictionaryGetCount(h), 2);
    EXPECT_EQ(CollectionDictionaryGetItem(h, 1), 100);
    EXPECT_EQ(CollectionDictionaryGetItem(h, 2), 200);
    // Non-existent key
    EXPECT_EQ(CollectionDictionaryGetItem(h, 3), 0);
    free_mock_handle(h);
}

TEST(CollectionDict, SetItem_UpdateExisting) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionDictionaryAdd(h, 1, 100);
    CollectionDictionarySetItem(h, 1, 999);
    EXPECT_EQ(CollectionDictionaryGetItem(h, 1), 999);
    EXPECT_EQ(CollectionDictionaryGetCount(h), 1);
    free_mock_handle(h);
}

TEST(CollectionDict, SetItem_AddNew) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionDictionarySetItem(h, 1, 100);
    CollectionDictionarySetItem(h, 2, 200);
    EXPECT_EQ(CollectionDictionaryGetCount(h), 2);
    EXPECT_EQ(CollectionDictionaryGetItem(h, 1), 100);
    EXPECT_EQ(CollectionDictionaryGetItem(h, 2), 200);
    free_mock_handle(h);
}

TEST(CollectionDict, TryGetValue_Found) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionDictionaryAdd(h, 1, 100);
    CHAOS_IL2CPP_INTPTR out = 0;
    auto found = CollectionDictionaryTryGetValue(h, 1, &out);
    EXPECT_EQ(found, 1);
    EXPECT_EQ(out, 100);
    free_mock_handle(h);
}

TEST(CollectionDict, TryGetValue_NotFound) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CHAOS_IL2CPP_INTPTR out = 0xDEAD;
    auto found = CollectionDictionaryTryGetValue(h, 99, &out);
    EXPECT_EQ(found, 0);
    // out_value should not be modified
    EXPECT_EQ(out, 0xDEAD);
    free_mock_handle(h);
}

TEST(CollectionDict, TryGetValue_NullHandle) {
    CHAOS_IL2CPP_INTPTR out = 0;
    EXPECT_EQ(CollectionDictionaryTryGetValue(0, 1, &out), 0);
}

TEST(CollectionDict, TryGetValue_NullOutValue) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    EXPECT_EQ(CollectionDictionaryTryGetValue(h, 1, nullptr), 0);
    free_mock_handle(h);
}

TEST(CollectionDict, ContainsKey_Yes) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionDictionaryAdd(h, 42, 1);
    EXPECT_EQ(CollectionDictionaryContainsKey(h, 42), 1);
    free_mock_handle(h);
}

TEST(CollectionDict, ContainsKey_No) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    EXPECT_EQ(CollectionDictionaryContainsKey(h, 99), 0);
    free_mock_handle(h);
}

TEST(CollectionDict, ContainsKey_NullHandle) {
    EXPECT_EQ(CollectionDictionaryContainsKey(0, 1), 0);
}

TEST(CollectionDict, Remove_Existing) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionDictionaryAdd(h, 1, 100);
    CollectionDictionaryAdd(h, 2, 200);
    auto removed = CollectionDictionaryRemove(h, 1);
    EXPECT_EQ(removed, 1);
    EXPECT_EQ(CollectionDictionaryGetCount(h), 1);
    EXPECT_EQ(CollectionDictionaryGetItem(h, 2), 200);
    free_mock_handle(h);
}

TEST(CollectionDict, Remove_NonExistent) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    EXPECT_EQ(CollectionDictionaryRemove(h, 99), 0);
    free_mock_handle(h);
}

TEST(CollectionDict, Remove_NullHandle) {
    EXPECT_EQ(CollectionDictionaryRemove(0, 1), 0);
}

TEST(CollectionDict, Add_DuplicateKey) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionDictionaryAdd(h, 1, 100);
    CollectionDictionaryAdd(h, 1, 200);  // duplicate — appends without overwriting
    EXPECT_EQ(CollectionDictionaryGetCount(h), 2);
    // GetItem returns first match
    EXPECT_EQ(CollectionDictionaryGetItem(h, 1), 100);
    free_mock_handle(h);
}

TEST(CollectionDict, NullHandle_Safe) {
    // All operations on null handle must not crash
    CollectionDictionaryGetItem(0, 1);
    CollectionDictionarySetItem(0, 1, 2);
    CollectionDictionaryAdd(0, 1, 2);
}

// ═══════════════════════════════════════════════════════════════════════════
// HashSet<T> tests
// ═══════════════════════════════════════════════════════════════════════════

TEST(CollectionHashSet, GetCount_Empty) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    EXPECT_EQ(CollectionHashSetGetCount(h), 0);
    free_mock_handle(h);
}

TEST(CollectionHashSet, GetCount_NullHandle) {
    EXPECT_EQ(CollectionHashSetGetCount(0), 0);
}

TEST(CollectionHashSet, Add_NewValue) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    auto added = CollectionHashSetAdd(h, 42);
    EXPECT_EQ(added, 1);
    EXPECT_EQ(CollectionHashSetGetCount(h), 1);
    free_mock_handle(h);
}

TEST(CollectionHashSet, Add_Duplicate) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionHashSetAdd(h, 42);
    auto added = CollectionHashSetAdd(h, 42);
    EXPECT_EQ(added, 0);  // duplicate — not added
    EXPECT_EQ(CollectionHashSetGetCount(h), 1);
    free_mock_handle(h);
}

TEST(CollectionHashSet, Add_NullHandle) {
    EXPECT_EQ(CollectionHashSetAdd(0, 42), 0);
}

TEST(CollectionHashSet, Contains_Yes) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionHashSetAdd(h, 42);
    EXPECT_EQ(CollectionHashSetContains(h, 42), 1);
    free_mock_handle(h);
}

TEST(CollectionHashSet, Contains_No) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    EXPECT_EQ(CollectionHashSetContains(h, 99), 0);
    free_mock_handle(h);
}

TEST(CollectionHashSet, Contains_NullHandle) {
    EXPECT_EQ(CollectionHashSetContains(0, 1), 0);
}

TEST(CollectionHashSet, Remove_Existing) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionHashSetAdd(h, 10);
    CollectionHashSetAdd(h, 20);
    auto removed = CollectionHashSetRemove(h, 10);
    EXPECT_EQ(removed, 1);
    EXPECT_EQ(CollectionHashSetGetCount(h), 1);
    EXPECT_EQ(CollectionHashSetContains(h, 20), 1);
    EXPECT_EQ(CollectionHashSetContains(h, 10), 0);
    free_mock_handle(h);
}

TEST(CollectionHashSet, Remove_NonExistent) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    EXPECT_EQ(CollectionHashSetRemove(h, 99), 0);
    free_mock_handle(h);
}

TEST(CollectionHashSet, Remove_NullHandle) {
    EXPECT_EQ(CollectionHashSetRemove(0, 1), 0);
}

TEST(CollectionHashSet, AddMultiple) {
    auto h = make_mock_handle();
    ASSERT_NE(h, 0);
    CollectionHashSetAdd(h, 1);
    CollectionHashSetAdd(h, 2);
    CollectionHashSetAdd(h, 3);
    EXPECT_EQ(CollectionHashSetGetCount(h), 3);
    EXPECT_EQ(CollectionHashSetContains(h, 1), 1);
    EXPECT_EQ(CollectionHashSetContains(h, 2), 1);
    EXPECT_EQ(CollectionHashSetContains(h, 3), 1);
    EXPECT_EQ(CollectionHashSetContains(h, 4), 0);
    free_mock_handle(h);
}
