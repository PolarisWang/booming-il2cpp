# Pipeline Workaround 清理 — 设计 v1

> 清理 `testing/foundation-dll/verification/stages/` 下 14 项仍活跃的 workaround
> 分批从低风险到高风险递进

---

## 架构概要

### 影响范围

```
testing/foundation-dll/verification/stages/
├── fact_chunk.py         — 批次 1 (3项: #3 #16注释) + 批次 3 (#1)
├── aggregate.py          — 批次 1 (1项: #4)
├── build.py              — 批次 1 (2项: #7 #15) + 批次 2 (#5 #9) + 批次 3 (#8)
├── benchmark_chunk.py    — 批次 1 (3项: #12 #13 #14)
├── hotupdate_chunk.py    — 批次 2 (2项: #10 #11)
└── context.py            — 批次 2 (1项: #2)
```

### 设计原则

1. **告警分层**：所有检测点分层为 three-tier — `INFO`(只记日志) → `WARN`(记日志+报告标记) → `FAIL`(阻断 stage)
2. **默认 fail-safe**：数据异常默认 WARN 而非 ignore，但只对**严重级别**才 FAIL
3. **不引入新配置**：所有升级逻辑 hardcode 在 Python stage 中

---

## 批次 1：低风险（7 + 1 注释项）

### #14 — Benchmark 质量告警不 fail

**文件**: `benchmark_chunk.py:L633-641`

**当前**：
```python
if zero_duration == method_count:
    status = "warning_no_valid_data"    # ← 不 fail
```

**改造**：
```python
# WARN: 部分零值 → warning_mostly_zero（仍 pass）
# FAIL: 全零时长，数据完全不可用 → "failed"而非"warning_no_valid_data"
if zero_duration == method_count:
    status = "failed"                     # ← UPGRADE: 全零→fail
    errors.append("all methods returned zero/negative duration")
elif zero_duration > method_count * 0.5:
    status = "warning_mostly_zero"        # ← 保持 WARN
    print(f"WARNING: {zero_duration}/{method_count} zero duration")
```

**影响**: 极少触发（目前无全零场景），不会把现有绿构建变红。

---

### #13 — Benchmark range 永久 disabled

**文件**: `benchmark_chunk.py:L428-435`

**当前**：
```python
use_range = False   # 硬编码 False
```

**分析**：根因是 `entry.exe`（旧 TPG 模板）不支持 `--benchmark-range`。该 workaround 无法在 Python 层消除——需要底层 TPG 模板支持后才能启用。

**改造**：
- 将 `use_range = False` 替换为**显式能力检查**：检测 entry.exe 是否支持 `--benchmark-range`（试运行 + 检查退出码）
- 移除空洞注释，改为清晰的状态打印

```python
# Check if entry.exe supports --benchmark-range
# (older TPG templates do not implement this flag)
use_range = _check_range_support(exe_path)
```
其中 `_check_range_support` 尝试 `--help` 输出中是否含 `benchmark-range`。

**影响**: 如果当前 entry.exe 已更新支持，自动启用 range 模式；否则行为不变。

---

### #12 — Benchmark 校准只跑 method 0

**文件**: `benchmark_chunk.py:L166-200`

**当前**：
```python
result = _run_entry_once(exe_path, 10, ..., start_idx=0, end_idx=1)
```

**分析**：`--benchmark-all` 在调用全量 method 时会崩溃——根因在 entry.exe 内部。Python 层只能检测崩溃后自动 fallback。

**改造**：
- 改成分**两阶段**：先尝试验证 method 0 校准参数对 method N 是否合理
- 增加**校准质量监控**：在实际 benchmark 运行后，检查 method 0 的校准迭代次数对其他方法的实际耗时是否在合理范围（±3σ），如果偏差过大则 WARN

```python
# 校准阶段：method 0
calib_iter = _calibrate_from_method_0(...)
# 验证阶段：检查实际运行时长分布
duration_stats = _compute_duration_stats(per_method_stats)
if duration_stats["max"] > duration_stats["median"] * 10:
    print(f"WARNING: calibration from method 0 under-estimated for slow methods")
```

