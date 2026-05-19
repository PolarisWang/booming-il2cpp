#if defined(_WIN32)
#include <objbase.h>   // CoCreateInstance, IUnknown, HRESULT, GUID
#endif

#include <atomic>
#include <cstring>

namespace chaos::il2cpp::runtime_core {
// NOTE: no anonymous namespace — functions are declared in engine_binding.h
// and called from bootstrap.cpp, struct_marshal.cpp, and other TUs.

// TLS: HRESULT stored by ChaosThrowComExceptionForHR so the managed-side
// COMException constructor can retrieve it via ChaosGetComFailureHR().
thread_local CHAOS_IL2CPP_INT32 tls_com_failure_hr = 0;

// Forward declaration: g_next_task_id is defined in engine_lifecycle.cpp's
// anonymous namespace (same TU via unity build).
extern CHAOS_IL2CPP_ATOMIC(CHAOS_IL2CPP_INT32) g_next_task_id;

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
        const CHAOS_IL2CPP_UINT16* const end = p + CHAOS_IL2CPP_INT32_MAX / 2;
        while (p < end && *p != 0) { ++p; }
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

// ── String HGlobal marshalling ─────────────────────────────────────

CHAOS_IL2CPP_INTPTR MarshalStringToHGlobalAnsi(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string) {
    if (!IsAttached(runtime_state, thread_state) || managed_string == nullptr) return 0;

    const auto* hdr = static_cast<const StringObjectHeader*>(managed_string);
    const auto utf8_bytes = static_cast<const char*>(static_cast<const void*>(hdr + 1));
    const auto byte_count = static_cast<int>(hdr->byte_count);

#if defined(_WIN32)
    // UTF-8 → UTF-16 → ANSI (CP_ACP)
    int wide_needed = ::MultiByteToWideChar(CP_UTF8, 0, utf8_bytes, byte_count, nullptr, 0);
    if (wide_needed <= 0) return 0;
    auto* wide_buf = static_cast<wchar_t*>(_malloca(static_cast<CHAOS_IL2CPP_SIZE>(wide_needed + 1) * sizeof(wchar_t)));
    ::MultiByteToWideChar(CP_UTF8, 0, utf8_bytes, byte_count, wide_buf, wide_needed);
    wide_buf[wide_needed] = 0;

    int ansi_needed = ::WideCharToMultiByte(CP_ACP, 0, wide_buf, wide_needed, nullptr, 0, nullptr, nullptr);
    if (ansi_needed <= 0) { _freea(wide_buf); return 0; }

    auto memory = AllocateMarshalBlock(runtime_state, static_cast<CHAOS_IL2CPP_SIZE>(ansi_needed) + 1u, MarshalAllocationKind::HGlobal);
    if (memory == 0) { _freea(wide_buf); return 0; }
    auto* ansi_target = reinterpret_cast<char*>(GetMarshalAllocationStorage(memory));
    ::WideCharToMultiByte(CP_ACP, 0, wide_buf, wide_needed, ansi_target, ansi_needed, nullptr, nullptr);
    ansi_target[ansi_needed] = '\0';
    _freea(wide_buf);
    return memory;
#else
    // Non-Windows: ACP ≈ UTF-8, pass through.
    auto memory = AllocateMarshalBlock(runtime_state, static_cast<CHAOS_IL2CPP_SIZE>(byte_count) + 1u, MarshalAllocationKind::HGlobal);
    if (memory == 0) return 0;
    auto* target = reinterpret_cast<char*>(GetMarshalAllocationStorage(memory));
    if (byte_count > 0) { CHAOS_IL2CPP_MEMCPY(target, utf8_bytes, static_cast<CHAOS_IL2CPP_SIZE>(byte_count)); }
    target[byte_count] = '\0';
    return memory;
#endif
}

CHAOS_IL2CPP_INTPTR MarshalStringToHGlobalUni(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string) {
    if (!IsAttached(runtime_state, thread_state) || managed_string == nullptr) return 0;

    int wide_needed = MarshalStringToWide(managed_string, nullptr, 0);
    if (wide_needed < 0) return 0;

    CHAOS_IL2CPP_SIZE alloc_size = static_cast<CHAOS_IL2CPP_SIZE>(wide_needed + 1) * sizeof(CHAOS_IL2CPP_UINT16);
    auto memory = AllocateMarshalBlock(runtime_state, alloc_size, MarshalAllocationKind::HGlobal);
    if (memory == 0) return 0;

    auto* target = reinterpret_cast<CHAOS_IL2CPP_UINT16*>(GetMarshalAllocationStorage(memory));
    MarshalStringToWide(managed_string, target, wide_needed);
    target[wide_needed] = 0;
    return memory;
}

void* MarshalPtrToStringAnsiIcall(CHAOS_IL2CPP_INTPTR ansi_buffer, CHAOS_IL2CPP_INT32 length) noexcept {
    if (ansi_buffer == 0) return nullptr;
    auto* rs = GetCurrentRuntimeState();
    auto* ts = GetCurrentThreadState();
    if (rs == nullptr || ts == nullptr) return nullptr;
    return MarshalAnsiToString(rs, ts, reinterpret_cast<const char*>(ansi_buffer), length);
}

CHAOS_IL2CPP_INTPTR MarshalUnsafeAddrOfPinnedArrayElement(
    void* managed_array,
    CHAOS_IL2CPP_INT32 index) noexcept {
    if (managed_array == nullptr || index < 0) return 0;

    // Bounds check against array length.
    const auto* header = static_cast<const ArrayHeader*>(managed_array);
    if (static_cast<CHAOS_IL2CPP_SIZE>(index) >= header->length) return 0;

    // Determine element size via the element TypeInfoHot.
    // V1: instance_size is stored in GcLayout, not directly in TypeInfoHot.
    // For now, use a fallback of 1 (byte-level). The caller is responsible for
    // pinning and knowing the element size. Full resolution deferred.
    CHAOS_IL2CPP_SIZE elem_size = 1;

    const auto* data_start = reinterpret_cast<const uint8_t*>(managed_array) + sizeof(ArrayHeader);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(const_cast<uint8_t*>(data_start) + static_cast<CHAOS_IL2CPP_SIZE>(index) * elem_size);
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
    auto* layout = GcLayoutRegistry::Instance().Lookup(type_info->stable_id);
    if (layout != nullptr) {
        return static_cast<CHAOS_IL2CPP_INT32>(layout->instance_size - header_size);
    }
    return 0;
}

CHAOS_IL2CPP_INT32 MarshalOffsetOf(
    const StructMarshallingDescriptorV1* desc,
    const char* const* field_names,
    CHAOS_IL2CPP_INTPTR field_name_obj) noexcept {
    if (desc == nullptr || field_names == nullptr || field_name_obj == 0) return -1;

    // Extract UTF-8 data and byte count from the managed String object.
    const auto* hdr = static_cast<const StringObjectHeader*>(
        reinterpret_cast<const void*>(field_name_obj));
    const char* name_utf8 = reinterpret_cast<const char*>(hdr + 1);
    const auto name_len = static_cast<int>(hdr->byte_count);

    // Linear scan: compare each field name with the target.
    // strncmp bounded by the managed string's length prevents over-read;
    // the null-byte check on the static name ensures exact match.
    for (uint16_t i = 0; i < desc->field_count; ++i) {
        if (std::strncmp(name_utf8, field_names[i], name_len) == 0
            && field_names[i][name_len] == '\0') {
            return static_cast<CHAOS_IL2CPP_INT32>(desc->fields[i].offset);
        }
    }
    return -1;
}

CHAOS_IL2CPP_INT32 MarshalOffsetOfByType(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle type_handle,
    CHAOS_IL2CPP_INTPTR field_name_obj) noexcept {
    (void)runtime_state;
    (void)thread_state;

    auto* type_info = TryResolveTypeInfo(type_handle);
    if (type_info == nullptr) return -1;

    auto* desc = ResolveStructMarshallingDescriptor(type_info);
    if (desc == nullptr) return -1;

    auto* field_names = ResolveStaticMarshallingFieldNames(type_info->stable_id);
    if (field_names == nullptr) return -1;

    return MarshalOffsetOf(desc, field_names, field_name_obj);
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
    static std::atomic<HMODULE> s_oleaut32{nullptr};
    HMODULE mod = s_oleaut32.load(std::memory_order_acquire);
    if (mod == nullptr) {
        mod = ::LoadLibraryA("oleaut32.dll");
        s_oleaut32.store(mod, std::memory_order_release);
    }
    if (mod == nullptr) return nullptr;
    return reinterpret_cast<void*>(::GetProcAddress(mod, name));
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

// ── COM apartment management ────────────────────────────────────────

CHAOS_IL2CPP_INTPTR CoCreateComInstance(
    const CHAOS_IL2CPP_UINT8* clsid_bytes,
    const CHAOS_IL2CPP_UINT8* iid_bytes) noexcept {
#if defined(_WIN32)
    if (clsid_bytes == nullptr || iid_bytes == nullptr) return 0;
    GUID clsid;
    GUID iid;
    std::memcpy(&clsid, clsid_bytes, sizeof(GUID));
    std::memcpy(&iid, iid_bytes, sizeof(GUID));
    IUnknown* p_unknown = nullptr;
    HRESULT hr = ::CoCreateInstance(
        clsid, nullptr, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
        iid, reinterpret_cast<void**>(&p_unknown));
    return SUCCEEDED(hr) ? reinterpret_cast<CHAOS_IL2CPP_INTPTR>(p_unknown) : 0;
#else
    (void)clsid_bytes;
    (void)iid_bytes;
    return 0;
#endif
}

// ── RCW (Runtime Callable Wrapper) ─────────────────────────────────

CHAOS_IL2CPP_INTPTR MarshalCreateRcw(CHAOS_IL2CPP_INTPTR unknown_ptr) noexcept {
    if (unknown_ptr == 0) return 0;
    auto* rcw = com_rcw::FindOrCreateRcw(reinterpret_cast<void*>(unknown_ptr));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(rcw);
}

void MarshalReleaseRcw(CHAOS_IL2CPP_INTPTR rcw_native_ptr) noexcept {
    if (rcw_native_ptr == 0) return;
    com_rcw::ReleaseRcw(static_cast<com_rcw::ComRcwNative*>(
        reinterpret_cast<void*>(rcw_native_ptr)));
}

CHAOS_IL2CPP_INTPTR MarshalGetRcwUnknown(CHAOS_IL2CPP_INTPTR rcw_native_ptr) noexcept {
    if (rcw_native_ptr == 0) return 0;
    auto* rcw = static_cast<com_rcw::ComRcwNative*>(
        reinterpret_cast<void*>(rcw_native_ptr));
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(rcw->identity_unknown);
}

CHAOS_IL2CPP_INTPTR MarshalRcwQueryInterface(
    CHAOS_IL2CPP_INTPTR rcw_native_ptr,
    const CHAOS_IL2CPP_UINT8* iid_bytes) noexcept {
    if (rcw_native_ptr == 0 || iid_bytes == nullptr) return 0;
    auto* rcw = static_cast<com_rcw::ComRcwNative*>(
        reinterpret_cast<void*>(rcw_native_ptr));
    void* result = com_rcw::QueryInterfaceCached(rcw, iid_bytes);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

bool MarshalIsRcwHandle(CHAOS_IL2CPP_INTPTR ptr) noexcept {
    return com_rcw::IsComRcwHandle(ptr);
}

// ── ComVtable dispatch helpers (test/verification) ──────────────────
// These mirror the codegen's RCW-handle-aware vtable dispatch logic.
// They are used by P/Invoke test helpers in per-family verification builds.

/// RCW-aware ComVtable dispatch: if ptr is an RCW handle, extract
/// identity_unknown; then call vtable[slot](com_ptr, a, b).
/// Returns the int32 result from the COM method.
CHAOS_IL2CPP_INT32 MarshalCallComMethod(
    CHAOS_IL2CPP_INTPTR ptr,
    CHAOS_IL2CPP_INT32 slot,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b) noexcept
{
    if (ptr == 0) return 0;
    void* com_ptr = reinterpret_cast<void*>(ptr);

    // If ptr is an RCW handle, extract the identity IUnknown*.
    if (com_rcw::IsComRcwHandle(ptr)) {
        auto* rcw = static_cast<com_rcw::ComRcwNative*>(com_ptr);
        com_ptr = rcw->identity_unknown;
        if (com_ptr == nullptr) return 0;
    }

    // COM vtable dispatch: first read the vtable pointer, then call slot.
    using ComMethod = CHAOS_IL2CPP_INT32 (*)(void*, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32);
    void** vtable = *static_cast<void***>(com_ptr);
    auto method = reinterpret_cast<ComMethod>(vtable[slot]);
    return method(com_ptr, a, b);
}

/// Direct ComVtable dispatch (no RCW check): treat ptr as raw COM pointer
/// and call vtable[slot](com_ptr, a, b).
CHAOS_IL2CPP_INT32 MarshalCallDirectComMethod(
    CHAOS_IL2CPP_INTPTR com_ptr,
    CHAOS_IL2CPP_INT32 slot,
    CHAOS_IL2CPP_INT32 a,
    CHAOS_IL2CPP_INT32 b) noexcept
{
    if (com_ptr == 0) return 0;
    void* ptr = reinterpret_cast<void*>(com_ptr);

    using ComMethod = CHAOS_IL2CPP_INT32 (*)(void*, CHAOS_IL2CPP_INT32, CHAOS_IL2CPP_INT32);
    void** vtable = *static_cast<void***>(ptr);
    auto method = reinterpret_cast<ComMethod>(vtable[slot]);
    return method(ptr, a, b);
}

void ChaosThrowComExceptionForHR(CHAOS_IL2CPP_INT32 hr) noexcept {
    // Sentinel-based COMException delivery.  The kManagedExceptionComFailure
    // sentinel propagates through generated AOT catch blocks (which re-throw
    // sentinels) and the interpreter SEH layer up to the managed exception
    // dispatch, which creates a managed COMException from the HRESULT.
    // Store the HR in TLS so the managed-side COMException constructor can
    // retrieve it via ChaosGetComFailureHR().
    CHAOS_IL2CPP_LOG_WARN_M("COM", "ChaosThrowComExceptionForHR HRESULT 0x{0:x8} — throwing COMException sentinel",
                             static_cast<unsigned int>(hr));
    tls_com_failure_hr = hr;
    throw chaos_managed_exception{kManagedExceptionComFailure};
}

CHAOS_IL2CPP_INT32 ChaosGetComFailureHR() noexcept {
    return tls_com_failure_hr;
}

// ── CCW (COM Callable Wrapper) ─────────────────────────────────────

CHAOS_IL2CPP_INTPTR MarshalCreateCcw(
    CHAOS_IL2CPP_INTPTR managed_object,
    CHAOS_IL2CPP_INTPTR runtime_state) noexcept {
    return com_ccw::CreateCcw(
        reinterpret_cast<void*>(managed_object),
        reinterpret_cast<void*>(runtime_state));
}

// ── Variant support ─────────────────────────────────────────────────

// Minimal VARIANT vt-field values (Win32 COM ABI).  When <oleauto.h> is
// available these match the system definitions; defined here to avoid a
// hard dependency on oleauto.h in the build.
#ifndef CHAOS_IL2CPP_VT_EMPTY
#define CHAOS_IL2CPP_VT_EMPTY    0
#define CHAOS_IL2CPP_VT_NULL     1
#define CHAOS_IL2CPP_VT_I2       2
#define CHAOS_IL2CPP_VT_I4       3
#define CHAOS_IL2CPP_VT_R4       4
#define CHAOS_IL2CPP_VT_R8       5
#define CHAOS_IL2CPP_VT_BOOL     11
#define CHAOS_IL2CPP_VT_BSTR     8
#define CHAOS_IL2CPP_VT_UNKNOWN  13
#define CHAOS_IL2CPP_VT_BYREF    0x4000
#endif

// Minimal VARIANT layout (16 bytes on x64, 16 bytes on x86).
struct alignas(CHAOS_IL2CPP_SIZE) VariantLayout {
    CHAOS_IL2CPP_UINT16 vt;
    CHAOS_IL2CPP_UINT16 reserved1;
    CHAOS_IL2CPP_UINT16 reserved2;
    CHAOS_IL2CPP_UINT16 reserved3;
    union {
        CHAOS_IL2CPP_INT16   iVal;
        CHAOS_IL2CPP_INT32   lVal;
        float                fltVal;
        double               dblVal;
        CHAOS_IL2CPP_INT32   boolVal;
        void*                bstrVal;
        void*                punkVal;
    } data;
};

// ICALL: Marshal.GetObjectForNativeVariant(IntPtr) → Object
void* CHAOS_RUNTIME_ABI_CALL ChaosGetObjectForNativeVariant(CHAOS_IL2CPP_INTPTR variant_ptr) noexcept {
#if defined(_WIN32)
    if (variant_ptr == 0) return nullptr;
    const auto* v = static_cast<const VariantLayout*>(
        reinterpret_cast<const void*>(variant_ptr));

    auto* ts = GetCurrentThreadState();
    auto* rs = ts ? ts->runtime_state : nullptr;

    auto vt = static_cast<CHAOS_IL2CPP_UINT16>(v->vt & ~CHAOS_IL2CPP_VT_BYREF);
    switch (vt) {
    case CHAOS_IL2CPP_VT_EMPTY:
    case CHAOS_IL2CPP_VT_NULL:
        return nullptr;
    case CHAOS_IL2CPP_VT_I2:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Int16");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.iVal, sizeof(v->data.iVal));
        }
    case CHAOS_IL2CPP_VT_I4:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Int32");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.lVal, sizeof(v->data.lVal));
        }
    case CHAOS_IL2CPP_VT_R4:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Single");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.fltVal, sizeof(v->data.fltVal));
        }
    case CHAOS_IL2CPP_VT_R8:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Double");
            if (type_handle == 0) return nullptr;
            return BoxValueObject(rs, ts, type_handle, &v->data.dblVal, sizeof(v->data.dblVal));
        }
    case CHAOS_IL2CPP_VT_BOOL:
        if (rs == nullptr || ts == nullptr) return nullptr;
        {
            auto type_handle = ResolveTypeByName("System.Boolean");
            if (type_handle == 0) return nullptr;
            CHAOS_IL2CPP_INT32 bool_val = (v->data.boolVal != 0) ? 1 : 0;
            return BoxValueObject(rs, ts, type_handle, &bool_val, sizeof(bool_val));
        }
    case CHAOS_IL2CPP_VT_BSTR:
        return MarshalPtrToStringBSTR(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(v->data.bstrVal));
    case CHAOS_IL2CPP_VT_UNKNOWN:
        return v->data.punkVal != nullptr
            ? reinterpret_cast<void*>(MarshalCreateRcw(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(v->data.punkVal)))
            : nullptr;
    default:
        return nullptr;
    }
