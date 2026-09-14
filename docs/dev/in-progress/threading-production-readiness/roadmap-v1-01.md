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
| `T0.5` | 0 | **completed（先前已落盘）** | main | 🔴 **修 `fact_chunk.py` 分子分母跨技术混用**（T0.2 新发现的假绿通路） | T0.2 | batch-2 | `:310-318` 把 `passed`/`total` 整体换成 JIT 数，而 `:441` 的 `factory_gap_ct` 恒取自 AOT ⇒ 分子分母不同总体；症状 `gatePassed=520 > gateTotal=519` | `fact_chunk.py` 修复 + 回归 | ✅ 已由 `e0be16790` 修复并附回归测试 | `tests/e2e/verification/stages/fact_chunk.py` | 小 |
| `T1.1` | 1 | done | tbd | `CancellationToken.throw_if_cancellation_requested` 真实抛出 | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | revert 后测试失败 | `src/native/runtime-core/cancellation_token.cpp` | 中 |
| `T1.2` | 1 | done | tbd | `source_get_token` 构造真实 token | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | 同上 | `src/native/runtime-core/cancellation_token.cpp` | 小 |
| `T1.3` | 1 | done | tbd | `CreateLinkedTokenSource` 实现或显式拒绝 | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | 同上 | `src/native/runtime-core/cancellation_token.cpp` | 中 |
| `T1.4` | 1 | done | tbd | `Parallel` failed 接线 + 结果返回 + 异常传播 | T0.4 | batch-3 | 反例验证 | `parallel.cpp` + 测试 | 6 项测试全绿；**另有 cap 截断缺陷（本轮追补修复，见 T1.4 追补记录）** | `src/native/runtime-core/parallel.cpp` | 大 |
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

### T1.4 追补：cap 引入的**区间截断**（本项真正的缺陷，`chunks_for` 上限未同步缩放宽）

上面「反例验证部分成立」的记录写成时，T1.4 **仍带着一个自己引入的真缺陷**，
而当时全部测试都是绿的。该缺陷在本轮为「复核 60ms/14ms 性能数据」而重建 A/B 探针时才暴露。

**症状**：`Parallel.For(0, 4096, body)` 只跑约 **2200~2450** 次 body 调用（每次运行不同），
而不是 4096 次；调用方**照常返回**，不报任何错。加一句 `sleep` 在调用之后会 **SEGFAULT**。

**根因**：T1.4 把 dispatch 数改成 `min(总chunk数, hw*4, 64)`，但 chunk 的**跨度**仍写死 `kChunkSize = 32`。
两者相乘才是可达的索引空间，而 cap 只压了其中一个因子：

```
可达上界 = dispatch 数 × 每 worker 可领块数 × 跨度
         = 64          × kMaxClaimAttempts(4)            × 32   = 8192
```

8192 > 4096，**看似够**——这正是缺陷能长期隐藏的原因：它不依赖常量关系，只依赖
**claim 预算恰好被花在对的顺序上**。于是同一个调用在不同运行下覆盖不同数量的索引
（实测 2197 / 2248 / 2299 / 2365 / 2397 / 2444 / 2454，**没有一次跑满**）。

**为何五个测试全绿**：它们的最宽区间是 2000（63 块），**刚好卡在 64 块 cap 之下**。
把旧跨度原位注入回去后，五个测试**仍然全绿**——它们对本缺陷完全不承重。

**修法**：跨度由 dispatch 数反推，`span = ceil(count / chunks)`，下界 `kMinChunkSize`。
不变量从「靠预算算术侥幸成立」变成「由构造成立」：`chunks × span >= count` 恒真。
同时补一条真正承重的测试 `RangeWiderThanTheDispatchCapIsCoveredWhole`（区间 5000，
刻意越过 cap 且留余数块）。

**反例验证（本次成立）**：注入 `span = 固定 32` → 新测试 **FAIL**（`missing 791 of 5000,
first missing index 3772`），其余 5 条 **仍全绿**；恢复后 6/6 全绿。

