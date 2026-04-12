#include "hot_update.h"

#include <fstream>
#include <string>
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

std::string JoinPath(const char* root_utf8, const char* leaf_utf8) {
    if (root_utf8 == nullptr || leaf_utf8 == nullptr) {
        return {};
    }

    std::string path(root_utf8);
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
        path.push_back('/');
    }

    path.append(leaf_utf8);
    return path;
}

bool ReadTextFile(const std::string& path, std::string* out_contents) {
    if (out_contents == nullptr) {
        return false;
    }

    std::ifstream stream(path, std::ios::binary);
    if (!stream.good()) {
        return false;
    }

    out_contents->assign(
        std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>());
    return true;
}

std::string ExtractJsonString(const std::string& source, const char* key) {
    if (key == nullptr) {
        return {};
    }

    const std::string needle = std::string("\"") + key + "\"";
    const size_t key_offset = source.find(needle);
    if (key_offset == std::string::npos) {
        return {};
    }

    const size_t colon_offset = source.find(':', key_offset + needle.size());
    if (colon_offset == std::string::npos) {
        return {};
    }

    const size_t quote_begin = source.find('"', colon_offset + 1u);
    if (quote_begin == std::string::npos) {
        return {};
    }

    const size_t quote_end = source.find('"', quote_begin + 1u);
    if (quote_end == std::string::npos || quote_end <= quote_begin) {
        return {};
    }

    return source.substr(quote_begin + 1u, quote_end - quote_begin - 1u);
}

char* DuplicateCString(const std::string& value) {
    auto* buffer = new char[value.size() + 1u];
    for (size_t index = 0; index < value.size(); index++) {
        buffer[index] = value[index];
    }

    buffer[value.size()] = '\0';
    return buffer;
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

bool LoadHotUpdatePackage(const char* package_root_utf8, HotUpdatePackageHandle* out_handle) {
    if (package_root_utf8 == nullptr || out_handle == nullptr) {
        return false;
    }

    UnloadHotUpdatePackage(out_handle);

    const std::string manifest_path = JoinPath(package_root_utf8, "package.manifest.json");
    const std::string fallback_manifest_path = JoinPath(package_root_utf8, "manifest.json");
    std::string manifest_contents;
    if (!ReadTextFile(manifest_path, &manifest_contents)
        && !ReadTextFile(fallback_manifest_path, &manifest_contents)) {
        return false;
    }

    const std::string package_id = ExtractJsonString(manifest_contents, "PackageId");
    const std::string target_aot_version = ExtractJsonString(manifest_contents, "TargetAotVersion");
    const std::string assembly_name = ExtractJsonString(manifest_contents, "Name");
    const std::string metadata_name = ExtractJsonString(manifest_contents, "SupplementalMetadata");
    if (package_id.empty() || target_aot_version.empty() || assembly_name.empty() || metadata_name.empty()) {
        return false;
    }

    const std::string assembly_path = JoinPath(package_root_utf8, assembly_name.c_str());
    const std::string metadata_path = JoinPath(package_root_utf8, metadata_name.c_str());
    if (!LoadAssemblyImageFromPath(assembly_path.c_str(), &out_handle->assembly_image)) {
        return false;
    }

    if (!LoadSupplementalMetadataFromPath(metadata_path.c_str(), &out_handle->metadata_image)) {
        ReleaseAssemblyImage(&out_handle->assembly_image);
        return false;
    }

    out_handle->package_id = DuplicateCString(package_id);
    out_handle->target_aot_version = DuplicateCString(target_aot_version);
    out_handle->loaded = true;
    return true;
}

void UnloadHotUpdatePackage(HotUpdatePackageHandle* handle) {
    if (handle == nullptr) {
        return;
    }

    ReleaseAssemblyImage(&handle->assembly_image);
    ReleaseSupplementalMetadataImage(&handle->metadata_image);
    delete[] handle->package_id;
    delete[] handle->target_aot_version;
    handle->package_id = nullptr;
    handle->target_aot_version = nullptr;
    handle->loaded = false;
}

}  // namespace chaos::il2cpp::hot_update
