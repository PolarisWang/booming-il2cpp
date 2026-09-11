# STATUS — System.Threading.Tasks 生产级工程

## 元信息

```yaml
task_id: async-task-industrialization
task_type: roadmap
phase: phase-2-组合子
roadmap_or_plan: docs/dev/in-progress/async-task-industrialization/roadmap-v1-01.md
design: docs/dev/in-progress/async-task-industrialization/design-v1-01.md
created: 2026-09-07
updated: 2026-09-11
created_by: user
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: sequential
auto_continue: true
```

## 当前底线（Phase 0 完成后可验证事实）

### Phase 0-1 ✅ 恢复被 ci-r7-clean 覆盖的 P3 实现
| 文件 | feccdb166 (应有) | HEAD Phase-0 (现在) | 验证 |
|------|------------------|---------------------|------|
| `async_stubs.cpp` | 333 | 363 | ✅ `grep chaos_task_when_all` 命中；`chaos_task_delay_stub` 全 |
| `async_stubs.h` | 53 | 64 | ✅ 含 when_all/when_any/delay_stub 声明 + WIP 4 项 |
| `task_runner.cpp` | 121 | 121 | ✅ EC auto-flow 已恢复 |
| `S16.cs` | 805 | 839 | ✅ 含 WhenAll/WhenAny/RegisterTaskDelay |
| `Part1.cs` | 611 + dispatch | 612 | ✅ `RegisterTaskDelay` dispatch 行存在 |
| `smoke_test.cpp` | 1031 | 1072 | ✅ 含 WhenAll/WhenAny/TaskDelay test + D3 回归测试 |

### Phase 0-2 ✅ 修复语义造假
| # | 缺陷 | 修法 | 验证 |
|---|------|------|------|
| `ChaosAsyncTaskAwaiterGetIsCompleted` | 恒返 1 → 永不真挂起 | 直接读 `task->completed.load(acquire)` | codegen 2165/2165 + smoke 28/28 |
| `ChaosAsyncTaskAwaiterGetResultValue` | 恒返 0 → `await Task<T>` 得 0 | 直接读 `task->result`（已 gate on completed+faulted） | codegen 2165/2165 |

### Phase 0-3 ✅ 修复 D2/D3 ThreadPool 缺陷
| # | 缺陷 | 修法 | 验证 |
|---|------|------|------|
| D2 | `s_workers.size()` 无锁读 data race | 新增 `atomic<int32_t> s_worker_count` 镜像，替换 4 处无锁读 | code review 确认 |
| D3 | `s_initialized` 不重置 → re-init no-op | `ThreadPoolShutdown` 末尾 `s_initialized.store(false, release)` | ✅ 新测试 `ReinitializeAfterShutdownRestartsWorkers` PASS |

### Phase 0-4 ✅ 修复 threading-tasks C2660 build 错误
| 追因 | 修法 |
|------|------|
| 非泛型 `SetResult[]` / `TrySetResult[]` 声明 1 个 slot 但 native 函数要 2 arg | registry 改为 2-slot、`rawArgumentIndices: {0,1}` |

## Phase 1 — 核心执行语义

设计验收点（design-v1-01.md Phase 1）：**退出 = 1-3 反例通过（这一条是整条路径最重要的验收点）**

| # | 内容 | 反例 | 状态 |
|---|------|------|------|
| 1-1 | `Task.Run` 端到端 | 委托真的在线程池上执行（不只"任务完成"） | ✅ `4ae78ede3` |
| 1-2 | `Task<T>::Result` / `Task::Wait` 阻塞语义 | Wait 阻塞 ≥ 任务时长；且**不得自旋占核** | ✅ `4830b6064` |
| 1-3 | **异常传播** 🔴 | faulted 任务 `Wait()`/`Result` **必须抛**，不得静默返 0 | ✅ `da19ca0d0` |
| 1-4 | 三态区分 | cancelled / faulted / resolved 互斥且可区分 | ✅ `fae9f4071` |

配套测试修复：
| # | 内容 | 状态 |
|---|------|------|
| 1-5 | 测试 repo-root 遍历跳过 worktree 根 → 读到主检出陈旧 DLL | ✅ `f80aada50` |
| 1-6 | `SubjectIdDiscovery` 硬编码 `bin/Release` | ✅ `d39244fae` |
| 1-7 | `Task.Run` emission 层端到端断言（替换假 "KNOWN LIMITATION"） | ✅ `f96d838fb` |

