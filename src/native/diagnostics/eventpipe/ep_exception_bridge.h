// ep_exception_bridge.h — EventPipe exception event bridge
//
// Provides lightweight hook points for the interpreter to emit exception
// events.  These hooks are called from fast_dispatch.cpp (Handle_Throw,
// Handle_Rethrow) and interpreter_vm.cpp (Throw case).
//
// All functions are inline no-ops when EventPipe is not initialized.

#ifndef CHAOS_IL2CPP_EP_EXCEPTION_BRIDGE_H_
#define CHAOS_IL2CPP_EP_EXCEPTION_BRIDGE_H_

#include "ep_config.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_instance.h"
#include "ep_event.h"

#include <cstdint>

namespace chaos::il2cpp::diagnostics {

/// Emit an exception throw event.
/// Called from Handle_Throw and interpreter_vm Throw case.
inline void EpEmitExceptionThrow(const void* exc_obj) noexcept {
    if (EpIsInitialized()) {
        ::chaos::il2cpp::diagnostics::EpEmitExceptionThrow(exc_obj);
    }
}

/// Emit an exception rethrow event.
/// Called from Handle_Rethrow.
inline void EpEmitExceptionRethrow(const void* exc_obj) noexcept {
    if (EpIsInitialized()) {
        ::chaos::il2cpp::diagnostics::EpEmitExceptionRethrow(exc_obj);
    }
}

/// Emit an exception catch event.
/// Called when a catch handler is entered.
inline void EpEmitExceptionCatch(const void* exc_obj) noexcept {
    if (EpIsInitialized()) {
        ::chaos::il2cpp::diagnostics::EpEmitExceptionCatch(exc_obj);
    }
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE

#endif  // CHAOS_IL2CPP_EP_EXCEPTION_BRIDGE_H_
