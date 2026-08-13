# 推进 1+2 — 修复方案 + CoreCLR 深度分析

> 日期：2026-08-13
> 方法：双路 Explore（CoreCLR 实读 + CRAG 实读）+ 本地源码核实，只读分析。涉及问题 1 (A2b 跨代 UAF) 与问题 2 (CodegenStSFld 反向 root)。
> 输出：① CoreCLR「如何避免 store-then-barrier 竞态」的精确机制；② Problem 2 的确定性修复（立即、低风险）；③ Problem 1 的修复方案分析（含 CoreCLR 对齐路径）。

---

## 0. TL;DR（给决策者）

- **Problem 2（CodegenStSFld）是确凿、独立、低风险可立即修的正确性 bug**：AOT/JIT 静态字段写路径对**非 GC 的 `g_static_fields`** 仍调 SATB + root-change，与其他两条 StSFld 路径矛盾，重新引入 e6ea531e7 刚修的 BGC re-mark 内存损坏 hazard。**修复 = 删两行 barrier，纯 store 对齐另两路。**
- **Problem 1（A2b）根因是 CRAG 的写屏障与 CoreCLR 的根本语义差异**：CoreCLR 的写屏障是 **GC-poll-free LEAF**（store+card 在同一无挂起点的叶子内，协作式挂起只能在叶子*之后*的 poll 发生）→ 竞态窗口在体系上不存在。CRAG 的 `chaos_gc_dirty_card` 是**普通函数调用**（store 与 card 之间有 call 边界），且 safepoint 对 forbid-suspend 线程是 **ack-and-continue** → 窗口真实存在。**根治 = 对齐 CoreCLR 语义，而非"改顺序"或"硬 STW"。**
- 本报告给出 4 个方案（含 2 个新角度），推荐 **方案 A'（对齐 CoreCLR 叶子不变量，最小全局影响）** 为主，原有的 γ'/方案2a 降为备选。

---

## 一、Problem 2 — CodegenStSFld 反向 root（确凿，可立即修）

### 1.1 现状（三处源码核实）

| StSFld 路径 | 位置 | 行为 |
|------------|------|------|
| 解释器 Handle | `interpreter_vm.cpp:580-594` | **纯 store**，正规注释声明非 GC ✓ |
| JIT Reg_StSFld | `ir_reg_alloc.cpp:1207-1240` | **纯 store** ✓ |
| AOT/JIT CodegenStSFld | `jit_helpers.cpp:471-481` | **仍有 SATB + root-change** ✗ |

`CodegenStSFld`（`jit_helpers.cpp:471-481`）：
```cpp
extern "C" void CodegenStSFld(uint32_t field_offset, uint64_t value) noexcept {
    ...
    JitSatbPreWriteBarrier(reinterpret_cast<void**>(&g_static_fields[field_offset].obj));      // ✗
    chaos::il2cpp::runtime_core::BgcRecordRootChange(
        reinterpret_cast<void**>(&g_static_fields[field_offset].obj),
        g_static_fields[field_offset].obj);                                                    // ✗
    g_static_fields[field_offset] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}
```

### 1.2 根因（为何这是 bug）

`g_static_fields`（`interpreter_vm.cpp:37`，`CHAOS_IL2CPP_VECTOR<InterpreterValue>`）：
- **从未** `GcRegisterStaticRootRange` 注册，`GcScanStaticRoots`（`gc_static_roots.cpp:70-85`）不扫它（确证：全仓只有 `assembly_manager.cpp:151` 注册**另一块** GC 静态字段，非本向量）。
- 它存 `InterpreterValue::obj` = **指向解释器堆（MemoryDomain/TLS arena）的裸指针**，非 GC 托管对象。
- 对这样一个非 GC 槽调用 `BgcRecordRootChange` → BGC re-mark 时 `BgcDrainRootChangeBuffer`（`gc_bgc.cpp:207`）把解释器堆裸指针当作 GC 对象头去 mark/follow → **内存损坏/悬挂**。这正是 e6ea531e7 已修、本路径又重新引入的 hazard。

**路径可达性**（确证）：JIT emit `jit_codegen_emit.cpp:1362-1368` 的 `StSFld` → `EmitRuntimeHelperCall(::CodegenStSFld)`。即 AOT/JIT 每一条静态字段写都走这个函数，风险不是理论而是实际触发。

### 1.3 修复（确定性、低风险）

**删除 `JitSatbPreWriteBarrier` 与 `BgcRecordRootChange` 两行，改为与另两路一致的纯 store。**

