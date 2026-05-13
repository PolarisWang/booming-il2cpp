#include <chaos/span.h>
#include <chaos/ptr_tag.h>

#include <cstdint>
#include <cstdlib>

namespace chaos::il2cpp::common {

namespace {

// The managed array layout used by span operations.
// NOTE: This is a simplified view — the full ObjectHeader layout lives in runtime_core.
struct ManagedArrayView {
    CHAOS_IL2CPP_INTPTR type;
    CHAOS_IL2CPP_UINTPTR length;
    CHAOS_IL2CPP_INT32 element_type_shape; // 0 = reference, 1 = value type
    CHAOS_IL2CPP_INTPTR elements[1];
};

constexpr CHAOS_IL2CPP_INT32 k_type_shape_value = 1;

} // anonymous namespace

SpanRuntimeEntry* require_span_runtime_entry(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return reinterpret_cast<SpanRuntimeEntry*>(handle);
}

MemoryRuntimeEntry* require_memory_runtime_entry(CHAOS_IL2CPP_INTPTR handle)
{
    if (handle == static_cast<CHAOS_IL2CPP_INTPTR>(0))
    {
        CHAOS_IL2CPP_ABORT();
    }
    return reinterpret_cast<MemoryRuntimeEntry*>(handle);
}

CHAOS_IL2CPP_INTPTR create_array_span_int32(CHAOS_IL2CPP_INTPTR array_value, CHAOS_IL2CPP_INT32 start, CHAOS_IL2CPP_INT32 length)
{
    auto* arr = reinterpret_cast<ManagedArrayView*>(array_value);
    if (arr == nullptr ||
        arr->element_type_shape != k_type_shape_value ||
        start < 0 || length < 0 ||
        start > static_cast<CHAOS_IL2CPP_INT32>(arr->length) ||
        length > static_cast<CHAOS_IL2CPP_INT32>(arr->length) - start)
    {
        CHAOS_IL2CPP_ABORT();
    }
    auto* entry = new SpanRuntimeEntry{};
    entry->storage_kind = SpanStorageKind::kArray;
    entry->owner_handle = array_value;
    entry->start = start;
    entry->length = length;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(entry);
}

CHAOS_IL2CPP_INTPTR create_raw_span_int32(void* data, CHAOS_IL2CPP_INT32 length, bool read_only)
{
    if (length < 0)
    {
        CHAOS_IL2CPP_ABORT();
    }
    auto* entry = new SpanRuntimeEntry{};
    entry->storage_kind = SpanStorageKind::kRawInt32;
    entry->data_handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(data);
    entry->length = length;
    entry->read_only = read_only;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(entry);
}

CHAOS_IL2CPP_INTPTR create_memory_int32(CHAOS_IL2CPP_INTPTR array_value)
{
    auto* arr = reinterpret_cast<ManagedArrayView*>(array_value);
    if (arr == nullptr || arr->element_type_shape != k_type_shape_value)
    {
        CHAOS_IL2CPP_ABORT();
    }
    return create_array_memory_int32(array_value, 0, static_cast<CHAOS_IL2CPP_INT32>(arr->length));
}

CHAOS_IL2CPP_INTPTR create_array_memory_int32(CHAOS_IL2CPP_INTPTR array_value, CHAOS_IL2CPP_INT32 start, CHAOS_IL2CPP_INT32 length)
{
    auto* arr = reinterpret_cast<ManagedArrayView*>(array_value);
    if (arr == nullptr ||
        arr->element_type_shape != k_type_shape_value ||
        start < 0 || length < 0 ||
        start > static_cast<CHAOS_IL2CPP_INT32>(arr->length) ||
        length > static_cast<CHAOS_IL2CPP_INT32>(arr->length) - start)
    {
        CHAOS_IL2CPP_ABORT();
    }
    auto* entry = new MemoryRuntimeEntry{};
    entry->array_handle = array_value;
    entry->start = start;
    entry->length = length;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(entry);
}

CHAOS_IL2CPP_INTPTR memory_int32_get_span(CHAOS_IL2CPP_INTPTR memory_handle)
{
    auto* mem = require_memory_runtime_entry(memory_handle);
    return create_array_span_int32(mem->array_handle, mem->start, mem->length);
}

CHAOS_IL2CPP_INT32 span_int32_get_length(CHAOS_IL2CPP_INTPTR span_handle)
{
    return require_span_runtime_entry(span_handle)->length;
}

CHAOS_IL2CPP_INTPTR span_int32_get_item_address(CHAOS_IL2CPP_INTPTR span_handle, CHAOS_IL2CPP_INT32 index)
{
    auto* span = require_span_runtime_entry(span_handle);
    if (index < 0 || index >= span->length)
    {
        CHAOS_IL2CPP_ABORT();
    }
    switch (span->storage_kind)
    {
        case SpanStorageKind::kArray:
        {
            auto* arr = reinterpret_cast<ManagedArrayView*>(span->owner_handle);
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(
                &arr->elements[static_cast<CHAOS_IL2CPP_SIZE>(span->start + index)]);
        }
        case SpanStorageKind::kRawInt32:
        {
            auto* item = reinterpret_cast<CHAOS_IL2CPP_INT32*>(span->data_handle) + index;
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(item) | k_raw_int32_pointer_tag;
        }
        default:
            CHAOS_IL2CPP_ABORT();
    }
}

} // namespace chaos::il2cpp::common
