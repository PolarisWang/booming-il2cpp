# ACTIVE 活动指针与任务目录机制

## 作用

当前执行系统采用双层模型：

- `docs/dev/<lifecycle>/<task_id>/STATUS.md`：任务状态真源
- `docs/dev/ACTIVE.md`：当前 active 任务的轻量入口指针

这两个文件共同替代旧的 `docs/executions/CURRENT.md`。

## 目录职责

- `docs/dev/in-progress/`：正在推进的任务目录
- `docs/dev/hanging/`：可恢复的挂起任务目录
- `docs/archive/dev-completed/`：已完成任务目录
- `docs/dev/abandoned/`：已放弃任务目录

## ACTIVE.md 的职责

- 保存当前 active 任务是谁
- 指向对应任务目录与 `STATUS.md`
- 提供简短的最近摘要与下一步
- 作为 guard 的唯一活动入口

## STATUS.md 的职责

- 保存任务身份、生命周期和阶段
- 保存关键文档路径
- 保存最近摘要、下一步、风险/阻塞
- 保存父任务、来源任务和子任务关联

## 生命周期

- 开始时放在 `in-progress/`
- 挂起时移动到 `hanging/`
- 完成时移动到 `completed/`
- 放弃时移动到 `abandoned/`

同一时刻只允许一个 active 任务，因此 `ACTIVE.md` 始终只能指向一个任务目录。

## 最近更新

- `2026-04-04`：建立 `ACTIVE.md + STATUS.md + 目录终态` 的执行系统模型。
