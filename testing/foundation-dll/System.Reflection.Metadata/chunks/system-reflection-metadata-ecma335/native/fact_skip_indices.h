// Manually maintained fact skip list for known-failing methods.
// These methods fail because the ATG test calls them with parameters
// that trigger ArgumentNullException or invalid state errors (e.g.,
// InstructionEncoder::LoadConstantI4 with default encoder, or
// MetadataTokens methods with unchecked handle values).
#pragma once
// NOLINTNEXTLINE
static inline bool IsFactMethodSkipped(int index) noexcept {
    if (index == 105) return true;  // InstructionEncoder::LoadConstantI4
    if (index == 106) return true;  // InstructionEncoder::LoadConstantI4
    if (index == 275) return true;  // MetadataTokens::Handle
    if (index == 276) return true;  // MetadataTokens::MethodDefinitionHandle
    if (index == 277) return true;  // MetadataTokens::MethodDefinitionHandle
    if (index == 278) return true;  // MetadataTokens::MethodImplementationHandle
    if (index == 279) return true;  // MetadataTokens::MethodImplementationHandle
    return false;
}
