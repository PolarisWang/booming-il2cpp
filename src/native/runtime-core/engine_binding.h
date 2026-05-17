#ifndef CHAOS_IL2CPP_ENGINE_BINDING_H_
#define CHAOS_IL2CPP_ENGINE_BINDING_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>
#include "marshal_abi.h"
#include "runtime_capability.h"

namespace chaos::il2cpp::runtime_core {

CHAOS_IL2CPP_INT32 EngineLogWrite(
    const char* category_utf8,
    const char* message_utf8);
CHAOS_IL2CPP_UINTPTR CreateEngineObjectHandle(void* object_instance);
void* ResolveEngineObjectHandle(CHAOS_IL2CPP_UINTPTR handle);
bool RegisterEngineLifecycleCallback(
    const char* phase_utf8,
    EngineLifecycleCallback callback,
    void* user_data);
bool DispatchEngineLifecycleCallbacks(const char* phase_utf8);
bool IsMainThreadLane();
bool ThreadStaticInt32Add(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const char* slot_key_utf8,
    CHAOS_IL2CPP_INT32 delta,
    CHAOS_IL2CPP_INT32* out_value);
bool MonitorEnter(void* monitor_target);
bool MonitorExit(void* monitor_target);
bool MonitorTryEnter(void* monitor_target);
bool MonitorIsEntered(void* monitor_target);
bool MonitorWait(void* monitor_target, int32_t timeout_ms);
bool MonitorPulse(void* monitor_target);
bool MonitorPulseAll(void* monitor_target);
bool ThreadSleep(int32_t timeout_ms);
bool SpinLockExit(void* spinlock_target);
bool SpinLockIsHeld(void* spinlock_target);
bool LockEnter(void* lock_target);
bool LockExit(void* lock_target);
bool WaitHandleSet(void* wait_handle);
bool WaitHandleReset(void* wait_handle);
bool GcSafepoint(
    RuntimeState* runtime_state,
    ThreadState* thread_state);
CHAOS_IL2CPP_SIZE ReportThreadRoot(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const void* root_address,
    CHAOS_IL2CPP_SIZE root_size);
bool EnqueueFinalizer(
    RuntimeState* runtime_state,
    void* object_instance,
    FinalizerCallback finalizer);
CHAOS_IL2CPP_SIZE DrainFinalizerQueue(RuntimeState* runtime_state);
CHAOS_IL2CPP_INTPTR MarshalAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR size);
CHAOS_IL2CPP_INTPTR MarshalAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INT32 size);
CHAOS_IL2CPP_INTPTR MarshalReAllocHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INTPTR size);
CHAOS_IL2CPP_INTPTR MarshalReAllocCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_INT32 size);
bool MarshalFreeHGlobal(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
bool MarshalFreeCoTaskMem(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
bool MarshalZeroFreeCoTaskMemUtf8(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory);
void* MarshalPtrToStringUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR utf8_buffer,
    CHAOS_IL2CPP_INT32 length,
    bool has_explicit_length);
CHAOS_IL2CPP_INTPTR MarshalStringToCoTaskMemUtf8(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string);
void* MarshalPtrToStringWide(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    CHAOS_IL2CPP_INTPTR wide_buffer,
    CHAOS_IL2CPP_INT32 length,
    bool has_explicit_length);
CHAOS_IL2CPP_INTPTR MarshalStringToCoTaskMemWide(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string);
CHAOS_IL2CPP_UINT8 MarshalReadByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT16 MarshalReadInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT32 MarshalReadInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INT64 MarshalReadInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
CHAOS_IL2CPP_INTPTR MarshalReadIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset);
void MarshalWriteByte(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_UINT8 value);
void MarshalWriteInt16(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT16 value);
void MarshalWriteInt32(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 value);
void MarshalWriteInt64(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT64 value);
void MarshalWriteIntPtr(CHAOS_IL2CPP_INTPTR address, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INTPTR value);
CHAOS_IL2CPP_INT32 TaskKernelNewId();
CHAOS_IL2CPP_INT32 InteropKernel32GetLastError();
CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentProcessId();
CHAOS_IL2CPP_INT32 InteropKernel32GetCurrentThreadId();
CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentProcess();
CHAOS_IL2CPP_INTPTR InteropKernel32GetCurrentThread();
bool InteropKernel32CloseHandle(CHAOS_IL2CPP_INTPTR handle);
bool InteropKernel32FreeLibrary(CHAOS_IL2CPP_INTPTR handle);
ValueTypeKernelBackendKind GetCharClassificationKernelBackend();
ValueTypeKernelBackendKind GetFloatingClassificationKernelBackend();
ValueTypeKernelBackendKind GetBitArithmeticKernelBackend();
ValueTypeKernelBackendKind GetTickArithmeticKernelBackend();
VectorKernelBackendKind GetVectorKernelBackend();