**Phase 1 反例验证方式（决策2=A）**：每个反例都做过 in-place revert 反证 —— 复原缺陷后
对应测试必须失败，且不依赖该性质的测试保持绿。逐条记录在各 commit message。

- 1-4：恢复 `canceled := faulted&&!exception` → 3 FAIL（含 `PayloadLessFaultIsNotCancellation`）
- 1-3：恢复 Wait 静默 `return 1` → `PayloadLessFaultRaisesFromWaitToo` FAIL
- 1-1：注释掉 `chaos_delegate_object_invoke` → 3 FAIL / 2 PASS（不依赖委托的仍绿）
- 1-2：恢复 yield() 自旋 → `BlockedWaitDoesNotSpinACore` FAIL（157ms CPU / 150ms 等待）

### Phase 1 附带修复的真实缺陷

| 缺陷 | 说明 | commit |
|------|------|--------|
| Wait 自旋占核 + 饥饿活锁 | busy-wait 在 waiter 多于空闲核时饿死 worker → 表现为挂起 | `4830b6064` |
| TCS `try_set_canceled` 置 `faulted` | 三态回退（把取消上报为普通 fault） | `4830b6064` |
| `set_*`/`try_set_*` 未维护 `canceled` | 任务可能同时报告两种状态 | `4830b6064` |

## Phase 2 — 组合子

### 已完成的子任务

| # | 内容 | commit | 反例验证 |
|---|------|--------|---------|
| P2-1 | WhenAll/WhenAny 反例（同步 + 异步两条路径） | `db356fa82` | 关闭 winner 识别 → 2 FAIL，index-0 保持绿 |
| P2-2 | `Task.ContinueWith` native 实现 + codegen 接线 | `4887d7556` | 不调用委托 → 9 FAIL（3 个 null 用例保持绿）；传 0 代 antecedent → 7 FAIL |
| P2-3 | WhenAll 结果集 `[1,2,3]` + 子数组 use-after-free | `1be6aa15b` | 恢复 `delete[] mem` → **进程崩溃**；不建结果集 → 3 FAIL；结果轮转 → 2 FAIL |
| P2-4 | ContinueWith 重载面（20 个重载逐族路由）+ subject-id 参数切分 | `6f28100a3` | 3 处 in-place revert 各命中不同测试（见下） |
| P2-5 | `Task.Factory.StartNew` 接线 + `get_Factory` 占位 token | `dd66e912f` | 移除注册 → 管线 FAIL；native shim revert → 4 FAIL/2 PASS |
| P2-6 | `Task.WhenAll<TResult>` 泛型重载接线 + emitter 崩溃修复 + fixture 定位修复 | `f3593d72f` | 恢复旧 guard → 恰 2 FAIL（无关项绿）；revert emitter 修复 → ArgumentOutOfRangeException 崩溃 |
| P2-7 | async entry 状态机 box 上 GC heap（P2-6 暴露的预存在缺陷） | 见下 | 注释掉分配 → 2 FAIL（85 项 async/registry 保持绿） |

### Phase 2 附带修复的真实缺陷（P2-4）

| 缺陷 | 说明 |
|------|------|
| **ContinueWith 只有 1/20 可用** | 原注册是 `Register()` 精确匹配，而管线走 `TryMatchGenericShape()`，所以**连唯一那个重载也没接上**。修后按族路由：仅委托重载 → native；带 CT/Options/Scheduler/state 的重载 → 返回 null 走解释器（避免"忽略参数照跑 body"的假绿） |
| **同一类型名两种拼写** | registry 级测试用 ``System.Action`1<...>``，真实管线用 `System.Action<...>`。只匹配反引号形式 → **单测绿、管线全 null**。这是 P2-4 新增管线级测试存在的直接原因 |
| **参数切分被嵌套泛型击穿** | `Split(',')` 把带程序集限定的泛型实参切碎：`Func\`2[[A, asm, Version=...],[B, ...]]` 1 个参数被切成 **10** 段 → `paramTypes.Count` 判断失真 → 静默不匹配。影响**所有多泛型实参的 helper**，不止 ContinueWith |

