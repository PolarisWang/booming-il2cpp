# Harness Engineer 自动化实现计划

> **For agentic workers:** REQUIRED: Use booming-code:subagent-driven-development (if subagents available) or booming-code:executing-plans to implement this plan. Steps use checkbox (`- [ ]`) syntax for tracking.

**目标：** 将活动计划守卫、执行上下文持久化、执行归档和项目 wiki 沉淀接入现有 `.codex/skills` 工作流。

**架构：** 通过新增两个横切 skill、增强三个现有 workflow skill、补充 Claude Code skill 测试，并建立 `wiki/` 与 `docs/executions/` 的项目目录体系来完成。`.codex` 负责实现，`docs` 负责过程，`wiki` 负责长期知识。

**技术栈：** Markdown skill 文档、Markdown wiki 文档、PowerShell/Bash Claude Code 技能测试

---

**设计文档：** `docs/booming/2026-04-02-harness-engineer-automation/design-2026-04-02-01-harness-engineer-automation.md`

## Chunk 1: 测试先行

### 任务 1：为新增与增强的 skill 补充行为测试

**文件：**
- 创建：`.codex/tests/claude-code/test-active-execution-guard.ps1`
- 创建：`.codex/tests/claude-code/test-active-execution-guard.sh`
- 创建：`.codex/tests/claude-code/test-project-wiki-maintenance.ps1`
- 创建：`.codex/tests/claude-code/test-project-wiki-maintenance.sh`
- 创建：`.codex/tests/claude-code/test-executing-plans.ps1`
- 创建：`.codex/tests/claude-code/test-executing-plans.sh`
- 修改：`.codex/tests/claude-code/run-skill-tests.ps1`
- 修改：`.codex/tests/claude-code/run-skill-tests.sh`
- 修改：`.codex/tests/claude-code/README.md`

- [ ] **步骤 1：编写 `active-execution-guard` 的失败测试**

断言至少覆盖：
- 发现 `docs/executions/CURRENT.md` 时必须先处理活动计划
- 用户只允许 `继续` 或 `放弃`
- 未处理前不进入其他回答

- [ ] **步骤 2：编写 `project-wiki-maintenance` 的失败测试**

断言至少覆盖：
- `docs` 和 `wiki` 严格分离
- 只要产生长期有效知识就必须更新 `wiki`
- 更新 wiki 时必须同步维护 `INDEX.md`

- [ ] **步骤 3：编写 `executing-plans` 的失败测试**

断言至少覆盖：
- 开始执行时确认任务总数
- 创建或更新 `docs/executions/CURRENT.md`
- 每完成一项任务后更新 `CURRENT.md`
- 自动归档 `completed` 的条件

- [ ] **步骤 4：将新测试接入测试运行器**

运行：
```powershell
Get-Content .codex/tests/claude-code/run-skill-tests.ps1
Get-Content .codex/tests/claude-code/run-skill-tests.sh
```

预期：
- 运行器包含新增测试项
- README 中列出新增测试说明

- [ ] **步骤 5：运行新测试并确认当前失败**

运行：
```powershell
powershell -NonInteractive -File .codex/tests/claude-code/test-active-execution-guard.ps1
powershell -NonInteractive -File .codex/tests/claude-code/test-project-wiki-maintenance.ps1
powershell -NonInteractive -File .codex/tests/claude-code/test-executing-plans.ps1
```

预期：
- 至少因为 skill 不存在或规则未写入而失败

## Chunk 2: 新增横切 skill

### 任务 2：实现活动计划守卫 skill

**文件：**
- 创建：`.codex/skills/booming-code-active-execution-guard/SKILL.md`
- 测试：`.codex/tests/claude-code/test-active-execution-guard.ps1`
- 测试：`.codex/tests/claude-code/test-active-execution-guard.sh`

- [ ] **步骤 1：编写最小 skill 草案**

内容至少包含：
- 触发条件：任何问答前、仓库可能存在 `docs/executions/CURRENT.md`
- 检查流程：存在则读取并提示
- 用户选项：仅 `继续 / 放弃`
- 红旗：未处理活动计划前回答其他问题

- [ ] **步骤 2：运行测试验证 `active-execution-guard` 通过**

运行：
```powershell
powershell -NonInteractive -File .codex/tests/claude-code/test-active-execution-guard.ps1
```

