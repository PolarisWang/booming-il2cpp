# Nightly Build 增强 — 定量回归检测体系 v2

## 概述

对 nightly build 管线进行了 5 个维度的定量增强，扩展 `nightly-delta.json` 的 v2 schema，
新增 `verification/analysis/` 分析模块，以及回归分级（regression grading）CI gate。

## 文件结构

```
tests/e2e/verification/
├── nightly_build.py               ← (不改) 兼容旧入口
├── nightly_delta.py               ← 改: v2 schema 扩展
├── nightly_summary.py             ← 改: 新指标表格
├── nightly_runner/                ← (新增) 新式入口
│   ├── main.py                    ← Jenkins CLI
│   └── ...
├── analysis/                      ← (新增) 分析引擎
│   ├── trend_detector.py          ← 7 日滑动窗口趋势
│   ├── regression_grading.py      ← 回归分级 none/soft/hard
│   └── value_checker.py           ← 值正确性验证
└── stages/
    ├── coverage_audit.py          ← 改: namespace 分解
    ├── benchmark_chunk.py         ← 改: per-method 趋势输出
    ├── fact_chunk.py              ← 改: 值验证 + cross-tech 对比
    └── hotupdate_chunk.py         ← 改: benchmark 渐进启用
```

## Delta v2 Schema

### v1 字段（向后兼容保留）

| 字段 | 类型 | 说明 |
|------|------|------|
| date | string | 报告日期 |
| overall.factPassRate | float? | Fact 通过率 |
| overall.totalBenchmarked | int | Benchmark 方法数 |
| overall.totalCoverageGap | int | 未覆盖方法数 |
| regressions[] | array | 回归列表 |
| improvements[] | array | 改进列表 |

### v2 新增字段

| 字段 | 类型 | 说明 |
|------|------|------|
| `overall.totalCoverageGapPct` | float? | 覆盖缺口百分比 |
| `overall.totalAllocatedBytes` | int | 总内存分配字节 |
| `overall.regressionGrade` | string | 回归分级: none/soft/hard |
| `overall.regressedChunks` | int | 硬回归 chunk 数 |
| `overall.degradedChunks` | int | 软回归 chunk 数 |
| `chunks[N].coverageGapPct` | float? | 该 chunk 的缺口 % |
| `chunks[N].coverageGapPctDelta` | float? | 缺口 % 变化 |
| `chunks[N].benchAllocatedBytes` | float? | 内存分配字节 |
| `chunks[N].benchAllocDelta` | float? | 分配变化 % |
| `chunks[N].huRevertRegression` | int | Revert 后失败的方法数 |
| `chunks[N].profileSizeDelta` | float? | 代码大小变化 % |

## 回归分级（Regression Grading）

### 阈值

| 级别 | 持续时间恶化 | 内存分配恶化 | 含义 |
|------|-------------|-------------|------|
| `none` | < 25% | < 30% | 无回归 |
| `soft` | ≥ 25% | ≥ 30% | 需关注 |
| `hard` | ≥ 50% | ≥ 30% | CI 门禁阻断 |

### 低通滤波

只对 **CV (Coefficient of Variation) < 10%** 的方法进行回归检测。
高方差方法（GC 停顿、JIT 预热影响）自动跳过，避免误报。

阈值基于实际数据校准：

| Assembly | CV>20% 方法占比 |
|----------|----------------|
| System.Linq | 6.5% |
| System.Data.Common | 14.6% |
| System.Private.CoreLib/system-5 | 22.3% |

## 值正确性验证

### 两层对比

1. **Expected value**：如果 `subjects.metadata.json` 包含 `expectedValue` 字段，AOT fact 返回值与此对比
2. **Cross-tech 值对比**：同一方法的 AOT 返回值 vs .NET 8 managed fact 返回值

### 浮点容忍

```python
def values_match(a, b, tolerance=0.01):
    if a == b: return True
    if b == 0: return abs(a) < tolerance
    return abs(a - b) / abs(b) < tolerance
```

## Hotupdate Benchmark

`benchmark_iterations` 从 0（禁用）改为按 chunk 大小自动伸缩：

| 方法数 | benchmark_iterations |
|--------|---------------------|
| < 500 | 3 |
| 500-2000 | 2 |
| > 2000 | 1 |

已知限制：`ChaosDispatchMethodBench` 路径无 SEH 保护，patch 数据加载后
部分方法的 `direct_ptr` 可能失效。详见 `docs/dev/in-progress/nightly-build-enhance/hotupdate-crash-analysis.md`。

## Profile 默认开启

`--run-profile` 标志已替换为 `--no-profile`。Profile pass 默认执行，
收集 AOT 代码大小（方法数 + totalSize）。可通过 `--no-profile` 跳过。

## 报告目录

```
nightly-build-report/
└── YYYYMMDD_HHMMSS-{commit}/
    ├── summary/
    │   ├── nightly-delta.json          ← v2 schema
    │   ├── nightly-summary.md          ← v2 表格
    │   └── pipeline-status.json        ← CI gate 状态
    ├── reports/<asm>/                  ← _dll/reports/latest/
    ├── per-chunk/<asm>/<slug>/         ← chunk 级结果
    ├── benchmark-history/
    ├── logs/<asm>/<slug>/              ← 每 stage 独立日志
    └── metadata/                       ← git/jenkins/config
```