### 诚实负数结果（P2-4）

`IsAnyContinuationDelegate` 单独禁用**不改变任何测试结果** —— 当前 BCL 上没有"单参数但非委托"的 ContinueWith，arity 判断已经覆盖。该检查保留作为未来重载的守卫，注释已标注为**冗余**而非"承重"，不再声称其有效。

### 诚实负数结果（P2-5）🔴 假设被证伪

初始假设是**前缀碰撞**：`TypeDisplayNamePrefix: "System.Threading.Tasks.Task"` 用 `StartsWith` 匹配，
因此也会命中 `System.Threading.Tasks.TaskFactory`，导致 `Task.Run` 规则劫持 `TaskFactory::StartNew`。

**该假设是错的。** 它来自诊断代码里直接用手写 callee 调用 `TryMatchGenericShape()` —— 那是测量伪影，
不是真实管线行为。在真实管线路径上，去掉"按前缀长度排序"的重排、clean rebuild 后测试**依然通过**。

真实根因只是：**当时根本没有任何 `StartNew` 规则**。用"删掉 `RegisterTaskFactory` → 测试变红"直接证实。

因此那个"按前缀长度排序优先最具体匹配"的改动**不承重**，已**完整回退**（`RuntimeHelperShapeRegistry.cs`
最终无 diff）。把未证实的改动塞进共享匹配热路径、并以一个不存在的 bug 作理由，是不该做的事。

复现过的真实顺序事实（供将来参考，非本次修复依据）：`TryMatchGenericShape` 按注册序返回**首个**
method-name 与 type-prefix 同时命中的描述符；描述符命中后 resolver 返回 null 时**不会重试下一个**候选。
`TryMatchGenericShape` 也**不校验** `TypeDisplayNamePrefix` 是否真的前缀匹配 callee 的完整类型名
（只有 angle-bracket 回退分支做了该检查），所以前缀过捕获是该模块**确有**的隐患类别 ——
`ExternalRuntimeHelpers.cs` 中已有一个同类补丁（`System.Numerics.Vector` 前缀吃掉 `Vector2/3/4`，
用 `TryCreateVectorAllComparerHelper` 特判）。本次不修，登记为已知风险。

### 🔴 P2-6 暴露的预存在缺陷（P2-7 已修复）

修好 fixture 定位（见下）后，两个测试变红 —— 它们此前靠**读陈旧 Debug DLL** 而假绿：

| 测试 | 断言 | 修复前实际 |
|------|------|------|
| `GetOneEntry_AllocatesOnGcHeapNotStack` | async entry 用 `CHAOS_IL2CPP_NEW_GC` 分配 `>d__` box | entry 把状态机放在**栈局部** `&chaos_locals[0]`（`chaos_resolve_managed_value_pointer`），**没有** `CHAOS_IL2CPP_NEW_GC` |
| `MovenextEmittedSource_BoxPointerPassedByValueNotStackSlot` | 同上（box 按值传，非栈槽） | 同上 |

**已证明与 P2-6 无关**：用 pristine 测试文件复现两例皆红；把 emitter 修复 revert 掉也两例皆红。
即这是一条**真实的 async lowering 缺口**（R2b 声称已修但实际未生效）。**已由 P2-7 修复**（见下）。

### P2-7 根因（比测试名描述的更严重）

`IdentifyAsyncBoxPointerLocalSlots` 只有两条检测模式，**两条都不匹配真实 entry**：

- Pattern A 要求 `newobj` 一个 `>d__` 类型 → 但 Roslyn 对**不逃逸**的 async 方法（Release，全部
  4 个 fixture entry 都是）**根本不发 `newobj`**：状态机就是普通 `valuetype` 局部，entry 体
  只有 `ldloca.0` → `AsyncTaskMethodBuilder::Start<T>(ref T)`。实测 IL 确认（`ilspycmd -il`）。
- Pattern B 只覆盖 MoveNext（`IsAsyncStateMachineMoveNext`），entry 不走。

后果**不只是悬垂指针，而是栈缓冲溢出**：

```
_s2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(&chaos_locals[0]);   // 裸栈地址，无 local_slot_tag
...
auto* owner = chaos_resolve_managed_value_pointer<T>(_s2);        // 见 ChaosGeneratedRuntimePrelude.h:91
```

