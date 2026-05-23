# P1-A: 废弃 debug_agent + 清理 CMake（加入 DAP/EventPipe/SOS）

## Status

`completed`

## Parent Task

`diagnostics-industrialization`

## Source Relation

`roadmap-child`

## Clearance

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## Scope

废弃 v1 debug_agent 相关文件，将 v2 DAP 调试器（`debugger/`）、EventPipe（`eventpipe/`）、SOS 扩展（`sos/`）加入构建系统。

## Tasks

1. ✅ 全局搜索 debug_agent 的引用（其他模块是否 include 或 link `chaos_diagnostics`），确认废弃安全
2. ✅ 删除 debug_agent.*、debug_breakpoints.*、debug_stack_walk.*、debug_variables.*、debug_commands.h
3. ✅ 更新 `diagnostics/CMakeLists.txt`：
   - 去掉 debug_agent 源文件
   - 改为 umbrella CMakeLists.txt，包含 debugger/、eventpipe/、sos/ 子目录
4. ✅ 更新根 `CMakeLists.txt`：三个独立 add_subdirectory → 单一 `add_subdirectory(src/native/diagnostics)`
5. ✅ 编译验证：chaos_debugger + chaos_eventpipe + chaos_gc_sos 全部编译通过

## Expected Outputs

- 删除 9 个废弃文件 ✅
- 修改后的 `diagnostics/CMakeLists.txt` ✅
- 修改后的根 `CMakeLists.txt` ✅
- 验证编译通过的构建日志 ✅

## Exit Criteria

- 所有废弃文件已删除 ✅
- CMakeLists.txt 正确包含三个子目录 ✅
- `cmake --build` 全部通过（chaos_debugger + chaos_eventpipe + chaos_gc_sos）✅
- 无副作用（其他模块不引用已删除文件）✅

## Terminal Notes

执行摘要：
- 全局搜索确认：`chaos_diagnostics` 仅被自身 CMakeLists.txt 引用，无外部 C++ 依赖
- `hotupdate_events.h` 保留（被 ep_hotupdate_bridge.cpp 和 assembly_manager.cpp 使用）
- 根 CMakeLists.txt 原本直接包含三个子目录（lines 45-46, 58-62），现改为单一 `add_subdirectory(src/native/diagnostics)`
- SOS 条件检查（DbgEng SDK）移至 diagnostics/CMakeLists.txt umbrella 内
- CMake configure + build 全部通过，0 error