预期：
- 全部断言通过

- [ ] **步骤 3：补充红旗与边界说明**

补充内容：
- `CURRENT.md` 存在即视为活动计划
- 即使文档内容不完整也不得绕过
- 选择 `放弃` 后必须归档为 `abandoned`

## Chunk 3: 增强执行主线 skill

### 任务 3：增强 `writing-plans`

**文件：**
- 修改：`.codex/skills/booming-code-writing-plans/SKILL.md`

- [ ] **步骤 1：在计划规范中补充执行与 wiki 绑定要求**

补充点：
- 计划任务计数必须清晰
- 任务结构应注明预期知识沉淀目标
- 计划与设计文档路径应可被执行阶段引用

- [ ] **步骤 2：自检文案是否仍保持“触发条件与流程分离”**

预期：
- YAML 描述不泄漏长流程
- 正文明确可执行，但描述字段仍专注触发条件

### 任务 4：增强 `executing-plans`

**文件：**
- 修改：`.codex/skills/booming-code-executing-plans/SKILL.md`
- 测试：`.codex/tests/claude-code/test-executing-plans.ps1`
- 测试：`.codex/tests/claude-code/test-executing-plans.sh`

- [ ] **步骤 1：写入 `CURRENT.md` 生命周期规则**

补充点：
- 开始前确认任务总数
- 创建或更新 `docs/executions/CURRENT.md`
- 记录设计/计划文档路径、结构化上下文、下一步

- [ ] **步骤 2：写入任务后的沉淀规则**

补充点：
- 每完成一项任务都更新 `CURRENT.md`
- 产生长期知识时调用 `project-wiki-maintenance`
- 若无 wiki 更新，也要在 `CURRENT.md` 记录

- [ ] **步骤 3：写入自动归档规则**

补充点：
- 全部任务完成、验证通过、wiki 更新完成后自动归档为 `completed`
- 用户中途放弃则归档为 `abandoned`

- [ ] **步骤 4：运行 `executing-plans` 测试**

运行：
```powershell
powershell -NonInteractive -File .codex/tests/claude-code/test-executing-plans.ps1
```

预期：
- 断言通过

### 任务 5：增强 `subagent-driven-development`

**文件：**
- 修改：`.codex/skills/booming-code-subagent-driven-development/SKILL.md`
- 修改：`.codex/tests/claude-code/test-subagent-driven-development.ps1`
- 修改：`.codex/tests/claude-code/test-subagent-driven-development.sh`

- [ ] **步骤 1：补充活动执行上下文与 `CURRENT.md` 规则**

补充点：
- 开始时确认任务总数并创建/更新 `CURRENT.md`
- 每任务后更新 `CURRENT.md`
- 完成时自动归档

- [ ] **步骤 2：补充项目 wiki 维护协作规则**

补充点：
- 每任务后判断是否需要沉淀到 `wiki/`
- 明确引用 `project-wiki-maintenance`

- [ ] **步骤 3：运行现有和新增断言**

运行：
```powershell
powershell -NonInteractive -File .codex/tests/claude-code/test-subagent-driven-development.ps1
```

预期：
- 原有断言仍通过
- 新增关于 `CURRENT.md` 与 wiki 更新的断言通过

## Chunk 4: 新增项目 wiki 维护 skill 与项目文档骨架

### 任务 6：实现 `project-wiki-maintenance`

**文件：**
- 创建：`.codex/skills/booming-code-project-wiki-maintenance/SKILL.md`
- 测试：`.codex/tests/claude-code/test-project-wiki-maintenance.ps1`
- 测试：`.codex/tests/claude-code/test-project-wiki-maintenance.sh`

- [ ] **步骤 1：编写最小 skill 内容**

内容至少包含：
- `docs` 与 `wiki` 边界
- `03-功能模块` 与 `05-项目经验` 边界
- `INDEX.md` 的维护要求

- [ ] **步骤 2：运行 `project-wiki-maintenance` 测试**

运行：
```powershell
powershell -NonInteractive -File .codex/tests/claude-code/test-project-wiki-maintenance.ps1
```

预期：
- 全部断言通过

### 任务 7：建立项目 wiki 结构与 skill registry

