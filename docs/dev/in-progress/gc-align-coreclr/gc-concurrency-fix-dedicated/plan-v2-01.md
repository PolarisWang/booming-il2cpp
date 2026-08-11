# plan-v2-01 — GC 后续开发计划（补足能力 v2）

> 域：GC(6)。创建：2026-08-11。
> 目的：重新梳理两条主线 ——（A）**年轻代跨代引用正确性**（`gc_region_barrier_stress_test` dangling，
> 含系统性 region-gen 根 + T1 残余），（B）**与 CoreCLR 对齐补足 GC 能力**（父 roadmap GC-M1..M15）。
> 上游：`roadmap-v2-01.md`（GC-M1-M15）+ `plan-v1-01.md`（T1-T7）+ 本会话实测结论。
> arch_review_mode: `critical`（GC 正确性 + 测试主线）。

---

## 0. 现状基线与已落地（2026-08-11）

| 项 | 状态 | 说明 |
|----|------|------|
| 层1 `GcMarkRangeOld` | ✅ 已落地无回归 | 旧代/LOH 页 alloc 标 OLD |
| **系统性 region-gen 修复** | ✅ 已落地 | `AllocateRegion` 标记 region 全部 4MB chunk（nursery/gen1=young、tenured/LOH/POH=OLD）。GC 全组无回归；**stress 通过率 ~47%→75%** |
| T2 VerifyHeap 自检 | ✅ `gc_diagnostics.*` + HeapVerify 旋钮 + test(3/3) | GC 入口/出口挂接 |
| T3 condemned 概念 | 🟡 字段 + nursery region-gen 断言（young 5/5）；实际 3 代前置 | `condemned_gen_num` |

**已证实的共同根**：`AllocateRegion` 原先只标 begin 一个 4MB chunk，nursery 其余 chunk 默认 OLD(2) → 屏障/过滤/扫描对 young ref 误判。已修。

**T1 残余（对 CoreCLR 的再定位）**：残余 ~25%。
- smoking gun：`phase3e skipNonNursery==dangling`（Phase-1 已晋升并写回 tenured，post-check `IsInOldGen(tenured)==false`）。
- **不是"重扫卡片"而是"晋升对象的 tenured 地址在 post-check 时 IsInOldGen/page_array_ 失效"** —— 指向 **page_array_ 覆盖一致性 / 压缩-relocate 精确重定向**（对齐 CoreCLR `relocate_compact.cpp`），非事后扫卡。

---

## A. 主线一：年轻代跨代引用正确性（P0 正确性）

### A1. 系统性 region-gen（✅ 已落地，收尾）
- [done] `AllocateRegion` 全 chunk 标记。
- **待收尾**：`gd_diagnostics` 的 `GcVerifyRegionToGenerationMap` 现应能覆盖"nursery 全 chunk young"；
  跑 `CHAOS_GC_HeapVerify=1` 确认无 mismatch（回归网开启）。

### A2. T1 残余 —— 晋升对象 post-check 识别失败（🔴 深 GC，CoreCLR 对齐方向）
- **根因方向**（重定位为 CoreCLR 对齐）：晋升对象 tenured 地址在 post-check 时 `IsInOldGen` 失效。
  两候选：
  1. **page_array_ 覆盖缺口**：晋升页未进 `page_array_` / `RebuildPageArray` 时机 → post-check 找不到 → 补 page_array_ 一致性（与系统性 region-gen 同源，低险）。
  2. **压缩/全 GC 移动了对象未 relocate**：若旧代压缩移动晋升对象而没精确更新引用 → 需对齐 CoreCLR `relocate_compact.cpp` 精确重定向（高险，深手术）。
- **验证判据**：`DIAG-DANGLE`（ref region_gen=0 且 IsNursery=0 且 IsOoG=0… 改为检测 IsInOldGen 失效）+ stress 20x 全绿。
- **先低后高**：先查 page_array_ 一致性（候选1），确认不是再碰压缩 relocate（候选2）。

### A3. Verify 自检强化（承接 A2）
- `GcVerifyHeap` + region-gen 一致性（已有 T2）扩展到**晋升后 IsInOldGen 一致性抽查**：每次 young-GC 后，
  抽查晋升产物 fall into-tracked 旧代页。防 A2 类回归。

