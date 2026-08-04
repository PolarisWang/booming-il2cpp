# Benchmark 数据分析报告

> 数据来源: `testing/foundation-dll/*/_dll/reports/latest/benchmark-summary.json`
> 分析时间: 2026-06-29

---

## 1. 总览

| 指标 | 数值 |
|------|------|
| 有数据的 Family | 24 |
| 0 方法 Family | 6 |
| 总 benchmark 方法数 | ~2,500 |

## 2. Benchmark 覆盖率

### 🟢 充足 (>100 methods)
| Family | 方法数 | 说明 |
|--------|--------|------|
| System.Linq.Expressions | 726 | ✅ 充足 |
| System.ComponentModel.TypeConverter | 515 | ✅ |
| System.Linq | 157 | ✅ |
| System.Diagnostics.DiagnosticSource | 125 | ✅ |
| System.Net.Http | 113 | ✅ |

### 🔴 不足 (<30 methods)
| Family | 方法数 | 问题 |
|--------|--------|------|
| System.Data.Common | 21 | 🔴 覆盖率低 |
| System.IO.Compression.Brotli | 21 | 🔴 |
| System.IO.Pipelines | 23 | 🔴 |
| System.Runtime.InteropServices | 29 | 🔴 |
| System.Net.ServerSentEvents | 6 | 🔴 极低 |
| System.ObjectModel | 3 | 🔴 极低 |

### ⚫ 无 benchmark 数据
System.Formats.Asn1, System.Net.Sockets, System.Runtime, 
System.Runtime.Intrinsics, System.Security.Cryptography, System.Xml.ReaderWriter

## 3. 性能异常点

### 3.1 maxCV > 1.0 (结果不稳定)

**maxCV** = 所有方法中最大的变异系数 (标准差/均值)。>1.0 表示该方法的结果偏差比均值还大。

| Family | maxCV | 异常数/总数 | 根因推测 |
|--------|-------|-----------|----------|
| System.Linq.Expressions | **2.90** | 278/726 | 表达式树 JIT 编译首次加载 |
| System.Private.CoreLib | **2.73** | 31/36 | CoreLib 冷启动影响 |
| System.IO.Compression.ZipFile | **2.68** | 52/66 | 文件 I/O 非确定性延迟 |
| System.Net.Http | **1.54** | 75/113 | 网络请求延迟波动 |
| System.Data.Common | **1.51** | 19/21 | ADO.NET 连接初始化 |
| System.Collections | **1.22** | 37/70 | GC 分配触发 |
| System.Threading.Tasks.Parallel | **1.06** | 140/106 | 线程调度抖动 |

### 3.2 avg 耗时异常

| Family | avg | 峰值 | 说明 |
|--------|-----|------|------|
| System.Linq | **9.86ms** | 最高 | CloseTo/SequenceEqual 等大量比较操作 |
| System.Collections | **9.77ms** | 中 | Hashtable 等旧集合操作 |
| System.IO.Compression.Brotli | **5.93ms** | 中等 | 压缩算法本身慢 |
| System.Net.ServerSentEvents | **4.64ms** | 低 | 6 个方法，样本不足 |
| System.ObjectModel | **8.20ms** | 低 | 仅 3 个方法 |

## 4. 修复建议

| 优先级 | 问题 | 方案 |
|--------|------|------|
| P0 | 6 个 family 无 benchmark 数据 | 在 chunk.json 中补充 benchmark 配置 |
| P1 | maxCV > 2.0 (Linq.Expressions, CoreLib, ZipFile) | benchmark 迭代次数从 3 次增加到 10 次 |
| P2 | avg > 5ms 的 family | 拆分为更小的 benchmark method(每个只测单一操作) |
| P3 | 覆盖率 <30 methods 的 family | 扩展 subject 配置增加 benchmark method |

## 5. 架构影响

当前 benchmark 数据未检测到本次大会话修复导致的性能回归。但数据有限（仅 ~2,500 methods，且部分 family 覆盖率低），建议在 CI 中运行 **完整 benchmark 管线** 后再次对比。
