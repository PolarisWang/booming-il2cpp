# lock-handle-binding — 锁类「句柄未绑定」闸的按类型处置（升级为跨域设计）

> task_id: `lock-handle-binding`
> 类型：brainstorm → 跨域设计（运行时 native + CodeGen + ATG）
> 创建：2026-09-20
> phase: planned（设计已批准，计划已出，待执行）
> 上游：`threading-receiver-plan`（Phase A receiver 注入，已完成）、
>       `threading-tasks-hang`（同族 ATG 侧挂死诊断，已归档）

## 关键文档

- `plan-v1-01.md` — **本轮计划（执行入口）**
- `design-v1-01.md` — 设计
- `brainstorm-v1-01.md` — 立项背景

## 一、问题陈述

threading chunk 当前 **499/523 passed**，残余 21 failed 中 **14 项**集中在锁类
实例方法。它们**不是** receiver 未注入（Phase A 已修），而是**第二道独立的闸**：
锁的状态载体从未被创建。

根因是通用机制问题：`SubjectInstanceFactory.Create<T>()` 对引用类型统一返回
`GetUninitializedObject`（零初始化、**不跑构造函数**）。影响面远超 threading：
system chunk 有 65 个类型 / 1065 个调用点。

## 二、边界拍板

| 项 | 裁决 |
|---|---|
| 与 `threading-receiver-plan` 的关系 | **不冲突**。上游排除的是"在 threading 任务里顺手改泛型降级"；本设计把该机制**独立立项**，显式覆盖 runtime+codegen+ATG |
| 上游排除项「`Create<T>` 泛型降级 → 非 threading 域」 | 尊重——故本任务**不挂在** threading-receiver-plan 下，而是独立 task_id |

## 三、authority 决策

| 层 | 归属 | 是否改动 |
|---|---|---|
| ATG | 不归本设计 | ❌ |
| CodeGen | 不归本设计（`Create<T>` 是托管降低，自动跟随源码） | ❌ |
| **SDK/fixture** | `Chaos.TestFramework.Sdk/SubjectInstanceFactory.cs` | ✅ 主改点 |
| **运行时 native** | `reflection/invoke.cpp` 的 `ChaosReflectionCreateInstance` 无参分支 | ✅ 补一处 |

## 四、当前结论（三个裁决）

- **Q1 契约** = **(c) 真构造 + 无法构造则显式失败**
- **Q2 修复层** = **改 SDK factory 走真构造**（复用既有 `ChaosReflectionCreateInstance`）
- **Q3 失败方式** = **维持现有 `throw`**（落 `factoryGap`）——
  源码血泪注释证明返 null 会 `ud2`→SIGILL→**整 chunk 归零**，与本轮实测吻合

**三类失败已分解**（勿混为一谈）：

| 类 | 项 | 处置 |
|---|---|---|
| A. RWLS TryEnter* | 6 | ✅ 本设计主目标，真构造后转 `real` |
| B. SpinLock Enter/TryEnter | 4 | ⚠️ **不是** `Create<T>` 问题——是 `25e454f24` 加的对象头守卫在 receiver 注入后**已过时、在误伤合法值类型槽**；需撤 |
| C. MRE Wait | ~3 | ❌ 运行时行为**符合 .NET 语义**；归 ATG（与 `Wait_7__0` 同族） |

## 五、下一步入口

**探针**：在 threading 一个 chunk 上改 factory，量出转绿/转红比（R1 风险）。
判据：**转绿 > 转红且无新增挂起**，才继续全量。不对则回退 Q2-c。

## 六、风险评估摘要

| # | 风险 | 缓解 |
|---|---|---|
| R1 | 大量 subject 行为改变（现在"过"的可能变红） | **先探针一个 chunk** |
| R2 | 性能退化（P1） | 只在测试 fixture 路径，不进生产热路径 |
| R3 | 无参 ctor 不存在的类型 | 落 `factoryGap`，显式失败 |
| R4 | ctor 副作用（Timer 启计时、Mutex 占资源） | 探针阶段统计，必要时该类转 ATG 排除 |
| R5 | C 类仍阻塞 | `47dab8dff` 已修超时保留；C 类归 ATG |
| R6 | 主树并发度高（9 个未提交会话 + 2 pipeline） | worktree 隔离；跑前核对 `git status` |

## 七、三优先级权衡结论

- **P1 性能最优**：⚪ 中性——改动只在测试 fixture，不进生产 codegen 热路径
- **P2 架构完美**：✅ 强——契约自洽，复用既有实现，不新增抽象（否决 F2/F3）
- **P3 热更适配**：⚪ 中性——不触及 hotpatch/interpreter

**无优先级冲突**。若探针发现 R1 大面积转红，则回退 Q2-c（按类型补丁），
即 P2 让位于 P1 的稳定性诉求。

## 八、问题清零

```
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true   （Q1/Q2/Q3 + 推进方式 均已确认）
```

## 九、验收口径

- 探针：threading chunk A 类 6 项 `failed` → `real`，且无新增转红/挂起
- 全量：system / threading-tasks / reflection 对比不劣化
- 契约测试：`Create<T>()` 对有 ctor 类型返回已构造实例；对无 ctor 类型抛异常
