# STATUS — System.Threading.Tasks 生产级工程

## 元信息

```yaml
task_id: async-task-industrialization
task_type: roadmap
phase: phase-1-核心执行语义
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

## 当前通过测试

| 测试套 | 结果 |
|--------|------|
| codegen 全量 | **2172/2172 PASS** |
| `test_async_task_state` (1-4) | **8/8 PASS** |
| `test_async_task_exception` (1-3) | **8/8 PASS** |
| `test_async_task_run_e2e` (1-1) | **5/5 PASS** |
| `test_async_task_blocking` (1-2) | **5/5 PASS** |
| `test_async_integration_smoke` | **28/28 PASS**（无回归） |

基线修正：此前文档记的 codegen 2165/2165 是在**主检出**上跑出的数字；worktree 内正确
解析后为 **2172/2172**（1-6 修复后）。

## 关键文档

- `design-v1-01.md` — 完整 7 Phase 设计 + V1-V5 验收标准
- `assessment-2026-09-11.md` — 基线评估报告
- `roadmap-v1-01.md` — 阶段框架（已恢复）
- `handoff-2026-09-10-hotupdate-slot-mapping.md` — hotupdate 专项

## 下一步

Phase 2 — 组合子：`WhenAll` / `WhenAny` / `WhenEach` + `ContinueWith` 12 overloads
+ `Task.From*` + `Task.Factory` 21 methods。

native 侧 `chaos_task_when_all` / `chaos_task_when_any` 已存在（P3-3），但**尚无反例测试**，
且 `WhenEach` / `ContinueWith` / `Task.Factory` 需逐个接线。先建 WhenAll/WhenAny 反例
（结果集 = `[1,2,3]`、winner index 正确、子 fault → 聚合 fault）。

```yaml
recommended_next_child: ASYNC-P2-1
```