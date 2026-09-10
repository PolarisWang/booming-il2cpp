# Nightly-Driven Development — 设计 v1

> 任务目录：`docs/dev/in-progress/nightly-driven-dev/`
> 阶段：`brainstorming → design(cleared) → 执行中`
> 日期：2026-09-10

## 一句话

把 nightly build report 从"收尾验证工具"升级为**开发驱动力**：每轮 nightly 后
自动对比历史基线，产出结构化 triage 报告（退化项 + 建议动作），驱动
AOT 翻译管线、JIT 性能管线、hotupdate 管线的功能开发与优化。

## 边界拍板

### 做什么
1. 每轮 nightly 结束后**持久化基线**（per-chunk 聚合指标 + provenance）
2. 与**历史滚动窗口**（最近 5 轮）对比，按 **provenance 兼容性**过滤不可比数据
3. 用**统计显著性检验**（MAD/σ + 噪音地板）区分「确认退化」与「疑似噪音」
4. 三域独立判据：**fact / benchmark / hotupdate**（+ cross-tech 一致性）
5. 产出分级 triage 报告：🔴 Confirmed / 🟡 Watch / 📋 Actions / ⏭️ Skipped
6. **回填上轮 action 的验证结果**（闭环）

### 不做什么（非目标）
- ❌ 不做自动派发 agent —— 半自动，由用户确认后才派发
- ❌ 不改 codegen / runtime 引擎
- ❌ 不做实时监控（只做 nightly 粒度的批处理）
- ❌ 不引入外部时序数据库（纯 JSON 文件）

## authority 决策

| 决策点 | 结论 | 理由 |
|--------|------|------|
| 基线存储位置 | `tests/e2e/nightly-build-report/baseline/`（git 跟踪） | 跨机器/CI runner 共享基线 |
| triage 报告位置 | `tests/e2e/nightly-build-report/triage/`（不跟踪） | 每日产物，非源码 |
| 工具落位 | `verification/tools/`（非 `nightly/`） | 避免 `tools → nightly` 反向依赖；与 `honest_report.py` 同级 |
| 基线挂载点 | `nightly/aggregate.py` 末尾追加调用 | aggregate 已在 nightly 收尾路径上 |
| 对比口径 | 历史滚动窗口（最近 5 轮） | 用户拍板 A |
| 触发比例 | ≥30% | 用户拍板 A |
| 执行模式 | 半自动（报告产出 action，用户确认后派发） | 用户拍板 C |

## 三优先级权衡结论

| 维度 | 评分 | 依据 |
|------|:----:|------|
| P1 性能最优 | 🟢 | triage 是离线脚本（秒级），**不进 CI 关键路径**；baseline 写入 ~1ms/chunk |
| P2 架构完美 | 🟢 | `tools/` 独立工具，依赖方向正确；复用 `records.jsonl` 现成 provenance，零重复 |
| P3 热更适配 | 🟢 | hotupdate 为一等公民（`patchFailed`/`semanticChanged` 专用判据），不侵入热更新管线 |

低优先级让位点：无 —— 三个维度无冲突。

## 风险评估摘要

| 风险 | 触发条件 | 后果 | 缓解 |
|------|---------|------|------|
| 基线库膨胀 | 每轮 82 chunk × 10 轮 | git 变大 | 只存聚合指标 ~100KB/轮；per-method 只入 triage（不跟踪） |
| 首次基线缺失 | 第 1 轮无历史 | 无法对比 | 第 1 轮只建基线不产 action；第 2 轮起生效 |
| 统计法误判 | σ 受离群值污染 | 假阴性/假阳性 | 用 MAD（中位数绝对偏差）替代 σ；离群值先剔除 |
| 噪音带随 commit 漂移 | 代码改动改变性能特征 | 旧基线失效 | `--reset-baseline` 逃生舱；`reference` 轨定期重打 |
| 报告无人看 | 半自动依赖用户确认 | 报告堆积 | 报告顶部只放 🔴 Confirmed，其余折叠 |
| benchmark 噪音 | 实测仅 9.5% 方法 CV<0.05 | 单轮对比必误报 | 噪音地板（<0.05ms 排除）+ 滚动窗口 + MAD |

## 关键数据事实（探查所得，作为设计依据）

1. `records.jsonl` **已含** `gitCommit` / `device.id` / `nativeConfig` / `platform` / `technology` → provenance 现成
2. benchmark CV 分布（实测 CoreLib/text 801 方法）：
   - CV<0.05：76 (9.5%) 稳定
   - 0.05≤CV<0.15：370
   - 0.15≤CV<0.30：335
   - CV≥0.30：20 噪音
   → **单轮 p50 对比必然大量误报**，必须统计过滤
3. `fact-results.json` 含 `aot` + `jit` 两套结果 → cross-tech 维度可直接用
4. `provenance.json` 每 chunk 已写 `gitCommit`/`deviceId`/`nativeConfig`/`cacheKey`

## 组件契约

