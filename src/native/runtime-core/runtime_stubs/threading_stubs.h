// ── Threading stub declarations ────────────────────────────────
#pragma once

CHAOS_IL2CPP_INTPTR chaos_thread_get_current(void) noexcept;
CHAOS_IL2CPP_INT32 ChaosMonitorTryEnter(CHAOS_IL2CPP_INTPTR obj, CHAOS_IL2CPP_INT32 timeout) noexcept;