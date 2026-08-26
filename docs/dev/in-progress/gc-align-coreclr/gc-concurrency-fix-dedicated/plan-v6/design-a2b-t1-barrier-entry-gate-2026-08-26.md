# A2b 阶段1 可执行方案 — 关闭 barrier_inflight drain TOCTOU 窗（γ' refined）

> 日期：2026-08-26 | 依据：内容校验测试确定性复现 142/1024 UAF + 实读 `thread_state.cpp` forbid_suspend.h | 状态：**待执行**

---

## 一、问题（已确诊，非推测）

`content-check` barrier stress（nursery 对象 typed：TypeInfo@0 + self-pointer@8 + magic@16）4/4 稳定复现
**`magic-miss=142/1024, region-invalid=0`** —— 地址仍 GC-managed（位置校验全绿）但目标被回收复用（真 UAF）。

机制 gap 已实读确认：

`RequestGlobalSafepoint`（thread_state.cpp:429）collect 阶段：drain 循环（:496-524）用 `EnumerateThreads`
计 `suspend_ack != epoch 或 barrier_inflight != 0` 的线程，`s_remaining==0 → break`（:524）。**这是点态快照。**

`BarrierCriticalSectionScope`（forbid_suspend.h:104-115）构造函数**无条件**置 `barrier_inflight=1`
（不查 `suspend_seq` 是否已激活）。

**⇒ TOCTOU 窗**：coordinator `break` 之后、young-GC Phase-1 扫描开始之前，一个 worker 进入一个新的
`BarrierCriticalSectionScope`（inflight 0→1）。该 store 的 card 不在 drain 已扫视野内 → Phase-1 扫到该旧页
时 card 仍干净 → 跨代边丢 → 目标被回收 → slot 过期（UAF）。

量与 142/1024 吻合：每 worker 在 request→Phase-1 间隙完成的 store 数 ≈ 一次 GC-cycle 的量。

---

## 二、CoreCLR 对照（为什么这该修）

CoreCLR 写屏障是**无 poll 的 leaf**（`JIT_WriteBarrier_Byte_Region64`：纯 mov/cmp/shr/ret，无 call、无 poll）。
GC 只在 safe point 挂起 → 线程要么完整完成 store+card，要么未开始，**绝无中间态窗口**。

Chaos 发射是 `call chaos_gc_dirty_card_dst_ref`（call 边界 = 潜在 poll 点），故需**显式临界区**
（barrier_inflight）+ **coordinator 保证 Phase-1 开始时无在途 barrier**。当前实现只有"drain 当前在途"，
缺"阻止 request 后新开"。这就是与 CoreCLR 的差距。

---

## 三、修复设计（γ' 阶段1：二层防窗）

目标：**Phase-1 扫描开始时，任何已 store 的槽其 card 必已 dirty**；窗口缩小到零。

### 方案 A（推荐，纯运行时，无 codegen/JIT 改动）

**让 drain 覆盖"request→Phase-1 全窗"，并让 barrier 进入对已激活 safepoint 变"阻塞前完成"**：

1. **`BarrierCriticalSectionScope` 构造（forbid_suspend.h:106-110）**：进入前先读 `suspend_seq`。
   - 若 `suspend_seq == 0`（无 safepoint）：照常入区（inflight=1）。
   - 若 `suspend_seq != 0`（safepoint 已激活/请求中）：立即 `SafepointPoll()` 一次（ack + 若已非 fordid 则
     等待事件），**直到 seq 复位或本线程已 ack 才入区**。这样"已 store 未 card"的线程要么在 drain 之前
     入区（inflight 被 drain 看到），要么在 safepoint 激活后先 poll 让出。
2. **coordinator drain 精化（thread_state.cpp:496-524）**：`s_remaining==0 → break` 仅当本轮**未观察到
   任何 inflight 穿越**；改改为 loop 直到连续两轮 `s_remaining==0`（即无新 inflight 进入 + 无未 ack），
   才确认稳定。这消除"最后一次 drain 后刚入区"的单轮误断。