#else
    (void)variant_ptr;
    return nullptr;
#endif
}

// ICALL: Marshal.GetNativeVariantForObject(Object, IntPtr, IntPtr) → void
void CHAOS_RUNTIME_ABI_CALL ChaosGetNativeVariantForObject(
    void* obj, CHAOS_IL2CPP_INTPTR variant_ptr, CHAOS_IL2CPP_INTPTR destroy_old) noexcept {
#if defined(_WIN32)
    if (variant_ptr == 0) return;
    auto* v = static_cast<VariantLayout*>(reinterpret_cast<void*>(variant_ptr));
    if (destroy_old != 0 && v->vt != CHAOS_IL2CPP_VT_EMPTY) {
        CHAOS_IL2CPP_MEMSET(v, 0, sizeof(VariantLayout));
    }
    if (obj == nullptr) {
        v->vt = CHAOS_IL2CPP_VT_NULL;
        return;
    }

    // Inspect the managed object's type to determine the correct VT.
    // Boxed value-type layout: TypeInfoHot* (8B) [+ sync (8B if ThinLockable)] + value.
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindMask;
    using ::chaos::il2cpp::common::kTypeInfoHeaderKindPure;
    const auto* obj_type = *static_cast<const TypeInfoHot* const*>(obj);
    const CHAOS_IL2CPP_SIZE hdr = (obj_type->flags & kTypeInfoHeaderKindMask) == kTypeInfoHeaderKindPure ? 8u : 16u;
    const void* val_ptr = static_cast<const uint8_t*>(obj) + hdr;

    // Resolve primitive type handles and compare against the object's type.
    auto th = ResolveTypeByName("System.Int32");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_INT32*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Int16");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I2;
        v->data.iVal = *static_cast<const CHAOS_IL2CPP_INT16*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Byte");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_UINT8*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.SByte");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_INT8*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.UInt16");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = *static_cast<const CHAOS_IL2CPP_UINT16*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.UInt32");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_I4;
        v->data.lVal = static_cast<CHAOS_IL2CPP_INT32>(*static_cast<const CHAOS_IL2CPP_UINT32*>(val_ptr));
        return;
    }
    th = ResolveTypeByName("System.Single");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_R4;
        v->data.fltVal = *static_cast<const float*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Double");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_R8;
        v->data.dblVal = *static_cast<const double*>(val_ptr);
        return;
    }
    th = ResolveTypeByName("System.Boolean");
    if (th != 0 && obj_type == reinterpret_cast<const TypeInfoHot*>(th)) {
        v->vt = CHAOS_IL2CPP_VT_BOOL;
        // VARIANT_TRUE = -1 (0xFFFF), VARIANT_FALSE = 0
        const CHAOS_IL2CPP_UINT8 b = *static_cast<const CHAOS_IL2CPP_UINT8*>(val_ptr);
        v->data.boolVal = (b != 0) ? -1 : 0;
        return;
    }

    // Fallback: marshal as VT_UNKNOWN.
    v->vt = CHAOS_IL2CPP_VT_UNKNOWN;
    v->data.punkVal = obj;
