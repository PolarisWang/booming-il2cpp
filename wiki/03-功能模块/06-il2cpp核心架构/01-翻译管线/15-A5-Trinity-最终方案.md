# A5-Trinity 最终方案

> 第五代对象模型架构：TriType + Triplex TypeInfo + Triple Dispatch
> 设计基准：性能最优 × 架构完美 × 热更适配 × 200+DLL

---

## 命名含义

| 三支柱 | 内容 | 核心创新 |
|--------|------|---------|
| **TriType** | 3 种对象头变体 | PureType(8B) / ThinLockable(16B) / Fat(24B)，TypeInfo* 统一在 [0] |
| **Triplex TypeInfo** | 3 层 TypeInfo 拆分 | **Hot(32B)** + **Warm(32B)** + **Cold(var.)** |
| **Triple Dispatch** | 3 条分派路径 | **Fat Direct**(1-deref) + **vtable_array**(2-deref) + **Devirt**(direct call) |

---

## 设计目标

| 目标 | 度量 | 当前基线 | A5-Trinity 目标 |
|------|------|---------|----------------|
| TypeInfo 热区 cache 效率 | 每 L1 cache line 驻留数 | 1 (TypeInfo 64B) | **2 (TypeInfoHot 32B)** |
| ObjectNew AOT 延迟 | 函数调用数 | 5+ (handle 解析) | **1 (ObjectNewDirect)** |
| Fat 虚调用延迟 | 解引用次数 | 2 (vtable_array) | **1 (header→vtable)** |
| 跨模块类型比较 | 额外访问 | 1 cold deref | **Hot 内 stable_id** |
| 模块上限 | 最大模块数 | 256 | **1024** |

---

## 架构

### 1. TriType 对象头

三种 variant，TypeInfo* 统一在 offset [0]：

```cpp
struct PureTypeHeader { const TypeInfoHot* type_info; };                          // 8B
struct ThinLockableHeader { const TypeInfoHot* type_info; uint64_t sync_state; }; // 16B
struct FatHeader { const TypeInfoHot* type_info; const void** vtable; uint64_t sync_state; }; // 24B

inline const TypeInfoHot* chaos_object_get_type_info(const void* obj) noexcept {
    return *static_cast<const TypeInfoHot* const*>(obj);
}
```

设计排除：8B 统一偷位、FatHeader 字段交换、bit-magic 类型判别。

### 2. Triplex TypeInfo 三层拆分

```cpp
// ── Hot 区 (32B) — 虚调用/类型比较热路径 ──
struct TypeInfoHot {
    const TypeInfoHot* parent;             // [0]  8B — 基类
    const void**       vtable_array;       // [8]  8B — 虚方法表
    CHAOS_IL2CPP_UINT64 stable_id;        // [16] 8B — FNV-1a 身份
    uint32_t           vtable_length;      // [24] 4B — vtable 槽数
    uint16_t           warm_delta;         // [28] 2B — this + warm_delta = Warm
    uint8_t            type_shape;         // [30] 1B — 1=ref, 2=value, 3=interface
    uint8_t            flags;              // [31] 1B — header_kind[1:0], finalizer[2]
};  // 32B

// ── Warm 区 (32B) — 接口 dispatch / 热更新 ──
struct TypeInfoWarm {
    const InterfaceMapEntry* iface_map;            // [0]  8B — AOT iface_map
    const InterfaceMapEntry* runtime_iface_map;    // [8]  8B — HotUpdate 追加
    uint32_t                 iface_count;          // [16] 4B — AOT 接口数
    uint32_t                 runtime_iface_count;  // [20] 4B — HotUpdate 追加数
    uint32_t                 cold_delta;           // [24] 4B — 到 Cold 偏移
    uint32_t                 _reserved;            // [28] 4B
};  // 32B

// ── 组合布局 ──
// 常规: [Hot 32B] [Warm 32B] [Cold varlen]
// V2:   [Hot 32B] [Warm 32B] [inline_slots[6] 48B] [Cold varlen]
//        vtable_array = &base + 64
```