**影响**: 新增质量监控，不会改变 pipeline 行为。

---

### #7 — TFM net8.0 fallback

**文件**: `build.py:L599-639`

**当前**：构建失败静默降级到 net8.0

**改造**：
- 降级后改为 **WARN 但不 fail**（当前已经是这样的行为，只是没有明确标记）
- 在 release notes / dashboard 中标记 `tfmFallback: true`
- 不改变 status（仍然是重试，重试失败才 fail）

```python
if tfm != "net8.0":
    print(f"[build] WARNING: {tfm} build failed, falling back to net8.0")
    # ... retry ...
    if retry_success:
        # Mark in details but keep status = passed
        details["tfmFallback"] = True
```

**影响**: 无行为变更，只在 details 中增加标记字段。

---

### #4 — metadata 差距 20%/50% 仅 WARN

**文件**: `aggregate.py:L141-157`

**当前**：20% 差距 WARN，50% 严重差距只计数不 fail

**改造**：
- 50% 以上差距（`ratio < 0.5`）→ 提升到 `status = "partial"`（非 fail，但不再 `passed`）
- 20% 差距保持 WARN

```python
if ratio < 0.5:
    chunks_with_meta_mismatch += 1
    # UPGRADE: 严重差距使 aggregate 不是全绿
    status = "partial"   # ← 新增：非 fail 但也不 passed
```

**影响**: 仅在差距 50%+ 时将 stage status 从 `passed` 改为 `partial`。无明显破坏性。

---

### #3 — metadata drop 只记 error 不 fail

**文件**: `fact_chunk.py:L192-197`

**当前**：dropped > 0 只追加到 errors list，不影响 status

**改造**：
- dropped 方法数占 meta_total 比例 > 10% → `status = "partial"`
- dropped > 50% → `status = "failed"`（严重丢失）
- 阈值可调

```python
if aot_dropped > 0 and meta_total and meta_total > 0:
    drop_ratio = aot_dropped / meta_total
    if drop_ratio > 0.5:
        status = "failed"       # 严重丢失
    elif drop_ratio > 0.1:
        status = "partial"      # 轻度丢失
```

**影响**: 轻度丢失很罕见（当前没有 chunk 触发该阈值），不会打红现有构建。

---

### #15 — Hephaestus cache fallback

**文件**: `build.py:L695-699`

**当前**：cache 失败静默回退到完整构建

**分析**：这是正确的降级行为——cache 失败不应该阻断构建。问题在于**没有监控**cache 命中率和失败率。

**改造**：
- 不需要改 fail 逻辑（cache 降级是正确的）
- 在 `details` 中写入 `"cacheStatus": "miss"`/`"hit"`/`"restore_failed"`
- 在 pipeline 汇总中记录 cache 健康状况

```python
details["cacheStatus"] = "restore_failed"  # 而非 silence
```

**影响**: 零风险，只增加可观测性。

---

### #16 — 空洞注释清理

**文件**: `fact_chunk.py`

**当前**：注释中引用了不存在的 `fix_all_failures.py`

**修复**：删除/更新该注释引用。

---

## 批次 2：中风险（6 项）

### #2 — value_suspicious 只记录不处理

**文件**: `fact_chunk.py:L205-210`, `aggregate.py:L137-139`, `context.py:L29`

**当前**：negative value 只记录 `value_suspicious=True`，aggregate 只计数，不 fail

**改造**：
- 在 `fact_chunk.py` 中：当 `value_suspicious=True` 时，将 status 降为 `partial`（非 fail）
- 在 `aggregate.py` 中：当 `chunks_with_value_warnings > 0` 时，在 dashboard 中标记 `valueWarning: true`

```python
# fact_chunk.py
if value_suspicious:
    if status == "passed":
        status = "partial"  # 有 value 异常，不能算全绿
```

**影响**: 当前有多少 chunk 会触发？如果大量 chunk 都有 negative value，会使 dashboard 出现大量 partial。需要确认后实施。

