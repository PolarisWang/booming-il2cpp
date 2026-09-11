# System.Threading.Tasks 生产级工程 — Brainstorm 设计

> 版本：v1-01 | 创建：2026-09-11
> 上游：`assessment-2026-09-11.md`（事实基线）+ brainstorm 会话
> 状态：blocking_questions = []，question_clearance = cleared

---

## 一、背景与动机

评估报告确认：当前 async/Task AOT 翻译处于"基础设施就绪、语义严重不完整"状态（生产级合格度 ~25%）。~85% 的 Task API surface 走 `ChaosExternalRuntimeFallback` → 0。已实现的 builder/state-machine 部分存在 **"STATUS 声称 landed 但 HEAD 无对应代码"** 的回归，以及 **stub 返回恒值掩盖语义缺陷** 的问题。

目标：覆盖到 `Task`、`Task<T>`、`ValueTask`、`TaskFactory`、`CancellationToken`、`Context` 等完整 TPL surface，与 .NET 8 API surface 完全一致（不含已确定为非目标的项目），通过 foundation-dll 反假绿门禁，AOT 性能 < 2× .NET 8。

---

## 二、已锁定决策（不再重议）

| 决策 | 来源 | 内容 |
|------|------|------|
| 状态机方案 | roadmap-v1-01 | 手工状态机翻译，不做 C++20 coroutine |
| 非目标 | brainstorm | `IAsyncEnumerable`/`await foreach`/`IAsyncDisposable`/`await using`/`Pipelines`/`Channel<T>`/`TaskScheduler` 自定义调度器/`ManagedThreadPool` — 仍为非目标，但与 Q2=B 冲突时以 B 为准 |
| Q1 止血优先 | 用户拍板 | ### Q1=A（先恢复被抹除 P3 代码，再以可信基线往下做） |
| Q2 全覆盖 | 用户拍板 | ### Q2=B（TaskFactory 21 methods + ContinueWith 12 + ValueTask 6，与 .NET API surface 一致） |
| Q3 严格串行 | 用户拍板 | ### Q3=B（单一 worktree，严格串行推进） |
| Q4 验证后置 | 用户拍板 | ### Q4=B（Phase 5 统一建反假绿门禁，Phase 1-4 靠现有 fact + 人工审查推进，接受返工风险） |
| 验收反例 TDD | 用户拍板 | ### 决策2=A——每个子任务须附反例测试，作为 done 定义 |
| 防覆盖门禁 | 用户拍板 | ### 决策1=B——不加流程门禁，靠提交前手动 `git diff --cached` 检查 |
| 三优先级 | roadmap-v1-01 | P1（性能最优）> P2（架构完美）> P3（热更适配）；本次具体裁决：先正确性后性能 |

---

## 三、阶段切分与验收标准

### 通用验收维度（每个 Phase 必须全过）

| # | 维度 | 判据 | 反例 |
|---|------|------|------|
| V1 | 真实执行 | 该 Phase 的 API 有真实 C# subject（非 `return 42L`），AOT + JIT 双模式跑通 | subject 返回常量 42L 且无 native symbol 调用链 |
| V2 | 语义正确 | 返回值/副作用与 .NET 8 行为一致，有断言证明 | `await Task<int>` 得 0 但测试只断言"完成" |
| V3 | 代码可达 | registry entry 存在，生成的 C++ 含目标 native symbol（非 `ChaosExternalRuntimeFallback`） | registry 无 entry，代码走 fallback → 0 |
| V4 | 无回归 | `git diff` 可追溯；既有测试不减少不退化 | p3-3 代码落地后另一提交静默覆盖 |
| V5 | 证据留痕 | 提交 message 含可复现命令；测试输出粘入 STATUS.md | 只写"PASS"无复现路径 |

### Phase 0 — 止血与基线恢复（1-2 天）
**目标**：让 HEAD 回到"可信基线"，消除虚假声明

| 子项 | 内容 | 验收判据 |
|------|------|---------|
| 0-1 | 恢复被 ci-r7-clean 覆盖的 P3 实现 | `async_stubs.cpp` ≥ 333 行；`grep -c chaos_task_when_all` > 0；S16 `grep -c WhenAll` = 4；smoke ≥ 26 |
| 0-2 | 修复语义造假（stub 恒值问题） | `await` 未完成 task 时 `get_IsCompleted` 返 0（非恒 1）；`Task.FromResult(42).Result` 得 42 |
| 0-3 | 修复已知缺陷 D2/D3 | 无锁读已加锁；连续 2 次 init/shutdown/init 测试通过 |
| 0-4 | 修 `threading-tasks` build | `[TPG:err] error C2660`（`chaos_tcs_*` 参数数）归零 |
| 0-5 | 更新 STATUS.md 到真实基线 | 每条已完成附 file:line 证据 |
| 0-6 | （按决策1=B 不做防覆盖门禁） | — |

**退出 = 5 项全绿 + smoke 从 22 → ≥26**

### Phase 1 — 核心执行语义（2-3 周）
**目标**：`Task.Run` / `Task.Wait` / `Task.Result` / 异常传播 / `Task.FromResult/FromException`

