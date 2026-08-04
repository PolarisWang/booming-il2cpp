# Benchmark 优化方案

## 现状

| 等级 | 方法数 | Family |
|------|--------|--------|
| 🟢 充足 | 282 | System.Linq.Expressions |
| 🟢 充足 | 270 | System.Linq |
| 🟢 充足 | 228 | System.Diagnostics.DiagnosticSource |
| 🟢 充足 | 98 | System.IO.Compression.Brotli |
| 🟢 充足 | 96 | System.Net.Http |
| 🟢 中等 | 54 | System.Collections.NonGeneric |
| 🟢 中等 | 30 | System.Threading.Tasks.Parallel |
| 🟡 少量 | 30 | System.Runtime.InteropServices |
| 🟡 少量 | 18 | System.Runtime.Serialization.Formatters |
| 🟡 少量 | 12 | System.Reflection.Metadata |
| 🟡 少量 | 10 | System.Collections |
| 🟡 少量 | 8 | System.Net.ServerSentEvents |
| 🟡 少量 | 8 | System.Collections.Immutable |
| 🟡 少量 | 4 | System.Text.Json |
| 🔴 极少 | 0 | System.ObjectModel, System.Data.Common, IO.Compression.ZipFile, Security.Claims, Security.Principal.Windows, ComponentModel.TypeConverter |

## Benchmark 覆盖率的限制因素

Benchmark 方法来自 AutoTestGenerator 对 `[Benchmark]` attribute 的检测。覆盖率低的 family 的根因是 ATG 的 benchmark subject 配置不足，**不是 codegen 或编译问题**。

## 优化方向

| 优先级 | 优化 | 说明 | 执行方 |
|--------|------|------|--------|
| P0 | maxCV 优化 — 增加迭代次数 | 当前 3 次迭代 → 10 次，降低 CV | **benchmark 管线** |
| P1 | 0 benchmark 的 family | 在 chunk.json 中补充 benchmark subject | **ATG 配置** |
| P2 | 高 avg 耗时的 family | 拆分 benchmark method 为更细粒度 | **ATG 配置** |
| P3 | 异常点分析 | 对 outliers 做逐方法分析 | **数据分析** |

## 可立即执行的修复

修复高 CV 的方法：在 benchmark 阶段增加迭代次数（3→10）。
