---
name: executing-plans
description: 当你有一个已写好的实现计划，需要在当前会话中按批次执行、维护任务状态，并在必要时处理活动任务冲突时使用
---

# 执行计划

## 概述

加载计划，批判性地审查，执行所有任务，并以 `STATUS.md` 作为主记录维护执行真相；`docs/dev/ACTIVE.md` 仅维护当前活动任务指针；`notes/progress-*.md` 只在阻塞、重要决策或验证失败时追加；索引文件只在生命周期或可见入口发生变化时更新�?
计划收尾必须进入固定闭环：`结构告警与架构审�?-> 测试通过 -> 归档 completed -> 合并&提交`�?
对于来自已批�?roadmap �?child task，默认目标是自动继续执行，而不是在每一步都回头询问用户。执行前只做最�?`preflight self-review`；只要结果是 `pass` �?`warn`，就继续推进。只有命�?`stop` 条件，才停下来确认�?
**开始时宣布�?* "我正在使�?executing-plans 技能来实现这个计划�?

**注意�?* `dev:subagent-driven-development` 是另一条执行路径。只有当它具备与本技能等价的活动任务处理和状态维护规则时，才应切换过去�?
**状态原则：** 任务目录中的 `STATUS.md` 是执行真相源；`docs/dev/ACTIVE.md` 只是当前活动任务指针�?
**文档压缩原则�?*

- 普通推进以更新 `STATUS.md` 为主
- `docs/dev/ACTIVE.md` 只在当前活动任务、阶段或下一步摘要发生变化时更新
- 不要�?`notes/progress-*.md` 当作每一步都要追加的流水�?- 不要为了一次普通推进同时改 `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md`、索引和 wiki，除非这些对象真的都发生了变�?- completed 阶段默认写结构化证据，不写长总结

**目录一致性原则：**

- `docs/dev/in-progress/` 只允许承�?`lifecycle_status: in-progress` 的任�?- 如果任务进入 `completed`、`hanging` �?`abandoned`，必须同步更�?`STATUS.md` 前置字段、`current_dir`、索引和物理目录
- 绝不允许只在 `STATUS.md` 正文、计划复选框或父 roadmap 表格里写“已完成/已挂�?已放弃”，但当前任务目录仍停留�?`docs/dev/in-progress/`

**父子任务原则�?*

- 带有 `parent_task_id` �?`source_relation: roadmap-child*` 的任务，仍然是独立生命周期任�?- child task 到达终态时，先归档当前 child task，再回写父任务的 child mapping、`latest_stop_point`、下一步与索引
- 不要只更新父 roadmap 或父任务摘要，而跳过当�?child task 的终态归�?
## 文档语言要求

除非用户明确要求其他语言，由本技能创建或更新�?`STATUS.md`、`ACTIVE.md`、`notes/progress-*.md`、索引摘要、风险记录、阻塞说明都必须使用中文。代码、命令、路径、标识符保持原文�?

## 第零步：检查活动任�?

仅在你即将开始新的复杂计划执行、切换到另一份计划，或恢复已有计划时使用这一步�?

1. 检�?`docs/dev/ACTIVE.md` 是否存在�?
2. 如果不存在：继续进入“加载并审查计划”�?
3. 如果存在�?
   - 读取 `docs/dev/ACTIVE.md`
   - 根据其中�?`status_file` 读取对应任务目录下的 `STATUS.md`
   - 提取任务名、`task_id`、`task_dir`、`lifecycle_status`、`phase`、最近摘要、下一�?
4. 如果 `ACTIVE.md` 指向当前任务：直接恢复执行�?
5. 如果 `ACTIVE.md` 指向另一个任务：在处理完成前，不要开始新的计划执行�?

## 第一步：加载并审查计�?
1. 读取当前任务目录中的计划文件一�?2. 明确识别计划中的任务总数
3. 提取执行需要的关键引用�?   - 当前 `STATUS.md` 中的 `blocking_questions`
   - 当前 `STATUS.md` 中的 `question_clearance`
   - 当前 `STATUS.md` 中的 `clearance_confirmed_by_user`
   - 当前 `STATUS.md` 中的 `clearance_source`
   - 当前 `STATUS.md` 中的 `preflight_review`
   - 当前 `STATUS.md` 中的 `preflight_summary`
   - 当前 `STATUS.md` 中的 `auto_execution_decision`
   - 设计文档路径
   - 计划文档路径
   - 每个任务的完整文�?   - 每个任务的知识沉淀目标
