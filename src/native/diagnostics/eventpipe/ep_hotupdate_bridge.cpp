// ep_hotupdate_bridge.cpp — EventPipe bridge for HotUpdate diagnostics events

#include "ep_hotupdate_bridge.h"

#if CHAOS_IL2CPP_EVENTPIPE == 1

#include "ep_event.h"
#include <hotupdate_events.h>

namespace chaos::il2cpp::diagnostics {

namespace {

/// Callback registered with HotupdateRegisterEventCallback.
/// Translates HotupdateEvent (with data) to EventPipe emission calls.
void EpHotupdateCallback(HotupdateEvent event,
                          const HotupdateEventData* data,
                          void* /*user_data*/) noexcept {
    if (data == nullptr) return;

    switch (event) {
    case HotupdateEvent::ASSEMBLY_LOADED:
        EpEmitHuAssemblyLoaded(data->module_id, data->alc_id,
                                data->assembly_name);
        break;
    case HotupdateEvent::ASSEMBLY_UNLOADED:
        EpEmitHuAssemblyUnloaded(data->module_id, data->alc_id,
                                  data->assembly_name);
        break;
    case HotupdateEvent::PACKAGE_LOADED:
        EpEmitHuPackageLoaded(data->package_id);
        break;
    case HotupdateEvent::PACKAGE_LOAD_FAILED:
        EpEmitHuPackageLoadFailed(data->package_id, data->error_message);
        break;
    case HotupdateEvent::PACKAGE_UNLOADED:
        EpEmitHuPackageUnloaded(data->package_id);
        break;
    case HotupdateEvent::METHOD_REPLACED:
        EpEmitHuMethodReplaced(data->method_token);
        break;
    case HotupdateEvent::METHOD_REVERTED:
        EpEmitHuMethodReverted(data->method_token);
        break;
    case HotupdateEvent::ERROR:
        EpEmitHuError(data->error_message);
        break;
    default:
        break;
    }
}

}  // anonymous namespace

void EpHotupdateBridgeInitialize() noexcept {
    HotupdateRegisterEventCallback(EpHotupdateCallback, nullptr);
}

void EpHotupdateBridgeShutdown() noexcept {
    // HotupdateEventCallbacks cannot be individually unregistered.
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_EVENTPIPE
