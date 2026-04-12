#include <gc.h>
#include <cstdio>
#include <cassert>

int main() {
    GC_INIT();

    void* p = GC_MALLOC(16);
    assert(p != nullptr && "GC_MALLOC returned null");

    // Basic allocation loop
    for (int i = 0; i < 1000; ++i) {
        void* q = GC_MALLOC(64);
        assert(q != nullptr);
    }

    GC_gcollect();

    printf("GC init OK\n");
    return 0;
}
