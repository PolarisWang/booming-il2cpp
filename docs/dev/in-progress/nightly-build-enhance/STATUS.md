# Nightly Build 增强 — STATUS

## Phase
`brainstorming → design completed → phases 1-5 completed ✅`

## 边界拍板
- 5 个维度全部选最高强度 C 方案
- 新增 `verification/analysis/` 模块（value_checker, trend_detector, regression_grading）
- 改 6 个 stage/aggregate/delta/summary 文件
- Profile stage 默认开启（--no-profile 可关闭）
- Hotupdate benchmark 渐进启用（由 chunk 大小自动伸缩）

## 当前结论
全部 5 个 Phase 已完成实现，35 项既有测试全部通过。

### 已实现改动清单

| 维度 | 文件 | 改动 |
|------|------|------|
| **Coverage** | `stages/coverage_audit.py` | 新增 namespace 级 gap 分解、coveragePct |
| | `stages/aggregate.py` | 新增 coveragePct、memoryAlloc 入 dashboard |
| | `nightly_delta.py` | 新增 coverageGapPct、coverageGapPctDelta、coverage 趋势 |
| | `nightly_summary.py` | Coverage 列改为 gap + % |
| **Benchmark** | `stages/benchmark_chunk.py` | 新增 benchmark-trend.json per-method 输出 |
| | `analysis/regression_grading.py` | **新增** — 回归分级 none/soft/hard |
| | `nightly_delta.py` | 新增 regressionGrade、regressedChunks/degradedChunks |
| | `nightly_summary.py` | 新增 Regression Grade 行 |
| **Fact** | `stages/fact_chunk.py` | 值验证（AOT vs expected）、cross-tech 对比（AOT vs .NET 8） |
| | `analysis/value_checker.py` | **新增** — 浮点容忍值匹配引擎 |
| | | 新增 per-method fact 历史（fact-per-method-*.jsonl） |
| **Memory** | `stages/aggregate.py` | 新增 memory.totalAllocatedBytes、memory 入 dashboard |
| | `nightly_delta.py` | 新增 benchAllocatedBytes、benchAllocDelta、profileSizeDelta |
| | `nightly_summary.py` | 新增 Alloc 列 |
| | `nightly_runner/config.py` | profile 默认开启 |
| | `nightly_build.py` | --no-profile 替代 --run-profile |
| **Hotupdate** | `stages/hotupdate_chunk.py` | benchmark 渐进启用、revert integrity per-method 检查 |
| | `nightly_delta.py` | 新增 huRevertRegression、huHasBaselineBenchmark |
| | `nightly_summary.py` | HU 列新增 revert regression 指示 |
| **Analysis** | `analysis/__init__.py` | **新增** 空包 |
| | `analysis/trend_detector.py` | **新增** — 7 日滑动窗口趋势计算 |
| | `analysis/regression_grading.py` | **新增** — 回归分级 none/soft/hard |
| | `analysis/value_checker.py` | **新增** — 值比较引擎 |

### 测试
`test_nightly_build.py` 35 项全部通过 ✅
