---
name: subagent-driven-development
description: 当在当前会话中执行具有独立任务的实现计划时使用
---

# 子 Agent 驱动开发

通过为每个任务派发新鲜子 Agent 来执行计划，每个任务完成后进行两阶段审查：先进行规格合规性审查，再进行代码质量审查。

## 核心原则

- 控制器在开始时读取计划一次
- 控制器直接向实现者提供完整任务文本
- 实现者必须做自审并检查完整性
- 审查顺序固定为：规格合规性 → 代码质量
- 每个任务后都要更新 `STATUS.md`、`docs/dev/ACTIVE.md` 与索引

## 文档语言要求

除非用户明确要求其他语言，控制器写入的 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md`、任务摘要，以及通过 `dev:project-wiki-maintenance` 触发的知识文档都必须使用中文。代码、命令、路径、标识符保持原文。

## 流程

### 第一步：加载计划

1. 读取当前任务目录中的计划文件一次
2. 确认任务总数
3. 提取所有任务的完整文本和上下文
4. 创建或更新：
   - 当前任务目录中的 `STATUS.md`
   - `docs/dev/ACTIVE.md`
   - `docs/dev/INDEX.md` 与对应生命周期索引
5. 创建 TodoWrite

### 第二步：逐任务执行

对每个任务：

1. 派发实现者子 Agent，直接提供完整任务文本
2. 如果实现者需要更多上下文，补充后重新派发
3. 实现者完成后，先进行规格合规性审查
4. 规格合规性通过后，再进行代码质量审查
5. 如任一审查发现问题：
   - 由同一个实现者修复
   - 重新审查
   - 直到通过
6. 任务完成后更新：
   - `STATUS.md`
   - `docs/dev/ACTIVE.md`
   - `notes/progress-*.md`
   - 索引文件
   - 必要时的 wiki 页面

### 第三步：完成

所有任务完成、验证通过、必要 wiki 更新完成后：

- 更新 `STATUS.md` 为 `completed`
- 将任务目录移动到 `docs/dev/completed/`
- 删除 `docs/dev/ACTIVE.md`
- 更新索引文件
- 使用 `dev:finishing-a-development-branch`

## 审查规则

- 规格合规性审查者必须保持怀疑态度，不信任实现者的自报
- 审查者必须独立阅读代码
- 如果审查者发现问题，流程必须进入修复循环

## 红旗

**绝不：**

- 跳过规格合规性审查
- 在规格合规性通过前开始代码质量审查
- 让子 Agent 自己去读计划文件
- 在 main/master 上直接开始实现
- 让 `STATUS.md`、`ACTIVE.md` 或索引落后于当前进度

## 集成

- `dev:using-git-worktrees`
- `dev:writing-plans`
- `dev:project-wiki-maintenance`
- `dev:requesting-code-review`
- `dev:finishing-a-development-branch`
- 子 Agent 应使用 `dev:test-driven-development`
