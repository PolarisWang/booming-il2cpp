# Metadata Supplement 与 Bridge Baseline v1

## 适用范围

- 适用于 Phase 6 之后的 hot update 基线实现。
- 目标是把 AOT 主包已有 metadata、热更新新增 metadata，以及跨模式 bridge surface 固化为可复用的最小承载面。
- 本页记录长期有效的项目边界与验证入口，不记录单次执行流水。

## 产物基线

- convert 输出除了 `typed-il-ir.json`、`aot-manifest.json`、`metadata-registration.json`、`code-registration.json`、`optimization-facts.json`、`preserve-descriptor.json`、`native-reference.lowering-plan.json` 外，还必须包含 `hot-update/supplemental-metadata-template.json`。
- `closure.manifest.json` 必须把该产物登记为 `supplementalMetadataTemplate`，路径固定为 `hot-update/supplemental-metadata-template.json`。
- `DriverEntry.WriteJson` 必须能自动创建父目录；nested artifact 输出属于正式能力，不再允许按单个文件做特判。

## Managed Surface

- `Chaos.IL2CPP.HotUpdate` 继续只依赖 `Chaos.IL2CPP.Contracts`，不能反向依赖 `Chaos.IL2CPP.CodeGen`。
- supplemental metadata baseline surface 包含：
  - `SupplementalMetadataWriter`
  - `SupplementalMetadataLoader`
- bridge baseline surface 包含：
  - `BridgeGenerator`
  - `BridgeDispatcher`
- 当前 bridge baseline 至少覆盖 4 类路由：
  - `AotToHotUpdate`
  - `HotUpdateToAot`
  - `HotUpdateToEngine`
  - `DelegateWrappers`

## Native Surface

- native hot-update 模块必须暴露 supplemental metadata image 生命周期：
  - `LoadSupplementalMetadataFromPath`
  - `ReleaseSupplementalMetadataImage`
- Phase 6 的 native 侧仍是加载面与资源边界 baseline，不代表完整 interpreter 已经存在。

## Proof Baseline

- `BridgeRoundtripProof` 负责证明：
  - AOT -> HotUpdate 路径可用
  - HotUpdate -> AOT 路径可用
  - HotUpdate -> Engine 最小 `int -> int` 路径可用
  - delegate cross-mode wrapper 可用
- `GenericSupplementProof` 负责证明：
  - supplemental metadata loader 能查询 subjectId / token
  - 新增 generic instantiation 的 metadata supplement 路径已接通

## 验证入口

- 定向 RED/GREEN：`python -m pytest tests/unit/run/test_phase6_metadata_supplement_bridge.py -v`
- managed-closure 契约：`python -m pytest tests/unit/run/test_managed_closure_contract_bundle.py -v`
- proof 程序：
  - `dotnet run --project subjects/BridgeRoundtripProof/source/BridgeRoundtripProof.csproj --`
  - `dotnet run --project subjects/GenericSupplementProof/source/GenericSupplementProof.csproj --`
- 项目级完成前验证：
  - `./run.ps1 test pipeline --id pipeline/completion-managed-closure --json`
  - `./run.ps1 test pipeline --id pipeline/completion-runtime-core --json`

## 边界说明

- Phase 6 明确是 proof-first baseline，不等于完整 interpreter productization。
- `HotUpdateToEngine` 目前只验证最小整数路由；复杂 engine contract 调度留在后续阶段扩展。
- 任何后续改动只要影响 managed closure bundle 结构，就必须同步维护 Stage 3 规格文档与 `test_managed_closure_contract_bundle.py`。
