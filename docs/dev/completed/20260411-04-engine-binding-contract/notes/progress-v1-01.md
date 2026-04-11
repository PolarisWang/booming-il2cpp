# 进展记录 v1-01

## 时间

- 2026-04-11 12:58:38 +08:00

## 本轮完成

- 完成 Phase 3 当前表面审计：
  - 复查 `phase-architecture-v1-01.md` 的 Phase 3 目标、ABI 分层与验证条目
  - 复查 `runtime_core.h/.cpp` 中已有的 engine proof helper：
    - `EngineLogWrite`
    - `CreateEngineObjectHandle` / `ResolveEngineObjectHandle`
    - `RegisterEngineLifecycleCallback` / `DispatchEngineLifecycleCallbacks`
    - `IsMainThreadLane`
  - 复查 `EngineLogWriteLite` / `EngineObjectHandleLite` / `EngineLifecycleCallbackLite` / `HostEmbeddingLite`
- 写出 Batch 1 设计文档：
  - `brainstorm-v1-01.md`
  - `design-v1-01.md`
- 建立并完成 Batch 1 contract freeze 最小 slice：
  - 新增 `tests/unit/run/test_phase3_engine_contract_freeze.py`
  - 新增 `contracts/engine/v0/lifecycle.h`
  - 新增 `contracts/engine/v0/ownership.h`
  - 新增 `contracts/engine/v0/callback.h`
  - 新增 `contracts/engine/v0/thread-model.md`

## 验证结果

- `python -m pytest tests/unit/run/test_phase3_engine_contract_freeze.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase7_engine_binding.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_repo_layout.py -v` -> 18 passed

## 备注

- 本轮只完成 Batch 1 contract freeze；`src/native/engine-bridge/`、`Chaos.IL2CPP.EngineBinding`、`EngineHostProof` 仍未开始实现。
- 当前 contract 先冻结 ABI 名称和线程/ownership 语义，其中一部分函数族还没有对应实现；这是故意的“先边界、后实现”。

## Wiki

- 本轮未新增 wiki。
- 原因：长期知识已经直接沉淀为 `contracts/engine/v0/*` 正式 contract 文件，本轮无需额外写一份二次解释页。

## 下一步

- 继续审计 Batch 2 `engine-bridge` 的最小 RED 入口，决定是先迁 lifecycle/handle helper，还是先补 callback dispatch bridge。
