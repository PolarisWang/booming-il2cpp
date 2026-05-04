# VTable 虚方法分派架构 (B2+)

## 核心职责

虚方法与接口方法的分派中心。B2+ 方案使用 **Per-Object VTable Pointer**，每个对象头携带 vtable 指针，虚调用通过 `header->vtable[slot_index]` 实现 O(1) 间接分派，替代原有的 if-else 比较链。

## 设计目标

| 目标 | 说明 |
|------|------|
| O(1) 虚方法分派 | 无论继承链深度，虚调用开销恒定 |
| 自动 Slot 编号 | 每继承链独立，无需全局协调 |
| 接口方法分派 | iface_map + vtable 偏移，O(N) 线性扫描（接口数通常 < 10）|
| AOT 去虚化 | 已知接收者类型时发射 direct call，跳过 vtable |
| HotUpdate 兼容 | 动态类型可继承 AOT 类型的 vtable 布局 |

## 核心架构

### 对象头

```cpp
struct chaos_object_header {
    const void**        vtable;      // [0] vtable pointer
    const TypeInfo*     type_info;   // [8] type identity (GC, casting)
};
// sizeof = 16 bytes (原 8 bytes)
```

每个对象头部增加 8 字节的 vtable 指针。Object header 从 8→16 bytes，所有对象自动调整（堆分配使用 `sizeof(ManagedType)`，包含 header member）。

### TypeInfo（带 iface_map）

```cpp
struct TypeInfo {
    const TypeInfo* parent;                  // base type (nullptr for Object)
    CHAOS_IL2CPP_UINT64 stable_id;           // FNV-1a hash
    const InterfaceMapEntry* iface_map;      // AOT compile-time iface_map
    const InterfaceMapEntry* runtime_iface_map; // HotUpdate-追加的接口映射 (heap allocated)
    CHAOS_IL2CPP_UINT32 iface_count;         // AOT iface count
    CHAOS_IL2CPP_UINT32 runtime_iface_count; // HotUpdate 追加数
    CHAOS_IL2CPP_UINT8  type_shape;          // reference/value/interface
};
// sizeof = 48 bytes, inline (非 constexpr，因为 runtime_iface_map 可在运行时修改)
```

TypeInfo 携带 AOT iface_map + runtime_iface_map 双指针，分别用于静态编译和 HotUpdate 的接口映射。类型身份、GC、类型转换通过 `TypeInfo*` pointer equality。

### VTable 数组

每个类型在生成的 .cpp 中有一个 `static const void*` 数组：

```cpp
// 每类型的虚函数表（codegen 发射）
static const void* chaos_vtable_System_String[] = {
    reinterpret_cast<void*>(&String_Equals),
    reinterpret_cast<void*>(&String_GetHashCode),
    reinterpret_cast<void*>(&String_ToString),
    reinterpret_cast<void*>(&Object_Finalize),
    reinterpret_cast<void*>(&String_get_Length),
};

// 对象创建时赋值
obj->header.vtable = chaos_vtable_System_String;
obj->header.type_info = &chaos_type_info_System_String;
```

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

**Before（当前 if-else 链，O(N)）：**

```cpp
auto* chaos_type_info = chaos_header->type_info;
if (chaos_type_info == &chaos_type_info_A) {
    ((void(*)(void*))A_Method)(chaos_instance);
} else if (chaos_type_info == &chaos_type_info_B) {
    ((void(*)(void*))B_Method)(chaos_instance);
} else {
    // parent walk fallback...
    CHAOS_IL2CPP_ABORT();
}
```

**After（vtable dispatch，O(1)）：**

```cpp
auto fn = reinterpret_cast<void(*)(void*)>(chaos_header->vtable[3]);
fn(chaos_instance);
```

生成的指令：2 条 load（header→vtable, vtable[slot]）+ 1 条 indirect call。

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

