# 20260425-02 Runtime Skeleton 4C Framework Foundation Cutover

## 目标

把 runtime-skeleton `4C` 框架的第一阶段做成可执行基座，为后续 dispatcher 全量 cutover 和 family migration 提供稳定 contract。

## 范围边界

本 plan 包含：

- 盘点现有 runtime-skeleton dispatcher 与 builder 依赖
- 设计并实现基础 `BuildContext`
- 设计并实现 `MatchResult / BuildResult`
- 设计并实现 `Family / Handler` contract
- 给 runtime-skeleton 主入口接入第一轮新 dispatcher 骨架

本 plan 不包含：

- 全量 family 迁移
- 删除 legacy dispatcher
- 关闭 `System.Convert` 全部 coverage 缺口

## Capability Intake

- capabilityFamily: `runtime-skeleton-codegen-governance`
- capabilityItem: `runtime-skeleton-4c-family-handler-framework-foundation`
- ownerSubjectId: `subject/SolutionCorePack`
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: medium
- formalVerificationObjects:
  - `runtime-skeleton codegen unit/contracts`
  - `SolutionCorePack canonical subject run`
- requiredGates:
  - `tests/unit/**`
  - `tests/contracts/**`
  - impacted canonical subject run

## 执行步骤

1. 梳理 runtime-skeleton 总 dispatcher 与现有 builder 分类
2. 设计并实现 4C 基础 contract
3. 把总入口切成“新 dispatcher 骨架 + legacy fallback”
4. 补齐第一轮 tests/contracts
5. 跑最小 canonical 验证，确认未破坏现有能力面

## 交付物

- `NativeReferenceProofEmitter` 内或配套文件中的 4C 基础 contract
- 新 dispatcher 骨架
- 首轮 tests/contracts
- canonical 验证证据

## 风险与观察项

- 新 contract 可能与现有 helper 边界冲突
- dispatcher 骨架如果设计过重，会拖慢后续 family migration
- legacy fallback 期间必须保证 deterministic ordering

## 退出条件

- 4C 基础 contract 已落地
- runtime-skeleton dispatcher 已具备新骨架入口
- 关键 unit/contracts 通过
- 至少一轮 canonical/subject gate 通过