`chaos_resolve_managed_value_pointer` 只对**带 tag**的槽指针才做惰性分配；裸 `&chaos_locals[N]`
不带 tag，于是直接 `reinterpret_cast<T*>` —— 把一个 8 字节栈槽当成整个 `>d__` 结构体写入。
字段存储越过槽边界（栈溢出），且 `Start` 收到的是**只活到 entry 返回**的地址，而
`async_await_yield_resume` 会把它作为 resume target 排进线程池 → 悬垂。

**修法（两处）**：

1. `IdentifyAsyncBoxPointerLocalSlots` 增加 Pattern A'：无 `newobj` 的 entry 形态 —— 任何被
   `ldloca V` 喂给 `AsyncTaskMethodBuilder[<T>]::Start` / `AwaitUnsafeOnCompleted` 的局部槽
   都判定为状态机槽。
2. `MethodEmission` 在 prologue（body 之前）发出真正的分配：
   `chaos_locals[V] = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(CHAOS_IL2CPP_NEW_GC(<smType>, {}));`
   类型取自 `Start<T>` callee 的泛型实参，并按本方法程序集名补全前缀（callee 里的拼写
   **不带** `AsyncTestAssembly/`，直接用会生成不存在的符号）。

两处配合后：`_s2 = chaos_locals[0]`（box 值），不再是 `&chaos_locals[0]`。

**顺带修正一条假绿断言**：`MovenextEmittedSource_BoxPointerPassedByValueNotStackSlot` 的
`Assert.Contains("chaos_locals[0]", ...)` 在缺陷代码下**也通过** —— 因为 `&chaos_locals[0]`
包含子串 `chaos_locals[0]`。该断言本来就不具鉴别力。已改为必须同时含 `CHAOS_IL2CPP_NEW_GC`，
revert 验证后两测试**都**变红。

同时把该测试里硬编码的 `Assert.Contains("&chaos_locals[2]", ...)` 改成性质断言
`Assert.Contains("&chaos_locals[", ...)` —— 原断言钉死了 awaiter 的槽号，而槽号是局部布局的
实现细节，会随无关改动漂移（本次就漂到 `[3]`）。

### 附带修复的真实缺陷（P2-6）

| 缺陷 | 说明 |
|------|------|
| **emitter 硬崩溃** | `NativeAotEmitter.CollectExistingValueTypeTypedefs` 从 `struct chaos_valuetype_` 前缀**自身位置**开始找名字终止符，命中前缀内的空格 → `end - pos - 7` 为负 → `Substring` 抛 `ArgumentOutOfRangeException`。任何含值类型 struct 的 header 都会让 emitter 崩溃 |
| **fixture 定位解析错配置** | `LocateAsyncAssemblyDll` 从 `.../bin/Release/net8.0` 上溯两级到 `.../codegen`，再要求其父名为 `bin` —— 永不成立，静默回退 `Debug`。**Release 跑测读的是陈旧 Debug fixture**（方法新增后不出现，看起来像 linker/codegen bug，其实不是）。与 `f80aada50` 同一类缺陷，深了一层 |

### 更正此前记录
`IdentifyStructLocalSlots_NonValueTypeInitobj_NotCounted`、`UnknownExternalCall_UsesDispatchTable`、`CreatePseudoMetadataHandle_ReturnsNonZero` **不是**由本次 parser 修复。全量跑在 parser 回退与应用的两种情况下**都通过** —— 它们是环境/顺序相关，与 parser 无关。

### Phase 2 附带修复的真实缺陷（P2-3）

| 缺陷 | 说明 |
|------|------|
| **use-after-free** | `WhenAllAnyManagedArray` 拷贝子句柄到堆缓冲后立即 `delete[]`，而聚合的 continuation 在**每个子任务完成时**解引用该指针。异步完成的子任务读到已释放内存（ASan 可直接抓；无 ASan 时表现为错误的 winner/fault 或崩溃）。既有测试全部用同步完成的子任务，恰好走 inline 快路径而未触发 |
| **结果集缺失** | 聚合只记录完成/异常，从不收集子结果 → `agg->result` 恒为 0。旧 smoke 测试甚至断言 `result == 0` 并注释 "result not used"，把缺口编码成了期望行为 |

