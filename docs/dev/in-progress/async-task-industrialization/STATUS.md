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

### Phase 2 附带修复的真实缺陷（P2-4）

| 缺陷 | 说明 |
|------|------|
| **ContinueWith 只有 1/20 可用** | 原注册是 `Register()` 精确匹配，而管线走 `TryMatchGenericShape()`，所以**连唯一那个重载也没接上**。修后按族路由：仅委托重载 → native；带 CT/Options/Scheduler/state 的重载 → 返回 null 走解释器（避免"忽略参数照跑 body"的假绿） |
| **同一类型名两种拼写** | registry 级测试用 ``System.Action`1<...>``，真实管线用 `System.Action<...>`。只匹配反引号形式 → **单测绿、管线全 null**。这是 P2-4 新增管线级测试存在的直接原因 |
| **参数切分被嵌套泛型击穿** | `Split(',')` 把带程序集限定的泛型实参切碎：`Func\`2[[A, asm, Version=...],[B, ...]]` 1 个参数被切成 **10** 段 → `paramTypes.Count` 判断失真 → 静默不匹配。影响**所有多泛型实参的 helper**，不止 ContinueWith |

### 诚实负数结果（P2-4）

`IsAnyContinuationDelegate` 单独禁用**不改变任何测试结果** —— 当前 BCL 上没有"单参数但非委托"的 ContinueWith，arity 判断已经覆盖。该检查保留作为未来重载的守卫，注释已标注为**冗余**而非"承重"，不再声称其有效。

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
| `WhenAll<TResult>` 泛型重载 | ❌ 仅非泛型 `Task[]` 已接线 |
| `WhenEach` | ❌ 无实现 |
| `Task.Factory` 21 methods | ❌ 无实现 |
| `ContinueWith` 20 overloads | ✅ 逐族路由已定（1 族接线 / 4 族显式拒绝）；余下 15 个重载走同一 resolver 的 arity 判断，无需逐个登记 |

## 当前通过测试

| 测试套 | 结果 |
|--------|------|
| codegen 全量 | **2179/2179**（P2-4 +7；此前记的 3 项"预存在失败"实为环境/顺序相关，全量跑通过） |
| `test_async_task_state` (1-4) | **8/8 PASS** |
| `test_async_task_exception` (1-3) | **8/8 PASS** |
| `test_async_task_run_e2e` (1-1) | **5/5 PASS** |
| `test_async_task_blocking` (1-2) | **5/5 PASS** |
| `test_async_when` (P2-1) | **14/14 PASS** |
| `test_async_when_async` (P2-1) | **7/7 PASS** |
| `test_async_when_array` (P2-3) | **8/8 PASS** |
| `test_async_continue_with` (P2-2) | **12/12 PASS** |
| `test_async_integration_smoke` | **28/28 PASS** |

async 线合计 **95 项全绿**。

基线修正：此前文档记的 codegen 2165/2165 是在**主检出**上跑出的数字；worktree 内正确
解析后为 **2172/2172**（1-6 修复后）。

## 关键文档

- `design-v1-01.md` — 完整 7 Phase 设计 + V1-V5 验收标准
- `assessment-2026-09-11.md` — 基线评估报告
- `roadmap-v1-01.md` — 阶段框架（已恢复）
- `handoff-2026-09-10-hotupdate-slot-mapping.md` — hotupdate 专项

## 下一步

Phase 2 剩余：`WhenAll<TResult>` 泛型重载 + `WhenEach` + `Task.Factory` 21 methods。
其中 `Task.Factory` 是最大的未接线块（21 methods），需先确认 codegen 侧 `TaskFactory`
的 subject 是否可探测（查 ATG manifest），再决定 native 接线面。

P2-4 已建立的可复用经验：**registry 级测试必须用管线真实产生的 callee 拼写**，
否则会出现"单测绿、管线 null"的假绿。新增 ContinueWith 类 helper 时照此办理。

```yaml
recommended_next_child: ASYNC-P2-5
```