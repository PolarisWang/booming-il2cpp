---
name: verification-before-completion
description: 在即将声称工作完成、已修复或已通过时使用；在提交或创建 PR 之前使用，需要运行验证命令并先确认输出，再做任何成功声明
---

# 完成前验证

## 核心原则

先有证据，后有声明。

如果当前消息里没有运行验证命令并检查输出，就不能声称“已完成 / 已修复 / 已通过”。

## 闸门

在声明任何状态之前：

1. 识别：哪条命令能证明这个声明
2. 运行：执行完整命令
3. 阅读：检查退出码、失败数和关键输出
4. 验证：确认输出真的支持该声明
5. 只有这时，才能汇报结果

## 常见错误

- 用“应该”“可能”“看起来”代替验证结果
- 只跑部分验证就宣称全通过
- 只看运行日志，不刷新 formal source
- 没有 formal object 就归档 `completed`

## 项目绑定

- 项目级测试知识以 [`wiki/06-测试验证/INDEX.md`](../../../wiki/06-测试验证/INDEX.md) 为正式入口
- selector / registry 信息优先读 `artifacts/tests/registry/current/index.json` 或执行 `run test registry list --json`
- 如果 registry 给出 `canonicalCommand`，优先直接执行
- 如果用户明确要求“新的验证通过记录 / 新的数据”，或任务命中 `dev:project-test-governance` / `verification-v1` 主线，则 regression 通过后还必须刷新 formal source；默认执行 `run verify verification-v1 --json`
- `run test inventory` 不是 public verification entry
- `benchmark --record` 不等于 formal refresh；需要新的 benchmark archive / projection / merged data 时，仍要执行 `run verify verification-v1 --json`
- 只有 `summaryPath` / `eventsPath` / `consolePath`，但没有 `verification/archive/{latest,master,reports}` 或 `verification/evidence/owners/*/codegen-stubs/*` 更新时，不足以声称“新测试流程已打通”
- `docs/dev/**` 只是任务过程记录，不是长期 authority

## Foundation DLL family verification 补充约束

- 如果本轮改动触及 foundation-dll family verification authority、projection、dashboard 或 detail 消费链路，完成前必须判断是否应该刷新：
  - `verification/projections/foundation-dll-audit/family-verification-claims.json`
  - `verification/projections/foundation-dll-audit/family-verification.json`
- 典型命中点包括：
  - `build/toolchains/run/testing/foundation_dll/family_verification_claims.py`
  - `build/toolchains/run/testing/foundation_dll/verification_kernel.py`
  - `build/toolchains/run/testing/foundation_dll_audit_generator.py`
  - `Native Proof Detail`、family progress、tooltip detail
- 命中时，默认需要执行 `run verify verification-v1 --json`，而不是只改 HTML 或局部 JSON
- 完成前至少核对：
  - `verification/projections/foundation-dll-audit/family-verification-claims.json`
  - `verification/projections/foundation-dll-audit/family-verification.json`
  - `verification/projections/foundation-dll-audit/dlls/*.json`
  - `docs/verification/foundation-dll-audit/family-verification-claims.json`

## 结构告警与架构审视

在声称“可以 completed”之前，除了 formal verification 与受影响测试，还必须完成最新一轮结构审视：

- 是否引入重复代码、平铺 helper、临时兼容逻辑复制
- 是否需要补抽象或回收失效抽象
- 是否把策略判断、数据映射、输出拼装混在同一文件
- 如果是 `codegen`，是否把应留在宿主代码的语义判断塞进模板，或反过来

未解决的 `blocker` 会阻止完成声明。

## Obligation-driven 任务

如果任务命中以下任一字段或明显属于 AOT / IL2CPP / test governance 主线，则按 obligation-driven 模式验证：

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

默认顺序：

1. 先跑计划中定义的 formalVerificationObjects
2. 再确认 required gates
3. 再确认 owner managed/native/hotupdate/benchmark obligations
4. 最后确认 regression

## `dotnet` 编译崩溃闸门

只要 `dotnet build` / `dotnet test` / `msbuild` 崩溃：

- 当前验证立即视为失败
- 必须保留 `stderr`、`binlog`、堆栈或 dump
- 必须查明并修复根因后重跑
- 根因未修复前，不得宣称通过

## 结果汇报

汇报验证结果时，至少给出：

- `canonicalCommand`
- `summaryPath`
- `eventsPath`
- `consolePath`

如果本轮要求新的 verification 数据，还要给出：

- `refreshCommand`
- `verificationOutputRoot`
- `latestResultPath`
- `masterResultPath`
- `reportSummaryPath`
- `codegenStubPath`

建议使用最小证据卡：

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

运行命令。阅读输出。然后再声明结果。
