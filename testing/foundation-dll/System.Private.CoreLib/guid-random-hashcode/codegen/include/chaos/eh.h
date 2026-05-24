#ifndef CHAOS_IL2CPP_COMMON_EH_H_
#define CHAOS_IL2CPP_COMMON_EH_H_

// ── Unified Exception Handling Macros ─────────────────────────────
//
// These macros abstract over the three EH implementation modes:
//   - C++ try/catch          (default, no CHAOS_IL2CPP_EH_* defined)
//   - Win32 SEH (__try/__except)
//   - SETJMP / longjmp
//
// Usage — try/catch:
//
//   CHAOS_EH_TRY
//       // try body
//   CHAOS_EH_CATCH_BEGIN
//       // optional type check:
//       if (!chaos_eh_match_type(CHAOS_EH_EXCEPTION_OBJ, &SomeTypeInfo))
//           CHAOS_EH_RETHROW;
//       // handler body
//   CHAOS_EH_END
//
// Usage — try/finally (define _chaos_finally lambda first):
//
//   auto _chaos_finally = [&]() { /* cleanup */ };
//   CHAOS_EH_TRY_FINALLY
//       // try body
//   CHAOS_EH_FINALLY_END
//
// Usage — throw / rethrow:
//
//   CHAOS_EH_THROW(expr)
//   CHAOS_EH_RETHROW
//
// Requires: <chaos/runtime_core.h> included before this header (provides
// chaos::il2cpp::runtime_core::g_chaos_exception_obj,
// push/pop_exception_jmp_buf, chaos_raise_exception, etc.)

// ── Try/Catch ─────────────────────────────────────────────────────

#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)

// C++ try/catch mode
#define CHAOS_EH_TRY               try {
#define CHAOS_EH_CATCH_BEGIN       } catch (const chaos_managed_exception& chaos_exception) {
#define CHAOS_EH_END               }
#define CHAOS_EH_EXCEPTION_OBJ     (chaos_exception.object_value)
#define CHAOS_EH_THROW(val)        throw chaos_managed_exception{val}
#define CHAOS_EH_RETHROW           throw

#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)

// Win32 SEH mode
#define CHAOS_EH_TRY               __try {
#define CHAOS_EH_CATCH_BEGIN       } __except(CHAOS_SEH_FILTER_ALL()) {
#define CHAOS_EH_END               }
#define CHAOS_EH_EXCEPTION_OBJ     (reinterpret_cast<CHAOS_IL2CPP_INTPTR>( \
                                       chaos::il2cpp::runtime_core::g_chaos_exception_obj))
#define CHAOS_EH_THROW(val)        chaos::il2cpp::runtime_core::chaos_raise_exception(val)
#define CHAOS_EH_RETHROW           chaos::il2cpp::runtime_core::chaos_raise_exception( \
                                       CHAOS_EH_EXCEPTION_OBJ)

#else

// SETJMP mode
#define CHAOS_EH_TRY               { auto* _chaos_jmp = \
                                       chaos::il2cpp::runtime_core::push_exception_jmp_buf(); \
                                     if (setjmp(*_chaos_jmp) == 0) {
#define CHAOS_EH_CATCH_BEGIN       } else {
#define CHAOS_EH_END               } \
                                     chaos::il2cpp::runtime_core::pop_exception_jmp_buf(); }
#define CHAOS_EH_EXCEPTION_OBJ     (reinterpret_cast<CHAOS_IL2CPP_INTPTR>( \
                                       chaos::il2cpp::runtime_core::g_chaos_exception_obj))
#define CHAOS_EH_THROW(val)        chaos::il2cpp::runtime_core::chaos_raise_exception(val)
#define CHAOS_EH_RETHROW           do { \
                                       chaos::il2cpp::runtime_core::pop_exception_jmp_buf(); \
                                       chaos::il2cpp::runtime_core::chaos_raise_exception( \
                                           CHAOS_EH_EXCEPTION_OBJ); \
                                   } while(0)

#endif

// ── Try/Finally ───────────────────────────────────────────────────
// Requires `auto _chaos_finally = [&]() { /* finally body */ };` before use.

#if !defined(CHAOS_IL2CPP_EH_SETJMP) && !defined(CHAOS_IL2CPP_EH_WIN32_SEH)

// C++: RAII scope guard — runs _chaos_finally on scope exit (normal or exceptional).
#define CHAOS_EH_TRY_FINALLY       { auto _chaos_finally_guard = \
                                       chaos::il2cpp::common::make_finally_guard(\
                                           _chaos_finally)
#define CHAOS_EH_FINALLY_END       }

#elif defined(CHAOS_IL2CPP_EH_WIN32_SEH)

// Win32 SEH: __finally runs on both normal and unwind.
#define CHAOS_EH_TRY_FINALLY       __try {
#define CHAOS_EH_FINALLY_END       } __finally { _chaos_finally(); }

#else

// SETJMP: explicit finally call in exception path before re-raise.
#define CHAOS_EH_TRY_FINALLY       { auto* _chaos_jmp = \
                                       chaos::il2cpp::runtime_core::push_exception_jmp_buf(); \
                                     bool _chaos_caught = false; \
                                     if (setjmp(*_chaos_jmp) == 0) {
#define CHAOS_EH_FINALLY_END       } else { _chaos_caught = true; _chaos_finally(); } \
                                     chaos::il2cpp::runtime_core::pop_exception_jmp_buf(); \
                                     if (_chaos_caught) { \
                                       chaos::il2cpp::runtime_core::chaos_raise_exception( \
                                           reinterpret_cast<CHAOS_IL2CPP_INTPTR>( \
                                               chaos::il2cpp::runtime_core::\
                                                   g_chaos_exception_obj)); \
                                     } }

#endif

#endif // CHAOS_IL2CPP_COMMON_EH_H_
