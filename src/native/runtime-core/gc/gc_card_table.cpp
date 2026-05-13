#include "gc_card_table.h"

namespace chaos::il2cpp::runtime_core {

// ── Global card table ──────────────────────────────────────────
uint8_t g_card_table[kCardTableEntries] = {};

// ── Heap base (set once at startup) ────────────────────────────
uintptr_t g_heap_base = 0;

}  // namespace chaos::il2cpp::runtime_core