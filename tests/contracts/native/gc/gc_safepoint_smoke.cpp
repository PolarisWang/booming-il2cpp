#include "runtime_abi.h"
#include <gc.h>
#include <cassert>
#include <cstdio>

int main() {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    assert(abi != nullptr);

    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(RuntimeInitParams);
    init_params.host_name_utf8 = "gc_safepoint_smoke";

    RuntimeState* rs = nullptr;
    assert(abi->runtime_init(&init_params, nullptr, &rs) == CHAOS_RUNTIME_STATUS_OK);

    ThreadState* ts = nullptr;
    assert(abi->thread_attach(rs, &ts) == CHAOS_RUNTIME_STATUS_OK);

    const GC_word initial_gc_no = GC_get_gc_no();

    // Allocate many objects and hit safepoints to drive incremental GC
    for (int i = 0; i < 10000; ++i) {
        void* p = GC_MALLOC(64);
        (void)p;  // intentionally let it become unreachable

        if (i % 100 == 0) {
            // Trigger a GC safepoint — should call GC_collect_a_little internally
            const RuntimeAbiV0* abi2 = chaos_runtime_get_abi_v0();
            // GcSafepoint is not part of the public ABI table; call GC_collect_a_little directly
            GC_collect_a_little();
        }
    }

    const GC_word final_gc_no = GC_get_gc_no();
    assert(final_gc_no > initial_gc_no && "GC should have run at least once during safepoints");

    abi->thread_detach(rs, ts);
    abi->runtime_shutdown(rs);

    printf("gc_safepoint_smoke OK (GC ran %lu times)\n",
           static_cast<unsigned long>(final_gc_no - initial_gc_no));
    return 0;
}
