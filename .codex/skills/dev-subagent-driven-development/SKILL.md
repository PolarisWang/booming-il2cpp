---
name: subagent-driven-development
description: 当在当前会话中执行具有独立任务的实现计划时使用
---

# 子 Agent 驱动开发

通过为每个任务派发新鲜子 Agent 来执行计划，每个任务完成后进行两阶段审查：先做规格合规性审查，再做代码质量审查。对于来自已批准 roadmap 的 child task，控制器默认目标是自动续跑，而不是在每一步都回到用户处确认；在启动 child 或恢复 child 前，控制器先做一次最小 `preflight self-review`，只有命中 `stop` 条件才中断自动推进。

plan 收尾仍然必须进入固定闭环：`结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交`。

## 核心原则

- 控制器在开始时读取计划一次
- 控制器直接向实现者提供完整任务文本
- 实现者必须做自审并检查完整性
- 审查顺序固定为：规格合规性 → 代码质量
- 每个任务后都要先完成最新结构告警，再以 `STATUS.md` 为主记录状态
- 所有任务完成后，不能直接宣布结束，必须先通过固定收尾链路
- 在开始新的复杂任务、切换任务或恢复任务前，必须先执行与 `dev:executing-plans` 等价的 `ACTIVE.md` 守卫

**文档压缩原则：**

- 普通推进以更新 `STATUS.md` 为主
- `docs/dev/ACTIVE.md` 只在当前活动任务、阶段或下一步摘要发生变化时更新
- `notes/progress-*.md` 只在阻塞、重要决策或验证失败时追加
- 不要为了一次普通推进同时改 `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md`、索引和 wiki，除非这些对象真的都发生了变化
- completed 阶段默认写结构化证据，不写长总结

**目录一致性原则：**

- `docs/dev/in-progress/` 只允许承载 `lifecycle_status: in-progress` 的任务
- 如果任务进入 `completed`、`hanging` 或 `abandoned`，必须同步更新 `STATUS.md` 前置字段、`current_dir`、索引和物理目录
- 绝不允许只在 `STATUS.md` 正文、任务摘要或父 roadmap 表格里写“已完成/已挂起/已放弃”，但当前任务目录仍停留在 `docs/dev/in-progress/`

**父子任务原则：**

- 带有 `parent_task_id` 或 `source_relation: roadmap-child*` 的任务，仍然是独立生命周期任务
- child task 到达终态时，先归档当前 child task，再回写父任务的 child mapping、`latest_stop_point`、下一步与索引
- 不要只更新父任务摘要，而跳过当前 child task 的终态归档

## 文档语言要求

