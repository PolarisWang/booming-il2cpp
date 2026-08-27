# mark-hang 静态锁结构分析 — 交接后续执行 (task#16)

> 日期：2026-08-27 | 作者：GC expert（本会话）| 关联：`mark-hang-wct-handoff-2026-08-27.md`
> 目的：在不复现 ~2% heisenbug 的前提下，从源码锁结构刻划 hang 的真实机制，指导原生 WCT session 的判别与修复。

## 一、mark 期锁结构完整清点

**并行 mark 热路径（`DrainMarkStackParallel` / `ParallelMarkWorkerLoop`，gc_parallel_mark.cpp）**：
- 每个 worker 一个 `steal_mutex`，仅保护 `deque` push/pop/steal 临界区（`PushChunk`/`PopChunk`/`StealChunk`）。
- **无嵌套锁**：`StealChunk` 每次只对一个 victim 的 `steal_mutex` 取锁，O(3) 随机的 victim。
- `MarkObject`/`FindPage`/`MarkRange` 全部 **无锁原子位操作 / page_array_ 快照二分**，worker 不持阻塞锁。
- **终止协议**：`active_workers` 分布式计数 + `yield()` 自旋等待。last-idle-worker 设 `parallel_done`，设前调 `AnyWorkRemaining` 收敛复查（f7850324e 引入）。

**worker pool（`gc_worker_pool.cpp`）**：worker 空闲时 `cv_.wait_for(60s)` **阻塞**在 cv；`RunWorkers` 主线程 `yield()` 自旋等 `completed_`。`cv_.wait_for` 带 predicate，`round_` 原子 bump + `notify_all` → **predicate 保护，无丢失唤醒**（即使 notifier 不持 mtx_）。

## 二、判别链结论（关键）

**手off 观察：CPU=BLOCKED(delta≈3s) + WCT 0 wait-chain + watchdog 0 触发 + `S2_after_mark` 缺失。**

静态分析三组同步范式，逐一对照：

| 范式 | 阻塞/WCT可见 | 与观察一致性 |
|------|-------------|-------------|
| `steal_mutex`（steal/pop/push 临界区） | 阻塞可见，但临界区极短且无嵌套 | ✗ 不会形成 AB-BA 环 |
| `cv_.wait_for`（worker pool 空闲 park）| 阻塞可见，predicate 保护无丢唤醒 | ✗ 不 hang，且 round 变更即醒 |
| **`yield()` 自旋终止**（mark worker loop + pool 主线程）| **不可见**（无同步对象），过度订阅下可**调度 Livelock** | **✓ 100% 吻合** |

**根因指向：并行 mark 终止的 `yield()` 自旋在"6-8 进程 × 最高 worker 数"极端过订阅下退化为调度 Livelock** —— 所有 worker + pool 主线程都在 `yield()`，调度器在满负荷下反复重调度但无一获得足够连续 quantum 完成 `ProcessChunk→flush→fetch_sub`，`parallel_done` / `completed_` 长期不掷地 → 表现为：
- **WCT 0 wait-chain**（无锁/CV 等待可枚举，纯 yield 无同步对象）。
- **CPU 采样呈现 BLOCKED**（每个线程只在被重调度时消耗量,满过订阅下单线程量子被摊薄,3s 窗 delta≈3s）。
- **watchdog 0 触发**（全线程饿死,连 watchdog 的 `sleep_for(300ms)` 都被拖慢；且 `total_marked` 冻结时 watchdog 依赖自身被调度）。
- **早期 mark 段(600-1700 页,>kMinPagesForParallelMark=2)必经并行** → hang 位置在 `collect_start` 与 `S2_after_mark` 之间,完全吻合。

> 「BLOCKED 但 WCT 不可见」并不矛盾：yield-spin 在极端过订阅下不烧满单核，呈现为"线程存在但无进展"，被 CPU 采样工具读为近似 idle/BLOCKED，却被 WCT 判为 0 chain。这是调度饥饿，非共享锁环死锁。

## 三、修复方向（原生 session 验证后落地）

候选（按优先级，均需原生 WCT/CPU 复现确认后选）：

1. **给终止协议接 cv 事件（首选）**：把 `yield()` 轮询改为 `parallel_done` 的 `std::condition_variable` 通知。Mark worker loop 全部 `yield()`/idle 轮询点改 `cv_.wait(pred)` + 一个共享 `mark_done_cv_`/`mark_done_mutex_`。彻底消除调度 Livelock 窗口,`active_workers` 终局由 last worker `notify_all`。这与 S3-A bounded-wait 同构（`99191f128`）。
   - 风险：await 期间必须保证 `ProcessChunk` 产生的 steal 仍被探测——用 `wait_for(短间隔)` 混合轮询 + cv,或保持 steal_mutex 探测但只用 cv 做 done 通知。
