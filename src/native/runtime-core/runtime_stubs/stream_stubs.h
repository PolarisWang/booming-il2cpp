// ── Stream/IO stub declarations ──────────────────────────────
#pragma once

void ChaosStreamFlush(CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamSeek(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 offset, CHAOS_IL2CPP_INT32 origin) noexcept;
void ChaosStreamSetLength(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamGetLength(CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamGetPosition(CHAOS_IL2CPP_INTPTR stream) noexcept;
void ChaosMemoryStreamCtor(CHAOS_IL2CPP_INTPTR instance) noexcept;
void ChaosStringReaderCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR input) noexcept;
void ChaosStringWriterCtor(CHAOS_IL2CPP_INTPTR instance) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadLine(CHAOS_IL2CPP_INTPTR reader) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadToEnd(CHAOS_IL2CPP_INTPTR reader) noexcept;
void ChaosTextWriterWrite(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosTextWriterWriteLine(CHAOS_IL2CPP_INTPTR writer) noexcept;
void ChaosTextWriterWriteLineStr(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept;