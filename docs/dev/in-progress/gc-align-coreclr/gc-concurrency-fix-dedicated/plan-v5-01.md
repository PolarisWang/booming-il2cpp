# plan-v5-01 — GC 开发计划（落地导向：功能 + 测试 + 完成判据）

> 域：GC(6)+JIT(4)+运行时(2)。创建：2026-08-11。复用 plan-v4 的 CoreCLR 存在性核实结论。
> 目标：**推进顺序编号、每项含「实施步骤 / 测试落地 / 完成判据」**，执行后功能落地 + 测试落地 + 全部完成。
> 裁决原则：方案疑问一律选对齐 CoreCLR。arch_review_mode: `critical`。

## 执行约定（每项完成后才进下一项）
1. 改代码 → 2. 写/改测试 → 3. `test_driver.py --layer unit` 全绿（无非 known FAIL）→ 4. 域 STATUS 记 completed → 5. commit（含 root_cause/fix_strategy/regression_check）。XL/深项拆里程碑，每里程碑测试全绿再进。

---

## P1 — 主线 A 残余（年轻代跨代引用正确性，P0 先行）

### P1-A3 `VerifyHeap` 强化（CoreCLR verify_heap 对齐）
- **目标**：GC 出口核验晋升产物 fall-in-tracked 旧代页，防引用模型回归。
- **实施步骤**：`gc_diagnostics.cpp` 的 `GcVerifyHeap` kFull 级，遍历 `YoungCollectionResult` 晋升 worklist，断言每个 `IsInOldGen`；挂 `GcYoungCollection` 出口。
- **测试**：`gc_diagnostics_test` 增「晋升产物 tracked 一致性」子测。
- **完成判据**：`gc_diagnostics_test` 全绿 + `CHAOS_GC_HeapVerify=2` 跑 stress 确认晋升页可识别。

### P1-A4 卡重扫语义（CoreCLR clear-before-scan / only-condemned / bit 原子）
- **目标**：先清卡再 mark、只清 condemned 代卡、共享卡字 bit 原子 `lock or`。
- **实施步骤**：复核 young-GC Phase-1 clearing 顺序；`gc_card_table.h DirtyCard` 对非 0xFF 先用原子位 OR 再写，避免并发写卡丢 bit；确认旧代卡保留。
- **测试**：`gc_card_table_ext_test` 增「并发写同卡字不丢 bit」多线程子测。
- **完成判据**：card_table_ext 全绿 + stress 无新增回归。

### P1-A2b 复现器/引用模型修复（旧 A2b 重定位——非 CoreCLR 功能，降为 bug）
- **目标**：`OldMessage`(raw struct 无 GC layout) 的引用不被 mark 扫描 → 晋升对象被回收。修复为托管语义。
- **实施步骤**：a) 用 `GcLayoutRegistry` 给测试 `OldMessage` 注册 GC layout（pointer offsets=0..127），使其引用被精确扫描；或 b) 若真实托管旧对象也丢引用 → 修 `Gc_OldGen().Collect` 用 layout 扫存活旧对象引用。
- **测试**：`gc_region_barrier_stress_test`（托管语义下）20x 全绿。
- **完成判据**：stress（托管回归）20x 全绿；`DIAG-DANGLE` 归零。

> **主线 A 完成 = P1-A3 + P1-A4 + P1-A2b 全部完成，stress 20x 全绿。**

---

## P2 — 主线 B1（P0 验证，CoreCLR 真实功能）

### P2-M1 K2c 世代写屏障管线 regen
- **目标**：foundation-dll 单 chunk regen，确认生成代码**实际发射** `_dst_ref`。
- **实施步骤**：选有 gc-ref field-write 形状的 chunk，regen 生成 `.cpp`，grep 确认 `chaos_gc_dirty_card_dst_ref` 出现在函数体；原生编译链接通过。
- **测试**：生成代码编译 + foundation-dll 管线测试。
- **完成判据**：生成 `.cpp` 实际含 `_dst_ref` 调用 + AOT/JIT 编译通过。

### P2-M3A Server GC 多堆集成测试
- **目标**：GC_SERVER=ON 多堆协调跑通。
- **实施步骤**：启 `CHAOS_IL2CPP_GC_SERVER` 多堆路径；验证 `gc_coordinator`/`gc_heap_manager` 多堆协调；域卸载 per-heap。
- **测试**：新增 Server 多堆集成测试（多 heap 分配+GC+域卸载）。
- **完成判据**：Server 模式多堆测试全绿，域卸载 O(region)。