// TypeA vtable layout:
// [0]Object.Equals [1]A.GetHashCode [2]A.ToString [3]Object.Finalize
// [4]A.CompareTo(IComparable slot0)  [5]A.Dispose(IDisposable slot0)
```

**Interface dispatch codegen：**

```cpp
// 编译期已知：IComparable.CompareTo (iface_slot = 0)
uint32_t offset = chaos_find_interface_offset(
    chaos_header->type_info, &chaos_type_info_IComparable);
auto fn = reinterpret_cast<int(*)(void*,void*)>(
    chaos_header->vtable[offset + 0]);
return fn(obj, other);
```

**`chaos_find_interface_offset` — 线性扫描 AOT iface_map + runtime_iface_map（接口数通常 1-5）：**

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
| Polymorphic | 一般情况 | `vtable[slot]` — indirect call |
| Interface | 编译期已知接收者实现接口 | `vtable[iface_offset + slot]` |

### 与旧 VTableRegistry 的关系

B2+ vtable 系统与现有的 `vtable_registry.*`（token-based 运行时注册表）共存：
- **B2+ vtable**：codegen 发射的 static vtable 数组，用于 AOT 编译类型的虚方法分派
- **vtable_registry**：`MakeGenericType` 运行时泛型类型的虚方法解析，使用 token-based 查找

未来可将 vtable_registry 迁移到 B2+ vtable + iface_map 体系，但短期保持共存。

## 与 B1 / Unity IL2CPP 对比

| 维度 | B1 (Global Slot) | B2+ | Unity IL2CPP |
|------|-------------------|-----|--------------|
| Slot 范围 | 全局唯一 | 继承链内编号 | 继承链内编号 |
| 分配复杂性 | 需全局协调 | 自动局部编号 | 自动局部编号 |
| 接口 dispatch | 全局 iface table offset | iface_map (linear scan) | iface_map (binary search) |
| 跨 TU 一致性 | ❌ 需要全局协调 | ✅ 自动 | ✅ 自动 |
| HotUpdate 友好 | ❌ 需预留 slot | ✅ 追加新 slot | ⚠️ 需预留或重建 |
| 对象头大小 | 8 bytes (type_id) | **16 bytes** | 16+ bytes |
| Dispatch 延迟 | O(depth) | O(1) indirect call | O(1) indirect call |
| Devirtualization | 静态分析 → direct | 静态分析 → direct | static + sealed |

## 迁移阶段

| Phase | 内容 | 涉及文件 | 状态 |
|-------|------|----------|------|
| **1** | 对象头 + vtable 数组 + virtual dispatch 迁移 | ObjectModelEmission, MethodEmission, generated_code_compat | ✅ 已完成 |
| **2** | Interface dispatch 迁移 | iface_map + InterfaceMapEntry | ✅ 已完成 (2026-05-04) |
| **3** | AOT 去虚化优化 | codegen 静态分析 | 已部分实现 (monomorphic) |
| **4a** | vtableLengths 截断 + HotUpdate 类型注册验证 | ObjectModelEmission.cs, type_registry.cpp | **✅ 完成** (2026-05-04) |
| **4b** | runtime_iface_map + 接口追加 | 运行时 API + TypeInfo 扩展 | **✅ 完成** (2026-05-04) |

### 实施进度 (2026-05-04)

| 组件 | 状态 | 位置 |
|------|------|------|
| chaos_object_header.vtable | ✅ | `generated_code_compat.h` |
| VTable slot 分配算法 | ✅ | ObjectModelEmission.cs:534-563 |
| VTable 数组发射 (chaos_vtable_TypeX[]) | ✅ | ObjectModelEmission.cs:566-627 |
| VTable 对象赋值 (new/反射/异常) | ✅ | 多个 Emission 文件 |
| chaos_vtable_resolve() 辅助函数 | ✅ | ObjectModelEmission.cs:629-635 |
| **EmitVirtualDispatchCall vtable[slot/offset]** | **✅ 完成 (2026-05-04)** | MethodEmission.cs:1052-1101 — vtable[slot] + interface dispatch via chaos_find_interface_offset |
| Interface dispatch via vtable_offset | ✅ 已实现 | InterfaceMapEntry struct, chaos_find_interface_offset(), vtable[offset + method_index] |
| type_registry.cpp iface_map bug | ✅ 已修复 | chaos_register_type() 现在保存 iface_map/iface_count |
| **vtableLengths 精确计算** | **✅ Phase 4a 完成** | ObjectModelEmission.cs:402-426 -- maxSlotInHierarchy + 1 |
| **runtime_iface_map 扩展** | **✅ Phase 4b 完成** | TypeInfo 新增 runtime_iface_map/runtime_iface_count (48 bytes) |
| **ChaosTypeAddInterface 运行时 API** | **✅ Phase 4b 完成** | type_registry.cpp:103-143 -- realloc 追加接口映射 |
| **TypeInfo codegen 非 constexpr 化** | **✅ Phase 4b 完成** | ObjectModelEmission.cs: `inline constexpr TypeInfo` → `inline TypeInfo` |
| **chaos_find_interface_offset runtime_iface_map 扫描** | **✅ Phase 4b 完成** | ObjectModelEmission.cs:745-764 -- 双扫描: AOT + runtime |
| **chaos_type_implements_interface runtime_iface_map 扫描** | **✅ Phase 4b 完成** | ObjectModelEmission.cs:718-743 -- 双扫描: AOT + runtime |
| **TypeInfo 7-field initializer codegen** | **✅ Phase 4b 完成** | ObjectModelEmission.cs:487-603 -- 全部 5 种初始化模式更新 |

## 解释器虚方法分派

解释器（InterpreterVM）使用 `vtable_registry` 在执行期解析 `CallVirt` 指令，而非 AOT 的 static vtable 数组：

### 调用流程

```
CallVirt 指令 → 检查 this 对象的 type_token → 
  ResolveVirtualMethodPointer(instance_token, declared_method_token) → 
  沿继承链查找 → call_target 供外部派发
