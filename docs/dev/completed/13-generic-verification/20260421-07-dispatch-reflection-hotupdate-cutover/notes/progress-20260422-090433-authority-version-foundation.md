# 2026-04-22 09:04:33 +08:00 authority/version foundation

## 本轮完成

- 为 `HotUpdateMethodRegistry`、`RuntimeManager`、`BridgeDispatcher`、package version topology 补了 RED tests
- 在 `ManagedMethodIdentityArtifact` 上补齐 `InstantiationStubId`，新增 `ResolveExecutionAuthorityKey(...)`
- HotUpdate runtime / registry / dispatcher 已改为以 execution authority key 为主索引
- 新增 generation-scoped `HotUpdateMethodHandle` 与 stable reason code
- package manifest 已显式拆成 `PackageFormatVersion` + `KernelArtifactVersion`
- retained proofs / fixtures / benchmarks 已补齐新 contract 所需字段

## 验证

- `python -m pytest tests/unit/compatibility/test_phase2_runtime_shared_contract_consumers.py tests/unit/compatibility/test_phase3_hotupdate_runtime_foundation.py tests/unit/compatibility/test_hot_update_e2e_flows.py -q`
  - `16 passed`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - `0 Error(s)`，仅有既有 warnings
- `python -m pytest tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_hot_update_skeleton_subject.py -q`
  - `1 failed, 11 passed`
  - 失败点为 metadata supplement 断言旧 subject-id 形态，与本轮 authority/version foundation 无直接代码交叉

## 下一步建议

- 继续把 reflection construction / invoke proof 接到同一 authority 模型
- 落 stale-handle / version mismatch / host<->patch / advanced carrier formal proofs