### Phase 2 剩余的缺口（尚需实现）

| API | 状态 |
|-----|------|
| `WhenAll<TResult>` 泛型重载 | ✅ 已接线（P2-6）；`WhenAll<TReturn>(Task<T>[])` 与非泛型共用 `chaos_task_when_all_array` |
| `WhenEach` | 🟡 **A1 已完成**（显式检测+诊断，`bd77bcd73`）；A2-A4（native builder / codegen 注册 / yield-return IR）待做 |
| `Task.Factory` | ⚠️ 部分接线：`get_Factory` + 委托版 `StartNew` 已接；余下 `StartNew` 变体(CT/Options/state/TResult) 显式走解释器。**接口真实规模 74 个公共实例方法**（设计文档写 21，是错的）；`FromAsync`(22) 无原生模型(APM/IAsyncResult)，`ContinueWhenAll/Any`(16+16) 未接 |
| `ContinueWith` 20 overloads | ✅ 逐族路由已定（1 族接线 / 4 族显式拒绝）；余下 15 个重载走同一 resolver 的 arity 判断，无需逐个登记 |
| **async entry box 栈分配** | ✅ 已修（P2-7）：entry prologue 用 `CHAOS_IL2CPP_NEW_GC` 分配 `>d__`，并按值传给 `Start` |

### Phase 2 / ASYNC-P2-8 — async iterator 分期（A1 已落地）

侦察（`async-iterator-recon-2026-09-11.md`）确认真实成本远超设计文档画像：
`Task.WhenEach` 的 `WhenEachState.Iterate<T>` 是 `async IAsyncEnumerable<T>`，其完整函数体
还依赖 Monitor.Enter/Exit、`ValueTask.AsTask`、`WaitAsync(CancellationToken)`、
`CancellationTokenSource` —— "完整 async iterator" 不是"再造一遍 Phase 2"，而是 ≥ Phase 2。

| 期 | 内容 | 验收 | 状态 |
|----|------|------|------|
| **A1** | 显式检测 + 诊断：识别 `AsyncIteratorMethodBuilder` 状态机，**显式记录**而非静默返 0 | 反例：去掉检测 → 未支持形态静默返 0（gate 必须变红） | ✅ `bd77bcd73` |
| **A2** | native `AsyncIteratorBuilder` + **池化** `ValueTask<bool>` source（`IValueTaskSource`） | P1 硬约束：池化非可选 | ✅ 本轮（见下） |
| A3 | codegen：registry 5-op 注册（Create/MoveNext/AwaitOnCompleted/AwaitUnsafeOnCompleted/Complete）、classify、await 侧目录 | — | ✅ 本轮（见下） |
| A4 | yield-return IR lowering（state=-4 续跑）、`IAsyncEnumerable`/`IAsyncEnumerator` 接口 vtable（slot 必须来自反射，不得手写常量）、`await foreach` 消费侧、`IAsyncDisposable`/`<>w__disposeMode` | **端到端可运行**（`Iterate<T>` 真跑通，`await foreach` 能消费） | ⬜ |

**A1 的关键发现（值得全项目记住）**：emission 跑在 `BuildMethodSourceSafe` 之下，
它捕获**一切**异常并替换成 `BuildAotUnreachableMethodStub`。A1 的第一版实现是
"抛 `NotSupportedException`"，测试正确地失败了——异常被吞掉、`CodegenFailureCount` 静默 +1、
构建保持绿色。**在这个位置用抛异常实现"响亮失败"，本身就是一种假绿。**
信号只能是"被记录的值"，不能是异常。故 A1 用 `UnsupportedAsyncIteratorSubjectIds` 记录，
并**无条件**（干净时为 0）emit `kUnsupportedAsyncIteratorCount` /
`kUnsupportedAsyncIteratorSubjects` 到生成 C++，镜像既有 `kCodegenFailureCount` 通道；
条件 emit 会让"无 iterator"与"信号未接线"无法区分。

**A2 交付**：`chaos/async_iterator.h`（`AsyncIteratorSourceCore` ≙
`ManualResetValueTaskSourceCore<bool>`、`AsyncIteratorSourcePool`、5 个 builder op）
+ `async_stubs.{h,cpp}` 的 extern C 入口 + `test_async_iterator`（14 项）。

