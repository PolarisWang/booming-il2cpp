# Contracts Overview v0

## 目标

`contracts/` 是 Stage 1A 之后的 proof-facing contract 统一语义根，用来把 artifact contract、native boundary、文档入口和样例入口收拢到同一个版本化坐标系下。

## Canonical Layout

- `contracts/artifacts/v0/schemas/`: canonical artifact schema 根
- `contracts/examples/v0/artifacts/`: canonical artifact 最小样例
- `contracts/native/v0/`: native header contract
- `contracts/native/examples/v0/`: native 侧代码样例
- `contracts/docs/v0/`: 版本化文档总入口

## Compatibility Layout

Stage 1A 先冻结 surface，不把所有消费者一次性迁完，所以保留以下 compatibility 路径：

- `analysis/contracts/schemas/`
- `analysis/contracts/examples/`
- `tests/contract/schema/`

其中：

- `analysis/contracts/*` 是 artifact contract 的 compatibility mirror
- `tests/contracts/schema/` 仍是 canonical snapshot baseline
- `tests/contract/schema/` 只是尚未迁完的 compatibility snapshot 路径

## Shared Identity

first-proof 相关 artifact 必须共享同一套 canonical `subjectId` 规则：

- 类型：`HelloWorldObject/Greeter`
- 字段：`HelloWorldObject/Greeter::_name`
- 方法：`HelloWorldObject/Greeter::BuildMessage()`

`typed-il-ir` 可以继续保留 artifact-local `methodId`，但不能再把它当成跨 artifact 的主 identity；正式追溯一律以 `subjectId` 为准。

## First-Proof Frozen Set

Stage 1A 实际冻结的 first-proof contract 面包括：

- `typed-il-ir`
- `aot-manifest`
- `code-registration`
- `metadata-registration`
- `runtime ABI` proof-required subset
- `codegen bridge` proof-required subset

其中 generated `Program.Main`、`Greeter::.ctor(string)`、`Greeter.BuildMessage()` 明确采用 direct-call landing。

## Reading Order

建议按下面顺序读：

1. 本文，先看目录语义和 compatibility 边界
2. `contracts/docs/v0/runtime-abi-proof-boundary.md`
3. `contracts/docs/v0/codegen-bridge-proof-boundary.md`
4. `contracts/native/docs/v0/runtime-abi.md`
5. `contracts/native/docs/v0/codegen-bridge.md`
6. `contracts/artifacts/v0/schemas/*.schema.json`
7. `contracts/examples/v0/artifacts/*.min.json`
