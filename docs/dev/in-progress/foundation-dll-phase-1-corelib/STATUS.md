---
task_id: foundation-dll-phase-1-corelib
title: Phase 1 System.Private.CoreLib 全量il2cpp开发
task_type: plan
lifecycle_status: in-progress
phase: assessment
created_at: 2026-05-02 03:30:00 +08:00
updated_at: 2026-05-03 19:00:00 +08:00
parent_task_id: foundation-dll-full-development
parent_task_phase: phase-1
parent_task_relation: child

## 实际状态（2026-05-03 修正）

### 42 families 全部为 scaffolding-only

2026-05-03 全量审计确认 CoreLib 42 families 的生成代码**全部为 skeleton stub，无真实 il2cpp 翻译代码**：

| 检查项 | 状态 | 说明 |
|--------|------|------|
| managed proof（*Proof.cs） | 42/42 缺失 | Phase A managed 代码未创建 |
| native skeleton stub 生成 | 31/42 有骨架 | RuntimeSkeletonPage0001.cpp — 全部 CHAOS_BRIDGE_STATUS_OK |
| 真实 il2cpp 翻译（bridge->调用） | 0/42 | 无任何 family 有真实 bridge 调用 |
| benchmark | 31/42 有骨架 | BenchmarkNativeEntry.cpp — 全部 return 42 |
| hotupdate | 42/42 缺失 | 无 hotupdate 目录 |
| Phase A emitter 补齐 | 未做 | 未分析 IL + 未补 emitter |
| 无 skeleton 的 family | 11 | assembly, custom-metadata, field-property, generics, member-complete, module, parameters, reflection-field-property, reports, type |

### 修正说明

此前 STATUS.md 记录为"32/32 全部通过"是基于验证工具的确认偏差：
- 验证工具检查文件存在和编译通过，但未检查代码真实性
- RuntimeSkeletonPage0001.cpp 编译通过但所有方法返回 CHAOS_BRIDGE_STATUS_OK
- BenchmarkNativeEntry.cpp 编译通过但全部 return 42
- managed proof / hotupdate proof 从未实际执行

**当前真实完成度：Phase 1 实质性 il2cpp 开发未启动。验证工具本身需要修复。**

## 下一步

1. 全体 42 families codegen review：逐 family 审查生成代码的真实状态
2. 修复验证体系：验证工具能区分 stub vs 真实代码
3. 从 Phase A 开始 CoreLib 真实 il2cpp 开发
