# 19 — A4-Dual+V2 ObjectHeader 优化决策

## 决策

ObjectHeader 优化采用 **A4-Dual + V2 Inline VTable 分型方案**：按类型特征选择 PureType(8B) / ThinLockable(8B) / Fat(16B) 三种 header 布局，V2 类型将 ≤6 个 vtable 槽内联到 TypeInfo 尾部。同步采用 CoreCLR 风格偷位薄锁。6 个关键决策如下：

| 决策 | 选择 | 理由 |
|------|------|------|
| 1. FatHeader 字段顺序 | **交换** — `{type_info(0), vtable(8)}` | `chaos_object_get_type_info()` 无分支、单次读完成 |
| 2. vtable_array 统一 | **V2 自指** — `vtable_array = &inline_slots[0]` | 统一非 Fat 类型访问路径，接口 dispatch 无运行时 fallback |
| 3. 运行时判别 | **2-bit Magic** — bits[1:0] 编码 4 态 | 零额外内存，~85% 对象仅 1 次读，CoreCLR 20+ 年验证 |
| 4. 分配策略 | **按类分配** — 8B/8B/24B 三种路径 | 加权平均 ~9.6B/对象，全局按分配的 sync_state 纯浪费 |
| 5. V2 TypeInfo 形态 | **TypeInfoV2 结构体** — 112B | C++ 标准无 UB，语义清晰 |
| 6. 同步位布局 | **CoreCLR 风格** — 薄锁 + 懒膨胀 | 无竞争 1 次 CAS，SyncBlock 零分配 |

## 问题

当前对象头布局存在显著内存浪费和架构优化空间：

| 问题 | 当前状态 | 影响 |
|------|---------|------|
| ObjectHeader 56B | vtable(8) + type_info(8) + sync_state(8) + field_storage[32] | 百万对象 53.4 MB，冗余占 76% |
| field_storage[32] | 反射路径专用 buffer，生成代码不用 | -32B/对象，纯浪费 |
| sync_state 独立 | 8 字节独立字段，95% 对象永不 sync | 可偷位嵌入 header，省 8B |
| vtable 指针冗余 | 每个对象存 vtable 指针 | 可改为 TypeInfo 间接查，省 8B |
| TypeInfo 有 7B padding | type_shape 后全为填充字节 | 可复用放 flags |

## 方案对比

### 整体方案对比

| 维度 | CoreCLR | Unity IL2CPP | 当前 Chaos | A4-Dual+V2（选中） |
|------|---------|--------------|-----------|-------------------|
| 对象头大小 | 8B 统一 | 16B 统一 | 56B | **~9.6B 平均** |
| 虚调用 | 1-2 derefs | 2 derefs | 1 deref | **1-2 derefs** |
| 同步 | 1 CAS 薄锁 | monitor 分配 | 1 CAS 薄锁 | **1 CAS 薄锁** |
| 热更 | ★★★ JIT 依赖 | ★★★★ 有限 | ★★★★★ | **★★★★★** |
| 破坏性 | — | — | — | **高（一次全量 re-gen）** |

### 决策 1：FatHeader 字段顺序

| 维度 | A 交换（选中） | B 不交换 | C 位标记 |
|------|:------------:|:--------:|:--------:|
| **性能** | 🟢 get_type_info 无分支 | 🔴 chicken-and-egg，运行时不可行 | 🟡 每次 callvirt 多 1 ALU mask |
| **内存** | 🟢 同 | 🟢 同 | 🟢 同 |
| **架构** | 🟢 所有 header 第一字段语义一致 | 🔴 Fat/PureType 无法区分 | 🟡 vtable 指针需 tag 处理 |
| **热更** | 🟢 无影响 | 🟡 热更对象运行时判别难 | 🟢 无影响 |
| **风险** | 🟡 一次全量 re-gen | 🔴 结构问题不可行 | 🟡 GC 保守扫描可能追踪 tagged ptr |

### 决策 2：vtable_array 统一路径

