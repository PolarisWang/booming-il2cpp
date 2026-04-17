---
name: executing-plans
description: 当你有一个已写好的实现计划，需要在当前会话中按批次执行、维护任务状态，并在必要时处理活动任务冲突时使用
---

# 执行计划

## 概述

加载计划，批判性地审查，执行所有任务，并维护当前任务目录中的 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md` 与索引文件。

计划收尾必须进入固定闭环：`审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交`。

**开始时宣布：** "我正在使用 executing-plans 技能来实现这个计划。"

**注意：** `dev:subagent-driven-development` 是另一条执行路径。只有当它具备与本技能等价的活动任务处理和状态维护规则时，才应切换过去。

**状态原则：** 任务目录中的 `STATUS.md` 是执行真相源；`docs/dev/ACTIVE.md` 只是当前活动任务指针。

## 文档语言要求

除非用户明确要求其他语言，由本技能创建或更新的 `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md`、索引摘要、风险记录、阻塞说明都必须使用中文。代码、命令、路径、标识符保持原文。

## 第零步：检查活动任务

仅在你即将开始新的复杂计划执行、切换到另一份计划，或恢复已有计划时使用这一步。

1. 检查 `docs/dev/ACTIVE.md` 是否存在。
2. 如果不存在：继续进入“加载并审查计划”。
3. 如果存在：
   - 读取 `docs/dev/ACTIVE.md`
   - 根据其中的 `status_file` 读取对应任务目录下的 `STATUS.md`
   - 提取任务名、`task_id`、`task_dir`、`lifecycle_status`、`phase`、最近摘要、下一步
4. 如果 `ACTIVE.md` 指向当前任务：直接恢复执行。
5. 如果 `ACTIVE.md` 指向另一个任务：在处理完成前，不要开始新的计划执行。

## 第一步：加载并审查计划

1. 读取当前任务目录中的计划文件一次
2. 明确识别计划中的任务总数
3. 提取执行需要的关键引用：
   - 设计文档路径
   - 计划文档路径
   - 每个任务的完整文本
   - 每个任务的知识沉淀目标
4. 批判性审查计划，识别任何疑虑
5. 如无疑虑：创建或更新
   - 当前任务目录中的 `STATUS.md`
   - `docs/dev/ACTIVE.md`
   - `docs/dev/INDEX.md` 与对应生命周期索引

## 第二步：执行任务

对每个任务：

1. 在开始实现前，先把当前任务编号或任务名称写入 `STATUS.md` 与 `docs/dev/ACTIVE.md`
2. 严格按照计划步骤执行
3. 按规定运行验证
4. 更新 `STATUS.md`
   - 最近摘要
   - 下一步
   - 验证结果
   - 风险与阻塞
   - wiki 决策
5. 更新 `docs/dev/ACTIVE.md`
6. 追加 `notes/progress-*.md`
7. 更新索引文件
8. 判断是否产生长期有效知识
   - 如果有：使用 `dev:project-wiki-maintenance`
   - 如果没有：明确记录“本任务无 wiki 更新”

## AOT / Obligation-Driven Completion Gate

如果计划里出现以下任一字段，或任务明显命中 AOT / IL2CPP / test governance 主线，则本次执行属于 obligation-driven：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

对 obligation-driven 任务，`completed` 前必须显式消费 `dev:verification-before-completion`，并至少确认：

- `requiredGates` 指定的接线闸门；未显式指定时默认 `collector -> registry -> workspace`
- owner managed proof
- owner native proof
- `hotupdateImpact = Smoke/Proof` 时的 hotupdate proof
- `benchmarkRequired = true` 时的 benchmark
- 受影响 regression
- plan 中列出的全部 `formalVerificationObjects`

如果 authority 要求正式对象或 case，但仓库里还不存在，不得跳过；先补对象或 case，再继续完成链路。

## 第三步：完成开发

所有任务完成后，必须按固定顺序收尾：`审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交`。

1. 审视架构合理性
   - 检查模块边界、职责分层、依赖方向、抽象数量是否仍然合理
   - 检查是否残留临时兼容代码、调试分支、重复逻辑或不必要复杂度
   - 如果发现架构不合理：回到任务执行阶段继续修复
2. 运行 formal verification gate 与项目级测试
   - obligation-driven 任务先按 `dev:verification-before-completion` 选择并执行正式对象
   - 再运行受影响项目级测试套件或项目约定的完整命令
   - 如果测试阶段出现 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃：
     - 本次验证立即视为失败
     - 收集 `stderr`、`binlog`、崩溃堆栈或 dump 信息（如果可用）
     - 使用 `dev:systematic-debugging` 定位并修复根因
     - 根因未修复前，不得继续归档或收尾
3. 将架构审视结论与验证结果写入 `STATUS.md`、`docs/dev/ACTIVE.md` 与最新 `notes/progress-*.md`
4. 如果任一验证失败：
   - 记录失败摘要、定位信息、下一步
   - 停止，不要归档为 `completed`
5. 只有在以下条件都满足后，才允许归档为 `completed`：
   - 计划任务全部完成
   - formal verification gate 通过
   - 架构合理性审视通过
   - 受影响项目测试通过
   - 本轮应写入的 wiki 更新已完成
6. 满足后：
   - 更新 `STATUS.md` 为 `completed`
   - 在 `notes/` 中追加最终进展记录
   - 将任务目录移动到 `docs/dev/completed/`
   - 删除 `docs/dev/ACTIVE.md`
   - 更新索引文件
   - 宣布："我正在使用 finishing-a-development-branch 技能来完成这项工作。"
   - 使用 `dev:finishing-a-development-branch`

## 何时停下来寻求帮助

遇到以下情况立即停止执行：

- 遇到阻塞（缺少依赖、测试失败、指令不清晰）
- 计划存在关键缺口导致无法开始
- 你不理解某条指令
- 验证反复失败

在停下来之前，把新的风险、阻塞和下一步记录进 `STATUS.md` 与最新 `notes/progress-*.md`。

## 集成

- `dev:using-git-worktrees`
- `dev:project-wiki-maintenance`
- `dev:verification-before-completion`
- `dev:systematic-debugging`
- `dev:finishing-a-development-branch`
