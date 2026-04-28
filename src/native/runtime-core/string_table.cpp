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
uint32_t g_aot_entry_count = 0u;

CHAOS_IL2CPP_MUTEX g_dynamic_mutex;
CHAOS_IL2CPP_UNORDERED_MAP(StringId, StringView) g_dynamic_entries;
// Intermediate typedef avoids MSVC >> issue with nested macros
using DomainVector = CHAOS_IL2CPP_VECTOR(StringId);
using DomainRegistrations = CHAOS_IL2CPP_UNORDERED_MAP(uint32_t, DomainVector);
DomainRegistrations g_domain_registrations;

}  // anonymous namespace

void InitializeFromAot(const StringEntry* entries, uint32_t count)
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

    // 1. Check dynamic entries (hot-update registered strings).
    {
        CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_dynamic_mutex);
        const auto it = g_dynamic_entries.find(id);
        if (it != g_dynamic_entries.end())
        {
            return it->second;
        }
    }

    // 2. Binary search in AOT entries (must be sorted by id).
    if (g_aot_entries != nullptr && g_aot_entry_count > 0u)
    {
        const auto* begin = g_aot_entries;
        const auto* end = g_aot_entries + g_aot_entry_count;
        const auto* result = CHAOS_IL2CPP_LOWER_BOUND(
            begin, end, id,
            [](const StringEntry& entry, StringId value) { return entry.id < value; });

        if (result != end && result->id == id)
        {
            return StringView{result->utf8_data, result->byte_count};
        }
    }

    return StringView{};
}

StringId Register(const char* utf8_data, uint32_t byte_count, uint32_t domain_id)
{
    if (utf8_data == nullptr || byte_count == 0u)
    {
        return kStringIdNull;
    }

    StringId id = 14695981039346656037ULL;
    for (uint32_t i = 0u; i < byte_count; ++i)
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

    auto* owned_data = new char[byte_count];
    CHAOS_IL2CPP_MEMCPY(owned_data, utf8_data, byte_count);

    g_dynamic_entries[id] = StringView{owned_data, byte_count};

    if (domain_id != 0u)
    {
        g_domain_registrations[domain_id].push_back(id);
    }

    return id;
}

StringId Intern(const char* utf8_data, uint32_t byte_count)
{
    return Register(utf8_data, byte_count, 0u);
}

void UnregisterDomain(uint32_t domain_id)
{
    CHAOS_IL2CPP_LOCK_GUARD(CHAOS_IL2CPP_MUTEX) lock(g_dynamic_mutex);

    const auto domain_it = g_domain_registrations.find(domain_id);
    if (domain_it == g_domain_registrations.end())
    {
        return;
    }

    for (const auto id : domain_it->second)
    {
        const auto entry_it = g_dynamic_entries.find(id);
        if (entry_it != g_dynamic_entries.end())
        {
            delete[] entry_it->second.utf8_data;
            g_dynamic_entries.erase(entry_it);
        }
    }

    g_domain_registrations.erase(domain_it);
}

}  // namespace chaos::il2cpp::string_table