// ── Native library cache (P/Invoke) ─────────────────────────────
/// Load a native DLL with caching and refcounting.
/// Thread-safe: concurrent calls with the same name return the same handle.
void* NativeLibraryLoad(const char* name_utf8);

/// Resolve a symbol from a loaded library.
/// Returns a function pointer or nullptr.
void* NativeLibraryGetProcAddress(void* handle, const char* symbol_utf8);

/// Release one reference to a native library.
/// When refcount reaches zero the library is unloaded.
bool NativeLibraryFree(void* handle);

// ── String marshalling helpers (P/Invoke) ─────────────────────────
/// Convert a managed UTF-8 string to a native UTF-16 buffer.
/// Returns the number of UTF-16 code units written (excluding NUL).
/// If buf_capacity is 0, returns the required size (excluding NUL).
CHAOS_IL2CPP_INT32 MarshalStringToWide(void* managed_string, CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 buf_capacity);

/// Convert a native UTF-16 buffer to a managed UTF-8 string.
/// Returns a managed String object.
void* MarshalWideToString(RuntimeState* runtime_state, ThreadState* thread_state, const CHAOS_IL2CPP_UINT16* wide_buf, CHAOS_IL2CPP_INT32 wide_len);

/// Convert a native ANSI/UTF-8 buffer to a managed UTF-8 string.
/// Returns a managed String object.
void* MarshalAnsiToString(RuntimeState* runtime_state, ThreadState* thread_state, const char* ansi_buf, CHAOS_IL2CPP_INT32 byte_len);

// ── String marshalling helpers (HGlobal variants) ─────────────────────
/// Convert a managed UTF-8 string to an HGlobal ANSI (CP_ACP) buffer.
/// Returns pointer to the HGlobal-allocated buffer, or 0.
/// Win32: UTF-8 → UTF-16 → CP_ACP. Non-Win32: UTF-8 pass-through (ACP == UTF-8 assumption).
CHAOS_IL2CPP_INTPTR MarshalStringToHGlobalAnsi(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string);

/// Convert a managed UTF-8 string to an HGlobal UTF-16 buffer.
/// Returns pointer to the HGlobal-allocated buffer, or 0.
CHAOS_IL2CPP_INTPTR MarshalStringToHGlobalUni(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* managed_string);

/// Convert a native ANSI buffer to a managed String (ICALL entry point).
/// Wraps MarshalAnsiToString for callers that don't have RuntimeState/ThreadState.
void* MarshalPtrToStringAnsiIcall(
    CHAOS_IL2CPP_INTPTR ansi_buffer,
    CHAOS_IL2CPP_INT32 length) noexcept;

/// Return a pinned pointer to the element at the given index inside a managed array.
/// The caller is responsible for pinning the array before calling this.
/// Returns 0 if array is null or index is out of bounds.
CHAOS_IL2CPP_INTPTR MarshalUnsafeAddrOfPinnedArrayElement(
    void* managed_array,
    CHAOS_IL2CPP_INT32 index) noexcept;

/// Extract the inner handle value from a SafeHandle (or CriticalHandle) object.
/// Returns the raw IntPtr handle value, or 0 if the object is nullptr.
CHAOS_IL2CPP_INTPTR MarshalSafeHandleGetHandle(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    void* safe_handle_obj);

/// Return the marshalled size of a value type via its StructMarshallingDescriptorV1.
/// Falls back to the type's instance_size when no descriptor is registered.
/// Returns 0 for unknown types.
CHAOS_IL2CPP_INT32 MarshalSizeOf(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    const TypeInfoHot* type_info);

