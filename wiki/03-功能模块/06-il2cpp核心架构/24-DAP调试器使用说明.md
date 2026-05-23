# DAP 调试器使用说明

> Chaos IL2CPP 实现了 Debug Adapter Protocol (DAP)，通过 VS Code 的 C# 或 Debug Visualizer 扩展进行调试。

## 启用

DAP 调试器在 CHECK/PROFILE 构建中默认启用，由 `CHAOS_IL2CPP_DEBUGGER` 编译开关控制：

```cpp
// src/native/diagnostics/CMakeLists.txt
// 在 cmake/config-tier.cmake 中 CHECK/PROFILE 构建自动定义
target_compile_definitions(chaos_debugger PRIVATE CHAOS_IL2CPP_DEBUGGER=1)
```

SHIP 构建中完全禁用（`CHAOS_IL2CPP_DEBUGGER=0`），零开销。

## 连接

DAP 通过标准输入/输出进行 JSON-RPC 通信。VS Code 的 `launch.json` 配置示例：

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Chaos IL2CPP Debug",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/my_game.exe",
            "args": [],
            "stopAtEntry": false,
            "console": "integratedTerminal"
        }
    ]
}
```

## 支持的功能

| 功能 | 当前状态 | DAP 请求 | 说明 |
|------|---------|---------|------|
| 初始化/配置 | 完成 | `initialize`, `configurationDone`, `launch` | 标准 DAP 握手流程 |
| 断点 | 完成 | `setBreakpoints`, `setFunctionBreakpoints` | 按源文件行号设置；支持多次覆盖 |
| 暂停/继续 | 完成 | `pause`, `continue` | `pause` 设置原子标志，解释器指令边界检查并停止 |
| 步进 | 完成 | `next`, `stepIn`, `stepOut` | 支持 StepOver/StepInto/StepOut 三种模式 |
| 调用栈 | 完成 | `stackTrace` | 按帧深度返回调用栈 |
| 变量 | 完成 | `scopes`, `variables` | 返回局部变量和参数的名称、值、类型 |
| 表达式求值 | 完成 | `evaluate` | 支持整数常量（十进制/十六进制）和局部变量名匹配 |
| 线程 | 完成 | `threads` | 始终返回 "Main Thread" (ID=1) |
| 断开 | 完成 | `disconnect` | 清除所有断点和步进状态 |

## 调试事件

DAP 服务器在以下场景自动发送 `stopped` 事件：
- **`breakpoint`**: 到达预设断点时触发
- **`step`**: 步进完成后触发
- **`pause`**: 用户主动暂停时触发
- **`entry`**: 暂不支持（stopAtEntry 未实现）

## 实现细节

### 断点解析流程
```
setBreakpoints(source.path, line[])
  → DbgResolveMethodsForSource() 通过 MetadataRegistry 查找
    与源文件关联的 method token
  → DbgSetBreakpoint(token, il_offset, path, line)
  → 存入 ConcurrentMap<token, BreakpointInfo>
```

### 暂停机制
```
pause 命令 → g_dbg_pause_requested = true
            ↓
解释器指令循环每次迭代检查 DbgShouldPause()
  → 优先级: pause > breakpoint > step
  → 暂停时发送 stopped 事件 (reason: "pause")
  → 等待 DbgSignalResume() condvar 唤醒
```

### 步进状态机
```
StepInto → 每条指令后暂停（不跳过 call 内部）
StepOver → 在当前帧内跳过 call 内部（帧深度跟踪）
StepOut  → 运行到当前帧返回后暂停
Continue → 清除所有步进状态
```

## 已知限制

- 单线程调试（仅支持主线程）
- 不支持条件断点和日志断点
- 不支持 `stopAtEntry`
- 表达式求值支持整数字面量和局部变量名，不支持复杂表达式
- 变量展开只支持托管对象字段（递归深度有限制）
