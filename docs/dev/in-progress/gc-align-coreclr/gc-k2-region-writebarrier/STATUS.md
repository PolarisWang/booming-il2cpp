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

### K2b（双参世代写屏障）✅ `53246252f`
- `chaos_gc_dirty_card_dst_ref(dst,ref)`：忠实复刻 CoreCLR 3 短路（dst.gen==0 / ref==null / ref.gen>=dst.gen），否则 `DirtyCard(dst)`。
- 用 K2a `GetRegionGen` 表；纯 C++ 跨平台。
- 保留单参兼容入口。

## 待续（K2c-e）

- **K2c：codegen 升级发射双参屏障**（🔴 高风险，触碰 codegen 边界，需 BOUNDARY_OVERRIDE）。将 stfld/stobj/stelem.ref 发射点从 `chaos_gc_dirty_card(dst)` 升级为 `_dst_ref(dst,ref)`，让 gen0→gen0 真实省卡。需验证 AOT + JIT + 热更新兼容。
