# STATUS — Threading 生产级就绪

## 元信息

```yaml
task_id: threading-production-readiness
task_type: roadmap
phase: roadmap
roadmap_or_plan: docs/dev/in-progress/threading-production-readiness/roadmap-v1-01.md
design: docs/dev/in-progress/threading-production-readiness/design-v1-01.md
created: 2026-09-13
updated: 2026-09-13
created_by: user
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: T0.0
latest_stop_point: roadmap 已产出，等待启动 T0.0
```

## 最近摘要

对抗性审查（3 阻断级 + 6 需修正 + 12 确认无误）**已全部闭环**，
`roadmap-v1-01.md` 已产出：**22 个子任务 / 5 个阶段 / `dispatch_model: sequential`**。

用户拍板：**Phase 2 保留在 roadmap 内**（不拆出独立任务）；
Phase 0 拆 4 个 T0.x + 检查点。

**发现的最严重问题**：源树陈旧 `fact.json` 构成**活着的假绿向量**
（`reporting.py:57` 读源树而非产物根），已转为 **T0.1** 并钉死验收读取根。

## 下一步

**启动 `T0.0`** —— 主工作区执行，完整重建，拿实测编译错误。
完成后**立即切 worktree**（`EnterWorktree(name=threading-phase0)`），再进 T0.1。

## 问题来源

用户要求「整体看下 thread 的功能完善情况，如果要达到生产机的开发品质，还需要做些什么？」。
产出一份综合分析报告后，用户要求用 dev-brainstorm 规划并整体 review。

**分析报告的关键结论**：Threading 是全仓库**最不可信**的一族——不是实现最差，而是**已知最少**。
内核（ThreadPool / TimerQueue / Monitor / Interlocked / WaitHandle）已是真实工业实现，
但「验证面」与「翻译面」两头断裂。

## 边界拍板

### 做什么

全量 Phase 0-4：

| 阶段 | 内容 |
|---|---|
| Phase 0 | 止血：实测 build 错误、**清除源树陈旧假绿产物**、重建 3 chunk、堵假绿向量、CI 接入 |
| Phase 1 | 关闭语义造假（CancellationToken 3 点 / Parallel / SynchronizationContext） |
| Phase 2 | 接线 **缺 ABI 出口层**的原语（**非「未注册」——部分已在 ShapeRegistry**；成本大幅上调，见 design §12） |
| Phase 3 | 加固（非原子 static / 句柄表 / 热路径 fprintf / 文档 / benchmark） |
| Phase 4 | 制度化（门禁阈值 / 回归告警） |

### 不做什么
- **hotupdate slot mapping** → 用户拍板**拆出独立任务**
- **POSIX 平台缺陷** → 仅 Windows 目标，登记 watch_item
- **Thread.Abort 语义** → .NET Core 已弃用该 API
- **前缀过捕获** → 审计确认未发生，属误报

## worktree 策略（2026-09-13 增补）

**切出时机：T0.0 完成、拿到实测编译错误之后立即切，不早于也不晚于。**

| 工作区 | 覆盖 |
|---|---|
| 主工作区（`main`） | T0.0 只读取证 —— 需要已有的构建缓存与工具链 |
| `worktree-A` | Phase 0 代码改动（T0.1-T0.4） |
| `worktree-B` | Phase 1-4；与 A 经 main 的 merge 交接 |

**不早切**：T0.0 是纯读取证，不产生改动，无需隔离。
**不晚切**：T0.1 起产生代码改动，而主工作区**已有并发 agent 活动**（且直接触及 threading —— 见 design §8.2），
暴露在 `parallel-agent-clean-checkout-clobber` 记录的整树覆盖竞态下。

**已知成本**：worktree 的 `artifacts/` 与 `testing/artifacts/` 均为 gitignored 且不共享，
工具链与 chunk 产物需冷重建。**若 T0.2 实测构建 > 30 分钟，退化为单 worktree 覆盖 Phase 0-4。**

详见 `design-v1-01.md` §3.1。

## authority 决策

| 决策点 | 结论 |
|---|---|
| 权威 partition | `tests/e2e/translation/.../namespace-partition.json`（200/chunk，**有 threading-2**） |
| `manifest.py:110-113` | 仅为**默认命名映射表**，非权威 chunk 来源 |
| 源根 / 产物根 | `foundation_root()` = `tests/e2e/translation/`；`build_root()` = `artifacts/foundation-dll/` |
| 修改授权 | **四层全开**（native / codegen / 验证管线 / CI） |

