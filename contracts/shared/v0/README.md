# Shared Contract v0

## 目标

`contracts/shared/v0/` 冻结 AOT 主线与热更运行模式共享的最小 contract 面。这里的定义必须同时被以下两条主线接受：

- `IL2CPP AOT Mainline`
- `Hot Update Runtime Mode`

## 读取顺序

1. `identity-model.md`
2. `object-model.md`
3. `handle-model.md`
4. `abi-calling-convention.md`
5. `exception-boundary.md`
6. `delegate-abi.md`
7. `metadata-token-mapping.md`
8. `version-policy.md`
9. `interpreter-ir-decision.md`
10. `package-manifest.schema.json`

## 事实来源

- managed identity: `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- runtime handles / ABI: `contracts/native/v0/runtime_abi.h`
- bridge status / delegate helpers: `contracts/native/v0/codegen_bridge.h`
- object / array / boxed / string proof layout: `src/native/runtime-core/runtime_core.cpp`
- delegate proof layout: `src/native/bootstrap/bootstrap.cpp`

## 边界

- `contracts/shared/v0/` 只保存 formal definition，不保存 sample、snapshot、fixture。
- concrete validation fixture 放在 `tests/contracts/shared/`。
- 任何破坏兼容的字段删除、重命名、语义反转都必须提升 `formatVersion`。