| 维度 | A V2 自指（选中） | B 各自独立 | C Fat 统一 |
|------|:---------------:|:---------:|:---------:|
| **性能** | 🟢 V2 同 cache line，接口 dispatch 统一 | 🟡 接口 dispatch 需运行时 fallback | 🔴 Fat 1→2 derefs 退化 |
| **内存** | 🟢 同 | 🟢 同 | 🟢 同 |
| **架构** | 🟢 vtable_array 通用字段，V1/V2 统一 | 🟡 codegen 多分支 + 运行时辅助 | 🟢 codegen 无分支 |
| **热更** | 🟢 写 vtable_array[i] 统一 | 🟢 同 | 🟢 同 |
| **风险** | 🟢 低 | 🟡 多路径覆盖需更多测试 | 🟡 Fat 性能倒退 |

### 决策 3：运行时 Header 判别

| 维度 | A 2-bit Magic（选中） | B TypeInfo.flags |
|------|:-------------------:|:---------------:|
| **性能** | 🟢 1 次读 + 位测试，~85% 无分支 | 🟡 先 mask + 读 flags + 可能重读 = 2-3 次 |
| **内存** | 🟢 零额外（偷位） | 🟢 零额外 |
| **架构** | 🟢 CoreCLR 验证 20+ 年 | 🔴 chicken-and-egg（决策 1 选 B 时不可行） |
| **热更** | 🟢 透明 | 🟢 透明 |
| **风险** | 🟢 低 | 🟡 多 1-2 次读，架构脆弱 |

### 决策 4：分配策略

| 维度 | A 按类分配（选中） | B 统一 24B | C 统一 16B |
|------|:---------------:|:---------:|:---------:|
| **性能** | 🟢 85% 对象 8B，缓存效率最高 | 🟡 85% 对象浪费 16B 缓存行 | 🟡 PureType 浪费 8B |
| **内存** | 🟢 加权 ~9.6B/对象 | 🔴 24B/对象（多 150%） | 🟡 16B/对象（多 67%） |
| **架构** | 🟡 3 条分配路径，ObjectNewThin/Pure | 🟢 ObjectNew 不变 | 🟡 需要统一 struct 定义 |
| **热更** | 🟢 HeaderKind 确定时分配路径透明 | 🟢 同 | 🟢 同 |
| **风险** | 🟡 中低，codegen 需覆盖所有 newobj 点 | 🟢 低 | 🟡 内存浪费不符合设计目标 |

### 决策 5：V2 TypeInfo 形态

| 维度 | A TypeInfoV2（选中） | B 柔性数组 | C union 嵌入 |
|------|:------------------:|:---------:|:----------:|
| **性能** | 🟢 同（inline_slots 都在 base 尾部） | 🟢 同 | 🟢 访问直接 |
| **内存** | 🟢 V2 112B，V0 48B，V1 64B | 🟢 同实际 | 🔴 全 104B，V0 浪费 56B/类型 |
| **架构** | 🟢 C++ 标准，语义清晰，static_assert 可用 | 🔴 C++ UB，不同编译器 tail padding 行为各异 | 🟡 埋没语义，V0 无谓膨胀 |
| **热更** | 🟢 inline_slots[i] 直接寻址 | 🟡 需偏移量计算 | 🟢 同 |
| **风险** | 🟢 低，编译器安全 | 🔴 高，UB + LTO 可能破坏 | 🟡 V0 类型纯浪费 |

### 决策 6：ThinLockable 同步位布局

| 维度 | A CoreCLR 风格（选中） | B 直接膨胀 | C 双态分离 |
|------|:--------------------:|:---------:|:---------:|
| **性能** | 🟢 无竞争 1 CAS，纳秒级 | 🔴 每次 lock 走全局表查询 + 可能分配 | 🟡 锁路径同 B，GetHashCode O(1) |
| **内存** | 🟢 SyncBlock 懒分配，0 额外 | 🔴 5% 锁对象 * ~48B = 2.4B/对象 | 🟡 同 B |
| **架构** | 🟢 CoreCLR 20+ 年验证的状态机 | 🟢 简单，但性能差 | 🔴 锁膨胀 + 哈希内联，两套语义不一致 |
| **热更** | 🟢 运行时行为，无关 | 🟢 无关 | 🟢 无关 |
| **风险** | 🟡 中高，但复用当前 sync_state 薄锁实现，核心逻辑一致 | 🟢 低 | 🟡 哈希和锁的不一致设计 |

## 架构

### Header 布局

