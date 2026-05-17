# 压力测试管理系统

> 统一 CLI 入口，覆盖 GC stress、capacity stress、threading stress 测试，支持 JSONL 记录、基线备份和回归检测。

## 入口

```bash
run stress <test-name> [options]
run stress list|status|history|compare|export|import|regressions
```

## 可用测试

| 测试 | 描述 | 二进制 | 典型耗时 |
|------|------|--------|----------|
| `gc-stress` | 11 场景 GC 压力（A-K：并发分配、混合大小、young GC、domain unload 等） | `chaos_gc_stress_test` | 10-15 分钟 |
| `gc-stress-mode` | GC 重入安全（mode=1 每次分配后 full GC, mode=2 每次 safepoint 后 heap verify） | `chaos_gc_sanity_test` | < 1 秒 |
| `capacity-stress` | 200+ DLL 容量（module/domain/assembly 注册极限） | `chaos_capacity_stress_test` | < 5 秒 |
| `threading-stress` | 多线程 monitor/interlocked 并发 | `entry.exe` (foundation-dll 构建) | < 30 秒 |

## 选项

| 选项 | 说明 |
|------|------|
| `--record` | 存入 JSONL 记录 |
| `--json` | JSON 格式输出（AI 友好） |
| `--quick` / `-q` | 缩减规模快速验证 |
| `--build` | 先构建测试二进制 |
| `--workers N` | 覆盖线程数 |
| `--duration N` | 覆盖持续时长(ms) |
| `--mode N` | gc-stress-mode 专用：1=alloc-stress, 2=verify-stress |

## 管理命令

```bash
run stress list                          # 列出可用测试
run stress status [<test-name>]          # 最新结果摘要
run stress history <test-name>           # 最近运行记录
run stress compare <test> <id1> [id2]    # 对比两次运行
run stress regressions                   # 检测所有测试的回归
run stress export <test> --output <path> # 导出记录
run stress import <path>                 # 导入记录
```

## 数据存储

```text
.artifact/verification/stress-records/
  gc-stress/records.jsonl           # JSONL 格式，append-only
  gc-stress-mode/records.jsonl
  capacity-stress/records.jsonl
  threading-stress/records.jsonl

verification-history/stress/
  gc-stress/latest/baseline.json     # 最新全量基线
  gc-stress/20260517T033326/          # 历史备份
```

## 回归检测阈值

| 指标 | 阈值 | 方向 |
|------|------|------|
| `patternVerificationFailures` | 0 | 任何新增为回归 |
| `youngAvgPauseNs` | +20% | 上涨为回归 |
| `fullAvgPauseNs` | +20% | 上涨为回归 |
| `activeRegionsAfter` | +30% | 上涨可能为泄漏 |
| `failures` | 0 | 任何失败为回归 |
| `opsPerSecond` | -15% | 下降为回归 |

## 基线记录

2026-05-17 建立首次基线：

| 测试 | 结果 | 提交 |
|------|------|------|
| gc-stress-mode=1 | 17/17 passed | 07ffcb4d |
| gc-stress-mode=2 | 17/17 passed | 07ffcb4d |
| capacity-stress | 6/6 passed | 07ffcb4d |
| threading-stress | 15/15 passed | 07ffcb4d |

## 已知缺口

- **gc-stress** 全量 11 场景挂在场景 B（缺少 CollectorThread），仅场景 A 可过。是测试自身问题，非运行时回归。
- **threading-stress** ops/sec 指标始终为 0（entry.exe `--stress` 输出 "Passed: N/M" 而非 ops/sec）。
- **--quick 模式** gc-stress 的 `CHAOS_IL2CPP_STRESS_SCALE` 环境变量支持已从测试代码中移除，当前 --quick 不缩减负载。

## 关键文件

| 文件 | 职责 |
|------|------|
| `build/toolchains/run/commands/stress.py` | CLI 命令分发 |
| `build/toolchains/run/testing/stress_records.py` | JSONL 存储 |
| `build/toolchains/run/testing/stress_runners.py` | 4 个测试适配器 |
| `build/toolchains/run/testing/stress_comparison.py` | 回归检测 |