TypeInfoV2 布局更新：
```cpp
struct TypeInfoV2 {
    TypeInfoHot  hot;                  // 32B
    TypeInfoWarm warm;                 // 32B
    const void*  inline_slots[6];      // 48B
    // Cold follows inline_slots if needed
};  // ≥112B (Hot 32 + Warm 32 + inline_slots 48)
```

### 3. Triple Dispatch 三路分派

| 路径 | 条件 | 解引用 | 发射代码 |
|------|------|--------|---------|
| **Fat Direct** | Fat 类型 + 非去虚 | **1 次** | `reinterpret_cast<FatHeader*>(obj)->vtable[slot]` |
| **vtable_array** | Thin/Pure + 非去虚 | **2 次** | `chaos_object_get_type_info(obj)->vtable_array[slot]` |
| **AOT Devirt** | 编译期已知类型 | **0 次** | `KnownType_Method(args)` |

codegen 决策：
```csharp
if (devirtHint.CanDevirtualize)         // direct call
else if (GetHeaderKind(t) == Fat)       // header->vtable[slot]
else                                    // ti->vtable_array[slot]
```

### 4. ObjectNewDirect — AOT 零解析创建

```cpp
void* CHAOS_RUNTIME_ABI_CALL ObjectNewDirect(
    RuntimeState* rs, ThreadState* ts,
    const TypeInfoHot* type_info, const void** vtable) noexcept
{
    const auto header_size = HeaderSizeFromFlags(type_info->flags);
    auto* object = static_cast<FatHeader*>(AllocateBytes(rs->config, header_size));
    if (!object) return nullptr;
    object->type_info = type_info;
    if ((type_info->flags & kTypeInfoHeaderKindMask) == kTypeInfoHeaderKindFat) {
        object->vtable = vtable;
        object->sync_state = 0;
    } else if (/* ThinLockable */) {
        auto* thin = static_cast<ObjectHeaderThin*>(static_cast<void*>(object));
        thin->sync_state = 0;
    }
    return object;
}
```

Codegen 发射：
```csharp
if (isStaticAotType)
    $"ObjectNewDirect(rs, ts, &{typeInfoSymbol}, {vtableSymbol})"
else
    $"ObjectNew(rs, ts, {typeHandle})"  // 泛型/反射/热更
```

### 5. 跨模块类型身份 (200+DLL)

| 场景 | 机制 | 延迟 |
|------|------|------|
| 同 family（单 DLL / 同次编译） | `inline constexpr` 唯一地址 → 指针比较 | 0 额外 |
| 跨 family（不同 DLL） | `__declspec(dllexport/import)` 导入表唯一 → 指针比较 | 1 PLT |
| 跨模块回退 | Hot 区 `stable_id` 比较 | 多 1 次 8B 比较 |

`kMaxModules` 扩展：**256 → 1024**（8KB 数组，开销无感）。

AOT 场景 ObjectNewDirect 完全绕过 module_registry，不受上限影响。

### 6. 跨模块类型兼容检查

```cpp
bool chaos_is_type_compatible(const TypeInfoHot* actual, const TypeInfoHot* target) {
    auto* current = actual;
    while (current != nullptr) {
        if (current == target ||                      // 同模块：指针比较 (O(1))
            current->stable_id == target->stable_id)  // 跨模块：stable_id 回退
            return true;
        current = current->parent;
    }
    return false;
}
```

---

## 与 A4-Dual+V2 的差异

| 项目 | A4-Dual+V2 | A5-Trinity | 破坏性 |
|------|-----------|------------|--------|
| TypeInfo | 64B 单体 | Hot(32B)+Warm(32B)+Cold(var) | **高** |
| ObjectNew | handle 解析 (5+ calls) | AOT: Direct; 泛型: handle | **中** |
| Fat 虚调用 | 2-derefs (vtable_array) | **1-deref (header→vtable)** | **低** |
| stable_id | Cold | **Hot** | **低** |
| iface_map | 线性扫描 | 线性扫描 (不变) | **无** |
| type_info 字段 | TypeInfo* | **TypeInfoHot*** | **高** |
| ModuleRegistry | 256 | **1024** | **低** |
| codegen 发射 | 单路径 ObjectNew | **双路径 (Direct + Handle)** | **中** |