```
PureType (8B):
  ┌──────────────────────┐
  │ TypeInfo*       [+0] │  8 bytes
  ├──────────────────────┤
  │ instance fields      │
  └──────────────────────┘

ThinLockable (8B):
  ┌──────────────────────┐
  │ type_and_flags  [+0] │  8 bytes
  │ [63:3]=TypeInfo*     │
  │ [2]=hash             │
  │ [1]=inflated         │
  │ [0]=locked           │
  ├──────────────────────┤
  │ instance fields      │
  └──────────────────────┘

Fat (16B):
  ┌──────────────────────┐
  │ TypeInfo*       [+0] │  8 bytes
  │ vtable*         [+8] │  8 bytes
  ├──────────────────────┤
  │ sync_state     [+16] │  8 bytes（运行时内部）
  ├──────────────────────┤
  │ instance fields      │
  └──────────────────────┘
```

### TypeInfo 变体

```
V0 (48B, 无 vtable)       V1 (64B, indirect)       V2 (112B, inline)
┌──────────────────┐     ┌──────────────────┐     ┌──────────────────┐
│ parent(8)        │     │ parent(8)        │     │ base (64B)      │
│ stable_id(8)     │     │ stable_id(8)     │     │ ...             │
│ iface_map(8)     │     │ iface_map(8)     │     │ vtable_array* → │──┐
│ runtime_iface(8) │     │ runtime_iface(8) │     │ vtable_length(4)│  │
│ iface_count(4)   │     │ iface_count(4)   │     ├──────────────────┤  │
│ runtime_count(4) │     │ runtime_count(4) │     │ inline_slots[6] │←─┘
│ type_shape(1)    │     │ type_shape(1)    │     │ (48B)           │
│ flags(1)         │     │ flags(1)         │     └──────────────────┘
│ padding(6)       │     │ padding(6)       │
└──────────────────┘     │ vtable_array*(8) │
                          │ vtable_length(4) │
                          │ padding(4)       │
                          └──────────────────┘
```

### 同步状态机

```
                    CAS(expect=free, set=locked)
  空闲态 ─────────────────────────────────────→ 薄锁态
  bits[2:0]=000                                  bits[2:0]=001
  bits[63:3]=TypeInfo*                           bits[63:3]=thread_id|recursion
    │                                               │
    │ GetHashCode                                    │ 竞争发生
    │ CAS(bit[2]=1)                                  │ CAS 失败 → 膨胀
    ↓                                               ↓
  哈希态                                          膨胀态
  bits[2:0]=100                                  bits[2:0]=011
  bits[63:3]=hash_value                          bits[63:3]=SyncBlock*
    │                                               │
    └── 类型检查: mask bits[2:0] → TypeInfo* ───────┘
         (膨胀态时从 SyncBlock->type_info 取)
```

### 分配策略

| 类型 | 分配函数 | 头部大小 | 占比 |
|------|---------|---------|------|
| PureType | `ObjectNewPure(totalSize)` | 8B | ~30% |
| ThinLockable | `ObjectNewThin(handle, fieldsSize)` | 8B | ~55% |
| Fat | `ObjectNew(handle, fieldsSize)` | 24B | ~15% |

**加权平均头大小**：30% × 8 + 55% × 8 + 15% × 24 = **~9.6B/对象**

### 统一访问器

```cpp
// 运行时 — 从任意对象获取 TypeInfo*
// 利用 2-bit Magic 判别：bits[1:0]=00→PureType/Fat bits[1:0]!=00→ThinLockable
inline const TypeInfo* chaos_object_get_type_info(const void* obj) noexcept {
    uintptr_t first = *static_cast<const uintptr_t*>(obj);
    if (first & 3) {
        // ThinLockable（锁态或膨胀）
        if (first & 2) // inflated → SyncBlock->type_info
            return static_cast<SyncBlock*>(first & ~7ull)->type_info;
        return reinterpret_cast<const TypeInfo*>(first & ~7ull);
    }
    // PureType 或 Fat — TypeInfo* 都在 [0]
    return reinterpret_cast<const TypeInfo*>(first);
}
```

## 影响范围

