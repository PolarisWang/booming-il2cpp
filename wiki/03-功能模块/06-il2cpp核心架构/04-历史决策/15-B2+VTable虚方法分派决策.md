# B2+ VTable 虚方法分派方案决策

## 背景

原有的虚方法分派基于 if-else 链比较 `TypeInfo*` 指针：

```cpp
if (type_info == &TypeA) { call A_impl; }
else if (type_info == &TypeB) { call B_impl; }
// parent walk fallback...
```

此方案存在三个根本问题：
1. **性能非最优**：O(N) 比较链，随类型数量线性增长，I-cache 污染严重
2. **代码膨胀**：每个虚调用点生成完整的分支树
3. **无法优化**：间接调用、inline cache 等优化手段无法应用

## 方案对比

### B1: Global Slot VTable

全局唯一 slot 编号，所有类型共享同一套编号空间。

- 优点：slot 编号可直接用于数组索引
- 缺点：需全局协调编号（跨 TU 一致性问题）；HotUpdate 需预留 slot（浪费）；接口方法 slot 浪费更大

### B2: Hierarchy Slot VTable

每个继承链独立编号。

- 优点：自动编号，无跨 TU 问题；slot 紧凑无浪费
- 缺点：接口方法仍需独立处理

### B2+: Hierarchy VTable + Interface Offset Map

在 B2 基础上增加 per-type iface_map。

- 优点：B2 全部优点 + 接口方法分派 via iface_map；`stable_id` 匹配支持跨模块 HotUpdate

## 决策结论

选择 **B2+** 方案。核心理由：

1. **性能最优（第一优先级）**：O(1) 虚方法分派，间接 call 的 BTB 预测优于分支树
2. **方案完美性（第二优先级）**：slot 自动分配消除跨 TU 不一致，iface_map 提供统一的接口分派路径
3. **HotUpdate 支持（第三优先级）**：动态类型在同继承链追加 slot，无需预留；`stable_id` 支持跨模块接口匹配

### 核心设计

**Per-Object VTable Pointer：**

```cpp
struct chaos_object_header {
    const void**        vtable;      // [新增] vtable pointer
    const TypeInfo*     type_info;   // 类型身份（不变）
};
// sizeof = 16 bytes (原 8 bytes)
```

对象头增加 8 字节。TypeInfo 保持 `inline constexpr` 不变。

**VTable 数组（codegen 发射）：**

```cpp
static const void* chaos_vtable_TypeX[] = { /* 函数指针数组 */ };
obj->header.vtable = chaos_vtable_TypeX;
```

**Interface Dispatch via iface_map：**

```cpp
struct InterfaceMapEntry {
    CHAOS_IL2CPP_UINT64 iface_stable_id;
    CHAOS_IL2CPP_UINT32 vtable_offset;
    CHAOS_IL2CPP_UINT32 method_count;
};
// 线性扫描（接口数 < 10），binary search 为将来优化保留
```

### 与 B1 / Unity IL2CPP 横向对比

| 维度 | B1 | B2+ | Unity IL2CPP |
|------|-----|-----|--------------|
| Slot 范围 | 全局唯一 | 继承链内编号 | 继承链内编号 |
| 接口 dispatch | 全局 iface offset | iface_map linear scan | iface_map binary search |
| HotUpdate | ❌ 需预留 slot | ✅ 追加新 slot | ⚠️ 预留/重建 |
| Dispatch 延迟 | O(depth) | O(1) indirect call | O(1) indirect call |

### TypeInfo 约束

TypeInfo 保持 `inline constexpr`，不添加 vtable/iface_map 字段。vtable 指针放在对象头以最小化 dispatch 路径的间接层数。

## 迁移计划

| Phase | 内容 | 文件 |
|-------|------|------|
| 1 | 对象头 vtable + virtual dispatch 迁移 | ObjectModelEmission, MethodEmission, generated_code_compat |
| 2 | Interface dispatch 迁移 | iface_map codegen |
| 3 | AOT 去虚化优化 | 编译期类型分析 |
| 4 | HotUpdate vtable 支持 | 运行时 API |

## 参考

- 架构文档: `01-翻译管线/14-VTable注册表.md`
- Slot 分配算法: `src/managed/Chaos.IL2CPP.CodeGen/Planning/VTableSlotPlanner.cs`
- vtable_registry (旧系统): `src/native/runtime-core/vtable_registry.*`