执行中的三处修正，都是"先验设计被实测推翻"：

1. **await 入口的 handle 语义写错了**。A2 初稿的
   `await_on_completed(source, token, ...)` 把 continuation 注册到**迭代器自己的池化
   source** 上；实际该注册的是"迭代器当前 await 的对象"（`await Task.Yield()` 里的那个
   Task）。挂在 source 上等于挂错对象，状态机永不恢复。已改为 `AsyncTask` handle +
   `async_task_on_completed`，并补上原计划遗漏的 `AwaitUnsafeOnCompleted` 独立入口。
2. **A2-5 并发反例的预测错了**。"去掉 mutex → 不稳定"是错的验收标准：前两版测试去掉
   mutex 后 50/50、40/40 **全绿**（零判别力），第三版改用计数式别名 oracle 才 6/6
   确定性变红。详见设计文档 §3 的修正表。
3. **`Create()` 泄漏**。池是堆分配的，`Complete()` 不释放（状态机可二次枚举）。
   补 `destroy` 入口作为唯一释放点。

A2 的反例取证（每条都实机执行、并已 diff 校验回退干净）：

| 反例 | 结果 |
|------|------|
| 去掉 `Reset()` 的 `++version` | **3 项确定性变红**（Recycled/Stale/TokenFromEntryPoints） |
| 去掉池 mutex | `ConcurrentAcquireNeverDoubleHandsASlot` **6/6 确定性变红** |
| 去掉池溢出分配路径 | **别名被直接检出**（6 组指针相同） |
| 去掉 await 的注册调用 | resume 测试 + `EXPECT_DEATH` 守卫 **双红** |

**A3 交付**：`AsyncIteratorMethodBuilder` 的 5 个 op 全部注册进
`RuntimeHelperShapeRegistry`（`Create` / `Complete` / `MoveNext<SM>` /
`AwaitOnCompleted<Ta,SM>` / `AwaitUnsafeOnCompleted<Ta,SM>`），并**放宽**
A1 加的 `TryGetAsyncStateMachineTypeName` 全量拒绝——只对 MoveNext/Await 解析 `<SM>`，
Create/Complete 仍返回 false（它们不带泛型参数，乱接受会让符号解析指向错误状态机）。

A3 的三处关键设计（都是"先验被实测逼出来"的）：

1. **A1 的一刀切必须被精确化，不能直接删**。A1 让
   `TryGetAsyncStateMachineTypeName` 对**所有** iterator callee 返回 false，这同时打断了
   `TryResolveAsyncRuntimeContinuationMethod` → 使 iterator 的 MoveNext 符号不可解析。
   A3 不是"恢复旧行为"，而是按 op 分辨：MoveNext/Await 解析，Create/Complete 拒绝。
2. **`AwaitUnsafeOnCompleted` 必须独立注册**。两个拼写签名相同、共用一个 native 实现，
   但只注册一个会让另一个的 await 落到 `ChaosExternalRuntimeFallback` → 0，
   迭代器在该 await 处**静默停止推进**。
3. **await 入口注册在"被 await 的对象"上**，不是迭代器自己的池化 source——
   这条在 A2 已修正过一次，A3 的 registry 侧沿用同一语义
   （`*resolve_native_int_slot(chaos_arg_0)` 取出 awaited task handle）。

A3 的反例取证（每条都实机执行、并已 diff 校验回退干净）：

| 反例 | 结果 |
|------|------|
| 从注册 `foreach` 数组删掉 `AwaitUnsafeOnCompleted` | `BothAwaitSpellings(AwaitUnsafeOnCompleted)` **变红，另一行保持绿** |
| 把 MoveNext resolver 的 native 符号改成 `chaos_BOGUS_*` | `MoveNextResolverEmitsTheRealNativeSymbol` **变红** |
| 把 await resolver 体内的 `{mnSym}` 换成 `0` | `BothAwaitSpellings` **两行全红** |
| 从解析器数组删掉 `"AwaitOnCompleted"` | 解析层 **2 项变红** |
| 解析器改成"全接受"（返回假名 `"Bogus"`） | Create/Complete 负向断言 **1 项变红** |

