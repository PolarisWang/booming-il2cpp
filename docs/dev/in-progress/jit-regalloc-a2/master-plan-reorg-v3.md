# JIT 分配质量 + 性能线 — 完整工作计划（重新梳理后 v3）

> **日期**：2026-08-12（基于真实运行实证修正后）
> **状态**：权威续跑计划。取代 `cross-platform-unify-plan.md` 中失焦的 §1-7 旧结构；§9 性能线并入本计划。
> **核心修正**：经预编译 AOT entry.exe **实际跑码**，推翻「中间层普遍 fake-return / 落解释器」假设。

---

## 0. 定位：这套工作在整个 il2cpp 里处理什么

**在「GC 必须扫到存活引用」+「方法必须有真实 native 执行」这两个正确性前提下，让 JIT 生成的机器码质量更高、且 AOT 跨 assembly 调用不落回解释器/假返回。** 纯性能 + 正确性收敛，不改 IL/翻译/HotUpdate 语义。

---

## 1. 已实证的现状（2026-08-12 真实运行数据）

### 1.1 AOT 中间层：健康，仅 1/242 失败（推翻旧假设）

用预编译 AOT product 跑 fact（`RunFactAll`，遍历 subject，`ChaosDispatchMethodGetValue` 比对实际返回值）：

| Assembly | fact 结果 |
|---|---|
| System.Linq | **157/157** |
| System.Private.CoreLib/buffers | **23/23** |
| System.ObjectModel | **10/10** |
| System.Collections | **51/52** |
| **合计** | **241/242（99.6%）** |

**结论 A**：`kChaosExternalRuntimeFnTable` / dispatch / interpreter 路径对 **subject 方法** 健康。我此前「普遍 fallback return-0 / 落解释器」**错误，已推翻**。

**结论 B（1 个真实 failure）**：`System.Collections` index96 = `...CollectionsUtilTests::CreateCaseInsensitiveHashtable_1_int_0` **返回 0、checksum 失败**。它在全源码树**无 C++ 函数体定义**、`Hashtable`/`CollectionsUtil` **无 native stub**、但注册了 `kHotpatchKeepNative`。→ fallback `return 0` 路径**真实存在**，但**罕见**（1/242），是定点正确性缺口，非全局面板。

**结论 C（checksum 语义）**：subject 返回的是**哨兵 42/0**，`42` = 方法完整执行到返回标志，`0` = 返回 0（可能是合法 void/零值，也可能是 fallback return-0）。fact 能抓"崩溃/未执行到末尾"，但**不能保证复杂跨 assembly callee 的字节级正确**（42 哨兵会掩盖 `Buffer.Memmove` 是否真拷贝正确）。

### 1.2 寄存器分配：已达 x64 硬件极限

`CHAOS_IL2CPP_CODEGEN_STATS` 实测 1091 方法：GPR load 命中 **97.5%**、spill 623、write-through 7268（arg-reg 基线）。x64 持久池 = R12-R15（+Win64 RDI），RAX/RCX/RDX 被 Div/Mul/Shift 隐式 clobber、RBX/RSI/RBP 架构保留 → **扩持久预算无余量且非主导**。

### 1.3 T4.1 基准可信度：已由既有 harness 覆盖

adaptive + outlier + FP-12 零时长守卫 + `_MIN_ELAPSED_FLOOR` + perf-store overwrite 修复均已就绪。

---

## 2. 已完成（本 session 交付）

| Commit | 内容 | 验证 |
|---|---|---|
| `7e69de781` | 方案3 捕获原语下沉 `PalCaptureReliable()` + JIT `kReliableRegisterCapture`；方案4 deopt 精确 spill `RecordDeoptValues`（union live@deopt+osr）统一 5 deopt 点 | test_jit_native 70/70 + osr/gc/abi/pgo/seh/simd/inline/mode/unwind/il_smoke 全绿 |
| `27e7cf718` | 性能线记录：x64 持久寄存器扩无余量、T4.1 已覆盖 | — |
| `6d64ef994` | 消除中间层**调研**（真因初判 = 跨 assembly callee 落解释器） | 文档 |
| （本次） | **实证修正**：241/242 通过，推翻初判 | fact 实测 |

---

## 3. 方向 A 的重定位（核心修正）

