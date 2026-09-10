# skilling registry

> 本页由 `skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要判断该用哪些 skill，或要新建/改造 skill 体系时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-il2cpp`](../../library/skills/dev-il2cpp/SKILL.md) | 开始任何对话时；需要判断激活哪些技能时 | 项目唯一入口 skill — 对话启动时加载，通过 skill-index.md 发现领域并路由到对应技能 | 2.0.0 | `00-skills-maintainers` | small | active |
| [`dev-skill-evolution`](../../library/skills/dev-skill-evolution/SKILL.md) | 需要查看技能健康状态时；需要审查或晋升进化提案时；需要运行技能自进化管线时 | 技能自进化管线操作入口，包括健康检查、进化提案审查与晋升、自动退役。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-writing-skills`](../../library/skills/dev-writing-skills/SKILL.md) | 需要创建新技能时；需要编辑现有技能时；需要在部署前验证技能时 | 创建、编辑和验证技能，包括技能评估和优化。 | 1.0.0 | `00-skills-maintainers` | medium | active |
