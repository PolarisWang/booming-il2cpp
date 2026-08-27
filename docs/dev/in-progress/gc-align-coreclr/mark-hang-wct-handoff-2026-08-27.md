# GC flakiness → mark-hang 全局死锁：WCT 专项交接

> 日期：2026-08-27 | 状态：主导 SEGFAULT 已根治；残余 mark-hang(~2%, 仅人为过载) 的全局死锁判别完成，捕获需原生 Windows 环境 + WCT 工具。
> 关联：task#16、`gc-n6-liveness-findings-2026-08-25.md`、`scripts/wct_deadlock_spy.cpp`。

## 一、已完成（全部 root-caused + 根治 + 验证 + push）

| 项 | 根因 | fix | 验证 |
|----|------|-----|------|
| S1 幽灵 CONCURRENT_MARK 死锁 | `StartBgcCycle` 只查 `IsBusy()`(phase!=IDLE) 不查 `bgc_running_`，无线程仍置 marking → young GC `PauseForYoungGc` 死转 | `StartBgcCycle` 加 `!bgc_running_` guard(4374ae23c) + 线程退出/Stop 清 marking(ac7b03eb6) | scenario C 6-10/10、单测 10/10 |
| S3-A BGC 协调握手 | `PauseForYoungGc/Resume` 无限 spin | bounded 2s deadline + dead-BGC fast-path + force-stop(99191f128) | scenario C/L/bgc_smoke 绿、0 spurious |
| **SEGFAULT(主导崩溃)** | CrossPageCompact `RelocateRoots` **自栈扫描**裸读(2422)→读活跃栈帧下 redzone → 0xC0000005 | `AsanRead/WritePtrNoCheck`(3e020aa28)，所有 GcScanAllThreadRoots 回调 + RelocateRoots self-stack NoCheck | ASan 串行 96→0 错；normal 30/30 |

## 二、残余问题（本专项目标）

**session-C mark hang / 全局死锁**（~2%，仅人为 6-8 进程过载触发，正常负载 30/30 零崩溃零挂）：
- 复现：`./chaos_gc_stress_test.exe --scenario 2` 并行 6-8 进程循环；或单进程 ~50-100 次。
- 判别链（已刻划）：
  1. watchdog（独立 300ms 线程，fprintf stderr）**0 触发** → 全局线程饿死/死锁，NOT mark 局部发散。
  2. 外部 CPU 采样 = **BLOCKED**（3s 窗 delta≈3s）→ 非自旋，是共享锁/cv 阻塞。
  3. mark 看似 stall（`collect_start page_count=600-1700`、`S2_after_mark` 缺失）实为全 GC 冻住。
- 重框定：非"mark 不收敛"，是**全 GC 早段(600-1700 页)共享锁死锁**(AB-BA lock-order 或 cv 丢 wake)。
- 另：并行过载下偶见**稀有 Segfault**(更稀有, SEGFAULT 未完全消失)。

## 三、WCT 工具（scripts/wct_deadlock_spy.cpp）

**作用**：Windows Wait Chain Traversal 非挂起采样 target 进程每个线程阻塞在哪个同步对象 + 谁持有 → 定位 AB-BA 环。**不挂起 target**（不扰动时序，正对 BLOCKED 死锁）。

**Build（原生 Windows cmd 环境，非 MSYS-bash）**：
```
call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
cl /nologo /EHsc scripts\wct_deadlock_spy.cpp /Fe:scripts\wct_deadlock_spy.exe
```
**Run**：`wct_deadlock_spy.exe <hung-pid>` → 每线程 `TID <id>: blocked on <object> [type]` + CYCLE 标记。

