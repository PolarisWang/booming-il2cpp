// crt_stubs.cpp — Stub definitions for symbols referenced by chaos
// prebuilt libraries that are not available in the testing build.
//
// These are intentional no-ops: the stubs prevent linker errors when
// functionality is not exercised by tests.

#include <cstdint>

namespace chaos::il2cpp::diagnostics {

void EpEmitComDispatchInvoke(int, int) noexcept {}
void EpEmitComRcwCreated(uint64_t) noexcept {}
void EpEmitComRcwReleased(uint64_t) noexcept {}
void EpEmitComCcwCreated(uint64_t, uint64_t) noexcept {}
void EpEmitComCcwReleased(uint64_t) noexcept {}

}  // namespace chaos::il2cpp::diagnostics
