# review registry

> 本页由 `.ai/skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要处理代码审查结果、修复 review findings 时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-review-fix-expert`](../../library/skills/dev-review-fix-expert/SKILL.md) | 收到代码审查反馈时；收到审阅结果/报告时；发现多个跨域 review finding 时；review finding 涉及内存安全/GC正确性/契约一致性/根因已修判断时 | 代码审查结果修复入口——按复杂度分级（triager）处理 review finding：simple 快速派发，complex 走 dev-brainstorm 深挖根因+多方案权衡后修复。 | 2.1.0 | `00-skills-maintainers` | large | active |
