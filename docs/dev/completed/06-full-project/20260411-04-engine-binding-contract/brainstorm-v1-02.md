# Brainstorm v1-02

## 背景

Batch 1 已完成 `contracts/engine/v0` 冻结，但当前仍没有承载这些 contract 的独立 native 模块：

- root `CMakeLists.txt` 只接了 `runtime-core`、`support`、`bootstrap`
- `runtime_core.cpp` 里已经有一批 engine proof helper，但它们仍混在 runtime-core 内部
- `subjects/Engine*` proof host 直接链接 `runtime_core` / `bootstrap`

如果继续把所有 engine-facing 行为塞进 `runtime_core`，Phase 3 的 contract 很快又会退化成隐式 helper 集合。

## 候选方案

### 方案 A：继续扩 `runtime_core.cpp`

- 优点：
  - 当前改动最少
- 缺点：
  - engine-facing 边界继续和 runtime-core 内部实现纠缠
  - 不利于后续 `EngineBinding` / hot update / host embedding 复用

### 方案 B：新建独立薄封装 `src/native/engine-bridge/`

- 内容：
  - 新增 `chaos_engine_bridge` 静态库
  - 用 `contracts/engine/v0/*` 作为唯一对外头文件来源
  - 内部复用 `chaos_runtime_get_abi_v0()` 与 `runtime_core` 现有 helper
  - 先实现最小 `engine_runtime_context_t`、callback registry 和 ownership bookkeeping
- 优点：
  - 把 contract 与 runtime-core helper 分层
  - scope 仍可控，不需要马上引入 managed `EngineBinding`
  - 可以先作为 Phase 3 的 native 落点
- 缺点：
  - 需要补一个新的 native target 与 CMake 接线

### 方案 C：直接实现 `EngineBinding` + `engine-bridge`

- 优点：
  - 功能推进更快
- 缺点：
  - managed/native 两层同时开工，风险和 scope 都太高

## 选择

选择方案 B。

## Batch 2 最小入口

- 新建 `src/native/engine-bridge/`
  - `CMakeLists.txt`
  - `engine_bridge.h`
  - `engine_bridge.cpp`
- root `CMakeLists.txt` 接入 `add_subdirectory(src/native/engine-bridge)`
- 最小实现只做三件事：
  - lifecycle/context：`engine_init` / `engine_shutdown` / `engine_reload` / `engine_scene_attach`
  - callback registry：`engine_register_callback` / `engine_dispatch_callback`
  - ownership bookkeeping：`engine_retain_object` / `engine_release_object` / `engine_transfer_ownership`

## 边界

- Batch 2 只建立 native bridge 骨架，不要求任何 subject 已切到它。
- Batch 2 不引入 `Chaos.IL2CPP.EngineBinding`，也不要求 `HostEmbeddingLite` 改写。