#else
    (void)obj;
    (void)variant_ptr;
    (void)destroy_old;
#endif
}

// ── ICustomMarshaler (V3 — interpreter dispatch) ─────────────────────────
//
// ICustomMarshaler allows user-defined managed types to control parameter
// marshalling for P/Invoke calls decorated with
// [MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(T), Cookie = "...")].
//
// V3 approach:
//   The cookie encodes the ICustomMarshaler implementor type name.
//   Native stub resolves the type, calls its static GetInstance(cookie) to
//   obtain/reuse a marshaler instance, then dispatches to the instance's
//   MarshalNativeToManaged / MarshalManagedToNative methods via the ABI
//   method_invoke table.
//
//   Results are cached per unique cookie string to avoid repeated resolution.

namespace {

// ── Cookie → marshaler cache ─────────────────────────────────────────────
// Fixed-size cache with linear scan (small N, low contention).
// Uses thread-safe slots: marshaler_instance != 0 means occupied.

struct MarshalerSlot {
    char cookie[64];             // cookie key (truncated)
    void* marshaler_instance;    // resolved ICustomMarshaler instance
    MethodInfoHandle method_n2m; // cached MarshalNativeToManaged
    MethodInfoHandle method_m2n; // cached MarshalManagedToNative
};

constexpr uint32_t kMarshalerCacheSize = 8;
static MarshalerSlot s_marshaler_cache[kMarshalerCacheSize] = {};

// ── Helpers ──────────────────────────────────────────────────────────────

/// Extract the fully-qualified type name from a cookie string.
/// Format: "TypeName" or "TypeName:Suffix" — returns only the TypeName part.
/// Returns a pointer into the input (null-terminated at the separator or end).
static const char* TypeNameFromCookie(const char* cookie_utf8, const char*& out_end) {
    if (cookie_utf8 == nullptr) {
        out_end = nullptr;
        return nullptr;
    }
    const char* sep = cookie_utf8;
    while (*sep != '\0' && *sep != ':') ++sep;
    out_end = sep;
    return cookie_utf8;
}

static void* ResolveOrCreateMarshaler(
    RuntimeState* rs,
    ThreadState* ts,
    const RuntimeAbiV0* abi,
    const char* cookie_utf8) {
    if (cookie_utf8 == nullptr || cookie_utf8[0] == '\0') return nullptr;

    // ── Scan cache ──
    for (uint32_t i = 0; i < kMarshalerCacheSize; ++i) {
        if (s_marshaler_cache[i].marshaler_instance != nullptr &&
            std::strncmp(s_marshaler_cache[i].cookie, cookie_utf8,
                         sizeof(s_marshaler_cache[i].cookie)) == 0) {
            return s_marshaler_cache[i].marshaler_instance;
        }
    }

    // ── Cache miss: resolve type and call GetInstance ──
    const char* type_name_end = nullptr;
    const char* type_name_start = TypeNameFromCookie(cookie_utf8, type_name_end);
    if (type_name_start == nullptr) return nullptr;

    // Build null-terminated type name from the cookie prefix.
    char type_name_buf[128];
    const size_t type_name_len = static_cast<size_t>(type_name_end - type_name_start);
    if (type_name_len == 0 || type_name_len >= sizeof(type_name_buf)) return nullptr;
    std::memcpy(type_name_buf, type_name_start, type_name_len);
    type_name_buf[type_name_len] = '\0';

    const auto type_handle = ResolveTypeByName(type_name_buf);
    if (type_handle == 0) {
        CHAOS_IL2CPP_LOG_WARN("ICustomMarshaler: type '{0}' not found", type_name_buf);
        return nullptr;
    }

    // Ensure type is initialized.
    abi->class_init(rs, type_handle);

    // Find GetInstance(string) static method.
    const MethodInfoHandle get_instance_method = abi->type_find_method(
        type_handle, "GetInstance", 1);
    if (get_instance_method == 0) {
        CHAOS_IL2CPP_LOG_WARN("ICustomMarshaler: GetInstance not found on {0}", type_name_buf);
        return nullptr;
    }

    // Create managed string from cookie for the argument.
    void* cookie_managed = abi->string_new_utf8(rs, ts, cookie_utf8,
        static_cast<CHAOS_IL2CPP_INT32>(std::strlen(cookie_utf8)));
    if (cookie_managed == nullptr) return nullptr;

    // Call GetInstance(cookie) — static method, so object_instance = nullptr.
    void* const args_get_instance[] = { &cookie_managed };
    void* marshaler_instance = nullptr;
    ExceptionHandle exc = nullptr;
    abi->method_invoke(rs, ts, get_instance_method,
        nullptr, args_get_instance, 1, &marshaler_instance, sizeof(void*), &exc);

    if (marshaler_instance == nullptr) {
        CHAOS_IL2CPP_LOG_WARN("ICustomMarshaler: GetInstance returned null for cookie '{0}'",
                              cookie_utf8);
        return nullptr;
    }

    // Pre-resolve MarshalNativeToManaged and MarshalManagedToNative method handles
    // so we don't need to re-resolve them on each call. We find them on the
    // marshaler's actual type (not ICustomMarshaler interface).
    const MethodInfoHandle method_n2m = abi->type_find_method(
        type_handle, "MarshalNativeToManaged", 1);
    const MethodInfoHandle method_m2n = abi->type_find_method(
        type_handle, "MarshalManagedToNative", 1);

    // Store in first empty cache slot.
    for (uint32_t i = 0; i < kMarshalerCacheSize; ++i) {
        if (s_marshaler_cache[i].marshaler_instance == nullptr) {
            std::strncpy(s_marshaler_cache[i].cookie, cookie_utf8,
                         sizeof(s_marshaler_cache[i].cookie) - 1);
            s_marshaler_cache[i].cookie[sizeof(s_marshaler_cache[i].cookie) - 1] = '\0';
            s_marshaler_cache[i].marshaler_instance = marshaler_instance;
            s_marshaler_cache[i].method_n2m = method_n2m;
            s_marshaler_cache[i].method_m2n = method_m2n;
            break;
        }
    }

    return marshaler_instance;
}

}  // anonymous namespace

