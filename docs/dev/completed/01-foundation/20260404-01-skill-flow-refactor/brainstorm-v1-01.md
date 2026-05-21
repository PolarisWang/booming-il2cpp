# .codex Skill 流程重构头脑风暴 v1

## 目标

将 `.codex` 下的 skill 主流程从旧的 `docs/booming + docs/executions/CURRENT.md + history archive` 模式，调整为以 `docs/dev` 任务目录为真源、以 `ACTIVE.md` 为轻量活动指针的新模型。

## 已确认决策

1. 复杂任务仍然先走 `brainstorm`。
2. `brainstorm` 后必须做分流判断：
   - 满足以下任一条件进入 `roadmap`：
     - 需要分 2 个及以上阶段推进
     - 会拆成 2 个及以上相对独立的子任务
     - 当前还不能直接写出稳定执行计划，必须先定优先级或阶段边界
     - 任务会跨多次会话持续推进
   - 其余进入 `plan`
3. 分流结果是 `plan` 时，agent 可直接继续；分流结果是 `roadmap` 时，agent 先说明理由并等待用户确认。
4. 只要触发 `brainstorm`，就属于复杂任务，必须建任务目录；不触发 `brainstorm` 的请求按小请求处理，不强制立项。
5. 新任务目录体系为：
   - `docs/dev/in-progress/`
   - `docs/dev/hanging/`
   - `docs/dev/completed/`
   - `docs/dev/abandoned/`
6. 任务目录名直接使用 `task_id`，格式为 `日期 + 序号 + slug`。
7. 每个复杂任务只有一个主目录；核心文档位于根目录，辅助材料放在 `notes/` 与 `attachments/`。
8. `STATUS.md` 作为任务状态真源，`ACTIVE.md` 作为当前活动任务轻量指针。
9. active guard 选项固定为 `继续 / 挂起 / 放弃`：
   - `继续`：继续当前任务，不处理新请求
   - `挂起`：挂起当前任务，然后直接处理新请求
   - `放弃`：放弃当前任务，然后直接处理新请求
10. 同一时刻只允许 1 个 active 任务。
11. `roadmap` 是父任务，不承担具体长期执行；具体实施通过派生子任务目录完成。
12. `completed` 与 `abandoned` 不能直接恢复，只能派生新任务；`hanging` 是唯一可恢复的暂停态。
13. 这次改造任务本身按用户明确要求以 `plan` 方式执行，作为新流程中的例外案例。

## 关键结论

- `docs/dev/ACTIVE.md` 替代旧 `docs/executions/CURRENT.md`，但只承担入口指针职责，不再保存完整执行真源。
- 执行过程与状态摘要回到任务目录内，避免设计、计划、执行文档分散在不同根目录。
- `roadmap` 作为父任务存在时，父任务在无当前动作时进入 `hanging`，需要继续规划时再恢复。
- `docs/executions/history/` 不再参与现行流程，目录终态改由 `completed/abandoned` 表达。

## 待实施事项

- 建立 `docs/dev` 目录、索引、模板和当前任务文档。
- 新增正式 `roadmap` skill。
- 修改 `using-booming`、`active-execution-guard`、`brainstorm`、`writing-plans`、`executing-plans`、`subagent-driven-development`、`project-wiki-maintenance`。
- 同步迁移 wiki 和 Claude Code tests。

