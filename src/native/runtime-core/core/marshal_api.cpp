namespace chaos::il2cpp::runtime_core {
// NOTE: no anonymous namespace — functions are declared in engine_binding.h
// and called from bootstrap.cpp, struct_marshal.cpp, and other TUs.

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

void* MarshalPtrToStringWide(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR wide_buffer,
    CHAOS_IL2CPP_INT32 length,
    bool has_explicit_length) {
    if (!IsAttached(runtime_state, thread_state) || wide_buffer == 0) return nullptr;

    const CHAOS_IL2CPP_UINT16* wide_chars = reinterpret_cast<const CHAOS_IL2CPP_UINT16*>(wide_buffer);
    CHAOS_IL2CPP_INT32 wide_len;
    if (has_explicit_length) {
        if (length < 0) return nullptr;
        wide_len = length;
    } else {
        const CHAOS_IL2CPP_UINT16* p = wide_chars;
        while (*p != 0) { ++p; }
        wide_len = static_cast<CHAOS_IL2CPP_INT32>(p - wide_chars);
    }

    return MarshalWideToString(runtime_state, thread_state, wide_chars, wide_len);
}

CHAOS_IL2CPP_INTPTR MarshalStringToCoTaskMemWide(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string) {
    if (!IsAttached(runtime_state, thread_state) || managed_string == nullptr) return 0;

    CHAOS_IL2CPP_INT32 wide_needed = MarshalStringToWide(managed_string, nullptr, 0);
    if (wide_needed < 0) return 0;

    CHAOS_IL2CPP_SIZE alloc_size = static_cast<CHAOS_IL2CPP_SIZE>(wide_needed + 1) * sizeof(CHAOS_IL2CPP_UINT16);
    auto memory = AllocateMarshalBlock(runtime_state, alloc_size, MarshalAllocationKind::CoTaskMem);
    if (memory == 0) return 0;

    auto* target = reinterpret_cast<CHAOS_IL2CPP_UINT16*>(GetMarshalAllocationStorage(memory));
    MarshalStringToWide(managed_string, target, wide_needed);
    target[wide_needed] = 0;
    return memory;
}

CHAOS_IL2CPP_INTPTR MarshalSafeHandleGetHandle(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* safe_handle_obj) {
    if (!IsAttached(runtime_state, thread_state) || safe_handle_obj == nullptr) return 0;

    // Object header starts with a TypeInfoHot* pointer (8 bytes).
    // The instance data begins at offset = header_size where header_size is
    //   8  for PureType  (flags & 0x03 == 0x00)
    //   16 for ThinLockable (flags & 0x03 == 0x01)
    // SafeHandle/CriticalHandle has 'handle' (IntPtr) as the first instance field,
    // which immediately follows the header.
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindMask;
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindPure;
    const auto* ti = *static_cast<const TypeInfoHot* const*>(safe_handle_obj);
    const CHAOS_IL2CPP_SIZE header_size = (ti->flags & kTypeInfoHeaderKindMask) == kTypeInfoHeaderKindPure ? 8u : 16u;

    return *reinterpret_cast<CHAOS_IL2CPP_INTPTR*>(
        static_cast<uint8_t*>(safe_handle_obj) + header_size);
}

CHAOS_IL2CPP_INT32 MarshalSizeOf(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const TypeInfoHot* type_info) {
    (void)thread_state;
    if (runtime_state == nullptr || type_info == nullptr) return 0;

    // Try the struct marshalling descriptor first (covers complex value types).
    auto* desc = ResolveStructMarshallingDescriptor(type_info);
    if (desc != nullptr) {
        return static_cast<CHAOS_IL2CPP_INT32>(desc->total_size);
    }

    // For types without a descriptor, use the GC instance_size minus header.
    // instance_size includes the object header; managed size excludes it.
    auto header_size = (type_info->flags & kTypeInfoHeaderKindMask) == kTypeInfoHeaderKindPure ? 8u : 16u;
    // instance_size might not be directly available here.
    // Fallback: return 0 for types without a registered descriptor.
    return 0;
}

// ── P/Invoke SetLastError support ──────────────────────────────────

void SetLastPInvokeError(ThreadState* ts, CHAOS_IL2CPP_INT32 error) noexcept {
    if (ts != nullptr && ts->internal_state != nullptr) {
        ts->internal_state->last_pinvoke_error = error;
    }
}

CHAOS_IL2CPP_INT32 GetLastPInvokeError(ThreadState* ts) noexcept {
    if (ts != nullptr && ts->internal_state != nullptr) {
        return ts->internal_state->last_pinvoke_error;
    }
    return 0;
}

void ClearOsLastError() noexcept {
#if defined(_WIN32)
    ::SetLastError(0);
#endif
}

CHAOS_IL2CPP_INT32 GetOsLastError() noexcept {
#if defined(_WIN32)
    return static_cast<CHAOS_IL2CPP_INT32>(::GetLastError());
#else
    return 0;
#endif
}

// ICALL helper: Marshal.GetLastPInvokeError() / Marshal.GetLastWin32Error()
// Takes no managed args; retrieves ThreadState from TLS internally.
CHAOS_IL2CPP_INT32 GetLastPInvokeErrorIcall() noexcept {
    auto* ts = GetCurrentThreadState();
    return GetLastPInvokeError(ts);
}

// ── DestroyStructure non-generic ──────────────────────────────────
// Forward declarations (struct_marshal.cpp is unity-included after marshal_api.cpp
// in runtime_core.cpp, so these aren't visible yet at parse time).
// Close runtime_core and open the actual struct_marshal peer namespace.
} // close chaos::il2cpp::runtime_core
namespace chaos::il2cpp::struct_marshal {
void DestroyMarshalledStruct(
    const ::chaos::il2cpp::marshal_abi::StructMarshallingDescriptorV1* desc,
    unsigned char* native_ptr,
    RuntimeState* runtime) noexcept;
}
namespace chaos::il2cpp::runtime_core { // reopen

CHAOS_IL2CPP_INTPTR ChaosDestroyStructureByType(CHAOS_IL2CPP_INTPTR struct_ptr, CHAOS_IL2CPP_INTPTR type_obj) noexcept {
    if (struct_ptr == 0 || type_obj == 0) return 0;

    // Extract RuntimeTypeHandle from the managed Type object.
    // Managed Type layout: [object_header(8|16B)][m_handle IntPtr(8B)].
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindMask;
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindPure;
    const auto* ti = *static_cast<const TypeInfoHot* const*>(reinterpret_cast<void*>(type_obj));
    const CHAOS_IL2CPP_SIZE header_size = (ti->flags & kTypeInfoHeaderKindMask) == kTypeInfoHeaderKindPure ? 8u : 16u;
    const CHAOS_IL2CPP_INTPTR type_handle = *reinterpret_cast<const CHAOS_IL2CPP_INTPTR*>(
        static_cast<const uint8_t*>(reinterpret_cast<const void*>(type_obj)) + header_size);
    if (type_handle == 0) return 0;

    const auto* type_info = reinterpret_cast<const TypeInfoHot*>(type_handle);
    auto* desc = ResolveStructMarshallingDescriptor(type_info);
    if (desc == nullptr) return 0;

    auto* runtime = GetCurrentRuntimeState();
    if (runtime == nullptr) return 0;

    chaos::il2cpp::struct_marshal::DestroyMarshalledStruct(desc, reinterpret_cast<unsigned char*>(struct_ptr), runtime);
    return 0;
}

// ── BStr helper (P/Invoke — oleaut32.dll on Win32) ──────────────

#if defined(_WIN32)
// Resolve SysAllocString/SysFreeString/SysStringLen from oleaut32 at runtime
// to avoid link-time dependency on oleaut32.lib.
static void* ResolveOleAut32Proc(const char* name) noexcept {
    static volatile HMODULE s_oleaut32 = nullptr;
    if (s_oleaut32 == nullptr) {
        s_oleaut32 = ::LoadLibraryA("oleaut32.dll");
    }
    if (s_oleaut32 == nullptr) return nullptr;
    return reinterpret_cast<void*>(::GetProcAddress(s_oleaut32, name));
}

static CHAOS_IL2CPP_INTPTR SysAllocStringThunk(const CHAOS_IL2CPP_UINT16* str) noexcept {
    using FuncPtr = CHAOS_IL2CPP_INTPTR(__stdcall*)(const CHAOS_IL2CPP_UINT16*);
    static FuncPtr s_fn = reinterpret_cast<FuncPtr>(ResolveOleAut32Proc("SysAllocString"));
    return s_fn ? s_fn(str) : 0;
}

static CHAOS_IL2CPP_INT32 SysStringLenThunk(CHAOS_IL2CPP_INTPTR bstr) noexcept {
    using FuncPtr = CHAOS_IL2CPP_INT32(__stdcall*)(CHAOS_IL2CPP_INTPTR);
    static FuncPtr s_fn = reinterpret_cast<FuncPtr>(ResolveOleAut32Proc("SysStringLen"));
    return s_fn ? s_fn(bstr) : 0;
}

static void SysFreeStringThunk(CHAOS_IL2CPP_INTPTR bstr) noexcept {
    using FuncPtr = void(__stdcall*)(CHAOS_IL2CPP_INTPTR);
    static FuncPtr s_fn = reinterpret_cast<FuncPtr>(ResolveOleAut32Proc("SysFreeString"));
    if (s_fn) s_fn(bstr);
}
#endif

// ICALL: Marshal.StringToBSTR(string) → IntPtr
CHAOS_IL2CPP_INTPTR CHAOS_RUNTIME_ABI_CALL MarshalStringToBSTR(void* managed_string) noexcept {
    if (managed_string == nullptr) return 0;
#if defined(_WIN32)
    auto* string_header = static_cast<StringObjectHeader*>(managed_string);
    const auto byte_count = static_cast<int>(string_header->byte_count);
    // Convert managed UTF-8 string to UTF-16 for SysAllocString.
    const auto* utf8_data = reinterpret_cast<const char*>(string_header + 1);
    int wide_needed = ::MultiByteToWideChar(CP_UTF8, 0, utf8_data, byte_count, nullptr, 0);
    if (wide_needed <= 0) return 0;
    auto* wide_buf = static_cast<CHAOS_IL2CPP_UINT16*>(std::malloc(static_cast<CHAOS_IL2CPP_SIZE>(wide_needed + 1) * sizeof(CHAOS_IL2CPP_UINT16)));
    ::MultiByteToWideChar(CP_UTF8, 0, utf8_data, byte_count, reinterpret_cast<wchar_t*>(wide_buf), wide_needed);
    wide_buf[wide_needed] = 0;
    auto result = SysAllocStringThunk(wide_buf);
    std::free(wide_buf);
    return result;
#else
    (void)managed_string;
    return 0;
#endif
}

// ICALL: Marshal.PtrToStringBSTR(IntPtr) → String
void* CHAOS_RUNTIME_ABI_CALL MarshalPtrToStringBSTR(CHAOS_IL2CPP_INTPTR bstr_ptr) noexcept {
    if (bstr_ptr == 0) return nullptr;
#if defined(_WIN32)
    auto length_chars = SysStringLenThunk(bstr_ptr);
    if (length_chars <= 0) return nullptr;
    auto* wide_chars = reinterpret_cast<const CHAOS_IL2CPP_UINT16*>(bstr_ptr);
    auto* ts = GetCurrentThreadState();
    auto* rs = ts ? ts->runtime_state : nullptr;
    if (rs == nullptr) return nullptr;
    return MarshalWideToString(rs, ts, wide_chars, length_chars);
#else
    return nullptr;
#endif
}

// ICALL: Marshal.FreeBSTR(IntPtr) → void
void CHAOS_RUNTIME_ABI_CALL MarshalFreeBSTR(CHAOS_IL2CPP_INTPTR bstr_ptr) noexcept {
    if (bstr_ptr == 0) return;
#if defined(_WIN32)
    SysFreeStringThunk(bstr_ptr);
#else
    (void)bstr_ptr;
#endif
}

// ── Variant stubs (V1: no-op — game engine scenario has minimal Variant use) ──

// ICALL: Marshal.GetObjectForNativeVariant(IntPtr) → Object
void* CHAOS_RUNTIME_ABI_CALL ChaosGetObjectForNativeVariant(CHAOS_IL2CPP_INTPTR /*variant_ptr*/) noexcept {
    return nullptr;  // V1: return null
}

// ICALL: Marshal.GetNativeVariantForObject(Object, IntPtr, IntPtr) → void
void CHAOS_RUNTIME_ABI_CALL ChaosGetNativeVariantForObject(
    void* /*obj*/, CHAOS_IL2CPP_INTPTR /*variant_ptr*/, CHAOS_IL2CPP_INTPTR /*destroy_old*/) noexcept {
    // V1: no-op
}

}  // namespace chaos::il2cpp::runtime_core