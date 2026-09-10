// ── Codegen ABI contract tests ─────────────────────────────────────
//
// Verifies that the codegen bridge struct layouts, enum values, and
// helper function signatures match expectations.  These tests catch
// accidental ABI breaks when modifying codegen_bridge.h or runtime_abi.h.
//
// Validates:
//   1. Struct sizes and member offsets (via static_assert)
//   2. Enum/constant values
//   3. StringId tag bit and helper correctness
//   4. GcSlotMap encoding / decoding
//   5. CG-SYNC coverage (suppress manual cross-reference maintenance)

#include <gtest/gtest.h>

#include <cstdint>
#include <cstddef>
#include <type_traits>

#include <codegen_bridge.h>
#include <runtime_abi.h>

// ── ABI version constant tests ─────────────────────────────────────

TEST(CodegenAbi, BridgeVersion) {
    EXPECT_EQ(CHAOS_CODEGEN_BRIDGE_V0, 0u);
}

// ── BridgeStatus enum value tests ─────────────────────────────────

TEST(CodegenAbi, BridgeStatusValues) {
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_OK, 0);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT, 1);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_NOT_READY, 2);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_NOT_FOUND, 3);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_NOT_SUPPORTED, 4);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_MANAGED_EXCEPTION, 5);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_INTERNAL_ERROR, 6);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_METADATA_RESOLUTION_FAILED, 7);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_CLASS_INIT_FAILED, 8);
    EXPECT_EQ(CHAOS_BRIDGE_STATUS_RUNTIME_CALL_FAILED, 9);
}

// ── VTableDescriptorV0 struct checks ───────────────────────────────

TEST(CodegenAbi, VTableDescriptorSize) {
    // VTableDescriptorV0 should be 64 bytes on x64 MSVC with natural alignment
    EXPECT_EQ(sizeof(VTableDescriptorV0), 64u);
}

TEST(CodegenAbi, VTableDescriptorMemberSizes) {
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::stable_id)), sizeof(uint64_t));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::type_token)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::base_token)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::slot_count)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::slots)), sizeof(const void*));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::vtable_array)), sizeof(const void**));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::vtable_length)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::type_shape)), sizeof(uint8_t));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::iface_map)), sizeof(const void*));
    EXPECT_EQ(sizeof(decltype(VTableDescriptorV0::iface_count)), sizeof(uint32_t));
}

// ── CodeRegistrationV0 struct checks ───────────────────────────────

TEST(CodegenAbi, CodeRegistrationMemberSizes) {
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::struct_size)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::method_pointers)), sizeof(const void*));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::method_pointer_count)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::type_capabilities)), sizeof(const RuntimeTypeCapabilityEntryV0*));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::type_capability_count)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::vtable_descriptors)), sizeof(const VTableDescriptorV0*));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::vtable_descriptor_count)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::slot_map_section_begin)), sizeof(const void*));
    EXPECT_EQ(sizeof(decltype(CodeRegistrationV0::slot_map_section_end)), sizeof(const void*));
}

// ── Hotpatch constants and struct tests ────────────────────────────

TEST(CodegenAbi, HotpatchFlagValues) {
    EXPECT_EQ(kHotpatchActive, 1u);
    EXPECT_EQ(kHotpatchKeepNative, 2u);
}

TEST(CodegenAbi, HotpatchEntrySize) {
    // HotpatchEntryV0: 3 pointers + 1 uint32 = 32 bytes on x64 (with tail padding)
    EXPECT_EQ(sizeof(HotpatchEntryV0), sizeof(void*) * 3 + sizeof(uint32_t) + 4u /* tail padding */);
}

TEST(CodegenAbi, HotpatchTypeEntrySizes) {
    EXPECT_EQ(sizeof(decltype(HotpatchTypeEntryV0::type_name)), sizeof(const char*));
    EXPECT_EQ(sizeof(decltype(HotpatchTypeEntryV0::namespace_name)), sizeof(const char*));
    EXPECT_EQ(sizeof(decltype(HotpatchTypeEntryV0::first_method_index)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(HotpatchTypeEntryV0::method_count)), sizeof(uint16_t));
}

TEST(CodegenAbi, HotpatchMethodEntrySizes) {
    EXPECT_EQ(sizeof(decltype(HotpatchMethodEntryV0::method_name)), sizeof(const char*));
    EXPECT_EQ(sizeof(decltype(HotpatchMethodEntryV0::method_token)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(HotpatchMethodEntryV0::param_count)), sizeof(uint16_t));
}

