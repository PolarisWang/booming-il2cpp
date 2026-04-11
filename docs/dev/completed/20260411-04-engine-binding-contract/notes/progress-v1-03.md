# 进展记录 v1-03

## 时间

- 2026-04-11 13:59:04 +08:00

## 本轮完成

- 完成 Batch 4 `EngineHostProof`
  - 新增 `subjects/EngineHostProof/subject.manifest.json`
  - 新增 `subjects/EngineHostProof/source/EngineHostProof.csproj`
  - 新增 `subjects/EngineHostProof/source/Program.cs`
  - 新增 `subjects/EngineHostProof/validation/proof/native-reference/main.cpp`
  - 新增 `subjects/EngineHostProof/expected/runtime/windows-reference-trace/host-proof.json`
  - 新增 `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.EngineHostProof.cpp.scriban`
  - 更新 `NativeReferenceLoweringPlanner.cs` / `NativeReferenceProofEmitter.cs`，把 `EngineHostEntry::Run()` 收敛到 `engine.host-proof.minimal`
  - 更新 `build/toolchains/run/testing/subject_workers.py`，让 direct proof build 直接接入 `contracts/engine/v0`、`src/native/engine-bridge` 与 `engine_bridge.cpp`
- 完成 Batch 4.4 lifecycle stress
  - `EngineHostProof` 模板已固化 `1000` 次 `engine_init -> retain/transfer/release -> register_callback -> dispatch_callback -> engine_shutdown`
  - 预期 trace 已增加 `"stressIterations": 1000`
- 完成 Batch 4.3 `HostEmbeddingLite` ownership 协议迁移
  - 新增 `OwnershipProxy`
  - `ExerciseOwnershipProtocol()` 改为真实 `GCHandle.Alloc(..., GCHandleType.Normal/Weak)` 与 `.Free()`
  - `Main()` 已输出 `ownershipSummary`
- 修正误报测试
  - 更新 `tests/unit/run/test_phase3_host_embedding_ownership_protocol.py`
  - 将对普通字符串字面量的断言放宽为实际存在的插值片段

## 验证结果

- `python -m pytest tests/unit/run/test_phase3_engine_host_proof.py tests/unit/run/test_phase3_host_embedding_ownership_protocol.py -v` -> 4 passed
- `dotnet run --project subjects/HostEmbeddingLite/source/HostEmbeddingLite.csproj --` -> 输出包含 `ownership=host-ownership-payload|managed->engine-host|strong:True|weak:True|weakAlive:True|released:true`
- `HostEmbeddingLite windows-managed-output subject matrix` -> `status=ok`
- `HostEmbeddingLite windows-managed-trace subject matrix` -> `status=ok`，`trace-compare.report.json` 的 `errors=[]`
- `python -m pytest tests/unit/run/test_phase3_engine_binding_codegen_baseline.py tests/unit/run/test_phase3_engine_bridge_minimal.py tests/unit/run/test_phase3_engine_contract_freeze.py tests/unit/run/test_phase3_engine_host_proof.py tests/unit/run/test_phase3_host_embedding_ownership_protocol.py tests/unit/run/test_phase7_engine_binding.py tests/unit/run/test_repo_layout.py tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_workers.py tests/unit/run/test_subject_reporting.py -v` -> 74 passed
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug` -> succeeded（16 个既有 nullable warnings，0 error）

## 备注

- `HostEmbeddingLite` 的 ownership roundtrip 故意不写入 `session.Trace`，以保持既有 warmup trace snapshot 稳定。
- `EngineHostProof` 的 `1000` 次 lifecycle stress 证明当前 mock host 最小链路可重复执行，不等于 production engine perf baseline。

## Wiki

- 已新增 `wiki/04-工具与集成/engine-host-proof-baseline-v1.md`
- 原因：`EngineHostProof` 的 host proof 范围、direct proof build 接线规则与 `HostEmbeddingLite` ownership 模拟边界属于长期有效知识。

## 下一步

- 激活 `20260411-05-mobile-runtime-host`
- 先审计 `verify-runtime-baseline`、`build/toolchains`、`HostEmbeddingLite` 与 `HelloWorld` 的复用面，再为 Batch 1 shared host framework 写 RED
