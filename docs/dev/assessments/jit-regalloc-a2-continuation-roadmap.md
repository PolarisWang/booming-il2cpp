# Chaos IL2CPP — JIT / 原生寄存器分配 后续开发 Roadmap

> **状态**：现行路线图｜ **创建**：2026-08-10
> **背景**：A-2「原生寄存器分配（图着色 + 精确 GC 默认化）」深挖中，系统性修复了 JIT 测试基建（ABI 技术债 **~已被验证**），并暴露了多个真实的、未闭合的欠缺。本文把这些欠缺**全部列出**，作为启动新方案的执行清单。
> **关联**：`docs/dev/assessments/il2cpp-core-capability-guide.md`（全局能力缺口）、`t4-limitations.md`（JIT/GC 局限）

---

## 0. 当前已交付（已验证，尚未提交）

> 这些是本次调查已修复并验证的成果，**亟待提交固化**，别丢。

| 项 | 改动 | 验证 |
|---|---|---|
| ABI thunk spawn | `t4_jit_call.asm`（RBX/RSI 重绑）+ 8 个 jit 测试文件 `ExecuteNative`→`T4CallNative` + CMake ASM_MASM | codegen_native_test 18/18、test_jit_native 61/61、il_smoke 16/16、simd 8/8、mode 12/12 |
| tree 优化器修复 | `jit_tree_builder.cpp` +34 行（`Dup`/`Box`/`Unbox` 映射 kNop 丢弃 → 值别名/透传） | CopyProp/UnboxElim 在默认优化器下 PASS |
| orphan 测试恢复 | `codegen_native_test.cpp` 从 `codegen/`→`jit/` 重构孤儿 重写到新 JIT API + 恢复 CMake（原 EXCLUDED） | 首次能编译运行，18/18 |

**第一优先：把上表提交**（新分支，含 `root_cause`/`fix_strategy`/`regression_check` 三段元数据）。

---

## 1. 未闭合的真实缺陷（深挖中实测发现，独立成任务）

### 1.1 🔴 T4 kQuick / no-optimizer 发射路径返回 0（pre-existing，已 baseline 归因）
- **证据**：`PgoTest.Tier0CodeWorks` 用 `MakeTier0Config`（`enable_optimizer=false` + `compile_tier=kQuick`）期望 42，返回 0。**baseline（无任何我的改动）同样返回 0** → 非我引入。
- **说明**：T4 在 `enable_optimizer=false`（不走 tree 优化器）时，`LdcI4+Ret` 等基础场景发射出错。默认 config（optimizer=true）经 thunk 修复后已正常，但 **no-optimizer/kQuick 路径有独立 bug**。
- **影响**：PGO/tier0 相关（`test_jit_pgo` 5/6 FAIL）及任何关闭优化器的使用。
- **验收**：`PgoTest.Tier0CodeWorks` 期望 42 返回 42；`test_jit_pgo` 全过。
- **建议**：深挖 `jit_engine.cpp` 的 kQuick/tier0 发射分支（非 tree 优化器路径）。

### 1.2 🟡 `test_jit_bench` 1 FAIL — gtest 断言边界
- **证据**：`JitBench.ExecTime_Tier0_vs_Tier1`，值 5050 正确、性能 1.73x 正确，但 `EXPECT_EQ(ExecuteNative(...), 5050ULL)` FAIL（期望显示 5050 实际 5050）。
- **说明**：gtest 整数比较/打印边界，非正确性回归。
- **验收**：修断言（如用 `EXPECT_EQ(x, uint64_t{5050})` 或拆分），测试转绿。

---

## 2. A-2 本体（本次未开始，真正的目标）

> 深挖证明了 T4 在正确 ABI 下"能跑"，但**图着色 + 精确 GC 的默认化收敛未做**。这是 A-2 主题。

### 2.1 🔴 图着色寄存器分配默认化（非 write-through）
- **现状**：`jit_reg_alloc.h`（706 行 Chaitin-Briggs）已写出但**未作为默认路径**；`t4-limitations.md #5`。T4 仍走栈式文件/部分 write-through。
- **工作**：图着色结果接入 T4 发射主路径；消除 `load→compute→store` 栈往返（非 write-through）。
- **验收**：生成代码寄存器常驻，关键路径无栈往返；`test_jit_native` 的寄存器相关断言通过。

### 2.2 🔴 精确 GC slot maps（保守扫描 → 精确）
- **现状**：`t4-limitations.md #4` 保守栈扫描。Liveness 已规划未生成精确 slot mask。
- **工作**：Liveness 分析 → 每个 GC safepoint 的精确物理寄存器掩码 + 溢出精确栈槽。
- **验收**：GC 只扫描存活引用；无过度保留；`jit_gc_slot_map_test` 覆盖精确扫描。