> **A3 的测试教训（值得记住）**：registry 层最初的 MoveNext 测试**只断言 descriptor**
> （`Assert.Equal("MoveNext", descriptor.MethodName)`），**不断言产出的符号**。
> 反例实测打脸——把 native 符号改成 `chaos_BOGUS_*` 后该测试**仍然全绿**：
> 注册照常匹配，只有真正链接时才会炸。断言"匹配上了"与断言"产出正确"是两回事，
> 前者对符号错误零判别力。已改为驱动**真实 `Create()`** 构造 planner、
> 解析并断言 `CppSource` 内的真实符号（`CreatePlannerWithIteratorMoveNext`）。
> 最初版本还传了 `null` planner，那只是证明 resolver 会崩——同样是假绿。

## 当前通过测试

| 测试套 | 结果 |
|--------|------|
| codegen 全量 | **2199 通过 / 5 失败（均为预存在）**；详见下方基线说明 |
| `AsyncIteratorDetectionTests` (A1) | **3/3 PASS** |
| `test_async_task_state` (1-4) | **8/8 PASS** |
| `test_async_task_exception` (1-3) | **8/8 PASS** |
| `test_async_task_run_e2e` (1-1) | **5/5 PASS** |
| `test_async_task_blocking` (1-2) | **5/5 PASS** |
| `test_async_when` (P2-1) | **14/14 PASS** |
| `test_async_when_async` (P2-1) | **7/7 PASS** |
| `test_async_when_array` (P2-3) | **8/8 PASS** |
| `test_async_continue_with` (P2-2) | ⚠️ **10 通过 / 2 失败（均为预存在）** — 见下方修正 |
| `test_async_task_factory` (P2-5) | **6/6 PASS** |
| `test_async_iterator` (P2-8 A2) | **14/14 PASS**（5 次重复） |
| codegen A3 注册/解析（P2-8 A3） | **13/13 PASS**（6 解析层 + 3 registry 层 + 2 await 拼写 + 2 负向） |
| `test_async_integration_smoke` | **28/28 PASS** |

async 线合计 **128 项**：126 绿 + 2 预存在红（A3 新增 13 项 codegen 测试）。

> ⚠️ `test_async_continue_with` 基线修正：此前记的 **12/12** 有误。A2 期间实跑为
> **10 通过 / 2 失败**（`AsyncContinueWith.ContinuationTaskCarriesTheContinuationsReturnValue`、
> `AsyncContinueWith.ContinuationsChain`）。**这两个失败是预存在的**：在 HEAD `db68e9ad1`
> 的干净 detached worktree（无任何 A2 改动）上实跑复现同一组 2 个失败，**已验证，不是推断**。
> A2 未触碰 `async.h` / `continue_with` 路径，这两个用例从未在干净基线上核对过——
> 与 A1 那次"12/12"一样，是把主检出或更早的记录当成了 worktree 基线。
> 教训同 A1 第 2 条：**基线数字必须在本 worktree 实跑获得。**

> ⚠️ codegen 基线修正：此前记的 **2188/2188** 是在**主检出**上跑出的数字。worktree 内
> 实跑为 **2186 通过 / 5 失败 / 2191 总计**。5 个失败
> （`AsyncPipelineTests.MovenextEmittedSource_*`：`ContainsStateMachineFields` /
> `SetResultAndSetExceptionRouteToNativeAsyncBuilder` /
> `ContainsSetResultAndAwaitUnsafeOnCompleted` / `BoxPointerPassedByValueNotStackSlot` /
> `SuspendReturnsAfterAwaitUnsafeOnCompleted`）是**预存在的**：在 HEAD 的干净 detached
> worktree（无任何 A1 改动）上逐一复现同一组 5 个失败，**已验证，不是推断**。
> 症状是 emission 产出 `// AOT-unreachable stub` 而非真实 MoveNext 体——
> 与 A1 的 "BuildMethodSourceSafe 吞异常" 是同一机制的不同受害面，值得单独专项。
> 新增的 codegen 测试计数（2191 = 2188 + A1 的 3 项）也确认 A1 未误伤既有用例。
>
> **A3 复核方式**：这次不再建 detached worktree（在 `/tmp` 下 `dotnet test` 会因
> `Could not locate repository root (.git directory)` 而**全量假红**——该路径下 386 项
> 报错，完全不可用；上一轮 codegen 基线修正用的就是这种方式，其数字同样不可信）。
> 改为**就地回退**：备份 5 个文件 → `git checkout --` → 跑基线
> （**2186 通过 / 5 失败**，失败集与 A3 后**逐一相同**）→ 回填复核。A3 后为
> **2199 通过 / 5 失败**，即 +13 项新测试、0 回归。

