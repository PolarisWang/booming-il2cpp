# review registry

> 本页由 `.ai/skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要处理代码审查结果、修复 review findings 时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-review-fix-expert`](../../library/skills/dev-review-fix-expert/SKILL.md) | 收到代码审查反馈时；收到审阅结果/报告时；发现多个跨域 review finding 时 | 代码审查结果修复专属入口——收到 review 立即分类并全量 Workflow 派发 expert，禁止先读代码确认。 | 1.0.0 | `00-skills-maintainers` | large | active |
