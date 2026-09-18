# thread / async 线 — 收尾待办（2026-09-18）

> 本轮起点：清点中断的 thread 任务。**结论：thread worktree 全部已合入 main，无独有工作丢失。**
> 17 个 worktree + 16 个分支已清理（含 7 个「疑似独有」经 patch-id/blob 比对确认
> 已由 main 的重提交落地）。

## 一、已交付（本轮）

| 项 | commit | 验证 |
|---|---|---|
| WaitAll/WaitAny 超时重载不再被单槽 shim 静默丢弃 | `fb9a42033` | threading-tasks 355→**359**/456，AV 4→**0**，跨技术分歧 7→**3** |
| worktree / 分支清理 | — | `git worktree list` 仅剩 main；16 个 `worktree-*` 分支已删 |
| 三跑污染记录 + 根因闭环文档 | `984ea3343` `8b46175d2` | `system-threading-chunk-triage/baseline-2026-09-17-2017.md` |

## 二、threading-tasks chunk 当前缺口（456 项 / 97 红）

```
factoryGap 65   ← ATG fixture 域，非 runtime 缺陷
failed     29
nullArg     2
realDefect  1
```

### 方法分布（97 红）

| 数量 | 方法 | 归属 |
|---|---|---|
| 40 | `ContinueWith` | **T4 批次**（roadmap 预估 5-8 天） |
| 10 | `WaitAsync` | T5 |
| 8 | `Run` | T2 |
| 6 | `Delay` / `WhenAny` | T1 / T3 |
| 4 | `TrySetCanceled` + 2 `SetResult` + 2 `TrySetResult` + 1 `SetCanceled` | T5 (TCS) |
| 3 | `Wait` / `WhenAll` | T1 / T3 |
| 其余 | GetAwaiter/ConfigureAwait/FromResult/FromException/FromCanceled/AsTask/Preserve | T1/T5 |

**关键判断**：`ContinueWith` 一项占 40/97（41%），是单点最大缺口，对应 **T4 批次**。
roadmap 明确 T4 **不需要新 native 入口**（「已有混沌设备」），是纯接线批次 —— 性价比最高。

## 三、roadmap 批次状态

来源：`docs/dev/in-progress/async-task-industrialization/roadmap-v1-01.md`

| Batch | 内容 | 状态 | 依赖 |
|---|---|---|---|
| T1 | Delay / Wait / ConfigureAwait | ✅ 大部分已交付 | — |
| T2 | StartNew / Task.Run | ✅ 已交付（`524101330`） | T1 |
| T3 | WhenAll / WhenAny / WaitAll / WaitAny | ✅ 已交付 + 本轮补 AV 修复 | T1 |
| **T4** | **ContinueWith / ContinueWith\<TR\>** | ⏳ **未做**（40 红） | T1 |
| T5 | TaskCompletionSource / WaitAsync | ⏳ 部分（TCS .ctor/get_Task 残留） | T1 |

`recommended_next_child: ASYNC-P2-8-A2`（STATUS.md）

## 四、优先建议

### P1 — T4 ContinueWith（40 红，最大单块）
- **无需新 native 入口**（roadmap 原话：「已有混沌设备」）
- 参考 T3 的教训：**shim 槽数必须等于 callee arity**。
  `ContinueWith` 有 ~36 个重载（含泛型 `ContinueWith<TResult>`），
  若沿用「按首参一刀切」会重蹈 WaitAll 覆辙 —— **必须逐 arity 注册**。
- 分批：先做非泛型 `ContinueWith(Action)` 族，再做 `ContinueWith<TResult>`。

### P2 — 残余 3 项跨技术分歧（AOT=PASS / JIT=FAIL）
`WhenAll_71_Taskint_0`、`ValueTask::AsTask_5__0`、`ValueTask::Preserve_6__0`

方向与 WaitAll 相反（AOT 对、JIT 错），且 T3 已交付 —— 是**独立**的 JIT 侧发射点问题，
按「同一托管方法多处发射点」判别法（memory: `system-chunk-verification-loop-discipline`）
核对两份生成物。

### P3 — factoryGap 65（ATG 线，非 runtime）
Task 家族 probe 构造失败。**需要 ATG 侧修 fixture**，不是 codegen/runtime 能解的。
这条线独立于 T4，可并行。

## 五、⚠️ 执行前必读（本轮血泪）

### 并发污染（memory: `concurrent-pipeline-shares-artifacts-root`）
**跑 chunk 验证前先查并发**：
```bash
tasklist | grep -iE "MSBuild|cl\.exe"    # 应为 0
git log -1 --format='%h %ad'              # 有无他人新提交
```
三轮 system chunk 全部被并发会话污染（半成品 Generator / 产物覆盖 / 陈旧 exe）。
判据：数字在修复后仍不变 / `entry.exe` 早于 codegen / 编译错误自行消失。

### 管线两条命令（缺一不可）
```bash
CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/testing/foundation-dll \
PYTHONPATH=D:/agent/chaos-il2cpp/tests/e2e \
python tests/e2e/verification/chunk_pipeline.py \
  --assembly System.Private.CoreLib --chunk threading-tasks --stages build,fact
```

### system chunk 基线缺失
`system` chunk **目前无可信基线**（三次尝试均被污染）。若 T4 涉及 `System.Private.CoreLib`
的 system chunk 回归，需先解决并发问题再测。
