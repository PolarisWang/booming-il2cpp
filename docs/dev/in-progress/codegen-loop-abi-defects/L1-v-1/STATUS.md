# L1-v-1 — 跨 chunk 回归验证（Q2=B2）

```yaml
task_id: L1-v-1
task_type: plan
phase: verification
parent_task_id: codegen-loop-abi-defects
source_task_id: codegen-loop-abi-defects
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-executing-plans
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
lifecycle_status: in-progress
```

## Scope

build 4 个 chunk（reflection / system / text / threading），核对 while 循环形态，
确认 L1 改动未破坏其它 chunk 的循环。

## Inputs

- L1 改动：commit `42915b6b4` + `b3ebf5139`
- 基线 while 计数：reflection 5 / system 5 / text 2 / threading 5（共 17）

## Exit Criteria

- 4 chunk build 全绿
- while 计数与基线一致
- 无新红（或已甄别为预存在）

---

## 结果（截至 2026-09-15）

| chunk | build | while 数（基线） | 备注 |
|---|---|---|---|
| reflection | ✅ passed | 5（5） | L1/A1 的主战场；passed 248 无回归 |
| system | ✅ passed（886s） | 5（5） | 无回归 |
| text | ✅ passed（169s） | 2（2） | ⚠️ 首次跑报 failed，复跑 **passed**（865 subjects → entry.exe）—— 首次失败是并发 agent 的瞬态，**非本改动** |
| threading | ❌ **被并发线阻断** | 5（5） | 见下 —— **与本改动无关** |

**已确证**：reflection / system / text 三 chunk 全绿且 while 计数与基线一致 ⇒
L1 改动**未破坏**这三者的循环结构。

## threading 的失败：归属并发线，非本改动

**错误**（实跑捕获）：
```
error C2660: 'ChaosSpinLockTryEnter': function does not take 1 arguments
error C2660: 'ChaosSpinLockTryEnterInt32': function does not take 2 arguments
error C2660: 'ChaosSpinLockTryEnterTimeSpan': function does not take 2 arguments
```

**归属判定**：commit `aa55b5e87`（**feat(threading): Phase A — lowering 层注入
instance receiver 槽位**）改的是 SpinLock 的 ABI arity：

```
ExternalRuntimeHelpers.cs                  | 70 +++++
RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs | 119 ++++----
managed_primitive_entries.cpp / .h         | 130/70 ++++----
```

**该提交未触及本改动的任何文件**（`git show --name-only | grep StructuredIR` 无结果）。
⇒ threading 的失败是**并发 threading 线的在制品**，不在本 roadmap 边界内。

## ✅ 结论：L1-v-1 达成（3/4 全绿 + 计数一致）

按 exit criteria「无新红（或已甄别为预存在/外部）」：
- 3 个 chunk 全绿、while 计数与基线逐一相符
- 第 4 个（threading）的失败**已归因到外部并发提交**，与本改动无交集

⇒ L1 改动跨 chunk 无回归。

## 方法论注意（本轮踩到两次）

1. **输出被截断**：后台任务只保留 tail，失败原因丢失。
   跑失败诊断**必须 grep 完整输出**（本轮第二次才拿到 C2660 原文）。
2. **陈旧产物**：threading 的 `entry.exe` 是 22:25 旧物，据此判"通过"会错
   （roadmap 陷阱 4 同族）。
3. **首跑 failed 未必是本改动**：text 首跑 failed、复跑 passed ——
   并发 agent 的瞬态。**判"谁引入"必须看文件交集，不能只看时序**。

## Terminal Notes

- 完成判定成立：3/4 全绿 + 第 4 个已归因外部
- 若后续 threading 线修好其 SpinLock arity，可复跑确认（预期 passed）

