namespace chaos::il2cpp::runtime_core {
namespace {

CHAOS_IL2CPP_INTPTR MarshalAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR size) {
    return runtime_state == nullptr || size < 0 ? 0 : AllocateMarshalBlock(runtime_state, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::HGlobal);
}

CHAOS_IL2CPP_INTPTR MarshalAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INT32 size) {
    return runtime_state == nullptr || size < 0 ? 0 : AllocateMarshalBlock(runtime_state, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::CoTaskMem);
}

CHAOS_IL2CPP_INTPTR MarshalReAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INTPTR size) {
    return runtime_state == nullptr || size < 0 ? 0 : ReallocateMarshalBlock(runtime_state, memory, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::HGlobal);
}

CHAOS_IL2CPP_INTPTR MarshalReAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INT32 size) {
    return runtime_state == nullptr || size < 0 ? 0 : ReallocateMarshalBlock(runtime_state, memory, static_cast<CHAOS_IL2CPP_SIZE>(size), MarshalAllocationKind::CoTaskMem);
}

bool MarshalFreeHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory) {
    return runtime_state == nullptr ? false : FreeMarshalBlock(runtime_state, memory, MarshalAllocationKind::HGlobal, false);
}

bool MarshalFreeCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory) {
    return runtime_state == nullptr ? false : FreeMarshalBlock(runtime_state, memory, MarshalAllocationKind::CoTaskMem, false);
}

bool MarshalZeroFreeCoTaskMemUtf8(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory) {
    return runtime_state == nullptr ? false : FreeMarshalBlock(runtime_state, memory, MarshalAllocationKind::CoTaskMem, true);
}

void* MarshalPtrToStringUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR utf8_buffer,
    CHAOS_IL2CPP_INT32 length,
    bool has_explicit_length) {
    if (!IsAttached(runtime_state, thread_state) || utf8_buffer == 0) return nullptr;

    const char* utf8_bytes = reinterpret_cast<const char*>(GetMarshalAllocationStorage(utf8_buffer));
    CHAOS_IL2CPP_SIZE byte_count = 0u;
    if (has_explicit_length) {
        if (length < 0) return nullptr;
        byte_count = static_cast<CHAOS_IL2CPP_SIZE>(length);
    } else {
        byte_count = CHAOS_IL2CPP_STRLEN(utf8_bytes);
    }

    return StringNewUtf8(runtime_state, thread_state, utf8_bytes, static_cast<CHAOS_IL2CPP_UINTPTR>(byte_count));
}

CHAOS_IL2CPP_INTPTR MarshalStringToCoTaskMemUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string) {
    if (!IsAttached(runtime_state, thread_state) || managed_string == nullptr) return 0;

    auto* string_header = static_cast<StringObjectHeader*>(managed_string);
    const auto byte_count = static_cast<CHAOS_IL2CPP_SIZE>(string_header->byte_count);
    auto memory = AllocateMarshalBlock(runtime_state, byte_count + 1u, MarshalAllocationKind::CoTaskMem);
    if (memory == 0) return 0;

    auto* target = reinterpret_cast<char*>(GetMarshalAllocationStorage(memory));
    const char* source = reinterpret_cast<const char*>(string_header + 1);
    if (byte_count != 0u) {
        CHAOS_IL2CPP_MEMCPY(target, source, byte_count);
    }
    target[byte_count] = '\0';
    return memory;
}

CHAOS_IL2CPP_UINT8 MarshalReadByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_UINT8>(address, offset);
}

CHAOS_IL2CPP_INT16 MarshalReadInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INT16>(address, offset);
}

CHAOS_IL2CPP_INT32 MarshalReadInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INT32>(address, offset);
}

CHAOS_IL2CPP_INT64 MarshalReadInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INT64>(address, offset);
}

CHAOS_IL2CPP_INTPTR MarshalReadIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset) {
    return MarshalReadValue<CHAOS_IL2CPP_INTPTR>(address, offset);
}

void MarshalWriteByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_UINT8 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT16 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT64 value) {
    MarshalWriteValue(address, offset, value);
}

void MarshalWriteIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INTPTR value) {
    MarshalWriteValue(address, offset, value);
}

CHAOS_IL2CPP_INT32 TaskKernelNewId() {
    return g_next_task_id.fetch_add(1, CHAOS_IL2CPP_MEMORY_ORDER_ACQ_REL);
}

}  // anonymous namespace
}  // namespace chaos::il2cpp::runtime_core