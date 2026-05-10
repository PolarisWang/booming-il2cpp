// aot_core_ir_reader.h — Deserialize AotCoreIr JSON into IRMethod for interpreter execution.
//
// The managed codegen produces aot-core-ir.json
// with per-method instruction data including numeric OpCode fields. This reader
// deserializes that JSON into the C++ IRMethod struct consumed by InterpreterVM.
//
// The resolve_subject_id callback maps subject IDs (e.g. "System.Console::WriteLine")
// to call_target pointers (MethodInfoHandle or bridge dispatch target).

#ifndef CHAOS_IL2CPP_AOT_CORE_IR_READER_H_
#define CHAOS_IL2CPP_AOT_CORE_IR_READER_H_

#include "interpreter_vm.h"
#include <cstdint>
#include <cstddef>

namespace chaos::il2cpp::runtime_core {

// Callback type for resolving subject IDs to call_target pointers.
// Returns nullptr if the subject ID cannot be resolved.
using ResolveSubjectIdFn = void* (*)(const char* subject_id, void* user_data);

// Deserialize an AotCoreIr method JSON string into an IRMethod.
//
// json:      The JSON string for a single AotCoreIrMethodArtifact
// length:    Length of the JSON string in bytes
// resolve_fn:  Callback to resolve subject IDs to call_target pointers
//              (may be nullptr if no call_target resolution is needed)
// resolve_ctx: User data passed through to resolve_fn
//
// Returns an IRMethod with heap-allocated instructions and SEH clauses.
// The caller must free both instructions and seh_clauses vectors.
//
// On parse failure, returns an IRMethod containing a single Ret instruction.
interpreter::IRMethod DeserializeAotCoreIrMethod(
    const char* json,
    size_t length,
    ResolveSubjectIdFn resolve_fn,
    void* resolve_ctx);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_AOT_CORE_IR_READER_H_