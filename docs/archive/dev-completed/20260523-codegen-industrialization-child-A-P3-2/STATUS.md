# A-P3-2: G7 Goto Elimination + G8 Pluginization

> **父任务:** 20260523-codegen-industrialization
> **创建日期:** 2026-05-23
> **设计来源:** parent-roadmap (codegen 工业化 brainstorm-v1-01.md + 用户确认 "G7 Goto 消除：B, G8 方案B")

---

## Phase

`planning`

## Type

`child-task`

## Source Relation

`roadmap-child`

## Clearance Source

`parent-roadmap`

## Blocking Questions

`[]`

## Question Clearance

`cleared`

## Clearance Confirmed By User

`true`

## 设计摘要

### 范围
- G7: 消除 AOT codegen 中所有 goto fallback（IRFlatRegion/FlatFallbackCount=0）
- G8: Complete pluginization — 所有 NativeReference family handler 接入 [RuntimeSkeletonFamilyPlugin] 属性注册 + 删除 hand-written sub-array 分派

### 边界拍板
- **G7 方案**: B（全面 interval-based CFG restructuring — 替换 ApplyNodeSplitting，修复 RecoverStructure，删除 IRFlatRegion）
- **G8 方案**: B（Full pluginization — 属性注册 + generated array，不止于 family dispatch）
- **非目标**: 不重新设计 IR，不改 Scriban 模板渲染架构

### 关键改动
- G7: 新增 `IntervalAnalysis.cs`（T1-T2 interval graph），修改 `StructuredIR.cs`/`StructuredControlFlow.cs`/`ExceptionEmission.cs`
- G8: 60+ handler 添加 `[RuntimeSkeletonFamilyPlugin]` 属性，删除 9 个 hand-written sub-arrays

### 风险评估摘要
- Interval analysis 复杂度适中，预留 IsCfgReducible 双重验证
- Generic exception shape fallback 仅在 6 种标准 shape 不匹配时触发
- Exception partition fallback 保留但不再触发 s_flatRegionCount

### 三优先级权衡结论
继承父 roadmap 结论：P1 性能 > P2 架构 > P3 hotupdate。
- G7 直接提升性能（消除 goto 使 C++ 编译器能优化结构化控制流）
- G8 提升架构完整性（消除 hand-written vs generated 两层分派）

## 设计文档

`plan-v1-01.md`

## Entry Skill

`dev-executing-plans`

## Preflight Self-Review

- **边界检查**: ✅ A-P3-2 定义为 G7+G8，范围一致
- **依赖检查**: ✅ 前置 A-P3-1（G6 Generics）已完成
- **Blocking questions**: ✅ 无（用户已确认方案选择）
- **可行性**: ✅ 改动定义明确，无未解决的架构问题

**结论**: `pass` — 可继续执行。

## 推荐下一步

执行 plan-v1-01.md 中的 Phase 1 Step 1.1（实现 Interval Analysis）