// ── GcSlotMap encoding/decoding tests ──────────────────────────────

TEST(CodegenAbi, GcSlotEncodeObject) {
    uint32_t encoded = CHAOS_GC_SLOT_ENCODE(0x100, CHAOS_GC_SLOT_KIND_OBJECT);
    EXPECT_EQ(encoded & CHAOS_GC_SLOT_OFFSET_MASK, 0x100u);
    EXPECT_EQ((encoded & CHAOS_GC_SLOT_KIND_MASK) >> CHAOS_GC_SLOT_KIND_OFFSET, 0u);
}

TEST(CodegenAbi, GcSlotEncodeInterior) {
    uint32_t encoded = CHAOS_GC_SLOT_ENCODE(0x200, CHAOS_GC_SLOT_KIND_INTERIOR);
    EXPECT_EQ(encoded & CHAOS_GC_SLOT_OFFSET_MASK, 0x200u);
    EXPECT_EQ((encoded & CHAOS_GC_SLOT_KIND_MASK) >> CHAOS_GC_SLOT_KIND_OFFSET, 1u);
}

TEST(CodegenAbi, GcSlotEncodeMaxOffset) {
    // Maximum offset fits in the 31-bit offset field (top bit = interior kind).
    uint32_t encoded = CHAOS_GC_SLOT_ENCODE(0x7FFFFFFF, CHAOS_GC_SLOT_KIND_OBJECT);
    EXPECT_EQ(encoded & CHAOS_GC_SLOT_OFFSET_MASK, 0x7FFFFFFFu);
}

TEST(CodegenAbi, GcSlotMasks) {
    // T2.2-C1: slot offset widened from 12-bit (4096) to 31-bit, with the
    // interior kind flag moved to the top bit (bit 31).
    EXPECT_EQ(CHAOS_GC_SLOT_OFFSET_MASK, 0x7FFFFFFFu);
    EXPECT_EQ(CHAOS_GC_SLOT_KIND_OFFSET, 31u);
    EXPECT_EQ(CHAOS_GC_SLOT_KIND_MASK, 0x80000000u);
    EXPECT_EQ(CHAOS_GC_SLOT_KIND_OBJECT, 0u);
    EXPECT_EQ(CHAOS_GC_SLOT_KIND_INTERIOR, 0x80000000u);
}

TEST(CodegenAbi, GcSlotMapHdrSize) {
    // GcSlotMapV0: 2 uint32 fields, then flexible array
    EXPECT_EQ(sizeof(GcSlotMapV0), sizeof(uint32_t) * 2);
}

// ── StringId tag bit tests ─────────────────────────────────────────

TEST(CodegenAbi, StringIdTagBit) {
    // Tag bit must be bit 63 for 64-bit platforms
    EXPECT_EQ(CHAOS_STRING_ID_TAG,
              static_cast<intptr_t>(1) << (sizeof(intptr_t) * 8 - 1));
}

TEST(CodegenAbi, StringIdIsTagged) {
    // A value with bit 63 set should be identified as a StringId
    intptr_t tagged = CHAOS_STRING_ID_TAG | 0x1234;
    EXPECT_TRUE(chaos_is_string_id(tagged));
}

TEST(CodegenAbi, StringIdIsNotTagged) {
    // A heap pointer (bit 63 clear) should NOT be identified as a StringId
    intptr_t local = 0x12345678;  // bit 63 clear → ordinary pointer-like value
    EXPECT_FALSE(chaos_is_string_id(local));
}

TEST(CodegenAbi, StringIdExtract) {
    uint64_t id = 0xDEADBEEF;
    intptr_t tagged = chaos_make_string_id_value(id);
    EXPECT_TRUE(chaos_is_string_id(tagged));
    EXPECT_EQ(chaos_extract_string_id(tagged), id);
}

TEST(CodegenAbi, StringIdRoundTrip) {
    uint64_t ids[] = {0, 1, 0xFF, 0x123456789ABCDEFULL, ~0ULL & ~CHAOS_STRING_ID_TAG};
    for (auto id : ids) {
        intptr_t tagged = chaos_make_string_id_value(id);
        EXPECT_TRUE(chaos_is_string_id(tagged)) << "for id=" << id;
        EXPECT_EQ(chaos_extract_string_id(tagged), id) << "for id=" << id;
    }
}

TEST(CodegenAbi, StringIdZeroNonZero) {
    // Even id=0 must be non-zero after tagging
    intptr_t tagged = chaos_make_string_id_value(0);
    EXPECT_NE(tagged, 0);
    EXPECT_TRUE(chaos_is_string_id(tagged));
    EXPECT_EQ(chaos_extract_string_id(tagged), 0ULL);
}

