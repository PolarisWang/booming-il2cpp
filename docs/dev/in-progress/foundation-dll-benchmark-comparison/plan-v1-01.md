# 实现计划：Benchmark 差异化分析 + .NET 8 基准对比

> **面向执行 Agent：** 使用 `dev-executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 新增 benchmark_report pipeline stage + benchmark_diff CLI，实现每 method 以 .NET 8 为基准的跨 technology 对比和跨日 diff。

**架构：** 纯报告层，不改变 codegen/dispatch/native 构建。4 个文件的改动可独立验证。

---

## 修改清单

| # | 文件 | 改动 | 规模 |
|---|------|------|------|
| 1 | `verification/stages/benchmark_report.py` | **新建** — JSONL 解析 + technology 对齐 + % vs NET8 计算 + history 输出 | ~280 行 |
| 2 | `verification/chunk_pipeline.py` | 注册 benchmark_report stage（在 managed_benchmark + benchmark 之后，aggregate 之前） | ~10 行 |
| 3 | `verification/stages/aggregate.py` | 读取 comparison-summary.json 扩展 dashboard.json | ~40 行 |
| 4 | `verification/benchmark_diff.py` | **新建** — CLI 跨日 diff 工具 | ~180 行 |

---

## Task 1：新建 benchmark_report.py

**文件**: `testing/foundation-dll/verification/stages/benchmark_report.py`

**职责**: 读取 `benchmark-history.jsonl`，逐 methodSubjectId 对齐 4 个 technology，计算 % vs .NET 8，输出 comparison 数据。

### 输入
- `benchmark-history.jsonl`（路径：`results/foundation-dll/{assembly}/{slug}/perf/benchmark-history.jsonl`）
- 每行含 `technology`, `methodSubjectId`, `methodIndex`, `metrics.elapsedMilliseconds`, `status`

### 输出
1. **per-chunk**: `{chunk_dir}/results/comparison.json`
2. **aggregate**: `_dll/reports/latest/comparison-summary.json`
3. **history**: `_dll/reports/history/comparison-YYYY-MM-DD.json`

### 核心逻辑

```
def run_benchmark_report(ctx, stages) -> StageResult:
    for each chunk:
        read benchmark-history.jsonl
        group by methodSubjectId
        for each method:
            extract net8_ms, net10_ms, chaosAot_ms, chaosJit_ms
            if net8_ms exists:
                net10VsNet8Pct    = (net8_ms - net10_ms) / net8_ms * 100   (or null)
                chaosAotVsNet8Pct = (net8_ms - aot_ms)  / net8_ms * 100
                chaosJitVsNet8Pct = (net8_ms - jit_ms)  / net8_ms * 100   (or null)
            else:
                mark status="missing_net8"
        compute aggregate stats (mean/median/min/max per technology)
        write comparison.json
    write comparison-summary.json (aggregate across all chunks)
    write history/comparison-YYYY-MM-DD.json