| # | 内容 | 验收反例 |
|---|------|---------|
| 1-1 | `Task.Run` registry （native 已完整，缺接线） | `Task.Run(() => 42).Result` → 42，且生成 C++ 含 `async_task_run_fn` 调用 |
| 1-2 | `Task<T>::Result` / `Task::Wait` | **阻塞语义**：`Task.Run(长任务).Wait()` 断言耗时 ≥ 任务执行时间；`Result` 返回真值 |
| 1-3 | **异常传播** 🔴 | `await Task.FromException<X>()` **必须抛 X**；`Task.Wait()` 抛 `AggregateException`；当前静默返 0 是反例 |
| 1-4 | 三态区分 | cancelled / faulted / resolved 三种场景断言值互不相同 |

**退出 = 1-3 反例通过（这一条是整条路径最重要的验收点）**

### Phase 2 — 组合子（3-4 周）
**目标**：WhenAll/WhenAny/WhenEach + ContinueWith 12 overloads + Task.From* + Task.Factory 21 methods

| # | 验收反例（选代表） |
|---|-----------------|
| WhenAll | 3 task 各返 1/2/3 → 结果集 = `[1,2,3]`（不只能断言"完成了"） |
| WhenAny | winner index 正确 |
| WhenAll 失败 | 子 fault → 聚合 fault 异常传播 |
| ContinueWith | 12 overloads 代表用例每个确实收到前序结果并正确回调 |

### Phase 3 — 取消与上下文（2-3 周）
**目标**：CancellationToken 全套 registry + SyncCtx + AsyncLocal + ConfigureAwait + OperationCanceledException

| # | 验收反例 |
|---|---------|
| CT | `Task.Delay(10s, ct).Cancel()` → ~10ms 内抛异常，不耗时 10s |
| ConfigureAwait(false) | 恢复线程 ≠ 原线程 |
| AsyncLocal | 跨 await 值保持：设 1 → await → 仍 1 |
| OperationCanceledException | ct 参与的正确 fault 类型 |

### Phase 4 — ValueTask 与长尾（3-4 周）
**目标**：ValueTask(6) + TaskToAsyncResult(3) + TaskExtensions.Unwrap + ConcurrentExclusiveSchedulerPair + UnobservedTaskExceptionEventArgs

### Phase 5 — 全量验证与门禁（3 周）
**目标**：反假绿门禁 + 回头重验 Phase 1-4 + Parallel 107/107 清零 + GC stress

| # | 验收 |
|---|------|
| 门禁自身 | 构造 `return 42L` 假 subject → 门禁判 FAIL |
| Phase 1-4 重验 | 每 API 重跑专属反例；允许发现返工 |
| Parallel 清零 | 107/107 重做，新计数有断言支撑 |
| GC stress | 100k Task 分配 ASan/Valgrind 无 UAF/泄漏 |

### Phase 6 — 性能（3 周）
**目标**：AOT < 2× .NET 8 + AsyncTask GC ownership + Frame 池化 + benchmark

---

## 四、Phase 内部子任务拆法

根据决策 2-A（TDD with 反例测试），每个子任务统一形态：

```
1. 写反例测试：当前行为上，assert 它失败
2. 实现 API
3. 跑反例测试 → GREEN
4. V1-V5 自查 + 更新 STATUS.md
```

每个子任务 submission 必须包含：测试输出 log / `git diff --cached` 扫描无意外删除 / 既有测试数不减少。

---

## 五、风险评估

| 风险 | 等级 | 缓解 |
|------|------|------|
| 同类覆盖再次发生（无门禁） | 🔴 高 | 靠提交前手动 `git diff --cached` 检查（决策1-B）。接受风险。 |
| Phase 5 返工量大 | 🟡 中 | 已接受。Phase 1-4 每个 API 落地时**顺手写真实 subject**（不留给 Phase 5）|
| feccdb166 恢复冲突 | 🟡 中 | Phase 0 逐文件恢复 + 叠加 WIP commit，逐提交 build 验证 |
| 200+ API 工期失控 | 🟡 中 | 按 Phase 分批交付、每 Phase 独立验收 |
| hotupdate slot mapping 未解 | 🟡 中 | 独立于主线，Phase 3 或 Phase 5 专项切入 |

---

## 六、三优先级权衡结论

已在 roadmap-v1-01 定案，本次 brainstorm 补充执行期细则：

| 场景 | 裁决 |
|------|------|
| stub 语义 vs 性能 | **先正确性后性能**。Phase 0 修的恒值 stub 直接阻碍正确执行；Phase 6 才做性能 |
| API 覆盖面 vs 每个 API 深度 | 全覆盖（Q2=B），但同一 Phase 内先做最常用的 overload，然后补其余变体 |
| 异常路径 vs 正常路径 | **异常路径先于正常路径**（Phase 1-3 必须验证异常语义正确才能认为"正常路径可用"）|
| 验证门禁 vs 实现进度 | 实现领先于验证（Q4=B），接受跳跃交付。Phase 5 返工是预期成本 |

---

## 关键文档

- `roadmap-v1-01.md`（权威阶段划分框架，本设计基于其扩展）
- `STATUS.md`（主状态追踪，由本设计覆盖为最新）
- `assessment-2026-09-11.md`（事实基线报告）
- 旧版归档：`STATUS-archive-2026-09-10.md`（HEAD 被覆盖前的 STATUS 备份）
- `handoff-2026-09-10-hotupdate-slot-mapping.md`（hotupdate 专项）

---

## 下一步

编制 roadmap（调用 `dev-roadmap`），将 7 个 Phase 拆解到可执行的子任务，每个子任务按上述统一形态（反例测试 → 实现 → V1-V5 自查）定义。