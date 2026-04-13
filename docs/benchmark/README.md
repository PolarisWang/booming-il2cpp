# Benchmark Dashboard

此目录由工具自动维护，**请勿手动编辑** JSON 文件。

## 文件说明

| 文件 | 说明 |
|------|------|
| `dashboard.html` | 静态 HTML 仪表盘，用浏览器直接打开 |
| `overview.json` | 所有 subject 最新数据汇总（机器可读） |
| `subjects/*.json` | 每个 subject 的完整历史记录 |

## 数据更新方式

```bash
# 跑某个 subject 的 native benchmark 并记录
run benchmark --subject PerformanceFeaturePack --mode native --record

# 跑所有 Bench* subjects（全部模式）
run benchmark --all --record

# 仅重新生成 dashboard（不跑 benchmark）
run benchmark --dashboard --open
```

## 三方对比模式

| 模式 | 说明 | 性能目标 |
|------|------|---------|
| `managed` | C# (.NET CLR JIT) | 基线参考 |
| `native` | IL2CPP AOT 生成 C++ | ≥ 2x faster than C# |
| `interpreter` | HotUpdate 解释执行 | ≤ 50x slower than C# |

## 设备记录

数据按 `(subject, mode, device_id)` 三元组存储，支持多设备对比。  
原始数据: `subjects/{subject}/benchmark-records/records.jsonl`
