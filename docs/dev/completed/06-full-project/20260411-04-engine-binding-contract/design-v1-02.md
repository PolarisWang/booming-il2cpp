# Design v1-02

## Batch 2 最小实现设计

### 目标

建立独立的 native `engine-bridge` 模块，让 Phase 3 contract 第一次拥有真实实现承载点，同时保持 scope 收敛：

- 不直接继续膨胀 `runtime_core.cpp`
- 不提前引入 managed `Chaos.IL2CPP.EngineBinding`
- 不提前改写现有 engine subjects

### 设计范围

#### 1. native 模块结构

- 新建 `src/native/engine-bridge/`
  - `CMakeLists.txt`
  - `engine_bridge.h`
  - `engine_bridge.cpp`
- target 名称：
  - `chaos_engine_bridge`
- 依赖：
  - `chaos_runtime_core`
  - `contracts/native/v0`
  - `contracts/engine/v0`

#### 2. `engine_runtime_context_t`

- 在 `engine_bridge.cpp` 内定义 opaque context
- 最小状态：
  - `const RuntimeAbiV0* abi`
  - `RuntimeState* runtime`
  - `ThreadState* thread`
  - callback registry
  - ownership bookkeeping map
- `engine_init`
  - 通过 `chaos_runtime_get_abi_v0()` + `runtime_init` + `thread_attach` 建立上下文
  - 使用 Batch 1 contract 中的 `engine_init_params_t`
- `engine_shutdown`
  - detach thread
  - shutdown runtime

#### 3. lifecycle bridge

- `engine_reload`
  - 复用 `DispatchEngineLifecycleCallbacks("reload")`
- `engine_scene_attach`
  - 复用 `DispatchEngineLifecycleCallbacks("scene-attach")`
- `engine_init`
  - 建立上下文后尝试触发 `DispatchEngineLifecycleCallbacks("init")`
- `engine_shutdown`
  - 在真正 shutdown 前尝试触发 `DispatchEngineLifecycleCallbacks("shutdown")`
- 规则：
  - “无 callback 被注册”不视为失败
  - runtime ABI attach/init 失败才算 bridge 初始化失败

#### 4. callback registry

- `engine_register_callback`
  - 在 context 内部分配递增 `engine_callback_id_t`
  - 记录 `callback_name_utf8`、function pointer、user_data
- `engine_dispatch_callback`
  - 按 callback id 查找并执行
  - 将 callback 返回值映射为 `engine_status_t`

#### 5. ownership bookkeeping

- 在 context 内维护 `engine_object_ref_t -> { strong_count, weak_count, owner }`
- `engine_retain_object`
  - 根据 `engine_handle_kind_t` 增加 weak/strong 引用计数
- `engine_release_object`
  - 减计数；非法下溢返回 `ENGINE_STATUS_INVALID_ARGUMENT`
- `engine_transfer_ownership`
  - 更新 owner 文本
  - 要求对象已在 bookkeeping 中出现

### RED 测试

- 新增 `tests/unit/run/test_phase3_engine_bridge_minimal.py`
- 断言：
  - `src/native/engine-bridge/` 三个文件存在
  - root `CMakeLists.txt` 已接入 `add_subdirectory(src/native/engine-bridge)`
  - `engine_bridge.h` 引入 `lifecycle.h` / `ownership.h` / `callback.h`
  - `engine_bridge.cpp` 包含：
    - `chaos_runtime_get_abi_v0`
    - `DispatchEngineLifecycleCallbacks`
    - `engine_register_callback`
    - `engine_dispatch_callback`
    - `engine_retain_object`
    - `engine_release_object`

### GREEN 验证

- `python -m pytest tests/unit/run/test_phase3_engine_bridge_minimal.py -v`
- `python -m pytest tests/unit/run/test_phase3_engine_contract_freeze.py -v`
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`

### 风险

- lifecycle dispatch 当前复用的是 runtime-core 现有 helper，仍是 proof-first 语义；后续 `EngineHostProof` 可能需要更严格的 host event model。
- ownership bookkeeping 目前只是 bridge 内部记录，不等于完整 GCHandle ↔ engine native ref 协议。
