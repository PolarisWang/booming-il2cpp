# roadmap-v1-01 — Threading 生产级就绪

> **任务 ID**：`threading-production-readiness`
> **上游设计**：`design-v1-01.md`（542 行，经 3 阻断级 + 6 需修正的对抗性审查并全部闭环）
> **日期**：2026-09-13
> **dispatch_model**：`sequential`

---

## 1. 目标

把 threading 子系统从**全仓库最不可信的一族**推到生产级开发品质。当前断裂在两头：

- **验证面**：三个 chunk 全部 `build.status=error`，28 个 native 测试文件（244 用例）零 CI 门禁
- **翻译面**：12 个原语缺 `extern "C"` ABI 出口，codegen 无法调用

内核（ThreadPool / TimerQueue / Monitor / Interlocked / WaitHandle）已是真实工业实现，
本路线图补的是「验证」与「翻译」这两条断链。

---

## 2. 范围边界

**做**：Phase 1（runtime 语义）/ Phase 2（codegen ABI 接线）/ Phase 3（加固）/ Phase 4（制度化）。
Phase 0（止血）作为前置批次。

**授权**：四层全开 —— native / codegen(C#) / 验证管线(Python) / CI 配置。

---

## 3. 非目标

| 排除项 | 理由 |
|---|---|
| **hotupdate slot mapping** | 用户拍板拆出独立任务；根因与 threading 无关，已有独立 handoff |
| **POSIX 平台缺陷** | 仅 Windows 目标；登记 watch_item |
| **Thread.Abort 语义** | .NET Core 已弃用该 API |
| **前缀过捕获** | 审计确认未发生，属误报 |
| **修复 `reporting.py:57`** | 读源树而非产物根是**验证管线自身缺陷，影响所有 chunk**；本计划只规避（T0.1），不顺手改 |

---

## 4. 阶段列表

| 阶段 | 内容 | 优先级裁决 |
|---|---|---|
| **Phase 0** | 止血：实测 build 错误、清假绿残留、重建 3 chunk、堵假绿向量、CI 接入 | — |
| **Phase 1** | 关闭语义造假：CancellationToken 3 点、Parallel、SynchronizationContext | P1 前提 |
| **Phase 2** | 接线 12 个**缺 ABI 出口**的原语（含 ABI 出口层 + 句柄映射机制） | P1 前提 |
| **Phase 3** | 加固：非原子 static、句柄表、热路径 fprintf、文档不符、benchmark | — |
| **Phase 4** | 制度化：门禁阈值、回归告警 | — |

**全局优先级裁决**：threading 是运行时基础设施，其正确性影响所有上层。
当前存在**会静默产出错误结果**的 P0 缺陷（`throw_if_cancellation_requested` 空体、
`Parallel` 恒返 -1）—— 性能（P1）的前提是语义正确，故 Phase 1/2 必须先于任何性能优化。
P3（HotUpdate）无冲突：本路线图不触碰 hotupdate 路径。
**无优先级冲突需裁决。**

---

## 5. 每阶段完成定义

### Phase 0 — 止血

- `goal`：三个 threading chunk 从 `build.status=error` 转为 `passed`，且验证信号可信
- `exit_criteria`：
  - 三个 chunk 在**产物根**产出 `fact.json`，`build.status=passed`
  - 产物根 `fact.json` 的 mtime **晚于**同目录 `provenance.json`
  - 源树陈旧残留已清除，honest report 不再出现来源不明的 PASS 行
  - CI workflow 跑出 **244** 用例结果（首跑可 `continue-on-error`）
- `deliverables`：
  - `artifacts/foundation-dll/System.Private.CoreLib/chunks/{threading,threading-2,threading-tasks}/results/fact.json`
  - `.github/workflows/threading-native-tests.yml`
  - T0.3 取证报告（`BuildMethodSourceSafe` 结论）
- `dependencies`：无
- `resolved_decisions`：
  - worktree 在 **T0.0 完成后**切出（design §3.1）；主工作区只跑 T0.0
  - 验收读取根**钉死为产物根**，不读源树（design §7.1）
- `watch_items`：
  - T0.0 实测出新错误类型（非 crt_stubs）→ Phase 0 形状重写
  - 重建后意外转绿 → **不等于**推断正确，必须回溯确认生效改动
  - 244 用例首跑失败分布

### Phase 1 — runtime 语义

- `goal`：消除会静默产出错误结果的 P0 语义造假
- `exit_criteria`：5 项各自的**反例验证**通过（in-place revert 后对应测试失败，无关测试保持绿）
- `deliverables`：`src/native/runtime-core/` 下 5 处修复 + 对应 native 测试
- `dependencies`：Phase 0（需要可信的 fact 基线）
- `resolved_decisions`：承袭 async-task-industrialization 的「反例验证」纪律
- `watch_items`：某修复被证明不承重时，显式标注冗余并回退

### Phase 2 — codegen ABI 接线

- `goal`：12 个缺 ABI 出口的原语可被 codegen 调用
- `exit_criteria`：T2.0/T2.1 建成；各原语注册后 **real% 显著提升**；新 ABI 符号经 native 测试覆盖
- `deliverables`：
  - `src/native/runtime-core/runtime_stubs/` 下新增 ABI 出口头/实现
  - 句柄映射机制（新机制）
  - `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs` 等注册条目
- `dependencies`：Phase 1
- `resolved_decisions`：
  - **用户拍板保留在 roadmap 内**（不拆出独立任务）
  - 定性为「**缺 ABI 出口层**」而非「未注册」—— `CancellationToken`/`Parallel`/`SynchronizationContext`/`Interlocked`/`Volatile` 已在 ShapeRegistry
- `watch_items`：
  - **开工前必须重新盘点** ShapeRegistry 状态（并发 agent 正在改 `Part1.S16.cs`）
  - 注册后 real% 不升 → 存在第二道断点，**不得**直接加大注册量
  - 句柄映射机制与 GC 的交互（托管实例 ↔ native 槽位的生命周期）

### Phase 3 — 加固

- `goal`：消除并发正确性隐患与热路径开销
- `exit_criteria`：5 项各自的验证通过；benchmark HTML 非空模板
- `deliverables`：`synchronization.cpp` 等修复 + benchmark 数据
- `dependencies`：Phase 2
- `watch_items`：非原子 static 的修复可能改变既有并发时序

### Phase 4 — 制度化

- `goal`：门禁防回归
- `exit_criteria`：threshold 配置生效；回归告警可触发
- `deliverables`：门禁阈值配置 + 告警接入
- `dependencies`：Phase 3
- `watch_items`：是否一并评估 `reporting.py:57` 的修复（design §7.1 遗留）

---

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| `T0.0` | 0 | **completed** | main | 实测 build 错误（**完整重建**，非读日志） | — | — | 主工作区跑，不切 worktree | 实测错误文本 + 错误类型判定 | ✅ 已完成：**非 crt_stubs**；C3861 声明缺口 + C3313/C3536 | 只读，无写出 | 中 |
| `T0.0b` | 0 | **completed** | main | 🔴 **补 `chaos_cancellation_token_*` 的 codegen 可见声明** | T0.0 | batch-0 | 定义/注册都在（`cancellation_token.cpp:255,289,297` / `S16.cs:1697,1731,1739`），**仅缺 header 声明**；参照 `runtime_stubs/threading_stubs.h` 既有模式 | `runtime_stubs/` 下新增声明头 | C3861 三个符号消失 | `src/native/runtime-core/runtime_stubs/` | 中 |
| `T0.0c` | 0 | **completed** | main | 🔴 定位 `chaos_result` 为 `const void`（C3313/C3536） | T0.0 | batch-0 | 与 cancellation **无关的第二缺陷**；疑似返回 void 的 SimpleForward 被赋给变量 | 根因定位 + 修复 | C3313/C3536 消失 | `src/managed/Chaos.IL2CPP.Generator/` | 中 |
| `T0.1` | 0 | **completed** | main | 清除源树陈旧假绿产物 | T0.0b | batch-1 | `rm -f` 显式列举，禁用 `git clean` | 三个 chunk 源树 `results/fact.json` 已删 | 源树下三路径均不存在 | `tests/e2e/translation/System.Private.CoreLib/chunks/threading*/results/` | 小 |
| `T0.2` | 0 | **completed** | main | 重建三个 threading chunk | T0.1 | batch-1 | 重建后记录构建耗时（判定单/双 worktree）；**必须读 `[CODGEN-FAIL] total=` 与 `kCodegenFailureCount`** | 产物根 `fact.json` × 3 + codegen 降级计数 | ✅ build `1/1`（threading 489s / threading-2 557s，均 <30min ⇒ 维持双 worktree）；`kCodegenFailureCount` 已查=0；**fact 比对跑出 2 个真缺陷**（见 STATUS） | `artifacts/foundation-dll/`（不入仓库） | 大 |
| `T0.3` | 0 | **completed** | main | 取证 `BuildMethodSourceSafe` 是否吞 threading 异常 | T0.0 | batch-1 | 独立取证，不依赖 T0.2 结果 | 取证报告 | ✅ 结论：吞（无过滤器 catch + 信号零消费者） | `src/managed/Chaos.IL2CPP.Generator/`（只读优先） | 中 |
| `T0.4` | 0 | **completed** | main | 新建 threading native test workflow | T0.2 | batch-2 | 独立 workflow，不进 ci-framework；首跑 `continue-on-error` | `.github/workflows/threading-native-tests.yml` | ✅ 交付；**并发现 ctest 从未发现过任何测试**（`enable_testing()` 缺失，Windows 0→300）；244 用例首次真跑 = **20/27 通过，7 失败** | `.github/workflows/threading-native-tests.yml`、`CMakeLists.txt` | 中 |
| `T1.7` | 1 | done | tbd | 🔴 **T0.4 首次真跑暴露的 7 个 native 测试失败**（此前从未被任何人看到） | T0.4 | batch-3 | 5 SEGFAULT + 1 失败 + 1 超时：`test_threading_benchmark` / `test_async_when_async` / `test_queue_backpressure` / `test_phase3_industrialization`（SEGFAULT）、`test_async_continue_with`（SEH 0xC0000005）、`test_async_when_each`（`NullElementStillTerminatesTheStream` 挂起 23s）、`test_threading_stress`（1800s 超时，需判定真死锁 vs 阈值过紧） | 7 项各自定位 + 修复或显式登记为 known-fail | 7 项全部转绿或按基线显式登记 | `src/native/runtime-core/` 等 | 大 |
| `T0.5` | 0 | ready | tbd | 🔴 **修 `fact_chunk.py` 分子分母跨技术混用**（T0.2 新发现的假绿通路） | T0.2 | batch-2 | `:310-318` 把 `passed`/`total` 整体换成 JIT 数，而 `:441` 的 `factory_gap_ct` 恒取自 AOT ⇒ 分子分母不同总体；症状 `gatePassed=520 > gateTotal=519` | `fact_chunk.py` 修复 + 回归 | 分子分母同源；`gatePassed <= gateTotal` 恒成立 | `tests/e2e/verification/stages/fact_chunk.py` | 小 |
| `T1.1` | 1 | done | tbd | `CancellationToken.throw_if_cancellation_requested` 真实抛出 | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | revert 后测试失败 | `src/native/runtime-core/cancellation_token.cpp` | 中 |
| `T1.2` | 1 | done | tbd | `source_get_token` 构造真实 token | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | 同上 | `src/native/runtime-core/cancellation_token.cpp` | 小 |
| `T1.3` | 1 | done | tbd | `CreateLinkedTokenSource` 实现或显式拒绝 | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | 同上 | `src/native/runtime-core/cancellation_token.cpp` | 中 |
| `T1.4` | 1 | done | tbd | `Parallel` failed 接线 + 结果返回 + 异常传播 | T0.4 | batch-3 | 反例验证 | `parallel.cpp` + 测试 | 5 项测试全绿 | `src/native/runtime-core/parallel.cpp` | 大 |
| `T1.5` | 1 | done | tbd | `SynchronizationContext::Post` 真实入队 | T0.4 | batch-3 | 反例验证 | `synchronization_context.cpp` + 测试 | 同上 | `src/native/runtime-core/synchronization_context.cpp` | 中 |
| `T1.6` | 1 | **completed（假说证伪）** | main | 🔴 ~~`ChaosAsyncTaskAwaiterGetResultVoid` 指针类型混淆~~ **→ 真因：`SubjectInstanceFactory.Create<T>()` 降级为 fallback（恒返 null）** | T0.5 | batch-3 | 取证结论：原「指针混淆」**被自身取证证伪** —— 4 个 factoryGap 中 3 个 AOT=JIT 同为 factoryGap 且生成体不含 awaiter 调用；唯一真 diff `DisposeAsync_1__0` 的判别信号是 **null 是否被消费**（`chaos_locals` store→reload→null-guard），非 helper 内部。helper 本身无缺陷，**不改代码** | 取证报告（STATUS 已重写） + Phase 2 覆盖项登记 | ✅ AOT/JIT 全量对照完成；结论已写入 STATUS「缺陷 1」 | `docs/dev/in-progress/threading-production-readiness/STATUS.md` | 中 |
| `T2.0` | 2 | planned | tbd | 新建 `extern "C"` ABI 出口层（**前置，不可跳过**） | T1.* | batch-4 | 参照 `interlocked_stubs.h` / `threading_stubs.h` 既有模式 | `runtime_stubs/` 下新增头/实现 | ABI 符号可被 codegen 生成的 C++ 调用 | `src/native/runtime-core/runtime_stubs/` | 大 |
| `T2.1` | 2 | planned | tbd | 托管对象 ↔ native 句柄映射机制（**新机制，无先例**） | T2.0 | batch-4 | 需处理生命周期与 GC 交互 | 映射机制实现 | 托管 `SemaphoreSlim` 实例可绑定 native 槽位 | `src/native/runtime-core/` | 大 |
| `T2.2` | 2 | planned | tbd | 注册 SemaphoreSlim + ReaderWriterLockSlim（含 upgradeable） | T2.1 | batch-5 | 最难；含 upgradeable 语义 | ShapeRegistry 注册 + 测试 | real% 提升；测试通过 | `.../RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs` | 大 |
| `T2.3` | 2 | planned | tbd | 注册 Barrier + CountdownEvent | T2.1 | batch-5 | — | 同上 | 同上 | 同上 | 中 |
| `T2.4` | 2 | planned | tbd | 注册 ManualResetEvent(Slim) + AutoResetEvent + Timer | T2.1 | batch-6 | — | 同上 | 同上 | 同上 | 中 |
| `T2.5` | 2 | planned | tbd | 注册 SpinWait + SpinLock + ThreadPool 可调用面 + Parallel.Invoke | T2.1 | batch-6 | — | 同上 | 同上 | 同上 | 中 |
| `T3.1` | 3 | planned | tbd | 非原子 static 桥变量（8 处） | T2.5 | batch-7 | — | `synchronization.cpp` 等 | 并发验证通过 | `src/native/runtime-core/` | 中 |
| `T3.2` | 3 | planned | tbd | 句柄表真正释放 | T2.5 | batch-7 | — | `synchronization.cpp` / `wait_handle.cpp` | 泄漏检测通过 | 同上 | 中 |
| `T3.3` | 3 | planned | tbd | 移除热路径 fprintf | T2.5 | batch-7 | — | `thread_pool.cpp` 等 | 热路径无 fprintf | 同上 | 小 |
| `T3.4` | 3 | planned | tbd | 文档不符修正 | T2.5 | batch-8 | — | wiki / STATUS 文档 | 文档与实现一致 | `wiki/`、`docs/` | 小 |
| `T3.5` | 3 | planned | tbd | benchmark 填充 | T2.5 | batch-8 | 4 个空模板 HTML | benchmark 数据 | timing > 0 | `tests/e2e/verification/reporting/` | 中 |
| `T4.1` | 4 | planned | tbd | threading chunk 门禁阈值 + 回归告警 | T3.* | batch-9 | **必须把 `kCodegenFailureCount` 纳入阈值**（当前零消费者，见 STATUS T0.3 结论） | 阈值配置 + 告警接入 | 阈值生效；告警可触发；`kCodegenFailureCount>0` 能阻断 | `.github/workflows/`、验证管线配置 | 中 |

**status 说明**：`T0.0` = `ready`（无前置依赖，可立即启动）；其余 `planned`。

### T1.7 完成记录（7 项全部转绿，commit `65508dbdb`）

| 测试 | 原症状 | 真根因 |
|------|--------|--------|
| `test_threading_stress` | 1800s 超时 | **RWLock 丢唤醒**（见下） |
| `test_async_continue_with` | SEH 0xC0000005 | `chaos_task_continue_with` 释放后使用：先行任务已完成时回调同步内联并 `delete st`，调用方随后读 `st->continuation_task` |
| `test_queue_backpressure` | SEGV | `TryStealFromWorker` 无锁读 `s_worker_queues`（写入侧持 `s_mutex`）→ UAF。回退验证：回退后 20 次崩 7 次，保留 0 次 |
| `test_async_when_each` | 23s 挂起 | **非缺陷**——`may_have_next` 在"有元素排队或子任务未完成"时返回 1 是文档化契约；原测试不排空就等终止，是断言错 |
| `test_threading_benchmark` / `test_async_when_async` / `test_phase3_industrialization` / `test_rwlock_upgrade` | SEGFAULT / 失败 | **非独立缺陷**——均为上述 RWLock/偷取缺陷在竞争下的下游表现，修复后各自单独全绿 |

**RWLock 死锁的真根因（本任务最重的一项）**：实测挂死现场为
`state=0 waiting_readers=7 waiting_writers=1` —— `state==0` 说明锁是空闲的，
但所有线程都在 `cv.wait` 里睡着，即**丢唤醒**。根因是 `waiting_readers` /
`waiting_writers` 在慢路径里**持 `entry->mutex`** 自增，三个 Exit 路径却
**不加锁**读这两个计数再决定要不要 `notify`：一个"已发布计数但还没走到
`cv.wait()`"的等待者，对裸 `notify_all` 是不可见的，若此后没有新的释放
就永久 park。修法是把 notify 决策统一移入 `entry->mutex` 临界区。

配套的两个状态机缺陷：(a) `ExitRead` 无条件 `fetch_sub` 先把 `state` 改掉
再判"是否真持有读锁"，错配 exit 会把计数推成负数且不可恢复——挂死现场曾
观察到 `state=-1` 且 `writer_tid=0`，即锁被"无人持有的写者"永久占住；
(b) `EnterWrite`/`UpgradeToWrite` 慢路径无条件 `state.store(-1)` 会覆盖
读者快路径刚 CAS 上的 `0→1`，使锁同时交给两个持有者。两者分别改为 CAS。

**方法论教训**：本项前两次修复尝试（`notify_one`→`notify_all`）均失败，因为
那只是把"叫错人"换成"叫了但没人被叫到"，未触及可见性根因。真正解决问题的是
**读活状态而非推断**——为此新增 NDEBUG 门控的 `ChaosDebugDumpRWLocks()`
导出（附加到挂死进程后 `.call` + `g`，把锁表打到 stdout）；一行
`state=0 waiting_readers=7 waiting_writers=1` 直接定性为丢唤醒，而栈回溯
只能显示"全体在 cv.wait 里睡着"。该诊段保留在 `synchronization.cpp` 供后续复用。

### T1.4 完成记录（`parallel.cpp`，反例验证**部分成立**——一个假说被自身证伪）

**修了什么（实测支撑）**

| 改动 | 实测证据 |
|------|---------|
| dispatch 数由 `总chunk数` 改为 `min(总chunk数, hw*4, 64)` | 旧码在**热池**上跑一次 `Parallel.For(0,10000)` 把 worker 从 10 撑到 **94**，耗时 **60ms**；改后 14ms |
| 等待自旋由 `CHAOS_IL2CPP_PAUSE_HINT` 改为 `std::this_thread::yield()` | `PAUSE_HINT` 在 x64 上展开为 `_mm_pause()`，**不让出时间片**；而这里等的是别的线程干的活，自旋方占满一个核正是"并行比串行还慢"的构型 |
| 完成计数改为"按 chunk 释放"（`fetch_sub` 返回值判末位） | 见下方**证伪**——保留为健壮性，非实测缺陷修复 |

**被证伪的假说（重要，勿据其结论行动）**

原判据是「`remaining` 按 worker 数播种、却按 chunk 释放 ⇒ 调用方永久自旋」。
按 roadmap 的反例纪律把**旧 worker 形状原位注入**（无 claim 上限 + 每个 worker 退出时释放一次）后：

- 5 项测试**全部仍然通过**，一次挂起都没有复现；
- 插桩实测 **32 个 chunk 对应恰好 32 次 worker 退出**——池子为每个 work item 起了独立 worker，
  于是「按 worker」与「按 chunk」在数值上等价，计数没有错。

**结论：该挂起在本代码路径上不存在**。计数改动因此**不是**缺陷修复，
没有测试能把它钉住；它在测试文件里被显式登记为 robustness-only，避免后人误以为有承重测试。

**为何测不出**：要真正复现需要"一个 worker 领走多个 chunk"，而池的增长策略
（每次入队 `depth > 3×workers` 即加一个 worker，gate tick 每 15ms 再加）
几乎总是补足到"一个 work item 一个 worker"，很难确定性地构造出该条件。

**顺带实测发现的**：`空区间` 与 `null delegate` 都返回 `-1`，与"正常跑完"**不可区分**
（`ParallelLoopResult` 语义下 `-1` 表示 completed-without-break）。已登记，未改。

**方法论教训**：本项最初的分析是**从代码形状推断**出"必然挂起"，并据此写了自认为承重的反例。
两次反例注入都通过后才去插桩实测——实测直接否定了推断。
**顺序反了**：应当先取实测（插桩计数/worker 退出数），再据此写反例；
"看起来必然"的并发缺陷必须先用测量确认它真的发生，否则写出来的是**假承重**测试——
它与假绿同族：都让人以为有覆盖而实际没有。

---

```
T0.0 ──┬─→ T0.1 ─→ T0.2 ─→ T0.4 ─→ T1.1..T1.5 ─→ T2.0 ─→ T2.1 ─┬─→ T2.2, T2.3
       │                   └─→ T0.5 ─┘  （Phase 1）              ├─→ T2.4, T2.5
       └─→ T0.3（独立取证）  └────→ T1.6                     │
                            └────→ T1.7（T0.4 首次真跑的 7 个失败）└─→ T3.1..T3.5 ─→ T4.1
```

**关键串行点**：
- `T0.0 → T0.1`：T0.0 的结果决定 Phase 0 形状（若错误类型非 crt_stubs，T0.1 需重评）
- `T0.2 → T0.4`：CI workflow 需要知道实际的测试规模与失败基线
- `T0.2 → T0.5`：**T0.2 的 fact 比对新发现**，先堵计数假绿再谈基线
- `T0.5 → T1.6`：T1.6 的验收依赖 T0.5 修好的 cross-tech diff 计数可信
- `T0.4 → T1.7`：**T0.4 首次真跑暴露的 7 个失败**；T1.7 的优先级**高于** T1.1-T1.5
  （崩溃 > 静默错误结果 > 语义缺失），建议 T1.7 先行
- `T1.* → T2.0`：Phase 2 依赖 Phase 1 关闭的语义造假（否则 ABI 接线接的是错的语义）
- `T2.1 → T2.2..T2.5`：句柄映射机制是所有原语注册的前置

**独立点**：`T0.3` 与 T0.1/T0.2 无依赖（只依赖 T0.0 的实测结果）

> **T0.2 追加发现的溯源说明**：`T0.5` 与 `T1.6` 均**不是**原计划的子任务，
> 而是 T0.2「运行 + fact 比对」这一步的实测产出。原 roadmap 把 T0.2 的验收写成
> 「`build.status=passed` 且 `kCodegenFailureCount` 已查」——那只覆盖了**编译+降级**，
> 漏掉了「跑起来对不对」。补上这一步后立刻暴露 2 个缺陷，**证实该验收条件不充分**。
> 教训：`build passed` ≠ 语义正确，两者之间必须插入「运行 + 跨技术比对」。
>
> **T1.6 假说证伪的溯源说明**：T1.6 原本被写成「`ChaosAsyncTaskAwaiterGetResultVoid`
> 指针类型混淆」，依据是插桩观测到 `canceled=107`（栈垃圾）。**该结论是错的**，
> 后续全量对照推翻了它：4 个 `factoryGap` 中 3 个在 AOT/JIT 下**同为** `factoryGap`，
> 且其生成体**不含任何 awaiter helper 调用**。真判别是「`Create<T>()` 返回的 null
> 是否被消费」。**教训：插桩观测到异常值 ≠ 该值是因果链上的原因；先做分母审计
> （全量对照 AOT/JIT），再谈单点根因。** 这正是 T0.5 修好计数后可做的事 ——
> **修假绿通路本身放大了后续取证的分辨率**。
>
> **T1.7 的溯源说明（第四条假绿通货路的教训）**：T0.4 本被写成一件"建 workflow"的
> 配置活（估时"中"）。实际执行时发现 **ctest 在 Windows 下从未发现过任何测试** ——
> `enable_testing()` 缺失，`add_test()` 全部落入虚空。修复后 244 用例**首次真正运行**，
> 立刻暴露 **7 个失败（5 个 SEGFAULT + 1 个挂起 + 1 个超时）**，它们此前从未被任何人看到。
>
> 这条通路与 T0.1（读源树残留）、T0.3（codegen 降级信号零消费者）、T0.5（分子分母
> 跨技术混用）同族，但**形态更隐蔽**：前三者是「读错了源」，这一个是「**根本没读**」。
> `add_test` 静默失败、`ctest` 零测试退出码 0 —— 中间没有任何一处会报错。
>
> **教训：验证设施本身必须被验证。** 一个声称覆盖 N 个用例的门禁，如果没有任何
> 断言去核对"实际发现了几个测试"，那它报告的绿是**关于它自己**的，不是关于代码的。
> ⇒ T0.4 交付的 workflow 因此内置了**显式的发现数断言**（发现 0 个即 `exit 1`）。

---

## 8. 风险

| 风险 | 触发条件 | 后果 | 缓解 |
|---|---|---|---|
| T0.0 根因与推断不符 | 实测错误非 crt_stubs | Phase 0 形状重写 | T0.0 设为首任务；**不得**无实测开工 T0.1 |
| T0.0 实际是完整重建 | 无构建日志可读 | 耗时不符预期 | 按「完整重建」预估耗时 |
| 重建后意外转绿 | 错误为暂态 | 根因推断落空 | 转绿**不等于**推断正确；必须回溯确认生效改动 |
| **假绿向量（源树残留）** | 陈旧 `fact.json` 未清 | chunk 实际 `build.status=error` 却被判通过 | T0.1 清除 + 验收读取根钉死 + mtime/runId 断言 |
| **🔴 假绿向量（codegen 静默降级）** | `BuildMethodSourceSafe` 吞异常降级为空 stub | `build.status=passed` 但方法体是空的 | **T0.3 已确证存在**（无过滤器 catch + 信号零消费者）；T0.2 必须读 `[CODGEN-FAIL]`/`kCodegenFailureCount`；T4.1 纳入门禁 |
| 主工作区被并发 agent 抹除 | 已有 agent 改 `Part1.S16.cs`（threading 注册表本身） | 改动丢失、注册状态漂移 | T0.0 后**立即切 worktree**；commit 前 `git diff --cached` |
| worktree 冷构建成本超预期 | 无共享 `artifacts/` | T0.2 起步耗时不可控 | T0.2 记录耗时；> 30 分钟退化单 worktree |
| Phase 2 接线后 real% 不升 | 存在第二道断点 | 最大杠杆动作失效 | 专项取证；**三次规则**后转 dev-brainstorm |
| 244 用例首跑大量失败 | 测试长期未跑已腐烂 | CI 长期红 | 首跑 `continue-on-error` 收基线，再定阻断阈值 |
| 重建触发 cross-chunk 回归 | crt_stubs 重写影响其他 chunk | 已有绿色 chunk 变红 | 单 chunk 隔离 + honest report 前后对比 |

---

## 9. 备选路径

| 备选 | 触发条件 | 内容 |
|---|---|---|
| **单 worktree 覆盖全部** | T0.2 构建 > 30 分钟 | 放弃阶段间隔离，`artifacts/` 只建一次 |
| **Phase 2 拆出独立任务** | Phase 2 开工后发现 ABI 出口层 + 句柄映射的实际工作量远超预估 | 父 roadmap 收口 Phase 0/1/3/4，Phase 2 单独立项 |
| **`threading-2` 降级为已知限制** | 该 chunk 长期无法转绿且阻塞推进 | 隔离为 watch_item，不阻塞 `threading` |

---

## 10. 当前建议推进顺序

1. **T0.0** —— 立即启动，主工作区，完整重建，拿实测错误
2. **切 worktree**（`EnterWorktree(name=threading-phase0)`）
3. **T0.1 → T0.2** 串行；**T0.3** 可并行
4. **T0.4** → **检查点**：重新评估 Phase 1-4 优先级
5. Phase 1 五项（同一文件 `cancellation_token.cpp` 的三项需注意 T1.1/T1.2/T1.3 的文件冲突）
6. Phase 2（T2.0 → T2.1 严格串行，之后可并行）
7. Phase 3 → Phase 4

---

## 11. 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

- 子任务默认入口 `dev-writing-plans`（先写 plan → 再执行）
- T0.1 / T3.3 等极小任务可直接 `dev-executing-plans`
- 子任务进入终态后必须**物理移动**目录到 `docs/dev/completed/` 等

---

## 12. 调度策略

**`dispatch_model: sequential`** —— 判据：

- **存在强依赖链**：`T0.0 → T0.1 → T0.2 → T0.4 → T1.* → T2.0 → T2.1 → ...`，前置未满足无法启动
- **存在文件冲突**：`T1.1/T1.2/T1.3` 同改 `cancellation_token.cpp`；`T2.2-T2.5` 同改 `Part1.S16.cs`；`T3.1/T3.2` 同改 `synchronization.cpp`
- **构建资源独占**：`T0.2` 的 chunk 重建与任何其他 native 构建争抢

⇒ 不满足 `parallel-batch` 的「同 batch 内无文件冲突」硬规则。
`batch_id` 字段保留，标示**串行链上可连续执行的相邻任务**（供后续若改为 hybrid 时复用），
当前不作为并行派发依据。
