# Benchmark Design Brainstorm

## Phase
implemented — 所有 7 个问题已在 `writing-plans` 阶段完成实现并验证

## 问题边界

| 编号 | 影响 | 问题 | 实现状态 |
|------|------|------|---------|
| H1 | 高 | Matrix selection 无 logging/assertion | ✅ 完成 |
| H2 | 高 | O(N) native 全量构建 | ✅ 完成 |
| H3 | 高 | N+1 冗余运行 | ✅ 完成 |
| M1 | 中 | 无统计 QC | ✅ 完成 |
| M3 | 中 | Baseline 跨 commit 不一致 | ✅ 完成 |
| L1 | 低 | Exception 吞咽 | ✅ 完成 |
| L2 | 低 | Windows 锁文件竞态 | ✅ 完成 |

## 实现清单

### H1 — Matrix selection 添加决策日志 + 断言

**文件**: `build/toolchains/run/commands/benchmark.py`

**改动**:
- `_select_benchmark_matrix_id()`: 每个候选 matrix 打印 `[matrix-select]` 日志到 stderr（含评分、pipeline_id、runtime_profile）
- 选中最优 matrix 后记录到 stderr
- 末尾添加 `assert best_matrix_id is not None`
- 函数增加 docstring 说明 H1 设计目标

### H2 — Native 构建改为增量构建

**文件**: `build/toolchains/run/testing/verification_layout.py`, `build/toolchains/run/commands/benchmark.py`

**改动**:
- `verification_layout.py`: 新增 `benchmark_builds_root()` 和 `benchmark_build_path()` 路径解析函数
- `benchmark.py`: 新增 `_get_benchmark_build_dir()` — 创建/获取持久构建目录
- `benchmark.py`: 新增 `_clean_stale_benchmark_builds()` — TTL 7 天清理
- `benchmark.py`: `_run_native_benchmark_pipeline()` 和 `_run_pipeline_and_record()` 中设置 `CHAOS_IL2CPP_BENCHMARK_BUILD_ROOT` env var
- `benchmark.py`: 添加 `--clean-builds` CLI 标志

### H3 — N+1 冗余运行懒缓存跳过

**文件**: `build/toolchains/run/commands/benchmark.py`

**改动**:
- 新增模块级 `_benchmark_case_run_cache` dict
- `dispatch()` 中 per-case 循环添加缓存命中检查：cache_key = (subject_id, mode, stableId)
- 命中时打印日志并 `continue` 跳过

### M1 — 完整统计 QC

**文件**: `testing/foundation-dll/verification/stages/benchmark_chunk.py`

**改动**:
- `_run_entry_once()` — 抽取单次 entry.exe 执行逻辑
- `_mean()` / `_stddev()` — 统计计算辅助函数
- `_quartile()` / `_remove_outliers()` — IQR 离群值剔除（样本 < 15），±3σ（样本 ≥ 15）
- `_compute_per_method_stats()` — 多轮采样后计算均值/stddev/CV/离群计数
- `run_benchmark_chunk()` — 重写为 warmup(2) → sample(5) → stats 三阶段
- 输出: `perMethodStats` 数组（含 stddevMs, cv, sampleCount, outlierCount）
- `records.jsonl`: 扩展 M1 字段

**文件**: `testing/foundation-dll/verification/stages/aggregate.py`

**改动**:
- 汇总层新增 meanCv, maxCv, totalOutliers, warmupRounds, sampleRounds 字段
- aggregatePerf 新增 aggregateMeanCv, totalOutliers

### M3 — Baseline 跨 commit 一致性检查

**文件**: `build/toolchains/run/commands/benchmark.py`

**改动**:
- `dispatch()` init-baseline 分支: 提取 managed/native/interpreter 记录的 gitCommit
- 三者一致则 `comparison["gitCommitVerified"] = True`
- 不一致则输出 WARN 到 stderr，设 `gitCommitVerified = False`

### L1 — Exception 始终 re-raise

**文件**: `build/toolchains/run/commands/benchmark.py`

**改动**:
- `_run_subject_benchmark_pipeline()`: 移除 `try/except Exception as error: return {"error": str(error)}`
- 异常直接传播到上层调用者

### L2 — Windows 锁简化为追加写

**文件**: `build/toolchains/run/testing/benchmark_records.py`

**改动**:
- 移除 `import os`, `import time`, `import fcntl` 及相关 try/except
- 移除 `_append_windows()` 和 `_append_unix()`
- `append_record()` 简化为直接 `open(path, "a", encoding="utf-8")`

## 文件变更汇总

| 文件 | 改动 |
|------|------|
| `build/toolchains/run/commands/benchmark.py` | H1, H2, H3, M3, L1 |
| `build/toolchains/run/testing/benchmark_records.py` | L2 |
| `build/toolchains/run/testing/verification_layout.py` | H2 |
| `testing/foundation-dll/verification/stages/benchmark_chunk.py` | M1 |
| `testing/foundation-dll/verification/stages/aggregate.py` | M1 |
