// ── Exception stub declarations ────────────────────────────────
#pragma once

CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INT32  ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept;

CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept;

// Invoke an Action delegate for Assert.Throws<T> verification blocks.
// The delegate is expected to throw a managed exception (chaos_managed_exception).
// This function is NOT noexcept because the caller wraps it in a try/catch.
void ChaosInvokeAction(CHAOS_IL2CPP_INTPTR action);