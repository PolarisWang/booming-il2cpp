# I-DBG: Debugger MVP — 执行状态

> **task_id**: I-DBG-debugger-mvp
> **parent_task_id**: interpreter-industrialization
> **创建日期**: 2026-05-23
> **更新日期**: 2026-05-23
> **phase**: 8 — 集成 + 测试
> **lifecycle_status**: completed
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true

## Scope

基于 DAP 协议的 IL-level 调试支持，覆盖 FastExecute 路径的断点/单步/调用栈/变量检视。

## 完成检查清单

- [x] Phase 1: DAP 协议核心 — `dbg_protocol.h`, `dbg_transport.h/cpp`, `dbg_server.h/cpp`
- [x] Phase 2: 断点管理 — `dbg_breakpoint.h/cpp`
- [x] Phase 3: FastExecute 断点 Hook — `dbg_runtime.h/cpp` + fast_dispatch.cpp
- [x] Phase 4: RegisterExecute 断点 Hook — interpreter_vm.cpp
- [x] Phase 5: 调用栈展开 — `dbg_stack.h/cpp` (基于帧快照)
- [x] Phase 6: 变量检视 — `dbg_variables.h/cpp` (基于帧快照)
- [x] Phase 7: 单步执行 — `dbg_stepping.h/cpp`
- [x] Phase 8: 构建集成 — CMakeLists.txt + 根目录注册

## 产物

| 文件 | 目的 |
|------|------|
| `src/native/diagnostics/debugger/` (18 文件) | Debugger 核心库 |
| `src/native/interpreter/fast_dispatch.cpp` | FastExecute 断点检查 + 帧快照捕获 |
| `src/native/interpreter/interpreter_vm.cpp` | RegisterExecute 断点检查 + 帧快照捕获 |
| `src/native/interpreter/interpreter_vm.h` | ExecutionFrame 添加 method_token 字段 |
| `src/native/interpreter/interp_entry/entry_direct.cpp` | 设置 frame.method_token |
| `src/native/codegen/t4_seh_handler.cpp` | 修复预存编译错误 |
| `CMakeLists.txt` | 注册 debugger/eventpipe subdirectory |
| `src/native/runtime-core/CMakeLists.txt` | 链接 chaos_debugger/chaos_eventpipe |
| `src/native/interpreter/CMakeLists.txt` | MSVC `/D_SILENCE_ALL_CXX17_C99_MATH_MACROS` |

## 构建验证

- `chaos_debugger.lib` — 0 errors ✅
- `chaos_eventpipe.lib` — 0 errors ✅
- `chaos_runtime_core.lib` — 0 errors ✅
- `chaos_codegen.lib` — 0 errors ✅ (修复了 2 个预存 error)
- `chaos_interpreter.lib` — 0 errors ✅ (修复了 1 个预存 error)

## 设计决策

### 帧快照 (Frame Snapshot) 架构

为避免 debugger 模块与 FastFrame/ExecutionFrame 的循环依赖，使用**帧快照**模式：

1. FastExecute/RegisterExecute 在断点命中时，先捕获帧快照（walk prev_frame chain + locals）
2. `dbg_runtime.h` 定义快照结构 `DbgFrameSnapshot` / `DbgCapturedFrame` / `DbgCapturedLocal`
3. debugger server 处理 `stackTrace`/`scopes`/`variables` 请求时，从快照读取数据
4. 快照是 per-process 的（MVP 阶段仅支持单线程调试）

### FastExecute 帧链

entry_direct.cpp 中有 `thread_local void* tls_current_frame` 维护 FastFrame 链。FastExecute 的 `prev_frame` 在 `SetupFastFrame` 时设置，执行返回时恢复：
```
SetupFastFrame → ff->prev_frame = tls_current_frame → tls_current_frame = ff
TearDown      → tls_current_frame = ff->prev_frame
```

### RegisterExecute 路径

RegisterExecute 路径从 entry_direct.cpp 的 FastFrame 内调用，但 `InterpreterVM::Execute` 内部无法访问 FastFrame。为此：
- `ExecutionFrame` 添加 `method_token` 字段
- entry_direct.cpp 在执行前设置 `frame.method_token = patch_method->token`
- 断点检查时仅捕获顶层帧（不含 prev_frame 链）

## 备注

- 帧快照使用进程级全局变量，多线程调试时需要改为 TLS
- 变量检视仅支持 FastExecute 路径的 locals 捕获；RegisterExecute 路径暂未捕获 locals（ExecutionFrame 使用 std::vector<InterpreterValue>）
- VSCode launch.json 配置已创建（`.vscode/launch.json`），支持 native attach/launch
