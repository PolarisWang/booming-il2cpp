// unified_metadata.h — 统一元数据查询接口（面向 VM / 调试器 / OSR）
//
// 为 Phase 2（寄存器式 VM）和 Phase 3（调试器）提供
// 不依赖 metadata_interface.h 内部实现的纯接口声明。

#ifndef CHAOS_UNIFIED_METADATA_H_
#define CHAOS_UNIFIED_METADATA_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ── Opaque handle types ───────────────────────────────────────────────
// Composite key encoding: (module_id << 32) | token
// module_id=0 for AOT root, module_id>0 for hot-update modules.

typedef uint64_t TypeHandle;
typedef uint64_t MethodHandle;
typedef uint64_t FieldHandle;

// ── Module classification flags ───────────────────────────────────────

#define CHAOS_METADATA_SOURCE_AOT      0x01
#define CHAOS_METADATA_SOURCE_HOTPATCH 0x02
#define CHAOS_METADATA_SOURCE_DYNAMIC  0x04
#define CHAOS_METADATA_SOURCE_ANY      0xFF

// ── MetadataRegistry v0 (过程内查询接口) ─────────────────────────────
// 挂载在 CodegenBridgeV0 或通过全局单例访问。

typedef struct UnifiedMetadataRegistryV0 {
    uint32_t abi_version;
    uint32_t struct_size;

    // ── 类型解析 ──
    TypeHandle (*resolve_type)(uint32_t module_id, uint32_t type_token);
    const char* (*get_type_name)(TypeHandle type, const char** out_namespace);

    // ── 方法解析 ──
    MethodHandle (*resolve_method)(uint32_t module_id, uint32_t method_token);
    const char* (*get_method_name)(MethodHandle method);

    // ── 字段解析 ──
    FieldHandle (*resolve_field)(uint32_t module_id, uint32_t field_token);

    // ── 跨模块查找 ──
    // 返回值: (module_index<<32) | token, 0 = not found
    uint64_t (*find_token)(uint32_t method_token);

    // ── 调试器支持 ──
    uint32_t (*enumerate_methods)(
        uint32_t module_id,
        void** out_methods,    // 输出缓冲区
        uint32_t max_count);   // 缓冲区大小

} UnifiedMetadataRegistryV0;

#ifdef __cplusplus
}
#endif

#endif  // CHAOS_UNIFIED_METADATA_H_
