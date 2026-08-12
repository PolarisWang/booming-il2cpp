# JIT A-2 完整执行计划（更新版 — 含深挖根因）

> **创建**：2026-08-10 ｜ **状态**：执行中（第 1 批已完成 4 项）
> **上游**：`jit-regalloc-a2-continuation-roadmap.md` + 本会话深挖发现
> 本文档是**唯一权威执行蓝本**，取代 roadmap 中的推进细节，纳入所有新发现。

---

## 0. 深挖已确认的关键根因（修正原文档诊断）

| 原文档声明 | 深挖实测结论 | 影响 |
|---|---|---|
| §1.1 kQuick 返回 0 = kQuick 发射 bug | **调用方 ABI 问题**，§0 thunk 已修复，6/6 PASS | ✅ 已解决 |
| §1.2 test_jit_bench = "gtest 断言边界" | **测试构造非法高 vreg IR** (`MakeAccumulateMethod(100)` → vreg>64)，测试需修 | ✅ 已修 |
| §4.1 unwind 测试 | **测试断言过时的 Win64 UNWIND_CODE 布局**，实现正确 | ✅ 已修 |
| test_jit_native/Fuzz 未提及 | **图着色在合法 vreg 下 Compile emit 崩溃**（非 SEH 可捕），= §2.1 核心真实 bug | 🔴 待修 |
| ctest -j8 并行 | **test_jit_mode/bench 并行 flaky**（串行稳定 PASS）= 测试基建 RESOURCE_LOCK 序列化缺失 | 🔴 待修 |

## 1. 已完成（3 commit 已提交）

| Commit | 内容 | 验证 |
|---|---|---|
| `14532ebee` | §0 ABI thunk + tree-builder alias + orphan 恢复；消除 §1.1 | test_jit_native 67/67(Fuzz 除外), pgo 6/6 |
| `25adce74a` | §1.2 bench 测试改合法 vreg loop | test_jit_bench 10/10 |
| `2b845d36e` | §4.1 unwind 断言对齐 Win64 | test_jit_unwind 5/5 |

当前 jit 基线：**14/15**（唯一真实失败 = test_jit_native/Fuzz 图着色 emit crash）。

---

## 2. 剩余任务完整清单（按依赖顺序）

### 阶段 J2-A：图着色 emit 崩溃修复（Fuzz）— 最前置
- **根因（已深挖实证）**：`CodegenNativeTest.Fuzz`（seed=42）= 栈溢出（c00000fd, CDB 确认 prologue rep stos）。崩溃 emit 含 `op=7 LdLoc dst=64`/`Ret src1=64`（**vreg=64**）。Fuzz 原始 `rm.instructions` **无高 vreg**（HIREG=0 实证）→ vreg=64 由 **引擎 Compile 内部（enable_optimizer=false + enable_register_caching=true 图着色路径）产生**。
- **CoreCLR 对照（已读 regalloc.cpp/gcencode.cpp/compiler.h）**：
  1. 局部变量用**动态有类型 lvaTable**（LclVarDsc），堆分配，无固定上限。
  2. GC 栈映射用 **`StackSlotIdKey` + 去重哈希表**，offset 是 `INT32 + GC_SP_REL/GC_FRAMEREG_REL`（不限 12bit）。
  3. 精确 vs 保守：`GC_SLOT_TRACKED` / `GC_SLOT_UNTRACKED`，struct 字段级。
- **Review 修正（平台/热更新实测，源码实证）**：
  - **平台**：GC 扫描端用 `frame.frame_ptr + offset`（gc_root_scanner.cpp:116），**slot 偏移相对统一帧基址，跨平台中性**（非 RSP-rel as 原判定）。真正的限制是 **12bit offset(4096)** = 编码紧凑性，非平台隔离问题。`ArchTraits` 已映射 x64 RBP / ARM64 X29。**待验证**：JIT 编码端 GprOff 基址是否 == frame_ptr。
  - **热更**：registry 是 **"code address → GcSlotMapV0"**（gc_root_scanner.cpp:27），按 `return_address` 精确查找 → **天然就是"代码版本 → GC map"**，对齐 CoreCLR ReJitManager 语义，**无需新造版本化**。§2.3 收敛为"验证重编译后新 map 正确 + 新旧共存回归测试"。
