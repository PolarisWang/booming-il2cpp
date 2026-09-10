# Subject Stage A+B+C 设计 v1.01

Date: 2026-04-06
Status: design-approved

## 1. 设计目标

为 `subject` 测试框架的首批实现提供一个严格限界的设计：

- 先完成真源迁移
- 再完成内部执行脊柱
- 再让 `HelloWorldObject` 的 Windows 首批 matrix 跑在新脊柱上

## 2. 设计边界

### 包含

- `subjects/HelloWorldObject/`
- contract source cutover
- artifact bucket 正式落点
- `subject.manifest.json` 增量 schema
- planner / executor / worker skeleton
- Windows 首批 matrix cutover
- Stage 4 / `verify-roadmap-0` 隐式 orchestration 拆解

### 排除

- subject / session 聚合 reporting 的完整落地
- matrix / subject / session events 的完整对齐
- public CLI / registry / TUI
- `perf.dev / perf.release`

## 3. 实施原则

1. `subjects/` 成为唯一正式真源。
2. worker 只围绕单 stage / 单 bucket。
3. Stage 4 和 `verify-roadmap-0.py` 的暗箱编排必须显式拆开。
4. 兼容层只保留最小 bridge，不形成第二套正式框架。

## 4. 后续计划位置

- `Stage D + Stage E`
  - 后续合并成第二份计划
- `Stage F`
  - 后续单独计划
  - 后续测试工程清理只做结构收敛；现有测试工程全部保留，并转为长期基线

## 5. 设计输出

这份设计直接服务 `plan-v1-01.md`，不再单独扩展更多方案比较。
