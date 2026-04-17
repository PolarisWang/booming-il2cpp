# AOT Skill Flow And Token Optimization Design v1.01

Date: 2026-04-17
Status: design-frozen

## 1. 目标

把当前 skill 流程优化成一条更适合 IL2CPP/AOT 主线的正式工作流，使它在处理 `managed -> native -> hotupdate`、`proof / benchmark obligation` 和 `completed` 收口时满足以下要求：

- capability、owner subject、proof / benchmark obligation 在 plan 阶段显式冻结，而不是隐式依赖作者记忆；
- `completed` 之前必须经过 formal verification gate，而不是只跑一轮泛化“项目测试套件”；
- 高频 skill 只保留 routing / gate / decision，长篇 reference 回落到 architecture 与 `wiki/06-测试验证/`；
- 多个 skill 之间对同一条规则只保留一个 authority，不继续复制粘贴式增长。

## 2. 当前问题

### 2.1 authority 已有，但 control skill 没有完全消费

当前 authority 已存在于：

- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
- `wiki/06-测试验证/INDEX.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`

但 control skill 仍存在两个缺口：

- `writing-plans` 没有要求显式写出 `capabilityFamily / capabilityItem / ownerSubjectId / proofRequired / benchmarkRequired / hotupdateImpact`；
- `executing-plans` 与 `subagent-driven-development` 还没有把 AOT formal verification chain 变成 `completed` 之前的硬门。

### 2.2 高频 skill 重复携带同一条规则

以下规则目前被多个 skill 重复叙述：

- `dotnet build/test/msbuild` 编译崩溃必须追根因并修复
- `completed` 前必须验证
- benchmark 不能替代 correctness
- AOT 主线必须走 managed/native/hotupdate 正式验证

重复叙述带来的问题不是“字数不好看”，而是：

- token 消耗偏高；
- authority 容易漂移；
- 修改规则时容易漏改某一份镜像。

### 2.3 AOT onboarding 缺少快路径

当前仓库已经有非常明确的 AOT feature onboarding 规则，但 skill 主线还没有为这类高频、标准化任务提供足够轻量的入口。结果是：

- 复杂度已经被 architecture / wiki 冻结，但 skill 仍然每次都要重复携带大量说明；
- `using-booming` 能正确分流，但后续 skill 仍可能把任务执行成“泛化计划”而不是“AOT obligation-driven plan”。

## 3. 设计原则

### 3.1 authority 与 control 分层

- authority：`docs/architecture` 与 `wiki/06-测试验证/`
- control：`.codex/skills/**`

authority 负责说明长期规则，control 只负责：

- 什么时候必须进入哪条规则；
- 完成前必须过哪些 gate；
- 如果缺规则对象，下一步去哪里补。

### 3.2 plan 必须冻结 obligation

所有 AOT / IL2CPP 主线的正式计划，都必须在 plan 文档里冻结：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- 准备执行的 declared proof / benchmark 或 formal verification object

### 3.3 completed 必须绑定 formal verification chain

对 AOT feature onboarding 类任务，`completed` 之前的正式执行链固定为：

`collector/registry/workspace gate -> owner managed proof -> owner native proof -> hotupdate proof(按需) -> benchmark(按需) -> affected regressions`

其中 benchmark 只作为补充证据，不替代 correctness。

### 3.4 高频 skill 采用薄入口

高频 skill 应缩成：

- 短 description
- 核心 gate
- 指向 authority 的明确入口

不应在每个 skill 正文里都完整重讲一遍 owner subject、proof obligation、collector gate 与 benchmark 边界。

## 4. 目标结构

### 4.1 control skill 分工

- `using-booming`: 负责 routing，明确哪些任务必须先过 `project-test-governance`
- `project-test-governance`: 负责治理边界与 authority 入口，不负责完整重讲所有 AOT onboarding 细节
- `writing-plans`: 负责把 capability / owner / obligation 写进 plan
- `executing-plans` / `subagent-driven-development`: 负责在 `completed` 前强制 formal verification gate
- `verification-before-completion`: 负责选择正式对象、执行 `canonicalCommand`、处理 verification escalation

### 4.2 authority 文档分工

- `docs/architecture/managed-native-hotupdate-test-pipeline.md`: 主线边界、长期契约、分层原则
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`: onboarding 顺序、gate、proof / benchmark obligation
- `wiki/06-测试验证/INDEX.md`: 正式入口与对象导航

## 5. 成功标准

完成后应满足：

- 一个 AOT 新 feature 进入 plan 时，不需要作者额外口头补充 owner/proof/benchmark obligation；
- `completed` 前无法只靠 managed green 或 benchmark green 就宣布完成；
- `project-test-governance` skill 正文与其 wiki 镜像、architecture、AOT 自测规范不再漂移；
- 高频热路径 token 成本相比当前组合下降约 25%~35%，主要来自去重，而不是删规则；
- 通过 scenario-based 验证，确认 skill 仍能正确处理：
  - AOT 新 feature 接入
  - benchmark 异常排查
  - `completed` 前 formal verification gate
