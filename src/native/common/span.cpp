#include "chaos/span.h"
#include "chaos/ptr_tag.h"

#include <cstdint>
#include <cstdlib>

namespace ChaosIl2cpp::Common {

namespace {

// The managed array layout used by span operations.
// NOTE: This is a simplified view — the full ObjectHeader layout lives in runtime_core.
struct ManagedArrayView {
    std::intptr_t type;
    std::uintptr_t length;
    std::int32_t element_type_shape; // 0 = reference, 1 = value type
    std::intptr_t elements[1];
};

constexpr std::int32_t k_type_shape_value = 1;

} // anonymous namespace

SpanRuntimeEntry* require_span_runtime_entry(std::intptr_t handle)
{
    if (handle == static_cast<std::intptr_t>(0))
    {
        std::abort();
    }
    return reinterpret_cast<SpanRuntimeEntry*>(handle);
}

MemoryRuntimeEntry* require_memory_runtime_entry(std::intptr_t handle)
{
    if (handle == static_cast<std::intptr_t>(0))
    {
        std::abort();
    }
    return reinterpret_cast<MemoryRuntimeEntry*>(handle);
}

std::intptr_t create_array_span_int32(std::intptr_t array_value, std::int32_t start, std::int32_t length)
{
    auto* arr = reinterpret_cast<ManagedArrayView*>(array_value);
    if (arr == nullptr ||
        arr->element_type_shape != k_type_shape_value ||
        start < 0 || length < 0 ||
        start > static_cast<std::int32_t>(arr->length) ||
        length > static_cast<std::int32_t>(arr->length) - start)
    {
        std::abort();
    }
    auto* entry = new SpanRuntimeEntry{};
    entry->storage_kind = SpanStorageKind::kArray;
    entry->owner_handle = array_value;
    entry->start = start;
    entry->length = length;
    return reinterpret_cast<std::intptr_t>(entry);
}

std::intptr_t create_raw_span_int32(void* data, std::int32_t length, bool read_only)
{
    if (length < 0)
    {
        std::abort();
    }
    auto* entry = new SpanRuntimeEntry{};
    entry->storage_kind = SpanStorageKind::kRawInt32;
    entry->data_handle = reinterpret_cast<std::intptr_t>(data);
    entry->length = length;
    entry->read_only = read_only;
    return reinterpret_cast<std::intptr_t>(entry);
}

std::intptr_t create_memory_int32(std::intptr_t array_value)
{
    auto* arr = reinterpret_cast<ManagedArrayView*>(array_value);
    if (arr == nullptr || arr->element_type_shape != k_type_shape_value)
    {
        std::abort();
    }
    return create_array_memory_int32(array_value, 0, static_cast<std::int32_t>(arr->length));
}

std::intptr_t create_array_memory_int32(std::intptr_t array_value, std::int32_t start, std::int32_t length)
{
    auto* arr = reinterpret_cast<ManagedArrayView*>(array_value);
    if (arr == nullptr ||
        arr->element_type_shape != k_type_shape_value ||
        start < 0 || length < 0 ||
        start > static_cast<std::int32_t>(arr->length) ||
        length > static_cast<std::int32_t>(arr->length) - start)
    {
        std::abort();
    }
    auto* entry = new MemoryRuntimeEntry{};
    entry->array_handle = array_value;
    entry->start = start;
    entry->length = length;
    return reinterpret_cast<std::intptr_t>(entry);
}

std::intptr_t memory_int32_get_span(std::intptr_t memory_handle)
{
    auto* mem = require_memory_runtime_entry(memory_handle);
    return create_array_span_int32(mem->array_handle, mem->start, mem->length);
}

std::int32_t span_int32_get_length(std::intptr_t span_handle)
{
    return require_span_runtime_entry(span_handle)->length;
}

std::intptr_t span_int32_get_item_address(std::intptr_t span_handle, std::int32_t index)
{
    auto* span = require_span_runtime_entry(span_handle);
    if (index < 0 || index >= span->length)
    {
        std::abort();
    }
    switch (span->storage_kind)
    {
        case SpanStorageKind::kArray:
        {
            auto* arr = reinterpret_cast<ManagedArrayView*>(span->owner_handle);
            return reinterpret_cast<std::intptr_t>(
                &arr->elements[static_cast<std::size_t>(span->start + index)]);
        }
        case SpanStorageKind::kRawInt32:
        {
            auto* item = reinterpret_cast<std::int32_t*>(span->data_handle) + index;
            return reinterpret_cast<std::intptr_t>(item) | k_raw_int32_pointer_tag;
        }
        default:
            std::abort();
    }
}

} // namespace ChaosIl2cpp::Common
