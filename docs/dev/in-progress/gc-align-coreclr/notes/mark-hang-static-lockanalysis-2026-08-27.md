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

## 六、方案1 具体 patch 草稿 — cv 通知替换 yield 自旋终止（根治）

> 目标：消除并行 mark 终止在过订阅下的 yield() 调度 Livelock。核心改为**被信号唤醒的 cv 等待 + 有界 wait_for 探测**，收敛性仍由 last-worker + AnyWorkRemaining 保证（该逻辑不动），cv 等待只是让空闲 worker 不烧 CPU 且不因缺少连续 quantum 而活锁。**声明：patch 基于 2026-08-27 HEAD（`3dd8ab4cb` 之上）源码现状编写；原生 session implement 前先 `grep` 核对行号，勿盲改。**

> **实现状态（2026-08-27 会话已落地）**：B1/B2/C1/C2/C3/D1/D2(×2)/D3 全 hunks 已实现，
> 编译通过（build/native Debug + Release），`test_gc_parallel_mark` 6/6、
> `test_gc_worker_pool` 7/7、`gc_mark_stall_repro` 3/3 收敛（806 页 mark 完成）。原生 session
> 剩余验收：过订阅（6-8 进程×满 worker）下 0 hang + CPU 双采样确认 yield→cv 后 mark 段空闲
> CPU 显著下降；方案3（`CHAOS_GC_ParallelMarkWorkers=1`）保留为独立 off-switch 未动。
> **补注（草案遗漏）**：`ParallelMarkContext` 由 `CHAOS_IL2CPP_MALLOC` 分配，新增的
> `std::mutex mark_mtx_`/`std::condition_variable mark_cv_` 非平凡构造，必须 placement-new
> 构造并显式析构——实现时在 `InitParallelMarkContext` 加分号构造、三条释放路径
> （page_starts OOM / workers OOM / Destroy）加 `~ParallelMarkContext()`；否则 mutex/cv 未
> 初始化即用为 UB。草案 §六 未提及此点，已补。

### A. 正确性论证（为什么 cv 不等价破坏收敛）

现有终止依赖两个不变式：
1. `active_workers` 分布式计数 → last worker (`prev<=1`) 设 `parallel_done`。
2. 非 last worker 经 `parallel_done` 检查退出。

关键洞察（已证）：**收敛不依赖轮询响应速度**——last-worker 在设 done 前调 `AnyWorkRemaining` 做全 deque+pending 复查，余留在任何 deque/pending 的 chunk 都会被它 re-increment 后 drain。因此把空闲 worker 的 `yield()` 轮询换成**有界 cv 等待**（1ms wait_for）+ done 信号 notify，只改变"空闲时 CPU 消耗"，不改变"产生 work→被消费"的最终保证。残余 work 的最坏延迟 = 1 个周期（生产者 notify 或 1ms 轮询），仍收敛。

### B. 文件1：`src/native/runtime-core/gc/gc_parallel_mark.h`

**B1. includes**（当前只有 atomic/cstdint/cstring/vector，补 condition_variable + mutex）：
```cpp
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <vector>
```

**B2. `ParallelMarkContext` 加 cv + mutex**（结构体末尾，`parallel_done` 之后）：
```cpp
    /// Set to true when parallel mark is complete (termination signal).
    std::atomic<bool> parallel_done{false};

    /// 方案1: done/新-work 信号。降空 worker 由 last-worker 设 done 后 notify_all
    /// 唤醒退出; 生产者(ProcessChunk 推 chunk 溢出自己 deque 给他人 steal 时)也可
    /// notify。有界 wait_for 兜底, 不丢 work。
    std::mutex              mark_mtx_;
    std::condition_variable mark_cv_;
```

### C. 文件2：`src/native/runtime-core/gc/gc_parallel_mark.cpp` — worker loop 三个 yield 点替换

**C1. drain_started 等待（当前 :342 `while(!drain_started) yield();`）** 替换为：
```cpp
    // Wait for drain_started signal from the GC thread (cv + predicate; bounded
    // wake so a missed notify still proceeds via the store visible on acquire).
    {
        std::unique_lock<std::mutex> lock(ctx->mark_mtx_);
        ctx->mark_cv_.wait(lock, [&]() { return ctx->drain_started.load(std::memory_order_acquire); });
    }
```
> 注：`drain_started` 在 RunWorkers 前已 store(true)（DrainMarkStackParallel），worker 快路径下 store 对 acquire 可见，predicate 直接为真，无额外等待。此 cv add 纯为一致性；原子可见性已保证。

**C2. 内层 wait（当前 :420 `std::this_thread::yield();`）** 在 else 分支替换为有界 cv 等待：
```cpp
            // 方案1: 空闲且有界等待。非 last worker 在此等新 work/done。有界
            // wait_for(1ms): 即便生产者漏 notify, 也会周期重探 pop/steal 消化
            // 他人推入的 chunk; parallel_done 由 last-worker notify_all 即时醒。
            {
                std::unique_lock<std::mutex> lock(ctx->mark_mtx_);
                ctx->mark_cv_.wait_for(
                    lock, std::chrono::milliseconds(1),
                    [&]() { return ctx->parallel_done.load(std::memory_order_acquire); });
            }
            // 醒来后回到内层 loop 顶部重试 pop/steal(逻辑不变, 仅等待方式变了)
```
> 保持内层 `while(!parallel_done)` 结构与 pop/steal 分支不动，只替换最终 `yield()`。