### 2.3 🟡 GC-map + deopt spill 契约的单一权威定义 + 热更新一致性
- **工作**：`GcSlotMapV0` 序列化 + deopt 的 `ReadSpillSlot` 用同一套 offset 语义，固化为权威文档；验证热更重编译（`DemoteJittedMethod`→重跑 T4）后 GC map 一致。
- **风险**：`static_assert(kFrameSize <= 4096)`（12 位编码）——图着色扩大寄存器可能改变帧布局，需监控不踩上限。
- **验收**：契约文档 + 热更后 GC map/回归测试。

### 2.4 🟡 T4 JIT monolithic 类拆分
- **现状**：`jus_engine.cpp` 6229 行 `NativeCodeGenerator` 单类承载寄存器分配/SEH/unwind/tier/编码/OSR。8-module 拆分仅 proposal（memory `[[jit-engine-modularization]]`）。
- **建议**：作为 2.1-2.3 的架构配套（改图着色/GC 时同步拆）。

---

## 3. 全局能力缺口（来自能力评估报告，P1 优先）

> 详见 `il2cpp-core-capability-guide.md`，此处列 AJ - 要新方案一并解决的。

### 3.1 🔴 性能：消除中间层 dispatch
- AOT 相对 .NET 8 核心路径 2.5–30× 落后，瓶颈 `kChaosExternalRuntimeFnTable` + cross-assembly 绕行 + intrinsic 落 interpreter。
- 前提：先修 benchmark 可信度。

### 3.2 🔴 性能证据：benchmark harness 可信度
- AOT 微方法 ns/alloc=0/fast-path=1.0，无法区分"真实优化"vs"短路未测"。修 harness 伪影 + mean/median 口径统一。

### 3.3 🟡 翻译语义缺口：（翻译域 agent 发现）
- 泛型虚方法 dispatch（GVM）整体缺失、一等 `Span<T>`/ref struct（ByRefLike）未建模、`vararg` 无处理、`tail.` 前缀被静默丢弃。

### 3.4 🟡 覆盖盲区 + 数据治理
- CoreLib 内层语义核心方法未被 fact/benchmark 覆盖；aggregate 均值 vs median 分叉；audit 台账过期。

---

## 4. 贯穿性技术债（系统性，一次清）

### 4.1 🔴 JIT 测试群孤儿/ABI 补全（系统性恢复所有 test_jit_*）
- 本次修了 codegen_native_test + 6 个 test_jit 文件（用 thunk）。但**其余 test_jit_*（unwind/`test_jit_abi`/gc_slot_map/osr 等）需逐一确认**是否都用正确 ABI/已恢复。跑全 `ctest -L jit` 建立回归基线（当前 `<100%`）。

### 4.2 🟡 L11 迁移残留清理
- 旧 `testing/` 路径残留（`artifacts/presets/*/testing/src/native/jit/*.vcxproj` 是陈旧产物，指向已删源）；CMake 需干净重新生成，移除 stale 产物。

### 4.3 🟡 孤儿测试系统化
- 深挖发现多处"测试引用已迁移/被重构 API"。建一个确定性检查器（方案 B 的 `check_workarounds.py` + 一致的 API 引用检查）防复发。

---

## 5. 启动建议（分序立项）

| 序 | 任务簇 | 依赖 | 立即价值 |
|---|---|---|---|
| 1 | **提交 §0 已验证成果** | 无 | 固化不丢，回滚安全 |
| 2 | **§1.1 T4 kQuick 发射 bug（pre-existing）** | 无 | 修真正正确性缺陷 |
| 3 | **§2 A-2 本体（图着色+精确GC）** | §1.1（依赖 T4 正确） | 性能/内存核心 |
| 4 | **§3.2 benchmark 可信度** | 无 | 让性能决策可信 |
| 5 | **§3.1 消除中间层** | §3.2 | 性能跃升 |
| 6 | **§4.1/4.2 测试群恢复 + L11 清理** | §1 | 回归基线可靠 |

---

## 验收总定义
- **全 `ctest -L jit` 100% 通过**（当前 < 100%，因 §1.1/§4.1 未闭合）。
- **`codegen_native_test`/`test_jit_native` 保持 18/18、61/61**（回归护栏）。
- 图着色默认化后，`test_jit_native` 寄存器相关断言 + 性能数据证明无栈往返。