- **收敛后修复路径（1.5-2.5d，工作量较原方案下降）**：
  - **R1 验证基址一致性（~2h）**：✅ **已完成（本次）**。结论：`thread_state.cpp:675-680` 用 `t4_rbp - nm->rbp_to_rsp_offset` 得 frame_ptr（=扫描时 SP），`slot_addr=frame_ptr+GprOff` 与 JIT 编码的 SP-rel offset **一致，基址正确，非栈溢出根因**。平台适配确认 OK（x64 RBP/ARM64 X29）。
  - **R2 定位 vreg=64 产生点**：✅ **已完成（根因链全解析，2 commit 已固化）**。① 崩溃是 `Test_Fuzz` 的 mismatch-recompile(`cfg_no_cache` 未设 optimizer→默认 true) 走 tree/linearizer 生成 vreg≥64(LdLoc)→emit 栈溢出(c00000fd)。② 改 cfg_no_cache 禁 optimizer(**843f415bc**) → **Fuzz 从崩溃→完整运行**。③ Fuzz landing Ret 原 `pick_written()` 可选中未定义 vreg(UB: T4未初始栈槽垃圾 vs RE零初始), 改 entry-defined vreg(**934569aba**) → **mismatch 8→2**。
  - **R3 扩 slot offset 位数**（~0.5-1d，待 §2.1）：12bit(4096) → 更大 offset，去 4096 上限。
  - **R4 高 vreg emit 栈安全 / run514 剩余 2 mismatch**：**已收窄**——实证排除: int32 slot 高位残留(StoreGpr 归一化实验无效, 已回退)、[9]Dup(emit 正确)、[12]Shl 32 位宽(与 RE `&0x1F` 一致)。剩余: T4 某运算算出 `0xcf37ffff` vs RE `0xffffffff`, 需**完整反汇编 + 逐条对 RE int32 模拟**锁定 → **§2.1 运算语义统一（T2.1 子项）**。
  - **R5 热更一致性回归测试**（~0.5d）：验证现有 code→map registry 在新旧共存下正确。
  - **R6 补测试**：高 vreg 精确 GC/帧用例 + Fuzz 边界。
- **验收**：Fuzz 1000 run 全过；test_jit_native 15/15。
- **依赖**：无（独立）。**阻断**：T2.1/2.2/2.3/2.4。

### 阶段 J2-B：§2.1 图着色默认化（A1 全量）
- **根因已明**：图着色已被调用（jit_engine.cpp:4292）但仅部分接入。A1=全 vreg 走图着色，消除 write-through 栈往返。
- **前置**：J2-A（图着色正确）。A2（tier 分级）作为 benchmark 数据兜底。
- **验收**：`test_jit_native` 寄存器断言无栈往返；benchmark 证明性能。

### 阶段 J2-C：§2.2 精确 GC slot maps
- **现状**：`RecordGcPoint`(jit_engine.cpp:581) + `slot_map_entries_` 已生成，但保守（按 live_in_）。
- **工作**：liveness → 每 safepoint 精确物理寄存器掩码 + 溢出精确栈槽。
- **依赖**：J2-A/B（图着色 liveness 是输入）。
- **验收**：`jit_gc_slot_map_test` 覆盖"精确扫描只扫存活引用"。

### 阶段 J2-D：§2.3 spill 契约 + 热更一致性
- **现状**：`GcSlotMapV0` 序列化(12bit) + deopt `ReadSpillSlot`，offset 语义需固化为单一权威。
- **风险**：`static_assert(kFrameSize<=4096)`——图着色扩大寄存器改变帧布局，监控。
- **验收**：契约文档 + 热更（DemoteJittedMethod→重跑T4）后 GC map 一致回归测试。

### 阶段 J2-E：§2.4 monolith 拆分（先拆再改，D3）
- **现状**：jit_engine.cpp 6229 行单类 NativeCodeGenerator。
- **拆法**：按 memory `[[jit-engine-modularization]]` —— regalloc/gc/unwind/emit/tier/osr/seh/encoding 8 文件。**纯移动不改逻辑**。
- **前置**：J2-A（绿基线护栏）。拆分时机在 J2-A~D 之间（图着色 bug 先修）。
- **验收**：类职责单一化，原测试 15/15 全过。

### 阶段 J3：测试/构建基建（D 部分已含）
- **J3.2 L11 stale 产物清理**：删 `artifacts/presets/*/testing/` 陈旧 vcxproj + CMake 重生成。
- **J3.3 孤儿检查器**：`check_workarounds.py`（已存在）+ API 引用检查，CI 门禁。
- **并行 flaky 修复**：jit 测试加 `RESOURCE_LOCK`（native.py 注释已预告 GC 组需串行化）。

### 阶段 C 线（跨域，D6：并行批次但不用 worktree）
- **C1/T4.1 benchmark 可信度**：修 ns/alloc=0/fast-path=1.0 伪影，mean/median 统一。供 J2-B 性能验收证据。
- **C2/T4.2 消除中间层 dispatch**：`src/managed/Chaos.IL2CPP.Generator/` 优化 kChaosExternalRuntimeFnTable 绕行 + intrinsic。CodeGen 域。
- **C3/T4.3 翻译语义 GVM/Span/vararg/tail**：翻译域，需 Agent 注入 translation-expert。
- **C4/T4.4 数据治理**：CoreLib 覆盖 + aggregate 口径 + audit。

---

## 3. 执行顺序决策

用户指令："深挖理清逻辑 → 计划列完整 → 再执行"。

1. **当前**：ASan 已 configure + building。用它定位 Fuzz/图着色崩点 → 精确根因。
2. **拿到崩点后**：修 §2.1 图着色 emit bug → Fuzz 过 → **jt 15/15 绿基线**。
3. **并行 flaky**：加 RESOURCE_LOCK → 修 ctest -j8 稳定性。
4. **再评估** monolith 拆分 / 精确 GC / 跨域：在绿基线上逐个闭环。
