# STATUS — 闭包 lambda 被误判为 display-class 构造器

```yaml
task_id: closure-lambda-emitting
task_type: design
phase: design
design: docs/dev/in-progress/closure-lambda-emitting/design-v1-01.md
created: 2026-09-20
updated: 2026-09-20
created_by: user
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
dispatch_model: sequential
```

## 边界拍板

- **做**：把 `MethodEmission.cs:169` 的 `Contains("<>c::")` 收窄为
  「`<>c::` 且非 `b__`」，让 lambda body 正常发射、display-class 构造器仍走 fallback。
- **不做**：ctor 的 newobj 结构化发射修复（方案 C）、发射能力探测（方案 B）、
  GUO 挂死修复（并发会话）。
- **范围**：`src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs`

## authority 决策

- 发射器是唯一 authority：哪类 `<>c::` 走 fallback 由 `MethodEmission` 判定。
- 修复采用 `b__` 正向判定（用户拍板），与工作区既有 WIP 一致。
- 未来新形态（`g__`/`d__`）的加固已登记为后续项，不在本任务。

## 关键实测数据

| 指标 | 值 |
|---|---|
| 被误伤 lambda（CoreLib 4 chunk） | **200**（system 54 / threading 12 / threading-2 12 / threading-tasks 122） |
| 正目标 ctor | 24 |
| lambda `bodyAvailability` | 全 `has-canonical-body` |
| lambda IL 指令数 | 全 ≤ 2，无 >10 |

## 风险评估摘要

| 风险 | 等级 | 缓解 |
|---|---|---|
| 验证被并发 WIP（`1c830ad24` GUO 挂死）阻塞 | 🔴 | 已定：切不含该 WIP 的干净 worktree 验证 |
| `b__` 判据不完备（未来 `g__`/`d__`） | 🟡 | 用户已接受；加固项登记于 §8 |
| 解封后下游仍有独立缺口（`QueueUserWorkItem` 3 参） | 🟡 | 预期不全部转绿，逐项记录 |
| 与并发会话撞车 | 🟡 | 用户拍板直接接手提交 |

## 三优先级权衡结论

P1 性能 ➖ 中性／P2 架构 ⚠️ 仍是启发式（已接受，加固登记）／P3 热更 ➖ 中性。
**无冲突**。方案 A 因成本/风险最低被选。

## 验收判据

**不是"12 个全绿"**，而是：
- (a) ctor 仍走 fallback 且数量正确（6 个 threading / 8 个 threading-tasks）
- (b) lambda 拿到真实 body（不再是从 `ChaosExternalRuntimeFallback` 假体）

## 下一步入口

1. 验证工作区既有修复（`M` 状态 `MethodEmission.cs`）编译通过、单测无回归
2. 提交修复（含 `IsLambdaBodySubjectId` helper）
3. 切干净 worktree 验证 threading chunk
4. 回归 threading-tasks（122 lambda）

## 关键文档

- `design-v1-01.md` — 根因、定量、方案取舍（§4-§8）
- `il-data-table-not-registered/closure-body-rootcause.md` — 并发会话独立根因分析