```cpp
extern "C" void CodegenStSFld(uint32_t field_offset, uint64_t value) noexcept {
    CHAOS_IL2CPP_PROFILE_SCOPE("Codegen::StSFld");
    using namespace chaos::il2cpp::interpreter;
    if (field_offset >= g_static_fields.size()) {
        g_static_fields.resize(field_offset + 1u);
    }
    // g_static_fields 是非 GC 向量（从未 GcRegisterStaticRootRange 注册），存的是
    // 解释器堆 InterpreterObject*。不可调 SATB / BgcRecordRootChange——那会把解释器
    // 堆指针当 GC root 在 BGC re-mark 时误标 → 内存损坏。对齐 interpreter_vm.cpp:580
    // 与 ir_reg_alloc.cpp:1207 的 barrier-free 路径。
    g_static_fields[field_offset] = InterpreterValue::from_i64(static_cast<int64_t>(value));
}
```

> ⚠️ **裁决点**：若未来希望 `g_static_fields` 真正被 GC 追踪（converting it to GC-managed static root），正确做法是**真正 `GcRegisterStaticRootRange`**（像 `assembly_manager.cpp:151`），同时把 `InterpreterValue::obj` 换成真正的 GC 引用 + 正确 barrier。但那是解释器对象模型物化（Step 1.2，已被用户中止）的一部分，**本轮只做止血对齐**，不扩大范围。

### 1.4 验证
- 编译 + `ctest`（jit/解释器无回归）。
- 补三路 StSFld 一致性测试：断言 `CodegenStSFld` 写 `g_static_fields[offset]` 后不再调用 `BgcRecordRootChange`（或直接单元测试该函数无 side effect beyond store）。
- BGC re-mark 解释器堆指针损坏 hazard 消除（e6ea531e7 语义恢复）。

---

## 二、Problem 1 — A2b 跨代 UAF：CoreCLR 机制深挖（用户要求的"看下 coreclr 怎么避免"）

### 2.1 决定性事实（双路 Explore 交叉核实）

**(a) CoreCLR 的 store 与 card-set 确实是两个独立指令**（非原子），见 `D:\OpenSource\dotnet\runtime\src\coreclr\vm\amd64\JitHelpers_FastWriteBarriers.S`：
- `JIT_WriteBarrier_PreGrow64`：store at line 14，card at lines 39-46。
- `JIT_WriteBarrier_Byte_Region64`：store at line 223，card at 269-276。
- `JIT_WriteBarrier_SVR64`：store at 175，card at 182-190。

**但**每条 barrier 都以 `LEAF_ENTRY`/`LEAF_END`（`pal/inc/unixasmmacrosamd64.inc:36-66`）声明为 **leaf**：
> leaf = 无栈帧、无 GC info、**无任何 call / safepoint poll**。

**(b) 核心不变量**（`WriteBarriers.S:165-167` 明文注释）：
> *"Note that we rely on the fact that no GC can occur between here and the card table update we may perform below."*

含义：**协作式挂起只在 safe point（JIT 插的 poll 点）生效**。线程在 leaf 内执行 store+card 时，GC 无法掐它（因为它没到 poll 点）。**要么完整跑完 store+card（poll 在下一条），要么还没开始。绝不可能被挂在 store 与 card 之间。** 从 GC 角度看，store+card 是一个原子步骤。

**(c) 并发/BGC 路径的兜底**（`gc/background.cpp:1637 background_mark_phase`）：
- 并发阶段（mutator 运行）：`revisit_written_pages(TRUE)` 两次（lines 1879-1883），用 **write-watch（4KB 页粒度，保守）** 重扫污染页。
- STW remark（lines 1965, 2058-2059）：`bgc_suspend_EE()` 后对**所有污染页再扫一次** `revisit_written_pages(FALSE)`。世界停止时无在途 store → 任何 store+card 拆分都被此刻捕获。

> **结论**：CoreCLR **不做** store+card 原子化，也不依赖改顺序或硬 STW。它靠两条：
> 1. **STW/前台 GC**：写屏障是 poll-free leaf → 竞态窗口体系性不存在。
> 2. **并发/BGC GC**：write-watch 页重扫 + 结束时 STW remark 兜底在途拆分。

### 2.2 CRAG 与 CoreCLR 的差距（实读证据）

| 维度 | CoreCLR | CRAG（当前） | 差距 |
|------|---------|-------------|------|
| store+card 形态 | 同一 **poll-free leaf** 内 | `store`（内联指令）→ `call chaos_gc_dirty_card*`（普通函数） | **有 call 边界，可被挂起**（`jit_helpers.cpp:96-102`, `LinearEmission.cs:84-89`） |
| safepoint 对 forbid-suspend 线程 | 一致点才停 | **ack-and-continue**（`thread_state.cpp:274` depth>0 只 ack 不 block） | GC 可在 mutator store 与 card 之间进 Phase-1 |
| card 元素内存序 | 无 fence（也不需要，因 leaf） | relaxed atomic（`gc_card_table.h:170-175`） | 无序耦合到 store |
| 卡清除顺序 | STW 扫时边清边扫（`mark_phase.cpp:3579`, `clear_cards` 3673/3717） | **scan-then-clear**（`gc_young_collector.cpp:468-493`扫, 853-876 最后清） | 内部一致，但窗口在"GC 获取 safepoint vs 线程 store/barrier"之间 |
| SATB 方向 | post-store 卡负责 old→nursery | SATB 是 pre-write（保护失去引用），**不覆盖** young-into-old 方向 | SATB 无法兜 A2b（`gc_bgc_inline.h:42-102` 读旧值） |
| 尾巴兜底 | write-watch 页重扫 + STW remark | **无保守全 old-gen 重扫兜底** | 并发下无 remark 兜底 |

