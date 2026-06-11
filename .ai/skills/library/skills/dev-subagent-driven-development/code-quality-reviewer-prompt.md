# 代码质量审查者提示模板

派发代码质量审查者子 Agent 时使用此模板。

**用途：** 验证实现构建良好（整洁、经过测试、可维护）

**仅在规格合规性审查通过后派发。**

```
Task tool (dev-reviewer):
  Use template at requesting-code-review/code-reviewer.md

  WHAT_WAS_IMPLEMENTED: [来自实现者的报告]
  PLAN_OR_REQUIREMENTS: 任务 N，来自 [plan-file]
  BASE_SHA: [任务前的提交]
  HEAD_SHA: [当前提交]
  DESCRIPTION: [任务摘要]
```

**除标准代码质量问题外，审查者还应检查：**
- 每个文件是否有一个具有定义良好接口的清晰职责？
- 单元是否分解为可以独立理解和测试的形式？
- 实现是否遵循了计划中的文件结构？
- 此实现是否创建了已经很大的新文件，或显著增大了现有文件？（不要标记已有的文件大小问题——专注于此次变更带来的影响。）

**代码审查者返回：** 优点、问题（关键/重要/次要）、评估
