# VTable 虚方法分派架构 (A4-Dual+V2)

## 核心职责

虚方法与接口方法的分派中心。A4-Dual+V2 方案使用 **TypeInfo->vtable_array** 统一 vtable 访问，
虚调用通过 `type_info->vtable_array[slot_index]` 实现 O(1) 间接分派。

## 设计目标

| 目标 | 说明 |
|------|------|
| O(1) 虚方法分派 | 无论继承链深度，虚调用开销恒定 |
| 自动 Slot 编号 | 每继承链独立，无需全局协调 |
| 接口方法分派 | iface_map + vtable 偏移，O(N) 线性扫描（接口数通常 < 10）|
| AOT 去虚化 | 已知接收者类型时发射 direct call，跳过 vtable |
| HotUpdate 兼容 | 动态类型可继承 AOT 类型的 vtable 布局 |

## 核心架构

### 对象头 (A4-Dual+V2)

从 A4-Dual+V2 起，对象头按类型特征分为三种 variant，只有 Fat 类型携带 vtable 指针：

```cpp
// PureType (8B) — 无虚方法、无同步
struct PureTypeHeader {
    const TypeInfo* type_info = nullptr;
};

// ThinLockable (16B) — 无虚方法、有同步
struct ThinLockableHeader {
    const TypeInfo* type_info   = nullptr;
    uint64_t        sync_state  = 0;
};

// Fat (24B) — 有虚方法 + 同步
struct FatHeader {
    const TypeInfo* type_info   = nullptr;  // [0]
    const void**    vtable      = nullptr;  // [8] — vtable 指针
    uint64_t        sync_state  = 0;        // [16]
};
```

三种 variant 的 `TypeInfo*` 均在偏移 [0]，统一访问：

```cpp
inline const TypeInfo* chaos_object_get_type_info(const void* obj) noexcept {
    return *static_cast<const TypeInfo* const*>(obj);
}
```

### vtable 访问路径

虚调用不再通过 `obj->header.vtable[slot]`，而是通过 TypeInfo 间接：

```cpp
// A4-Dual+V2: 统一 vtable_array 访问
const auto* ti = chaos_object_get_type_info(obj);
auto fn = reinterpret_cast<void(*)(void*)>(ti->vtable_array[slot]);
fn(obj);
```

| Variant | vtable 获取方式 | 解引用次数 | 说明 |
|---------|---------------|-----------|------|
| Fat | `obj->vtable[slot]` | 2 次（obj→vtable, vtable[slot]） | vtable 指针在头部 |
| V2 (≤6 slots) | `ti->vtable_array[slot]` | 2 次（obj→ti, ti→inline_slots[slot]） | vtable_array 自指 &inline_slots[0] |
| V1 (>6 slots) | `ti->vtable_array[slot]` | 2 次（obj→ti, ti→vtable_array[slot]） | vtable_array 指向独立数组 |

对于 Fat 类型，vtable 指针仍在头部（可直接读取），返回 FatHeader 布局与 codegen 兼容。

### TypeInfo（带 vtable_array + iface_map）

```cpp
struct TypeInfo {
    const TypeInfo* parent;                  // base type (nullptr for Object)
    CHAOS_IL2CPP_UINT64 stable_id;           // FNV-1a hash
    const InterfaceMapEntry* iface_map;      // AOT compile-time iface_map
    const InterfaceMapEntry* runtime_iface_map; // HotUpdate-追加的接口映射 (heap allocated)
    CHAOS_IL2CPP_UINT32 iface_count;         // AOT iface count
    CHAOS_IL2CPP_UINT32 runtime_iface_count; // HotUpdate 追加数
    CHAOS_IL2CPP_UINT8  type_shape;          // reference/value/interface
    CHAOS_IL2CPP_UINT8  flags;               // bit[0:1]=header_kind, bit[2]=has_finalizer
    // 2 bytes padding
    const void**        vtable_array;        // 统一 vtable 访问指针 (V2 自指 &inline_slots[0])
    CHAOS_IL2CPP_UINT32 vtable_length;       // vtable slot 数
    // 4 bytes padding
};
// sizeof = 64 bytes
```

