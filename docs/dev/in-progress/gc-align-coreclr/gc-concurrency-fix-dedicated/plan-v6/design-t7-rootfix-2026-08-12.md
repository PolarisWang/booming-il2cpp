# T7 根治方案与深度分析 — store-then-barrier 竞态的根因修正 + 零假阳性压力验证

> 日期：2026-08-12
> 输入：本会话 M1(A2b) 已锁死根因（store-then-barrier 竞态，DIAG-2 scavenge_total=0）+ investigations.md#B2 + CoreCLR 对照（memory coreclr-region-barrier-solution-reference）
> 定位：**彻底修正根因 + 无假阳性的压力验证**（不是造 detector 区分漏卡）。含方案分析稿，未写代码。

---

## 0. 方向纠正确认
你否定了"造 detector 区分漏卡/晋升不稳"（方案 B/C）。正确方向 = **消除竞态根因 + 让压力测试天然零误报**。本文给出 3 个根本修复方案 + 零假阳性的验证设计。

---

## 1. 竞态根因的精确定义（M1 已锁死）

```
当前发射（LinearEmission.cs:84-89 / 267-273，三套一致）：
  [store]  slot = nursery_ref
  [card]   chaos_gc_dirty_card_dst_ref(dst, ref)   ← 设卡在 store 之后
```
窗口：GC safepoint 落在 `store` 与 `card` 之间 → Phase-1 扫到槽时 card 未 dirty → 漏 → 对象死。

**根因 = 「已 store 的槽，其 card 可能在 Phase-1 扫描时仍未 dirty」。** 根治 = 让「槽里的 nursery 引用 ⇒ 该槽 card 必 dirty」成为**在 Phase-1 时刻恒成立的不变量**；而当前 store-then-card 顺序**天然破坏**这个不变量。

---

## 2. 三个根本修复方案

### 方案 α — 预设卡：card-before-store【⚠️ 单独不根治，需修正认知】

**（重要诚实修正 2026-08-12：本节推翻早先"α 即根治"的结论。深挖后 α 单独不足。）**

改发射顺序 card-before-store：
```
  BgcSatbPreWriteBarrier → [card] chaos_gc_dirty_card_dst_ref → [store] slot=ref
```

**为什么单独不足（决定性机制，thread_state.cpp:274 确认）**：
- `_dst_ref` 是 `extern "C"` **真函数调用**（非 inline），store 与 card 之间存在**调用边界**。
- satpoint 对 `tls_forbid_suspend_depth>0` 的线程**ack 后直进（不 wait）**（thread_state.cpp:274）——即 GC 可在 mutator 仍在 write-barrier 中间态时进 Phase-1。
- α 把窗口搬到 card 与 store 之间：若 GC Phase-1 落在 [card] 和 [store] 之间，card 已 dirty、槽仍是**旧值**→ Phase-1 没看到新 ref → 未晋升；随后线程 store ref → nursery 对象只被此槽引用 → nursery reset 回收 → **槽悬垂。同样 UAF，只是镜像到 store 另一侧。**

**结论**：顺序（α）单独**不能根治**。真正决定安全的是**「store 与 card 之间无可挂起点」**（CoreCLR 硬 STW 在安全点挂起，barrier 内无 poll）。Chaos 的 ack-and-continue 恰恰破坏了这个保证。

---

## 2b. 【修正后】真正的根本修复（二选一）

### 路径 β' — 硬挂起在安全点（对齐 CoreCLR，最彻底）
- young-GC 用**真停线程**：只在安全点（无 write-barrier 中间态）挂起所有 cooperative 线程，Phase-1 无并发 mutator。
- 决定性：**Phase-1 开始时，没有任何线程处于 store 与 card 之间**。
- 代价：pal 未接真挂起原语（`pal_preempt_win32.cpp:52`）；成本最高。"最先明白最彻底"。

### 路径 γ' — barrier 感知的 safepoint（改 wait 语义，本仓库可行度最高）
- 让 safepoint 对处在 write-barrier 临界区（`tls_forbid_suspend_depth>0`）的线程**正确地"等它退出临界区再开始 Phase-1"**，而不是 ack-and-continue 直进。
- 具体：Phase-1 前，协调器**必须等到所有线程都由 `tls_forbid_suspend_depth` 回到 0**（即都真正完成了各自的 store+card，处于安全点）。
- 这等价给 write-barrier 一个"GC 排他"语义：**Phase-1 只能在无在途 barrier 时开始。**
- 配合：把 store+card 包进一个**会让 safepoint 等待其完成的临界区**（真正原子化，而非 ack-and-continue）。
- 代价：改 safepoint wait 协议 + 识别在途 barrier；侵入协调器/safepoint，需防死锁。

### 路径 δ' — 双阶段 scan（兜底，非根治但能做对）
- Phase-1 扫 dirty cards 后，**在真正全部 mutator parked 的时刻**再重扫一次全部 dirty cards（第二次 transfer-drain）。
- 但因 ack-and-continue，"全部 parked"不保证 → 需配合 γ' 的 wait 语义才能真正对。
- **不是独立根治，是 γ'/β' 的兜底。**

---

## 2c. 【修正后的对比】

