# roadmap

> 项目中的路线图规划主线，负责把多阶段复杂事项拆成阶段、边界和可派生子任务。

## 基本信息

- 分层：主线工作流
- 项目职责：在 brainstorm 之后承接复杂任务分流，生成 roadmap 父任务文档并派生子任务
- 实现路径：`.codex/skills/dev-roadmap/SKILL.md`
- 状态：`active`

## 触发时机

- brainstorm 之后发现任务需要多阶段推进
- 需要拆成 2 个及以上相对独立的子任务
- 当前还不能稳定写出实现计划
- 任务会跨多次会话持续推进

## 在本项目中的作用

- 生成当前任务目录内的 `roadmap-v1-01.md`
- 定义阶段、完成定义、依赖、风险和建议推进顺序
- 维护 roadmap 父任务与子任务的关系
- 为后续 child task 创建新的任务目录

## 项目规则

- 若分流结果是 roadmap，必须先向用户说明原因并等待确认
- roadmap 是父任务，不承担具体长期执行
- 具体实施必须通过独立子任务目录完成
- roadmap child task 是独立生命周期任务；进入 `completed` / `hanging` / `abandoned` 后必须物理移出 `docs/dev/in-progress/`
- 父 roadmap 只能同步 child mapping、摘要和下一步，不能用父任务文案替代 child task 的归档动作

## 最近变更

- `2026-04-04`：新增 roadmap 主线，承接 brainstorm 后的多阶段规划分流。
- `2026-04-22`：明确 roadmap-child 必须物理归档，父任务不能只在表格里把 child 标成 completed。