4. 批判性审查计划，识别任何疑虑
   - 如果发现执行前问题尚未清零，或清零状态缺失且无法从已批准输入中确认：停止执行，返�?`brainstorming` �?`writing-plans`
   - 不要把执行阶段变成补设计拍板或补 open questions 的地�?5. 如无疑虑：创建或更新
   - 当前任务目录中的 `STATUS.md`
   - `docs/dev/ACTIVE.md`
   - `docs/dev/INDEX.md` 与对应生命周期索�?
## 执行前输入守�?
- 开始实现前，必须确认当前任�?`STATUS.md` 至少满足�?  - `blocking_questions: []`
  - `question_clearance: cleared`
  - `clearance_confirmed_by_user: true`
- 如果任务来自 `roadmap-child`、`plan` 或恢复执行，但上述条件不成立�?  - 不得继续实现
  - 不得在执行阶段自行补拍板后假定问题已关闭
  - 返回上游 `brainstorming` �?`writing-plans` 修正输入
- 执行阶段允许继续提出的问题，只能是：
  - 当前实现遇到�?`blocker`
  - 验证失败或编译崩溃的根因
  - 结构告警与架构审视发现的实现级问�?- 执行阶段不负责继续确认边界、authority、阶段切分或关键结构方案

## roadmap-child 自动执行守卫

- 如果当前任务带有 `source_relation: roadmap-child*`，或 `clearance_source: parent-roadmap`�?  - 默认按父 roadmap �?`child_execution_mode` / `auto_continue` / `auto_stop_policy` 执行
  - 若父 roadmap �?`child_execution_mode = auto`，则不为�?child 再次向用户索要启动确�?- 开始实现前，必须存在一次最�?`preflight self-review`
  - 如果 `preflight_review` 已存在：直接消费
  - 如果缺失：先补做一次最�?`preflight self-review` 并写�?`STATUS.md`
- 最�?`preflight self-review` 检查项�?  - 当前 child 是否仍处于父 roadmap 已批准边界内
  - 是否出现新的边界漂移、authority 变化、职责变化或关键结构改动
  - `depends_on`、阶段依赖、`requiredGates` �?obligation 是否已满�?  - 是否出现新的执行前问题，无法直接从父 roadmap / plan / design 输入中解�?  - 是否新增 formal object、proof、benchmark、hotupdate 或其它必须先确认的义�?  - 当前 `ACTIVE.md`、目录状态、父子任务指针是否一�?- 结果只允许：
  - `preflight_review: pass`
  - `preflight_review: warn`
  - `preflight_review: stop`
- 对应动作�?  - `pass`：写 `auto_execution_decision: continue`，直接继�?  - `warn`：把风险写入 `STATUS.md`，写 `auto_execution_decision: continue`，继�?  - `stop`：写 `auto_execution_decision: stop`，停止自动推进，并向用户说明问题后等待确�?- 只有以下情况才应判为 `stop`�?  - 新的执行前问题无法用已批准输入解�?  - 子任务范围已超出�?roadmap 已批准边�?  - authority / owner / contracts / required gates 发生变化
  - 必须新增 proof / benchmark / formal object
  - 前置依赖未满足，继续推进会造成错误执行
  - �?roadmap 本身需要修改，而不�?child 内部实现即可解决
- 以下通常只应判为 `warn`，不要因此打断用户：
  - 实现级结构风�?  - 文件偏大或可进一步拆�?  - 重复逻辑苗头但不阻塞当前推进
  - 可在当前执行批次内消化的架构修整

## 第二步：执行任务

对每个任务：

1. 在开始实现前，先把当前任务编号或任务名称写入 `STATUS.md` �?`docs/dev/ACTIVE.md`
2. 严格按照计划步骤执行
3. 按规定运行验�?4. 完成一次最新的结构告警与架构审�?   - 优先使用仓库内已有的自动结构告警；如果仓库尚无脚本，则按相同检查项手动完成
   - 结构告警只分 `info / warn / blocker`
   - `warn` 由当前执行者当场判断并处理或记录，不单独拉人工复核流程
   - `blocker` 必须在进�?`completed` 前解�?5. 更新 `STATUS.md`
   - 最近摘�?   - 下一�?   - 验证结果
   - 最新结构告警结�?   - 风险与阻�?   - wiki 决策
