#include "hot_update.h"

#include <fstream>
#include <vector>

namespace chaos::il2cpp::hot_update {

namespace {

template <typename TImage>
bool LoadBinaryImageFromPath(const char* path_utf8, TImage* out_image) {
    if (path_utf8 == nullptr || out_image == nullptr) {
        return false;
    }

    std::ifstream stream(path_utf8, std::ios::binary);
    if (!stream.good()) {
        return false;
    }

    const auto begin = std::istreambuf_iterator<char>(stream);
    const auto end = std::istreambuf_iterator<char>();
    auto* buffer = new std::vector<unsigned char>(begin, end);

    out_image->data = buffer->data();
    out_image->size = buffer->size();
    out_image->owner = buffer;
    return true;
}

template <typename TImage>
void ReleaseBinaryImage(TImage* image) {
    if (image == nullptr) {
        return;
    }

    auto* buffer = static_cast<std::vector<unsigned char>*>(image->owner);
    delete buffer;
    image->data = nullptr;
    image->size = 0u;
    image->owner = nullptr;
}

}  // namespace

bool LoadAssemblyImageFromPath(const char* assembly_path_utf8, HotUpdateAssemblyImage* out_image) {
    return LoadBinaryImageFromPath(assembly_path_utf8, out_image);
}

void ReleaseAssemblyImage(HotUpdateAssemblyImage* image) {
    ReleaseBinaryImage(image);
}

bool LoadSupplementalMetadataFromPath(const char* metadata_path_utf8, SupplementalMetadataImage* out_image) {
    // struct SupplementalMetadataImage is declared in hot_update.h.
    return LoadBinaryImageFromPath(metadata_path_utf8, out_image);
}

void ReleaseSupplementalMetadataImage(SupplementalMetadataImage* image) {
    ReleaseBinaryImage(image);
}

}  // namespace chaos::il2cpp::hot_update
