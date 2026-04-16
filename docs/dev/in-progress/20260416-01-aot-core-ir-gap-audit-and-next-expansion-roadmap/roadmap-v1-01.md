# AOT Core IR Gap Audit And Next Expansion Roadmap v1.01

Date: 2026-04-16 08:54:25 +08:00
Status: in-progress

## 1. 目标

在 `20260415-11` 与 `20260415-18` 已完成能力的基础上，继续审计并收口剩余 `AotCoreIr` / planner / emitter 真实缺口，形成下一轮 AOT core 主线 roadmap。该 roadmap 的目标不是“再列一份愿望清单”，而是把剩余能力固定为可逐批执行的 child task，并明确：
- capability family
- stage impact
- owner subject
- proof / benchmark / gate evidence
- Scriban 落地要求

## 2. 范围边界

本 roadmap 负责：
- 审计 `LoaderStage`、`AotCoreIr`、`NativeAotLoweringPlanner`、`NativeAotEmitter + Scriban` 的剩余真实能力缺口
- 按 capability family 重新切批
- 固定 `SolutionCorePack` 作为纯 AOT owner subject 主体
- 为后续每一批定义 checklist、验收标准与 evidence 要求

本 roadmap 不负责：
- 重构 `Chaos.TestFramework`
- 把热更或混合执行主线重新并入纯 AOT 主线
- 扩大 subject 内部 string 协议
- 在父 roadmap 中直接完成全部代码实现

## 3. 非目标

- 不以单个 `NotSupportedException` 报错驱动 backlog
- 不让 `HotUpdateHostPack` / `MixedExecutionFeaturePack` 默认承担纯 AOT owner subject
- 不把 Scriban widening 延后到后续轮次
- 不把测试框架代码混入 AOT core / planner / emitter

## 4. 阶段列表

### Phase 0: Gap Ledger And Owner Evidence Audit

- goal:
  - 冻结 capability-first gap ledger
  - 冻结 owner subject / proof / benchmark / gate 映射
  - 明确每个 family 的 stage impact 与 deferred 理由
- exit_criteria:
  - 所有主要缺口都能映射到唯一 capability family 与 child task
  - 纯 AOT owner subject 边界固定到 `SolutionCorePack`

### Phase 1: Loader And Typed-IL Input Closure

- goal:
  - 扩大 `LoaderStage` 对剩余 entry handle、IL opcode、type/method/field/ldtoken handle 与 resolution scope 的解析能力
- exit_criteria:
  - 目标 family 不再在 loader 阶段硬失败
  - typed-IL 输入身份、签名和 target metadata 可供下游稳定消费

### Phase 2: AotCoreIr Semantic Surface Expansion

- goal:
  - 扩大 `AotCoreIr` 对剩余 object model、address/byref/object/value helper、method shape 与 reference shape 的表达能力
- exit_criteria:
  - 目标 family 不再依赖 planner / emitter 临时推断 IR 缺失语义
  - IR contract 与 loader 输入面形成稳定衔接

### Phase 3: NativeAOT Planner And Entry ABI Generalization

- goal:
  - 扩大 planner 对 entry ABI、dispatch、EH、opcode legalization 与 widened ABI carrier 的统一规划能力
- exit_criteria:
  - native entry 不再限定于 `parameterless System.Int32`
  - planner 成为 widened entry / wrapper / thunk / EH / carrier 的单一事实来源

### Phase 4: NativeAOT Emitter And Scriban Shape Expansion

- goal:
  - 让 widened planner model 真正落到 `NativeAotEmitter + Scriban`
- exit_criteria:
  - 新增 workload / entry / wrapper / helper shape 全部通过 Scriban 输出
  - 不再只剩 `generic-managed-entry + int(void)` 一种 emitter 组合

### Phase 5: SolutionCorePack Owner Evidence And Benchmark Closure

- goal:
  - 把纯 AOT 新能力的 owner proof / benchmark 收口到 `SolutionCorePack`
- exit_criteria:
  - 新 capability 在 `SolutionCorePack` 中可见且可验证
  - `supported / missing evidence / unsupported` 对外口径一致

### Phase 6: Engineering Gates And Full-Solution Validation

- goal:
  - 用完整 solution 级流程验证 widened AOT core 能力
- exit_criteria:
  - full-solution native generate 路径可执行
  - 关键 benchmark 流程可执行
  - 工程级 gates / release evidence 与 capability state 对齐

