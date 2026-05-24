---
family: "{family_name}"
assembly: "{assembly_name}"
risk_tier: "P0/P1/P2/P3"
phase2_order: "{序号}"
status: "not_started / in_progress / passed / failed-blocking / failed-accepted"
---

# 验证 Checklist

## 1. 基本信息

| 字段 | 值 |
|------|-----|
| Family | {family_name} |
| Assembly | {assembly_name} |
| Risk Tier | P0/P1/P2/P3 |
| Phase 2 执行顺序 | {序号} |
| 基线扫描结果 | ✅ passed / ❌ failed |
| 基线失败 stage | {stage list} |
| 复杂度评分 | {score} |
| Method 数 | {N} |
| 执行日期 | {date} |

---

## 2. 13-Stage 执行记录

| # | Stage | 耗时 | 结果 | 备注 |
|:-:|-------|:----:|:----:|------|
| 0 | Preflight | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 1 | Codegen AOT | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 2 | JitCodegen | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 3 | Fact AOT | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 4 | Fact JIT | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 5 | Audit | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 6 | AsmCompare | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 7 | Microbench | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 8 | Benchmark | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 9 | HotUpdate AOT Fact | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 10 | HotUpdate AOT Bench | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 11 | HotUpdate JIT Fact | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 12 | HotUpdate JIT Bench | ___ms | ☐ passed ☐ failed ☐ skipped | |
| 13 | Aggregate | ___ms | ☐ passed ☐ failed ☐ skipped | |
| | **总计** | ___ms | **☐ passed ☐ failed** | |

### 2a. 未通过 stage 详情

> 只有结果不是 passed 时填写

| 失败的 stage | 实际状态 | 失败信息 |
|:-----------:|:--------:|---------|
| {stage name} | failed / error / skipped-unexpected | {summary 或 errors} |

---

## 3. Benchmark 基线（首次通过时记录）

> 首次 13-stage 全部通过时，记录 benchmark 快照作为基线。
> 后续每次修复重跑时，与本次基线做对比。

### 3.1 概览

| 指标 | 值 |
|------|:---:|
| totalMethods | {N} |
| matchedCount | {N} |
| averageSpeedupPercent | {±X.X%} |
| nativeAotFasterCount | {N} |
| managedFasterCount | {N} |
| equalCount | {N} |
| nativeJitSlowdownFactor | {X.X} |

### 3.2 多运行时分类（multi-run-report.json）

| 分类 | 数量 |
|------|:----:|
| chaos_faster_than_all | {N} |
| chaos_faster_than_jit | {N} |
| chaos_midfield | {N} |
| chaos_slower_than_jit | {N} |
| not_benchmarked | {N} |

### 3.3 拖后腿方法（speedup < -50%）

| Method | AOT speedup | JIT speedup | status |
|--------|:-----------:|:-----------:|:------:|
| {methodSubjectId} | -157.7% | -89.2% | completed |
| {methodSubjectId} | -65.3% | -45.1% | stub |

---

## 4. Triage 记录

> 仅当 13-stage 中有任何非 passed 时填写

### 4.1 通用三问

| # | 问题 | 回答 |
|:-:|------|------|
| 1 | 基础设施问题还是翻译问题？ | infra / translation |
| 2 | 已知问题还是新问题？ | known:{issue_id} / new |
| 3 | 单 family 还是系统性？ | single / systemic |

### 4.2 失败分类

| 分类 | 匹配 | 判断依据 |
|:----:|:----:|---------|
| A: Codegen 崩溃/超时 | ☐ | exit code / TimeoutExpired / Fatal error |
| B: C++ 编译错 | ☐ | MSVC error code + message |
| C: Fact AOT 运行时崩溃 | ☐ | exit code (0xC0000005/0409/00FD/80000003) |
| D: Fact JIT 失败 | ☐ | entry-jit.exe exit code |
| E: Audit VIOLATION | ☐ | principle-alignment-report.json |
| F: Microbench 崩溃 | ☐ | entry.exe --microbench crash |
| G: Benchmark 质量门 | ☐ | matchedCount < 50% / avgSpeedup < -400% |
| H: HotUpdate 失败 | ☐ | hotupdate-*-report.json |
| I: Pipeline 基础设施 | ☐ | Python 异常/脚本错误 |

