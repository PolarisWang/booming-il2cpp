#ifndef CHAOS_IL2CPP_STRING_TABLE_H_
#define CHAOS_IL2CPP_STRING_TABLE_H_

#include <cstddef>
#include <cstdint>
#include <chaos/native_types.h>

namespace chaos::il2cpp::string_table {

using StringId = CHAOS_IL2CPP_UINT64;

constexpr StringId kStringIdNull = 0u;

struct StringView {
    const char* utf8_data = nullptr;
    CHAOS_IL2CPP_UINT32 byte_count = 0u;
};

struct StringEntry {
    StringId id;
    const char* utf8_data;
    CHAOS_IL2CPP_UINT32 byte_count;
};

// TLS resolve cache — exposed for fast-path inlining.
// ResolveFast() checks this inline; Resolve() updates it on cache miss.
struct ResolveCacheEntry {
    StringId id;
    StringView view;
};

extern thread_local ResolveCacheEntry g_tls_resolve_cache;

/// Global direct-mapped string resolve cache (4-entry, no TLS overhead).
///
/// On x64, aligned 64-bit loads/stores are atomic.  Cache update order:
/// data → length → id.  A concurrent reader may see a stale id (cache miss
/// → Resolve fallback) or a matching id (all data valid).  Worst case on
/// concurrent update: one call misses the cache and takes the Resolve() path.
struct FastCacheEntry {
    const char* utf8_data;
    CHAOS_IL2CPP_UINT32 byte_count;
    CHAOS_IL2CPP_UINT64 id;
};

inline FastCacheEntry g_fast_cache[4] = {};

/// Read-only AOT string table (sorted by id).  Set once during startup,
/// never modified after — safe for lock-free direct access from any thread.
extern const StringEntry* g_aot_table;
extern CHAOS_IL2CPP_UINT32 g_aot_table_count;

void InitializeFromAot(const StringEntry* entries, CHAOS_IL2CPP_UINT32 count);

StringView Resolve(StringId id);

/// Fast path: check TLS cache inline, fall back to full Resolve on miss.
inline StringView ResolveFast(StringId id) noexcept
{
    if (id == g_tls_resolve_cache.id)
        return g_tls_resolve_cache.view;
    return Resolve(id);
}

/// Lock-free global direct-mapped cache (no TLS).
/// Indexed by (id & 3).  On hit: 1 global load + 1 compare.
/// On miss: calls Resolve() and updates the cache entry.
inline StringView ResolveWithGlobalCache(StringId id) noexcept
{
    auto& slot = g_fast_cache[id & 3];
    if (slot.id == id)
        return StringView{slot.utf8_data, slot.byte_count};

    auto view = Resolve(id);
    if (view.byte_count > 0)
    {
        // Update cache: write data fields before id so a matched id
        // guarantees valid data.  On x86, stores are ordered; the
        // compiler barrier prevents reordering.
        slot.utf8_data = view.utf8_data;
        slot.byte_count = view.byte_count;
        slot.id = id;  // written last — makes the entry visible
    }
    return view;
}

StringId Register(const char* utf8_data, CHAOS_IL2CPP_UINT32 byte_count, CHAOS_IL2CPP_UINT32 domain_id = 0u);

StringId Intern(const char* utf8_data, CHAOS_IL2CPP_UINT32 byte_count);

/// Lookup-only: returns the interned StringId if the UTF-8 bytes are already
/// present in the string table (AOT or dynamic), otherwise kStringIdNull.
/// Never inserts — satisfies XmlNameTable::Get(query-only) semantics.
/// Thread-safe (dynamic map is mutex-protected; AOT table is read-only).
StringId Find(const char* utf8_data, CHAOS_IL2CPP_UINT32 byte_count);

void UnregisterDomain(CHAOS_IL2CPP_UINT32 domain_id);

}  // namespace chaos::il2cpp::string_table

#endif  // CHAOS_IL2CPP_STRING_TABLE_H_