6. 仅在当前活动任务、阶段或下一步摘要变化时更新 `docs/dev/ACTIVE.md`
7. 仅在以下情况追加 `notes/progress-*.md`
   - 遇到阻塞
   - 做出重要结构/流程决策
   - 验证失败
8. 仅在生命周期、目录位置或可见入口变化时更新索引文�?9. 判断是否产生长期有效知识
   - 如果有：使用 `dev:project-wiki-maintenance`
   - 如果没有：在 `STATUS.md` 中记�?`wiki: n/a`
10. 如果当前任务带有 `parent_task_id`�?   - 同步父任务中�?child mapping、最近摘要、`latest_stop_point` 与下一�?   - 确保父任务文档中�?child 状态与当前任务目录生命周期一�?   - 如果�?roadmap 写明 `child_execution_mode: auto` �?`auto_continue: true`�?     - 优先读取父任务中�?`recommended_next_child`
     - 如果存在唯一、依赖已满足的下一子任务：默认自动启动或恢复它，不额外询问用户
     - 只有当下一子任务的 `preflight self-review = stop`、下一项不唯一、或�?roadmap 明确要求 `manual` 时，才停下来确认

## 结构告警与架构审�?
每次任务实现收敛到一个可提交批次后，都要完成最新一轮结构告警与架构审视。核心目的是尽早发现结构问题，而不是在 completed 前临时做一次大审判�?
固定检查项�?
- 是否出现重复代码、平�?helper、重�?emitter/mapper 或临时兼容逻辑复制
- 是否需要调整结构、补抽象，或回收已经失去价值的抽象
- 是否存在单文件混入多种职责，尤其是策略决策、数据映射、输出拼装耦合在一�?- �?`codegen`，是否应把输出拼装下沉到 `Scriban`；或反过来把复杂语义决策�?`Scriban` 提回宿主代码

约束�?
- 不使用固定行数作�?gate；文件大小只能作为上下文信号
- 不引入独立人工复核流程；`warn` 由当前执行者在推进中处�?- 最新一轮结构告警必须覆盖当�?diff 或当前批次涉及的文件
- `arch_review_mode = critical` 的任务，除上述检查外，还必须补齐最�?`权责图审核`

`critical` 任务的最�?`权责图审核` 至少包括�?
- `authority`
- `upstream`
- `downstream`
- `allowed_deps`
- `shim_debt`
- `proof`

## AOT / Obligation-Driven Completion Gate

如果计划里出现以下任一字段，或任务明显命中 AOT / IL2CPP / test governance 主线，则本次执行属于 obligation-driven�?

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

�?obligation-driven 任务，`completed` 前必须显式消�?`dev:verification-before-completion`，并至少确认�?

- `requiredGates` 指定的接线闸门；未显式指定时默认 `collector -> registry -> workspace`
- owner managed proof
- owner native proof
- `hotupdateImpact = Smoke/Proof` 时的 hotupdate proof
- `benchmarkRequired = true` 时的 benchmark
- 受影�?regression
- plan 中列出的全部 `formalVerificationObjects`

如果 authority 要求正式对象�?case，但仓库里还不存在，不得跳过；先补对象或 case，再继续完成链路�?

