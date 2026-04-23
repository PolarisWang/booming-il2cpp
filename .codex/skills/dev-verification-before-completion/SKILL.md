---
name: verification-before-completion
description: 在即将声称工作完成、已修复或通过时使用，在提交或创建 PR 之前使用——需要运行验证命令并在做出任何成功声明之前确认输出；始终先有证据再做断言
---

# 完成前验�?

## 概述

在没有验证的情况下声称工作完成是不诚实，而非高效�?

**核心原则�?* 先有证据，后有声明，始终如此�?

## 铁律

```text
在没有最新验证证据的情况下，不得做出任何完成声明
```

如果你在本条消息中没有运行验证命令，你不能声称它通过了�?

## 门函�?

```text
在声明任何状态之前：

1. 识别：哪个命令证明这个声明？
2. 运行：执行完整命�?
3. 阅读：检查退出码、失败数、关键输�?
4. 验证：输出是否真的支持该声明�?
5. 只有到那时：才做出声�?
```

## 常见失败

| 声明 | 需�?| 不足�?|
| --- | --- | --- |
| 测试通过 | 最新测试输出，且失败数�?0 | “应该通过�?|
| 构建成功 | 构建命令退�?0 | linter 通过 |
| `dotnet` 编译稳定 | `dotnet build/test/msbuild` 退�?0，且没有崩溃 | 多重试几次后暂时不崩 |
| bug 已修�?| 原始症状对应的验证重新通过 | “代码已经改了�?|
| agent 完成�?| 独立检�?diff 并重新验�?| 信任 agent 报告 |
| 可以归档 completed | formal verification gate + 受影响测�?+ wiki 都完成，且最新结构告警无未解�?`blocker` | “任务做完了�?|

## 红旗

- 使用“应该”、“可能”、“似乎�?
- 在验证之前表达满意或完成
- 依赖部分验证
- 试图在没�?formal object 的情况下归档 `completed`
- 疲惫并想要直接结�?

## 项目绑定

- 项目级测试知识只�?`wiki/06-测试验证/INDEX.md` 这一正式入口
- selector 信息优先读取 `artifacts/tests/registry/current/index.json` 或执�?`run test registry list --json`
- registry 给出 `canonicalCommand` 时，优先直接执行该命�?- registry 给出 `docRefs` 时，再回�?`wiki/06-测试验证/` 读取对象说明
- 如果用户明确要求“新的验证通过记录 / 新的数据”，或任务命中 `dev:project-test-governance` / `verification-v1` 主线，则 formal objects 与 regression 通过后还必须刷新 formal source；默认执行 `run verify verification-v1 --json`。
- `run test inventory` 只是内部命令，不是 public verification entry。
- `benchmark --record` 只写 raw benchmark records，不等于 formal refresh；需要新的 benchmark archive / projection / merged data 时，仍要执行 `run verify verification-v1 --json`。
- 只有 `summaryPath` / `eventsPath` / `consolePath`，但没有最新 `verification/archive/{latest,master,reports}` 或 `verification/evidence/owners/*/codegen-stubs/*` 更新时，不足以宣称“新测试流程已打通”。
- `docs/dev/**` 只记录任务过程，不是长期 authority


## 结构告警与架构审视闸�?
在声称“可�?completed”之前，�?formal verification 与受影响测试外，还必须确认最新一轮结构告警与架构审视已经完成�?
规则如下�?
- 结构告警只分 `info / warn / blocker`
- 只有未解决的 `blocker` 会阻�?`completed`
- `warn` 由当前执行者在推进过程中处理或记录，不创建独立人工复核流程
- 不得把固定文件行数当作失败依据；文件大小只能作为上下文，真正要检查的是结构信�?
必须检查的结构信号�?
- 是否引入重复代码、平�?helper、重�?emitter/mapper、临时兼容逻辑复制
- 是否需要调整结构、补抽象，或回收失去价值的抽象
- 是否存在单文件混入多种职责，尤其是策略决策、数据映射、输出拼装耦合
- �?`codegen`，是否把应属�?`Scriban` 的输出拼装堆在宿主代码，或把应属于宿主代码的复杂语义决策堆进 `Scriban`

如果仓库提供自动结构告警，优先使用自动结果；如果没有，则按同等检查项完成手动架构审视�?
## Obligation-Driven AOT 任务

如果计划或任务文档中出现以下任一字段，或任务明显命中 AOT / IL2CPP / test governance 主线，则必须�?obligation-driven 模式验证�?
- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

执行顺序�?

1. 先读取计划中�?obligation 字段
2. 如果计划已写�?`formalVerificationObjects`，先执行这些正式对象
3. 再确认最�?gate�?
   - `requiredGates`；未显式指定时默�?`collector -> registry -> workspace`
   - owner managed proof
   - owner native proof
   - `hotupdateImpact = Smoke/Proof` 时的 hotupdate proof
   - `benchmarkRequired = true` 时的 benchmark
   - 受影�?regression
4. 如果 authority 要求正式对象�?case，但 registry 里不存在�?   - 先补对象�?case
   - 再运行它
   - 在此之前不得宣称完成

## 主线权责图审�?
如果任务触及 `AOT`、`IL2CPP`、`codegen`、`contracts`、`loader`、`test governance` 任一主线，在声称“可�?completed”之前，还必须确认最�?`权责图审核` 已完成�?
最小字段如下：

- `authority`
- `upstream`
- `downstream`
- `allowed_deps`
- `shim_debt`
- `proof`

缺少上述字段中的任一关键项时，不得宣称主线任务已通过架构闸门�?
## 测试阶段 `dotnet` 编译崩溃闸门

在任何测试、构建或验证命令中，如果 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃�?

- 本次验证立即视为失败
- 必须先读取崩溃原因，定位失败�?project / target / task
- 必须保留并检�?`stderr`、`binlog`、崩溃堆栈或 dump 信息（如果可用）
- 必须修复根因并重新运行受影响的验证对�?
- 在根因未查明、未修复前，不得声称“测试通过”、“问题已修复”或“可以完成�?

## 重要验证升格

- 修复真实 bug 的回归验证，必须沉淀为正�?case
- 被认定为 `required-before-completion` 的验证，必须沉淀为正�?case、`module/...` �?`system/...` 对象
- �?`module/system` 引用的验证，必须沉淀为正式对�?
## 结果汇报

汇报验证结果时，默认使用结构化证据而不是长篇结论。至少给出可定位信息�?
- `canonicalCommand`
- `summaryPath`
- `eventsPath`
- `consolePath`
- 如本轮要求刷�?verification 数据，还要给出：
  - `refreshCommand`
  - `verificationOutputRoot`
  - `latestResultPath`
  - `masterResultPath`
  - `reportSummaryPath`
  - `codegenStubPath`
- 如存�?`phaseResults[]`，显式说�?`code / module / system` 分层状�?
completed 阶段建议使用最小证据卡�?
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
```

## 底线

**验证没有捷径�?*

运行命令。阅读输出。然后声明结果�?



