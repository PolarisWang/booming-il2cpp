# HybridCLR 对齐的 AOT 与热更开发计划审查 v1.02

Date: 2026-04-15 07:15:00 +08:00
Status: post-closeout-correction

## 1. 结论

`20260414-37` 当前不应处于 `completed`。

正确判断应为：

- roadmap 主线仍在进行中
- Phase 0-3 已完成
- `20260415-05` 只完成了 `Phase 4A`，即 `AotCoreIr` 骨架与最小 native path
- Phase 4 的大部分真实能力面、以及依赖这些能力面的 Phase 5/6 仍未完成

## 2. 原完成判定为何不成立

### 2.1 Phase 4 退出条件被缩窄了

父 roadmap 对 Phase 4 的定义是：

- `AotCoreIr` 承接对象模型、调用模型、泛型、异常、元数据闭包
- `NativeAotEmitter` 的核心路径迁移到 `AotCoreIr`
- AOT 与热更共享 identity，不再形成平行世界

实际落地内容只有：

- `AotCoreIr` 进入 managed closure
- `NativeAotEmitter` 改为消费 `AotCoreIr`
- 最小静态 `call`、最小 arithmetic / branch / ret 闭环
- gap ledger 建立

这只能证明 `AotCoreIr` 已经成为正式中层入口，不能证明 Phase 4 已经完成。

### 2.2 Phase 5 / Phase 6 的 reuse audit 只证明“旧 gate 没坏”

`phase-5-6-reuse-audit-v1-01.md` 的有效结论是：

- 既有 owner subject / engineering gate 闭环仍然存在
- 当前 `AotCoreIr` 切线没有打断旧的 dashboard / full-solution / mixed execution / hot update 基线

但它不能证明：

- 当前 roadmap 里承诺的 AOT translation surface 已被补齐
- 当前 roadmap 的 Phase 5 feature coverage expansion 已完成
- 当前 roadmap 的 Phase 6 engineering gate 已基于新的 AOT coverage 重跑并收口

## 3. 当前实现边界

### 3.1 已完成

- `AotCoreIrArtifact` / `AotCoreIrMethodArtifact` / `AotCoreIrInstructionArtifact` 合同已建立
- `NativeSymbol`、`ManagedInstructionReference`、direct-call metadata 已进入中层
- `NativeAotEmitter` 已改为以 `AotCoreIr` 为正式输入
- 可运行的最小 opcode 集合已经贯通：
  - `ldc.i4`
  - `ldarg`
  - `ldloc`
  - `stloc`
  - `add/sub/mul/div/rem`
  - `br`
  - `blt`
  - `call`
  - `ret`

### 3.2 未完成

- 对象模型：
  - `newobj`
  - `box/unbox`
  - `castclass/isinst`
  - `newarr`
  - `ldfld/stfld`
  - `ldsfld/stsfld`
- 泛型 / sharing：
  - generic context carrier
  - codegen closure
  - runtime sharing hooks
- 异常：
  - `try/catch/finally/filter/rethrow`
  - landing pad / EH lowering
- 调度：
  - instance call
  - virtual call
  - interface call
  - bridge thunk native emission
  - external runtime call lowering
- ABI / type system：
  - 非 `System.Int32` 参数 / 返回
  - value type
  - reference type runtime layout
  - array / string / delegate runtime service
- metadata closure：
  - token-level metadata stitching
  - preserve / reflection / closure stitching

## 4. 纠偏后的阶段判断

- Phase 0：completed
- Phase 1：completed
- Phase 2：completed
- Phase 3：completed
- Phase 4A：completed
- Phase 4B：待做
  - object model / field / array / runtime-service carrier
- Phase 4C：待做
  - generic / sharing / metadata closure contract
- Phase 4D：待做
  - exception / dispatch / ABI widening
- Phase 5：待做
  - owner-subject coverage expansion
- Phase 6：待做
  - engineering gate / benchmark / release evidence closeout

## 5. 建议

- 不另起一个模糊 follow-up。
- 直接把 `20260414-37` 恢复为 `in_progress`。
- 用 `roadmap-v1-02.md` 重写 Phase 4-6。
- 把 `20260415-05` 重新解释为 `Phase 4A` 已完成切片。
- 然后从 `Phase 4B` 开始继续执行。
