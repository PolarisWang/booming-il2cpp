# Nightly Build 改进 — 五维度增强设计 v1

## 背景

基于 nightly build 差距分析，5 个维度（coverage、fact、benchmark、memory、hotupdate）均存在数据浪费或缺失。所有维度选最高强度方案（C）。

## 架构总览

```
nightly-delta.json v2（向后兼容 v1）
  ├── coverage:    gapPct + namespace分解 + 7日趋势 + ⚠️预警
  ├── fact:        valueCorrectRate + crossTechMatchRate + per-method历史
  ├── benchmark:   ops/sec + allocatedBytes + per-method CV + 分级(soft/hard)
  ├── memory:      totalAllocatedBytes + gcPauseNs + profileSize + GC趋势
  └── hotupdate:   benchEnabled + perfImpact + revertRate + integrity
```

## 各维度设计

### 1. Coverage C — 百分比 + namespace 分解 + 7 日趋势预警

**改动的文件**：
| 文件 | 改动 |
|------|------|
| `stages/coverage_audit.py` | 新增 `namespaceGaps` 输出（按 namespace 统计 declared/missing/gapPct） |
| `stages/aggregate.py` | 新增 `coverage.overallPct`、`coverage.trendDirection` 入 dashboard.json |
| `nightly_delta.py` | `coverageGapPct` 替代 `coverageGap` 做 delta；新增 `coverageTrend` 字段 |
| `nightly_summary.py` | Coverage 列从原始数字改为 `gapPct%` + 趋势 emoji |

**趋势预警算法**：
```
sliding window 7 天
  last_3_gaps = [gapPct(d-2), gapPct(d-1), gapPct(d)]
  prev_4_gaps = [gapPct(d-6), gapPct(d-5), gapPct(d-4), gapPct(d-3)]
  if mean(last_3) > mean(prev_4) * 1.05 → "expanding" ⚠️
  if mean(last_3) < mean(prev_4) * 0.95 → "narrowing" ✅
  else → "stable"
```

### 2. Fact C — 值正确性 + cross-tech 对比

**改动的文件**：
| 文件 | 改动 |
|------|------|
| `stages/fact_chunk.py` | 值验证逻辑：对比 fact result value vs metadata expectedValue；新增 cross-tech 对比（AOT vs .NET 8 同一 methodSubjectId 的 value） |
| `verification/analysis/value_checker.py` | **新增** — 值验证引擎，浮点容忍度比较 |
| `stages/aggregate.py` | 新增 `valueCorrectRate`、`crossTechMatchRate` 入 dashboard |
| `nightly_delta.py` | 新增 `valueCorrectRate`、`crossTechMatchRate`、`factStability` |
| `nightly_summary.py` | 新增 Fact Quality 表格行 |

**浮点比较容忍度**：
```
def values_match(a, b, tolerance=0.01):
    if a == b: return True
    if b == 0: return abs(a) < tolerance
    return abs(a - b) / abs(b) < tolerance
```

### 3. Benchmark C — 校准修复 + 回归分级 + CI gate

**改动的文件**：
| 文件 | 改动 |
|------|------|
| `stages/benchmark_chunk.py` | `_calibrate_iterations()` 改为全方法中位数校准；新增 per-method 趋势计算 → `benchmark-trend.json` |
| `nightly_delta.py` | 新增 `benchRegressionGrade`（none/soft/hard）；新增 `opsPerSecDelta`、`allocDelta` |
| `nightly_summary.py` | 新增 Regression Grade 列 |
| `orchestrator.py` / `report_collector.py` | `pipeline-status.json` 新增 `ciGate.passed` |

**回归分级算法**：
```
per-method regression detection:
  for each method with 7-day history:
      mean_elapsed_7d = historical_avg
      if today_elapsed > mean_elapsed_7d * 1.20 → hard regression
      if today_elapsed > mean_elapsed_7d * 1.10 → soft regression

chunk-level grade:
  if any method has hard regression → grade="hard"
  elif any method has soft regression → grade="soft"
  else → grade="none"

CI gate:
  if grade="hard" → ciGate.passed=false
```

### 4. Memory C — GC 指标 + profile 默认开启

