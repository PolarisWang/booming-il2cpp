#include "jit_demotion.h"

namespace chaos::il2cpp::runtime_core {

namespace {

JitDemoteByTokenFn         g_demote_by_token_fn         = nullptr;
JitDemoteByCallSiteTokenFn g_demote_by_call_site_token_fn = nullptr;

}  // namespace

void RegisterJitDemotionCallbacks(
    JitDemoteByTokenFn demote_by_token,
    JitDemoteByCallSiteTokenFn demote_by_call_site_token) noexcept {
    g_demote_by_token_fn         = demote_by_token;
    g_demote_by_call_site_token_fn = demote_by_call_site_token;
}

uint32_t DemoteJittedMethod(uint32_t method_token) noexcept {
    if (g_demote_by_token_fn == nullptr) return 0;
    return g_demote_by_token_fn(method_token);
}

uint32_t DemoteJittedCallSite(uint32_t method_token) noexcept {
    if (g_demote_by_call_site_token_fn == nullptr) return 0;
    return g_demote_by_call_site_token_fn(method_token);
}

}  // namespace chaos::il2cpp::runtime_core