```

### 错误处理

| 情况 | 处理 |
|------|------|
| JSONL 不存在 | 标记 `status: "no_data"`，跳过该 chunk |
| net8-jit 缺失 | 该 method `status: "missing_net8"`，% 字段为 null |
| chaos-jit 缺失 | `chaosJitMs = null`, `chaosJitVsNet8Pct = null` |
| net8 CV > 20% | 标记 `highVariance: true`，aggregate 排除 |
| net8 threw=true | 标记 `net8Error: true`，% 为 null，aggregate 排除 |

### aggregate 统计维度

per chunk:
- `methodsWithNet8` — 有 net8 数据的 method 数
- `methodsWithAllTechs` — 4 个 technology 都有的 method 数
- `chaosAotVsNet8Pct` — mean, median, min, max, p25, p75
- `chaosJitVsNet8Pct` — mean, median, min, max (or null)
- `net10VsNet8Pct` — mean, median, min, max
- `highValueMethods_betterThanNet8` — chaosAot 比 net8 快的 method 占比

### 输出 comparison-summary.json 格式

```json
{
  "assemblyName": "System.Private.CoreLib",
  "timestamp": "2026-06-04T15:34:42",
  "totalMethods": 21759,
  "methodsWithNet8": 21000,
  "methodsWithAllTechs": 20500,
  "perChunk": [
    {
      "slug": "system",
      "methodCount": 99,
      "methodsWithNet8": 99,
      "aggregate": {
        "chaosAotVsNet8Pct": { "mean": 85.2, "median": 87.1, "min": 12.3, "max": 99.8, "p25": 80.0, "p75": 95.0 },
        "chaosJitVsNet8Pct": { "mean": 82.0, "median": 84.0, "min": 10.0, "max": 99.0 },
        "net10VsNet8Pct":    { "mean": 15.3, "median": 12.7, "min": -5.0, "max": 40.0 },
        "highValueMethods_betterThanNet8": 98.5
      },
      "methods": [
        {
          "methodSubjectId": "...",
          "net8Ms": 0.0350, "net10Ms": 0.0280,
          "chaosAotMs": 0.0042, "chaosJitMs": 0.0051,
          "net10VsNet8Pct": 20.0, "chaosAotVsNet8Pct": 88.0, "chaosJitVsNet8Pct": 85.4,
          "highVariance": false, "net8Error": false
        }
      ]
    }
  ]
}
```

### 文件结构

```python
from __future__ import annotations
import json, time, math
from datetime import date
from pathlib import Path
from collections import defaultdict
from verification.orchestration.context import ChunkContext, StageResult

def _align_technologies(records: list[dict]) -> list[dict]: ...
def _compute_pct(net8_ms, tech_ms) -> float | None: ...
def _compute_aggregate_stats(values: list[float]) -> dict: ...
def run_benchmark_report(ctx: ChunkContext, stages: dict[str, StageResult]) -> StageResult: ...
```

---

## Task 2：注册 pipeline stage

**文件**: `testing/foundation-dll/verification/chunk_pipeline.py`

### 修改

1. 在 `STAGE_ORDER` 中 `managed_benchmark` 和 `benchmark` 之后、`aggregate` 之前插入 `"benchmark_report"`:

```python
STAGE_ORDER = [
    "build",
    "codegen",
    "managed_benchmark",
    "benchmark",
    "benchmark_report",    # ← 新增
    "hotupdate",
    "aggregate",
]
```

2. 在 `_STAGE_IMPORT_MAP` 和 `_STAGE_RUN_MAP` 中注册：

```python
"benchmark_report": "verification.stages.benchmark_report",
```

```python
"benchmark_report": run_benchmark_report,
```

3. 确保 `benchmark_report` 的 `allowed_dependencies` 包含 `["managed_benchmark", "benchmark"]`（或直接用 `None` = 不限制）。

---

## Task 3：扩展 aggregate.py

**文件**: `testing/foundation-dll/verification/stages/aggregate.py`

在 dashboard 输出段（line ~237 附近），读取 `comparison-summary.json`，合并到 `dashboard["summary"]`：

```python
comparison_path = latest_dir / "comparison-summary.json"
if comparison_path.exists():
    cs = json.loads(comparison_path.read_text(encoding="utf-8"))
    dashboard["summary"]["benchmarkComparison"] = {
        "methodsAnalyzed": cs.get("methodsWithAllTechs", 0),
        "chaosAotVsNet8": cs.get("aggregate", {}).get("chaosAotVsNet8Pct", {}),
        "net10VsNet8": cs.get("aggregate", {}).get("net10VsNet8Pct", {}),
        "highValueMethods_betterThanNet8": cs.get("aggregate", {}).get("highValueMethods_betterThanNet8", 0),
    }
