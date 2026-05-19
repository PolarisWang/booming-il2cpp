#include "t4_demotion.h"

namespace chaos::il2cpp::runtime_core {

namespace {

T4DemoteByTokenFn         g_demote_by_token_fn         = nullptr;
T4DemoteByCallSiteTokenFn g_demote_by_call_site_token_fn = nullptr;

}  // namespace

void RegisterT4DemotionCallbacks(
    T4DemoteByTokenFn demote_by_token,
    T4DemoteByCallSiteTokenFn demote_by_call_site_token) noexcept {
    g_demote_by_token_fn         = demote_by_token;
    g_demote_by_call_site_token_fn = demote_by_call_site_token;
}

uint32_t DemoteT4ByToken(uint32_t method_token) noexcept {
    if (g_demote_by_token_fn == nullptr) return 0;
    return g_demote_by_token_fn(method_token);
}

uint32_t DemoteT4ByCallSiteToken(uint32_t method_token) noexcept {
    if (g_demote_by_call_site_token_fn == nullptr) return 0;
    return g_demote_by_call_site_token_fn(method_token);
}

}  // namespace chaos::il2cpp::runtime_core
