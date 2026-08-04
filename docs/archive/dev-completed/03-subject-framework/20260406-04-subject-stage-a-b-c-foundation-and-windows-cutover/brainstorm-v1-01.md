# Subject Stage A+B+C Brainstorm v1.01

Date: 2026-04-06
Status: approved-for-plan

## 1. 本任务目标

把并行 roadmap 中的前三个阶段收成首份实现计划：

- Stage A：`subject` 真源与 canonical inputs
- Stage B：planner / executor / worker 最小脊柱
- Stage C：`HelloWorldObject` Windows 首批 matrix cutover

## 2. 已确认的边界

本任务默认视为已确认以下决策：

1. 首份计划明确排除：
   - `perf.dev`
   - `perf.release`
2. `subjects/HelloWorldObject/` 第一阶段物理落地。
3. public CLI / registry / TUI 后置。
4. Windows `verify-roadmap-0` 在重构期间继续作为 mandatory compatibility gate。
5. 后续阶段不会丢失，而是在 roadmap 中后置并继续补后续计划。

## 3. 为什么只先做 Stage A+B+C

因为当前最大的风险不在：

- 聚合视图
- 菜单入口
- perf 报告

而在：

- `subject` 真源还没物理落地
- planner / executor / worker 还不存在
- Stage 4 与 `verify-roadmap-0` 仍然有隐式 orchestration

## 4. 后续阶段如何处理

后续阶段不进入首份计划，但也不会留成空白：

- `Stage D + Stage E`
  - 作为下一份合并计划后置
- `Stage F`
  - 继续单独后置

## 5. 本任务输出

本任务只输出：

- 一份可执行 `plan-v1-01.md`

不承诺：

- 立即进入实现
- 立即覆盖 `Stage D + E + F`
