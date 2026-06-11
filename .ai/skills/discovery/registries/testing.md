# testing registry

> 本页由 `skills/tooling/catalog/generate_skill_catalog.py` 自动生成。修改 `skill.manifest.json` 或 `discovery/domain-catalog.json` 后重新生成。

## 何时读取本域

任务涉及测试框架、subject 定义、测试生成或测试治理时读取本域 registry。

## 技能清单

| skill.name | 触发信号 | 摘要 | version | owner | budget | status |
| ----------- | -------- | ---- | ------- | ----- | ------ | ------ |
| [`dev-pressure-test-management`](../../library/skills/dev-pressure-test-management/SKILL.md) | 修改了 GC、内存分配、线程调度相关代码后需要验证稳定性时；需要查看压力测试历史记录或回归检测结果时；AI 开发过程中需要快速验证改动没有引入性能退化时 | 压力测试管理：通过 run stress CLI 运行 GC/容量/线程压力测试，记录结果，检测回归。 | 1.0.0 | `00-skills-maintainers` | small | active |
| [`dev-project-test-governance`](../../library/skills/dev-project-test-governance/SKILL.md) | 修改 subject/test 工作流时；修改 Chaos.TestFramework 时；修改 managed/native/hotupdate runners 时；修改 benchmark 或 unit verification 流程时；修改项目 codegen 结构时 | 测试治理：subject/test 工作流、Chaos.TestFramework、runner、benchmark、codegen 结构变更。 | 1.0.0 | `00-skills-maintainers` | large | active |
