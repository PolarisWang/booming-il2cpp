# git registry

> 本页由 `.ai/skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

需要创建/切换 worktree、提交本地改动或收口开发分支时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-finishing-a-development-branch`](../../library/skills/dev-finishing-a-development-branch/SKILL.md) | 执行主线已归档为 completed 时；需要合并结果回目标分支时 | 执行主线完成后合并回目标分支并完成本地提交收口。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-using-git-worktrees`](../../library/skills/dev-using-git-worktrees/SKILL.md) | 需要创建隔离开发环境时；需要管理多个并行分支时；需要规范化提交流程时 | 隔离开发的 worktree 管理和本地提交与备份分支工作流。 | 1.0.0 | `00-skills-maintainers` | small | active |
