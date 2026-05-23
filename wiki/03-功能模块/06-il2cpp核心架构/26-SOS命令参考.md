# SOS 命令参考

> CRAG GC WinDbg SOS 调试扩展命令参考。加载 `chaos_gc_sos.dll` 后可通过 `!gc.<command>` 调用。

## 加载扩展

```windbg
.load chaos_gc_sos
!gc.help
```

如果扩展加载成功，会输出 `CRAG GC SOS extension loaded.`。

## 命令总览

| 命令 | 用途 | 输出内容 |
|------|------|---------|
| `!gc.help` | 显示帮助信息 | 所有可用命令列表 |
| `!gc.info` | GC 子系统概要 | GC Index、最后代次、各代回收计数和平均暂停时间 |
| `!gc.stats` | 完整 GC 统计 | Young/Full/Gen1 三组统计 + 分配统计 |
| `!gc.events` | GC 事件环缓冲 | 最近 64 个 GC 事件（类型/暂停时间/处理对象/回收字节） |
| `!gc.histogram` | 暂停时间分布 | 6 桶直方图（0-1ms / 1-5ms / 5-10ms / 10-50ms / 50-100ms / 100+ms） |
| `!gc.dumpall` | 全部转储 | 依次运行 help + info + stats + histogram + events |

## 命令详情

### !gc.info

显示 GC 核心状态：

```
CRAG GC Subsystem State
═══════════════════════
  GC Index:         42
  Last Generation:  Young
  Young Collections:  150
  Full Collections:   3
  Gen1 Collections:   5

  Young Pause (total):  12000000 ns  (avg: 80000 ns)
  Full  Pause (total):  50000000 ns  (avg: 16666666 ns)
  Gen1  Pause (total):  25000000 ns  (avg: 5000000 ns)
```

### !gc.stats

显示详细的 GC 统计计数器：

```
CRAG GC Statistics
══════════════════
── Young Collection ──
  Collections:      150
  Objects Promoted: 45000
  Bytes Promoted:   7200000
  Bytes Reclaimed:  12800000
  Cards Scanned:    8000
  Pause (total):    12000000 ns
  Pause (avg):      80000 ns

── Allocation ──
  Total Allocs:     1000000
  Total Bytes:      256000000
  Oversized Allocs: 50
  Finalization Pending: 12
```

### !gc.events

显示 GC 事件环缓冲（最近 64 个事件）：

```
GC Event Ring Buffer (cap=64, head=12)
══════════════════════════════════════
  #  | Type     | Pause (ns) | Processed | Reclaimed
  ---+----------+------------+-----------+----------
   0 | Young    |      82000 |       300 |     85000
   1 | Young    |      91000 |       420 |    102000
   2 | Full     |   18500000 |     12000 |   4500000
  ...
```

空事件（pause=0, reclaimed=0）自动跳过。如果没有事件，显示 `(no events recorded yet)`。

### !gc.histogram

显示 GC 暂停时间分布（6 桶）：

```
GC Pause Time Histogram (total samples: 158)
══════════════════════════════════════════════
  0-1 ms     :   120  ( 75.9%)
  1-5 ms     :    25  ( 15.8%)
  5-10 ms    :     8  (  5.1%)
  10-50 ms   :     3  (  1.9%)
  50-100 ms  :     2  (  1.3%)
  100+ ms    :     0  (  0.0%)
  ────────────
  Total      :   158  (100.0%)
```

## 数据来源

SOS 扩展通过 `extern "C"` 调试合约符号在目标进程内存中定位 GC 数据：

| 符号 | 类型 | 用途 |
|------|------|------|
| `g_chaos_gc_stats_ptr` | `GcStats*` | 主统计结构体指针 |
| `g_chaos_gc_event_ring_ptr` | `GcEventEntry[]` | 事件环缓冲 |
| `g_chaos_gc_event_ring_head_ptr` | `int32_t*` | 环缓冲头索引 |
| `g_chaos_gc_pause_histogram_ptr` | `uint64_t[]` | 暂停直方图桶 |
| `g_chaos_gc_pause_bucket_bounds_ptr` | `uint64_t[]` | 桶边界值 |

## 结构体布局

SOS 扩展定义了两个 POD 结构体来读取目标内存：

### GcStatsPod (192 字节, pack=8)
与 GC runtime 的 `GcStats` 布局兼容。包含 22 个 `uint64_t`、3 个 `int32_t`、1 个 `uint32_t` padding。

### GcEventEntryPod (32 字节)
与 `GcEventEntry` 布局兼容：`is_full_gc`(8 字节含 padding) + `pause_ns`(8) + `objects_processed`(8) + `bytes_reclaimed`(8)。

## 构建

SOS 扩展需要 Windows DbgEng SDK。构建命令：

```bash
cmake --build build --target chaos_gc_sos
```

在单元测试模式下（`CHAOS_SOS_UNIT_TEST` 定义时），DbgEng 依赖代码被排除，只进行结构体布局验证。
