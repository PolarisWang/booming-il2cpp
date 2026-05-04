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

## 实施状态（截至 2026-05-04）

| 组件 | 状态 | 位置 |
|------|------|------|
| Per-Object Header vtable | ✅ 已实现 | `generated_code_compat.h` |
| VTable slot 分配算法 | ✅ 已实现 | `ObjectModelEmission.cs:534-563` |
| VTable 数组发射 | ✅ 已实现 (static const void* chaos_vtable_TypeX[]) | `ObjectModelEmission.cs:566-627` |
| VTable 赋值到对象 | ✅ 已实现 (new/反射/异常等路径) | `ExceptionEmission.cs`, `ReflectionObjectEmission.cs`, `NativeAotLoweringPlanner.cs` |
| chaos_vtable_resolve 辅助函数 | ✅ 已实现 | `ObjectModelEmission.cs:629-635` |
| Virtual dispatch (MethodEmission) | ✅ 已实现 (vtable[slot]) | `MethodEmission.cs:986-1101` |
| Interface dispatch (iface_map) | ✅ 已实现 (InterfaceMapEntry + vtable_offset) | `ObjectModelEmission.cs`, `MethodEmission.cs` |
| AOT 去虚化 | ✅ 已实现 (monomorphic + sealed class) | `InvocationPlanning.cs:412-442`, `MethodEmission.cs:1030-1051` |
| HotUpdate vtable 支持 | ❌ 待实现 (Phase 3) | 运行时 API |

## 迁移计划（更新版）

| Phase | 内容 | 文件 | 状态 |
|-------|------|------|------|
| 1 | Virtual dispatch: if-else → vtable[slot] | `MethodEmission.cs:986-1086` | **✅ 完成** (2026-05-04) |
| 2 | Interface dispatch: iface_map → InterfaceMapEntry | `ObjectModelEmission.cs`, `TypeInfo`, `type_registry.cpp` | **✅ 完成** (2026-05-04) |
| 3 | AOT 去虚化扩展 | codegen 静态分析 | **✅ Phase 3a 完成** (2026-05-04) |
| 4 | HotUpdate vtable 支持 | 运行时 API | 待规划 |

## Phase 1 实施记录 (2026-05-04)

### 变更内容

**`NativeAotLoweringPlanner.MethodEmission.cs`** — `EmitVirtualDispatchCall` 方法（行 986-1086）：

**删除：**
- `ResolveVirtualDispatchRoutes()` 调用及其 `readOnlyList` 变量（原行 989-993）— 不再需要路由解析
- 整个 TypeInfo* if-else 比较链（原行 1057-1161，约 105 行），包含：
  - Exact type match foreach 循环（遍历所有 VirtualDispatchRoute 生成 if/else-if 链）
  - Parent chain walk（`while (chaos_current_type_info != nullptr)` 遍历父类型）
  - `CHAOS_IL2CPP_ABORT()` 回退

**新增：**
- VTable slot 查找（行 1052-1073）：
  - 通过 `GetMethodSignatureSuffix()` + `_vtableSlotMap` 获取 slot index
  - 生成 `reinterpret_cast<fn_type>(chaos_header->vtable[slot])(args)` 间接调用
  - 找不到 slot 时 `CHAOS_IL2CPP_ABORT()` 回退

### 生成的 C++ 代码对比

```cpp
// BEFORE (O(N) if-else chain):
if (chaos_header->type_info == &chaos_type_info_A ||
    chaos_header->type_info->stable_id == (&chaos_type_info_A)->stable_id) {
    A_Method(chaos_instance, chaos_arg_1, ...);
    goto chaos_callvirt_done_42;
}
// ... N more type checks ...
// parent chain walk fallback ...

// AFTER (O(1) vtable dispatch):
auto fn = reinterpret_cast<void(*)(CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INT32)>(
    chaos_header->vtable[3]);
fn(chaos_instance, chaos_arg_1, ...);
```

### 验证
- `dotnet build` — 0 errors, 89 warnings (pre-existing)
- `batch_native_aot_runner.py` — 29/33 PASS (4 pre-existing failures)
- interface-dispatch family — PASS (关键回归测试)

## Phase 2 实施记录 (2026-05-04)

### 变更内容

**`src/native/common/chaos/type_info.h`** — InterfaceMapEntry 结构体定义：

```cpp
struct InterfaceMapEntry {
    CHAOS_IL2CPP_UINT64 iface_stable_id;    // FNV-1a hash of interface subject ID
    CHAOS_IL2CPP_UINT32 vtable_offset;      // starting vtable slot index
    CHAOS_IL2CPP_UINT32 method_count;       // number of methods in this interface
};
```

- TypeInfo::iface_map 类型从 `const uint64*` 改为 `const InterfaceMapEntry*`
- `chaos_register_type()` 参数更新以匹配新类型

**`src/native/runtime-core/type_registry.cpp`** — 修复 iface_map 未初始化的问题：
- 原实现未保存 iface_map 到 TypeInfo（遗漏 `ti->iface_map = iface_map; ti->iface_count = iface_count;`）

**`NativeAotLoweringPlanner.ObjectModelEmission.cs`** — vtable slot 分配提前 + iface_map 发射：

1. **VTable slot 分配提前到 iface_map 发射前**（需要 `_vtableSlotMap` 来计算 `vtable_offset`）
2. **新增 `ComputeInterfaceVtableInfo()`** 辅助方法：
   - 遍历接口的非静态方法，通过 `_vtableSlotMap` 查找全局 slot
   - 返回 `(vtableOffset = min(slots), methodCount = slots.Count)`
3. **InterfaceMapEntry 数组发射**，替换旧的 `uint64` 数组：
   ```cpp
   static constexpr InterfaceMapEntry chaos_iface_map_TypeX[] = {
       { stable_id, vtable_offset, method_count },
       // ...
   };
   ```
4. **新增 `chaos_find_interface_offset()`** C++ 运行时函数：
   - 线性扫描 TypeInfo::iface_map，按 stable_id 匹配，返回 vtable_offset

**`NativeAotLoweringPlanner.MethodEmission.cs`** — Interface dispatch：

- `EmitVirtualDispatchCall()` 新增接口分派路径：
  - 检测 `DeclaringTypeSubjectId` 是否为接口类型（通过 `_interfaceTypeSubjectIds`）
  - 接口方法通过 `chaos_find_interface_offset()` 获取 vtable 基偏移
  - `methodIndex = vtableSlot - ifaceVtableOffset`（编译时计算）
  - 生成 `chaos_header->vtable[chaos_iface_offset + methodIndex](args)` 间接调用
  - 非接口方法保留原有 `vtable[slot]` 路径

**`NativeAotLoweringPlanner.cs`** — 新增 `_interfaceTypeSubjectIds` 字段：
- 在 `EmitObjectModelDeclarations` 结束时保存接口类型集合
- 供 `EmitVirtualDispatchCall` 判断调用目标是否为接口方法

### 验证
- `dotnet build Chaos.IL2CPP.CodeGen.csproj` — 0 errors, 89 warnings (pre-existing)

## 参考

- 架构文档: `01-翻译管线/14-VTable注册表.md`
- Slot 分配算法: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` (行 534-563)
- VTable 数组发射: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` (行 566-627)
- Virtual dispatch codegen: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` (行 986-1176)
- devirtualization hint: `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs` (行 412-426)
- vtable_registry (旧系统): `src/native/runtime-core/vtable_registry.*`
