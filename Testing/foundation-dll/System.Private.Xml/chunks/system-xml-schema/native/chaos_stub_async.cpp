// Auto-generated stubs for async runtime helpers.
// The real implementations are in async_stubs.cpp (part of
// chaos_runtime_core.lib) but the SDK prebuilt lib may not
// include them.  Test entry points don't await, so stubs suffice.
#include <chaos/native_types.h>
extern "C" {
CHAOS_IL2CPP_INTPTR chaos_async_yield_create(void) noexcept { return 0; }
CHAOS_IL2CPP_INTPTR chaos_async_yield_get_awaiter(CHAOS_IL2CPP_INTPTR) noexcept { return 0; }
CHAOS_IL2CPP_INT32 chaos_async_yield_get_is_completed(CHAOS_IL2CPP_INTPTR) noexcept { return 1; }
void chaos_async_yield_get_result(CHAOS_IL2CPP_INTPTR) noexcept {}
}