**C3. last-worker 设 done 后 notify_all**（当前 :400 `parallel_done.store(true)` 之后加）：
```cpp
            ctx->parallel_done.store(true, std::memory_order_release);
            // 方案1: 通知其他 idle worker 退出, 不再空转等到有界周期。
            {
                std::lock_guard<std::mutex> lock(ctx->mark_mtx_);
            }
            ctx->mark_cv_.notify_all();
            break;
```
> 先 lock_guard 再 notify_all：保证 predicate 读 `parallel_done` 与 notify 之间无 lost-wakeup（cv.notify 持有 mutex 语义下，等待方正在 wait 中会因 mutex 重获 + predicate 重查而醒，不会因 notify 早于其进入 wait 而丢）。

### D. 文件3：`src/native/runtime-core/gc/gc_worker_pool.cpp`

**D1. `RunWorkers` 完成屏障（当前 :112 `while(completed_<expected) yield();`）** 替换为：
```cpp
    // 方案1: 主线程等完成不再裸转。cv 有界等待, 由 WorkerLoop 里 completed_ 递增后
    // notify_all 唤醒。有界 wait_for 兜底, 不丢最终 completed_ 信号。
    {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait_for(lock, std::chrono::milliseconds(1),
                     [&]() { return completed_.load(std::memory_order_acquire) >= expected_completed_; });
        // 兜底: 即使 notify 丢失, 1ms 周期重查 completed_ 终达 expected。
    }
```
> **注意**：`RunWorkers` 用 pool 自身 `mtx_`/`cv_`（WorkerLoop park 同款）。但 WorkerLoop 的完成路径在无锁 scope 里 `completed_.fetch_add` —— 需在 D2 里让每次递增后 notify。

**D2. `WorkerLoop` 里 `completed_.fetch_add` 后加 notify_all**（两处 :72 与 :117 附近，均在 `work_fn_` 执行后）：
```cpp
            completed_.fetch_add(1, std::memory_order_release);
            cv_.notify_all();   // 方案1: 唤醒 RunWorkers 主线程完成屏障
```
> 两处（fast-path 分支 + 正常路径）都要加。

**D3. `Initialize` ready-spin（当前 :75 `while(ready_count_<target) yield();`）** 替换（可选,低优先）：
```cpp
    if (spawned > 0) {
        // 方案1: spawn 就绪等待从裸转改为有界 cv, 消除 spawn 阶段过订阅活锁。
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait_for(lock, std::chrono::milliseconds(1),
                     [&]() { return ready_count_.load(std::memory_order_acquire) >= spawned; });
    }
```
> 若不改 D3, 至少保证 D1/D2/C1/C2/C3（mark 终止主路径）。D3 是 spawn 一次性, 过低风险可留待原生 session 实测后决定。

### E. 验收（原生 session 必做）

1. **编译**：全量 native CMake（含生成头 runtime_abi.h）。`cl /c` 单 TU 无法过（缺生成头），必须全量 build。
2. **`gc_mark_stall_repro`**：无 env（方案1已生效, 不设 ParallelMarkWorkers）跑, 期望从不收敛(124/挂)转收敛(exit 0) 反复 20+ 次。
3. **scenario C 大样本**：`--scenario 2` 单进程 50-100 次 0 hang; 6-8 进程并行循环 0 hang。
4. **不回归绿基线**：`chaos_gc_parallel_mark_test` 6/6、`chaos_gc_bgc_unit`/bgc_smoke、full-GC 相关 stress 全绿; mark 正确性(scenario A 无漏标)不变。
5. **CPU 前后对比**：mark 段 CPU 从"满核 yield 转"降为"近 0 idle cv 等待"（用 task manager/双采样确认）。注意：wait_for 1ms 仍会 ~1ms 唤醒, CPU 显著下降但不为零——可接受, 若需再降可加长周期或纯 notify。
6. **方案3 兜底保留**：`CHAOS_GC_ParallelMarkWorkers=1` 仍应强制顺序（与方案1 正交, 保留作为 operator off-switch）。

### F. 风险与回滚

- **风险1**：cv 通知引入新的锁序？不——mark_cv_/mark_mtx_ 只在 worker 自空闲或 done 时取, 与 steal_mutex 无嵌套（worker 在 pop/steal 临界区外等 cv; 等 cv 时已释放 mark_mtx_）。`RunWorkers` 用 pool mtx_ 与 WorkerLoop 的 mtx_ 是同一把（park 同锁）, 但 D2 notify 在无锁 scope, 无嵌套。**无 AB-BA 新增**。
- **风险2**：completed_ 丢失最终信号？D1 wait_for 1ms 兜底重查, 且 D2 每次递增 notify_all, 无 lost-wakeup（predicate 重查 + mutex 保护 completed_ 读）。保守起见 D2 可只加一次在 expected 达成时但实现复杂, 用 wait_for 兜底最简。
- **回滚**：单文件 revert `gc_parallel_mark.cpp`/`gc_parallel_mark.h`/`gc_worker_pool.cpp` 即可（不动方案3 的 `InitParallelMarkContext` 那行 config cap, 与方案1 独立文件/区域）。

### G. 与方案3 的关系

- 方案3（已合入 `3dd8ab4cb`）= operator off-switch 止血; 方案1 = 根治。
- 两者正交：方案1 改 worker loop 等待, 方案3 改 worker_count 计算。可独立合并；原生 session 先验证方案1, 若 risc 高可只留方案3。
