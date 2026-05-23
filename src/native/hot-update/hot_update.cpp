#include "hot_update.h"

#include "generic_context.h"
#include "memory_domain.h"
#include "metadata_interface.h"
#include "string_table.h"

#include <atomic>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace chaos::il2cpp::hot_update {

namespace {

template <typename TImage>
bool LoadBinaryImageFromPath(const char* path_utf8, TImage* out_image) {
    if (path_utf8 == nullptr || out_image == nullptr) {
        return false;
    }

    CHAOS_IL2CPP_IFSTREAM stream(path_utf8, CHAOS_IL2CPP_IOS_BINARY);
    if (!stream.good()) {
        return false;
    }

    const auto begin = CHAOS_IL2CPP_ISTREAMBUF_ITERATOR(char)(stream);
    const auto end = CHAOS_IL2CPP_ISTREAMBUF_ITERATOR(char)();
    auto* buffer = new CHAOS_IL2CPP_VECTOR(unsigned char)(begin, end);

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

    auto* buffer = static_cast<CHAOS_IL2CPP_VECTOR(unsigned char)*>(image->owner);
    delete buffer;
    image->data = nullptr;
    image->size = 0u;
    image->owner = nullptr;
}

CHAOS_IL2CPP_STRING JoinPath(const char* root_utf8, const char* leaf_utf8) {
    if (root_utf8 == nullptr || leaf_utf8 == nullptr) {
        return {};
    }

    CHAOS_IL2CPP_STRING path(root_utf8);
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
        path.push_back('/');
    }

    path.append(leaf_utf8);
    return path;
}

bool ReadTextFile(const CHAOS_IL2CPP_STRING& path, CHAOS_IL2CPP_STRING* out_contents) {
    if (out_contents == nullptr) {
        return false;
    }

    CHAOS_IL2CPP_IFSTREAM stream(path, CHAOS_IL2CPP_IOS_BINARY);
    if (!stream.good()) {
        return false;
    }

    out_contents->assign(
        CHAOS_IL2CPP_ISTREAMBUF_ITERATOR(char)(stream),
        CHAOS_IL2CPP_ISTREAMBUF_ITERATOR(char)());
    return true;
}

CHAOS_IL2CPP_STRING ExtractJsonString(const CHAOS_IL2CPP_STRING& source, const char* key) {
    if (key == nullptr) {
        return {};
    }

    const CHAOS_IL2CPP_STRING needle = CHAOS_IL2CPP_STRING("\"") + key + "\"";
    const CHAOS_IL2CPP_SIZE key_offset = source.find(needle);
    if (key_offset == CHAOS_IL2CPP_STRING::npos) {
        return {};
    }

    const CHAOS_IL2CPP_SIZE colon_offset = source.find(':', key_offset + needle.size());
    if (colon_offset == CHAOS_IL2CPP_STRING::npos) {
        return {};
    }

    const CHAOS_IL2CPP_SIZE quote_begin = source.find('"', colon_offset + 1u);
    if (quote_begin == CHAOS_IL2CPP_STRING::npos) {
        return {};
    }

    const CHAOS_IL2CPP_SIZE quote_end = source.find('"', quote_begin + 1u);
    if (quote_end == CHAOS_IL2CPP_STRING::npos || quote_end <= quote_begin) {
        return {};
    }

    return source.substr(quote_begin + 1u, quote_end - quote_begin - 1u);
}

char* DuplicateCString(const CHAOS_IL2CPP_STRING& value) {
    auto* buffer = static_cast<char*>(CHAOS_IL2CPP_MALLOC(value.size() + 1u));
    if (buffer == nullptr) return nullptr;
    for (CHAOS_IL2CPP_SIZE index = 0; index < value.size(); index++) {
        buffer[index] = value[index];
    }

    buffer[value.size()] = '\0';
    return buffer;
}

}  // namespace

// ── Hot-update generic registration ───────────────────────────────────────

void RegisterHotUpdateModuleGenerics(
    const ModuleGenericRegistrationV0* registration_data)
{
    if (registration_data == nullptr) {
        return;
    }

    // The caller is responsible for providing:
    //   - registration_data->module_id  (matching HotUpdatePackageHandle::module_id)
    //   - registration_data->source_image (for token→handle resolution)
    // If source_image is nullptr, tokens are stored as opaque handles
    // (MakeOpaqueHandle-style) and will be resolved lazily.
    using chaos::il2cpp::runtime_core::ModuleLifecycleManager;
    ModuleLifecycleManager::Get()->RegisterHotUpdateGenerics(
        registration_data->module_id, registration_data);
}

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

// ── Incremental update version registry ────────────────────────────────────
// Tracks loaded packages by package_id → target_aot_version.
// Enables idempotent reload: if the same or newer version is already loaded,
// the load is skipped.  This is a placeholder for full incremental update.
std::mutex g_version_registry_mutex;
std::unordered_map<CHAOS_IL2CPP_STRING, CHAOS_IL2CPP_STRING> g_version_registry;

