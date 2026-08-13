# T7 世代写屏障压力验证 — 方案展开与深度风险分析

> 日期：2026-08-12
> 输入：plan-v6 investigations.md#B2 调查设计 + 本会话 M1(A2b) 实证（store-then-barrier 竞态已锁死 + young-GC 晋升不稳）
> 状态：**分析稿（未写代码）** —— 用户要求先展开问题/方案/风险点

## 0. 一句话定位
T7 的目标是"**一个可信的 detector：能证明世代写屏障不漏跨代引用（漏卡=UAF），且误报率可控**"。但它建在**已知不稳的 young-GC 晋升路径**之上，所以 T7 的设计本质上是：**如何在"被测系统本身有一条已知会漏的路径"的前提下，造出一个能区分『真漏卡』和『晋升不稳误报』的检测器。** 这是 T7 的全部难点，也是所有风险点的根源。

---

## 1. 问题展开：T7 到底在验什么

### 1.1 漏卡（UAF）机制（已实读 + M1 实证）
```
mutator: store old_slot[i] = nursery_ptr  →  [窗口A]  →  chaos_gc_dirty_card_dst_ref(old_slot, nursery_ptr)
                                                     ↑
                                               GC safepoint 可落此
young-GC Phase-1: 扫 dirty card → 该槽所在 card 未 dirty → 跳过 → 对象未晋升 → 回收 → 引用悬空
```
`_dst_ref` 屏障**纯函数**（gc_card_table.cpp:95-117，只读 GetRegionGen + DirtyCard，不碰槽值），store 与卡是**两个非原子操作** → 存在竞态窗口。这是 M1 已锁死的根因。

### 1.2 关键认知：T7 的 detector 必须区分两种"引用丢失"
| 丢失类型 | 机制 | 是否为屏障 bug |
|---------|------|---------------|
| **(A) 真漏卡** | 屏障该设卡但没设（或设错了 gen）→ Phase-1 看不到 | **是（T7 要抓的）** |
| **(B) 晋升不稳** | 卡设对了，但 young-GC 晋升路径本身丢对象（**M1 known-issue**：`chaos_gc_young_collector_test` gtest 2 测试 baseline 失败 / contracts 偶发 1 失败） | **否（既有 bug，非 T7 引入）** |

**T7 只要能区分 A 和 B，就成功；分不清，就会把 B 误报成 A（假阳性）→ 永远测不过 → 无法落地。**

### 1.3 现存 K2b 的两个真实缺陷（已实读）
1. **位置校验 ≠ 存活校验**（缺口③）：`gc_region_barrier_stress_test.cpp:127-148` 只检查引用"位置合法"（IsNurseryPointer / IsInOldGen），不检查"对象存活"。一个被回收的 nursery 对象地址在重置后仍 in-range → 假阴性（测不出漏卡）。
2. **单代方向**（缺口①）：只有 Old→Nursery，无 Gen1/LOH 交叉。
3. **无确定性漏卡激发**（缺口④）：靠概率命中竞态窗口。

---

## 2. 候选方案（3 个，思考完备）

### 方案 A：存活级 detector（investigations.md 推荐的 `GcVerifyCrossGenEdges`）
- 建四代（Nursery/Gen1/Old/LOH）交叉对象图，注册 static root 保证根都在。
- 每轮 full-GC 后，遍历所有跨代引用 `(src_slot → ref)`，用存活谓词断言 ref 存活：nursery→`IsNurseryPointer+region_gen==young`、old→`IsMarked`、loh→`IsLohMarked`、gen1→gen1 区间 + 非 0xFF/sentinel。
- 期望：**detector 干净时全 PASS**；对照组（跳过 barrier）必然 FAIL（self-check）。
- **主风险：它默认 A2b(晋升不稳) 已被修** —— 但 M1 证明**晋升路径不稳定（测试树里还有 baseline 失败的测试）**。所以真实跑起来会**把 B(晋升不稳) 误报成漏卡** → 测不过 → 暴露 M1 尚未修的事实。

### 方案 B：barrier 层单元级验证（不依赖 young-GC 存活）
- 绕过 full-GC 晋升，直接在**单元层**验证 `chaos_gc_dirty_card_dst_ref` 的**卡表写入正确性**：给定各种 `(dst_gen, ref_gen)` 组合，断言「该设卡的 card 被设为 dirty、该跳过的被跳过」——**验证屏障的决策逻辑本身**，不验证"对象存活"。
- 优点：不碰晋升路径 → **不踩 A2b 的坑**，确定性、可快速收敛。
- 缺点：**测不到"对象被正确晋升保留"这个端到端语义** —— only 证明屏障决策正确，不证明整个 barrier→scan→promote 闭环无损。

### 方案 C：组合方案（推荐）
- **B（屏障决策单元验证）作为基础 + 确定性基线**（证明屏障逻辑 100% 正确）。
- **A（存活级端到端）作为上层**，但**用「已知晋升不稳」关进去**：接受它在 M1 修复前会 FAIL，把它当作 **M1 的验收测试**（A2b 修好后必须全 PASS），而不是现阶段"必须绿的单元测试"。
- 关键：**给存活级校验加一个『B 类排除』机制** —— 区分"漏卡"和"晋升路径自身丢对象"，避免混为一谈。

---

## 3. 深度风险分析（按严重度）