### 4.3 根因定位

```
Stage 输出（关键错误片段）:
  {粘贴关键错误信息}

根因摘要:
  {一句话描述根因}

根因文件:
  {源码路径及行号}
```

### 4.4 系统性影响评估

| 问题 | 答案 | 操作 |
|------|------|------|
| 根因是否在 shared code 中？ | ☐ yes → 系统性 ☐ no → 单 family | |
| 如果是系统性，SYS-ID | SYS-{N} | 无 → 创建新 SYS |
| 受影响的其他 family 数 | {N} | 扫描 contract.json 确认 |
| 是否已标记到 tracker | ☐ yes ☐ no | 更新 verification-tracker.json |

---

## 5. 修复记录

> 每次修复操作记录一条

---

### Fix #{N}

**描述**: {fix description}

**修改文件**: {file path}

**关联 SYS**: SYS-{N} / N/A

**根因分类**: A/B/C/D/E/F/G/H/I

**Benchmark 变化**:

| 指标 | 修复前 | 修复后 | Δ |
|------|:------:|:------:|:-:|
| matchedCount | {N}/{total} | {N}/{total} | ±N |
| averageSpeedupPercent | {±X.X%} | {±X.X%} | ±X.Xpp |
| nativeAotFasterCount | {N} | {N} | ±N |
| managedFasterCount | {N} | {N} | ±N |
| chaos_faster_than_all | {N} | {N} | ±N |
| chaos_slower_than_jit | {N} | {N} | ±N |

**拖后腿方法变化**（speedup < -50% 的方法）：

| Method | 修复前 | 修复后 | Δ |
|--------|:------:|:------:|:-:|
| {methodSubjectId} | -157.7% | -42.1% | +115.6pp |
| {methodSubjectId} | -89.3% | -30.2% | +59.1pp |

**新增完成的方法**（status 从 stub/exception → completed）：

| Method | 修复前 status | 修复后 status |
|--------|:------------:|:------------:|
| {methodSubjectId} | stub | completed |
| {methodSubjectId} | exception | completed |

**验证结果**: ☐ ✅ passed ☐ ❌ failed → 回到 triage

---

### Fix #{N+1}

...

---

## 6. 系统性修复批量重跑记录

> 仅当本次修复是系统性（涉及 SYS-N）时填写

| SYS-ID | Affected Family | 修复前 avgSpeedup | 修复后 avgSpeedup | Δ | 结果 |
|:------:|----------------|:----------------:|:----------------:|:-:|:----:|
| SYS-{N} | {assembly}/{family} | {±X.X%} | {±X.X%} | ±X.Xpp | ☐ passed ☐ failed |
| SYS-{N} | {assembly}/{family} | {±X.X%} | {±X.X%} | ±X.Xpp | ☐ passed ☐ failed |

## 7. 回归验证记录

> 对已 passed 的 family 选代表性样本，验证修复没有引入 regression

| 验证 family | 选择理由 | 修复前 avgSpeedup | 修复后 avgSpeedup | Δ |
|------------|---------|:----------------:|:----------------:|:-:|
| {assembly}/{family} | {受该修复影响最大的已 passed family} | {±X.X%} | {±X.X%} | ±X.Xpp |
| {assembly}/{family} | {另一个受影响不同的 family} | {±X.X%} | {±X.X%} | ±X.Xpp |

---

## 8. 最终状态

| 字段 | 值 |
|------|-----|
| **最终结果** | ☐ ✅ PASSED ☐ ❌ FAILED (blocking) ☐ ⚠️ FAILED (accepted) |
| 失败 stage（如有） | {stage list} |
| 接受的已知限制 | {accepted limitations} |
| 关联 SYS-ID | SYS-{N}, SYS-{M} |
| 该 family 的修复次数 | {N} |
| Benchmark 基线最终版 avgSpeedup | {±X.X%} |
| Benchmark 基线最终版 matchedCount | {N}/{total} |
| 总执行耗时（含重跑） | {time} |
| 最后更新 | {date} HH:MM |