bool LoadHotUpdatePackage(const char* package_root_utf8, HotUpdatePackageHandle* out_handle) {
    if (package_root_utf8 == nullptr || out_handle == nullptr) {
        return false;
    }

    // ── Incremental update: version check ──────────────────────────────
    // Check if a package with the same ID is already loaded.
    // Also extract the manifest version to compare.
    const CHAOS_IL2CPP_STRING manifest_path = JoinPath(package_root_utf8, "package.manifest.json");
    const CHAOS_IL2CPP_STRING fallback_manifest_path = JoinPath(package_root_utf8, "manifest.json");
    CHAOS_IL2CPP_STRING manifest_contents;
    if (!ReadTextFile(manifest_path, &manifest_contents)
        && !ReadTextFile(fallback_manifest_path, &manifest_contents)) {
        return false;
    }

    const CHAOS_IL2CPP_STRING package_id = ExtractJsonString(manifest_contents, "PackageId");
    const CHAOS_IL2CPP_STRING target_aot_version = ExtractJsonString(manifest_contents, "TargetAotVersion");
    const CHAOS_IL2CPP_STRING assembly_name = ExtractJsonString(manifest_contents, "Name");
    const CHAOS_IL2CPP_STRING metadata_name = ExtractJsonString(manifest_contents, "SupplementalMetadata");
    if (package_id.empty() || target_aot_version.empty() || assembly_name.empty() || metadata_name.empty()) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_version_registry_mutex);
        auto it = g_version_registry.find(package_id);
        if (it != g_version_registry.end()) {
            if (it->second >= target_aot_version) {
                CHAOS_IL2CPP_LOG_INFO_M("HotUpdate",
                    "Incremental: package '{}' version '{}' already loaded (requested '{}'), skipping",
                    package_id, it->second, target_aot_version);
                return false;
            }
        }
    }

    UnloadHotUpdatePackage(out_handle);

    const CHAOS_IL2CPP_STRING assembly_path = JoinPath(package_root_utf8, assembly_name.c_str());
    const CHAOS_IL2CPP_STRING metadata_path = JoinPath(package_root_utf8, metadata_name.c_str());
    if (!LoadAssemblyImageFromPath(assembly_path.c_str(), &out_handle->assembly_image)) {
        return false;
    }

    if (!LoadSupplementalMetadataFromPath(metadata_path.c_str(), &out_handle->metadata_image)) {
        ReleaseAssemblyImage(&out_handle->assembly_image);
        return false;
    }

    out_handle->package_id = DuplicateCString(package_id);
    out_handle->target_aot_version = DuplicateCString(target_aot_version);
    out_handle->assembly_name = DuplicateCString(assembly_name);

    // Register the package version in the incremental update registry.
    {
        std::lock_guard<std::mutex> lock(g_version_registry_mutex);
        g_version_registry[package_id] = target_aot_version;
    }

    // Register the hot-update module in the ModuleRegistry so reflection
    // queries (GetTypeByName, GetTypeFromHandle) can find its types.
    // A minimal ModuleDescriptor is registered with just the assembly name;
    // type_names / type_flags / type_info_ptrs / image will be populated
    // from supplemental metadata in a later phase.
    {
        using chaos::il2cpp::runtime_core::ModuleDescriptor;
        using chaos::il2cpp::runtime_core::RegisterModule;
        using chaos::il2cpp::runtime_core::kInvalidModuleId;

        ModuleDescriptor hu_desc = {};
        hu_desc.name_utf8 = out_handle->assembly_name;
        // type_count = 0, all other pointers = nullptr (minimal registration)
        uint32_t mod_id = RegisterModule(out_handle->assembly_name, &hu_desc);
        if (mod_id == kInvalidModuleId) {
            CHAOS_IL2CPP_LOG_ERROR_M("HotUpdate",
                "LoadHotUpdatePackage: RegisterModule failed for '{0}'",
                out_handle->assembly_name ? out_handle->assembly_name : "(null)");
            ReleaseAssemblyImage(&out_handle->assembly_image);
            ReleaseSupplementalMetadataImage(&out_handle->metadata_image);
            g_version_registry.erase(package_id);
            return false;
        }
        out_handle->module_id = mod_id;
    }
    out_handle->loaded = true;

    // Register a per-package memory domain so marshal allocations during
    // hot-update method execution are attributed to the correct domain.
    // On package unload, UnregisterMemoryDomain destroys the domain heap,
    // releasing all marshal allocations for this package at once.
    namespace md = chaos::il2cpp::memory_domain;
    md::DomainInit domain_init = {};
    domain_init.module_name = out_handle->assembly_name;
    domain_init.module_kind = 1u;  // HotUpdate
    domain_init.usage_limit = 0;
    domain_init.runtime_config = nullptr;
    out_handle->domain_id = md::RegisterMemoryDomain(domain_init);
    return true;
}

void UnloadHotUpdatePackage(HotUpdatePackageHandle* handle) {
    if (handle == nullptr) {
        return;
    }

    // Unregister generic instantiations before releasing memory resources.
    if (handle->module_id != 0u) {
        using chaos::il2cpp::runtime_core::ModuleLifecycleManager;
        using chaos::il2cpp::runtime_core::MarkModuleTombstone;
        ModuleLifecycleManager::Get()->UnregisterHotUpdateGenerics(handle->module_id);
        MarkModuleTombstone(handle->module_id);
        handle->module_id = 0u;
    }

    // Unregister the memory domain next, which destroys the domain heap
    // and releases all marshal allocations attributed to this package.
    if (handle->domain_id != 0u) {
        chaos::il2cpp::string_table::UnregisterDomain(handle->domain_id);
        chaos::il2cpp::memory_domain::UnregisterMemoryDomain(handle->domain_id);
        handle->domain_id = 0u;
    }

    ReleaseAssemblyImage(&handle->assembly_image);
    ReleaseSupplementalMetadataImage(&handle->metadata_image);

    // Remove from version registry.
    if (handle->package_id != nullptr) {
        std::lock_guard<std::mutex> lock(g_version_registry_mutex);
        g_version_registry.erase(CHAOS_IL2CPP_STRING(handle->package_id));
    }

    delete[] handle->package_id;
    delete[] handle->target_aot_version;
    delete[] handle->assembly_name;
    handle->package_id = nullptr;
    handle->target_aot_version = nullptr;
    handle->assembly_name = nullptr;
    handle->loaded = false;
}

}  // namespace chaos::il2cpp::hot_update
