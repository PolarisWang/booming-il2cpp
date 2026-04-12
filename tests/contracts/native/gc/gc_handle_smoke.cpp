#include "runtime_abi.h"
#include <gc.h>
#include <cassert>
#include <cstdio>

int main() {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    assert(abi != nullptr && "Failed to get runtime ABI");

    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(RuntimeInitParams);
    init_params.host_name_utf8 = "gc_handle_smoke";

    RuntimeState* rs = nullptr;
    RuntimeStatus status = abi->runtime_init(&init_params, nullptr, &rs);
    assert(status == CHAOS_RUNTIME_STATUS_OK);
    assert(rs != nullptr);

    ThreadState* ts = nullptr;
    status = abi->thread_attach(rs, &ts);
    assert(status == CHAOS_RUNTIME_STATUS_OK);

    // Allocate a dummy object via GC (bypassing ABI to test GC handle directly)
    void* obj = GC_MALLOC(64);
    assert(obj != nullptr);

    // Test unpinned handle
    GCHandle h1 = abi->gc_handle_new(rs, obj, false);
    assert(h1 != CHAOS_GC_HANDLE_INVALID && "GcHandleNew should return a valid handle");

    abi->gc_handle_free(rs, h1);

    // Second handle after free should have a different ID
    GCHandle h2 = abi->gc_handle_new(rs, obj, false);
    assert(h2 != CHAOS_GC_HANDLE_INVALID);
    assert(h2 != h1 && "New handle ID should be unique");
    abi->gc_handle_free(rs, h2);

    // Test pinned handle survives a GC cycle
    void* pinned_obj = GC_MALLOC(128);
    assert(pinned_obj != nullptr);
    GCHandle h3 = abi->gc_handle_new(rs, pinned_obj, true);
    assert(h3 != CHAOS_GC_HANDLE_INVALID);

    GC_gcollect();  // pinned_obj must survive

    abi->gc_handle_free(rs, h3);

    // Null args handled gracefully
    assert(abi->gc_handle_new(nullptr, obj, false) == CHAOS_GC_HANDLE_INVALID);
    assert(abi->gc_handle_new(rs, nullptr, false) == CHAOS_GC_HANDLE_INVALID);
    abi->gc_handle_free(rs, CHAOS_GC_HANDLE_INVALID);  // must not crash

    abi->thread_detach(rs, ts);
    abi->runtime_shutdown(rs);

    printf("gc_handle_smoke OK\n");
    return 0;
}