> **防死锁**：步骤 1 的 `SafepointPoll` 在 `tls_forbid_suspend_depth>0` 时不阻塞（现有语义），仅当
> barrier scope 退出、depth==0 时才真的等待；但步骤 1 是在**进入** barrier scope 前调用（此时 depth==0），
> 所以 SafepointPoll 会正确等待。协调器仍保留 bounded-wait + `PalPreemptRequest` APC/POSIX 兜底（已有）。

### 方案 B（备选，更彻底，改动大）

在 `BarrierCriticalSectionScope` 入口若检测到 safepoint 激活，则**优先完成当前 barrier 的早期部分**再 poll，
与方案 A 类似；不同在 coordinator 侧：drain 后加**一次同步栅栏**（`PalPreemptRequest` 强制所有 cooperative
线程到 poll，再确认 inflight==0）——更彻底但依赖 APC 可靠。

**推荐 A**：改动集中在 `forbid_suspend.h` + `thread_state.cpp` 两处、无 codegen/JIT/解释器改动、可单测、风险可控。

---

## 四、实现步骤（步骤序是硬依赖）

### Step 1：证明窗（可选，但推荐先做）
在 content-check 测试（临时）加日志：drain `break` 时刻 vs 第一个 `barrier_inflight` 穿越（0→1）的时间戳，
确认 142 个 UAF 都落在 request→Phase-1 窗。非必选，因机制已实读确认。

### Step 2：方案 A 的 coordinator drain 精化（thread_state.cpp:496-524）
把 `if (s_remaining == 0) break;` 改为**连续两轮 0 才 break**（加 `s_quiescent` 计数；任一轮 >0 则重置）。
不动 suspend_seq 设置/APC 兜底。

### Step 3：方案 A 的 barrier 入口 gate（forbid_suspend.h:106-110）
`BarrierCriticalSectionScope` 构造函数：若 `suspend_seq != 0` 则先 `threading::SafepointPoll()`（等待或 ack），
再入区。需 `#include thread_state.h`（check 无循环依赖）。同时保留 JIT leaf 路径（thread_state.cpp:803）的
inflight 语义——leaf 是内联汇编，入口若线程在 safepoint 激活时仍会执行，由 drain 第二层的双轮防住。

### Step 4：回归 + A2b 验收
1. 全量 GC 单测（old_gen/region/card_table/gen1/scheduler/events/bgc）无回归、无死锁（stress 20x 超时防挂）。
2. **content-check barrier stress：20x 全部 `magic-miss==0`**（对应用户的 definitive reproducer）。
3. 对照组：临时把 barrier 入口 gate 去掉 → 应重新出现 142（证明 gate 有效）。
4. codegen/JIT 路径：`test_driver.py --layer unit` 无 new FAIL（不涉及 emission，但仍确认）。

---

## 五、风险与护栏

| 风险 | 护栏 |
|------|------|
| Step 3 的 SafepointPoll 在纯 store+card 热路径开销 | 只在不常见路径（safepoint 激活时）触发；正常 seq==0 快路径不变 |
| 死锁（barrier 内再轮询）| barrier scope 内不放 poll；SafepointPoll 在 depth>0 不阻塞（现有语义）；bounded-wait + APC |
| drain 双轮仍漏（极窄）| 与 Step 3 门联合，双层覆盖；content-check 20x 是最终判据 |
| codegen/JIT 未包 scope 的 store | 阶段 1 的 drain（Step 2）+ 入口 gate（Step 3）在运行时兜住"未进临界区但已 poll 让出"——对齐 γ' 6 节"阶段 1 必须处理不在临界区的线程" |

## 六、验收门
- content-check barrier stress 20x：**0 dangling（magic-miss==0）**。
- 对照组（缺 gate）重新出现 142 → 证明修复有效。
- GC 全量单测 + 20x stress 无死锁/回归。

## 七、交接（如不立即做）
- 复现/验证工具 = content-check barrier stress（需临时重建，见 docs/gc-n6-liveness-findings-2026-08-25.md 五节。
  位置校验版保持快 gate 绿；content-check 版作为 A2b 验收专用、不入 gate，修好后可入。
- 精确定位点：`forbid_suspend.h:106-110`（barrier 入口）+ `thread_state.cpp:524`（drain break）。