## 第三步：完成开�?
所有任务完成后，必须按固定顺序收尾：`结构告警与架构审�?-> 测试通过 -> 归档 completed -> 合并&提交`�?
1. 完成最新一轮结构告警与架构审视
   - 覆盖最�?diff 或最终受影响文件集合
   - 重点确认重复逻辑、结�?抽象、文件职责拆分、`codegen` �?`Scriban` 分层
   - 如果存在未解�?`blocker`：回到任务执行阶段继续修�?   - 如果任务�?`critical`：确认最�?`权责图审核` 已补�?2. 运行 formal verification gate 与项目级测试
   - obligation-driven 任务先按 `dev:verification-before-completion` 选择并执行正式对�?   - 再运行受影响项目级测试套件或项目约定的完整命�?   - 如果任务命中 `dev:project-test-governance`、需要新的验证通过记录 / 新的数据，或本轮改动触及 `verification-v1` consumer / projection / codegen-stubs：在 formal object 与项目测试通过后，必须执行 `run verify verification-v1 --json` 刷新正式产物
   - 只有�?`run verify verification-v1 --json` 返回�?`artifacts` 中出现最�?`verification/archive/latest/*`、`master/*`、`reports/*`，且命中 codegen 主线时出�?`verification/evidence/owners/*/codegen-stubs/*`，本轮才算真正打通新测试流程
   - 如果测试阶段出现 `dotnet build` / `dotnet test` / `msbuild` 编译崩溃�?     - 本次验证立即视为失败
     - 收集 `stderr`、`binlog`、崩溃堆栈或 dump 信息（如果可用）
     - 使用 `dev:systematic-debugging` 定位并修复根�?     - 根因未修复前，不得继续归档或收尾
3. 将结构告警、架构审视结论与验证结果写入 `STATUS.md`
   - 如果本轮执行�?`run verify verification-v1 --json`，同时写入新�?formal verification 数据路径，而不只是运行日志路径
   - 如果 `docs/dev/ACTIVE.md` 仍指向当前任务，再同步其中的摘要指针
   - 只有在重要决策或验证失败时才需要额外写�?`notes/progress-*.md`
   - 默认使用结构化证据卡，不写长篇完成总结
4. 如果任一验证失败�?   - 记录失败摘要、定位信息、下一�?   - 停止，不要归档为 `completed`
5. 只有在以下条件都满足后，才允许归档为 `completed`�?   - 计划任务全部完成
   - formal verification gate 通过
   - 如本轮要求新�?verification 数据：`verification-v1` formal source 已刷新完�?   - 最新结构告警不存在未解�?`blocker`
   - `critical` 任务的最�?`权责图审核` 已完�?   - 受影响项目测试通过
   - 本轮应写入的 wiki 更新已完�?6. 满足后：
   - 更新 `STATUS.md` �?`completed`
   - 如当前任务此前已使用 `notes/progress-*.md`，或确有必要沉淀最终失�?决策，再追加最终进展记�?   - 将任务目录移动到 `docs/dev/completed/`
   - 删除 `docs/dev/ACTIVE.md`
   - 更新索引文件
   - 如果存在 `parent_task_id`：回写父任务�?child mapping、`latest_stop_point`、下一步与对应索引
   - 如果�?roadmap 允许自动续跑，且存在 `recommended_next_child`：优先自动切换到下一 child，而不是停下来等待额外确认
   - 宣布�?我正在使�?finishing-a-development-branch 技能来完成这项工作�?
   - 使用 `dev:finishing-a-development-branch`

### Completed 结构化证据卡

completed 阶段默认�?`STATUS.md` 中写入简短、结构化的证据卡，而不是长篇叙述。最小字段建议如下：

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

如果某项不存在，�?`n/a`，不要为了“完整叙述”补长段说明。`summaryPath` / `eventsPath` / `consolePath` 只要仓库提供，就应写入；如本轮要求新�?verification 数据，则 `verification_data.*` 也必须同步写入，保证后续恢复与复查可重新定位正式产物�?
## 何时停下来寻求帮�?

遇到以下情况立即停止执行�?
- 遇到阻塞（缺少依赖、测试失败、指令不清晰�?- 计划存在关键缺口导致无法开�?- 你不理解某条指令
- 验证反复失败
- roadmap-child �?`preflight self-review = stop`
- �?roadmap 边界、authority 或执行策略需要重�?
在停下来之前，把新的风险、阻塞和下一步记录进 `STATUS.md`。如果本次停下属于阻塞、重要决策或验证失败，再同步写入最�?`notes/progress-*.md`�?
如果当前任务需要停在非 `in-progress` 终态，必须同时完成目录移动与索引同步；不能把终态只写在正文里�?
## 集成

- `dev:using-git-worktrees`
- `dev:project-wiki-maintenance`
- `dev:verification-before-completion`
- `dev:systematic-debugging`
- `dev:finishing-a-development-branch`

