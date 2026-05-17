namespace chaos::il2cpp::runtime_core {

RuntimeMode GetRuntimeMode() {
    return g_runtime_mode.load(CHAOS_IL2CPP_MEMORY_ORDER_ACQUIRE);
}

void SetRuntimeMode(RuntimeMode mode) {
    g_runtime_mode.store(mode, CHAOS_IL2CPP_MEMORY_ORDER_RELEASE);
}

bool IsMixedMode() {
    return GetRuntimeMode() == RuntimeMode::Mixed;
}

void* BoxValueObject(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle value_type,
    const void* value,
    CHAOS_IL2CPP_SIZE value_size) {
    CHAOS_IL2CPP_PROFILE_SCOPE("BoxValueObject");

    if (!IsAttached(runtime_state, thread_state)
        || value_type == 0
        || value == nullptr
        || value_size == 0u) {
        return nullptr;
    }

    const CHAOS_IL2CPP_SIZE allocation_size = sizeof(BoxedValueHeader) + value_size;
    unsigned char* storage = static_cast<unsigned char*>(GcAllocateAtomic(allocation_size));
    if (storage == nullptr) return nullptr;

    auto* header = reinterpret_cast<BoxedValueHeader*>(storage);
    header->type = value_type;
    header->byte_count = value_size;
    CHAOS_IL2CPP_MEMCPY(storage + sizeof(BoxedValueHeader), value, value_size);
    return header;
}

RuntimeStatus UnboxValueObject(
    RuntimeState* runtime_state,
    void* boxed_object,
    void* out_value,
    CHAOS_IL2CPP_SIZE out_value_size) {
    if (runtime_state == nullptr || boxed_object == nullptr || out_value == nullptr || out_value_size == 0u) {
        return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    }

    const auto* header = reinterpret_cast<const BoxedValueHeader*>(boxed_object);
    if (header->byte_count < out_value_size) return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;

    CHAOS_IL2CPP_MEMCPY(out_value, reinterpret_cast<const unsigned char*>(boxed_object) + sizeof(BoxedValueHeader), out_value_size);
    return CHAOS_RUNTIME_STATUS_OK;
}

bool ArrayStoreReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index,
    void* value) {
    if (array_instance == nullptr) return false;

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (index >= header->length) return false;

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    elements[index] = value;
    return true;
}

void* ArrayLoadReference(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR index) {
    if (array_instance == nullptr) return nullptr;

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (index >= header->length) return nullptr;

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    return elements[index];
}

bool ArrayCopyReferenceRange(
    void* source_array_instance,
    CHAOS_IL2CPP_UINTPTR source_index,
    void* target_array_instance,
    CHAOS_IL2CPP_UINTPTR target_index,
    CHAOS_IL2CPP_UINTPTR length) {
    if (source_array_instance == nullptr || target_array_instance == nullptr) return false;

    auto* source_header = reinterpret_cast<ArrayHeader*>(source_array_instance);
    auto* target_header = reinterpret_cast<ArrayHeader*>(target_array_instance);
    if (source_header->element_type == 0 ||
        target_header->element_type == 0 ||
        source_header->element_type != target_header->element_type) {
        return false;
    }

    if (source_index > source_header->length ||
        target_index > target_header->length ||
        length > (source_header->length - source_index) ||
        length > (target_header->length - target_index)) {
        return false;
    }

    if (length == 0u) return true;

    auto* source_elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(source_array_instance) + sizeof(ArrayHeader));
    auto* target_elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(target_array_instance) + sizeof(ArrayHeader));
    CHAOS_IL2CPP_MEMMOVE(
        target_elements + target_index,
        source_elements + source_index,
        static_cast<CHAOS_IL2CPP_SIZE>(length) * sizeof(void*));
    return true;
}

bool ArrayClearReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length) {
    if (array_instance == nullptr) return false;

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (header->element_type == 0) return false;
    if (start_index > header->length || length > (header->length - start_index)) return false;

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    for (CHAOS_IL2CPP_UINTPTR index = 0; index < length; ++index) {
        elements[start_index + index] = nullptr;
    }

    return true;
}

bool ArrayReverseReferenceRange(
    void* array_instance,
    CHAOS_IL2CPP_UINTPTR start_index,
    CHAOS_IL2CPP_UINTPTR length) {
    if (array_instance == nullptr) return false;

    auto* header = reinterpret_cast<ArrayHeader*>(array_instance);
    if (header->element_type == 0) return false;
    if (start_index > header->length || length > (header->length - start_index)) return false;

    if (length <= 1u) return true;

    auto* elements = reinterpret_cast<void**>(reinterpret_cast<unsigned char*>(array_instance) + sizeof(ArrayHeader));
    CHAOS_IL2CPP_UINTPTR left = start_index;
    CHAOS_IL2CPP_UINTPTR right = start_index + length - 1u;
    while (left < right) {
        void* temporary = elements[left];
        elements[left] = elements[right];
        elements[right] = temporary;
        ++left;
        --right;
    }

    return true;
}

}  // namespace chaos::il2cpp::runtime_core