| 方案 | 根治竞态 | 对齐 CoreCLR | 成本 | 假阳性 |
|------|---------|-------------|------|--------|
| β' 硬 STW 安全点 | ✅ 彻底 | 🏆 最贴原文 | 最高 | 零 |
| γ' barrier-aware safepoint | ✅（同本文 4 的不变量） | ✅ | 中-高（改 safepoint） | 零 |
| α card-before-store（单独） | ❌ 不根治 | — | 中 | — |

**诚实结论（重要）**：我此前的 α 方案有分析缺陷——card-before-store **单独不能根治**，因为它没消除"safepoint 可落在 card 与 store 之间 + 该 ref 未晋升即被回收"的窗口。真正根治必须**让 safepoint 在 write-barrier 临界区前等待（γ'）**或**硬挂起在安全点（β'）**（即 CoreCLR 的"安全点不落 store/card 之间"）。

**如仍要走 α**：必须把它**升级为 γ'**（α + barrier 临界区 wait 语义），否则它只是把一个确定漏的窗口换成另一个确定漏的窗口。


---

## 4. 零假阳性的正确压力验证（α 落地后）

**前提**：α 治愈屏障窗口后，"屏障漏卡"假阳性归零。**但晋升路径 (A2b) 是独立 bug**（young_collector gtest 2 测试 baseline 失败），它会让"young 对象被错误回收"——这不是漏卡，但同样使存活断言失败。**要真零假阳性，必须把 A2b 也收到可信（或验证设计隔离它）。**

### 验证设计（四层隔离，杜绝假阳性）
1. **持根法**：所有用于跨代引用的**源对象**（OldMessage/Gen1/LOH 载体）都经 `GcRegisterStaticRootRange` **强持根**，保证它们前一轮 GC 存活——使断言的失败**只可能**来自"屏障漏掉某条跨代边"或"晋升丢对象"二者之一，非重根。
2. **双根对照（A2b 隔离）**：对每条待验的跨代引用，同时**额外经一个强根**持有其目标 young 对象。若 GC 后目标仍死 → **不是屏障问题（卡对了、根也给了，只是晋升路径丢）→ 明确归因 A2b**；若目标因"仅经屏障边"而活（去掉额外根那份对照）→  证明屏障边起了作用。**这是把 A2b 和漏卡在结果上解耦的关键**。
3. **存活级 + 代归属双重判定**：断言用 `IsMarked`（old/loh）+ `IsNurseryPointer∧region_gen==young`（nursery）+ gen1 区间，杜绝"已亡但地址仍 in-range"的假阴性。
4. **对照组自证**：一列跳过 `_dst_ref` 的约束场景，断言 detector 必报失败（self-check：证明测试真能捕捉漏卡），但**用独立 heap 隔离**不污染主断言。

### A2b 的现实约束（诚实）
- 若 A2b 是真独立 bug（非屏障子集）：在 α 落地上后，只要晋升不稳，压力测试仍偶发红。
- **两个正确处理**：(a) 让验证只断言"屏障边保持的引用存活"且目标经双根（从而 A2b 影响被隔离/归因），(b) 或把 A2b 一并纳入本轮根治（见下）。
- **关键判断**：M1 DIAG-2 `scavenge_total=0` 说明 stress dangling 是屏障 window（card 从未 dirty）；若 α 修复后 stress dangling 归零，**证明 A2b 在 stress 场景下就是屏障 window 的子集**，则完美收敛。gtest 的 2 个失败（forwarding bit）可能是另一个 scavenge 枝，需 α 后复测定位。

---

## 5. 主推荐 + 落地路径（结论）

**诚实修正**：不能单纯做 α（card-before-store 单独不根治）。必须走**路径 γ'（barrier-aware safepoint）** 或 **β'（硬 STW 安全点）**。

**我的主推荐（本仓库现实度优先）= 路径 γ'**：
- 让 young-GC 的 safepoint **真正等待所有 write-barrier 临界区完成**（Phase-1 开始时无在途 store/card 中间态），这等价于 CoreCLR 的"安全点不落 store/card 之间"。
- 实现核心：Phase-1 前，协调器保证 `tls_forbid_suspend_depth` 全部归 0（或 barrier 提供"我已完成"的 ack 提交语义）。
- **可叠加 α**（card-before-store）作为收紧，但绝**不能**仅 α。
- 之后零假阳性压力验证（第四节）才成立。

**落地顺序**：
1. **γ'**：改 safepoint 为"Phase-1 前等待所有 barrier 临界区退出"（识别在途 barrier + 协调器 wait），防死锁。
2. 复测重构的压力测试 20x——验证 dangling 归零 + 零假阳性。
3. β'（硬 STW）作为远期最彻底项，本仓库 pal 未接，单独立项。

**为什么**：γ' 是在 Chaos 现有 ack-and-continue 基础上**修正其缺陷**（把"ack 就算安全点"改为"ack 且无在途 barrier 才是安全点"），成本低于硬 STW、且同本文不变量，能真正达成"从根本修正 + 零假阳性"。


---

## 6. 已知缺口与请确认
1. **A2b 是否独立于屏障**：需 α 落地后实证（DIAG 倾向"stress 下是子集"，gtest 需复测）。若独立，需单独立项（深 GC 手术）。
2. **α 的跨层改造成本**：三套发射 + `.generated.cpp` regen + snapshot——是本方案主要工作量，需确认可接受。
3. **β（硬 STW）**：不首选，但若你倾向最贴 CoreCLR 原文，可立项为专门 session（成本最高）。

（本稿未写代码，工作区无改动。此为根治方案分析稿，待你定方案后再实现。）