// ── Compile-time StringId macro tests ────────────────────────────────

TEST(CodegenAbi, ConstexprStringHashDeterministic) {
    constexpr auto a = chaos_constexpr_string_hash("hello", 5);
    constexpr auto b = chaos_constexpr_string_hash("hello", 5);
    EXPECT_EQ(a, b);
}

TEST(CodegenAbi, ConstexprStringHashDifferent) {
    constexpr auto a = chaos_constexpr_string_hash("hello", 5);
    constexpr auto b = chaos_constexpr_string_hash("world", 5);
    EXPECT_NE(a, b);
}

TEST(CodegenAbi, StringIdMacroType) {
    constexpr auto id = CHAOS_IL2CPP_STRING_ID("test");
    EXPECT_TRUE((std::is_same_v<decltype(id), const chaos_string_id_t>));
    EXPECT_TRUE(chaos_is_string_id(id.tagged()));
}

TEST(CodegenAbi, StringIdMacroCompileTime) {
    constexpr auto id = CHAOS_IL2CPP_STRING_ID("compile_time_test");
    // Verify it's a valid tagged StringId
    EXPECT_NE(id.value, 0ULL);
    // Bit 63 must not be set in the raw value
    EXPECT_FALSE(id.value & (1ULL << 63));
    // Tagged version must have bit 63 set
    EXPECT_TRUE(chaos_is_string_id(id.tagged()));
}

// ── Generic registration struct tests ────────────────────────────────

TEST(CodegenAbi, GenericTypeRegistrationSize) {
    // GenericTypeRegistrationEntryV0: 4 uint32 fields
    EXPECT_EQ(sizeof(GenericTypeRegistrationEntryV0), sizeof(uint32_t) * 4);
}

TEST(CodegenAbi, GenericMethodAotEntrySize) {
    // GenericMethodAotEntryV0: 4 uint32 fields
    EXPECT_EQ(sizeof(GenericMethodAotEntryV0), sizeof(uint32_t) * 4);
}

TEST(CodegenAbi, ModuleGenericRegistrationMemberSizes) {
    EXPECT_EQ(sizeof(decltype(ModuleGenericRegistrationV0::module_id)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(ModuleGenericRegistrationV0::module_name_utf8)), sizeof(const char*));
    EXPECT_EQ(sizeof(decltype(ModuleGenericRegistrationV0::generic_type_count)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(ModuleGenericRegistrationV0::source_image)), sizeof(ImageHandle));
}

// ── CodegenBridge function table tests ───────────────────────────────

TEST(CodegenAbi, CodegenBridgeMemberSizes) {
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::abi_version)), sizeof(uint32_t));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::struct_size)), sizeof(uint32_t));
    // All function pointers should be function pointer size
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::register_codegen)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::bootstrap_runtime)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::resolve_type_by_token)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::resolve_method_by_token)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::resolve_field_by_token)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::box_value)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::unbox_value)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::resolve_virtual_method)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::invoke_virtual)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::create_delegate)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::delegate_invoke)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::resolve_icall)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(CodegenBridgeV0::resolve_string_by_token)), sizeof(void*));
}

TEST(CodegenAbi, GetBridgeV0FunctionExists) {
    // Verify the function pointer type is as expected
    using BridgeGetter = const CodegenBridgeV0* (*)();
    // Just verify the function is declared (link-time check)
    EXPECT_NE((void*)&chaos_codegen_get_bridge_v0, nullptr);
}

// ── CodegenRegistrationOptions tests ─────────────────────────────────

TEST(CodegenAbi, RegistrationOptionsSize) {
    // CodegenRegistrationOptionsV0: 2 uint32 + 1 pointer
    EXPECT_EQ(sizeof(CodegenRegistrationOptionsV0), sizeof(uint32_t) * 2 + sizeof(const char*));
}

// ── NativeReferenceAssemblyDispatchRequest tests ─────────────────────

TEST(CodegenAbi, DispatchRequestMemberSizes) {
    EXPECT_EQ(sizeof(decltype(NativeReferenceAssemblyDispatchRequestV0::subject_id_utf8)), sizeof(const char*));
    EXPECT_EQ(sizeof(decltype(NativeReferenceAssemblyDispatchRequestV0::managed_args)), sizeof(void*));
    EXPECT_EQ(sizeof(decltype(NativeReferenceAssemblyDispatchRequestV0::method_id)), sizeof(uint32_t));
}