**关键实现点**（已踩坑固化）：
- WCT 函数在 **Advapi32.dll**（`WINADVAPI` → Advapi32），非 WinSxS 的 WaitChain.dll/api-ms-set（本机无，LoadLibrary 失败）。
- `WAITCHAIN_NODE_INFO`：union `LockObject.ObjectName` / `ThreadObject.ProcessId/ThreadId`，无 ObjectId。
- session：`OpenThreadWaitChainSession(0, nullptr)`（synchronous）；`GetThreadWaitChain(handle,0,WCT_OUT_OF_PROC_CS_FLAG,tid,&nodeCount,nodes,&isCycle)`，`NodeInfoArray` 用 `WCT_MAX_NODE_COUNT(16)`。

**已知限制（专项要解决）**：对 hung 进程枚举 11 线程但报告 **0 wait-chain** → BLOCKED 死锁可能在 WCT 不可见对象（`std::mutex` 的 yield 自旋、或内部 CriticalSection WCT 未关联）。**下一步**：
1. 确认 hard hang 是 `std::mutex` 的 EnterCriticalSection（WCT 应可见）还是 `yield()` 自旋（WCT 不可见）——若是自旋，CPU 分类应显高(矛盾)，需重测 CPU 采样确认 BLOCKED。
2. 若 std::mutex 但 WCT 不报：用 `GetCurrentThreadId` 映射 → 或直接在死锁时对每个 blocked 线程 `NtQueryInformationThread(ThreadIsIoPending?)` + 外部 `!locks`（挂起, 仅确认锁序）。
3. 终极：进程内 timed_mutex 探针(thread_local 持有锁栈 + try_lock_for 超时自报) —— 侵入, 测完删(见 user 约束)。

> ## 2026-08-27 交接后续 session 补充（实证，见 `notes/mark-hang-static-lockanalysis-2026-08-27.md`）
> **WCT 工具已原生 build 且验证完成**（`scripts/_build_wct.cmd` + `scripts/wct_deadlock_spy.exe`），对 explorer 等枚举正常。
> **关键负结果：WCT 对本工具配置系统性盲** —— 合成 `std::mutex` AB-BA 与原生 `CRITICAL_SECTION` AB-BA 死锁，`GetThreadWaitChain`(synchronous + `WCT_OUT_OF_PROC_CS_FLAG`) 均返回 `ok=1 nodeCount=1`（0 wait-chain）。**↑ 上文步骤 1 的"WCT 应可见"前提不成立**：即使真阻塞锁环本 WCT 也报 0。
> **静态分析已刻划 hang 真机制**（§二）：并行 mark 终止的 `yield()` 自旋在 6-8 进程过订阅下退化为调度 Livelock —— 与 BLOCKED + 0 wait-chain + watchdog 0 触发 + `S2_after_mark` 缺失 全部吻合。
> **下一原生 session 不要再单点押 WCT**。判别改用：CPU 双采样(HIGH=yield-spin) → `S2_after_mark` 缺失+`total_marked` 冻结 → `cdb !cs /!locks`(挂起) 或进程内 timed_mutex 探针。修复推荐：方案 1(cv 通知替换 yield)根治 / 方案 3(降并行 worker 或关并行)止血。

## 四、专项建议执行顺序（原生 Windows 环境）
1. 修好 wct build/run 环境（vcvars + 非 MSYS），确认能对已知死锁(或构造一个)输出 wait-chain。
2. 复现 mark hang(并行 6-8 进程), 挂起时跑 WCT → 若报 CriticalSection/Mutex 环 → 直接定位锁序修。
3. 若 0 wait-chain → 判断是 yield 自旋(重测 CPU) 或加进程内 timed_mutex 探针抓锁序环。
4. 修锁序/补 cv 唤醒后, scenario C 大样本(100+)验证 0 hang。

## 五、相关文件
- `docs/dev/in-progress/gc-align-coreclr/notes/gc-n6-liveness-findings-2026-08-25.md`（完整判别链/SEGFAULT 根因）
- `scripts/wct_deadlock_spy.cpp`（工具源码，.exe gitignored）
- `gc_young_collector.cpp`/`gc_old_gen.cpp`(RelocateRoots)/`gc_bgc.cpp`（S1/S3-A/SEGFAULT fix 落点）
