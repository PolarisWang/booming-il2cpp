// try_run.cpp — minimal test to check if GC init + NurseryAllocate work in this build config
#include <cstdio>
#include "gc_region.h"
#include "gc_young_gen.h"
#include "thread_state.h"

using namespace chaos::il2cpp::runtime_core;

int main() {
    printf("Step 1: InitYoungGeneration...\n");
    InitYoungGeneration();
    printf("Step 2: NurseryAllocate...\n");
    void* p = NurseryAllocate(64);
    printf("Step 3: got %p\n", p);
    printf("OK\n");
    return 0;
}
