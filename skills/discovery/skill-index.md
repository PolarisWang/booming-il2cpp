# Skill Index

> 本页由 `skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 加载说明

1. 先按「领域索引」选择一个最相关的 registry 页面。
2. 只读取命中的 `SKILL.md`；确有需要时再读取 `references/` 或 `scripts/`。
3. 启动阶段不加载全量 catalog。

## 领域索引

| 领域 | 何时读取 | registry | skill 数量 |
| ---- | -------- | -------- | --------- |
| `workflow` | 需要开始、拆解、推进或调试实现任务时先读取本域 registry。 | [workflow](./registries/workflow.md) | 9 |
| `quality` | 需要审查代码、验证结果或在完成前补证据时读取本域 registry。 | [quality](./registries/quality.md) | 4 |
| `git` | 需要创建/切换 worktree、提交本地改动或收口开发分支时读取本域 registry。 | [git](./registries/git.md) | 2 |
| `knowledge` | 任务产出需要写入 wiki 或维护知识索引时读取本域 registry。 | [knowledge](./registries/knowledge.md) | 1 |
| `testing` | 任务涉及测试框架、subject 定义、测试生成或测试治理时读取本域 registry。 | [testing](./registries/testing.md) | 2 |
| `il2cpp` | 需要开发 il2cpp 翻译逻辑或查询架构路径时读取本域 registry。 | [il2cpp](./registries/il2cpp.md) | 13 |
| `skilling` | 需要判断该用哪些 skill，或要新建/改造 skill 体系时读取本域 registry。 | [skilling](./registries/skilling.md) | 4 |

## 维护规则

- 新增 skill 时，必须先创建 `skill.manifest.json` 并选择唯一 domain。
- formal `library/skills/` 只保留正式内容；draft 应停留在 `lifecycle/incubator/`。
- registry 由生成脚本刷新，不手工维护长表。
- 所有修改需在 `skills/library/skills/` 中完成，`.claude/skills/` 和 `.codex/skills/` 由工具自动同步。
