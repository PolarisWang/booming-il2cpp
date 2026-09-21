// ── Stream/IO stub declarations ──────────────────────────────
#pragma once

void ChaosStreamFlush(CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamSeek(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 offset, CHAOS_IL2CPP_INT32 origin) noexcept;
void ChaosStreamSetLength(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 value) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamGetLength(CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INT64 ChaosStreamGetPosition(CHAOS_IL2CPP_INTPTR stream) noexcept;
void ChaosMemoryStreamCtor(CHAOS_IL2CPP_INTPTR instance) noexcept;
void ChaosMemoryStreamCtorWithBuffer(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR buffer) noexcept;
void ChaosStringReaderCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR input) noexcept;
void ChaosStringWriterCtor(CHAOS_IL2CPP_INTPTR instance) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadLine(CHAOS_IL2CPP_INTPTR reader) noexcept;
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadToEnd(CHAOS_IL2CPP_INTPTR reader) noexcept;
void ChaosTextWriterWrite(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept;
void ChaosTextWriterWriteLine(CHAOS_IL2CPP_INTPTR writer) noexcept;
void ChaosTextWriterWriteLineStr(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept;

// ── Stream.Read/Write/CopyTo ──
CHAOS_IL2CPP_INT32 ChaosStreamRead(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INTPTR buffer, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosStreamWrite(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INTPTR buffer, CHAOS_IL2CPP_INT32 offset, CHAOS_IL2CPP_INT32 count) noexcept;
void ChaosStreamCopyTo(CHAOS_IL2CPP_INTPTR source, CHAOS_IL2CPP_INTPTR destination) noexcept;

// ── BinaryReader ──
void ChaosBinaryReaderCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR stream) noexcept;
CHAOS_IL2CPP_INT32 ChaosBinaryReaderReadInt32(CHAOS_IL2CPP_INTPTR reader) noexcept;
CHAOS_IL2CPP_INTPTR ChaosBinaryReaderReadString(CHAOS_IL2CPP_INTPTR reader) noexcept;
double ChaosBinaryReaderReadDouble(CHAOS_IL2CPP_INTPTR reader) noexcept;

// ── BinaryWriter ──
void ChaosBinaryWriterCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR stream) noexcept;
void ChaosBinaryWriterWriteInt32(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INT32 value) noexcept;
void ChaosBinaryWriterWriteString(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept;