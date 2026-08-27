# quality registry

> 本页由 `.ai/skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要审查代码、验证结果或在完成前补证据时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-fixing-code-review`](../../library/skills/dev-fixing-code-review/SKILL.md) | 收到一份 code review 报告，需要逐条验证其有效性并落地修复；review 结果跨多域(如 GC+并发/JIT+CodeGen)，需要按域分派专家修复；对 review 发现做 meta-review：判断真伪、标定严重度、发现遗漏；修复 review 结果时需按领域路由到 domain expert 并跑回归 | 通用 code-review 结果修复 orchestrator — 对任意域的 review 报告逐条 meta-review(验证有效性/定严重度/找遗漏)，按 file:line 自动路由到底层 domain expert 落地修复并回归。 | 1.0.0 | `00-skills-maintainers` | medium | active |
| [`dev-receiving-code-review`](../../library/skills/dev-receiving-code-review/SKILL.md) | 收到代码审查反馈时；审查意见不清晰或有技术疑问时 | 在收到代码审查反馈时技术严谨地评估建议，而非表演性同意。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-requesting-code-review`](../../library/skills/dev-requesting-code-review/SKILL.md) | 完成任务时；实现主要功能后；合并之前需要验证时 | 审查新增/修改代码的完整性、正确性，确保满足需求与编码规范。 | 1.0.0 | `00-skills-maintainers` | medium | active |
| [`dev-trace-enforcement`](../../library/skills/dev-trace-enforcement/SKILL.md) | 需要检查 trace 覆盖时；代码审查中需要 trace 强制检查时 | 扫描新增/修改代码中缺少 trace 点的场景，强制 trace 覆盖。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-verification-before-completion`](../../library/skills/dev-verification-before-completion/SKILL.md) | 即将声称工作完成时；已修复或已通过时；提交或创建 PR 之前 | 在完成前运行验证命令，确认输出后再做任何成功声明。 | 1.0.0 | `00-skills-maintainers` | small | active |
