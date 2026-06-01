// pal_crash_win32.cpp — Win32 VectoredExceptionHandler crash handling

#include <chaos/pal/pal_crash.h>

#define NOMINMAX
#include <windows.h>

namespace chaos::il2cpp::pal {

static PalCrashCallback s_crash_callback = nullptr;
static void* s_veh_handle = nullptr;

static LONG CALLBACK VectoredHandler(PEXCEPTION_POINTERS info) noexcept {
    if (!s_crash_callback) return EXCEPTION_CONTINUE_SEARCH;

    CrashContext ctx;
    ctx.signal = static_cast<int>(info->ExceptionRecord->ExceptionCode);
    ctx.fault_address = info->ExceptionRecord->ExceptionAddress;

    s_crash_callback(ctx);

    // Never handle the exception — let the normal SEH chain process it.
    return EXCEPTION_CONTINUE_SEARCH;
}

bool PalRegisterCrashHandler(PalCrashCallback callback) noexcept {
    if (s_veh_handle) {
        ::RemoveVectoredExceptionHandler(s_veh_handle);
        s_veh_handle = nullptr;
    }
    s_crash_callback = callback;
    s_veh_handle = ::AddVectoredExceptionHandler(1, VectoredHandler);
    return s_veh_handle != nullptr;
}

void PalRemoveCrashHandler() noexcept {
    s_crash_callback = nullptr;
    if (s_veh_handle) {
        ::RemoveVectoredExceptionHandler(s_veh_handle);
        s_veh_handle = nullptr;
    }
}

void PalDebugBreak() noexcept {
    __debugbreak();
}

}  // namespace chaos::il2cpp::pal
