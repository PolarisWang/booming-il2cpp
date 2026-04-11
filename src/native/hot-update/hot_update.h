#ifndef CHAOS_IL2CPP_HOT_UPDATE_H_
#define CHAOS_IL2CPP_HOT_UPDATE_H_

#include <cstddef>

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

bool LoadAssemblyImageFromPath(const char* assembly_path_utf8, HotUpdateAssemblyImage* out_image);
void ReleaseAssemblyImage(HotUpdateAssemblyImage* image);
bool LoadSupplementalMetadataFromPath(const char* metadata_path_utf8, SupplementalMetadataImage* out_image);
void ReleaseSupplementalMetadataImage(SupplementalMetadataImage* image);

}  // namespace chaos::il2cpp::hot_update

#endif  // CHAOS_IL2CPP_HOT_UPDATE_H_
