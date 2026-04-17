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
| 可以归档 completed | formal verification gate + 受影响测试 + wiki 都完成 | “任务做完了” |

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
- `docs/dev/**` 只记录任务过程，不是长期 authority

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

## 测试阶段 `dotnet` 编译崩溃闸门

在任何测试、构建或验证命令中，如果 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃：

- 本次验证立即视为失败
- 必须先读取崩溃原因，定位失败的 project / target / task
- 必须保留并检查 `stderr`、`binlog`、崩溃堆栈或 dump 信息（如果可用）
- 必须修复根因并重新运行受影响的验证对象
- 在根因未查明、未修复前，不得声称“测试通过”、“问题已修复”或“可以完成”

## 重要验证升格

- 修复真实 bug 的回归验证，必须沉淀为正式 case
- 被认定为 `required-before-completion` 的验证，必须沉淀为正式 case、`module/...`、`system/...` 或 `pipeline/...` 对象
- 被 `module/system/pipeline` 引用的验证，必须沉淀为正式对象

## 结果汇报

汇报验证结果时，至少给出可定位信息：

- `canonicalCommand`
- `summaryPath`
- `eventsPath`
- `consolePath`
- 如存在 `phaseResults[]`，显式说明 `code / module / system` 分层状态

## 底线

**验证没有捷径。**

运行命令。阅读输出。然后声明结果。
