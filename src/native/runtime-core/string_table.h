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

void InitializeFromAot(const StringEntry* entries, CHAOS_IL2CPP_UINT32 count);

StringView Resolve(StringId id);

StringId Register(const char* utf8_data, CHAOS_IL2CPP_UINT32 byte_count, CHAOS_IL2CPP_UINT32 domain_id = 0u);

StringId Intern(const char* utf8_data, CHAOS_IL2CPP_UINT32 byte_count);

void UnregisterDomain(CHAOS_IL2CPP_UINT32 domain_id);

}  // namespace chaos::il2cpp::string_table

#endif  // CHAOS_IL2CPP_STRING_TABLE_H_
