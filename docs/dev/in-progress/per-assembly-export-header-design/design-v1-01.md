# Per-Assembly Export Header + Per-Page Selective Include — 设计文档

## 背景

当前 codegen 在 200+ DLL 场景下有两个 O(N²) 瓶颈：
1. **每个 assembly 的每 page** 都 include 全部 200 个 assembly 的全量类型声明（`native-aot.generated.header.h`）
2. **每 page** 都 include 20+ 无条件运行时头文件，不管该方法体是否实际需要

## 目标

- 消除 200+ DLL 场景下的编译时间 O(N²) 问题
- 热路径性能不能下降（P1 > P2）
- 不改变 TypeInfo 的内存布局（不改 runtime-core）
- 保持与现有 snapshot 测试的兼容性

## 方案设计

### 组件 1：Per-Assembly Export Header Generator

**位置：** `src/managed/Chaos.IL2CPP.Generator/AssemblyExportHeaderGenerator.cs`

**输入：** `AssemblyExportRegistry`
- `RegisterCall(callerAssembly, calleeSubjectId)` 已有数据
- 每个 assembly 的 `GetExportsForAssembly(name)` 返回被外部调用的方法

**输出：** 每个 assembly 生成一个 `<assembly-name>.exports.h`：
```cpp
// chaos_System_Collections_exports.h — Auto-generated
// Assembly 'System.Collections' exports these types/methods for cross-assembly consumers.
#pragma once

// Types consumed by other assemblies
struct chaos_type_System_Collections_ArrayList;
struct chaos_type_System_Collections_Hashtable;
// ... only types actually referenced by other assemblies

// Extern "C" method symbols for cross-assembly dispatch
extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_ArrayList__ctor();
extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_System_Collections_Hashtable__Add();
```

**消费侧：** 当 Assembly A 需要引用 Assembly B 的类型时：
```
// Assembly A's header.h:
// 不再 #include ALL of Assembly B's types
// 而是:
#include "chaos_System_Collections_exports.h"
```

### 组件 2：Per-Page Type Reference Tracker

**位置：** `NativeAotLoweringPlanner.cs` + `NativeAotEmitter.cs`

**当前状态：** `_allEmittedTypeSubjectIds` 是 assembly 级全局集合

**改动：** 扩展 Planner 在 Emission 阶段，为每个 method 记录它引用的类型集合，然后在分页时聚合到 page 粒度：

```csharp
// 新增数据结构
class PageTypeReference {
    int PageNumber;
    HashSet<string> DirectReferencedTypeIds;  // 方法体直接引用的类型
    HashSet<string> TransitiveReferencedTypeIds; // 传递闭包（通过父类、接口）
}
```

**发射时使用：** `BuildTypeDeclarationsCode()` 改为按 page 过滤：
```csharp
// 当前: 全量
_ = BuildTypeForwardDeclarations(_allEmittedTypeSubjectIds);

// 优化后: 按 page 过滤
_ = BuildTypeForwardDeclarations(page.DirectReferencedTypeIds);
```

### 组件 3：Per-Page Header Dependency Analysis

**位置：** `NativeAotEmitter.cs` — `BuildIncludes()` 方法（当前 lines 1440-1505）

**当前：** 所有 page 无条件 include 20+ 头文件

**改动：** 按 page 方法体的实际需要裁剪 include：
```csharp
// 每个 page 识别它需要的运行时模块
var neededModules = page.Methods
    .SelectMany(m => m.RequiredRuntimeModules)
    .Distinct()
    .ToHashSet();

// 只 include 需要的模块
if (neededModules.Contains(RuntimeModule.GcCardTable))
    includes.Add("<gc/gc_card_table.h>");
if (neededModules.Contains(RuntimeModule.ComInterop))
    includes.Add("\"com_ccw.h\"");
// ...
```

### 组件 4：NativeAotEmitter.cs — 共享 Header 拆分

**当前：** `BuildTypeDeclarationsCode()` 生成 `native-aot.generated.header.h`，包含全量类型

**改动：** 
- 生成 `native-aot.assembly-exports.h`（本 assembly 导出给外部的类型）
- 每个 page 根据 `PageTypeReference` 生成自己的最小 include set
- Page 0 仍保留完整的 TypeInfoV0 定义，但非 Page 0 只获取 extern 声明

---

## 实现计划

### Phase 1：Assembly Export Header（~3 天）

| Step | 文件 | 改动 |
|------|------|------|
| 1.1 | `AssemblyExportHeaderGenerator.cs` (新) | 读取 AssemblyExportRegistry，生成最小导出 header |
| 1.2 | `NativeAotEmitter.cs` | 发射时调用 generator，生成 `<assembly>.exports.h` |
| 1.3 | `NativeAot.TypeDeclarations.h.scriban` | 添加条件 include 路径 |
| 1.4 | Snapshot 测试 | 更新基线，验证导出 header 内容正确 |

### Phase 2：Per-Page Type Reference（~4 天）

| Step | 文件 | 改动 |
|------|------|------|
| 2.1 | `NativeAotLoweringPlanner.cs` | 新增 `PageTypeReference` 数据模型，在 Emission 时填充 |
| 2.2 | `NativeAotLoweringPlanner.xxx.cs` (各 emission file) | 在每个 method emission 路径中记录 `MethodUsesType` |
| 2.3 | `NativeAotEmitter.cs` | `BuildTypeDeclarationsCode()` 接受 page filter |
| 2.4 | Snapshot 测试 | 验证 per-page 声明内容正确 |

### Phase 3：Per-Page Header 裁剪（可选，~3 天）

| Step | 文件 | 改动 |
|------|------|------|
| 3.1 | `NativeAotEmitter.cs` — `BuildIncludes()` | 按 page 的 `RequiredRuntimeModules` 裁剪头文件 |
| 3.2 | 运行时模块映射表 | 建立方法模式 → 运行时模块的映射（如 `chaos_gc_dirty_card` → `gc/gc_card_table.h`） |

---

## 三优先级权衡

| 优先级 | 评估 |
|--------|------|
| **P1 性能最优** | ✅ 收益：每 page compile 减少 50K→100 个 extern 声明，编译时间从 111min→~5min。热路径内联不受影响（同 page 调用）。 |
| **P2 方案完美** | ✅ 架构更清晰：assembly 边界明确，导出/导入显示声明，不再全量相互依赖。 |
| **P3 HotUpdate** | 🟢 无影响 — HotUpdate 路径不走 codegen header 裁剪 |

---

## 风险评估

| 风险 | 概率 | 缓解 |
|------|------|------|
| MethodUsesType 信息不全导致 missing declaration | 🟡 中 | Phase 2 使用保守策略：若 type ref 信息缺失，回退到全量声明 |
| Per-page include 裁剪过早导致编译失败 | 🟡 中 | Phase 3 可选，默认为全量 include + 只裁剪 extern 声明 |
| AssemblyExportRegistry 数据不完整 | 🟢 低 | 导出 header 使用白名单+黑名单模式：显式引用的类型 + 已知基类型（Object, ValueType）的自动包含 |
