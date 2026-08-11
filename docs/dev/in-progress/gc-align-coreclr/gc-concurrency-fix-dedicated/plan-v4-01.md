# plan-v4-01 — GC 后续开发计划（CoreCLR 存在性核实版）

> 域：GC(6)+JIT(4)+运行时(2)。创建：2026-08-11。
> 裁决原则：方案有疑问一律选对齐 CoreCLR。
> **本版动作**：逐项核实 plan-v3 每待办对应的 CoreCLR 功能是否真实存在（核对 `D:\OpenSource\dotnet\runtime\src\coreclr` 源码）。
> 结论：**主线 B 全部是 CoreCLR 真实功能**；**仅 A2b 需修正/重定位**（非 CoreCLR 平行功能，见 §A2b）。

---

## 0. CoreCLR 存在性核实结果（2026-08-11）

| 计划项 | CoreCLR 对象 | 源码证据 | 存在 |
|--------|-------------|---------|------|
| A3 Verify 强化 | verify_heap / verify_region_to_generation_map | `gc/diagnostics.cpp` (7 处) | ✅ |
| A4 卡重扫 | clear_cards / only-condemned / bit `lock or` | `gc/mark_phase.cpp` + `vm/amd64/JitHelpers_FastWriteBarriers.asm` (2) | ✅ |
| GC-M1 K2c regen | WriteBarrierManager | `vm/writebarriermanager.cpp` | ✅ |
| GC-M3A Server GC | MULTIPLE_HEAPS 多堆 | `gc/gc.cpp`/`gcpriv.h` | ✅ |
| GC-M3B Dynamic Heap | dynamic_heap_count.cpp | `gc/dynamic_heap_count.cpp` | ✅ |
| GC-M4 provisional | collect.cpp provisional | `gc/collect.cpp` (17) | ✅ |
| GC-M5 BGC 分相 | background.cpp | `gc/background.cpp` | ✅ |
| GC-M6 伺服 | dynamic_tuning.cpp | `gc/dynamic_tuning.cpp` | ✅ |
| GC-M7 demotion | regions_segments / plan demotion | `gc/regions_segments.cpp`/`plan_phase.cpp` | ✅ |
| GC-M8 plan-gen | plan_phase set_region_gen_num | `gc/plan_phase.cpp` | ✅ |
| GC-M9 三代 | gc.h soh_gen0/1/2 | `gc/gc.h` (4) | ✅ |
| GC-M10 gen>condemned | mark_phase condemned_gen | `gc/mark_phase.cpp` (75) | ✅ |
| GC-M11 全量旋钮 | gcconfig GcConfig keys | `gc/gcconfig.h` (10) | ✅ |
| GC-M12 GCHandle | handletablecore / objecthandle | `gc/handletablecore.cpp` + `objecthandle.cpp` | ✅ |
| GC-M13 事件集 | eventtrace | `gc/gceventstatus.cpp` 等 | ✅ |
| GC-M14 ProvStress | GCProvModeStress | `gc/gcconfig.h` (1) | 🔶 弱存在（仅 config，collect 无）|
| GC-M15 oom_budget | allocation.cpp oom_budget | `gc/allocation.cpp` (3) | ✅ |
| **A2b** 旧代 mark 跟进 raw-struct | **CoreCLR 无此平行**（用精确 layout/gcinfo 扫引用）| — | ⚠️ 需重定位 |

---

## 主线 A：年轻代跨代引用正确性（残余，P0）

### A1 ✅（已提交）系统性 region-gen 全 chunk 标记 —— stress 47%→75%