### 2.3 为什么会 dangling（窗口机制，与 2-12 charter 一致但更精确）

```
mutator:  *old_slot = nursery_ptr（store，普通call外）
          → 此刻 GC 请求 safepoint，线程 ack-and-continue 未 block
          → GC 进 Phase-1：该页卡还是干净的（barrier 未跑）→ 跳过该槽
          → chaos_gc_dirty_card(slot) 才执行（已晚）
          → reset nursery → 仅从此槽可达的 nursery 对象被回收 → dangling
```
注：`nursery_ptr` 在这窗口内**不止从该槽**可达可能避免，但 A2b 场景是**仅从此槽可达** → 回收。safepoint **can land between store and barrier** 正是因为 store 在普通指令、barrier 是普通调用、safepoint 对 forbid 线程 ack-and-continue。

### 2.4 SATB 不保护此方向（关键澄清）

SATB（`gc_bgc_inline.h:42-102`）在读 **OLD 值**（`old_obj = *slot`），记录被覆盖前的引用，用于并发 mark 期间不丢失"失去可达性"的对象。它**不保护**新增的 `*old_slot = nursery_ptr` 这条 young-into-old 边——那是 **card table post-write barrier** 的职责。所以即使把 SATB 原子化也不闭合 A2b。

---

## 三、Problem 1 修复方案（4 个，含 2 个新角度）

### 方案 A'（推荐）：对齐 CoreCLR 叶子不变量 —— 最小全局影响

**核心**：让「store + card」成为 GC 不可插队的原子段。对齐 CoreCLR 的方式不是硬 STW，而是保证**协作式挂起无法落在 store 与 card 之间**。

两条子路（可组合）：

**A'-1：safepoint 语义修正（查 ack-and-continue 的根因修正）** — 修 `thread_state.cpp:274`：
当前 forbid-suspend 线程 `depth>0` 时 ack-and-continue → GC 不等它。改为：**深度归零才真正 ack**。协调器 `RequestGlobalSafepoint` 除等 `suspend_ack`，还等所有线程 `tls_forbid_suspend_depth==0`（= 无在途 barrier）。
- 优点：一处运行时改动根治窗口；任何 store+card 只要包在 `ForbidSuspendScope`（已有原语）内即可。
- 风险：需正确枚举"在途 barrier"，且改 wait 语义是全局（易死锁）。这是 γ' 的核心，但 γ' 只改了部分（仍 95-117），因为**代码路径没有全部包**。

**A'-2：统一/对齐 store+barrier 到共享 helper 内相邻 + no-poll 保证**（γ' 的骨架）：
JIT 已把 GC-ref 写统一路由到共享 helper（`jit_helpers.cpp` `CodegenStFld`/`StElem`/`StObj`），这些 helper 把 store+barrier 收在一函数内。**在函数头包 `ScopedBarrierAtomic`（复用 ForbidSuspendScope）**，一处覆盖 JIT 全部发射。codegen（managed .cs `LinearEmission.cs`）是唯一需逐站点包的。

> **为何 A'-1 是"对齐 CoreCLR"的本质**：CoreCLR 靠"leaf 内无 poll"天然保证。CRAG 无法轻易把 C++ helper 变成真 `LEAF`（那是 asm），但**语义等价**的做法是：修改 safepoint 让 forbid-suspend 的 barrier 区在 GC 看来不可打断——即让 GC 等 barrier 退出再进 Phase-1。这与 CoreCLR "GC 只能在 poll 点停"的语义对齐。

**成本/风险**：主要风险在 A'-1 的 wait 语义（全局）。建议先做 Problem 2（独立、止血），再做 A'-1 单独提交 + 全 ctest 防死锁。

---

### 方案 B（原 γ'/方案 2a）：helper 临界区 + safepoint 真等

即 charter 主线：共享 helper 包 `ForbidSuspendScope`(store+barrier) + **修 safepoint 等待语义**（scope 内线程不可 ack，GC 真等其退出）。发射器 0 改动（helper 天然收口）。
- 已探明所有站点的 store→barrier 关系（charter §4）。
- 与方案 A' 实质是同一根（修 wait 语义 + 包临界区），差别是**是否逐站点改 codegen**。A' 更强调先把运行时"等 barrier 退出"做对，B 更强调把所有站点都包住。

