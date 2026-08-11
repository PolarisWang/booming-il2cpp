# plan-v3-01 — GC 能力补足 最终可落地计划（CoreCLR 对齐裁决）

> 域：GC(6)+JIT(4)+运行时(2)。创建：2026-08-11。
> **方案裁决原则（用户拍板）**：凡有方案疑问，一律选**对齐 CoreCLR** 的方案。
> 本计划把所有待办（主线A 正确性 + 主线B GC-M1..M15）落到 CoreCLR 对齐的具体方案 + 文件 + 步骤 + 验证。
> 上游：`plan-v2-01.md` + `roadmap-v2-01.md` + 本会话实测。arch_review_mode: `critical`。

---

## 0. 现状基线（2026-08-11 已落地）
- 层1 `GcMarkRangeOld` ✅
- 系统性 region-gen（nursery 全 chunk young / 旧代全 OLD）✅ —— stress 47%→75%
- T2 VerifyHeap + HeapVerify 旋钮 + `gc_diagnostics_test`(3/3) ✅
- T3 `condemned_gen_num` 字段 + nursery region-gen 断言 ✅

---

## 主线 A：年轻代跨代引用正确性（P0，CoreCLR 对齐）

### A1 ✅ 系统性 region-gen（收尾）
- 已落地；收尾＝`GcVerifyRegionToGenerationMap` 覆盖 nursery 全 chunk，跑 `CHAOS_GC_HeapVerify=1` 无 mismatch。

### A2 — T1 残余：晋升对象 post-check `IsInOldGen` 失效（🔴 最高优先）
**CoreCLR 对齐方案**：CoreCLR 用**精确 relocation**（`relocate_compact.cpp`）在压缩时把所有指向被移动对象的指针改写到新地址，从源头杜绝 stale 引用；同时对象晋升后**归属 region 不变**、`page_array_` 保持一致，post-check 必能识别。
**实施「低险先行，高险后置」**：
1. **A2a（低险，先行）**：核 `page_array_` 一致性 —— 每次 `AllocatePage`/压缩回收后，确认晋升对象所在页在 `page_array_`、`RebuildPageArray` 时机正确。若 post-check 失效是"页不在数组"→ 修 page_array_ 覆盖/重建时机（文件 `gc_old_gen.cpp RebuildPageArray/Collect`）。验证：stress + `DIAG-DANGLE` 归零候选，20x 全绿即止。
2. **A2b（高险，后置，CoreCLR 对齐）**：若 A2a 不足以解决（是压缩移动了对象），对齐 `relocate_compact.cpp` 的**精确引用重定向**：旧代压缩时遍历被移动对象的引用区，把引用更新为新地址。文件 `gc_old_gen.cpp`(compact) + `gc_root_change.*`（已存在 relocate 协作）。需专门设计 + T2 Verify 护航。

**A2 验证**：`ctest -R region_barrier -L stress` 20x 全绿 + `DIAG-DANGLE`（IsInOldGen 失效计数）归零。

### A3 — Verify 强化（防 A2 回归）
**CoreCLR 对齐**：`verify_heap` 每次 GC 出口校验 region→gen 表 + 晋升产物 fall-in-tracked 页。
实施：`GcVerifyHeap` kFull 级加"晋升产物抽查"（遍历晋升 worklist，断言 `IsInOldGen`）。文件 `gc_diagnostics.cpp`。
验证：`gc_diagnostics_test` 增晋升一致性子测。

### A4 — 卡重扫语义（对齐 CoreCLR clear-before-scan / only-condemned / bit 原子）
**CoreCLR 对齐方案**：
- 先清卡再 mark（`mark_phase.cpp` clear_cards before scan），竞态重设卡被下次 GC 兜底。
- 只清 condemned 代卡、保留旧代卡（`mark_phase.cpp:3733`）。
- 共享卡字 bit 原子 `lock or`（`JitHelpers_FastWriteBarriers` Bit Region64）避免并发丢 bit。
实施：复核 young-GC Phase-1 clearing 顺序 + 加 bit 卡原子写（`gc_card_table.h DirtyCard` 对共享字用原子 OR）。验证：`gc_card_table_ext_test` 增并发写卡原子子测。

---

## 主线 B：CoreCLR 对齐补足能力（GC-M1..M15）

### B1 P0（正确性/验证，先做，每项对齐 CoreCLR 文件）