```

### 对象类型跟踪

解释器对象（`InterpreterObject`）新增 `type_token` 字段，在 `NewObj` 时从 metadata token 赋值：

```cpp
struct InterpreterObject {
    CHAOS_IL2CPP_VECTOR(InterpreterValue) fields = {};
    CHAOS_IL2CPP_UINT32 type_token = 0u;  // 虚方法分派用
};
```

### 继承链解析

`ResolveVirtualMethodPointer` 从 `instance_type_token` 开始，沿 `TypeVTable.base_token` 向上遍历，查找 method_token 匹配的 slot：

```cpp
current = instance_type_token;
while (current != 0) {
    vt = by_token[current];
    for (slot in vt.slots) {
        if (slot.method_token == declared_method_token)
            return slot.method_pointer;
    }
    current = vt.base_token;
}
```

### 测试覆盖

| 测试 | 场景 |
|------|------|
| TestCallVirtDirectResolution | 一元类型：instance_token 直接匹配已注册 vtable |
| TestCallVirtInheritanceChain | 继承链解析：derived 重写方法 → 返回 derived 的 method_pointer |

## 位置

- TypeInfo 定义: `src/native/common/chaos/type_info.h`
- 对象头定义: `src/native/runtime-core/generated_code_compat.h`
- Slot 分配算法: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` (行 534-563)
- VTable 数组发射: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` (行 566-627)
- Virtual dispatch codegen: `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` (行 986-1176)
- DevirtualizationHint: `src/managed/Chaos.IL2CPP.CodeGen/Planning/NativeAotLoweringPlanner.InvocationPlanning.cs` (行 412-426)
- 运行时 vtable_registry: `src/native/runtime-core/vtable_registry.*`
- 解释器虚方法分派: `src/native/interpreter/interpreter_vm.cpp` (CallVirt case)