> ⚠️ native 构建注意：`artifacts/build/rtnative` 未设 `ROADMAP0_PRESET_TARGET`，
> 因此顶层 CMakeLists 的 googletest FetchContent 块被跳过，**该树下所有 gtest 目标
> 都编译失败**（C1083 gtest/gtest.h）—— 与本次改动无关，是预存在的构建树配置问题。
> 跑 native async 测试请用 `artifacts/presets/windows-x64-reference`（已设该变量且有 `_deps`）。

基线修正：此前文档记的 codegen 2165/2165 是在**主检出**上跑出的数字；worktree 内正确
解析后为 **2172/2172**（1-6 修复后）。

## 关键文档

- `design-v1-01.md` — 完整 7 Phase 设计 + V1-V5 验收标准
- `assessment-2026-09-11.md` — 基线评估报告
- `roadmap-v1-01.md` — 阶段框架（已恢复）
- `handoff-2026-09-10-hotupdate-slot-mapping.md` — hotupdate 专项

## 下一步

Phase 2 剩余：`WhenEach` + `Task.Factory` 剩余族（`ContinueWhenAll/Any` 32 个、
`FromAsync` 22 个无原生模型、4 个属性需真实 factory 对象模型）。

P2-4 经验：**registry 级测试必须用管线真实产生的 callee 拼写**，
否则会出现"单测绿、管线 null"的假绿。新增 ContinueWith 类 helper 时照此办理。

P2-5 经验：**诊断结论必须在真实管线上复核**。直接调用内部匹配函数并喂手写 callee
得到的"命中/顺序"结果，与管线真实行为可能不一致（P2-5 的前缀碰撞假设就是这样被误导的）。
反例验证（删注册看是否变红）比阅读代码更能确定承重点。

P2-6 经验：**测试的 fixture 解析本身要有反例意识**。`LocateAsyncAssemblyDll` 静默回退 Debug，
使 Release 跑测读到陈旧 DLL —— 全绿是假绿，且掩盖了两个真实缺陷。
定位类 helper 必须修成"解析不到就报错"，而不是"解析不到就用默认值"。

P2-7 经验：**检测器的"形状假设"要靠真实 IL 验证**。
`IdentifyAsyncBoxPointerLocalSlots` 假定 async entry 一定有 `newobj`，而 Roslyn 对不逃逸的
async 方法从不发 `newobj` —— 检测器静默不匹配，两个 R2b 测试却因读陈旧 fixture 而假绿。
**断言要用 `ilspycmd -il` 对拍真实 IL，而不是从源码语义推断 IL 形状**。
另：`Assert.Contains("chaos_locals[0]")` 在 `&chaos_locals[0]` 面前**也通过**（子串）；
"断言某个坏模式不存在" 之外还要 "断言好模式存在"，否则断言不具鉴别力。

A1 经验（两条，都值得推广）：
1. **在 `BuildMethodSourceSafe` 之下，抛异常 ≠ 响亮失败。** 任何从 emission 抛出的异常
   都会被吞掉并替换成 stub，构建保持绿。要造"显式失败"，必须走**非抛异常**通道
   （记录 + emit 符号）。反过来说：凡是"A1 式响亮失败"的设计，先问一句
   "这个抛会被谁吞掉"。
2. **"这个失败是预存在的吗"必须实测，不能推断。** A1 提交前 codegen 全量出现 5 个
   与 async 相关的失败，第一反应是"我改的"。用 `git worktree add --detach HEAD` 建一个
   干净基线树跑同一组测试，逐一复现同样 5 个失败，才敢断定与本次改动无关。
   注意：**不要用 `git stash` 做这件事**——stash 栈与其他 worktree/会话共享，
   可能 pop 掉别人的改动。`git worktree` 是安全且零共享状态的替代。

```yaml
recommended_next_child: ASYNC-P2-8-A2
```
