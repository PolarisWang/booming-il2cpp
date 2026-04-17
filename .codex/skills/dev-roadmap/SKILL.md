---
name: roadmap
description: 当 brainstorming 判定复杂任务需要多阶段推进或拆分为多个独立子任务时使用
---

# 编写 Roadmap

## 概述

当复杂任务无法直接进入稳定的实现计划时，用 roadmap 把它拆成阶段、边界和可派生子任务。roadmap 是父任务文档，不承担具体长期执行；具体实施通过派生子任务目录完成。

**开始时宣布：** "我正在使用 roadmap 技能来创建阶段路线图。"

**保存位置：** 当前任务目录下的 `roadmap-v1-01.md`
- 默认路径示例：`docs/dev/in-progress/<task_id>/roadmap-v1-01.md`

## 使用时机

brainstorming 结束后，只要满足以下任一条件，就应进入本技能：

- 需要分 2 个及以上阶段推进
- 会拆成 2 个及以上相对独立的子任务
- 当前还不能直接写出稳定执行计划，必须先定优先级或阶段边界
- 任务会跨多次会话持续推进

## 用户确认门

如果分流结论是 `roadmap`，必须先向用户说明原因并等待确认，才能继续本技能。

## 文档语言要求

除非用户明确要求其他语言，roadmap 文档及其相关书面产物必须使用中文。代码、命令、路径、标识符保持原文。

## 文档结构

roadmap 文档固定包含：

1. 目标
2. 范围边界
3. 非目标
4. 阶段列表
5. 每阶段完成定义
6. 子任务映射
7. 依赖
8. 风险
9. 备选路径
10. 当前建议推进顺序

### 阶段列表

- 默认按顺序推进
- 允许在必要时标记“并行准备”或“并行执行”

### 每阶段完成定义

每个阶段固定包含：

- `goal`
- `exit_criteria`
- `deliverables`
- `dependencies`
- `open_questions`

### 子任务映射

子任务表固定字段：

- `task_id`
- `phase`
- `status`
- `owner`
- `purpose`
- `depends_on`

`status` 枚举：

- `planned`
- `ready`
- `in-progress`
- `blocked`
- `hanging`
- `completed`
- `abandoned`

其中：

- `planned` = 子任务已被识别，但尚未满足启动条件
- `ready` = 前置依赖已满足，可以正式创建或恢复为 active 子任务

## 流程

1. 读取当前任务目录下的 brainstorm/design 文档与 `STATUS.md`
2. 明确写出为什么该任务不能直接进入稳定 plan
3. 产出 roadmap 文档结构
4. 更新 `STATUS.md`
   - `task_type` 设为 `roadmap`
   - `phase` 设为 `roadmap`
   - `roadmap_or_plan` 指向新 roadmap 文档
5. 如果用户决定立即推进某个子任务：
   - 为子任务创建新的任务目录
   - 在子任务 `STATUS.md` 中写入 `parent_task_id`
   - 写入 `source_task_id` 与 `source_relation: roadmap-child`
   - 对子任务重新开始 brainstorm/plan 流程
6. 如果当前不继续推进父 roadmap：
   - 将父任务移动到 `docs/dev/hanging/`
   - 删除 `docs/dev/ACTIVE.md`
   - 保留后续恢复入口

## 完成判定

父 roadmap 任务只有在以下条件全部满足时才允许完成：

- roadmap 的阶段、边界、任务映射已经稳定
- 没有待继续拆分或待决策事项
- 所有已创建子任务都进入终态：`completed` 或 `abandoned`

