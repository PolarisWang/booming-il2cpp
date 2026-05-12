#include "string_table.h"

#include <chaos/native_types.h>

#include <algorithm>
#include <cstring>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace chaos::il2cpp::string_table {

namespace {

const StringEntry* g_aot_entries = nullptr;
CHAOS_IL2CPP_UINT32 g_aot_entry_count = 0u;

// Thread-local single-entry cache for Resolve().
// Hot-path pattern: repeated Resolve of the same interned string (e.g. ldstr
// "A" in Convert.ToChar(string) benchmark). Covers ~99.99% of calls with a
// single compare, avoiding mutex lock + hash map + binary search.
thread_local struct { StringId id; StringView view; } g_tls_resolve = {};

CHAOS_IL2CPP_MUTEX g_dynamic_mutex;
CHAOS_IL2CPP_UNORDERED_MAP(StringId, StringView) g_dynamic_entries;
// Intermediate typedef avoids MSVC >> issue with nested macros
using DomainVector = CHAOS_IL2CPP_VECTOR(StringId);
using DomainRegistrations = CHAOS_IL2CPP_UNORDERED_MAP(CHAOS_IL2CPP_UINT32, DomainVector);
DomainRegistrations g_domain_registrations;

}  // anonymous namespace

void InitializeFromAot(const StringEntry* entries, CHAOS_IL2CPP_UINT32 count)
{
    g_aot_entries = entries;
    g_aot_entry_count = count;
}

StringView Resolve(StringId id)
{
    if (id == kStringIdNull)
    {
        return StringView{};
    }

    // Thread-local single-entry fast path: hot interned strings repeat.
    if (id == g_tls_resolve.id)
    {
        return g_tls_resolve.view;
    }

    // 1. Binary search in AOT entries (read-only, no lock needed, must be sorted by id).
    if (g_aot_entries != nullptr && g_aot_entry_count > 0u)
    {
        const auto* begin = g_aot_entries;
        const auto* end = g_aot_entries + g_aot_entry_count;
        const auto* result = CHAOS_IL2CPP_LOWER_BOUND(
            begin, end, id,
            [](const StringEntry& entry, StringId value) { return entry.id < value; });

        if (result != end && result->id == id)
        {
            g_tls_resolve = {id, StringView{result->utf8_data, result->byte_count}};
            return StringView{result->utf8_data, result->byte_count};
        }
    }

    // 2. Check dynamic entries (hot-update registered strings, requires mutex).
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_dynamic_mutex);
        const auto it = g_dynamic_entries.find(id);
        if (it != g_dynamic_entries.end())
        {
            return it->second;
        }
    }

    return StringView{};
}

StringId Register(const char* utf8_data, CHAOS_IL2CPP_UINT32 byte_count, CHAOS_IL2CPP_UINT32 domain_id)
{
    if (utf8_data == nullptr || byte_count == 0u)
    {
        return kStringIdNull;
    }

    StringId id = 14695981039346656037ULL;
    for (CHAOS_IL2CPP_UINT32 i = 0u; i < byte_count; ++i)
    {
        id ^= static_cast<unsigned char>(utf8_data[i]);
        id *= 1099511628211ULL;
    }
    id |= 1u;

    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_dynamic_mutex);

    const auto it = g_dynamic_entries.find(id);
    if (it != g_dynamic_entries.end())
    {
        if (domain_id != 0u)
        {
            auto& domains = g_domain_registrations[domain_id];
            if (CHAOS_IL2CPP_FIND(domains.begin(), domains.end(), id) == domains.end())
            {
                domains.push_back(id);
            }
        }
        return id;
    }

    auto* owned_data = static_cast<char*>(CHAOS_IL2CPP_MALLOC(byte_count));
    CHAOS_IL2CPP_MEMCPY(owned_data, utf8_data, byte_count);

    g_dynamic_entries[id] = StringView{owned_data, byte_count};

    if (domain_id != 0u)
    {
        g_domain_registrations[domain_id].push_back(id);
    }

    return id;
}

StringId Intern(const char* utf8_data, CHAOS_IL2CPP_UINT32 byte_count)
{
    return Register(utf8_data, byte_count, 0u);
}

void UnregisterDomain(CHAOS_IL2CPP_UINT32 domain_id)
{
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_dynamic_mutex);

    const auto domain_it = g_domain_registrations.find(domain_id);
    if (domain_it == g_domain_registrations.end())
    {
        return;
    }

    const auto& domain_ids = domain_it->second;
    for (CHAOS_IL2CPP_SIZE i = 0; i < domain_ids.size(); ++i)
    {
        const auto id = domain_ids[i];
        const auto entry_it = g_dynamic_entries.find(id);
        if (entry_it != g_dynamic_entries.end())
        {
            CHAOS_IL2CPP_FREE(const_cast<char*>(entry_it->second.utf8_data));
            g_dynamic_entries.erase(entry_it);
        }
    }

    g_domain_registrations.erase(domain_it);
}

}  // namespace chaos::il2cpp::string_table