**实测修复效果**：区间 0..96 / 256 / 512 / 1024 / 2048 / 4096 / 8192 **全部跑满且无 gap**，
连跑 5 次稳定；修复前 A/B 探针在首次宽区间调用即 **SEGFAULT**，修复后完整跑完。

**方法论教训（比上一条更重）**：T1.4 的完成记录写成时，我给出的证据是「5 项测试全绿」——
而 5 项测试的**区间宽度全部在 cap 之下**，这个覆盖面是**结构性**不足，不是运气不好。
**新引入的「上限」类参数必须同时验证它与其它常量乘积仍是全称量词**；
只验证「有限几个区间仍绿」等于没验证。这也解释了为何当初的性能数据会引人误判：
探针区间 0..10000 早已越过 cap，只是当时把截断误读成了"测不准"。

---

### T1.5 追补：**测试与实现反向**（`PostInvokesTheCallbackWithItsState` 钉的是被 T1.5 删掉的行为）

T1.5 把 `Post` 从「内联执行」改成「入队 ThreadPool 后立即返回」——实现是对的，
但 `synchronization_context_test.cpp` 里那条老测试，在 `Post` 返回的**下一行**就断言
`g_post_count == 1`。这个断言**只有内联实现能满足**。于是：实现修对了，测试开始红。

> **这类测试比没有测试更坏**：它把**正确的修复**显示成回归，会诱导人回滚修复。

**取证（不是推断）**：在 HEAD 上另开干净 worktree 复现，**同样这一条失败**——排除「本次改动引入」。
随后读实现确认契约确实是异步（`synchronization_context.cpp:81-109` 入队后 `return true`，
不执行回调），而 `Send` 保持内联（`:111-117`），故 `Send` 那条一直绿。

**第一次修法仍然是错的（记录下来）**：我把断言拆成「立即读计数器必须为 0」+「等待后必须为 1」。
方向对，但**立即读计数器本身有竞态**——worker 可能在 `Post` 写完队列、调用方读计数器之间就把回调跑完了。
这是个**瞬时**窗口，单跑碰不上，只在合适调度下才炸。
而这正是同目录的姊妹测试 `test_synchronization_context_post` **早已明确否定**的做法，
它的注释原文写着：*"A 'did it run before Post returned?' assertion cannot: a fast worker makes that
observation inherently racy, so it would be flaky against a correct implementation."*
我等于重新发现了一遍已被记录过的坑——**同目录已经写好的教训没有被读**。

**正确的判别式是线程身份，不是时序**：

| 实现 | 回调执行线程 |
|---|---|
| 内联（旧） | **调用方**线程 |
| 入队（T1.5） | **其它**线程 |

入队时调用方正阻塞在等待里，不可能同时在跑 pool work，所以「回调跑在别的线程」是**确定性**判据。
计数器/state 的断言全部移到等待**之后**，不再与任何东西竞态。

**另修一个真缺陷（我引入的）**：改完测试后 gtest 打印 `[ PASSED ] 7 tests.` 却**进程退出码 3**，
ctest 因此判红。用 `--gtest_filter` 逐条二分，锁定到 `PostInvokesTheCallbackWithItsState`。
根因：该测试从**不调用 `ThreadPoolInitialize()`**，而它现在真的会往池里投递工作项；
入队到一个未初始化的池会留下退出阶段仍存活的线程，导致 teardown 异常退出。
**修法**：补 `ThreadPoolInitialize()` / `ThreadPoolShutdown()` 配对（与姊妹测试一致）。
> 教训：**gtest 打印 PASSED ≠ 进程退出码为 0**；ctest 看的是后者。校验测试是否真绿，
> 必须同时看退出码，否则会得到「测试全绿但 CI 红」这种自相矛盾的信号。

**反例验证（两次注入均按预期）**：
- 注入内联 `Post` → 仅 `PostInvokesTheCallbackWithItsState` **FAIL**，落在线程身份断言上，其余 6 条绿；
- 恢复 → **7/7 绿且退出码 0**。