### TypeInfoV2 — 内联 vtable

≤6 个虚方法的类型使用 TypeInfoV2，vtable 槽内联在 TypeInfo 尾部，同 cache line：

```cpp
struct TypeInfoV2 {
    TypeInfo                base;            // 64B (vtable_array = &inline_slots[0])
    const void*             inline_slots[6]; // 48B: 内联 vtable 槽
};
// sizeof = 112 bytes
```

### VTable 数组

Fat 类型（>6 虚方法或无法确定）在生成的 .cpp 中有 `static const void*` 数组：

```cpp
// 每类型的虚函数表（codegen 发射）
static const void* chaos_vtable_System_String[] = {
    reinterpret_cast<void*>(&String_Equals),
    reinterpret_cast<void*>(&String_GetHashCode),
    reinterpret_cast<void*>(&String_ToString),
    reinterpret_cast<void*>(&Object_Finalize),
    reinterpret_cast<void*>(&String_get_Length),
};

// TypeInfo 初始化
inline TypeInfo chaos_type_info_System_String = {
    /*parent=*/       &chaos_type_info_System_Object,
    /*stable_id=*/    CHAOS_STABLE_ID_System_String,
    /*iface_map=*/    chaos_iface_map_System_String,
    /*runtime_iface=*/nullptr,
    /*iface_count=*/  CHAOS_IFACE_COUNT_System_String,
    /*runtime_count=*/0,
    /*type_shape=*/   1,
    /*flags=*/        kTypeInfoHeaderKindFat,  // Fat 类型
    /*vtable_array=*/ chaos_vtable_System_String,
    /*vtable_length=*/5,
};

// 对象创建时赋值
reinterpret_cast<FatHeader*>(obj)->type_info = &chaos_type_info_System_String;
reinterpret_cast<FatHeader*>(obj)->vtable = chaos_vtable_System_String;
```

V2 类型的 vtable_array 自指到 inline_slots[0]，不分配独立数组。

### Slot 分配算法

每个继承链独立编号。slot index = 方法在链中首次声明的序号：

```
Object                           [0]Equals [1]GetHashCode [2]ToString [3]Finalize
String : Object                  [0]Equals★ [1]GetHashCode★ [2]ToString★ [3]Finalize [4]get_Length
StringBuilder : Object           [0]Equals  [1]GetHashCode★ [2]ToString★ [3]Finalize [4]Capacity [5]Append
                                 (★ = override, slot index 继承自 parent 声明)
```

规则：
- 每个 virtual method 由 declaring type 决定 slot index
- Override 复用 parent 的 slot index
- New virtual 分配下一个可用 slot

### Virtual Dispatch Codegen

**A4-Dual+V2 — 统一 vtable_array 访问 (O(1))：**

```cpp
const auto* chaos_ti = chaos_object_get_type_info(chaos_instance);
auto fn = reinterpret_cast<void(*)(void*)>(chaos_ti->vtable_array[3]);
fn(chaos_instance);
```

生成的指令：1-2 条 load（obj→ti, ti→vtable_array[slot]）+ 1 条 indirect call。

对于 Fat 类型（header 直接携带 vtable 指针），也可以直接读取头部 vtable，
但统一使用 vtable_array 路径使 codegen 不分叉。

### Interface Dispatch

**接口方法 slot 编号** — 每个接口内方法从 0 编号：

```
IComparable:    [0]CompareTo
IDisposable:    [0]Dispose
```

**InterfaceMapEntry：**

```cpp
struct InterfaceMapEntry {
    CHAOS_IL2CPP_UINT64 iface_stable_id;  // 接口 stable_id（非指针，支持跨模块）
    CHAOS_IL2CPP_UINT32 vtable_offset;    // 该接口方法在 vtable 中的起始偏移
    CHAOS_IL2CPP_UINT32 method_count;     // 接口方法数
};
```

**每类型 iface_map：**