### 3.1 原假设（已推翻）
> "跨 assembly InternalCall 普遍落 `ChaosExternalRuntimeFallback` → 解释器 → 2.5-30× 普遍。"

### 3.2 真实现状（实证）
- subject 方法走 native direct_ptr，中间层健康（241/242）。
- fallback return-0 只落在 **「无 C++ body 定义 + 无 native stub」** 的少数方法（已抓到 1 个：`CreateCaseInsensitiveHashtable`）。

### 3.3 方向 A 的价值修正
从「普遍性能面板」→ **「定点正确性 + 性能修复」**：

> **让每个「有真实调用需求、但 codegen 没产出 body / 没有 native stub / 落到 fallback return-0」的方法拥有真实 native 执行体。** 对已正常的方法，方向 A **无改动、无回归**；对有缺口的方法，消除错返回 + 获得真实执行/性能。

### 3.4 需要先回答的关键问题（决定优先级）
**这类失败方法在真实 workload 里有多少？**
- 现在只有 System.Collections 抓到一个。需要 **fd-verification fact 覆盖率报告** 跨全部 20 个 entry 盘点「返回 0 但预期非 0」的方法= 方向 A 的**定点清单**。
- 若清单小（如 < 十几），方向 A 是低量定点修复；若大，才值得做机制层（如扩 ShapeRegistry / 编译闭包）。

---

## 4. 新工作计划（重新打分优先级）

> ⚠️ 优先级调整：原「消除中间层」不是宽泛优化，而是**定点正确性修复**；其价值取决于 §3.4 的清单规模。

### 优先级 P0 — 定盘点法（最高，先做，数据驱动一切）
**任务 A1**：跨全部预编译 entry（20 个）跑 `--fact-json`，盘点「返回 0 但预期非 0」的方法清单。
- 产出：`docs/dev/in-progress/jit-regalloc-a2/external-fallback-gap-report.md`（方法 + 所属 assembly + 是否无 body + 是否无 native stub）。
- 验收：给出方向 A 的**完整目标清单 + 规模**，据此决定走定点还是机制。

### 优先级 P1 — 定点方向 A（依 A1 清单，分批）
**任务 A2**（浅层，便宜）：对清单中**有 native 需求但缺注册**的方法，在 ShapeRegistry 补 SimpleForward（类似 `RuntimeHelpers::InitializeArray`）。如 `Buffer::Memmove` 已证有 `ChaosBufferMemmove` stub、ABI 可核对。
**任务 A3**（深层）：对清单中**无 C++ body 且无 stub** 的方法（如 `CreateCaseInsensitiveHashtable` 需要 `Hashtable` 语义），判断是「补 codegen 让方法进编译闭包」还是「补 native stub」。这决定是否值得做机制层。
- 验证：这些方法 fact 从失败转通过；相关 benchmark `dispatch_overhead` 减少。

### 优先级 P2 — 机制层（仅当 A1 清单足够大）
- 扩 ShapeRegistry shape 集 / 让跨 assembly callee 进 `_methodsBySubjectId` 编译闭包。机制已有（SimpleForward → direct native call），只是覆盖面和 ABI 核对工作量大。
- 依赖 A1 数据显示缺口的量与形态。

### 优先级 P3 — 寄存器线（已近硬件极限，仅保留）
- 无新 x64 动作。可选 ARM64 持久池（12 vs allocator 10），但测试盲区，非优先。

---

## 5. 待确认项 / 风险

1. **A1 清单规模**是方向 A 走「定点」还是「机制」的分水岭——**必须先量**。
2. **checksum 局限**：42 哨兵掩盖深层字节正确性。若要验证 `Buffer::Memmove` 真拷贝正确，需专门构造带非平凡 payload 的测试（fact 当前抓不到）。
3. 方向 A 修复需完整 AOT 重构建 + fact 回归（本环境只有预编译二进制，改 codegen 后需重建才能验证）。
4. 不回归保证：方向 A 只动「当前落 fallback return-0」的方法，不影响已正常路径。

---

## 6. 立即下一步（建议）

**执行任务 A1（定盘点法）**——用现有 20 个预编译 entry 跑 `--fact-json`，盘点所有「返回 0 但预期非 0」的方法。这一步不需要改任何代码、立即能做、直接决定方向 A 的工作量。跑完后给你一份带真实规模的缺口报告，再定 A2/A3 怎么排。

要我现在执行 A1 吗？
