---
name: dev-subagent-driven-development
description: 当在当前会话中执行具有独立任务的实现计划时使用
---

# 子 Agent 驱动开发

通过为每个任务派发新鲜子 Agent 来执行计划，每个任务完成后进行两阶段审查：先做规格合规性审查，再做代码质量审查。

**与 `dev-executing-plans` 的关系：** 本技能是 `dev-executing-plans` 的子 Agent 派发变体。所有通用原则（文档压缩、目录一致性、父子任务原则、文档语言要求、`ACTIVE.md` 守卫、执行前输入守卫、preflight self-review、结构告警与架构审视、obligation-driven completion gate、收尾闭环、证据卡模板）均直接沿用 `dev-executing-plans` 中的定义，本技能不再重复。以下仅记录子 Agent 特有的差异。

## 核心差异

- 控制器在开始时读取计划一次，通过派发子 Agent 来执行每个独立任务，不直接接触实现代码
- 审查顺序固定为：规格合规性 → 代码质量，每轮审查发现问题后由同一实现者修复
- 每个任务完成后控制器执行结构告警与状态维护
- plan 收尾仍然必须进入固定闭环：`结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交`

## 流程

### 第零步：检查活动任务

与 `dev-executing-plans` 第零步一致。

### 第一步：加载计划

1. 读取当前任务目录中的计划文件一次，确认任务总数
2. 提取所有任务的完整文本，以及 `STATUS.md` 中的 `blocking_questions` / `question_clearance` / `clearance_confirmed_by_user` / `clearance_source` / `preflight_review` / `preflight_summary` / `auto_execution_decision`
3. 创建或更新 `STATUS.md`、`docs/dev/ACTIVE.md`、`docs/dev/INDEX.md`

### 第二步：逐任务执行

对每个任务：

1. 派发实现者子 Agent，直接提供完整任务文本
2. 如果实现者需要更多上下文，补充后重新派发
3. 实现者完成后，先进行规格合规性审查
4. 规格合规性通过后，再进行代码质量审查
5. 如任一审查发现问题：由同一个实现者修复，重新审查，直到通过
6. 任务实现收敛后，控制器完成一轮结构告警（同 `dev-executing-plans` 中的定义）
7. 更新 `STATUS.md`，必要时更新 `docs/dev/ACTIVE.md`、`notes/progress-*.md`、索引、wiki
8. 如果当前任务带有 `parent_task_id`：同步父任务的 child mapping、摘要、`latest_stop_point`、下一步

### 第三步：完成

所有任务完成后，按 `dev-executing-plans` 中的收尾闭环执行：`结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交`。

## 审查规则

- 规格合规性审查者必须保持怀疑态度，不信任实现者的自报
- 审查者必须独立阅读代码
- 如果审查者发现问题，流程必须进入修复循环

## Batch 同类小任务合并派发

当计划里多个任务是**同一种微小编辑、重复跨文件**（同一 one-line fix、同一常量/字段的批量修改）时，**不要逐任务各派一个 subagent**。合并为一个 dispatch brief（列出每文件的具体改动），一个 implementer 完成、一个 diff 一并审查。

保留一任务一 subagent 的标准，仅用于需要独立判断、独立测试或独立审查面的任务。判定信号：任务是否有自己的判断 / tests / review surface。

## 修复循环上限与模型分级

- **审查修复循环 5 轮封顶**：两阶段审查发现问题的修复，记录轮次 R。R≤3 时恢复**原实现者**继续修；R=4、5 时派**更强模型的新实现者**重做该任务 diff（用该任务上下文，不继承原实现者成果）；breaker tripped（R=5 仍有未决 load-bearing finding）时，逐条裁决留痕，不再无限重试。
- **每次派发 subagent 显式指定 model**，不静默继承会话默认模型（否则全用最贵档）。
- **模型分级**：机械任务（plan 里含完整代码，实现=转录+测试）← fast 档；多文件集成判断 ← 标准档；架构/全分支查找/修复升级(R≥4) ← 最强档。**Turn count 胜过单 token 价格**：便宜模型常多花 2-3 倍轮次，反而更贵。

## 红旗

**绝不：**

- 跳过规格合规性审查
- 在规格合规性通过前开始代码质量审查
- 让子 Agent 自己去读计划文件
- 用"任务都做完了"替代 formal verification gate
- 只拿运行日志就声称"新测试流程已打通"，却没有刷新 verification formal source
- 用固定文件行数直接判定失败
- 把 `warn` 升级成独立人工复核流程
- 让 `STATUS.md`、`ACTIVE.md` 或索引落后于当前进度
- 在当前任务目录仍位于 `docs/dev/in-progress/` 时，只在正文或父 roadmap 里写 `completed`
- 在没有先处理 `docs/dev/ACTIVE.md` 指向任务的情况下，直接启动新的并行执行
- 因为 `warn` 级别问题就中断 roadmap-child 的自动推进

## 集成

- `dev-project-wiki-maintenance`
- `dev-verification-before-completion`
- `dev-systematic-debugging`
- `dev-finishing-a-development-branch`
- 子 Agent 应使用 `dev-test-driven-development`