> 两者可合并为一个实现计划：**阶段 1 修 safepoint wait（A'-1 核心件），阶段 2 包三套发射（A'-2/B），阶段 3 验证**。这正是设计-t7-final-gamma §6 的结构。

---

### 方案 C（新）：Phase 1 对"未 ack 线程涉及的卡/页"精确重扫（兜底）

在 Phase-1 之前加一次"对在 safepoint 协商期间仍活跃（或已 store 未 barrier）的线程"涉及的卡/页重扫。charter 已证 **naive 全槽重扫污染**（1023/1024，因为无精确布局扫不出对象边界）。但可用 **write-watch 思想**（CoreCLR）：
- 粒度到 **4KB 页**（保守）：任何 store 到页面任意字节 ⇒ 整页需重扫。
- 配合 `SoftwareWriteWatch`（CoreCLR `SoftwareWriteWatch`）或 CRAG 若已有类似 watchdog 无 → 需新基建。
- 定位：作为 A' 的**兜底**（A 负责"无在途 barrier"，C 负责"万一有遗漏站点/时序"）。

**成本**：需实现 write-watch（CRAG 目前无 `Software Write Watch`，8-12 review B5 列为缺口）。适合作为**长线加固**，非本轮主线。

---

### 方案 D（远期）：hard-STW（真 SuspendThread）— 对齐 CoreCLR 的完整形态

升级 Windows `pal_preempt_win32.cpp:52` 的 APC-park 为真 `SuspendThread + GetThreadContext`（`PalCaptureReliable` 翻 true，`pal_preempt.h:108`）。
- charter 已确证：**hard-STW 单独不足**（M1 曾证硬挂起仍可停在 store/card 之间），必须配方案 A/B（码 barrier 在扫前已置）。
- 定位：与 `jit-phase2-register-capture`（方案2/B SuspendThread 远期）同一基建，可作为跨平台对齐的长期项。

---

## 四、推荐落地计划（结合 problem 2 + problem 1）

| 阶段 | 内容 | 域 | 风险 | 验收 |
|------|------|----|------|------|
| **P2-a** | 修 `CodegenStSFld`：删两行 barrier，纯 store 对齐 | CodeGen+JIT(4/5) | **低** | 三路 StSFld 一致；e6ea531e7 语义恢复；编译通过 |
| **A-1** | 修 safepoint 的 ack-and-continue → "深度归零才 ack"、协调器等 barrier 退出 | 运行时(2) | 高 | 全 ctest 无死锁；stress 无 hang |
| **A-2** | 三套发射（jit_helpers / 解释器 / codegen LinearEmission）包 `ScopedBarrierAtomic` | GC+CodeGen(6,4) | 中 | 站点全包；regen snapshot |
| **A-3** | `gc_region_barrier_stress_test` 20x 全 0；从 known-failing 翻 PASS | GC(6) | — | 20x 0 dangling |
| **C**（备选兜底） | write-watch 页重扫兜底 | GC(6) | 中 | 并发 mark 下无漏 |
| **D**（远期） | hard-STW（SuspendThread） | 运行时(2) | 高 | PalCaptureReliable==true all平台 |

> 第一优先 = **P2-a（Problem 2 止血）**：独立、确凿、低风险、路径可达，直接消一个已核实的内存损坏 hazard。Problem 1 的 A-1 是真正的根治（对齐 CoreCLR 叶子语义），但风险高需单独提交 + 全回归。

---

## 五、测试 / CI 门禁（推动"最危险缺陷进屋"）

- **修复前**：把 `gc_region_barrier_stress_test` 从 `-LE` 排除翻转纳入 gate（已知失败但**必跑**），让红灯可见（8-12 报告建议）。
- **修复后**：20x 全 0 → 提为 PASS 强制 gate；`CHAOS_GC_HeapVerify=2` under stress 无 UAF。
- **补一致性测试**：三路 StSFld 写入等价性 + `CodegenStSFld` 不再触 root-change。
- 双测试树过滤不匹配（gc-ci `-R chaos_gc_` 匹配不到 `test_gc_*` 56 目标）——顺势修。

---

## 六、可信度边界

1. CoreCLR 机制基于实读 `D:\OpenSource\dotnet\runtime\src\coreclr\`（leaf、write-watch remark 均有 file:line）。
2. Problem 1 的"修复后 20x 全 0"是目标量级，非本次实测（本报告只读）。
3. A-1 的 wait 语义改动是全局高风险，需在阶段 1 做针尖实验确认能可靠枚举在途 barrier。
4. Problem 2 修复为确定性止血；"是否把 g_static_fields 转 GC 托管"是更大解释器物化项，单独决策。
