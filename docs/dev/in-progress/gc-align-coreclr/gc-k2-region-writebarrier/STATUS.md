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
- `chaos_gc_dirty_card_dst_ref(dst,ref)`：忠实复刻 CoreCLR 3 短路，否则 `DirtyCard(dst)`。用 K2a `GetRegionGen` 表。

### K2c（codegen 发射双参屏障）✅ `4f59836d1`
- 4 个可取 ref 的发射点升级 `_dst_ref`：value-type stfld / ref stfld / stind.ref / stelem.ref。
- 保留 cpobj 拷贝路径单参保守（无单 ref）。
- `chaos_gc_dirty_card_dst_ref` 声明加入 gc_helpers.h + gc_card_table.h。
- # BOUNDARY_OVERRIDE: issues/GC-K2c（Codegen 层产生 C++ 调用新 GC API）。
- 验证：runtime 编译链接 + Generator 0 error。

## 待续（K2d/e）

- **K2d：card bundle 2MB 粗卡 + ScanDirtyCards gen 过滤**（低/中风险，独立）。
- **K2e：mark 扫脏卡 gen>condemned 过滤**（中风险）。

## 完整验证注

K2c 完整 foundation-dll 管线 regen（生成 native 编译）为后续验证项（本会话做了源级 + runtime 链接级验证）。
