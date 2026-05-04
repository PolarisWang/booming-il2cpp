#ifndef CHAOS_IL2CPP_HOT_UPDATE_H_
#define CHAOS_IL2CPP_HOT_UPDATE_H_

#include <chaos/native_types.h>
#include <codegen_bridge.h>

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::hot_update {

struct HotUpdateAssemblyImage {
    const unsigned char* data = nullptr;
    CHAOS_IL2CPP_SIZE size = 0u;
    void* owner = nullptr;
};

struct SupplementalMetadataImage {
    const unsigned char* data = nullptr;
    CHAOS_IL2CPP_SIZE size = 0u;
    void* owner = nullptr;
};

struct HotUpdatePackageHandle {
    HotUpdateAssemblyImage assembly_image = {};
    SupplementalMetadataImage metadata_image = {};
    char* package_id = nullptr;
    char* target_aot_version = nullptr;
    char* assembly_name = nullptr;       ///< Duplicated manifest Name, used as MemoryDomain module_name.
    CHAOS_IL2CPP_UINT32 domain_id = 0u;             ///< Registered MemoryDomain id (0 = not registered).
    CHAOS_IL2CPP_UINT32 module_id = 0u;             ///< GenericContextRegistry module id (>0 for hot-update).
    bool loaded = false;
};

/// Register generic type/method instantiations from a hot-update package.
/// Called after LoadHotUpdatePackage when the package provides generic
/// registration data (e.g., from supplemental metadata).  The caller owns
/// the ModuleGenericRegistrationV0 data for the duration of the call.
/// module_id must match the HotUpdatePackageHandle::module_id.
void RegisterHotUpdateModuleGenerics(
    const struct ModuleGenericRegistrationV0* registration_data);

bool LoadAssemblyImageFromPath(const char* assembly_path_utf8, HotUpdateAssemblyImage* out_image);
void ReleaseAssemblyImage(HotUpdateAssemblyImage* image);
bool LoadSupplementalMetadataFromPath(const char* metadata_path_utf8, SupplementalMetadataImage* out_image);
void ReleaseSupplementalMetadataImage(SupplementalMetadataImage* image);
bool LoadHotUpdatePackage(const char* package_root_utf8, HotUpdatePackageHandle* out_handle);
void UnloadHotUpdatePackage(HotUpdatePackageHandle* handle);

}  // namespace chaos::il2cpp::hot_update

#endif  // CHAOS_IL2CPP_HOT_UPDATE_H_