```cpp
// TypeA : IComparable, IDisposable
static const InterfaceMapEntry chaos_iface_map_TypeA[] = {
    { /*stable_id*/IComp, /*vtable_offset=*/4, /*method_count=*/1 },
    { /*stable_id*/IDisp, /*vtable_offset=*/5, /*method_count=*/1 },
};

// TypeA vtable array layout:
// [0]Object.Equals [1]A.GetHashCode [2]A.ToString [3]Object.Finalize
// [4]A.CompareTo(IComparable slot0)  [5]A.Dispose(IDisposable slot0)
```

**Interface dispatch codegen：**

```cpp
// 编译期已知：IComparable.CompareTo (iface_slot = 0)
const auto* chaos_ti = chaos_object_get_type_info(obj);
uint32_t offset = chaos_find_interface_offset(chaos_ti, &chaos_type_info_IComparable);
auto fn = reinterpret_cast<int(*)(void*,void*)>(chaos_ti->vtable_array[offset + 0]);
return fn(obj, other);
```

**`chaos_find_interface_offset` — 线性扫描 AOT iface_map + runtime_iface_map：**

```cpp
inline uint32_t chaos_find_interface_offset(
    const TypeInfo* ti, const TypeInfo* iface_ti) {
    for (uint32_t i = 0; i < ti->iface_count; ++i) {
        if (ti->iface_map[i].iface_stable_id == iface_ti->stable_id) {
            return ti->iface_map[i].vtable_offset;
        }
    }
    // 未在 AOT iface_map 中找到，扫描 HotUpdate runtime_iface_map
    for (uint32_t i = 0; i < ti->runtime_iface_count; ++i) {
        if (ti->runtime_iface_map[i].iface_stable_id == iface_ti->stable_id) {
            return ti->runtime_iface_map[i].vtable_offset;
        }
    }
    CHAOS_IL2CPP_ABORT();
}
```

### AOT 去虚化

| 模式 | 条件 | Codegen |
|------|------|---------|
| Monomorphic | 编译期已知确切接收者类型 | `KnownType_Method(args)` — direct call |
| Sealed class | 接收者类型为 sealed | 同 monomorphic |
| Polymorphic | 一般情况 | `vtable_array[slot]` — indirect call |
| Interface | 编译期已知接收者实现接口 | `vtable_array[iface_offset + slot]` |

### 与旧 VTableRegistry 的关系

A4-Dual+V2 vtable 系统与现有的 `vtable_registry.*`（token-based 运行时注册表）共存：
- **vtable_array**：codegen 发射的 vtable 访问路径，统一走 TypeInfo->vtable_array
- **vtable_registry**：`MakeGenericType` 运行时泛型类型的虚方法解析，使用 token-based 查找

未来可将 vtable_registry 迁移到统一体系，但短期保持共存。

## 与 B1 / Unity IL2CPP 对比

| 维度 | B1 (Global Slot) | B2+ | Unity IL2CPP |
|------|-------------------|-----|--------------|
| Slot 范围 | 全局唯一 | 继承链内编号 | 继承链内编号 |
| 分配复杂性 | 需全局协调 | 自动局部编号 | 自动局部编号 |
| 接口 dispatch | 全局 iface table offset | iface_map (linear scan) | iface_map (binary search) |
| 跨 TU 一致性 | ❌ 需要全局协调 | ✅ 自动 | ✅ 自动 |
| HotUpdate 友好 | ❌ 需预留 slot | ✅ 追加新 slot | ⚠️ 需预留或重建 |
| 对象头大小 | 8 bytes (type_id) | **8-24B 分型** | 16+ bytes |
| Dispatch 延迟 | O(depth) | **1-2 derefs** (Fat:1, V2:2) | O(1) indirect call |
| Devirtualization | 静态分析 → direct | 静态分析 → direct | static + sealed |

## 位置

- TypeInfo 定义: `src/native/common/chaos/type_info.h`
- Header 类型定义: `src/native/runtime-core/generated_code_compat.h`
- Slot 分配算法: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` (行 534-563)
- VTable 数组发射: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` (行 566-627)
- Virtual dispatch codegen: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` (行 986-1176)
- DevirtualizationHint: `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs` (行 412-426)
- 运行时 vtable_registry: `src/native/runtime-core/vtable_registry.*`
- 解释器虚方法分派: `src/native/interpreter/interpreter_vm.cpp` (CallVirt case)