| 文件 | 改动 |
|------|------|
| `src/native/common/chaos/type_info.h` | TypeInfo 扩展（+flags +vtable_array +vtable_length），新增 TypeInfoV2 struct |
| `src/native/runtime-core/generated_code_compat.h` | 新增 PureTypeHeader/ThinLockableHeader/FatHeader（字段交换），chaos_object_get_type_info() 访问器 |
| `src/native/runtime-core/runtime_core.cpp` | 删除 field_storage[32]（ObjectHeader 56→24B），新增 ObjectNewThin/ObjectNewPure 分配路径，sync_state 移植到 ThinLockable 偷位，SyncBlock 扩展 type_info 字段 |
| `src/native/runtime-core/reflection_api.cpp` | 使用 chaos_object_get_type_info() 统一访问 |
| `src/native/runtime-core/exception_api.cpp` | sizeof(chaos_object_header) → sizeof(FatHeader)，偏移量验证 |
| `src/managed/Chaos.IL2CPP.Generator/NativeAotLoweringPlanner.cs` | 新增 HeaderKind/VTableVariant 决策引擎 |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` | TypeInfo 按 variant 发射（V0/V1/V2），struct 定义嵌入对应 header 类型 |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs` | EmitVirtualDispatchCall 按 header 分型发射 |

## 实施路线

### Phase 1：基础瘦身（3-4 天）
1. `runtime_core.cpp`：删除 field_storage[32]（ObjectHeader 56B→24B），适配 FieldGetValue/FieldSetValue
2. `type_info.h`：TypeInfo 扩展（+flags +vtable_array +vtable_length），定义 TypeInfoV2

### Phase 2：Codegen 改造（5-7 天）
3. `generated_code_compat.h`：定义 PureTypeHeader/ThinLockableHeader/FatHeader，交换 FatHeader 字段
4. `NativeAotLoweringPlanner.cs`：决策引擎（HeaderKind + VTableVariant 选择）
5. `ObjectModelEmission.cs`：TypeInfo 按 variant 发射（V0/V1/V2 结构体），struct 定义嵌入对应 header 类型
6. `MethodEmission.cs`：EmitVirtualDispatchCall 分型发射

### Phase 3：运行时适配（3-4 天）
7. `generated_code_compat.h` + `runtime_core.cpp`：chaos_object_get_type_info() 统一访问器，ThinLockable 分配函数
8. `runtime_core.cpp`：sync_state → ThinLockableHeader.type_and_flags 偷位移植
9. `reflection_api.cpp` + `exception_api.cpp`：使用统一访问器

**总工时预估**：11-15 天 + 1 周缓冲

## 对比评估表

| 维度 | CoreCLR | Unity IL2CPP | 当前 Chaos | A4-Dual+V2（选中） |
|------|---------|--------------|-----------|-------------------|
| **头大小** | 8B 统一 | 16B 统一 | 56B | **~9.6B 平均** |
| **百万对象内存** | 7.6 MB | 15.3 MB | 53.4 MB | **~9.2 MB** |
| **虚调用最快** | 1-2 derefs（MT 内联 ≤64槽） | 2 derefs（klass→vtable） | 1 deref（obj→vtable） | **1-2 derefs**（Fat:1, Thin:2） |
| **接口调度** | 2-3 derefs | 2 derefs | vtable[slot] 1 deref | **2 derefs**（统一 vtable_array） |
| **无竞争锁** | 1 CAS 偷位 | monitor 分配（堆操作） | 1 CAS sync_state | **1 CAS**（同 CoreCLR） |
| **Fat 锁态类型检查** | SyncBlock 反查（2 额外） | klass 不变（0 额外） | N/A | **O(1)**（Fat type_info 在 [0] 不变） |
| **热更新** | ★★★ | ★★★★ | ★★★★★ | **★★★★★** |
| **加权总分** | 4.03 | 3.54 | — | **4.70** |

## 引用

- [CoreCLR 对象头设计] MethodTable 低位偷位：`src/coreclr/src/vm/object.h`
- [Unity IL2CPP] Il2CppClass + monitor 指针：`il2cpp-config.h`
- [Chaos 当前设计] `generated_code_compat.h` `runtime_core.cpp` `type_info.h`
- [讨论记录] `memory/object_header_design_comparison.md`
- [极致变体] `memory/object_header_scheme_a_extreme_variants.md`
- [A4/A5 综合评估] `memory/object_header_a4_a5_comprehensive_assessment.md`
- [关键决策分析] `memory/a4_v2_key_decisions.md`
