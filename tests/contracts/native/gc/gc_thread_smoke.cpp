#include "runtime_abi.h"
#include <gc.h>
#include <cassert>
#include <cstdio>
#include <thread>
#include <vector>
#include <atomic>

static std::atomic<int> s_alloc_count{0};

static void worker_thread(const RuntimeAbiV0* abi, RuntimeState* rs) {
    ThreadState* ts = nullptr;
    RuntimeStatus status = abi->thread_attach(rs, &ts);
    assert(status == CHAOS_RUNTIME_STATUS_OK);

    // Allocate 500 objects from this thread
    for (int i = 0; i < 500; ++i) {
        void* p = GC_MALLOC(32);
        assert(p != nullptr);
        s_alloc_count.fetch_add(1, std::memory_order_relaxed);
    }

    abi->thread_detach(rs, ts);
}

int main() {
    const RuntimeAbiV0* abi = chaos_runtime_get_abi_v0();
    assert(abi != nullptr);

    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(RuntimeInitParams);
    init_params.host_name_utf8 = "gc_thread_smoke";

    RuntimeState* rs = nullptr;
    assert(abi->runtime_init(&init_params, nullptr, &rs) == CHAOS_RUNTIME_STATUS_OK);

    // Main thread attach
    ThreadState* main_ts = nullptr;
    assert(abi->thread_attach(rs, &main_ts) == CHAOS_RUNTIME_STATUS_OK);

    // Spawn 2 worker threads
    std::vector<std::thread> workers;
    workers.emplace_back(worker_thread, abi, rs);
    workers.emplace_back(worker_thread, abi, rs);

    for (auto& t : workers) {
        t.join();
    }

    assert(s_alloc_count.load() == 1000 && "Worker threads should have allocated 1000 objects total");

    // Trigger a full collection — must not crash with registered threads
    GC_gcollect();

    abi->thread_detach(rs, main_ts);
    abi->runtime_shutdown(rs);

    printf("gc_thread_smoke OK\n");
    return 0;
}