**改动的文件**：
| 文件 | 改动 |
|------|------|
| `stages/aggregate.py` | 读取 benchmark-history.jsonl 的 gcInfo → `memory.totalAllocatedBytes`、`gc.meanGcPauseNs`、`gc.gcSlowPathRatio` |
| `nightly_build.py`, `nightly_runner/config.py` | `run_profile` 默认从 False → True；新增 `--no-profile` 标志 |
| `nightly_delta.py` | 新增 `memoryAllocDelta`、`gcPauseDelta`、`profileSizeDelta` |
| `nightly_summary.py` | 新增 Memory 列（alloc + gcPause） |

### 5. Hotupdate C — 解禁 benchmark + 性能冲击 + revert 完整性

**改动的文件**：
| 文件 | 改动 |
|------|------|
| `stages/hotupdate_chunk.py` | `benchmark_iterations` 从 0 → 5；新增 revert integrity per-method 检查；修复 DispatchDirectVoid crash |
| `nightly_delta.py` | 新增 `huBenchRegression`、`huRevertRate`、`huRevertRegression` |
| `nightly_summary.py` | HU 列从 ✅/❌ 改为详细统计 |

**Hotupdate benchmark 安全策略**：
```
分段执行（避免 DispatchDirectVoid crash）:
  Phase 1: run fact only (baseline) → 收集 passed/failed
  Phase 2: run benchmark baseline → 仅非 void 方法
  Phase 3: apply patch → run fact (patched) → run benchmark (patched)
  Phase 4: revert → run fact (revert) → 验证 revert 完整性
```

## 文件改动清单汇总

| 文件 | 操作 | 维度 | 行数 |
|------|------|------|------|
| `stages/coverage_audit.py` | 改 | Coverage | +20 |
| `stages/fact_chunk.py` | 改 | Fact | +60 |
| `stages/benchmark_chunk.py` | 改 | Benchmark | +40 |
| `stages/hotupdate_chunk.py` | 改 | Hotupdate | +80 |
| `stages/aggregate.py` | 改 | 全部 | +50 |
| `nightly_delta.py` | 改 | 全部 | +120 |
| `nightly_summary.py` | 改 | 全部 | +60 |
| `nightly_build.py` | 改 | Memory | +2 |
| `nightly_runner/config.py` | 改 | Memory | +2 |
| `verification/analysis/value_checker.py` | **新增** | Fact | +80 |
| `verification/analysis/trend_detector.py` | **新增** | Coverage/Benchmark/Memory | +100 |
| `verification/analysis/regression_grading.py` | **新增** | Benchmark | +60 |

## 三优先级权衡

| 维度 | P1 性能 | P2 架构 | P3 热更 |
|------|---------|---------|---------|
| Coverage | namespace 分解无运行时开销 | 独立新文件，不污染 stage | 不涉及 |
| Fact | 值验证仅在 fact stage 内同步执行 | 新增 checker 模块，职责分离 | 不涉及 |
| Benchmark | 校准方法 0→median 实际提升精度 | per-method 趋势抽象为 analysis 模块 | 不涉及 |
| Memory | profile 默认开启+2x 构建时间（`--no-profile` 可关闭） | GC 指标走已有 JSONL 数据通路 | 不涉及 |
| Hotupdate | benchmark 仅对非 void 方法执行 | integrity check 复用 fact 验证 | ✅ 直接提升 |

## 风险评估

| 风险 | 缓解 |
|------|------|
| Profile 默认开启后构建 2x | `--no-profile` 标志 + Hephaestus Cache (cache hit 时几乎免费) |
| Hotupdate benchmark crash 根因不定 | 分段执行策略：先 baseline-only benchmark，patch 和 revert 仅 fact |
| 浮点值比较误报 | `tolerance=0.01` 相对误差 + 可配置 |
| 7 日趋势的冷启动问题 | 不满 7 天时使用已有数据，无趋势时降级为 "insufficient_data" |

## blocking_questions
- [x] 5 个维度全部选 C 方案
- [x] 值验证用相对误差容忍度
- [x] Benchmark 回归分级阈值（soft=10%, hard=20%）
- [x] Profile 默认开启 + `--no-profile` 回退
- [x] Hotupdate benchmark 分段安全策略