除非用户明确要求其他语言，控制器写入的 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md`、任务摘要，以及通过 `dev:project-wiki-maintenance` 触发的知识文档都必须使用中文。代码、命令、路径、标识符保持原文。

## 流程

### 第零步：检查活动任务

仅在你即将开始新的复杂计划执行、切换到另一份计划，或恢复已有计划时使用这一步。

1. 检查 `docs/dev/ACTIVE.md` 是否存在
2. 如果不存在：继续进入“第一步：加载计划”
3. 如果存在：
   - 读取 `docs/dev/ACTIVE.md`
   - 根据其中的 `status_file` 读取对应任务目录下的 `STATUS.md`
   - 提取任务名、`task_id`、`task_dir`、`lifecycle_status`、`phase`、最近摘要、下一步
4. 如果 `ACTIVE.md` 指向当前任务：直接恢复执行
5. 如果 `ACTIVE.md` 指向另一个任务：在处理完成前，不要开始新的计划执行

### 第一步：加载计划

1. 读取当前任务目录中的计划文件一次
2. 确认任务总数
3. 提取所有任务的完整文本和上下文
   - 当前 `STATUS.md` 中的 `blocking_questions` / `question_clearance` / `clearance_confirmed_by_user`
   - 当前 `STATUS.md` 中的 `clearance_source` / `preflight_review` / `preflight_summary` / `auto_execution_decision`
4. 创建或更新：
   - 当前任务目录中的 `STATUS.md`
   - `docs/dev/ACTIVE.md`
   - `docs/dev/INDEX.md` 与对应生命周期索引

## 执行前输入守卫

- 控制器在启动任何子 Agent 前，必须确认当前任务仍满足：
  - `blocking_questions: []`
  - `question_clearance: cleared`
  - `clearance_confirmed_by_user: true`
- 如果当前任务是 `roadmap-child*`，或 `clearance_source: parent-roadmap`：
  - 默认按父 roadmap 的 `child_execution_mode` / `auto_continue` / `auto_stop_policy` 执行
  - 不为本 child 再次询问用户启动确认，除非 `preflight self-review = stop`
- 若 `preflight_review` 缺失，控制器必须先补做一次最小 `preflight self-review`
- 最小 `preflight self-review` 检查项：
  - 当前 child 是否仍落在父 roadmap 已批准边界内
  - 是否出现新的边界漂移、authority 变化、职责变化或关键结构改动
  - `depends_on`、阶段依赖、`requiredGates` 与 obligation 是否已满足
  - 是否出现新的执行前问题，无法用已批准输入直接解出
  - 是否新增 formal object、proof、benchmark、hotupdate 或其它必须先确认的义务
  - 当前 `ACTIVE.md`、目录状态与父子任务指针是否一致
- 结果只允许：
  - `preflight_review: pass`
  - `preflight_review: warn`
  - `preflight_review: stop`
- 对应动作：
  - `pass`：写 `auto_execution_decision: continue`，继续派发
  - `warn`：写摘要到 `STATUS.md`，写 `auto_execution_decision: continue`，继续派发
  - `stop`：写 `auto_execution_decision: stop`，停止自动推进，并向用户说明问题后等待确认

### 第二步：逐任务执行

对每个任务：

1. 派发实现者子 Agent，直接提供完整任务文本
2. 如果实现者需要更多上下文，补充后重新派发
3. 实现者完成后，先进行规格合规性审查
4. 规格合规性通过后，再进行代码质量审查
5. 如任一审查发现问题：
   - 由同一个实现者修复
   - 重新审查
   - 直到通过
6. 任务实现收敛后，完成一次最新结构告警与架构审视
   - 优先使用仓库内已有的自动结构告警；如果仓库尚无脚本，则按相同检查项手动完成
   - 结构告警只分 `info / warn / blocker`
   - `warn` 由控制器在推进中处理或记录，不单独拉人工复核流程
   - `blocker` 必须在进入 `completed` 前解决
7. 任务完成后更新：
   - `STATUS.md`
   - 仅在当前活动任务、阶段或下一步摘要变化时更新 `docs/dev/ACTIVE.md`
   - 仅在阻塞、重要决策或验证失败时追加 `notes/progress-*.md`
   - 仅在生命周期、目录位置或可见入口变化时更新索引文件
   - 必要时的 wiki 页面
8. 如果当前任务带有 `parent_task_id`：
   - 同步父任务中的 child mapping、最近摘要、`latest_stop_point` 与下一步
   - 确保父任务文档中的 child 状态与当前任务目录生命周期一致
   - 如果父 roadmap 写明 `child_execution_mode: auto` 且 `auto_continue: true`：
     - 优先读取父任务中的 `recommended_next_child`
     - 如果存在唯一、依赖已满足的下一子任务：默认自动启动或恢复它，不额外询问用户
     - 只有当下一子任务的 `preflight self-review = stop`、下一项不唯一、或父 roadmap 明确要求 `manual` 时，才停下来确认

### 结构告警与架构审视

每次子 Agent 实现收敛到一个可提交批次后，控制器都要完成最新一轮结构告警与架构审视。目标是让结构问题在推进中暴露，而不是全部堆到最终 completed 前。

固定检查项：

- 是否出现重复代码、平行 helper、重复 emitter/mapper 或临时兼容逻辑复制
- 是否需要调整结构、补抽象，或回收已经失去价值的抽象
- 是否存在单文件混入多种职责，尤其是策略决策、数据映射、输出拼装耦合在一起
- 对 `codegen`，是否应把输出拼装下沉到 `Scriban`；或反过来把复杂语义决策从 `Scriban` 提回宿主代码

约束：

- 不使用固定行数作为 gate；文件大小只能作为上下文信号
- 不引入独立人工复核流程；`warn` 由控制器在推进中处理
- 最新一轮结构告警必须覆盖当前 diff 或当前批次涉及的文件
- `arch_review_mode = critical` 的任务，除上述检查外，还必须补齐最小 `权责图审核`

`critical` 任务的最小 `权责图审核` 至少包括：

- `authority`
- `upstream`
- `downstream`
- `allowed_deps`
- `shim_debt`
- `proof`

### 第三步：完成

所有任务完成后，也必须按固定顺序收尾：`结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交`。

1. 先完成最新一轮结构告警与架构审视；发现未解决 `blocker` 就回到实现阶段
   - 覆盖最终 diff 或最终受影响文件集合
   - 重点确认重复逻辑、结构/抽象、文件职责拆分、`codegen` 与 `Scriban` 分层
   - 如果任务是 `critical`：确认最小 `权责图审核` 已补齐
2. 对 obligation-driven 任务先消费 `dev:verification-before-completion`
   - 计划出现 `ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact`、`formalVerificationObjects`、`requiredGates` 等字段时，视为 obligation-driven
   - 至少确认 `requiredGates`、managed proof、native proof、按需的 hotupdate proof / benchmark、以及 plan 声明的 `formalVerificationObjects`
3. 再运行受影响项目级测试套件
4. 如果测试阶段出现 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃：
   - 当前验证立即失败
   - 收集 `stderr`、`binlog`、崩溃堆栈或 dump 信息（如果可用）
   - 使用 `dev:systematic-debugging` 查明根因并修复
5. 只有在以下条件都满足后，才允许归档：
   - 所有任务完成
   - 所有审查通过
   - formal verification gate 通过
   - 如本轮要求新的 verification 数据：`verification-v1` formal source 已刷新完成
   - 最新结构告警不存在未解决 `blocker`
   - `critical` 任务的最小 `权责图审核` 已完成
   - 受影响项目测试通过
   - 必要 wiki 更新完成
6. 满足后：
   - 更新 `STATUS.md` 为 `completed`
   - 如当前任务此前已使用 `notes/progress-*.md`，或确有必要沉淀最终失败/决策，再追加最终进展记录
   - 将任务目录移动到 `docs/dev/completed/`
   - 删除 `docs/dev/ACTIVE.md`
   - 更新索引文件
   - 如果存在 `parent_task_id`：回写父任务的 child mapping、`latest_stop_point`、下一步与对应索引
   - 使用 `dev:finishing-a-development-branch`

### Completed 结构化证据卡

completed 阶段默认在 `STATUS.md` 中写入简短、结构化的证据卡，而不是长篇叙述。最小字段建议如下：

```markdown
## 完成证据
- arch_review: [ok / blocker-fixed / n/a]
- authority_review: [ok / n/a]
- verification:
  - canonicalCommand: [...]
  - formalObject: [...]
  - summaryPath: [...]
  - eventsPath: [...]
  - consolePath: [...]