### A2b — 重定位：非 CoreCLR 平行功能，是仓库/测试模型问题
**核实结论**：CoreCLR 不通过"跟进 raw-struct 引用"解决问题；它用**精确 GC layout（gcinfo / method-table ref map）**在 mark 时扫描对象引用。本仓库 stress 测试的 `OldMessage` 是 `raw void*[128]` C++ 结构、**无注册 GC layout** → mark 不把它当含引用的托管对象 → 晋升对象被回收。**这是测试伪象 / 仓库引用模型缺口，非 CoreCLR 功能对齐项。**
**落地调整**：A2b 改为下列之一（据此是否保留）：
1. **（推荐，测试伪象修复）** 给 stress 测试的 `OldMessage` 注册 GC layout（如 GcLayoutRegistry），让其引用被精确扫描 —— 使复现器真实反映托管对象行为，验证明晋升引用链正确。
2. **（本仓库引用模型）** 若真实托管旧对象也丢引用 → 是仓库旧代 mark 的引用扫描缺失，需对齐 CoreCLR 精确 layout；但这是仓库缺陷不是"CoreCLR 功能"，归入 bug 修复而非计划项。
> 结论：A2b **不再是独立的"CoreCLR 对齐计划项"**，降为复现器/引用模型 bug（详见下）。把它从"对齐计划"删除，改为一条验证/修复 note。

### A3 ✅ 保留（CoreCLR verify_heap 对齐）
GC 出口核验晋升产物 fall-in-tracked 页（`GcVerifyHeap` kFull 级加抽查）。

### A4 ✅ 保留（CoreCLR clear-before-scan / bit 原子）
复核 young-GC Phase-1 clearing 顺序 + 共享卡字 bit 原子 `lock or`（对齐 CoreCLR）。

---

## 主线 B：CoreCLR 对齐补足能力（GC-M1..M15，全部核实为真实 CoreCLR 功能）

### B1 P0
| 项 | 内容 | CoreCLR 源 | 难度 |
|----|------|-----------|------|
| GC-M1 | 世代写屏障管线 regen（生成代码实际发射 `_dst_ref`）| writebarriermanager | M |
| GC-M3A | Server GC 多堆集成测试 | MULTIPLE_HEAPS | L |
| GC-M10 | gen>condemned 过滤实证 | mark_phase:1393 | S（M9 前置）|

### B2 P1（强依赖链）
| 项 | 内容 | CoreCLR 源 | 依赖 | 难度 |
|----|------|-----------|------|------|
| GC-M9 | 三代分代 | gc.h | K2/K3 | XL(拆A/B) |
| GC-M8 | plan-gen 重绑 | plan_phase | M9 | L |
| GC-M7 | region demotion | regions_segments | M8 | XL(拆A/B) |
| GC-M5 | BGC 两快照分相 | background.cpp | — | L |
| GC-M4 | provisional 完整 | collect.cpp | M5+M3B+D1 | L |
| GC-M3B | Dynamic Heap Count | dynamic_heap_count | M3A | L |
| GC-M6 | dynamic_tuning 伺服 | dynamic_tuning | E1+M10 | L |

### B3 P2（独立可并行）
GC-M11 全量旋钮 / GC-M12 GCHandle 类型 / GC-M13 事件集 / **GC-M14 ProvStress（CoreCLR 弱存在，仅 config 旋钮；collect 无实际 stress 路径 → 降优或删）** / GC-M15 oom_budget。

---

## 更新后的推进顺序
```
A: [A2b→改为 复现器/引用模型 bug note] → A3 → A4 → stress 复现器反映托管语义
B1: M1(K2c regen) → M3A(Server) → [M9后] M10
B2: M9→M8→M7 ; M5→M4 ; M3B / M6
B3: M11/M12/M13/M15（独立并行）；M14 降优(P2后置或删)
```

## 删除/降级项（核实后）
- **A2b 从"CoreCLR 对齐计划"删除** → 降到"复现器/引用模型 bug 修复"（Non-CoreCLR 功能）。
- **GC-M14 ProvStress** → CoreCLR 仅 config 旋钮、无实际 collect stress 路径 → **降优后置或删除**。
- 其余全部保留（核实为 CoreCLR 真实功能）。

## 验证
```bash
ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "region_barrier|gc_diagnostics|gc_*_test" -L stress --output-on-failure
python tests/runner/test_driver.py --layer unit
```

## 退出条件
- A：stress 复现器在托管语义下不再 dangling（或确认为测试伪象并注册 layout）。
- B：每 GC-M* 达 exit_criteria（实现+测试+多平台/JIT/AOT/热更 proof）。
