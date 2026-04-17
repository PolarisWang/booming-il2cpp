---
name: requesting-code-review
description: 在完成任务、实现主要功能或合并之前使用，以验证工作是否满足需求
---

# 请求代码审查

派发 dev-reviewer 子 Agent 以在问题级联之前发现它们。审查者获得精心构建的评估上下文——绝不是你的会话历史。这使审查者专注于工作产品，而非你的思考过程，并为你自己的继续工作保留上下文。

**核心原则：** 早审查，多审查。

## 何时请求审查

**必须：**
- 子 Agent 驱动开发中每个任务完成后
- 完成主要功能后
- 合并到 main 之前

**可选但有价值：**
- 卡住时（新鲜视角）
- 重构之前（基准检查）
- 修复复杂 bug 之后

## 如何请求

**1. 获取 git SHA：**
```bash
BASE_SHA=$(git rev-parse HEAD~1)  # 或 origin/main
HEAD_SHA=$(git rev-parse HEAD)
```

**2. 派发 dev-reviewer 子 Agent：**

使用 Task 工具和 dev-reviewer 类型，填写 `code-reviewer.md` 中的模板

**占位符：**
- `{WHAT_WAS_IMPLEMENTED}` - 你刚刚构建了什么
- `{PLAN_OR_REQUIREMENTS}` - 它应该做什么
- `{BASE_SHA}` - 起始提交
- `{HEAD_SHA}` - 结束提交
- `{DESCRIPTION}` - 简短摘要

**3. 根据反馈行动：**
- 立即修复关键问题
- 继续前修复重要问题
- 记录次要问题以备后续处理
- 如果审查者有误则反驳（附理由）

## 示例

```
[刚刚完成任务 2：添加验证函数]

你：让我在继续之前请求代码审查。

BASE_SHA=$(git log --oneline | grep "Task 1" | head -1 | awk '{print $1}')
HEAD_SHA=$(git rev-parse HEAD)

[派发 dev-reviewer 子 Agent]
  WHAT_WAS_IMPLEMENTED: 对话索引的验证和修复函数
  PLAN_OR_REQUIREMENTS: docs/dev/in-progress/<task_id>/plan-v1-01.md 中的任务 2
  BASE_SHA: a7981ec
  HEAD_SHA: 3df7661
  DESCRIPTION: 添加了带有 4 种问题类型的 verifyIndex() 和 repairIndex()

[子 Agent 返回]：
  优点：架构清晰，真实测试
  问题：
    重要：缺少进度指示器
    次要：报告间隔的魔术数字（100）
  评估：可以继续

你：[修复进度指示器]
[继续到任务 3]
```

## 与工作流的集成

**子 Agent 驱动开发：**
- 每个任务后审查
- 在问题复合前发现它们
- 在移到下一个任务前修复

**执行计划：**
- 每批次后审查（3 个任务）
- 获取反馈，应用，继续

**即席开发：**
- 合并前审查
- 卡住时审查

## 红旗

**绝不：**
- 因为"很简单"就跳过审查
- 忽视关键问题
- 带着未修复的重要问题继续
- 与有效的技术反馈争辩

**如果审查者有误：**
- 以技术理由反驳
- 展示证明有效的代码/测试
- 请求澄清

查看模板：requesting-code-review/code-reviewer.md
