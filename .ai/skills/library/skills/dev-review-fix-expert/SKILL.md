---
name: dev-review-fix-expert
description: 代码审查结果修复专属入口。收到代码审查反馈时，立即分类、全量路由 Workflow 派发 expert，禁止先读代码确认。
---

# 代码审查修复专家

收到代码审查反馈（平台 review 工具、ReportFindings、外部 review 报告、评审列表）时，用本流程修复。

## 铁律：拉起 expert agent 是不可绕过的前置步骤

**收到 review 结果的立即行为：**

1. **停止后续工具调用**
2. **执行域分类声明**（CLAUDE.md 第〇条：`echo "本轮任务涉及...{域}..." > .claude/.classified`）
3. **按整个 review 集映射到"≥2 域"规则** — 即使这些 finding 不属于开发新翻译路径，也按多域 Workflow 处理
4. **禁止先读代码确认 review 结果** —— review 已含文件:行号 + 完整诊断，直接信任并分工派 agent

> ⚠️ **为什么禁止先读代码**：惯性"先看一下文件再说"会导致延迟派发、上下文浪费、分工粒度粗糙。review 结果的可靠性已由出具方保证；你只需做"是否值得修"的**最小化**核实（如反例可否被源码证明），而非"复核每个 finding"。

## review → 域分类 → Workflow 派发流程

```
收到 review 结果（N 条 finding）
  │
  ├── 按文件所属域把所有 finding 分组（GC / native / codegen / ATG / 工具 / 文档 / ...）
  ├── ≥2 域 → 启动多域 Workflow：
  │     每个域 = 一个 expert agent（含"该域全部 finding"）
  │     一次性并行派出全部 agent（不要分批 3+3）
  ├── 全部 agent 完成后 → 统一 review 合并质量
  └── 验证 → commit（三段式 root_cause/fix_strategy/regression_check）× push
```

## 分工原则

- **按文件→域 切分**，一次性全量派发，不逐批补
- **每域一个 agent**，给全该域所有 finding（含文件:行号 + 完整诊断）
- 只有需要"该不该修"裁决的 finding 才单独核实（读反例源码）
- 批处理完后，主 agent 统一做一次跨域质量 review（不要全信子 agent 自述"已修"）

## 禁止

- ❌ 先 Read 多个文件复核再分工
- ❌ 分批派 agent（3+3）而非一次全派
- ❌ 让子 agent 自证"已修"就收工 —— 主 agent 需抽验
- ❌ git stash（CLAUDE.md 强制）—— 切换用 git worktree 或显式提交

## 下游

- 有效实施路径已梳理 → 交接给 `dev-requesting-code-review`（请求审查）或按 CLAUDE.md 三规则落 commit
- 复杂多域 → 参考 `dev-dispatching-parallel-agents` 的并行编排