**实测**：`ctest -L threading` **30/30 全绿**（修前为 97%，1 项常红）。
> 注：另有一次全量运行中 `test_async_integration_smoke` 单次失败，
> 但单独连跑 6 次及随后全量运行均通过，**未能复现**，记为已知 flaky，未做处置。

---

### T2.0 完成记录：`extern "C"` ABI 出口层（`runtime_stubs/synchronization_stubs.{h,cpp}`）

**这一项的工作量远小于 roadmap 预估，原因值得记录：native 实现早已存在。**

`SemaphoreSlim` / `ReaderWriterLockSlim`（含 upgradeable）/ `Barrier` / `CountdownEvent`
的完整实现已在 `synchronization.cpp`，`ManualResetEvent` / `AutoResetEvent` 已在
`wait_handle.cpp`，`Timer` 已在 `timer_queue.cpp`，`ThreadPool` 已在 `thread_pool.cpp`。
Roadmap 当初把 T2.2/T2.3/T2.4 标为「大/中」，但它们的 native 部分（也就是最难的部分）
**已经写完并通过既有测试**。

真正缺的只有两件事，且互为因果：

1. 这些函数是 **C++ 命名空间函数**（`chaos::il2cpp::runtime_core::threading::Xxx`）；
2. 因此 codegen 的 `SimpleForward` 发射**够不到它们**——它发射的是
   `reinterpret_cast<CHAOS_IL2CPP_INTPTR>(<NativeFnSymbol>(args...))`，
   一个**不带限定名**的直接调用，编译进独立的 TU。

所以 T2.0 的产物是 **ABI 边界，不是重新实现**：39 个 `extern "C"` 转发 shim。

**命名约定**：采用 `Chaos` + PascalCase，与 ShapeRegistry 已在引用的 Interlocked 先例一致
（`ChaosInterlockedMemoryBarrier`，见 `RuntimeHelperShapeRegistry.CoreStubs.Part2.S22.cs:155`）。
旧有的 `chaos_*` snake_case 一族（`chaos_monitor_enter`、`chaos_thread_*`）**保留不动**——
两种大小写都是合法 C 符号，registry 条目名与实现名一致即可，为风格统一去改已工作的符号是净损失。
**新增的线程导出统一用 `Chaos*`。**

**exit 判据（"ABI 符号可被 codegen 生成的 C++ 调用"）是链接期性质，故用链接期测试验证**：
`tests/unit/runtime-native/runtime-core/threading/synchronization_stubs_test.cpp`
**只 include ABI 头**，不 include 任何 `synchronization.h` / `wait_handle.h` / …，
调用全部走 header `extern "C"` 块内的非限定标识符——与生成 TU 所见完全一致。
该 TU 甚至还**不能**写 `using namespace chaos::il2cpp::runtime_core::threading;`
（实测报 `C3083`），因为 ABI 头根本不引入命名空间，这本身就是"它就是边界"的证据。

**客观证据（非推断）**：`dumpbin /symbols synchronization_stubs.obj` 显示
`External | ChaosSemaphoreSlimCreate` —— **未修饰**，且 `grep -c '\?Chaos[A-Za-z]*@@'` **为 0**，
即无任何 C++ 修饰名。

**反例验证**：把一个符号的声明挪到 `extern "C"` 块**之外**（正是 `threading_stubs.h`
所记录的漂移缺陷），编译期**无诊断**，链接期报
`LNK2019: unresolved external symbol "int __cdecl ChaosBarrierCreate_probe(void)"`——
注意返回值类型与 `__cdecl` 都进了符号名，证明该声明拿到的是 C++ 链接性与调用约定；
而对应定义是未修饰的 C 符号。恢复后 **9/9 绿、退出码 0**。