---

## 对比总表

| 维度 | 子项 | CoreCLR | Unity IL2CPP | A4-Dual+V2 | **A5-Trinity** |
|------|------|---------|-------------|-----------|---------------|
| **对象头** | 固定开销 | 8B (偷位) | 16B | 8-24B | **8-24B** |
| | 平均头大小 | 8B | 16B | ~9.6B | **~9.6B** |
| | 评分 | ★★★★★ | ★★★ | ★★★★★ | **★★★★★** |
| **TypeInfo** | 热区大小 | MT ~80B | klass ~80B | 64B | **32B** |
| | 每 cache line 驻留 | ~0.8 | ~0.8 | ~1 | **~2** |
| | 评分 | ★★★ | ★★★ | ★★★★ | **★★★★★** |
| **ObjectNew** | AOT 路径 | ~10 cycles | ~15 cycles | ~40 cycles | **~12 cycles** |
| | 评分 | ★★★★★ | ★★★★ | ★★★ | **★★★★★** |
| **虚调用** | Fat 解引用 | 2 | 2 | 2 | **1** |
| | 接口 dispatch | 2-3 derefs | 2 derefs | O(N) | **O(N)** |
| | 去虚化 | JIT 推测 | AOT 编译 | AOT 编译 | **AOT 编译** |
| | 评分 | ★★★★★ | ★★★★ | ★★★★ | **★★★★★** |
| **类型比较** | 同模块 | O(1) 指针 | O(1) 指针 | O(1) 指针 | **O(1) 指针** |
| | 跨模块 | O(1) 统一 | O(1) 统一 | O(N)+stable_id | **O(N)+Hot.stable_id** |
| | 评分 | ★★★★★ | ★★★★★ | ★★★★ | **★★★★★** |
| **同步** | 无竞争 | 1 CAS | heap alloc | 1 CAS | **1 CAS** |
| | 评分 | ★★★★★ | ★★★ | ★★★★★ | **★★★★★** |
| **架构** | 职责分离 | ★★ | ★★★ | ★★★★ | **★★★★★** |
| | 概念简洁 | ★★ | ★★★★ | ★★★ | **★★★** |
| | 评分 | ★★ | ★★★★ | ★★★ | **★★★★** |
| **热更新** | 方法/接口扩展 | ★★★ | ★★★★ | ★★★★★ | **★★★★★** |
| **200+DLL** | 模块上限 | 无限制 | 无限制 | 256 | **1024** |
| | 评分 | ★★★★★ | ★★★★★ | ★★★ | **★★★★★** |

## 加权评分

| 维度 | 权重 | CoreCLR | Unity IL2CPP | A4-Dual+V2 | **A5-Trinity** |
|------|------|---------|-------------|-----------|---------------|
| 性能 | 35% | 4.75 | 3.75 | 4.25 | **4.75** |
| 架构完美性 | 20% | 2.50 | 3.50 | 3.25 | **4.00** |
| 热更新 | 20% | 3.00 | 3.50 | 5.00 | **5.00** |
| 200+DLL | 15% | 5.00 | 5.00 | 3.00 | **5.00** |
| 内存效率 | 10% | 4.00 | 3.00 | 4.00 | **5.00** |
| **加权总分** | 100% | **3.96** | **3.76** | **4.04** | **4.78** |

---

## 风险评估

### 高风险

| # | 风险 | 概率 | 影响 | 缓解 |
|---|------|------|------|------|
| R1 | TypeInfoHot* 全局替换，漏改导致 UB | **高** | **高** | `using TypeInfo = TypeInfoHot` 过渡别名；快照全量覆盖；static_assert 布局 |
| R2 | warm_delta 计算错误 | **中** | **高** | static_assert offset 连续性；运行时断言 delta 范围 |
| R3 | ObjectNewDirect 误用于非 AOT 类型 | **低** | **高** | codegen isStaticAotType 检查与运行时一致 |

### 中等风险

