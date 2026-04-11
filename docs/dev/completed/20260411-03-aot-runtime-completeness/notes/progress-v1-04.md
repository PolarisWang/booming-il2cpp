# 进展记录 v1-04

## 时间

- 2026-04-11 04:10:23 +08:00

## 本轮完成

- 建立 Batch 2 async/await 的设计与执行入口：
  - 新增 `brainstorm-v1-01.md`
  - 新增 `design-v1-01.md`
  - 新增 `tests/unit/run/test_phase2_async_await_proof.py`
- 落地 Batch 2 async/await 最小 proof slice：
  - `LoaderStage` 不再丢弃嵌套类型方法，为 compiler-generated state machine 提供闭包基础
  - `SemanticWorldStage` 新增 `requires-async-state-machine` 与 `requires-task-awaiter`
  - `LinkerStage` 新增 compiler-generated async state machine 闭包保留逻辑
  - `NativeReferenceLoweringPlanner` / `NativeReferenceProofEmitter` 新增 `managed-async.awaitable-int.minimal`
  - 新增模板 `NativeReferenceProof.AsyncAwaitInt.cpp.scriban`
  - 新增 `subjects/AsyncAwaitProof/`

## 验证结果

- `python -m pytest tests/unit/run/test_phase2_async_await_proof.py -v` -> 3 passed
- `python -m pytest tests/unit/run -v` -> 157 passed
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug` -> succeeded
- `dotnet build subjects/AsyncAwaitProof/source/AsyncAwaitProof.csproj -c Debug` -> succeeded

## 备注

- core solution 构建存在若干已有 warning，本轮没有新增 error。
- Batch 2 当前是 proof-first 的同步化 lowering，不代表通用 async runtime 已完成。

## Wiki

- 本轮仍未写入 wiki。
- 原因：需要等 Batch 3/4 的 runtime-core 规则一起稳定后，再统一沉淀 Phase 2 runtime completeness 矩阵。

## 下一步

- 审计 Batch 3 threading/GC 当前基线，建立 `ThreadStatic` / `Monitor` / 线程模型的最小 proof slice 与 RED 测试。
