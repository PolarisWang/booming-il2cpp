# 修复计划：Hotupdate 验证管线第二阶段修复

> **面向执行 Agent：** 使用 `dev-executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 修复 A2（revert loop 包含基线已失败方法）和 B2（dashboard 缺失 hotupdate 字段）。

**架构：** 两层修改，各自独立，可并行实施。

## 修改清单

| # | 文件 | 改动 | 规模 |
|---|------|------|------|
| 1 | `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Templates/TestProject.RuntimeEntry.cpp.scriban` | Revert loop 增加 `baseline_ok[si]` 跳过保护 | ~5 行 |
| 2 | `testing/foundation-dll/verification/stages/aggregate.py` | Dashboard summary 增加 hotupdate 子段 | ~10 行 |

不修改：PatchDataExtractor、hotupdate_chunk.py、dispatch 路由、pass criterion。

---

## Task 1：Revert loop 跳过基线已失败方法

**文件**: `TestProject.RuntimeEntry.cpp.scriban`

**问题**: RunHotupdateMode 的 revert phase 对所有 kSubjectEntryCount 个 subject 执行 `ChaosDispatchMethod`，不检查 `baseline_ok[si]`。基线已 crash 的方法在 revert 阶段再次 crash → `all_revert=false` → chunk 被判定为失败。

**修改**: 在第 493 行附近的 `revertedFact` 输出循环中，对 `!baseline_ok[si]` 的方法输出 `{"passed":true,"skipped":true}` 并 `continue`。

**修改前**:
```cpp
printf("\"revertedFact\":[");
for (int si = 0; si < kCount; si++) {
    int i = kSubjectSlotMap[si];
    bool reverted_ok = false;
    CHAOS_EH_TRY
        chaos::il2cpp::runtime_core::ChaosDispatchMethod(...);
        reverted_ok = true;
    CHAOS_EH_CATCH_BEGIN
        all_revert = false;
    CHAOS_EH_END
```

**修改后**:
```cpp
printf("\"revertedFact\":[");
for (int si = 0; si < kCount; si++) {
    if (!baseline_ok[si]) {
        if (si > 0) printf(",");
        printf("{\"si\":%d,\"passed\":true,\"skipped\":true}", si);
        continue;
    }
    int i = kSubjectSlotMap[si];
    // ... 不变
```

---

## Task 2：Dashboard 增加 hotupdate 字段

**文件**: `testing/foundation-dll/verification/stages/aggregate.py`

**问题**: `dashboard.json` 的 `summary` 不含任何 hotupdate 相关指标。

**修改**: 遍历 `chunk_summaries`，收集 hotupdate 聚合指标，写入 `dashboard["summary"]["hotupdate"]`。

新增字段：

```python
dashboard["summary"]["hotupdate"] = {
    "chunksWithPatchData": chunks_with_patch_data,
    "chunksPatchFailed": chunks_with_patch_failed,
    "chunksWithRevertFailure": chunks_with_revert_failure,
    "totalHotupdatePassed": total_hu_passed,
    "totalHotupdateFailed": total_hu_failed,
}
```

---

## 验证

1. **全量 hotupdate 验证**:
   ```bash
   python -m verification.chunk_pipeline --all-chunks --stages hotupdate
   ```
   - system-2 和 collections-generic 应通过（不再因基线失败方法误判）

2. **Dashboard 检查**:
   ```bash
   python -c "import json; d=json.load(open('System.Private.CoreLib/_dll/reports/latest/dashboard.json')); print(d['summary']['hotupdate'])"
   ```
   - 应输出 6 个指标字段

---

## 执行顺序

```
Task 1 (Scriban revert skip) ──┬──> 并行修改
Task 2 (aggregate dashboard) ──┘
                                │
                                v
                         验证（全量 hotupdate + dashboard 检查）
```
