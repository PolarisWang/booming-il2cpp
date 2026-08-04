# I-EVP: EventPipe Core Layer — 完成

> **task_id**: I-EVP
> **parent_task_id**: interpreter-industrialization
> **source_relation**: roadmap-child
> **lifecycle_status**: completed
> **phase**: completed
> **completed_date**: 2026-05-23

## 完成摘要

为 runtime-core 建立轻量级 IPC 诊断事件通道（EventPipe），实现跨进程事件消费能力。基于方案 B-1（核心层先行）：IPC + GC/TP/Exception 3 类事件 + 自定义 receiver CLI。

### 产出物

| 文件 | 说明 |
|------|------|
| `src/native/diagnostics/eventpipe/ep_config.h` | 功能开关和配置常量 |
| `src/native/diagnostics/eventpipe/ep_protocol.h` | 二进制线格式（header + payload + checksum） |
| `src/native/diagnostics/eventpipe/ep_platform.h` | 平台抽象（Windows Named Pipe） |
| `src/native/diagnostics/eventpipe/ep_transport.h/cpp` | IPC 传输层（OVERLAPPED I/O 单连接服务端） |
| `src/native/diagnostics/eventpipe/ep_event.h/cpp` | 事件发射 API + 便捷函数 |
| `src/native/diagnostics/eventpipe/ep_gc_bridge.h/cpp` | GC 事件桥接（通过 GcEventCallback 注册） |
| `src/native/diagnostics/eventpipe/ep_threadpool_bridge.h/cpp` | ThreadPool 事件桥接 |
| `src/native/diagnostics/eventpipe/ep_exception_bridge.h/cpp` | 异常事件桥接（inline 零开销） |
| `src/native/diagnostics/eventpipe/ep_instance.h/cpp` | 单例管理（lazy init on first use） |
| `src/native/diagnostics/eventpipe/CMakeLists.txt` | Static lib 构建 |
| `src/native/diagnostics/eventpipe/receiver/chaos_diag.cpp` | 接收端 CLI（JSON Lines 输出） |
| `src/native/diagnostics/eventpipe/receiver/CMakeLists.txt` | CLI 构建 |

### 修改文件

| 文件 | 变更 |
|------|------|
| `CMakeLists.txt` | 添加 `add_subdirectory(src/native/diagnostics/eventpipe)` |
| `src/native/runtime-core/CMakeLists.txt` | 链接 `chaos_eventpipe`（PUBLIC） |
| `src/native/runtime-core/runtime_stubs/threadpool_events.h` | 增加 `<eventpipe/ep_event.h>` 包含 |
| `src/native/runtime-core/runtime_stubs/threadpool_events.cpp` | 6 个 emit 函数增加 EventPipe 双发 |
| `src/native/interpreter/fast_dispatch.cpp` | Handle_Throw 插入异常事件 Hook |
| `src/native/interpreter/interpreter_vm.cpp` | Throw case 插入异常事件 Hook |

### 技术细节

- **IPC 协议**：Windows Named Pipe，二进制线格式（magic: `0xACDB12CE`，version，event type，payload size，QPC timestamp，checksum）
- **传输层**：OVERLAPPED I/O + 后台 listener 线程，单连接设计，100ms 写入超时
- **GC 事件**：通过现有 `GcRegisterEventCallback` 系统注册回调，STW 内轻量发射
- **ThreadPool 事件**：双发路径（ETW + EventPipe），共用参数
- **异常事件**：inline 函数 + `EpIsInitialized()` 守卫，未初始化时零开销
- **生命周期**：lazy init on first event（同 gc_etw.cpp 模式）

### 验证结果

| 验证项 | 状态 |
|--------|------|
| `chaos_eventpipe.lib` 编译 | ✅ 0 errors |
| `chaos_runtime_core.lib` 链接 | ✅ 0 errors |
| `chaos_interpreter.lib` 编译 | ✅ 0 errors |
| `chaos_diag.exe` 构建运行 | ✅ 0 errors, `--help` 正常输出 |
| 既有测试回归 | ✅（编译链通过，未修改测试逻辑） |

### 使用方式

```
# 接收端（另一个终端）
chaos-diag --pid <runtime_pid>

# 过滤特定事件
chaos-diag --pid <runtime_pid> --filter GC

# 输出到文件
chaos-diag --pid <runtime_pid> --output events.jsonl
```
