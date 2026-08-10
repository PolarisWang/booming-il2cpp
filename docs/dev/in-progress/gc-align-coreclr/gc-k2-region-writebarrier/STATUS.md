# STATUS — GC-K2 RegionWriteBarrier（架构设计）

## 元信息

```yaml
task_id: gc-align-coreclr/gc-k2-region-writebarrier
task_type: design         # 架构设计阶段；实现按 K2a-e 分步
phase: design
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

世代感知 region→generation 写屏障，**尽量忠实对齐 CoreCLR 原本设计** + 跨平台 + AOT/JIT。本 phase = 架构设计文档（`design-k2-v1-01.md`），已深度只读 CoreCLR 还原。

## 设计要点（忠实 CoreCLR）

| CoreCLR 权威（文件:行号） | K2 对齐 |
|-----------------|---------|
| `region_info` 1字节：当前代(低2bit)+计划代(高2bit)+flag（gcpriv.h:1578-1596） | `GetRegionGen` 读 `table[addr>>22]&0x3` |
| `map_region_to_generation_skewed`（card_table.cpp:527, gcinternal.h:2290） | skewed 基址减 offset，一次 `>>shift` 查表 |
| region 写屏障 3 短路（asm Region64）：`dst.gen==0` 跳过 / `ref` 越界 / `ref.gen>=dst.gen` | `chaos_gc_dirty_card_dst_ref(dst,ref)` 复刻 3 条件 |
| card bundle 2MB 粗卡扫描加速（card_table.cpp:1619） | bundle 位图 + ScanDirtyCards 过滤 |
| mark 扫脏卡 `gen>condemned→skip`（mark_phase.cpp:1388） | ScanDirtyCards gen 过滤（对齐 GC-F1 handle 剪枝） |
| StompWriteBarrier 换函数体（gc.cpp:557） | **函数指针切换**（CRAG AOT/JIT 双模式，避免 stomp 跨平台复杂） |

## 忠实差异记录

1. **stomp→函数指针**：CoreCLR 单一 JIT stomp 代码体；CRAG AOT/JIT 双模式用函数指针 `g_write_barrier_fn` 切换，跨平台安全。
2. **双参屏障适配 codegen**：CoreCLR JIT barrier 拿 dst+ref；CRAG `chaos_gc_dirty_card(dst)` 单参 → 升级双参 `_dst_ref`，codegen stfld/stobj/stelem.ref 发射点改传 ref（**codegen 边界改动，K2c 需 BOUNDARY_OVERRIDE**）。
3. **保留单参兼容入口**（ref 不可用时报 dst 保守路径），保护热更新既有调用点。

## 分步落地（K2a-e）

K2a region→gen 表(低) → K2b 双参屏障(中) → K2c codegen 升级(🔴高) → K2d card bundle(中) → K2e mark 过滤(中)。每步独立验证提交。

## 期望 Output

- [x] `design-k2-v1-01.md`（忠实 CoreCLR + 跨平台 + AOT/JIT 适配设计）
- 实现：K2a-e 分步（后续会话）

## 下一步

- K2a：建 region→gen 表 + `GetRegionGen` + 代号维护（低风险，可先行）。
