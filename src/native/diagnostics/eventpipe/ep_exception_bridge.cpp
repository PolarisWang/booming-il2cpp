// ep_exception_bridge.cpp — EventPipe exception bridge (stub)
//
// The actual emission logic is inline in ep_exception_bridge.h for zero
// overhead when EventPipe is not initialized.  This file exists for
// build system completeness and future initialization hooks.

#include "ep_exception_bridge.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

namespace chaos::il2cpp::diagnostics {

// Exception emission is handled via inline functions in the header.
// No additional implementation needed at this layer.

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