CHAOS_IL2CPP_INTPTR CustomMarshalerNativeToManaged(
    const char* cookie_utf8,
    CHAOS_IL2CPP_INTPTR native_ptr) noexcept {
    auto* abi = GetRuntimeAbiV0();
    auto* rs = runtime_core::GetCurrentRuntimeState();
    auto* ts = runtime_core::GetCurrentThreadState();
    if (abi == nullptr || rs == nullptr || ts == nullptr) return 0;

    void* marshaler = ResolveOrCreateMarshaler(rs, ts, abi, cookie_utf8);
    if (marshaler == nullptr) return 0;

    // Find the MarshalNativeToManaged method from cache.
    MethodInfoHandle method_n2m = 0;
    for (uint32_t i = 0; i < kMarshalerCacheSize; ++i) {
        if (s_marshaler_cache[i].marshaler_instance == marshaler) {
            method_n2m = s_marshaler_cache[i].method_n2m;
            break;
        }
    }
    if (method_n2m == 0) return 0;

    // Call marshaler.MarshalNativeToManaged(native_ptr).
    void* const args[] = { &native_ptr };
    void* result = nullptr;
    ExceptionHandle exc = nullptr;
    abi->method_invoke(rs, ts, method_n2m, marshaler, args, 1,
                       &result, sizeof(void*), &exc);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(result);
}

