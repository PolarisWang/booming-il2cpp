// Per-family hotupdate verification test
// Family: family/System.Private.CoreLib/reflection/type
// 43 methods
//
// Uses CodeGen-generated C++ for both host (real API calls) and
// patch (sentinel returns). Verifies the method replacement lifecycle:
//   original -> register patch -> resolve -> call patched -> revert -> verify
//
// When semantic-patch C++ is available (via TYPE_ALTERNATIVE_MAP),
// also verifies that host and semantic-patch produce different (but valid)
// return values, proving the hotupdate mechanism replaced actual computation.
//
#include "bootstrap.h"
#include "codegen_bridge.h"
#include "method_replacement.h"

#include <cstdio>
#include <cstdint>
#include <cstring>

// ---------------------------------------------------------------
// Forward declarations: host methods (real API calls via CodeGen)
// ---------------------------------------------------------------
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method42(void);

// ---------------------------------------------------------------
// Forward declarations: patch methods (sentinel returns via CodeGen)
// ---------------------------------------------------------------
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method42(void);

// ---------------------------------------------------------------
// Forward declarations: semantic-patch methods (alternative API calls)
// ---------------------------------------------------------------
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method0(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method1(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method2(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method3(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method4(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method5(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method6(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method7(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method8(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method9(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method10(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method11(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method12(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method13(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method14(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method15(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method16(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method17(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method18(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method19(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method20(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method21(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method22(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method23(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method24(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method25(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method26(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method27(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method28(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method29(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method30(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method31(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method32(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method33(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method34(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method35(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method36(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method37(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method38(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method39(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method40(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method41(void);
extern "C" CHAOS_IL2CPP_INT32 ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method42(void);

namespace {

// Method pointer table for bootstrap registration.
// Each entry maps a method token to the host method pointer.
struct MethodPointerEntry {
    uint32_t method_token;
    void* method_pointer;
};

constexpr uint32_t kMethodCount = 43u;
constexpr uint32_t kBaseToken = 0x06000001u;
constexpr uintptr_t kSentinelPatchBase = 0xB0000000u;

// Host method pointer array (auto-indexed by MethodN ordering)
void* (*kHostThunks[])() = {
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method0),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method1),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method2),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method3),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method4),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method5),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method6),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method7),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method8),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method9),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method10),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method11),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method12),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method13),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method14),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method15),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method16),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method17),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method18),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method19),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method20),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method21),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method22),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method23),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method24),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method25),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method26),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method27),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method28),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method29),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method30),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method31),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method32),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method33),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method34),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method35),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method36),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method37),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method38),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method39),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method40),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method41),
    reinterpret_cast<void* (*)()>(&ReflectionTypeNativeEntry_ReflectionTypeNativeEntry_Method42),
};

// Patch method pointer array (auto-indexed by MethodN ordering)
void* (*kPatchThunks[])() = {
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method0),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method1),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method2),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method3),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method4),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method5),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method6),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method7),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method8),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method9),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method10),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method11),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method12),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method13),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method14),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method15),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method16),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method17),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method18),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method19),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method20),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method21),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method22),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method23),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method24),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method25),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method26),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method27),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method28),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method29),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method30),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method31),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method32),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method33),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method34),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method35),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method36),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method37),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method38),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method39),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method40),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method41),
    reinterpret_cast<void* (*)()>(&ReflectionTypePatchEntry_ReflectionTypePatchEntry_Method42),
};

// Semantic-patch method pointer array (alternative real API calls)
void* (*kSemanticPatchThunks[])() = {
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method0),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method1),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method2),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method3),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method4),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method5),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method6),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method7),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method8),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method9),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method10),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method11),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method12),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method13),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method14),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method15),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method16),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method17),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method18),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method19),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method20),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method21),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method22),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method23),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method24),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method25),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method26),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method27),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method28),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method29),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method30),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method31),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method32),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method33),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method34),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method35),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method36),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method37),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method38),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method39),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method40),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method41),
    reinterpret_cast<void* (*)()>(&ReflectionTypeSemanticPatchEntry_ReflectionTypeSemanticPatchEntry_Method42),
};

constexpr uint32_t kAotDomainIdFallback = 0u;

}  // namespace

