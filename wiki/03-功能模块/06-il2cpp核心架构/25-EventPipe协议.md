# EventPipe 协议与使用

> EventPipe 是 Chaos IL2CPP 的事件追踪管道，支持 GC、线程池、异常和热更新事件的跨进程采集。仅在 Windows 平台可用。

## 架构

```
源 (GC Runtime / ThreadPool / EH / HotUpdate)
  ↓ 回调注册
ep_bridge.cpp → 收集事件数据
  ↓
ep_event.cpp → 构建 EpEventHeader + payload → 计算校验和
  ↓
ep_transport.cpp → 通过 Windows 命名管道发送
  ↓
chaos_diag.exe (接收器) → 过滤/格式化为 JSON 输出
```

## 事件类型

### GC 事件 (0x0100~0x01FF)

| 事件 | 值 | Payload | 说明 |
|------|----|---------|------|
| `GcStart` | 0x0100 | `uint32_t generation` | GC 开始 |
| `GcEnd` | 0x0101 | `uint64_t pause_ns, uint64_t reclaimed_bytes` | GC 结束 |
| `GcYoungStart` | 0x0102 | `uint64_t nursery_used` | Young GC 开始 |
| `GcYoungEnd` | 0x0103 | 4 × `uint64_t` | Young GC 结束 |
| `GcFullStart` | 0x0104 | `uint32_t page_count` | Full GC 开始 |
| `GcFullEnd` | 0x0105 | 4 × `uint64_t` | Full GC 结束 |
| `GcOom` | 0x0106 | (无 payload) | OOM 发生 |
| `GcGen1Collect` | 0x0107 | 3 × `uint64_t` | Gen1 回收 |
| `GcAllocationTick` | 0x0108 | `uint32_t amount, uint32_t kind` | 分配节拍 |

### 线程池事件 (0x0200~0x02FF)

| 事件 | 值 | Payload |
|------|----|---------|
| `TpWorkerCreate` | 0x0200 | `int32_t thread_id` |
| `TpWorkerDestroy` | 0x0201 | `int32_t thread_id` |
| `TpWorkItemQueue` | 0x0202 | `int64_t work_item_id` |
| `TpWorkItemDequeue` | 0x0203 | `int64_t work_item_id` |
| `TpWorkerAdjust` | 0x0204 | 3 × `int32_t` |
| `TpIoCompletion` | 0x0205 | `uint32_t bytes_transferred` |

### 异常事件 (0x0300~0x03FF)

| 事件 | 值 | Payload |
|------|----|---------|
| `ExceptionThrow` | 0x0300 | `uint64_t exception_ptr` |
| `ExceptionRethrow` | 0x0301 | `uint64_t exception_ptr` |
| `ExceptionCatch` | 0x0302 | `uint64_t exception_ptr` |

### 热更新事件 (0x0400~0x04FF)

| 事件 | 值 | Payload |
|------|----|---------|
| `HuAssemblyLoaded` | 0x0400 | `module_id + alc_id + name[128]` |
| `HuAssemblyUnloaded` | 0x0401 | `module_id + alc_id + name[128]` |
| `HuPackageLoaded` | 0x0402 | `package_id[128]` |
| `HuPackageLoadFailed` | 0x0403 | `package_id[128] + error[256]` |
| `HuPackageUnloaded` | 0x0404 | `package_id[128]` |
| `HuMethodReplaced` | 0x0405 | `uint32_t method_token` |
| `HuMethodReverted` | 0x0406 | `uint32_t method_token` |
| `HuError` | 0x0407 | `error[256]` |

## Wire 协议

### 帧格式
```
┌─────────────────┬──────────────┬──────────────────┬──────────────┐
│ EpEventHeader   │ Payload      │ uint32_t checksum │
│ (24 字节)       │ (变长)       │ (4 字节)          │
└─────────────────┴──────────────┴──────────────────┘
```

### EpEventHeader 布局
```
偏移  大小  字段
0     4     magic      (0xACDB12CE)
4     4     version    (1)
8     4     event_type (EpEventType 枚举值)
12    4     payload_size
16    8     timestamp  (QueryPerformanceCounter 值)
```

### 校验和
校验和基于 XOR 算法：遍历 header 所有字节（24 字节）+ payload 所有字节。空 payload 只计算 header。

## 使用方式

### 启动接收器
```bash
chaos_diag.exe -p <pid> [-f gc,tp,exc,hu]
```

参数：
- `-p`：目标进程 PID（必填）
- `-f`：事件类别过滤器，用逗号分隔；支持 gc/tp/exc/hu；默认全开

### 输出格式
接收器将事件输出为 JSON 行（每行一个事件）：
```json
{"event":"GcYoungEnd","timestamp":12345678,"payload":{"pause_ns":150000,"objects_promoted":42,...}}
```

## 构建控制

```cmake
# diagnostics/CMakeLists.txt
# CHECK/PROFILE 构建启用 GC 事件
target_compile_definitions(chaos_eventpipe PRIVATE
    CHAOS_IL2CPP_GC_EVENTS=$<BOOL:${CHAOS_IL2CPP_GC_EVENTS}>)
```
