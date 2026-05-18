#ifndef CHAOS_IL2CPP_COMMON_THREAD_IMPL_H_
#define CHAOS_IL2CPP_COMMON_THREAD_IMPL_H_

// ThreadRuntimeEntry is entirely header-only via inline definitions.
// This .cpp exists to anchor the translation unit; all logic lives in thread.h.

namespace chaos::il2cpp::common {

// Fail hook for generated code verification (setjmp/longjmp fallback).
void (*g_chaos_fail_hook)() = nullptr;

} // namespace chaos::il2cpp::common

#endif // CHAOS_IL2CPP_COMMON_THREAD_IMPL_H_
