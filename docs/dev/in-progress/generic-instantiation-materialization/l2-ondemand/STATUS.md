# STATUS — L2 按需加载跨程序集泛型定义

```yaml
task_id: l2-ondemand-generic-definition-loading
task_type: design
phase: design
parent: generic-instantiation-materialization
design: docs/dev/in-progress/generic-instantiation-materialization/design-l2-ondemand-v1-01.md
created: 2026-09-18
updated: 2026-09-18
created_by: user
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
dispatch_model: sequential
```

## 边界拍板

- **做**：Loader 侧"按需解析被点名的泛型定义方法" + Linker 侧"以 demand 为闭包种子"。
  **合并为一个任务，必须端到端见效。**
- **不做**：ABI 层（L3）改动；跨 chunk 缓存；为每种 T 写 stub。
- **范围**：`src/managed/Chaos.IL2CPP.Loader/` + `src/managed/Chaos.IL2CPP.Linker/`。

## authority 决策

- 物化的**唯一权威源**仍是 `ProjectInstantiationMethod`（逻辑不变）。
- 新增的只是它的**输入构造方式**：由 eager 全量 → lazy 按需。
- 闭包可达性权威仍在 `LinkerStage.Reachability`，本任务给它**增加一个种子来源**。

## 当前结论

**方案 B1 可行**（spike 已验证机制层面）：
`DecodeMethodBody` 全程只以「缓存 + 回退」方式使用三个 owner 字典，
一切可从 PE metadata 按需重建；eager 的外层循环是唯一阻碍。

**关键量化**：threading-tasks 140 条 demand → **仅 89 个 distinct
`definitionSubjectId`**。需要的是 89 个方法，不是 2 个程序集。

## 风险评估摘要

| 风险 | 等级 | 缓解 |
|---|---|---|
| 回退路径 `TryResolveOwningTypeHandle` 是 O(类型数×字段数) 全表扫描 | 🟡 中 | 建轻索引；属实现后调优，不改可行性 |
| SubjectId 反查精度（`` `N `` / 嵌套类型） | 🟡 中 | 复用 L1 修复 `c419e8385` 已统一的 arity 逻辑 |
| 两阶段重排引入回归 | 🟡 中 | `ProjectInstantiationMethod` 保持原样，只重排调用点 |
| 闭包种子导致包体膨胀 | 🟡 中 | 只种子 89 个 definitionSubjectId，不做传递扩张；对比生成物体积 |
| **L3 会随后暴露** | 🟡 中 | B1+闭包成功后，原本进不了 ABI 的实例会进来，触发 `System.Char`/`!!0` 静默降级 → **L3 需紧随验证** |
| 并发会话冲突 | 🟡 中 | 开工前核对（本轮已多次踩） |

## 三优先级权衡结论

P1 性能 ✅ 无冲突（Loader 工作量下降，生成代码运行时性能不变）／
P2 架构 ✅ 无冲突（eager→lazy 正确演进）／P3 热更 ➖ 中性。
**无需裁决。**

## 下一步入口

用户批准 design 后 → `writing-plans` 拆实施步骤：

1. (a) 类型→方法句柄轻索引
2. (c) `CollectProjectedCrossAssemblyMethodInstantiations` 拆「收集 / 投影」两阶段
3. (b) 稀疏 `definitionMethodsBySubject` 构造（复用 `DescribeMethodDefinition` + `DecodeMethodBody`）
4. 闭包种子（`ExpandReachableMethods` 消费 demand 图）
5. **端到端验证**（CoreIR 有实例 **且** 生成代码含实例）

## Preflight Self-Review

- `preflight_review: pass`
- 依据：design 已批准、`blocking_questions: []`、无 roadmap-child 依赖、
  无未清 authority 问题。可安全自动继续。

## 计划

- `plan-v1-01.md` — 6 个 Task（轻索引 → 稀疏解析 → 两阶段重排 → 闭包种子 →
  端到端验证 → 收尾），含回退策略与执行纪律。
- `arch_review_mode: critical`（触及 loader + codegen 主线）

## 关键文档

- `design-l2-ondemand-v1-01.md` — 本任务设计（含 spike 结论）
- `STATUS.md`（父任务）— L1/L2/L3 全景与 §8 实验记录
- `L2-ANALYSIS.md`（父任务）— L2 定性分析与四方案取舍
