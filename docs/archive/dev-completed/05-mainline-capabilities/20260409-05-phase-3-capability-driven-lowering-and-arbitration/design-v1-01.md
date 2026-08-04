# Phase 3 Capability-Driven Lowering And Arbitration 设计 v1.01

Date: 2026-04-09
Status: plan-ready

## 1. 设计目标

把 lowering family 的主决策面正式切到 `CodeGen`：

- primary signals = `semantic subject structure + capability bundle`
- secondary signals = `dependencies / reasons` 一致性校验
- emitter 不再从 `typed-il` / instruction shape 反向识别 family

## 2. lowering family vocabulary

本轮冻结当前 4 个 family 名称：

- `managed-object.captured-state-instance-message.minimal`
- `managed-generic.static-forwarder-captured-getter.minimal`
- `reflection.closed-type-query.minimal`
- `interop.pinvoke-direct-call.minimal`

当前仍沿用 `NativeReferenceLoweringPlanArtifact.PlanKind` 字段，但语义上把它视为 `LoweringFamily`。

## 3. semantic structure 输入

为避免 `CodeGen` 回退到 proof-shape detection，本轮在 `SemanticWorld` 侧补最小 structure signals：

- `constructor`
- `imported-method`
- `static-forwarder`
- `instance-field-getter`
- 其余保留 `static-method` / `instance-method`

这些 signals 允许 `CodeGen` 在不依赖 dependency 名称和 proof-shape 的前提下，识别当前 family。

## 4. managed lowering plan artifact

新增 managed closure 侧 artifact：

- path: `native-reference.lowering-plan.json`
- owner stage: `CodeGen`
- role: emitter 的正式输入真源

native proof 输出目录仍保留：

- `native-proof.plan.json`

但它降级为 managed lowering plan 的镜像产物。

## 5. family selection 规则

本轮采用 deterministic rule，不引入 scoring：

1. 先按更具体 family -> 更通用 family 的顺序检查 required predicates。
2. primary predicates 只允许来自 semantic structure 与 capability bundle。
3. dependency / reason 只用于 consistency check，不作为 primary classifier。
4. 没有 family 命中时，直接抛出 `CodeGen` diagnostics。

## 6. emitter 边界

`NativeReferenceProofEmitter` 本轮只保留：

- 读取 managed lowering plan
- plan completeness validation
- backend invariant validation
- template selection
- generated source rendering

本轮删除：

- lowering family discovery
- `Is...Shape(...)`
- `TryCreate...LoweringPlan(...)`
- 基于 instruction shape 的 family 反推

## 7. deferred items

仍然延后：

- `contract` / `subject` 边界清理
- plan role-binding 新 contract
- `TemplateKind` 正式入约
- legality filter + performance arbitration 的更高阶策略化扩展
