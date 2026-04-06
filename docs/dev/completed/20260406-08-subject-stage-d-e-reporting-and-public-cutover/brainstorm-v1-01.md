# Subject Stage D+E Reporting And Public Cutover Brainstorm v1.01

Date: 2026-04-06
Status: approved-for-plan

## 1. 背景

父 roadmap `20260406-03-subject-test-framework-rollout-roadmap` 已经明确：

- `Stage A + Stage B + Stage C` 已完成并归档
- 下一步必须是 `Stage D + Stage E` merged follow-up plan
- `Stage F` 继续后置，单独保留

用户随后明确要求“继续”，因此本轮不再回到泛讨论，而是直接把 `Stage D + Stage E` 收成正式 child plan。

## 2. 已冻结的输入

- 现有 `subject` 真源、planner / executor / worker 最小脊柱已经落地
- `HelloWorldObject` 的四个 Windows matrix 已经跑在新的 subject artifact 链上
- reporting 方向已经在 `docs/discuss/20260406-20-reporting-schema-design-v1-01.md` 收敛为 `B2`
- event 方向已经在 `docs/discuss/20260406-21-event-schema-alignment-design-v1-01.md` 收敛为方案 `C`
- `perf`、多 subject 扩展和测试工程长期基线整理都属于 `Stage F`，本轮不提前引入

## 3. 备选推进方式

### 方案 A：继续保持 `Stage D + Stage E` 合并执行

- 先补三层 reporting 与三层 events
- 然后顺势把 subject registry / public command / TUI 入口接上
- legacy `suite/session/watch` 入口只做 compatibility bridge

优点：

- 与父 roadmap 已冻结的阶段边界完全一致
- reporting / events / public entry 共用同一套 subject 结果对象，不会出现重复 schema
- 执行期不会为了 public cutover 再回头改一次 session summary / event projection

缺点：

- 单份 plan 的覆盖面比纯 reporting 方案更宽

### 方案 B：把 `Stage D` 和 `Stage E` 再拆成两份 plan

- 第一份只做 reporting / events
- 第二份再做 registry / CLI / TUI

优点：

- 每份计划更窄

缺点：

- 与父 roadmap 的“合并 plan”决策冲突
- public cutover 很可能再次复制一遍 reporting/session bridge 逻辑

### 方案 C：public-first，再回补 reporting

- 先把 subject 暴露到 public command / registry
- reporting / events 只做最低限度适配

优点：

- 用户较早看到 public 入口

缺点：

- 会把 public surface 建在尚未稳定的 reporting/session 语义之上
- watch / summary / TUI 会缺乏稳定的 subject summary 入口

## 4. 选择

采用：

- **方案 A**

原因：

- 它与父 roadmap 的阶段冻结一致
- 它复用已经收敛的 reporting / event 设计，不需要再开一次大讨论
- 它允许 public cutover 直接消费稳定的 `matrix report -> subject summary -> session summary` 导航链

## 5. 本轮明确纳入

- `HelloWorldObject`
- 四个既有 Windows matrix
- `matrix report / subject summary / session summary`
- `matrix events / subject events / session events`
- subject registry 对象
- `run test subject --id ...` 一类 public entry
- `test watch` / `test summary` / TUI 的兼容投影

## 6. 本轮明确排除

- `perf.dev / perf.release`
- 第二个 subject
- macOS subject cutover
- 删除或重命名现有测试工程
- 彻底移除 legacy suite / registry / session 入口

## 7. 输出结论

本轮已经满足继续进入 `plan` 的条件：

- 阶段边界固定
- 方案已拍板
- 当前只需要把设计压成可执行 chunk，而不需要继续做 roadmap 拆分
