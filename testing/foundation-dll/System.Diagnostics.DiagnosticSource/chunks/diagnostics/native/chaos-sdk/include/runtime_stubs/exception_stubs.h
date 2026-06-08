// ── Exception stub declarations ────────────────────────────────
#pragma once

CHAOS_IL2CPP_INTPTR ChaosExceptionGetBaseException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INTPTR ChaosExceptionGetInnerException(CHAOS_IL2CPP_INTPTR exc) noexcept;
CHAOS_IL2CPP_INT32  ChaosExceptionGetHresult(CHAOS_IL2CPP_INTPTR exc) noexcept;

CHAOS_IL2CPP_INTPTR ChaosRuntimewrappedGetWrappedException(CHAOS_IL2CPP_INTPTR exc) noexcept;