CHAOS_IL2CPP_INTPTR CustomMarshalerManagedToNative(
    const char* cookie_utf8,
    CHAOS_IL2CPP_INTPTR managed_obj) noexcept {
    auto* abi = GetRuntimeAbiV0();
    auto* rs = runtime_core::GetCurrentRuntimeState();
    auto* ts = runtime_core::GetCurrentThreadState();
    if (abi == nullptr || rs == nullptr || ts == nullptr) return managed_obj;

    void* marshaler = ResolveOrCreateMarshaler(rs, ts, abi, cookie_utf8);
    if (marshaler == nullptr) return managed_obj;

    // Find the MarshalManagedToNative method from cache.
    MethodInfoHandle method_m2n = 0;
    for (uint32_t i = 0; i < kMarshalerCacheSize; ++i) {
        if (s_marshaler_cache[i].marshaler_instance == marshaler) {
            method_m2n = s_marshaler_cache[i].method_m2n;
            break;
        }
    }
    if (method_m2n == 0) return managed_obj;

    // Call marshaler.MarshalManagedToNative(managed_obj).
    void* const args[] = { &managed_obj };
    CHAOS_IL2CPP_INTPTR result = 0;
    ExceptionHandle exc = nullptr;
    abi->method_invoke(rs, ts, method_m2n, marshaler, args, 1,
                       &result, sizeof(CHAOS_IL2CPP_INTPTR), &exc);
    return result;
}