---

### #5 — Type-forwarder 静默重定向

**文件**: `build.py:L346-371`

**当前**：3 个已知 type-forwarder 硬编码映射

**改造**：
- 改为**自动检测**：扫描 DLL 的 TypeForwardedTo 属性（通过 `System.Reflection.Metadata` 或 Monodis）
- 如果不可自动检测，保留映射但增加 WARN：`"Type-forwarder: {asm} → {real}"`

```python
# 自动检测
forwarded_types = _detect_type_forwarders(target_dll)
if forwarded_types:
    print(f"[build] Type-forwarded assembly: {ctx.assembly} → {forwarded_types}")
    real_asm = forwarded_types[0].assembly
    # ...
```

**影响**: 对已知 type-forwarder 无行为变化。新增未映射的 type-forwarder 时，自动检测不会失败。风险低。

---

### #6 — JIT 构建跳过大 chunk

**文件**: `build.py:L256-267`

**当前**：500MB 阈值静默跳过，无验证覆盖

**改造**：
- 打印更清晰的跳过原因（含 chunk 名字和方法数）
- 在 `details` 记录 `jitSkipped: true, jitSkipReason: "codegen_output_too_large"`
- 在 aggregate 级别统计被跳过的 JIT chunk 数量

```python
details["jitSkipped"] = True
details["jitSkipReason"] = f"codegen {codegen_cpp_total//(1024*1024)}MB exceeds 500MB threshold"
```

**影响**: 零风险，只增加可观测性。根因修复（减小 JIT image size）是 runtime/codegen 层的独立任务。

---

### #9 — Custom subjects fallback

**文件**: `build.py:L456-503`

**当前**：ATG 产出 0 subjects 时静默降级到手写 .cs

**改造**：
- 降级时在 `details` 中写入 `"autoSubjectsGenerated": false, "customSubjectsUsed": true`
- 在 aggregate dashboard 中标记该 chunk 无 ATG 覆盖
- 不改变 status（降级到 custom subjects 是正常行为）

```python
details["autoSubjectsGenerated"] = False
details["customSubjectsUsed"] = True
```

**影响**: 零风险，只增加可观测性。

---

### #10 — HotUpdate 多种 skip status

**文件**: `hotupdate_chunk.py:L614-636`

**当前**：7+ 种非失败状态，`aggregate.py` 只聚合 `patchFailed` 和 `revertFailure`

**改造**：
- 在 `aggregate.py` 的 summary 中增加 `skippedCounts` 细分
- 区分"预期跳过"（no subjects）和"异常跳过"（patch failed、json truncated）
- `aggregate` 的 dashboard 中标记异常跳过数量

```python
hotupdate_skipped_counts = {
    "no_subjects": ...,
    "no_patch": ...,
    "patch_failed": ...,
    "patch_not_applied": ...,
    "truncated": ...,
}
```

**影响**: 只改 aggregate 报告层，不改 hotupdate_chunk.py 的 status 判断。

---

### #11 — HotUpdate cmake hack 群

**文件**: `hotupdate_chunk.py:L122-227`

**当前**：6 种修补（删除 stub、assert stub、weak symbol、allow-multiple-definition 等）

**分析**：这些 hack 是必要的——它们是 hotupdate 路径 cmake 链接问题的真实 workaround。移除任何一个可能导致 hotupdate 构建失败。问题在于**没有集中标记**。

**改造**：
- 为每个 hack 增加 logging：`"[hotupdate] LINK-HACK: {reason}: {action}"`
- 在 `details` 中记录应用了哪些 hack

```python
hack_id = "remove-bridge-redirect-stubs"
print(f"  [hotupdate] LINK-HACK: Avoiding LNK2019 from stale bridge redirect stubs")
```

**影响**: 零风险，只增加可观测性。

---

## 批次 3：高风险（2 项）

### #1 — JIT-promotion 掩盖 AOT 失败

**文件**: `fact_chunk.py:L180-189`

