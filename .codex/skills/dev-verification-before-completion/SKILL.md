---
name: verification-before-completion
description: 在即将声称工作完成、已修复或通过时使用，在提交或创建 PR 之前使用——需要运行验证命令并在做出任何成功声明之前确认输出；始终先有证据再做断言
---

# 完成前验证

## 概述

在没有验证的情况下声称工作完成是不诚实，而非高效。

**核心原则：** 先有证据，后有声明，始终如此。

## 铁律

```text
在没有最新验证证据的情况下，不得做出任何完成声明
```

如果你在本条消息中没有运行验证命令，你不能声称它通过了。

## 门函数

```text
在声明任何状态之前：

1. 识别：哪个命令证明这个声明？
2. 运行：执行完整命令
3. 阅读：检查退出码、失败数、关键输出
4. 验证：输出是否真的支持该声明？
5. 只有到那时：才做出声明
```

## 常见失败

| 声明 | 需要 | 不足够 |
| --- | --- | --- |
| 测试通过 | 最新测试输出，且失败数为 0 | “应该通过” |
| 构建成功 | 构建命令退出 0 | linter 通过 |
| `dotnet` 编译稳定 | `dotnet build/test/msbuild` 退出 0，且没有崩溃 | 多重试几次后暂时不崩 |
| bug 已修复 | 原始症状对应的验证重新通过 | “代码已经改了” |
| agent 完成了 | 独立检查 diff 并重新验证 | 信任 agent 报告 |
| 可以归档 completed | formal verification gate + 受影响测试 + wiki 都完成，且最新结构告警无未解决 `blocker` | “任务做完了” |

## 红旗

- 使用“应该”、“可能”、“似乎”
- 在验证之前表达满意或完成
- 依赖部分验证
- 试图在没有 formal object 的情况下归档 `completed`
- 疲惫并想要直接结束

## 项目绑定

- 项目级测试知识只认 `wiki/06-测试验证/INDEX.md` 这一正式入口
- selector 信息优先读取 `artifacts/tests/registry/current/index.json` 或执行 `run test registry list --json`
- registry 给出 `canonicalCommand` 时，优先直接执行该命令
- registry 给出 `docRefs` 时，再回到 `wiki/06-测试验证/` 读取对象说明
- 如果用户明确要求“新的验证通过记录 / 新的数据”，或任务命中 `dev:project-test-governance` / `verification-v1` 主线，则 formal objects 与 regression 通过后还必须刷新 formal source；默认执行 `run test inventory --json`
- 只有 `summaryPath` / `eventsPath` / `consolePath`，但没有最新 `docs/testing-inventory/verification/{latest,master,reports}` 或 `subjects/*/verification/codegen-stubs/*` 更新时，不足以宣称“新测试流程已打通”
- `docs/dev/**` 只记录任务过程，不是长期 authority

## 结构告警与架构审视闸门

在声称“可以 completed”之前，除 formal verification 与受影响测试外，还必须确认最新一轮结构告警与架构审视已经完成。

规则如下：

- 结构告警只分 `info / warn / blocker`
- 只有未解决的 `blocker` 会阻止 `completed`
- `warn` 由当前执行者在推进过程中处理或记录，不创建独立人工复核流程
- 不得把固定文件行数当作失败依据；文件大小只能作为上下文，真正要检查的是结构信号

必须检查的结构信号：

- 是否引入重复代码、平行 helper、重复 emitter/mapper、临时兼容逻辑复制
- 是否需要调整结构、补抽象，或回收失去价值的抽象
- 是否存在单文件混入多种职责，尤其是策略决策、数据映射、输出拼装耦合
- 对 `codegen`，是否把应属于 `Scriban` 的输出拼装堆在宿主代码，或把应属于宿主代码的复杂语义决策堆进 `Scriban`

如果仓库提供自动结构告警，优先使用自动结果；如果没有，则按同等检查项完成手动架构审视。

## Obligation-Driven AOT 任务

如果计划或任务文档中出现以下任一字段，或任务明显命中 AOT / IL2CPP / test governance 主线，则必须按 obligation-driven 模式验证：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

执行顺序：

1. 先读取计划中的 obligation 字段
2. 如果计划已写明 `formalVerificationObjects`，先执行这些正式对象
3. 再确认最小 gate：
   - `requiredGates`；未显式指定时默认 `collector -> registry -> workspace`
   - owner managed proof
   - owner native proof
   - `hotupdateImpact = Smoke/Proof` 时的 hotupdate proof
   - `benchmarkRequired = true` 时的 benchmark
   - 受影响 regression
4. 如果 authority 要求正式对象或 case，但 registry 里不存在：
   - 先补对象或 case
   - 再运行它
   - 在此之前不得宣称完成

## 主线权责图审核

如果任务触及 `AOT`、`IL2CPP`、`codegen`、`contracts`、`loader`、`test governance` 任一主线，在声称“可以 completed”之前，还必须确认最小 `权责图审核` 已完成。

最小字段如下：

- `authority`
- `upstream`
- `downstream`
- `allowed_deps`
- `shim_debt`
- `proof`

缺少上述字段中的任一关键项时，不得宣称主线任务已通过架构闸门。

## 测试阶段 `dotnet` 编译崩溃闸门

在任何测试、构建或验证命令中，如果 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃：

- 本次验证立即视为失败
- 必须先读取崩溃原因，定位失败的 project / target / task
- 必须保留并检查 `stderr`、`binlog`、崩溃堆栈或 dump 信息（如果可用）
- 必须修复根因并重新运行受影响的验证对象
- 在根因未查明、未修复前，不得声称“测试通过”、“问题已修复”或“可以完成”

## 重要验证升格

- 修复真实 bug 的回归验证，必须沉淀为正式 case
- 被认定为 `required-before-completion` 的验证，必须沉淀为正式 case、`module/...` 或 `system/...` 对象
- 被 `module/system` 引用的验证，必须沉淀为正式对象

## 结果汇报

汇报验证结果时，默认使用结构化证据而不是长篇结论。至少给出可定位信息：

- `canonicalCommand`
- `summaryPath`
- `eventsPath`
- `consolePath`
- 如本轮要求刷新 verification 数据，还要给出：
  - `refreshCommand`
  - `verificationOutputRoot`
  - `latestResultPath`
  - `masterResultPath`
  - `reportSummaryPath`
  - `codegenStubPath`
- 如存在 `phaseResults[]`，显式说明 `code / module / system` 分层状态

completed 阶段建议使用最小证据卡：

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

**验证没有捷径。**

运行命令。阅读输出。然后声明结果。
