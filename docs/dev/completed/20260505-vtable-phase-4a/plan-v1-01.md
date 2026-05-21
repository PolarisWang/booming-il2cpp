# VTable Phase 4a: vtableLengths 截断 + HotUpdate 类型注册验证

> **面向执行 Agent：** 使用 dev:executing-plans 执行本计划。步骤使用复选框语法跟踪。

**目标：** 修复 vtableLengths 跨链尾部 nullptr 空位 + 验证 chaos_register_type() 对 HotUpdate 场景的端到端可用性。

**架构：** 不改动全局 slot 分配算法（全局 counter 是接口派发正确性的前提），仅修复 vtableLengths 计算为 `maxSlotInHierarchy + 1`。HotUpdate 类型通过 `chaos_register_type()` 注册，继承 AOT 基类 slot 布局。

**技术栈：** C# CodeGen (NativeAotLoweringPlanner.ObjectModelEmission.cs)、C++ runtime (type_registry.cpp/type_info.h)

**架构审核模式：** critical

**结构告警重点：** 无结构告警（~20 行改动）。codegen 逻辑在正确层（ObjectModelEmission.cs 中 slot 分配职责不变）。

**权责图审核主题：** codegen slot 分配 vs 运行时接口派发的职责边界确认——slot 分配由 codegen 独家控制，运行时只消费 slotMap 产物。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: vtable
- capabilityItem: vtable-slot-allocation
- ownerSubjectId: NativeAotLoweringPlanner
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Smoke
- formalVerificationObjects: [vtableLengths computed correctly for all reference types]
- requiredGates: build -> native-aot-runner

**设计文档：** n/a（design结论在 brainstorm 阶段已确认）

**问题清零来源：** direct user confirmation（brainstorm 阶段 Phase 4a 范围和修正确认）

**计划来源：** direct-plan

**预期知识沉淀：** wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/15-B2+VTable虚方法分派决策.md

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 修改文件

| 文件 | 操作 | 改动行数 |
|------|------|---------|
| `Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` | 修改 vtableLengths 计算 | ~15 |
| `wiki/.../15-B2+VTable虚方法分派决策.md` | 追加 Phase 4a 决策记录 | ~20 |
| `src/native/runtime-core/type_registry.cpp` | 仅验证，不修改 | 0 |

## 任务

### Task 1: 修复 vtableLengths 为 maxSlotInHierarchy + 1

**现状（ObjectModelEmission.cs:402）：**

```csharp
vtableLengths[typeId] = nextSlot; // 全局 nextSlot → 跨链空位
```

全局 counter 正确维护了 slot 分配的唯一性，但 `vtableLengths` 包含了跨链方法的 slot，导致 vtable 数组有 nullptr 尾部空位。

**修复方法**：在 slot 分配后，增加一次 pass 精确计算每个类型的 vtable 长度：

```csharp
// After the slot allocation loop, recompute vtableLengths per type
// by finding the max slot used in each type's own hierarchy
foreach (string typeId in TopologicalSortReferenceTypes(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
{
    int maxSlot = -1;
    string current = typeId;
    while (current != null && referenceTypeSubjectIds.Contains(current))
    {
        if (methodsByDeclaringTypeVT.TryGetValue(current, out var typeMethods))
        {
            foreach (var method in typeMethods)
            {
                if (method.IsStatic) continue;
                var sig = GetMethodSignatureSuffix(method.SubjectId);
                if (slotMap.TryGetValue(sig, out int slot) && slot > maxSlot)
                    maxSlot = slot;
            }
        }
        referenceTypeBaseSubjectIds.TryGetValue(current, out current);
    }
    vtableLengths[typeId] = maxSlot + 1;
}
```

**效果**：

```
修复前                              修复后
B1: [0]Eq [1]... [3]Fin [4]null [5]null [6]B1Method → len=7
                                    B1: [0]Eq [1]... [3]Fin [6]B1Method → len=7
```

vtable 数组仍有 slot 空位（4-5 来自跨链方法），但**尾部空位被消除**。完整消除空位需要 per-hierarchy 分配，但与接口派发冲突，因此不纳入 Phase 4a。

### Task 2: Build 验证

```bash
dotnet build Chaos.IL2CPP.CodeGen.csproj
```

确认 0 errors。

### Task 3: chaos_register_type() HotUpdate 场景验证

**验证内容**：
1. `chaos_register_type()` 已接受 `InterfaceMapEntry*` 和 `iface_count`
2. HotUpdate 类型需要：知道基类 vtable 长度 → 从末尾追加新 slot → 构建 iface_map
3. 运行时 dispatch 路径：`chaos_find_interface_offset()` 通过 stable_id 匹配，同时支持 static constexpr 和动态 TypeInfo

**无需修改代码**。仅验证当前 API 足够覆盖：

```cpp
// HotUpdate 注册模式（伪代码，运行时实现）
TypeInfo* parent = &chaos_type_info_AOT_BaseType;
static InterfaceMapEntry hu_iface_map[] = {
    { IExistingInterface_stable_id, /*offset=*/4, /*count=*/2 },
    { INewInterface_stable_id, /*offset=*/6, /*count=*/1 },
};
chaos_register_type("HotModule/NewType", parent,
    chaos_type_shape_reference, hu_iface_map, 2);
```

### Task 4: Wiki 更新

在 `15-B2+VTable虚方法分派决策.md` 中追加 Phase 4a 决策记录：
- 全局 counter 是接口派发正确性的前提（非疏忽）
- 跨链 vtable nullptr 代价已知且接受
- vtableLengths 已精确到 maxSlotInHierarchy + 1

同时更新 `14-VTable注册表.md` 迁移状态表，Phase 4 状态改为"Phase 4a 已完成"。

### Task 5: 收尾

- `batch_native_aot_runner.py` 确认回归测试通过
- git commit

## 验证总表

| 步骤 | 命令 | 预期 |
|------|------|------|
| 1. Build | `dotnet build` | 0 errors |
| 2. Regression | `batch_native_aot_runner.py` | 29/33 PASS（不变）|
| 3. Wiki | 文档更新 | 内容准确 |

## 执行顺序

1. Task 1: 修复 vtableLengths
2. Task 2: Build
3. Task 3: 验证 chaos_register_type()
4. Task 4: Wiki 更新
5. Task 5: 收尾 + commit