## 5. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260416-02-phase-0-gap-ledger-and-owner-evidence-audit` | `Phase 0` | `completed` | `codex` | 冻结 capability-first gap ledger、owner subject、proof/benchmark/gate mapping | `20260416-01` |
| `20260416-03-phase-1-loader-and-typed-il-input-closure` | `Phase 1` | `completed` | `codex` | 第一批 loader widening：收口 `TypedReference` 与 `sizeof(non-primitive struct)` 的真实 C# 输入面 | `20260416-02` |
| `20260416-09-phase-1b-member-reference-parent-and-vararg-loader-closure` | `Phase 1` | `completed` | `codex` | 第二批 loader widening：收口 `MemberReference.Parent = MethodDefinition` 与 `__arglist` / vararg metadata + opcode 输入面 | `20260416-03` |
| `20260416-04-phase-2-aot-core-ir-semantic-surface-expansion` | `Phase 2` | `completed` | `codex` | 扩大 `AotCoreIr` 语义面，覆盖 object model / address / reference / method shape widening | `20260416-09` |
| `20260416-05-phase-3-native-aot-planner-and-entry-abi-generalization` | `Phase 3` | `in_progress` | `codex` | 扩大 planner 对 entry ABI、dispatch、EH 与 carrier 的统一规划 | `20260416-04` |
| `20260416-06-phase-4-native-aot-emitter-and-scriban-shape-expansion` | `Phase 4` | `planned` | `codex` | 让 widened planner model 落到 `NativeAotEmitter + Scriban` | `20260416-05` |
| `20260416-07-phase-5-solution-core-owner-evidence-and-benchmark-closure` | `Phase 5` | `planned` | `codex` | 将纯 AOT owner proof / benchmark 收口到 `SolutionCorePack` | `20260416-06` |
| `20260416-08-phase-6-engineering-gates-and-full-solution-validation` | `Phase 6` | `planned` | `codex` | 执行 full-solution 验证、benchmark 流程与工程级 gates 收口 | `20260416-07` |

## 6. 依赖

- `docs/dev/completed/20260415-11-phase-7-aot-core-ir-typed-il-translation-expansion-roadmap/roadmap-v1-01.md`
- `docs/dev/completed/20260415-18-post-phase-7-deferred-aot-shape-and-entry-abi-expansion-roadmap/roadmap-v1-01.md`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/`
- `subjects/SolutionCorePack/source/`
- `tests/unit/compatibility/`

## 7. 风险

- 如果 capability family 不先冻结，child task 会在 loader / planner / emitter 之间反复交叉返工
- 如果不固定 `SolutionCorePack` 为纯 AOT owner subject，工程级证据会再次分散
- 如果 planner 与 emitter widening 不同步，`Scriban` 路径会再次退化为只覆盖最小 workload ABI
- 如果 `Phase 2` 不按真实 semantic failing surface 切批，roadmap 会重新退化成逐条 unsupported 修补

## 8. 备选路径

### 备选路径 A：直接按 `unsupported` 报错逐条推进

- 不推荐
- 原因：碎片化，无法形成 capability 闭环

### 备选路径 B：先只做 `SolutionCorePack` proof 缺口，再倒推底层

- 不推荐作为主路线
- 原因：容易为了样例通过而污染底层 contract

### 备选路径 C：按 capability family 纵向推进

- 推荐
- 原因：最能同时约束底层 contract、Scriban 输出与 owner evidence

## 9. 当前建议推进顺序

1. `20260416-02-phase-0-gap-ledger-and-owner-evidence-audit` 已完成
2. 已完成 `20260416-03-phase-1-loader-and-typed-il-input-closure`，收口了 `TypedReference + sizeof` 的第一批 loader widening
3. 已完成 `20260416-09-phase-1b-member-reference-parent-and-vararg-loader-closure`，收口了 `MemberReference.Parent = MethodDefinition + __arglist`
4. `20260416-04` 已完成；`20260416-05` 正在执行，且已经完成 `ldc.i8 + and/or/xor + shl/shr/shr.un + not + beq/bgt/ble/bge/bge.un` 多批 widening，并为 branch owner proof 回补了 `LoaderStage` 的 `Bge_un / Bge_un_s` decode；下一步继续审计下一批 owner-driven blocker，再推进 `20260416-06`
5. 最后执行 `20260416-07` 与 `20260416-08`，把 `SolutionCorePack` owner evidence、benchmark 与 full-solution gate 收口
