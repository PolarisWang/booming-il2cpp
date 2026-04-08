# Contracts Overview v0

## 目标

`contracts/` 是当前仓库里 artifact contract 与 native boundary 的正式根目录。

## Canonical Layout

- `contracts/artifacts/v0/schemas/`: artifact schema
- `contracts/artifacts/v0/samples/`: 最小可读 sample
- `contracts/artifacts/v0/snapshots/`: canonical snapshot baseline
- `contracts/native/v0/`: native header contract
- `contracts/native/v0/samples/`: native sample
- `contracts/docs/v0/`: 版本化文档入口

## Repository Boundary

- `contracts/` 是唯一顶层 contract 根目录
- 根目录 `analysis/` 已退役，不再承载 schema、sample 或其他正式 contract 资产
- run 期 analysis 产物只允许落在 `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/...`
- `contracts/artifacts/v0/snapshots/` 是 canonical snapshot baseline

## Shared Identity

first-proof 相关 artifact 共享同一套 canonical `subjectId` 规则，例如：

- 类型：`HelloWorldObject/Greeter`
- 字段：`HelloWorldObject/Greeter::_name`
- 方法：`HelloWorldObject/Greeter::BuildMessage()`

`typed-il-ir` 可以继续保留 artifact-local `methodId`，但跨 artifact 追踪必须以 `subjectId` 为准。

## First-Proof Frozen Set

Stage 1A 冻结的 first-proof contract 包括：

- `typed-il-ir`
- `aot-manifest`
- `code-registration`
- `metadata-registration`
- `runtime ABI` proof-required subset
- `codegen bridge` proof-required subset

## Proof Spec

object-based `HelloWorld` 的 canonical proof 规格入口：

- `docs/architecture/roadmap-0/hello-world-proof-spec-v0.md`
- `docs/architecture/roadmap-0/managed-minimal-closure-v0.md`

当 `subjects/<subject-id>/source/`、contract sample 与 native direct-call landing 出现分歧时，以这些 proof spec 和本目录下的 canonical contract 为准。

## Reading Order

1. 本文
2. `docs/architecture/roadmap-0/hello-world-proof-spec-v0.md`
3. `docs/architecture/roadmap-0/managed-minimal-closure-v0.md`
4. `contracts/docs/v0/runtime-abi-proof-boundary.md`
5. `contracts/docs/v0/codegen-bridge-proof-boundary.md`
6. `contracts/native/docs/v0/runtime-abi.md`
7. `contracts/native/docs/v0/codegen-bridge.md`
8. `contracts/artifacts/v0/schemas/*.schema.json`
9. `contracts/artifacts/v0/samples/*.min.json`
10. `contracts/artifacts/v0/snapshots/*.snapshot.json`