2. **`RunWorkers` 主线程完成屏障** `yield()` → `cv_.wait_for(completed>=expected)`（pool 内已完成部分协作,主线程不应裸转）。
3. **降并行 worker 上限 / 关并行 mark 兜底**：`CHAOS_GC_ParallelMarkWorkers` 已可 latch。过订阅病态下强制顺序 mark（`DrainMarkStack`）可立即止损,代价是全 GC mark 延迟上浮。
4. （不推荐）保持 yield,仅在 `gc_worker_pool.cpp` 的 cv 对象给 mark 循环加一个活跃 worker 保底——复杂。

**静态判断**：方案 1 是根治;方案 3 是可立即合入的止血。原生 session 应先复现(确认 CPU 高/低)再选。

## 四、与 S1/S3-A/SEGFAULT 的边界

- S1/S3-A（已修,`4374ae23c`/`ac7b03eb6`/`99191f128`）是 BGC 状态机/协调死锁——锁/CV 可见,WCT 应能枚举。
- 本分析针对的**残余 ~2% 全 GC 早期 mark 段 hang** 是 `yield()` 调度 Livelock——WCT 不可见,这正是 WCT 报 0 chain 的直接解释。
- **WCT 对本次残余是盲的** → 原生 session 若仍报 0 chain + BLOCKED,勿再深挖 WCT 锁环,直接按方案 1/3 落地;若 WCT 报出 CriticalSection/Mutex 环,则另有真锁环(可能 S1/S3-A 残余或 `gc_card_table`/`bgc` 并行),按锁序修。

## 五、WCT 工具实证（本会话原生 build + 已知死锁负结果验证）已落地

> 时间线：2026-08-27 会话内完成 vcvars64 原生编译 + 两个合成死锁测试。

**工具已 build（原生 vcvars64 cmd 环境，绕过 MSYS bash 转义）**：
- `scripts/_build_wct.cmd`（批处理：`call vcvars64.bat` + `cl /EHsc wct_deadlock_spy.cpp`），产物 `scripts/wct_deadlock_spy.exe`（175KB，gitignored）。
- 经 `cmd.exe //c scripts\\_build_wct.cmd` 从 bash 调用可行；C4819 代码页 936 警告无碍。
- 性能验证：对 explorer.exe(15176) 枚举 389 threads，非挂起正确报 0 wait；用法/退出码正确。

**两个合成已知死锁的 WCT 负结果（决定性）**：
1. `std::mutex` AB-BA（MSVC /MT SRWLOCK 内部实现）：WCT 报 0 wait-chain。
2. **原生 `CRITICAL_SECTION` AB-BA**（`EnterCriticalSection` 内真阻塞，3 线程互持）：
   - `GetThreadWaitChain`（`WCT_OUT_OF_PROC_CS_FLAG`，synchronous session）对每个线程返回 **`ok=1 nodeCount=1`**——查询成功但**无待链**。
   - 即：**对 WCT 本该能走的 CriticalSection，even in-proc AB-BA 死锁，WCT 也不报。**
- → **WCT 的 "0 wait-chain" 不是工具 bug，不是"无线程阻塞"的证据，是本配置下 WCT 的根本性不可见**。

**结论（本会话 + 实证拼接）：**
- 手off 记录的 mark-hang "WCT 0 wait-chain" 现在有了**双重解释**：
  (a) 若 hang 确为并行 mark `yield()` 调度 Livelock（§二，WCT 完全不可见），则 WCT 报 0 是必然。
  (b) 即便 hang 是普通 `std::mutex`/CriticalSection 锁环，**本 WCT 工具也报 0**（实证）——因为 synchronous session + `WCT_OUT_OF_PROC_CS_FLAG` 无法枚举 out-of-proc CS 环。
- → **WCT 对该残余死锁是系统性盲的**。原生 session 不要再把产出押在 WCT 上。
- **替代判别**（下一原生 session 采用）：
  1. 复现时直接查 `S2_after_mark` 缺失 + `total_marked` 冻结 → 判 STUCK（yield-livelock 特征）。
  2. CPU 双采样：若 **HIGH**（每线程烧满）→ yield-spin 自旋活锁,直接按 §三方案 1/3。
     若 **LOW/BLOCKED** 且确认非调度-饿死 → 补真锁环证据,用 `cdb !cs / !locks`（挂起,仅确认锁序）而非 WCT。
  3. 终极进程内 `timed_mutex` 探针（thread_local 持锁栈 + try_lock_for 超时自报）抓锁序环（侵入,测完删,见 user 约束）。
- **WCT 保留价值**：仅对**真阻塞锁环**（Mut thread-wait / SendMessage / ALPC）等 WCT 可见对象有效；对 std::mutex/CS/HRW 环无效。doc 内标注其盲区,勿再单点依赖。