/// Look up a struct field offset by name using a StructMarshallingDescriptorV1
/// and its associated parallel field-names array (emitted as static constexpr
/// const char*[] by the struct marshalling descriptors template).
/// field_name_obj is a managed System.String whose UTF-8 payload is extracted
/// internally. Returns the field's byte offset within the struct, or -1 if
/// not found (caller should throw ArgumentException). Thread-safe.
CHAOS_IL2CPP_INT32 MarshalOffsetOf(
    const marshal_abi::StructMarshallingDescriptorV1* desc,
    const char* const* field_names,
    CHAOS_IL2CPP_INTPTR field_name_obj) noexcept;

/// Non-generic Marshal.OffsetOf(Type, string) ICALL.
/// Resolves TypeInfoHandle internally; requires a registered descriptor and
/// registered field-names array (see RegisterStaticMarshallingFieldNames).
/// Returns the field's byte offset, or -1 if the type or field is unknown.
CHAOS_IL2CPP_INT32 MarshalOffsetOfByType(
    RuntimeState* runtime_state,
    ThreadState* thread_state,
    TypeInfoHandle type_handle,
    CHAOS_IL2CPP_INTPTR field_name_obj) noexcept;

/// Store the P/Invoke last-error value into thread-local storage.
/// Called by codegen P/Invoke stubs after native calls that have SetLastError=true.
void SetLastPInvokeError(ThreadState* thread_state, CHAOS_IL2CPP_INT32 error) noexcept;

/// Retrieve the per-call P/Invoke last-error value from thread-local storage.
/// Called by Marshal.GetLastPInvokeError() ICALL and Marshal.GetLastWin32Error() ICALL.
CHAOS_IL2CPP_INT32 GetLastPInvokeError(ThreadState* thread_state) noexcept;
CHAOS_IL2CPP_INT32 GetLastPInvokeErrorIcall() noexcept;

/// Clear the OS-level last-error before a P/Invoke call (SetLastError(0) on Win32).
/// No-op on non-Windows platforms.
void ClearOsLastError() noexcept;

/// Capture the OS-level last-error after a P/Invoke call (GetLastError() on Win32).
/// Returns 0 on non-Windows platforms.
CHAOS_IL2CPP_INT32 GetOsLastError() noexcept;

/// Try to resolve a P/Invoke library through a registered DllImportResolver.
/// Returns the native library handle, or nullptr if no resolver is registered
/// or the resolver returned nullptr.
/// Thread-safe.
/// \param assembly_name_utf8  The declaring assembly name of the P/Invoke method.
/// \param library_name_utf8   The module name from [DllImport].
void* TryResolveDllImport(const char* assembly_name_utf8, const char* library_name_utf8) noexcept;

/// Register a reverse P/Invoke callback that implements DllImportResolver dispatch.
/// Called once during runtime initialization.
/// \param callback  Function pointer with signature
///   IntPtr(const char* assemblyName, const char* libraryName)
///   Returns a native library handle or 0.
void RegisterPInvokeResolverCallback(void* callback) noexcept;

/// Runtime helper for Marshal.DestroyStructure(IntPtr, Type) — non-generic overload.
/// Extracts TypeInfoHot* from the managed Type object, resolves the struct
/// descriptor, and frees non-blittable fields (CoTaskMem strings, etc.).
/// Defined in marshal_api.cpp.
CHAOS_IL2CPP_INTPTR ChaosDestroyStructureByType(CHAOS_IL2CPP_INTPTR struct_ptr, CHAOS_IL2CPP_INTPTR type_obj) noexcept;

/// ICALL: Marshal.StringToBSTR(string) → IntPtr
CHAOS_IL2CPP_INTPTR MarshalStringToBSTR(void* managed_string) noexcept;

/// ICALL: Marshal.PtrToStringBSTR(IntPtr) → String
void* MarshalPtrToStringBSTR(CHAOS_IL2CPP_INTPTR bstr_ptr) noexcept;

/// ICALL: Marshal.FreeBSTR(IntPtr) → void
void MarshalFreeBSTR(CHAOS_IL2CPP_INTPTR bstr_ptr) noexcept;

/// ICALL: Marshal.GetObjectForNativeVariant(IntPtr) → Object (V1 stub)
void* ChaosGetObjectForNativeVariant(CHAOS_IL2CPP_INTPTR variant_ptr) noexcept;

