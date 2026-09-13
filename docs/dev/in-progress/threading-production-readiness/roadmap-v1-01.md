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
| `T0.0` | 0 | ready | main | 实测 build 错误（**完整重建**，非读日志） | — | — | 主工作区跑，不切 worktree | 实测错误文本 + 错误类型判定 | 拿到真实编译错误，判定是否 crt_stubs | 只读，无写出 | 中 |
| `T0.1` | 0 | planned | tbd | 清除源树陈旧假绿产物 | T0.0 | batch-1 | `rm -f` 显式列举，禁用 `git clean` | 三个 chunk 源树 `results/fact.json` 已删 | 源树下三路径均不存在 | `tests/e2e/translation/System.Private.CoreLib/chunks/threading*/results/` | 小 |
| `T0.2` | 0 | planned | tbd | 重建三个 threading chunk | T0.1 | batch-1 | 重建后记录构建耗时（判定单/双 worktree） | 产物根 `fact.json` × 3 | `build.status=passed`；fact.json mtime > provenance.json | `artifacts/foundation-dll/`（不入仓库） | 大 |
| `T0.3` | 0 | planned | tbd | 取证 `BuildMethodSourceSafe` 是否吞 threading 异常 | T0.0 | batch-1 | 独立取证，不依赖 T0.2 结果 | 取证报告 | 明确结论：吞 / 不吞 | `src/managed/Chaos.IL2CPP.Generator/`（只读优先） | 中 |
| `T0.4` | 0 | planned | tbd | 新建 threading native test workflow | T0.2 | batch-2 | 独立 workflow，不进 ci-framework；首跑 `continue-on-error` | `.github/workflows/threading-native-tests.yml` | CI 跑出 244 用例结果 | `.github/workflows/threading-native-tests.yml` | 中 |
| `T1.1` | 1 | planned | tbd | `CancellationToken.throw_if_cancellation_requested` 真实抛出 | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | revert 后测试失败 | `src/native/runtime-core/cancellation_token.cpp` | 中 |
| `T1.2` | 1 | planned | tbd | `source_get_token` 构造真实 token | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | 同上 | `src/native/runtime-core/cancellation_token.cpp` | 小 |
| `T1.3` | 1 | planned | tbd | `CreateLinkedTokenSource` 实现或显式拒绝 | T0.4 | batch-3 | 反例验证 | `cancellation_token.cpp` + 测试 | 同上 | `src/native/runtime-core/cancellation_token.cpp` | 中 |
| `T1.4` | 1 | planned | tbd | `Parallel` failed 接线 + 结果返回 + 异常传播 | T0.4 | batch-3 | 反例验证 | `parallel.cpp` + 测试 | 同上 | `src/native/runtime-core/parallel.cpp` | 大 |
| `T1.5` | 1 | planned | tbd | `SynchronizationContext::Post` 真实入队 | T0.4 | batch-3 | 反例验证 | `synchronization_context.cpp` + 测试 | 同上 | `src/native/runtime-core/synchronization_context.cpp` | 中 |
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
| `T4.1` | 4 | planned | tbd | threading chunk 门禁阈值 + 回归告警 | T3.* | batch-9 | — | 阈值配置 + 告警接入 | 阈值生效；告警可触发 | `.github/workflows/`、验证管线配置 | 中 |

**status 说明**：`T0.0` = `ready`（无前置依赖，可立即启动）；其余 `planned`。

---

## 7. 依赖

```
T0.0 ──┬─→ T0.1 ─→ T0.2 ─→ T0.4 ─→ T1.1..T1.5 ─→ T2.0 ─→ T2.1 ─┬─→ T2.2, T2.3
       │                          （Phase 1）                  ├─→ T2.4, T2.5
       └─→ T0.3（独立取证）                                     │
                                                                └─→ T3.1..T3.5 ─→ T4.1
```

**关键串行点**：
- `T0.0 → T0.1`：T0.0 的结果决定 Phase 0 形状（若错误类型非 crt_stubs，T0.1 需重评）
- `T0.2 → T0.4`：CI workflow 需要知道实际的测试规模与失败基线
- `T1.* → T2.0`：Phase 2 依赖 Phase 1 关闭的语义造假（否则 ABI 接线接的是错的语义）
- `T2.1 → T2.2..T2.5`：句柄映射机制是所有原语注册的前置

**独立点**：`T0.3` 与 T0.1/T0.2 无依赖（只依赖 T0.0 的实测结果）

---

## 8. 风险

| 风险 | 触发条件 | 后果 | 缓解 |
|---|---|---|---|
| T0.0 根因与推断不符 | 实测错误非 crt_stubs | Phase 0 形状重写 | T0.0 设为首任务；**不得**无实测开工 T0.1 |
| T0.0 实际是完整重建 | 无构建日志可读 | 耗时不符预期 | 按「完整重建」预估耗时 |
| 重建后意外转绿 | 错误为暂态 | 根因推断落空 | 转绿**不等于**推断正确；必须回溯确认生效改动 |
| **假绿向量（源树残留）** | 陈旧 `fact.json` 未清 | chunk 实际 `build.status=error` 却被判通过 | T0.1 清除 + 验收读取根钉死 + mtime/runId 断言 |
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
