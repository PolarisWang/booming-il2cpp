---
name: dev-pressure-test-management
description: Pressure Test Management — GC/容量/线程压力测试管理，记录结果，检测回归
---

# Pressure Test Management

## 何时使用

修改了 GC、内存分配、线程调度等相关代码后，用于验证系统稳定性和检测性能退化。

## 可用命令

```
run stress list                        # 列出可用压力测试（gc-stress, capacity-stress, threading-stress, gc-stress-mode）
run stress gc-stress --build --record  # 编译并运行 GC 压力测试，记录结果
run stress gc-stress --quick           # 快速模式（<30s），AI 开发中频繁使用
run stress capacity-stress --build     # 容量压力测试（200+模块注册/卸载）
run stress threading-stress --workers 16 --duration 30000  # 线程压力测试
run stress all --quick                 # 全部压力测试快速模式
run stress status                      # 查看所有测试最近一次运行状态
run stress history gc-stress           # 查看 GC 压力测试历史记录
run stress compare gc-stress <id1> <id2>   # 对比两次运行指标
run stress regressions                 # 扫描全部测试，报告回归
run stress export gc-stress --output <path>  # 导出结果
run stress import <path>               # 导入历史结果
```

## 常用场景

| 场景 | 命令 | 预期耗时 |
|------|------|----------|
| GC 改动后快速验证 | `run stress gc-stress --quick` | ~30s |
| 完整 GC 压测 | `run stress gc-stress --build --record` | ~5min |
| 改动后全量检查 | `run stress all --quick` | ~1min |
| 提交前回归检查 | `run stress regressions` | ~1s |
| 查看状态 | `run stress status` | ~1s |

## 数据存储

- 记录: `.artifact/verification/stress-records/{test-name}/records.jsonl`
- 基线备份: `verification-history/stress/{test-name}/{timestamp}/`
- 记录字段: runId, testName, scenarioName, status, recordedAt, gitCommit, metrics, config, errors

## 回归阈值

| 指标 | 阈值 | 说明 |
|------|------|------|
| patternVerificationFailures | >0 | 任何校验失败即回归 |
| youngAvgPauseNs / fullAvgPauseNs | >20% 增加 | GC 停顿时间退化 |
| opsPerSecond | >15% 下降 | 线程吞吐量退化 |