**文件：**
- 创建：`wiki/INDEX.md`
- 创建：`wiki/01-项目总览/INDEX.md`
- 创建：`wiki/02-Skill体系/INDEX.md`
- 创建：`wiki/02-Skill体系/skill-registry.md`
- 创建：`wiki/02-Skill体系/01-主线工作流/INDEX.md`
- 创建：`wiki/02-Skill体系/02-执行守卫/INDEX.md`
- 创建：`wiki/02-Skill体系/03-项目知识维护/INDEX.md`
- 创建：`wiki/02-Skill体系/04-质量保障/INDEX.md`
- 创建：`wiki/02-Skill体系/01-主线工作流/executing-plans.md`
- 创建：`wiki/02-Skill体系/01-主线工作流/subagent-driven-development.md`
- 创建：`wiki/02-Skill体系/01-主线工作流/writing-plans.md`
- 创建：`wiki/02-Skill体系/02-执行守卫/active-execution-guard.md`
- 创建：`wiki/02-Skill体系/03-项目知识维护/project-wiki-maintenance.md`
- 创建：`wiki/02-Skill体系/04-质量保障/test-driven-development.md`
- 创建：`wiki/02-Skill体系/04-质量保障/systematic-debugging.md`
- 创建：`wiki/02-Skill体系/04-质量保障/verification-before-completion.md`
- 创建：`wiki/03-功能模块/INDEX.md`
- 创建：`wiki/03-功能模块/01-执行系统/INDEX.md`
- 创建：`wiki/03-功能模块/02-Skill工作流/INDEX.md`
- 创建：`wiki/03-功能模块/03-文档体系/INDEX.md`
- 创建：`wiki/03-功能模块/04-测试与验证/INDEX.md`
- 创建：`wiki/03-功能模块/05-工具能力/INDEX.md`
- 创建：`wiki/04-工具与集成/INDEX.md`
- 创建：`wiki/05-项目经验/INDEX.md`

- [ ] **步骤 1：创建顶层和二级目录的 `INDEX.md`**

要求：
- 使用固定模板
- 索引本层子类别与重要文档

- [ ] **步骤 2：创建项目强绑定 skill 页面**

要求：
- 按固定模板描述项目职责、输入输出、边界与关联 skill

- [ ] **步骤 3：创建 `skill-registry.md`**

要求：
- 登记 `.codex/skills/` 下全部 skill
- 标记 `core / project / general`
- 对项目强绑定 skill 提供相关页面路径

## Chunk 5: 收尾文档与验证

### 任务 8：更新仓库说明文档

**文件：**
- 修改：`.codex/README.md`
- 修改：`.codex/RELEASE-NOTES.md`

- [ ] **步骤 1：补充新增 skill 与项目目录说明**

补充点：
- `docs/executions` 的职责
- `wiki/` 的职责
- 新增两个 skill 的位置和作用

- [ ] **步骤 2：补充 release notes**

预期：
- 记录新增 skill
- 记录新增测试
- 记录新增 wiki 结构与执行文档规则

### 任务 9：运行最终验证

**文件：**
- 测试：`.codex/tests/claude-code/run-skill-tests.ps1`

- [ ] **步骤 1：运行新增的单项测试**

运行：
```powershell
powershell -NonInteractive -File .codex/tests/claude-code/test-active-execution-guard.ps1
powershell -NonInteractive -File .codex/tests/claude-code/test-project-wiki-maintenance.ps1
powershell -NonInteractive -File .codex/tests/claude-code/test-executing-plans.ps1
powershell -NonInteractive -File .codex/tests/claude-code/test-subagent-driven-development.ps1
```

预期：
- 全部通过

- [ ] **步骤 2：运行测试汇总脚本**

运行：
```powershell
powershell -NonInteractive -File .codex/tests/claude-code/run-skill-tests.ps1 --test test-active-execution-guard.ps1
powershell -NonInteractive -File .codex/tests/claude-code/run-skill-tests.ps1 --test test-project-wiki-maintenance.ps1
powershell -NonInteractive -File .codex/tests/claude-code/run-skill-tests.ps1 --test test-executing-plans.ps1
```

预期：
- 运行器能找到并执行新增测试

- [ ] **步骤 3：人工检查关键目录**

检查：
- `docs/booming/2026-04-02-harness-engineer-automation/`
- `wiki/`
- `.codex/skills/`

预期：
- 目录结构与设计一致