**当前**：
```python
if jit_status == "passed":
    status = "passed"  # ← 即使 AOT 全挂
```

**改造**：
- 改为三态:
  - `"passed"` — AOT 通过或 JIT 通过且 AOT 不是 error（至少 partial）
  - `"partial"` — AOT 完全 error 但 JIT 通过
  - `"failed"` — 两者都 fail

```python
if jit_status == "passed":
    if aot_status == "error" or aot_status == "failed":
        status = "partial"    # JIT 兜底但 AOT 完全不可用
    else:
        status = "passed"     # JIT 通过 + AOT 至少 partial
```

**⚠ 风险**：当前部分 chunk 的 AOT 完全不可用，但 JIT 一直兜底。如果将 status 降为 `partial`，这些 chunk 会从绿变黄。但这是正确的行为——AOT 失败不应被掩盖。

**防护**：先改为 WARN-only（记录 AOT 失败但不降级），观察一轮后再开启 fail。

---

### #8 — chaos_static_ 改写退出码

**文件**: `build.py:L762-810`

**当前**：
```python
tpg_result.returncode = 0  # ← Override: treat as success
```

**改造**：
- 不再改写退出码。改为：
  - 将原始 TPG 错误保存在 `details["tpgOriginalError"]` 中
  - 如果 cmake retry 成功 → `status = "passed"` 但 `details["autoRepaired"] = True`
  - 如果 cmake retry 也失败 → `status = "error"`

```python
if cmake_result.returncode == 0:
    status = "passed"
    details["autoRepaired"] = True
    details["autoRepairCount"] = len(truly_missing)
    # 不再 tpg_result.returncode = 0
else:
    return StageResult(stage="build", status="error", ...)  # 原始流程
```

**⚠ 风险**：如果 chaos_static_ 自动修复实际修复了大量 chunk，那么不复写退出码虽然 status 仍为 passed，但改写退出码的原始设计就是为了避免下游 `tpg_result.returncode` 被误读。需要确认当前有多少 chunk 触发该路径。

---

## 实施顺序

```
批次 1-0: #16 注释清理 (1 文件, 1 行改动)
批次 1-1: #14 Benchmark 全零 fail  (benchmark_chunk.py, ~5 行)
批次 1-2: #13 Benchmark range 能力检测 (benchmark_chunk.py, ~15 行)
批次 1-3: #12 Benchmark 校准质量监控 (benchmark_chunk.py, ~15 行)
批次 1-4: #3  metadata drop 阈值判断 (fact_chunk.py, ~10 行)
批次 1-5: #4  metadata 50%差距 partial (aggregate.py, ~3 行)
批次 1-6: #7  TFM fallback 标记 (build.py, ~3 行)
批次 1-7: #15 cache fallback 标记 (build.py, ~2 行)
──────────────────────────────────────
批次 2-1: #2  value_suspicious partial  (fact_chunk.py + aggregate.py, ~5 行)
批次 2-2: #5  Type-forwarder 自动检测 (build.py, ~20 行)
批次 2-3: #6  JIT 跳过可观测性 (build.py, ~3 行)
批次 2-4: #9  Custom subjects 标记 (build.py, ~3 行)
批次 2-5: #10 HotUpdate skip 聚合 (aggregate.py, ~15 行)
批次 2-6: #11 cmake hack logging (hotupdate_chunk.py, ~10 行)
──────────────────────────────────────
批次 3-1: #1  JIT-promotion 降级 (fact_chunk.py, ~10 行)
批次 3-2: #8  chaos_static_ 退出码修复 (build.py, ~5 行)
```

**总计**: ~14 次独立修改，涉及 6 个文件。

---

## 待确认问题

- [ ] #2 value_suspicious：当前有多少 chunk 会触发？如果大量触发，先改为 partial 还是先修根因？
- [ ] #1 JIT-promotion：先 WARN 一轮再 fail，还是直接 fail？
- [ ] #8 chaos_static_：当前有多少 chunk 触发该路径？是否可能使大量构建变红？