**顺带修掉一个真缺陷（与 T1.5 同根）**：`ChaosThreadPoolEnsureInitialized` 启动线程池后
若不关闭，进程在所有测试 PASSED 的情况下仍以**退出码 3** 结束，ctest 记红。
故 ABI 必须导出 `ChaosThreadPoolShutdown`（不能假设调用方有别的关闭途径）。
**这与 T1.5 的 `synchronization_context` 是同一个根因**：启动线程池/投递工作项却不收尾。
> 沉淀：**"启动了持有活线程的子系统"必须配套显式关闭出口**，否则症状是
> "gtest 全绿但 ctest 红"，且用 `--gtest_filter` 二分才能定位。

**实测**：`ctest -L threading` **31/31 全绿**（含新增 1 项）。

---

### ⚠️ T2.1 前提修正：句柄映射**有先例**，不是"新机制"

原 roadmap 把 T2.1 描述为「托管对象 ↔ native 句柄映射机制（**新机制，无先例**）」。
本轮排查（含一次独立专项调查）确认**该描述不成立**，至少三处先例：

| 先例 | 位置 | 形态 |
|------|------|------|
| **WaitHandle 表**（最接近） | `wait_handle.cpp:38-40` | `unordered_map<uint32_t, unique_ptr<WaitHandleEntry>>` + `shared_mutex`；`WaitHandleEntry{id, active, PalEvent*}`；单调递增 id |
| **同步对象固定数组表** | `synchronization.cpp` | `g_semaphores[1024]` / `g_rwlocks[1024]` / `g_barriers[1024]` / `g_countdown_events[1024]`；句柄=数组下标 + `active` 标志 |
| **通用对象句柄** | `engine_binding.cpp:19-34` | `CreateEngineObjectHandle(void*)` → `uint32_t`；`ResolveEngineObjectHandle(uint32_t)` → `void*` |

**T2.1 应当沿用既有形态，而不是发明新基础设施。** 唯一真正的新问题是
**生命周期与 GC 交互**（roadmap 原文这点是对的）：上述三者的句柄都是**原生侧自有**的，
而 T2.1 要绑定的是**托管对象**——必须回答"托管对象被 GC 回收后，native 槽位由谁释放"。
建议 T2.1 实现时以 `wait_handle.cpp` 的表为模板（它已处理 `active` 标志与并发查找），
并把 GC 侧的生命周期问题**单独**作为设计点，而不是把"没有先例"当成整项的风险来源。

---

### T0.5 复核记录（已完成，先前 roadmap 状态列滞后，commit `e0be16790`）

`e0be16790` 已修复并附回归测试 `tests/e2e/verification/tests/test_fact_chunk_reference_population.py`
（7 passed）。本轮复核确认**修复成立且不为假绿**，依据两条：

1. **同一总体，代码可读**：`select_reference_counts()` 只取 `aot_result`，并显式拒绝在
   JIT 通过率更高时换用 JIT 计数（函数注释即记录了 520>519 这一原始症状）。
2. **不变式由构造保证**，而非侥幸：`factoryGap` 记录的定义是"分派阶段就抛出、
   从未进到方法体"（`fact_chunk.py:164`），因此**必然是 non-passing**，天然不计入 `passed`；
   `gateTotal = total - factory_gap_ct` 又从分母剔掉同一批记录。两侧剔的是同一批，
   故 `gatePassed <= gateTotal` 恒成立。

**实测佐证**（threading chunk）：`passed=519 total=523 factoryGap=4 failed=0`，
五个桶 `real+unassertable+smoke+factoryGap+failed = 523 = total` 恰好闭合。

> **复核时踩到的坑（记录以免重蹈）**：我一度从 `passed == total - failed` 反推出
> "`failed < factoryGap` 时仍会违例"，并据此判定 T0.5 **未修好**。该推断是**错的**——
> 它漏掉了 `passed` 本身也已排除 factoryGap 记录这一事实。
> **教训：断言"修复不成立"之前，先把分母/分子的定义从代码里读出来，而不是套一个看似显然的公式。**
> 这与本 roadmap 反复出现的假绿主题同源：假绿是"以为绿了其实没绿"，
> 而这是它的镜像——"以为红了其实没红"，同样源于用推断代替读数。

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