## 当前结论

**三个 threading chunk 全部 `build.status = error`**，`results/` 下无 `fact.json`；
`threading` 与 `threading-2` 在 2026-09-12 honest report 中**完全缺席**（70 行差集的唯一两个）；
**28 个 native threading 测试文件 / 244 用例**（原写 15 文件 / 126 用例，已按实测修正）**零 CI 门禁**。

**⚠️ 诚实标注**：build 失败的**具体编译错误未实测**，报告原因为推断。计划 Task 0 即为此设。

### 🔴 活着的假绿向量（对抗性审查 R1，2026-09-13 实测确认）

`reporting.py:57` 读的是 `foundation_dir/asm/chunks`，而 `foundation_dir`
（`chunk_pipeline.py:321` → `foundation_root()`）是**源树**
`tests/e2e/translation/`，**不是产物根** `artifacts/foundation-dll/`。

现场证据：`tests/e2e/translation/.../threading-tasks/results/fact.json`
存在（**mtime 2026-09-08 14:54**，`passed:456`），而产物根下
`provenance.json` 显示 `build.status: error` 且**无 fact.json**。

⇒ honest report 中 `threading-tasks ... PASS` 那一行读的是**五天前的源树残留**。
该文件已被 `.gitignore:600` 忽略、未被 git 跟踪 ⇒ **不会随 `git status` 暴露**。

**处置**：Phase 0 新增 **T0.1 清除源树陈旧 fact.json**；验收读取根**钉死为产物根**，
并加 mtime/runId 一致性断言。详见 `design-v1-01.md` §7.1。

## 下一步入口

见上方「下一步」—— `roadmap-v1-01.md` 已产出，启动 **T0.0**。

## 风险评估摘要

| 风险 | 缓解 |
|---|---|
| T0.0 实测根因与推断不符 | Task 0 优先；**不得**无实测即开工 T0.2 |
| `BuildMethodSourceSafe` 是假绿源头 | 已拍板提到 Phase 0，取证优先于 rebuild |
| Phase 2 接线后 real% 不升 | 专项取证；三次规则后转 dev-brainstorm |
| 重建触发 cross-chunk 回归 | 单 chunk 隔离验证 + honest report 前后对比 |
| 244 用例首跑大量失败 | 首跑 `continue-on-error` 收基线，再定阻断阈值 |
| 主工作区被并发 agent 抹除改动 | **T0.0 后立即切 worktree**；commit 前核对 `git diff --cached` |
| worktree 冷构建成本超预期 | T0.2 首步实测耗时；> 30 分钟则退化单 worktree |

## 三优先级权衡结论

采纳**方案 B（阶段内并行、阶段间串行）**。

理由：threading 是运行时基础设施，其正确性影响所有上层 —— 性能（P1）的前提是语义正确，
而当前存在**会静默产出错误结果**的 P0 缺陷，必须先于性能优化处理。
P2（架构完美）体现为阶段门禁不放松；P3（HotUpdate）无冲突，该问题已拆出。
**无优先级冲突需裁决。**

## 已完成

- ✅ 三路并行审计（native 实现面 / 验证覆盖度 / codegen 翻译缺口）
- ✅ 证据链核验（partition 权威性、nightly 消费路径、chunk 缺席差集、crt_stubs 旧签名）
- ✅ 综合分析报告交付
- ✅ `design-v1-01.md` 写入
- ✅ `blocking_questions = []`，用户确认清零
- ✅ **对抗性审查（2026-09-13）**：3 阻断级 + 6 需修正 + 12 确认无误，**已全部处理**
  - R1 → §7.1 假绿向量 + 新增 T0.1
  - R2 → Phase 2 表述修正（「杠杆最大」撤销）
  - R3 → 偏差声明精确措辞 + 基线漂移披露
  - Y1 → 测试规模 126 → **244**
  - Y2 → 43% 分母声明
  - Y3 → §8.1 T0.0 真实形态（无日志，是完整重建）
  - Y4 → d5d0630da 外推标注
  - Y5 → §12「未注册」→「缺 ABI 出口」
  - Y6 → §8.2 并发 agent 触及 `Part1.S16.cs`

## 遗留（非本计划）

- `chaos_continuation` 回归测试仍未落地（`bb9a02fd0` 无测试守护）
- `pcdistpatch-cond-exit-target-dropped` 未修
- 其余 32 个陈旧 chunk 重建（属 `chaos-continuation-scope` 任务）