| 任务 | CoreCLR 对齐方案 | 本仓库实施 | 验证 | 难度 |
|------|-----------------|-----------|------|------|
| **GC-M1** K2c regen | 世代写屏障管线（`writebarriermanager`）| foundation-dll 单 chunk regen，确认生成 `.cpp` 实际发射 `_dst_ref` | 生成代码含 `_dst_ref` + 原生编译 | M |
| **GC-M2** stress(=A2) | `mark_phase.cpp:1388` gen 过滤实证 | = 主线 A | stress 20x 全绿 | M |
| **GC-M3A** Server GC 多堆 | multi-heap coordination | 加 GC_SERVER=ON 集成测试，gc_coordinator 多堆协调 | Server 模式多堆跑通，域卸载 O(region) | L |
| **GC-M10** gen>condemned | `mark_phase.cpp:1393` | 3 代(M9)前置后实测过滤不漏 | gen 过滤单测 | S |

### B2 P1（能力/性能，强依赖链，CoreCLR 对齐）

| 任务 | CoreCLR 对齐方案 | 依赖 | 难度 |
|------|-----------------|------|------|
| **GC-M5** BGC 两快照分相 | `background.cpp`（并发 mark + STW final_marking）| — | L |
| **GC-M9** 三代分代 | `gc.h`（gen0/1/2）| K2/K3 | XL(拆 A/B) |
| **GC-M8** plan-gen 重绑 | `plan_phase.cpp` region_to_generation | M9 | L |
| **GC-M7** region demotion | `regions_segments` / plan demotion | M8 | XL(拆 A/B) |
| **GC-M4** provisional 完整 | `collect.cpp:1638`（gen1 compact+NGC2）| M5+M3B+D1 | L |
| **GC-M3B** Dynamic Heap Count | `dynamic_heap_count.cpp` | M3A | L |
| **GC-M6** dynamic_tuning 伺服 | `dynamic_tuning.cpp`（free-list/frag/memory load）| E1+M10 | L |

### B3 P2（工程/可观测，随补，CoreCLR 对齐）
- **GC-M11** 全量旋钮 → `gcconfig`（~40 新增：HardLimit 分代等）
- **GC-M12** GCHandle 类型 → `handletablecore`/`objecthandle`（REFCOUNTED/WEAK_INTERIOR_POINTER 等）
- **GC-M13** 完整事件集 + per-GC 原因位图 → `eventpipe`/`gc_events`
- **GC-M14** GCProvModeStress + 并发根扫描 → `collect.cpp` + `background.cpp`
- **GC-M15** oom_budget gen 级缩放 → `allocation.cpp`

---

## C. 可落地执行顺序（串行主链 + 并行，能推进则推）

```
Phase A（正确性，先）:
  A1(收尾) → A2a(page_array_ 低险) → A3(Verify) 
  [若 A2a 不足] → A2b(relocate 对齐 CoreCLR，高险)
  → stress 20x 全绿 = A 完成 ✅

Phase B1（P0 验证）:
  M1(K2c regen) → [依赖 A2] M2 = A → M3A(Server) → [依赖 M9] M10(gen 过滤)

Phase B2（P1 能力，强依赖链，串行推进）:
  M9 三代(拆 A/B) → M8 plan-gen → M7 demotion(拆 A/B)
  M5 BGC 分相(独立) → M4 provisional(M5+M3B+D1)
  M3B(←M3A) → M6(M10)

Phase B3（P2 工程，独立可并行）:
  M11 / M12 / M13 / M14 / M15
```

> 说明：XL 项（M9/M7）与深手术（A2b/M4/M5）是**跨会话**工作量。本计划是完整 roadmap；每 Phase 完成后测试全绿再进下一阶段，避免在未修完正确性时堆叠能力。

## D. 关键验证
```bash
ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "region_barrier|gc_diagnostics|gc_young_collector|gc_card_table_ext" -L stress --output-on-failure
CHAOS_GC_HeapVerify=1 <stress exe>
python tests/runner/test_driver.py --layer unit
```

## E. 风险与护栏
| 风险 | 护栏 |
|------|------|
| A2b/M4/M5 深手术高险 | 低险先行(A2a)；A2b 用 relocate 精确重定向+C 自检；每阶段测试全绿再进 |
| XL(M9/M7/M3B) 跨会话 | 拆里程碑 A/B；A 里程碑先行，B 后置；保留 2 代备选 |
| 3 代重构破坏既有 | 保留 2 代 + gen1-独立代备选（父 roadmap）|

## F. 退出条件（全部完成）
- A 主线：stress 20x 全绿 + Verify 全覆盖 + unit 无回归。
- B 主线：M1-M15 每项达 exit_criteria（实现+测试+多平台/JIT/AOT/热更 proof，产出 wiki 沉淀 `24-CRAG-GC架构参考.md`）。
