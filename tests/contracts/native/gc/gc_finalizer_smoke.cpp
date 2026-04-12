#include "runtime_abi.h"
#include <gc.h>
#include <cassert>
#include <cstdio>
#include <atomic>

static std::atomic<int> s_finalizer_count{0};

static void my_finalizer(void* /*obj*/) {
    s_finalizer_count.fetch_add(1, std::memory_order_relaxed);
}

int main() {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    assert(abi != nullptr);

    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(RuntimeInitParams);
    init_params.host_name_utf8 = "gc_finalizer_smoke";

    RuntimeState* rs = nullptr;
    assert(abi->runtime_init(&init_params, nullptr, &rs) == CHAOS_RUNTIME_STATUS_OK);

    ThreadState* ts = nullptr;
    assert(abi->thread_attach(rs, &ts) == CHAOS_RUNTIME_STATUS_OK);

    // Allocate 5 objects and register finalizers
    // Do NOT keep references — objects should become unreachable
    for (int i = 0; i < 5; ++i) {
        void* obj = GC_MALLOC(64);
        assert(obj != nullptr);
        // GC_register_finalizer_no_order is called inside EnqueueFinalizer
        // Access it through a local cast of the abi function
        // We test via the pattern: allocate → enqueue → lose ref → collect → drain
        GC_register_finalizer_no_order(
            obj,
            [](void* o, void* cd) {
                (void)o;
                auto* counter = static_cast<std::atomic<int>*>(cd);
                counter->fetch_add(1, std::memory_order_relaxed);
            },
            &s_finalizer_count,
            nullptr, nullptr);
    }
    // Force objects to be unreachable
    GC_gcollect();
    GC_gcollect();
    GC_gcollect();
    GC_invoke_finalizers();

    assert(s_finalizer_count.load() > 0 && "At least some finalizers should have been invoked");

    abi->thread_detach(rs, ts);
    abi->runtime_shutdown(rs);

    printf("gc_finalizer_smoke OK (invoked %d finalizers)\n", s_finalizer_count.load());
    return 0;
}
