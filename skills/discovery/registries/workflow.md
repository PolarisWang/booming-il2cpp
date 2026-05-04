# workflow registry

> 本页由 `skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要开始、拆解、推进或调试实现任务时先读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-brainstorm`](../../library/skills/dev-brainstorm/SKILL.md) | 创建功能前；修改现有行为前；需求和约束尚未厘清时 | 在编码前把想法整理成设计规格，并决定进入 roadmap 还是直接计划。 | 1.0.0 | `00-skills-maintainers` | large | active |
| [`dev-dispatching-parallel-agents`](../../library/skills/dev-dispatching-parallel-agents/SKILL.md) | 存在两个以上互不依赖的子任务；需要并行探索或实现时；本地工作不依赖某个旁路结果时 | 把可并行的独立子任务分派给多个 agent，减少串行等待。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-executing-plans`](../../library/skills/dev-executing-plans/SKILL.md) | 已有实现计划时；需要分批推进任务时；需要维护 STATUS 或 progress 记录时 | 按批次执行已有计划，并维护状态文档和进度记录。 | 1.0.0 | `00-skills-maintainers` | medium | active |
| [`dev-roadmap`](../../library/skills/dev-roadmap/SKILL.md) | 任务复杂时；需要多阶段推进时；需要从 brainstorming 进入 roadmap 时 | 为复杂任务编写分阶段路线图，定义边界、阶段和派生任务。 | 1.0.0 | `00-skills-maintainers` | medium | active |
| [`dev-subagent-driven-development`](../../library/skills/dev-subagent-driven-development/SKILL.md) | 已有计划且适合委派时；存在可独立实现的任务块时；需要控制器协调多个实现 agent 时 | 按计划协调子 agent 执行实现任务，并在阶段间做双重审查。 | 1.0.0 | `00-skills-maintainers` | medium | active |
| [`dev-systematic-debugging`](../../library/skills/dev-systematic-debugging/SKILL.md) | 出现 bug 或异常行为时；测试失败但根因未知时；需要系统性定位问题时 | 通过证据链和根因分析定位 bug，避免只修症状。 | 1.0.0 | `00-skills-maintainers` | large | active |
| [`dev-test-driven-development`](../../library/skills/dev-test-driven-development/SKILL.md) | 开发功能或修复 bug 时；适合先写测试时；需要严格避免先写实现时 | 按红绿重构流程先写测试再写实现。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-writing-plans`](../../library/skills/dev-writing-plans/SKILL.md) | 面对规格说明或多步骤任务时；尚未开始编码时；需要先形成执行计划时 | 把多步骤任务写成可执行计划，并定义执行期状态与验收方式。 | 1.0.0 | `00-skills-maintainers` | medium | active |
