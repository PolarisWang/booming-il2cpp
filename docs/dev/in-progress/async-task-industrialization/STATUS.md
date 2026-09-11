# STATUS — System.Threading.Tasks 生产级工程

## 元信息

```yaml
task_id: async-task-industrialization
task_type: roadmap
phase: phase-0-止血
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

## 当前通过测试

| 测试套 | 结果 |
|--------|------|
| codegen 全量 | **2165/2165 PASS** |
| native async smoke | **28/28 PASS** (27 async + 1 D3 re-init) |
| threading-tasks chunk build | 待 foundation-dll pipeline 确认（依赖 uncommitted 改动） |

## 关键文档

- `design-v1-01.md` — 完整 7 Phase 设计 + V1-V5 验收标准
- `assessment-2026-09-11.md` — 基线评估报告
- `roadmap-v1-01.md` — 阶段框架（已恢复）
- `handoff-2026-09-10-hotupdate-slot-mapping.md` — hotupdate 专项

## 下一步

Phase 1（核心执行语义）：`Task.Run` / `Task.Wait` / `Task.Result` / **异常传播**。
首个子任务：`ASYNC-P1-1` — `Task.Run` registry 接线（已有完整 native 实现）。

```yaml
recommended_next_child: ASYNC-P1-1
```