### 🔴 风险 1：detector 假阳性 ——「晋升不稳(B) 被误判成 漏卡(A)」
**这是 T7 的第一大风险，也是我此前所有 M1 诊断的最直接应用。**
- 证据：`tests/unit/runtime-native` 的 `test_gc_young_collector` 2 个测试 `YoungCollectionEmpty`/`ConservativeSweepSelfRefs` **baseline 就失败**；contracts `chaos_gc_young_collector_test` kFull 下**偶发 1 失败**。
- 机制：这些是**晋升路径**问题（对象在 young-GC 中该留没留），不是屏障漏卡。如果一个存活级 detector 跑在这种 heap 上方，会把晋升丢的对象当成"漏卡"报 FAIL。
- **后果**：T7 永远测不过 → 无法收敛 → 重踩 M1 已归档的坑。
- **缓解**：方案 B 先行（不依赖晋升）；方案 A 明确标记为「依赖 A2b，验收 M1 用」，不混进需要绿的单元集。

### 🔴 风险 2：断言时机窗口 —— mark bitmap 存活期
- `IsMarked` 依赖 mark bitmap；`BgcCompact` 用 `clear_bitmap=false` 保留 bitmap（gc_old_gen.cpp:3172）。若断言在 bitmap 已被清/复用后跑，会把"已回收却未 0xFF"的 old 对象误判为漏卡。
- **缓解**：每次 `chaos_gc_collect` 返回后、任何新 mutation/GC 前，**立即**调 detector。但并发 worker 会抢先 mutation → 需要协调（safepoint 内或纯协调）。

### 🟠 风险 3：raw 载荷保守扫描的误判
stress 的 `OldMessage` 是 raw void*[128] 无 TypeInfo。存活级 detector 若对 raw 槽做保守扫描，会把**非指针字节**（碰巧落在 heap 范围）当引用 → 误报。
- **缓解**：载荷只用真指针槽，或让 Nursery 对象带有效 TypeInfo，或扫描时对 raw 槽保守跳过（接受覆盖率下降）。

### 🟠 风险 4：nursery 重置后地址仍 in-range 的代归属歧义
- 位置校验假阴性的根源：被回收的 nursery 地址重置后 `IsNurseryPointer` 仍返回 true。
- **缓解**：存活级判定必须叠加 `region_gen==young` + gen1_bump 边界，且**用 `IsMarked` 而非仅位置**。

### 🟡 风险 5：性能/时间
- 多线程多代 + 8-16 轮协调 GC + 每轮存活遍历 + 对照组，构建+运行耗时可能显著；`RESOURCE_LOCK bgc` 可能挂起（既有 bgc_race 悬挂 pre-existing）。

### 🟡 风险 6：`GcVerifyCrossGenEdges` 是新增 GC API
- 需在 gc_diagnostics.h/cpp 新增 + 打进 kFull 路径；审计/门禁要求真实读取点，否则 hook 拦截。

---

## 4. 我的主推荐（结论偏好）

**采用组合方案 C，分两步落地，先做 B 后做 A：**

1. **先做 B（屏障决策单元验证）** —— 不碰晋升路径，确定性、能快速绿、能证明 `_dst_ref` 的**决策逻辑 100% 正确**（该设卡/该跳过全对）。这是 T7 能落地的基石，也**不受 A2b 不稳影响**。
2. **再做 A（存活级端到端）** —— 但**明确把它定位为『A2b 的验收测试』**，现阶段根据 M1 未修的事实**预期它 FAIL**；等 M1(store-barrier 原子化) 修复后必须全 PASS。**不混进需要绿的单元套件**，而是归入 stress/验收。

**为什么不是纯 B 或纯 A**：
- 纯 B：只证屏障决策，不证端到端无损（漏需求）。
- 纯 A：直接撞上 A2b 不稳 → 永远红 → 重踩 M1。

**诚实的结论**：T7 的**期望价值（证明不漏卡）在当前 A2b 未修状态下无法兑现** —— 因为漏卡和晋升不稳在端到端存活检验里不可分割。**能现在落地、且不出假阳性的，只有 B（屏障决策），加上把 A 归档为 M1 的验收判据。**

---

## 5. 建议的落地步骤（若采纳组合方案 C）
1. **B**：写 `gc_dirty_card_decision_test`，枚举 `(dst_gen, ref_gen)` 全组合 + 越界/边界，断言卡表 dirty/clean 正确。纯确定性，预计 1 个 commit 可绿。
   → ✅ **已完成（2026-08-13）**：`tests/contracts/native/runtime-core/gc_dirty_card_decision_test.cpp` 落地，`chaos_gc_dirty_card_decision_test` ctest **PASSED**（9 元决策矩阵 + null-ref + null-dst 全绿）。顺带修复 `pal_eh_win32.cpp:31` 缺 `<cstdio>`（pre-existing 潜在编译 bug，`stderr`/`fprintf` 未声明）。
2. **A（归 stress/验收）**：写 `gc_region_multigen_stress_test`（四代交叉 + 存活级 detector + 对照组），标记 `LABELS stress`，**预期 A2b 修复前 FAIL**，作为 M1 复议验收项。
   → 待做（A2b=store-then-barrier 竞态，见 charter-hardstw-storecard-atomicity-2026-08-13.md）。
3. **注册**：B 进 unit；A 进 stress，二者隔离，A 不拖累 unit 绿。

---

## 6. 待用户拍板
你要求先展开风险。基于以上，我需要你定：
1. 是否**先做 B**（屏障决策单元验证，现在就能绿，不踩 A2b 坑）？
2. 是否把 **A（存活级端到端）归档为 M1 验收**（预期当前 FAIL，M1 修好后必须过）？
3. 还是你有别的优先级？

（本稿未写任何代码，工作区无改动。）