### A4. 卡重扫语义（对齐 CoreCLR clear-before-scan / only-condemned-clear）
- **待做**：对齐 CoreCLR 卡表"先清卡再 mark" + "只清 condemned 代卡、保留旧代卡"。当前 young-GC 清 nursery/Gen1 卡、
  旧代卡保留（已验证保留）。需复核 mark 前 clearing 与并发重设卡（CoreCLR bit 原子 `lock or`）。

---

## B. 主线二：与 CoreCLR 对齐补足能力（沿用父 GC-M1..M15）

### B1. P0 正确性/验证（先做）
| 任务 | 对齐 | 当前 | 目标 |
|------|------|------|------|
| **GC-M1** K2c 世代写屏障管线 regen | foundation-dll regen | 🟡 Phase 1a 已跑，函数体未实际调 `_dst_ref` | 单 chunk 生成代码**实际发射** `_dst_ref` + 编译 |
| **GC-M2** 世代写屏障压力（= 本任务 A 主线）| mark_phase:1388 过滤实证 | 🟠 75% | 20x 全绿（A2 解决）|
| **GC-M3A** Server GC 多堆集成测试 | gc_coordinator/heap_manager | ⬜ | GC_SERVER=ON 多堆协调跑通 |
| **GC-M10** mark 扫脏卡 gen>condemned 实证 | mark_phase:1393 | 🟡 字段有 | 3 代前置后实测过滤不漏 |

### B2. P1 能力/性能（中，依赖链）
| 任务 | 对齐 | 依赖 |
|------|------|------|
| **GC-M5** BGC 两快照/分相 | background.cpp | — |
| **GC-M4** provisional 完整 | collect.cpp:1638 | M5+M3B+D1 |
| **GC-M3B** Dynamic Heap Count | dynamic_heap_count.cpp | M3A |
| **GC-M6** dynamic_tuning 伺服 | dynamic_tuning.cpp | E1+M10 |
| **GC-M8** plan-gen 重绑 | plan phase | M9 |
| **GC-M9** 三代分代 | gc.h 三代 | K2/K3 |
| **GC-M7** region demotion | regions_segments demotion | M8 |

### B3. P2 工程/可观测（低，随补）
GC-M11 全量旋钮 / M12 GCHandle 类型 / M13 事件集 / M14 ProvStress / M15 oom_budget。

---

## C. 建议推进顺序（聚焦、依赖正确）

```
Block A（正确性，先行）:
  A1(已✅收尾 Verify 覆盖) → A2（候选1 page_array_ 一致性，低险先行 → 候选2 relocate） → A3 自检强化
  → stress 20x 全绿 = A 主线完成
Block B（对齐能力）:
  B1: M1(K2c regen) → M10(需 A2/3代) → M3A(Server GC)
  B2: M9三代 → M8 plan-gen → M7 demotion ; M5 BGC分相 → M4 provisional ; M3B/M6
  B3: 随补
```

## D. 关键验证命令
```bash
ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "region_barrier|gc_diagnostics|gc_young_collector" -L stress --output-on-failure
CHAOS_GC_HeapVerify=1 <stress exe>   # A1 Verify 回归网
python tests/runner/test_driver.py --layer unit   # 全量无回归
```

## E. 风险与护栏
| 风险 | 护栏 |
|------|------|
| A2 候选2（压缩 relocate）高险深手术 | 先查候选1 page_array_ 一致性；候选2 需专门设计 + Verify 自检护航 |
| M10 需 3 代前置 | M10 挂在 M9 后；先以 A 主线验证 gen>condemned 字段 |
| 3 代重构破坏既有 | 保留 2 代 + gen1-独立代（父 roadmap 备选）|

## F. 文档维护
- `STATUS.md` 主记录（blocking_questions:[]、clearance:cleared、arch_review_mode:critical）。
- 每子任务收尾：结构审视 → 测试 → STATUS completed → commit（根因/策略/回归）。
- 知识沉淀：wiki `24-CRAG-GC架构参考.md` 更新 region-gen 全 chunk 标记 + 卡表语义。

## G. 退出条件
- Block A：stress `-L stress` 20x 全绿 + `test_driver.py --layer unit` 无回归。
- Block B：每 GC-M* 达期 exit_criteria（实现+测试+多平台/JIT/AOT/热更 proof）。