```

注意：`comparison-summary.json` 的 aggregate 是跨所有 chunk 的，目前结构是 `{"perChunk": [...], "aggregate": {...}}`。benchmark_report 需要在 `comparison-summary.json` 顶层的 `"aggregate"` 中写入跨 chunk 汇总值。

---

## Task 4：新建 benchmark_diff.py

**文件**: `testing/foundation-dll/verification/benchmark_diff.py`

**职责**: CLI 工具，读取两个日期的 `comparison-YYYY-MM-DD.json`，逐 method 对比 `chaosAotVsNet8Pct`（和可选的 `chaosJitVsNet8Pct`, `net10VsNet8Pct`），按 10pp threshold 分类。

### CLI

```bash
python -m verification.benchmark_diff \
  --baseline _dll/reports/history/comparison-2026-06-03.json \
  --target  _dll/reports/history/comparison-2026-06-04.json \
  --threshold 10 \
  --output regression-report.json
```

### 核心逻辑

```
read baseline JSON, target JSON
build dict: methodSubjectId → chaosAotVsNet8Pct (baseline)
build dict: methodSubjectId → chaosAotVsNet8Pct (target)

for each method in target:
    baseline_pct = baseline.get(methodSubjectId)
    if baseline_pct is None: → "new"
    delta = target_pct - baseline_pct
    if delta >= threshold:  → "improved"
    if delta <= -threshold: → "regressed"
    else:                   → "unchanged"

for each method in baseline but not in target: → "missing"
```

### 输出格式（regression-report.json）

```json
{
  "baselineDate": "2026-06-03",
  "targetDate": "2026-06-04",
  "threshold": 10,
  "summary": {
    "total": 21000,
    "regressed": 3,
    "improved": 12,
    "unchanged": 20980,
    "new": 5,
    "missing": 0
  },
  "details": {
    "regressed": [
      { "methodSubjectId": "...", "baselinePct": 85.2, "targetPct": 72.1, "delta": -13.1 }
    ],
    "improved": [...],
    "unchanged_count": 20980
  }
}
```

### stdout 输出示例

```
=== Benchmark Regression Report ===
Baseline: 2026-06-03  Target: 2026-06-04  Threshold: 10pp

Regressed (3):
  system/Activator::CreateInstance   85.2% → 72.1%  (-13.1pp)
  text/String::Format                91.5% → 78.3%  (-13.2pp)
  io/File::ReadAllText               88.0% → 73.0%  (-15.0pp)

Improved (12):
  ...

Summary: 3 regressed, 12 improved, 20980 unchanged, 5 new
```

---

## 执行顺序

```
Task 1 (新建 benchmark_report.py) ──┐
                                     ├──> 并行/独立
Task 4 (新建 benchmark_diff.py) ────┘
                                     │
                                     v
Task 2 (chunk_pipeline.py 注册) ────> Task 3 (aggregate.py 扩展)
                                     │
                                     v
                             验证：全量 pipeline 跑 benchmark 阶段
                             确认 comparison-summary.json 输出正确
                             验证 benchmark_diff 跨日 diff
```

Task 1 和 Task 4 完全独立，可并行。Task 2 需要在 Task 1 之后（依赖 benchmark_report 的函数名）。Task 3 需要在 Task 1 之后（依赖 comparison-summary.json 格式）。

## 验证

1. **单元级验证**（手动检查输出）:
   ```bash
   # 跑 managed_benchmark + benchmark + benchmark_report
   python -m verification.chunk_pipeline --chunk system --stages managed_benchmark,benchmark,benchmark_report
   # 检查结果
   python -c "import json; d=json.load(open('System.Private.CoreLib/_dll/reports/latest/comparison-summary.json')); print(d['perChunk'][0]['aggregate']['chaosAotVsNet8Pct'])"
   ```

2. **Dashboard 验证**:
   ```bash
   python -c "import json; d=json.load(open('System.Private.CoreLib/_dll/reports/latest/dashboard.json')); print(d['summary'].get('benchmarkComparison', 'NOT FOUND'))"
   ```

3. **benchmark_diff 验证**:
   ```bash
   python -m verification.benchmark_diff \
     --baseline System.Private.CoreLib/_dll/reports/history/comparison-2026-06-03.json \
     --target  System.Private.CoreLib/_dll/reports/history/comparison-2026-06-04.json \
     --threshold 10
   ```

4. **全量回归验证**:
   ```bash
   python -m verification.chunk_pipeline --all-chunks --stages benchmark
   ```
