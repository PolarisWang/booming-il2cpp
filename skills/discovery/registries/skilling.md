# skilling registry

> 本页由 `skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要判断该用哪些 skill，或要新建/改造 skill 体系时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | owner | budget | status |
| ----------- | -------- | ---- | ----- | ------ | ------ |
| [`dev-using-booming`](../../library/skills/dev-using-booming/SKILL.md) | 开始任何对话时；需要判断激活哪些技能时 | 在开始对话时判断应该激活哪些技能，路由器/控制器技能。 | `00-skills-maintainers` | small | active |
| [`dev-writing-skills`](../../library/skills/dev-writing-skills/SKILL.md) | 需要创建新技能时；需要编辑现有技能时；需要在部署前验证技能时 | 创建、编辑和验证技能，包括技能评估和优化。 | `00-skills-maintainers` | medium | active |
