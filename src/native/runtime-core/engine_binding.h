#ifndef CHAOS_IL2CPP_ENGINE_BINDING_H_
#define CHAOS_IL2CPP_ENGINE_BINDING_H_

#include <chaos/native_types.h>
#include <runtime_abi.h>
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

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_ENGINE_BINDING_H_
