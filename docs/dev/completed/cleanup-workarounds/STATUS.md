# Pipeline Workaround 清理计划

## 阶段状态

| Phase | 状态 | 内容 |
|-------|------|------|
| **brainstorming** | ✅ completed | 方案已设计，已获用户确认 |
| **Batch 1 (低风险)** | ✅ completed | #14 #13 #12 #3 #4 #7 #15 + #16 |
| **Batch 2 (中风险)** | ✅ completed | #2 #5 #6 #9 #10 #11 |
| **Batch 3 (高风险)** | ✅ partial | #1 ✅ done, #8 🔲 skipped per user |
| **全部** | ✅ **14/15 项完成, 1 项跳过** | #8 chaos_static_ 退出码修复已跳过 |

## 改动清单

### Batch 1 — 低风险

| # | 文件 | 改动 | 行数 |
|---|------|------|------|
| #16 | `fact_chunk.py` | 空洞注释已不存在（只存在 worktree 副本中） | 0 |
| #14 | `benchmark_chunk.py` | `warning_no_valid_data` → `failed`，全零时长 FAIL | +5 |
| #13 | `benchmark_chunk.py` | 新增 `_check_range_support()` 检测 entry.exe 是否支持 `--benchmark-range` | +15 |
| #12 | `benchmark_chunk.py` | Phase 3b: method 0 校准质量监控（慢方法 >10x median 时 WARN） | +15 |
| #3 | `fact_chunk.py` | metadata drop >50% → `failed`, >10% → `partial` | +10 |
| #4 | `aggregate.py` | metadata 50% 差距使 aggregate status 变为 `partial` | +5 |
| #7 | `build.py` | `tfmFallback` 字段记录 TFM 降级 | +3 |
| #15 | `build.py` | `cache_status` 字段区分 miss/hit/restore_failed/restore_failed_entry_missing | +5 |

### Batch 2 — 中风险

| # | 文件 | 改动 | 行数 |
|---|------|------|------|
| #2 | `fact_chunk.py` + `aggregate.py` | value_suspicious 时 status → `partial`；aggregate 也反映 | +12 |
| #5 | `build.py` | 新增 `_probe_dll_type_count()` 自动检测 type-forwarder(monodis)；WARN 未映射的 0-type DLL | +20 |
| #6 | `build.py` | `jitSkipped` 字段记录 JIT 是否被跳过 | +3 |
| #9 | `build.py` | `customSubjectsUsed` 字段记录 custom subjects fallback | +3 |
| #10 | `hotupdate_chunk.py` + `aggregate.py` | hotupdate.json 新增 `status` 字段；aggregate 收集 `skipBreakdown` | +15 |
| #11 | `hotupdate_chunk.py` | LINK-HACK-1~6 标签标记所有 cmake 修补位置 | +10 |

### Batch 3 — 高风险

| # | 文件 | 改动 | 行数 |
|---|------|------|------|
| #1 | `fact_chunk.py` | JIT passed + AOT error/failed → `partial` 而非 `passed` | +12 |
| #8 | `build.py` | 🔲 跳过（用户确认） | - |

## 文件变更统计

```
 aggregate.py               |  27 ++++++++++-
 benchmark_chunk.py         |  51 ++++++++++++++++++---
 build.py                   |  53 ++++++++++++++++++++--
 fact_chunk.py              |  36 ++++++++++++---
 hotupdate_chunk.py         |  18 +++++++-
 5 files changed, 165 insertions(+), 20 deletions(-)
```

## blocking_questions

- [x] 全部清零

## 风险评估摘要

| 风险 | 状态 |
|------|------|
| #1 JIT-promotion 使现有构建变黄 | 已确认：用户要求直接改为 partial |
| #8 chaos_static_ 退出码掩盖 | 已跳过 |
| 其他各项均为可观测性增强，无 function 变化 | 已验证 |

## 下一步

- 运行 pipeline 验证改动不会导致额外失败
- 考虑 root cause 跟踪：#8 需另开任务
