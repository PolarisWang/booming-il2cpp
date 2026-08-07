/// common_ptr_tag_test — Unit tests for ptr_tag.h tag-bit resolution.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/ptr_tag.h>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Tag constants
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonPtrTag, TagValues) {
    EXPECT_EQ(1, k_managed_pointer_local_slot_tag);
    EXPECT_EQ(2, k_raw_int32_pointer_tag);
}

// ═══════════════════════════════════════════════════════════════════════════
// resolve_native_int_slot — untagged address passes through
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonPtrTag, ResolveUntagged) {
    CHAOS_IL2CPP_INTPTR value = 42;
    CHAOS_IL2CPP_INTPTR addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    auto* resolved = resolve_native_int_slot(addr);
    EXPECT_EQ(&value, resolved);
    EXPECT_EQ(42, *resolved);
}

// ═══════════════════════════════════════════════════════════════════════════
// resolve_native_int_slot — tagged with k_managed_pointer_local_slot_tag
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonPtrTag, ResolveTaggedSlot) {
    CHAOS_IL2CPP_INTPTR value = 99;
    CHAOS_IL2CPP_INTPTR raw_addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&value);
    CHAOS_IL2CPP_INTPTR tagged = raw_addr | k_managed_pointer_local_slot_tag;
    auto* resolved = resolve_native_int_slot(tagged);
    EXPECT_EQ(&value, resolved);
    EXPECT_EQ(99, *resolved);
}

// ═══════════════════════════════════════════════════════════════════════════
// resolve_block_copy_address — untagged
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonPtrTag, ResolveBlockCopyUntagged) {
    int data = 42;
    CHAOS_IL2CPP_INTPTR addr = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&data);
    void* resolved = resolve_block_copy_address(addr);
    EXPECT_EQ(&data, resolved);
}

// ═══════════════════════════════════════════════════════════════════════════
// resolve_block_copy_address — tagged with k_raw_int32_pointer_tag
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonPtrTag, ResolveBlockCopyTaggedInt32) {
    int data = 77;
    CHAOS_IL2CPP_INTPTR raw = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&data);
    CHAOS_IL2CPP_INTPTR tagged = raw | k_raw_int32_pointer_tag;
    void* resolved = resolve_block_copy_address(tagged);
    EXPECT_EQ(&data, resolved);
}

// ═══════════════════════════════════════════════════════════════════════════
// resolve_block_copy_address — tagged with k_managed_pointer_local_slot_tag
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonPtrTag, ResolveBlockCopyTaggedSlot) {
    int data = 55;
    CHAOS_IL2CPP_INTPTR raw = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&data);
    CHAOS_IL2CPP_INTPTR tagged = raw | k_managed_pointer_local_slot_tag;
    void* resolved = resolve_block_copy_address(tagged);
    EXPECT_EQ(&data, resolved);
}