// ── HRESULT exception helpers (V1) ────────────────────────────────────

void MarshalThrowExceptionForHR(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INT32 error_code) noexcept {
    (void)thread_state;
    if (error_code >= 0) return;
    if (runtime_state == nullptr) return;

    char msg_buf[48];
    std::snprintf(msg_buf, sizeof(msg_buf), "HRESULT: 0x%08X",
                  static_cast<unsigned int>(error_code));
    RaiseManagedException("System.Exception", msg_buf);
}

void* MarshalGetExceptionForHR(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INT32 error_code) noexcept {
    if (error_code >= 0) return nullptr;
    if (runtime_state == nullptr || thread_state == nullptr) return nullptr;

    auto* abi = GetRuntimeAbiV0();
    if (abi == nullptr) return nullptr;

    auto type_handle = ResolveTypeByName("System.Exception");
    if (type_handle == 0) return nullptr;

    abi->class_init(runtime_state, type_handle);
    auto* exc = abi->object_new(runtime_state, thread_state, type_handle);
    if (exc == nullptr) return nullptr;

    char msg_buf[48];
    const int msg_len = std::snprintf(msg_buf, sizeof(msg_buf), "HRESULT: 0x%08X",
                                       static_cast<unsigned int>(error_code));
    auto* msg_obj = abi->string_new_utf8(runtime_state, thread_state,
                                          msg_buf, static_cast<uintptr_t>(msg_len));
    if (msg_obj != nullptr) {
        ChaosReflectionSetExceptionMetadata(
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(exc),
            reinterpret_cast<CHAOS_IL2CPP_INTPTR>(msg_obj));
    }

    // Set _HResult field.
    // Exception layout: ThinLockableHeader(16B) + _message(8B) +
    //   _innerException(8B) + _stackTrace(8B) + _HResult(4B)
    const auto hr_offset = sizeof(ThinLockableHeader) + 3 * sizeof(CHAOS_IL2CPP_INTPTR);
    *reinterpret_cast<CHAOS_IL2CPP_INT32*>(
        static_cast<CHAOS_IL2CPP_UINT8*>(exc) + hr_offset) = error_code;

    return exc;
}

CHAOS_IL2CPP_INT32 MarshalGetHRForException(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* exception_obj) noexcept {
    (void)runtime_state;
    (void)thread_state;
    if (exception_obj == nullptr) return 0;

    // Exception layout: ThinLockableHeader(16B) + _message(8B) +
    //   _innerException(8B) + _stackTrace(8B) + _HResult(4B)
    const auto hr_offset = sizeof(ThinLockableHeader) + 3 * sizeof(CHAOS_IL2CPP_INTPTR);
    return *reinterpret_cast<CHAOS_IL2CPP_INT32*>(
        static_cast<CHAOS_IL2CPP_UINT8*>(exception_obj) + hr_offset);
}

}  // namespace chaos::il2cpp::runtime_core