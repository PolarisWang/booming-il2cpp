namespace chaos::il2cpp::runtime_core {
namespace {

// Dynamic registry for codegen-emitted static StructMarshallingDescriptorV1 entries.
// Replaces the earlier fixed-size (256) array with no upper limit, supporting
// 200+ patch modules without silent descriptor loss.
static CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(CHAOS_IL2CPP_UINT64, const StructMarshallingDescriptorV1*)
    g_static_descriptors;

// Runtime-constructed descriptors (reflection fallback), protected by mutex.
static CHAOS_IL2CPP_UNORDERED_DENSE_MAP(CHAOS_IL2CPP_UINT64, std::unique_ptr<StructMarshallingDescriptorV1>)
    g_runtime_descriptor_cache;
static std::mutex g_runtime_descriptor_mutex;

}  // anonymous namespace

void RegisterStaticMarshallingDescriptor(
    CHAOS_IL2CPP_UINT64 stable_id,
    const StructMarshallingDescriptorV1* desc) noexcept {
    if (desc == nullptr || stable_id == 0) return;

    // Insert if absent (deduplication).
    g_static_descriptors.try_emplace(stable_id, desc);
}

const StructMarshallingDescriptorV1*
ResolveStructMarshallingDescriptor(const TypeInfoHot* type) noexcept {
    if (type == nullptr || type->stable_id == 0) return nullptr;

    // 1. Try static (codegen-emitted) descriptors — O(1) hash lookup.
    {
        auto it = g_static_descriptors.find(type->stable_id);
        if (it != g_static_descriptors.end())
            return it->second;
    }

    // 2. Try runtime-constructed descriptor cache.
    {
        std::lock_guard<std::mutex> lock(g_runtime_descriptor_mutex);
        auto it = g_runtime_descriptor_cache.find(type->stable_id);
        if (it != g_runtime_descriptor_cache.end())
            return it->second.get();
    }

    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core