- verification_data:
  - refreshCommand: [... / n/a]
  - verificationOutputRoot: [... / n/a]
  - latestResultPath: [... / n/a]
  - masterResultPath: [... / n/a]
  - reportSummaryPath: [... / n/a]
  - codegenStubPath: [... / n/a]
- test_result: [passed / failed]
- wiki: [updated:<path> / n/a]
- next: [merge / follow-up / n/a]
```

如果某项不存在，写 `n/a`。`summaryPath` / `eventsPath` / `consolePath` 只要仓库提供，就应写入；如本轮要求新的 verification 数据，则 `verification_data.*` 也必须同步写入，保证后续恢复与复查可重新定位正式产物。

## 审查规则

- 规格合规性审查者必须保持怀疑态度，不信任实现者的自报
- 审查者必须独立阅读代码
- 如果审查者发现问题，流程必须进入修复循环

## 红旗

**绝不：**

- 跳过规格合规性审查
- 在规格合规性通过前开始代码质量审查
- 让子 Agent 自己去读计划文件
- 用“任务都做完了”替代 formal verification gate
- 只拿运行日志就声称“新测试流程已打通”，却没有刷新 `verification-v1` formal source
- 用固定文件行数直接判定失败
- 把 `warn` 升级成独立人工复核流程
- 让 `STATUS.md`、`ACTIVE.md` 或索引落后于当前进度
- 在当前任务目录仍位于 `docs/dev/in-progress/` 时，只在正文或父 roadmap 里写 `completed`
- 在没有先处理 `docs/dev/ACTIVE.md` 指向任务的情况下，直接启动新的并行执行
- 因为 `warn` 级别问题就中断 roadmap-child 的自动推进

## 集成

- `dev:project-wiki-maintenance`
- `dev:verification-before-completion`
- `dev:systematic-debugging`
- `dev:finishing-a-development-branch`
- 子 Agent 应使用 `dev:test-driven-development`