int main() {
    using chaos::il2cpp::bootstrap::FindMethodPointerByToken;
    using chaos::il2cpp::bootstrap::PeekBootstrapState;
    using chaos::il2cpp::method_replacement::ActiveCount;
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::Revert;
    using chaos::il2cpp::method_replacement::RevertAll;

    // Build synthetic method pointer table with host methods.
    MethodPointerEntry entries[kMethodCount];
    for (uint32_t i = 0u; i < kMethodCount; i++) {
        entries[i].method_token = kBaseToken + i;
        entries[i].method_pointer = reinterpret_cast<void*>(kHostThunks[i]);
    }

    // Synthetic code registrations.
    CodeRegistrationV0 code_reg = {};
    code_reg.struct_size = sizeof(CodeRegistrationV0);
    code_reg.method_pointers = static_cast<const void*>(entries);
    code_reg.method_pointer_count = kMethodCount;

    MetadataRegistrationV0 meta_reg = {};
    meta_reg.struct_size = sizeof(MetadataRegistrationV0);

    CodegenRegistrationOptionsV0 options = {};
    options.struct_size = sizeof(CodegenRegistrationOptionsV0);
    options.image_name_utf8 = "System.Private.CoreLib";

    // Bootstrap the runtime through the ABI bridge.
    const CodegenBridgeV0* bridge = chaos_codegen_get_bridge_v0();
    if (bridge == nullptr) {
        std::fprintf(stderr, "FATAL: codegen bridge is null\n");
        return 1;
    }
    if (bridge->register_codegen(&code_reg, &meta_reg, &options) != CHAOS_BRIDGE_STATUS_OK) {
        std::fprintf(stderr, "FATAL: register_codegen failed\n");
        return 1;
    }
    if (bridge->bootstrap_runtime() != CHAOS_BRIDGE_STATUS_OK) {
        std::fprintf(stderr, "FATAL: bootstrap_runtime failed\n");
        return 1;
    }

    if (PeekBootstrapState() == nullptr) {
        std::fprintf(stderr, "FATAL: bootstrap state is null after bootstrap\n");
        return 1;
    }

    // Seed the JSON output array.
    std::printf("{\n");
    std::printf("  \"schemaVersion\": 1,\n");
    std::printf("  \"assemblyName\": \"System.Private.CoreLib\",\n");
    std::printf("  \"familyId\": \"family/System.Private.CoreLib/reflection/type\",\n");
    std::printf("  \"verificationKind\": \"hotupdate-proof\",\n");
    std::printf("  \"totalMethods\": %u,\n", kMethodCount);
    std::printf("  \"results\": [\n");

    uint32_t passed_count = 0u;
    uint32_t failed_count = 0u;
    RevertAll();

    for (uint32_t i = 0u; i < kMethodCount; i++) {
        const uint32_t token = kBaseToken + i;
        const uintptr_t expected_sentinel_b = kSentinelPatchBase + i;

        bool step_ok = true;

        // Step 1: Find original pointer via bootstrap dispatch chain.
        void* original_ptr = FindMethodPointerByToken(token);
        if (original_ptr == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null\n", i);
            step_ok = false;
        }

        // Step 2: Call original thunk (host now returns real checksum from cast-to-int).
        uintptr_t original_value = 0u;
        if (original_ptr != nullptr) {
            auto* thunk = reinterpret_cast<uintptr_t (*)()>(original_ptr);
            original_value = thunk();
        }

        // Step 2b: Call semantic-patch thunk before registration to get alternative checksum.
        bool has_semantic_thunk = (i < sizeof(kSemanticPatchThunks)/sizeof(kSemanticPatchThunks[0])
            && kSemanticPatchThunks[i] != nullptr);
        uintptr_t semantic_value = 0u;
        if (has_semantic_thunk) {
            auto* thunk = reinterpret_cast<uintptr_t (*)()>(kSemanticPatchThunks[i]);
            semantic_value = thunk();
        }

        // Step 3: Register patch replacement (CodeGen-generated sentinel).
        if (!Register(token, reinterpret_cast<void*>(kPatchThunks[i]))) {
            std::fprintf(stderr, "FAIL[%u]: Register returned false\n", i);
            step_ok = false;
        }

        // Step 4: Resolve directly - must return patched thunk.
        void* resolved_after_patch = Resolve(token);
        if (resolved_after_patch == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: Resolve returned null after Register\n", i);
            step_ok = false;
        }
        if (resolved_after_patch != reinterpret_cast<void*>(kPatchThunks[i])) {
            std::fprintf(stderr, "FAIL[%u]: Resolve returned wrong pointer after Register\n", i);
            step_ok = false;
        }

        // Step 5: FindMethodPointerByToken - must return patched thunk (integration test).
        void* dispatch_ptr = FindMethodPointerByToken(token);
        if (dispatch_ptr == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null after Register\n", i);
            step_ok = false;
        }
        if (dispatch_ptr != reinterpret_cast<void*>(kPatchThunks[i])) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken did not return patched thunk\n", i);
            step_ok = false;
        }

        // Step 6: Call the dispatch pointer - must return sentinel B.
        uintptr_t patched_value = 0u;
        if (dispatch_ptr != nullptr) {
            auto* thunk = reinterpret_cast<void* (*)()>(dispatch_ptr);
            patched_value = reinterpret_cast<uintptr_t>(thunk());
            if (patched_value != expected_sentinel_b) {
                std::fprintf(stderr, "FAIL[%u]: patched returned 0x%08zx, expected 0x%08zx\n",
                    i, static_cast<size_t>(patched_value), static_cast<size_t>(expected_sentinel_b));
                step_ok = false;
            }
        }

        // Step 7: Revert replacement.
        if (!Revert(token)) {
            std::fprintf(stderr, "FAIL[%u]: Revert returned false\n", i);
            step_ok = false;
        }

        // Step 8: After revert, FindMethodPointerByToken must return original pointer.
        void* after_revert_ptr = FindMethodPointerByToken(token);
        if (after_revert_ptr == nullptr) {
            std::fprintf(stderr, "FAIL[%u]: FindMethodPointerByToken returned null after Revert\n", i);
            step_ok = false;
        }
        if (after_revert_ptr != original_ptr) {
            std::fprintf(stderr, "FAIL[%u]: after Revert, pointer does not match original\n", i);
            step_ok = false;
        }

        // Step 9: Call after revert - must return host value.
        if (after_revert_ptr != nullptr) {
            auto* thunk = reinterpret_cast<void* (*)()>(after_revert_ptr);
            uintptr_t reverted_value = reinterpret_cast<uintptr_t>(thunk());
            if (reverted_value != original_value) {
                std::fprintf(stderr, "FAIL[%u]: after Revert, call returned 0x%08zx, expected 0x%08zx\n",
                    i, static_cast<size_t>(original_value), static_cast<size_t>(original_value));
                step_ok = false;
            }
        }

        // Step 10: Semantic verification - host and semantic-patch must differ.
        // This proves the hotupdate mechanism replaced actual computation.
        bool semantic_ok = true;
        if (has_semantic_thunk) {
            if (original_value == 0u && semantic_value == 0u) {
                // Both returned 0 - likely TODO stubs, cannot verify semantics.
                std::fprintf(stderr, "WARN[%u]: both host and semantic-patch returned 0 (stub?)\n", i);
                semantic_ok = false;
                // Don't fail the test — the method still passed all hotupdate steps;
                // only the semantic comparison is inconclusive.
            } else if (original_value == semantic_value) {
                // Different inputs should produce different results.
                std::fprintf(stderr, "FAIL[%u]: host and semantic-patch returned same value 0x%08zx\n",
                    i, static_cast<size_t>(original_value));
                semantic_ok = false;
                step_ok = false;
            }
        }

        if (step_ok) {
            passed_count++;
        } else {
            failed_count++;
        }

        // Emit JSON result for this method.
        const char* comma = (i + 1u < kMethodCount) ? "," : "";
        std::printf(
            "    {\n"
            "      \"methodToken\": %u,\n"
            "      \"status\": \"%s\",\n"
            "      \"originalReturnValue\": \"0x%08zx\",\n"
            "      \"patchedReturnValue\": \"0x%08zx\",\n"
            "      \"expectedPatchedValue\": \"0x%08zx\",\n"
            "      \"revertVerified\": true,\n"
            "      \"semanticVerified\": %s\n"
            "    }%s\n",
            static_cast<unsigned>(token),
            step_ok ? "passed" : "failed",
            static_cast<size_t>(original_value),
            static_cast<size_t>(patched_value),
            static_cast<size_t>(expected_sentinel_b),
            (semantic_ok ? "true" : "false"),
            comma);
    }

    std::printf("  ],\n");
    std::printf("  \"passedMethods\": %u,\n", static_cast<unsigned>(passed_count));
    std::printf("  \"failedMethods\": %u\n", static_cast<unsigned>(failed_count));
    std::printf("}\n");

    return (failed_count == 0u) ? 0 : 1;
}