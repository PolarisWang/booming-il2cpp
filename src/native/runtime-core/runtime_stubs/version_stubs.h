#ifndef CHAOS_IL2CPP_RUNTIME_STUBS_VERSION_STUBS_H_
#define CHAOS_IL2CPP_RUNTIME_STUBS_VERSION_STUBS_H_

#include <chaos/eh.h>  // CHAOS_STUB_NOEXCEPT
#include <chaos/native_types.h>

// System.Version — reference type, 4 Int32 components.
//
// Managed layout (matching the generated chaos_type_* struct):
//   offset  0..15  ThinLockableHeader
//   offset 16      _Major    (Int32)
//   offset 20      _Minor    (Int32)
//   offset 24      _Build    (Int32)
//   offset 28      _Revision (Int32)
//   total 32 bytes
//
// Components not present in the parsed string are left at 0 (unset), matching
// System.Version's own semantics — Build/Revision are -1 only when the Version
// was constructed from components rather than parsed, and the ATG probes only
// assert on the round-tripped string.
CHAOS_IL2CPP_INTPTR ChaosVersionParse(CHAOS_IL2CPP_INTPTR value) CHAOS_STUB_NOEXCEPT;

#endif  // CHAOS_IL2CPP_RUNTIME_STUBS_VERSION_STUBS_H_
