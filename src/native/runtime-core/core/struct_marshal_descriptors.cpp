namespace chaos::il2cpp::runtime_core {
namespace {

struct MarshallingDescriptorEntry {
    CHAOS_IL2CPP_UINT64 stable_id;
    const StructMarshallingDescriptorV1* desc;
};

static constexpr CHAOS_IL2CPP_SIZE kMaxStaticDescriptors = 256;
static MarshallingDescriptorEntry g_static_descriptors[kMaxStaticDescriptors];
static CHAOS_IL2CPP_INT32 g_static_descriptor_count = 0;

static std::unordered_map<CHAOS_IL2CPP_UINT64, std::unique_ptr<StructMarshallingDescriptorV1>> g_runtime_descriptor_cache;
static std::mutex g_runtime_descriptor_mutex;

}  // anonymous namespace

void RegisterStaticMarshallingDescriptor(
    CHAOS_IL2CPP_UINT64 stable_id,
    const StructMarshallingDescriptorV1* desc) noexcept {
    if (desc == nullptr || stable_id == 0) return;
    if (g_static_descriptor_count >= kMaxStaticDescriptors) return;

    for (CHAOS_IL2CPP_INT32 i = 0; i < g_static_descriptor_count; ++i) {
        if (g_static_descriptors[i].stable_id == stable_id) return;
    }

    g_static_descriptors[g_static_descriptor_count].stable_id = stable_id;
    g_static_descriptors[g_static_descriptor_count].desc = desc;
    g_static_descriptor_count++;
}

static const StructMarshallingDescriptorV1* FindStaticDescriptor(CHAOS_IL2CPP_UINT64 stable_id) noexcept {
    for (CHAOS_IL2CPP_INT32 i = 0; i < g_static_descriptor_count; ++i) {
        if (g_static_descriptors[i].stable_id == stable_id)
            return g_static_descriptors[i].desc;
    }
    return nullptr;
}

const StructMarshallingDescriptorV1*
ResolveStructMarshallingDescriptor(const TypeInfoHot* type) noexcept {
    if (type == nullptr || type->stable_id == 0) return nullptr;

    auto* static_desc = FindStaticDescriptor(type->stable_id);
    if (static_desc != nullptr) return static_desc;

    {
        std::lock_guard<std::mutex> lock(g_runtime_descriptor_mutex);
        auto it = g_runtime_descriptor_cache.find(type->stable_id);
        if (it != g_runtime_descriptor_cache.end())
            return it->second.get();
    }

    return nullptr;
}

}  // namespace chaos::il2cpp::runtime_core