### P2-M10 gen>condemned 过滤实证（M9 前置后做）
- **目标**：卡扫描只 mark condemned 及更老代对象。
- **实施步骤**：3 代(M9)后，`GcScavengeObject*` 用 `condemned_gen_num` 实过滤，mark_phase:1393 对齐。
- **测试**：`gc_young_collector_test` 增 gen>condemned 丢弃子测。
- **完成判据**：过滤单测过 + 3 代后无回归。

---

## P3 — 主线 B2（P1 能力，强依赖链）
（每项难度级 + 里程碑，跨会话可拆）

### P3-M9 三代分代（gc.h soh_gen0/1/2）— XL(拆 A/B)
- 里程碑 A：gen1 独立代；→ 测试 gen1 晋升/回收。
- 里程碑 B：gen2/max_generation + 卡 bundle 扫描 + 回归。
- **完成判据**：3 代集内晋升/提升 AGE 正确，无条件回归。

### P3-M8 plan-gen 重绑（plan_phase set_region_gen_num）— 依赖 M9
- 目标：GC 后按新代重写 region-gen 表。
- **完成判据**：plan-gen 重绑正确，写屏障短路不退化。

### P3-M7 region demotion（regions_segments demotion）— 依赖 M8，XL(拆 A/B)
- 里程碑 A：基本 demotion；里程碑 B：age-based evacuation + 域卸载不碎片。

### P3-M5 BGC 两快照分相（background.cpp）— 独立，L
- 目标：并发 mark + STW final_marking 两快照 + foreground 交错。
- **完成判据**：bgc_* stress 全过 + pause 预算。

### P3-M4 provisional 完整（collect.cpp）— 依赖 M5+M3B+D1，L
- 目标：高记忆+高碎片 → gen1 强制 compact + NGC2 排队。
- **完成判据**：provisional 触发单测 + 无回归。

### P3-M3B Dynamic Heap Count（dynamic_heap_count.cpp）— 依赖 M3A，L
- 目标：运行期增减堆数闭环。
- **完成判据**：堆数变化测试 + 回归。

### P3-M6 dynamic_tuning 伺服（dynamic_tuning.cpp）— 依赖 E1+M10，L
- 目标：free-list/碎片/memory load → gen 预算闭环。
- **完成判据**：伺服单测 + 分配回归 <5%。

---

## P4 — 主线 B3（P2 工程，独立并行）
- **P4-M11** 全量旋钮（gcconfig，~40 新增）→ 测试 config 读取。
- **P4-M12** GCHandle 类型（handletable/objecthandle，REFCOUNTED/WEAK_INTERIOR 等）→ handles 测试。
- **P4-M13** 事件集（eventtrace）→ ETW/EventPipe 测试。
- **P4-M14 ProvStress（CoreCLR 弱存在）→ 降优后置或删除。**
- **P4-M15** oom_budget gen 级缩放（allocation.cpp）→ OOM 预算测试。

---

## 里程碑/阶段切分（跨会话安全）
- **M1（本会话或下一会话完成）**：P1 主线 A（A3/A4/A2b）→ stress 20x 全绿。
- **M2**：P2-M1 + P2-M3A。
- **M3**：P3-M9 → M8 → M7（三代链，XL 拆里程碑）。
- **M4**：P3-M5 → M4；M3B/M6。
- **M5**：P4 B3（并行）。
> 每里程碑结束：`test_driver.py --layer unit` 全绿 + `ctest -R gc_ -L stress` 无回归 + 提交；STATUS 记录。

## 验证命令
```bash
ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -R "gc_" -L unit --output-on-failure
python tests/runner/test_driver.py --layer unit
CHAOS_GC_HeapVerify=1 artifacts/native-runtime-core-test/Debug/chaos_gc_region_barrier_stress_test.exe
```

## 退出条件（全部完成）
- P1 主线 A：stress 20x 全绿。
- P2/P3/P4：每 GC-M* 达 exit_criteria（实现+测试+多平台/JIT/AOT/热更 + wiki `24-CRAG-GC架构参考.md` 沉淀）。

## 删除/降级（plan-v4 已核实）
- A2b 从对齐计划删除 → P1-A2b 复现器/引用模型 bug。
- GC-M14 ProvStress → 降优后置或删除。