### `tools/baseline_store.py`

```python
record_from_run(config, results) -> Path
    """一轮 nightly 结束后，抽取 per-chunk 聚合指标 + provenance，追加写基线。"""

load_recent(n: int = 5) -> list[Baseline]
    """加载最近 n 轮基线（滚动窗口）。"""

load_reference() -> Baseline | None
    """加载人工标记的金标准轮次。"""

mark_reference(run_id) -> None
    """把某轮标记为 reference（金标准）。"""

is_comparable(cur, base) -> tuple[bool, str]
    """provenance 兼容性判定（platform/device/nativeConfig）。"""
```

### `tools/nightly_triage.py`

```python
analyze(cur_run, history, reference) -> TriageResult
    """三域 delta + 统计检验 + 比例触发 → 结构化结果。"""

render_markdown(result) -> str
    """渲染分级报告（🔴/🟡/📋/⏭️ + 闭环回填）。"""

main()  # python -m verification.tools.nightly_triage [--against latest]
```

### 判据表

| 域 | 指标 | 🟡 Watch | 🔴 Regression | 比例触发 |
|----|------|---------|--------------|---------|
| fact | `realVerified/total` | Δ≥5% 在噪音带内 | Δ≥5% 且超 μ+k·MAD | ≥30% chunk → 翻译管线开发 |
| fact | `smokeUnknown` | ↑≥10% | ↑≥10% 且显著 | 同上 |
| bench | `meanDurationMs` (per-method) | Δ≥30% 噪音带内 | Δ≥30% 且超带 | ≥30% 方法 → **方案重整** |
| bench | 噪音地板 | 排除 `<0.05ms` | — | — |
| hu | `semanticChangedCount` | 异常变化 | `patchFailed`>30% | ≥30% chunk → 管线修复 |
| cross-tech | AOT vs JIT 一致 | 新增 diff | diff>30% | 翻译一致性修复 |

## 数据 schema

### `baseline/run-<run_id>.json`

```json
{
  "schemaVersion": 1,
  "runId": "20260910_120000-abc123",
  "timestamp": "2026-09-10T12:00:00Z",
  "provenance": {
    "gitCommit": "...", "gitBranch": "main",
    "platform": "windows-x64", "deviceId": "...",
    "nativeConfig": "check"
  },
  "chunks": {
    "System.Private.CoreLib/system": {
      "fact": {"total": 5384, "realVerified": 1460, "unassertable": 0,
               "smokeUnknown": 2319, "failed": 0},
      "benchmark": {"methodCount": 100, "meanDurationMs": 12.5,
                    "p95DurationMs": 45.2, "stubCount": 5,
                    "perMethod": {"<subjectId>": {"meanMs": 1.2, "cv": 0.08}}},
      "hotupdate": {"semanticChanged": 30, "activePatch": 34, "revertPassed": 32,
                    "patchFailed": false},
      "crosstech": {"aotPassed": 100, "jitPassed": 98, "diffCount": 2}
    }
  }
}
```

### `baseline/index.json`

```json
{
  "schemaVersion": 1,
  "runs": [
    {"runId": "...", "timestamp": "...", "gitCommit": "...", "reference": false}
  ],
  "reference": "20260910_120000-abc123"
}
```

## 文件影响清单

### 🆕 新增（3）
| 路径 | 职责 |
|------|------|
| `tests/e2e/verification/tools/baseline_store.py` | 基线读写 + provenance + 滚动窗口 + reference |
| `tests/e2e/verification/tools/nightly_triage.py` | 主分析器 + 报告渲染 |
| `tests/e2e/verification/tests/test_nightly_triage.py` | 单元测试 |

### ✏️ 修改（2）
| 路径 | 改动 |
|------|------|
| `tests/e2e/verification/nightly/aggregate.py` | 末尾追加 `baseline_store.record_from_run()` 调用 |
| `.gitignore` | 忽略 `nightly-build-report/triage/`，保留 `baseline/` |

### 📄 文档（2）
| 路径 | 内容 |
|------|------|
| `docs/dev/in-progress/nightly-driven-dev/STATUS.md` | 本文件 |
| `docs/dev/in-progress/nightly-driven-dev/design-v1-01.md` | 完整设计 |

## 阶段切分（roadmap）

| 阶段 | 内容 | 独立验证 |
|------|------|---------|
| **P1** | `baseline_store.py` + aggregate 挂钩 + .gitignore | 跑 1 轮 nightly 确认基线写入 |
| **P2** | `nightly_triage.py` 三域 delta + provenance 过滤 + 统计检验 | 用 2 轮数据验证 delta 正确性 |
| **P3** | triage 报告分级渲染 + action items + 闭环回填 | 人工审查报告质量 |
| **P4** | MAD 优化 + 噪音地板 + reference 轨 | 长周期效果验证 |

## 问题清零

```
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## 下一步入口

`roadmap`（4 阶段）→ 或直接 `executing-plans`（若按 P1→P4 顺序串行推进）
