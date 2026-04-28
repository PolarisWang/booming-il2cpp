#ifndef CHAOS_IL2CPP_HOT_UPDATE_H_
#define CHAOS_IL2CPP_HOT_UPDATE_H_

#include <cstddef>
#include <cstdint>

namespace chaos::il2cpp::hot_update {

struct HotUpdateAssemblyImage {
    const unsigned char* data = nullptr;
    size_t size = 0u;
    void* owner = nullptr;
};

struct SupplementalMetadataImage {
    const unsigned char* data = nullptr;
    size_t size = 0u;
    void* owner = nullptr;
};

struct HotUpdatePackageHandle {
    HotUpdateAssemblyImage assembly_image = {};
    SupplementalMetadataImage metadata_image = {};
    char* package_id = nullptr;
    char* target_aot_version = nullptr;
    char* assembly_name = nullptr;       ///< Duplicated manifest Name, used as MemoryDomain module_name.
    uint32_t domain_id = 0u;             ///< Registered MemoryDomain id (0 = not registered).
    bool loaded = false;
};

bool LoadAssemblyImageFromPath(const char* assembly_path_utf8, HotUpdateAssemblyImage* out_image);
void ReleaseAssemblyImage(HotUpdateAssemblyImage* image);
bool LoadSupplementalMetadataFromPath(const char* metadata_path_utf8, SupplementalMetadataImage* out_image);
void ReleaseSupplementalMetadataImage(SupplementalMetadataImage* image);
bool LoadHotUpdatePackage(const char* package_root_utf8, HotUpdatePackageHandle* out_handle);
void UnloadHotUpdatePackage(HotUpdatePackageHandle* handle);

}  // namespace chaos::il2cpp::hot_update

#endif  // CHAOS_IL2CPP_HOT_UPDATE_H_
