# Design v1-01

## Batch 1 最小实现设计

### 目标

把 Phase 3 的第一步收敛为正式 engine contract freeze：

- 建立 `contracts/engine/v0/`
- 把 lifecycle / ownership / callback / thread model 变成明确文件，而不是散落在 runtime-core helper 与 proof template 里的隐式约定
- 为后续 `engine-bridge` 与 `Chaos.IL2CPP.EngineBinding` 提供唯一上游边界

### 设计范围

#### 1. contract 目录

- 新建 `contracts/engine/v0/`
- 包含：
  - `lifecycle.h`
  - `ownership.h`
  - `callback.h`
  - `thread-model.md`

#### 2. ABI 设计

##### `lifecycle.h`

- 冻结最小 lifecycle ABI 名称：
  - `engine_init`
  - `engine_shutdown`
  - `engine_reload`
  - `engine_scene_attach`
- 提供最小上下文句柄与返回状态类型
- 本轮只冻结 ABI，不实现 engine bridge

##### `ownership.h`

- 冻结跨边界 ownership 协议：
  - `engine_retain_object`
  - `engine_release_object`
  - `engine_transfer_ownership`
- 冻结最小句柄类型：
  - `engine_object_ref_t`
  - `engine_handle_kind_t`
- 文档化：
  - engine 持有 native owner
  - managed 通过 handle/proxy 访问
  - transfer 只描述协议，不在本轮实现真实 bridge

##### `callback.h`

- 冻结 callback registration / dispatch ABI：
  - `engine_register_callback`
  - `engine_dispatch_callback`
- 冻结：
  - callback id
  - callback payload 指针
  - callback function typedef

##### `thread-model.md`

- 文档化最小线程约束：
  - `engine_post_to_main_thread`
  - `engine_is_main_thread`
  - 生命周期与 callback 默认在主线程 lane 上执行

#### 3. RED 测试

- 新增 `tests/unit/run/test_phase3_engine_contract_freeze.py`
- 断言：
  - `contracts/engine/v0/` 与四个文件存在
  - 头文件包含目标 ABI 名称
  - thread-model 文档包含主线程 lane 约束与 `engine_post_to_main_thread` / `engine_is_main_thread`
  - 现有 `runtime_core.h/.cpp` 仍保留与 contract 相关的最小 proof helper，作为当前实现支点

### 非目标

- 本轮不实现 `src/native/engine-bridge/`
- 本轮不创建 `Chaos.IL2CPP.EngineBinding`
- 本轮不创建 `subjects/EngineHostProof/`
- 本轮不把 `HostEmbeddingLite` 改写到正式 ownership protocol

### 验证策略

- RED：
  - `python -m pytest tests/unit/run/test_phase3_engine_contract_freeze.py -v`
- GREEN：
  - `python -m pytest tests/unit/run/test_phase3_engine_contract_freeze.py -v`
  - `python -m pytest tests/unit/run/test_phase7_engine_binding.py -v`
  - `python -m pytest tests/unit/run/test_repo_layout.py -v`

### 风险

- 如果在 Batch 1 就把 `engine-bridge` 和 `EngineBinding` 一起塞进来，会把 contract freeze 与实现重构混成一轮，失去“先边界、后实现”的控制。
- 当前 contract 中会包含 roadmap 目标函数名，其中一部分还没有实现；必须明确它们是 freeze 的 ABI，而不是现成可调用实现。
