#ifndef CHAOS_IL2CPP_STRING_TABLE_H_
#define CHAOS_IL2CPP_STRING_TABLE_H_

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::string_table {

using StringId = uint64_t;

constexpr StringId kStringIdNull = 0u;

struct StringView {
    const char* utf8_data = nullptr;
    uint32_t byte_count = 0u;
};

struct StringEntry {
    StringId id;
    const char* utf8_data;
    uint32_t byte_count;
};

void InitializeFromAot(const StringEntry* entries, uint32_t count);

StringView Resolve(StringId id);

StringId Register(const char* utf8_data, uint32_t byte_count, uint32_t domain_id = 0u);

StringId Intern(const char* utf8_data, uint32_t byte_count);

void UnregisterDomain(uint32_t domain_id);

}  // namespace chaos::il2cpp::string_table

#endif  // CHAOS_IL2CPP_STRING_TABLE_H_
