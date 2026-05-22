# CRAG GC ETW 诊断事件

## Provider 身份

| 属性 | 值 |
|------|-----|
| Provider Name | `Chaos-IL2CPP-GC` |
| Provider GUID | `{3A5C4B6D-8E9F-4A2B-9C1D-7E8F0A3B6C5D}` |
| Keyword | `0x1` (GC lifecycle events) |
| Level | 4 (Informational) |

## Events

| ID | Name | Payload | 触发点 |
|----|------|---------|--------|
| 0x01 | GCStart | generation (uint32) | 全量 GC 开始 |
| 0x02 | GCEnd | pause_ns, reclaimed_bytes (uint64×2) | 全量 GC 结束 |
| 0x03 | GCYoungStart | nursery_used (uint64) | Young GC 开始 |
| 0x04 | GCYoungEnd | pause_ns, objects_promoted, bytes_promoted, bytes_reclaimed (uint64×4) | Young GC 结束 |
| 0x05 | GCFullStart | page_count (uint32) | Full GC 开始 |
| 0x06 | GCFullEnd | pause_ns, reclaimed_bytes, objects_marked, pages_collected (uint64×4) | Full GC 结束 |
| 0x07 | GCOOM | (无 payload) | 内存耗尽 |
| 0x08 | GCGen1Collect | pause_ns, objects_promoted, bytes_reclaimed (uint64×3) | Gen1 收集完成 |

所有 Event Version = 0，与 Keyword 0x1 关联。

## 集成点

ETW fire 调用与现有 `GcFireEvent()` 回调一一对应，在以下文件中添加：

| 文件 | 事件 | 插入点 |
|------|------|--------|
| `gc_young_collector.cpp` | YoungStart, YoungEnd | GC_YOUNG_START / GC_YOUNG_DONE 处 |
| `gc_old_gen.cpp` | GCStart, GCEnd, FullStart, FullEnd | Collect() 入口 / GC_FULL_DONE 处 |
| `gc_gen1.cpp` | Gen1Collect | GcGen1Collection() 返回前 |
| `gc_bgc.cpp` | Gen1Collect (占位) | BGC 的 GC_GEN1_COLLECT 处 |

## 平台约束

- **仅 Windows**: 依赖 `<evntprov.h>` (EventRegister/EventWrite API)
- **非 Windows**: 所有函数编译为空实现（`noexcept {}`）
- **编译开关**: `CHAOS_IL2CPP_GC_EVENTS=1`（CMake option，默认 ON）

## 使用方式

### 生命周期

Provider 采用 lazy init：首次 `GcEtwFireXxx` 调用时自动 `EventRegister`，`GcShutdownEtw()` 在 runtime 关闭时调用 `EventUnregister`。

### 通过 xperf 捕获

```bat
:: 启动会话，捕获 Chaos-IL2CPP-GC provider 事件
xperf -start GCProviders -on *Chaos-IL2CPP-GC

:: 运行目标程序
your_app.exe

:: 停止并保存
xperf -stop GCProviders -o gc_events.etl

:: 查看事件
xperf -i gc_events.etl
```

### 通过 PerfView 捕获

启动 PerfView -> Collect -> 在 "Advanced Options" -> "Additional Providers" 中添加 `{3A5C4B6D-8E9F-4A2B-9C1D-7E8F0A3B6C5D}` -> Start Collection。

### 通过 Xperf/WPA 分析事件时间线

事件触发时包含 pause_ns（自旋计时的纳秒级暂停耗时），可用于分析：
- Young GC 频率与暂停分布
- Full GC 标记/回收效率
- Gen1 收集的晋升速率

## 实现参考

- Provider 声明: `gc_etw.h`
- Provider 实现: `gc_etw.cpp`
- 遵循 `runtime_stubs/threadpool_events.cpp` 的模式（`EVENT_DESCRIPTOR` 常量、`EventDataDescCreate` + `EventWrite`、`REGHANDLE` + `g_provider_registered` 原子标志）
