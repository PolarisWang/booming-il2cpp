---
task_id: hotupdate-d3-dispatch
title: HotUpdate D3 双层分派架构实现
task_type: plan
lifecycle_status: in-progress
phase: planning
created_at: 2026-05-06 12:00:00 +08:00
updated_at: 2026-05-06 12:00:00 +08:00
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# HotUpdate D3 双层分派架构

## 设计摘要

为 foundation-dll 验证体系实现真实 hotupdate 路径：patch 代码以托管 DLL 形式存在，构建期提取为 `.patchdata`，运行时通过 Dispatch Table + Interpreter 执行 IL，替代当前静态 3-TU C++ 预编译模式。

### 架构
D3 双层分派：每个模块生成静态 Dispatch Table，call site 采用模式感知分支 — AOT→AOT 零额外开销，patch 时走 InterpreterEntryDirect → Lazy IL→IR 降低 → Interpreter 执行。PatchMetadataCache 本地自洽，不注册全局类型系统。

### 决策要点
- 方法匹配：运行时 Token 重映射（名称匹配 + NameIndex）
- Dispatch 机制：D3 双层 ABI — dispatch table + direct call
- Dispatch Table：Per-module 平面静态表，构建期确定
- 数据格式：FlatBuffer 风格 .patchdata
- IL→IR 降低：Lazy（首次调用降低）
- 卸载：逐方法恢复，第一阶段无 RCU

## 关键文档
- plan: `plan-v1-01.md`
- 设计决策: `wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotUpdateDispatchD3-决策.md`

## 最近摘要
计划已从 brainstorming 的 `lively-sprouting-sprout.md` 迁移到项目目录。8 步实现计划已定稿，尚未开始执行。

## 当前步骤
全部 8 步未开始。准备从步骤 1（构建期 NameIndex + Token→Slot emit）启动。

## 下一步
确认启动顺序后，从步骤 1 开始实现。