/// ICALL: Marshal.GetNativeVariantForObject(Object, IntPtr, IntPtr) → void (V1 stub)
void ChaosGetNativeVariantForObject(void* obj, CHAOS_IL2CPP_INTPTR variant_ptr, CHAOS_IL2CPP_INTPTR destroy_old) noexcept;

// ── COM apartment management ────────────────────────────────────────
/// Initialize COM apartment for the current thread (CoInitializeEx wrapper).
/// apartment_type: COINIT_APARTMENTTHREADED=2, COINIT_MULTITHREADED=0, etc.
/// Returns S_OK (0) on success, or a COM HRESULT error code.
/// Defined in engine_lifecycle.cpp.
CHAOS_IL2CPP_INT32 CoInitializeApartment(CHAOS_IL2CPP_INT32 apartment_type) noexcept;

/// Uninitialize COM for the current thread (CoUninitialize wrapper).
/// Defined in engine_lifecycle.cpp.
void CoUninitializeApartment() noexcept;

// ── COM object creation ────────────────────────────────────────────
/// CoCreateInstance wrapper: creates a COM object from CLSID.
/// clsid_bytes: 16-byte GUID. iid_bytes: 16-byte IID.
/// Returns the IUnknown* as IntPtr, or 0 on failure.
/// Defined in marshal_api.cpp.
CHAOS_IL2CPP_INTPTR CoCreateComInstance(
    const CHAOS_IL2CPP_UINT8* clsid_bytes,
    const CHAOS_IL2CPP_UINT8* iid_bytes) noexcept;

// ── RCW (Runtime Callable Wrapper) ─────────────────────────────────
/// Wrap a raw IUnknown* COM pointer in an RCW.
/// Returns an IntPtr pointing to the ComRcwNative.
/// Defined in marshal_api.cpp.
CHAOS_IL2CPP_INTPTR MarshalCreateRcw(CHAOS_IL2CPP_INTPTR unknown_ptr) noexcept;

/// Release a managed wrapper's reference on an RCW.
/// Defined in marshal_api.cpp.
void MarshalReleaseRcw(CHAOS_IL2CPP_INTPTR rcw_native_ptr) noexcept;

/// Get the raw IUnknown* from an RCW (for vtable dispatch).
/// Defined in marshal_api.cpp.
CHAOS_IL2CPP_INTPTR MarshalGetRcwUnknown(CHAOS_IL2CPP_INTPTR rcw_native_ptr) noexcept;

/// QueryInterface on an RCW, returns raw interface pointer.
/// Defined in marshal_api.cpp.
CHAOS_IL2CPP_INTPTR MarshalRcwQueryInterface(
    CHAOS_IL2CPP_INTPTR rcw_native_ptr,
    const CHAOS_IL2CPP_UINT8* iid_bytes) noexcept;

/// Check whether an IntPtr is an RCW handle (by magic value).
bool MarshalIsRcwHandle(CHAOS_IL2CPP_INTPTR ptr) noexcept;

/// Throw a managed COMException for a failed HRESULT.
void ChaosThrowComExceptionForHR(CHAOS_IL2CPP_INT32 hr) noexcept;

// ── CCW (COM Callable Wrapper) ─────────────────────────────────────
/// Create a CCW that exposes a managed object as a COM IUnknown.
/// Returns the CCW pointer (IUnknown*) as IntPtr, or 0 on failure.
/// Defined in com_ccw.cpp.
CHAOS_IL2CPP_INTPTR MarshalCreateCcw(
    CHAOS_IL2CPP_INTPTR managed_object,
    CHAOS_IL2CPP_INTPTR runtime_state) noexcept;

// ── ICustomMarshaler (V1 stubs) ──────────────────────────────────────────
/// ICALL stub: resolve and invoke ICustomMarshaler.MarshalNativeToManaged.
/// V1: logs a warning and returns nullptr (caller should pass through).
CHAOS_IL2CPP_INTPTR CustomMarshalerNativeToManaged(
    const char* cookie_utf8,
    CHAOS_IL2CPP_INTPTR native_ptr) noexcept;

/// ICALL stub: resolve and invoke ICustomMarshaler.MarshalManagedToNative.
/// V1: logs a warning and returns native_ptr unchanged (pass-through).
CHAOS_IL2CPP_INTPTR CustomMarshalerManagedToNative(
    const char* cookie_utf8,
    CHAOS_IL2CPP_INTPTR managed_obj) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_ENGINE_BINDING_H_
