# STATUS — GC-K2 RegionWriteBarrier

## 元信息

```yaml
task_id: gc-align-coreclr/gc-k2-region-writebarrier
task_type: design → impl  # 架构设计完成；K2a 已实现
phase: implementation
roadmap_or_plan: docs/dev/in-progress/gc-align-coreclr/gc-k2-region-writebarrier/design-k2-v1-01.md
parent_task_id: gc-align-coreclr
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
created: 2026-08-10
created_by: main-agent
```

## Scope

世代感知 region→generation 写屏障，忠实 CoreCLR 设计 + 跨平台 + AOT/JIT。设计文档已完成；实现按 K2a-e 分步。

## 实现进度

### K2a（region→gen 表）✅ `cbc1b6925`
- `Region` 加 `uint8_t gen`（低 2bit 代表）；`kRegionGenMask/Young/Old`；`kRegionGenShift=22`。
- 全局 `g_region_to_gen` skewed 表 + inline `GetRegionGen`/`SetRegionGen`。
- `EnsureRegionGenCoverage` 懒增长；`AllocateRegion` NURSERY→young(0)/其余→old(2)。

### K2c（codegen 发射双参屏障）✅ `4f59836d1`
- 4 个可取 ref 的发射点升级 `_dst_ref`；保留 cpobj 单参保守。
- `chaos_gc_dirty_card_dst_ref` 声明入 gc_helpers.h + gc_card_table.h。
- BOUNDARY_OVERRIDE: issues/GC-K2c。

### K2d（card bundle 2MB 粗卡）✅ `547f8864f`
- `g_card_bundle` bitset + `CardBundleBit/Test/Set/ClearAll`。
- `DirtyCard` 设卡顺带着色 bundle；`ScanDirtyCards` segment 循环先检 bundle 整段 skip（对齐 CoreCLR find_card_dword）。
- `EnsureCardBundleCoverage` 随 L1 growth 懒增长；`ClearAllCards` 同步清 bundle。

### K2e（世代过滤）— 评估为已覆盖
CRAG 的 `DirtyCard` 已有 nursery fast-skip（gen0 卡不设），young GC Phase 2 精确扫整个 nursery —— 故"跳过 gen0 卡"的世代过滤在 CRAG 2-gen 模型下**等效于现有 nursery fast-skip**，K2b 双参屏障进一步避免设 gen0→gen0/同代卡。K2e 无新增必要，已由 K2b+K2d+既有 fast-skip 覆盖。

## GC-K2 全部完成（K2a/b/c/d + K2e 覆盖）

**完整 region 世代写屏障链路**：region→gen skewed 表（K2a）→ 双参世代屏障（K2b）→ codegen 发射（K2c）→ card bundle 扫描加速（K2d）。跨平台纯 C++ + AOT/JIT 双适配 + BOUNDARY_OVERRIDE 已标注。

## 完整验证注

K2c 完整 foundation-dll 管线 regen 为后续验证项（本会话源级 + runtime 链接级验证 + 11 项 GC 单测 0 回归）。
