// ── Managed Mutex stub declarations (System.Threading.Mutex) ──
#pragma once

// Create a named or unnamed Mutex.
// @param initially_owned  1 = caller owns the mutex immediately, 0 = not owned.
// @param name_ptr         Pointer to managed string (nullptr for unnamed).
// @param created_new      Output: 1 = new mutex created, 0 = existing opened.
//                         May be nullptr if caller does not need this info.
// @return Native handle (0 on failure).  On Windows this is a HANDLE; on
//         POSIX this is a sem_t* for named or pthread_mutex_t* for unnamed.
CHAOS_IL2CPP_INTPTR chaos_mutex_create(
    CHAOS_IL2CPP_INT32 initially_owned,
    CHAOS_IL2CPP_INTPTR name_ptr,
    CHAOS_IL2CPP_INTPTR* created_new) noexcept;

// Open an existing named Mutex.
// @param name_ptr  Pointer to managed string.
// @return Native handle (0 if not found or error).
CHAOS_IL2CPP_INTPTR chaos_mutex_open(CHAOS_IL2CPP_INTPTR name_ptr) noexcept;

// Release (unlock) the mutex.
// @param handle  Native handle from chaos_mutex_create or chaos_mutex_open.
// @return 0 = success, -1 = error.
CHAOS_IL2CPP_INT32 chaos_mutex_release(CHAOS_IL2CPP_INTPTR handle) noexcept;

// Wait to acquire the mutex.
// @param handle     Native handle.
// @param timeout_ms -1 = infinite, 0 = poll, >0 = bounded wait in ms.
// @return 1 = acquired, 0 = timeout, -1 = error.
CHAOS_IL2CPP_INT32 chaos_mutex_wait_one(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 timeout_ms) noexcept;

// Close (destroy) the mutex handle.
// @param handle  Native handle.
// @return 0 = success, -1 = error.
CHAOS_IL2CPP_INT32 chaos_mutex_close(CHAOS_IL2CPP_INTPTR handle) noexcept;
