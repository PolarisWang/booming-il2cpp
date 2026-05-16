namespace chaos::il2cpp::runtime_core {
namespace {

static bool ResolveObjectTypeInfo(TypeInfoHandle type_handle,
                                   const TypeInfoHot*& out_type_info,
                                   const void**& out_vtable) noexcept
{
    out_type_info = nullptr;
    out_vtable    = nullptr;

    if (type_handle == 0u) return false;

    // Path 1: Tag-encoded RuntimeInstantiatedType handle
    const auto* desc = TryDecodeReflectionQueryTypeHandle(type_handle);
    if (desc != nullptr) {
        if (desc->subject_id_utf8 != nullptr) {
            const CHAOS_IL2CPP_UINT64 stable_id =
                chaos_compute_type_stable_id(desc->subject_id_utf8);
            out_vtable = runtime_vtable::FindVTable(stable_id);
            return (out_vtable != nullptr);
        }
        return false;
    }

    // Path 2: Module-registry / HotUpdate handle
    uint32_t module_id = GetModuleId(type_handle);
    uint32_t token = GetTypeToken(type_handle);
    if (token != 0) {
        const auto* mod = LookupModule(module_id != 0u ? module_id : 0u);
        if (mod != nullptr && !mod->tombstone && mod->type_flags != nullptr) {
            uint32_t idx = TokenToIndex(token);
            if (idx < mod->type_count && mod->type_info_ptrs != nullptr) {
                out_type_info = mod->type_info_ptrs[idx];
                if (out_type_info != nullptr && out_type_info->stable_id != 0) {
                    out_vtable = runtime_vtable::FindVTable(out_type_info->stable_id);
                    return (out_vtable != nullptr);
                }
            }
        }
    }

    return false;
}

}  // anonymous namespace

void* CHAOS_RUNTIME_ABI_CALL ObjectNew(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle type) {
    CHAOS_IL2CPP_PROFILE_SCOPE("ObjectNew");
    if (!IsAttached(runtime_state, thread_state)) return nullptr;

    const TypeInfoHot* type_info = nullptr;
    const void** vtable = nullptr;
    ResolveObjectTypeInfo(type, type_info, vtable);
    if (type_info == nullptr) return nullptr;
    (void)vtable;

    const CHAOS_IL2CPP_SIZE header_size = HeaderSizeFromFlags(type_info->flags);
    auto* object = static_cast<ObjectHeaderThin*>(AllocateBytes(runtime_state->config, header_size));
    if (object == nullptr) return nullptr;

    object->type_info = type_info;
    object->sync_state = 0;

    return object;
}

void* CHAOS_RUNTIME_ABI_CALL ObjectNewDirect(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const TypeInfoHot* type_info,
    const void** vtable) noexcept
{
    (void)vtable;  // ABI compat — vtable no longer stored per-object; dispatch via type_info->vtable_array
    if (!IsAttached(runtime_state, thread_state)) return nullptr;
    if (type_info == nullptr) return nullptr;

    const CHAOS_IL2CPP_SIZE header_size = HeaderSizeFromFlags(type_info->flags);
    auto* object = static_cast<ObjectHeaderThin*>(AllocateBytes(runtime_state->config, header_size));
    if (object == nullptr) return nullptr;

    object->type_info = type_info;
    object->sync_state = 0;
    return object;
}

void* CHAOS_RUNTIME_ABI_CALL ArrayNew(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle element_type,
    CHAOS_IL2CPP_UINTPTR length) {
    CHAOS_IL2CPP_PROFILE_SCOPE("ArrayNew");
    if (!IsAttached(runtime_state, thread_state) || element_type == 0) return nullptr;

    const CHAOS_IL2CPP_SIZE allocation_size =
        sizeof(ArrayHeader) + (static_cast<CHAOS_IL2CPP_SIZE>(length) * sizeof(void*));
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, allocation_size));
    if (storage == nullptr) return nullptr;

    auto* header = reinterpret_cast<ArrayHeader*>(storage);
    header->element_type = element_type;
    header->length = length;

    void** elements = reinterpret_cast<void**>(storage + sizeof(ArrayHeader));
    if (length != 0u) {
        CHAOS_IL2CPP_MEMSET(elements, 0, static_cast<CHAOS_IL2CPP_SIZE>(length) * sizeof(void*));
    }

    return header;
}

void* CHAOS_RUNTIME_ABI_CALL StringNewUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* utf8_bytes,
    CHAOS_IL2CPP_UINTPTR byte_count) {
    CHAOS_IL2CPP_PROFILE_SCOPE("StringNewUtf8");
    if (!IsAttached(runtime_state, thread_state)) return nullptr;
    if (utf8_bytes == nullptr && byte_count != 0u) return nullptr;

    const CHAOS_IL2CPP_SIZE allocation_size = sizeof(StringObjectHeader) + static_cast<CHAOS_IL2CPP_SIZE>(byte_count) + 1u;
    unsigned char* storage = static_cast<unsigned char*>(AllocateBytesAtomic(allocation_size));
    if (storage == nullptr) return nullptr;

    auto* header = reinterpret_cast<StringObjectHeader*>(storage);
    header->type = 0;
    header->byte_count = byte_count;

    char* text = reinterpret_cast<char*>(storage + sizeof(StringObjectHeader));
    if (byte_count != 0u) {
        CHAOS_IL2CPP_MEMCPY(text, utf8_bytes, static_cast<CHAOS_IL2CPP_SIZE>(byte_count));
    }

    text[byte_count] = '\0';
    return header;
}

RuntimeStatus CHAOS_RUNTIME_ABI_CALL ClassInit(
    RuntimeState* runtime_state,
    TypeInfoHandle type) {
    if (runtime_state == nullptr) return CHAOS_RUNTIME_STATUS_INVALID_ARGUMENT;
    return type != 0 ? CHAOS_RUNTIME_STATUS_OK : CHAOS_RUNTIME_STATUS_NOT_FOUND;
}

}  // namespace chaos::il2cpp::runtime_core