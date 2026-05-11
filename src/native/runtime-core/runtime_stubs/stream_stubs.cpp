// stream_stubs.cpp — Stream/IO stub implementations
#include <chaos/native_types.h>
#include "generated_code_compat.h"

namespace chaos::il2cpp::runtime_core {
extern "C" {

void ChaosStreamFlush(CHAOS_IL2CPP_INTPTR stream) noexcept { (void)stream; }
CHAOS_IL2CPP_INT64 ChaosStreamSeek(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 offset, CHAOS_IL2CPP_INT32 origin) noexcept { (void)stream; (void)offset; (void)origin; return 0; }
void ChaosStreamSetLength(CHAOS_IL2CPP_INTPTR stream, CHAOS_IL2CPP_INT64 value) noexcept { (void)stream; (void)value; }
CHAOS_IL2CPP_INT64 ChaosStreamGetLength(CHAOS_IL2CPP_INTPTR stream) noexcept { (void)stream; return 0; }
CHAOS_IL2CPP_INT64 ChaosStreamGetPosition(CHAOS_IL2CPP_INTPTR stream) noexcept { (void)stream; return 0; }
void ChaosMemoryStreamCtor(CHAOS_IL2CPP_INTPTR instance) noexcept { (void)instance; }
void ChaosStringReaderCtor(CHAOS_IL2CPP_INTPTR instance, CHAOS_IL2CPP_INTPTR input) noexcept { (void)instance; (void)input; }
void ChaosStringWriterCtor(CHAOS_IL2CPP_INTPTR instance) noexcept { (void)instance; }
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadLine(CHAOS_IL2CPP_INTPTR reader) noexcept { (void)reader; return 0; }
CHAOS_IL2CPP_INTPTR ChaosTextReaderReadToEnd(CHAOS_IL2CPP_INTPTR reader) noexcept { (void)reader; return 0; }
void ChaosTextWriterWrite(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept { (void)writer; (void)value; }
void ChaosTextWriterWriteLine(CHAOS_IL2CPP_INTPTR writer) noexcept { (void)writer; }
void ChaosTextWriterWriteLineStr(CHAOS_IL2CPP_INTPTR writer, CHAOS_IL2CPP_INTPTR value) noexcept { (void)writer; (void)value; }

}  // extern "C"
}  // namespace chaos::il2cpp::runtime_core