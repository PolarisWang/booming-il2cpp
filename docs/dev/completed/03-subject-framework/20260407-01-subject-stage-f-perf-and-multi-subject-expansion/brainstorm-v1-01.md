# Subject Stage F Perf And Multi-Subject Expansion Brainstorm v1.01

Date: 2026-04-07
Status: approved-for-plan

## 1. 背景

父 roadmap `20260406-03-subject-test-framework-rollout-roadmap` 已经明确：

- `Stage A + Stage B + Stage C` 已完成并归档
- `Stage D + Stage E` 已完成并归档
- 当前唯一剩余 follow-up 是 `Stage F`

用户随后再次明确要求“继续任务”，因此本轮不再停留在父 roadmap，而是直接把 `Stage F` 派生成新的 child plan。

## 2. 已冻结的输入

- `subject` 已成为 registry / public command / TUI 的一等对象
- 现有 subject spine 仍明显偏向 `HelloWorldObject` 的 native proof / Windows correctness 路径
- `Stage F` 的职责已经在父 roadmap 中固定为：
  - `perf.dev`
  - `perf.release`
  - 第二个 subject 的稳定接入方式
  - 测试工程长期基线整理只做结构收敛，不做删除
- 用户已明确：
  - `echo` 需要保留
  - 后续清理不是删减，而是“全部保留、全部转长期基线”

## 3. 备选推进方式

### 方案 A：把第二个 subject 与 perf 目标合并到一份 Stage F child plan

- 先把 `GenericEcho` 接成第二个正式 subject
- 同时引入 subject-aware 的 `perf.dev` / `perf.release`
- 再把 perf baseline / report / 第二个 subject onboarding pattern 固定下来

优点：

- 与父 roadmap 对 `Stage F` 的冻结语义完全一致
- 能一次性收口“第二个 subject 怎么接”与“perf 在新 subject 模型下怎么跑”
- 不会再次把 perf 与 multi-subject 分成两套重复的 follow-up

缺点：

- 覆盖面比纯 perf 方案更宽

### 方案 B：先只做 `HelloWorldObject` 的 perf，再把第二个 subject 后置

- 第一阶段只给现有 `HelloWorldObject` 增加 perf 目标
- 第二个 subject 以后单开 follow-up

优点：

- 一开始改动面更窄

缺点：

- 与父 roadmap 的 `Stage F` 语义不完全一致
- 不能回答“第二个 subject 的稳定接入模式是什么”
- 很容易让 perf 继续绑定 `HelloWorldObject` 的特例路径

### 方案 C：先只做第二个 subject 的 correctness，再把 perf 后置

- 先把 `GenericEcho` 接成第二个 correctness subject
- `perf.dev` / `perf.release` 继续推迟

优点：

- 第二个 subject onboarding 更直观

缺点：

- 依然绕开了 `Stage F` 必须解决的 perf 维度
- 后续还得再开一次 perf follow-up，重复拆线

## 4. 选择

采用：

- **方案 A**

原因：

- 它最符合父 roadmap 已冻结的 `Stage F` 边界
- 它允许第二个 subject 直接以 benchmark / perf subject 的身份落地，而不是先走一轮再返工
- 它能把 perf baseline、report 与第二个 subject onboarding pattern 一次性压成统一计划

## 5. 本轮明确纳入

- 第二个正式 subject：`GenericEcho`
- subject canonical source 从 `subjects/GenericEcho/source/` 开始收敛
- `perf.dev`
- `perf.release`
- subject-aware perf baseline / report 契约
- `run test subject --id subject/GenericEcho --goal perf.dev|perf.release`
- 第二个 subject onboarding pattern
- 测试工程长期基线整理规则的文档化收口

## 6. 本轮明确排除

- 多 subject 批量调度
- macOS / Linux 的首批 perf host 扩展
- 把全部旧 smoke/gate/workflow 对象迁移成 subject
- 删除现有测试工程
- 把 `HelloWorldObject` 的 native correctness worker 全面泛化成任意 subject 通用 native proof 框架

## 7. 输出结论

本轮已经满足直接进入 `plan` 的条件：

- 阶段边界固定
- 方案已拍板
- 当前可以直接写成单份 `Stage F` child plan，而不需要再开一级 roadmap