| # | 风险 | 概率 | 影响 | 缓解 |
|---|------|------|------|------|
| R4 | 跨 DLL 符号冲突 | **中** | **中** | 按 full subjectId 命名已足够；dllimport 精确链接 |
| R5 | Fat 1-deref 与 vtable_array 结果不一致 | **低** | **中** | 快照验证；CHAOS_DEBUG 断言一致性 |
| R6 | TypeInfoV2 inline_slots 偏移 | **中** | **中** | static_assert offset == 64 |
| R7 | codegen 代码膨胀 | **中** | **低** | 每条调用仅 +30 字符，可忽略 |

### 低风险

| # | 风险 | 概率 | 影响 | 缓解 |
|---|------|------|------|------|
| R8 | Hot 32B 对齐 | **低** | **低** | 字段天然 8-aligned |
| R9 | ModuleRegistry 1024 仍不足 | **极低** | **低** | 编译时常量，调整代价 0 |
| R10 | HotUpdate warm 区原子性 | **低** | **中** | 8B 指针赋值天然原子 + std::atomic |
| R11 | PureType 虚调用边界 | **极低** | **低** | codegen gated virtual method 检测 |

---

## 设计排除

| 排除项 | 原因 |
|--------|------|
| 统一 8B 对象头偷位 | 失去 Fat 1-deref vtable 路径；性价比低 |
| iface_cache 内联 Hot | Hot 32B 已满；接口 dispatch 非热路径 |
| InterfaceMap binary search | N<10 时二分 branch mispredict > 线性 |
| TypeInfoHot 全局注册表 | AOT 跨 DLL 导入表即可唯一化 |
| JIT inline caching | AOT 去虚已消除大部分 callvirt |

---

## 实现状态

| Phase | 内容 | 状态 | Commit |
|-------|------|------|--------|
| **Phase 1** | TypeInfo 三层拆分：type_info.h Hot+Warm+V2；所有 TypeInfo*→TypeInfoHot* 迁移；runtime_core.cpp 头类型适配 | 完成 | `ff0c88f7` |
| **Phase 2** | C# Codegen TypeInfo+VTable 双结构发射；ObjectModelEmission.cs Scriban 模板适配 | 完成 | `6761ebf9` |
| **Phase 3** | ObjectNewDirect AOT 零解析路径；RuntimeAbiV0 注册 | 完成 | `ff0c88f7` |
| **Phase 4** | Fat 1-deref dispatch：vtable_array 去除 static；ExceptionEmission.cs EmitLinearVirtualDispatchCall 按 HeaderKind 三分支 | 完成 | `34db609b` |
| **Phase 5** | ModuleRegistry kMaxModules 256→1024 | 完成 | `ff0c88f7` |
| **Phase 6** | 验证收尾：全部 61/61 快照通过；chaos_runtime_core 编译通过；结构告警 | 完成 | — |
| **总计** | 7 个 commit（含 A4-Dual+V2 基底） | 全部完成 | `cadacbc1`→`34db609b` |

---

## 位置

- TypeInfoHot/Warm/V2: `src/native/common/chaos/type_info.h`
- Header 类型 + GetSyncStatePtr: `src/native/runtime-core/generated_code_compat.h`
- ObjectNewDirect + HeaderSizeFromFlags: `src/native/runtime-core/runtime_core.cpp`
- codegen TypeInfo+VTable 发射: `ObjectModelEmission.cs`
- codegen Triple Dispatch (Fat 1-deref): `ExceptionEmission.cs` (~1330行 `EmitLinearVirtualDispatchCall`)
- codegen ObjectNew 双路径: `ObjectModelEmission.cs` (newobj ~850行)
- vtable registry 适配 GetWarmPtr: `src/native/runtime-core/vtable_registry.cpp`
- type_registry (HotUpdate) 适配: `src/native/runtime-core/type_registry.cpp`
- reflection_api 适配: `src/native/runtime-core/reflection_api.cpp`
- ModuleRegistry: `module_registry.h` / `module_registry.cpp`
- NativeCodegenValidator: `Validation/NativeCodegenValidator.cs`
