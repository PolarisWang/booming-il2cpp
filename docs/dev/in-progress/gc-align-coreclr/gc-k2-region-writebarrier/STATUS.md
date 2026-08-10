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

### K2a（region→gen 表）✅ 完成
- `Region` 加 `uint8_t gen`（低 2bit 代表，对齐 CoreCLR `region_info` low2）。
- `kRegionGenMask/Young/Old` 常量；`kRegionGenShift=22`（4MB region）。
- **全局 `g_region_to_gen` skewed 表** + inline `GetRegionGen(addr)`/`SetRegionGen(addr,gen)`（对齐 CoreCLR `map_region_to_generation_skewed`：`addr>>shift`查表无减偏移）。
- `EnsureRegionGenCoverage` 懒增长（realloc 翻倍）。
- `AllocateRegion`: NURSERY→young(0)，其余→old(2)，同步 `r->gen` + 表。
- **验证**：编译通过；10 项 GC 单测 0 回归。

## 待续（K2b-e）

- K2b：`chaos_gc_dirty_card_dst_ref` 双参屏障（3 条件短路：dst.gen==0 / ref 越界 / ref.gen>=dst.gen）。
- K2c：codegen 升级 stfld/stobj/stelem.ref 发射双参屏障（🔴 codegen 边界，需 BOUNDARY_OVERRIDE）。
- K2d：card bundle + ScanDirtyCards gen 过滤。
- K2e：mark 扫脏卡 gen>condemned 过滤。
