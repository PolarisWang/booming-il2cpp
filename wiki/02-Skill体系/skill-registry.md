# Skill Registry

| Skill | 级别 | 分层 | 项目职责 | 实现路径 | 相关页面 |
| --- | --- | --- | --- | --- | --- |
| `dev-using-booming` | `core` | 主线工作流 | 全局 skill 入口与分流 | `.codex/skills/dev-using-booming/SKILL.md` | [`01-主线工作流/using-booming.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/using-booming.md) |
| `dev-brainstorm` | `core` | 主线工作流 | 需求澄清、方案比较、设计冻结 | `.codex/skills/dev-brainstorm/SKILL.md` | [`01-主线工作流/brainstorming.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/brainstorming.md) |
| `dev-roadmap` | `core` | 主线工作流 | 多阶段 roadmap 设计与分解 | `.codex/skills/dev-roadmap/SKILL.md` | [`01-主线工作流/roadmap.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/roadmap.md) |
| `dev-writing-plans` | `core` | 主线工作流 | 单任务计划、obligation intake 与固定收尾约束 | `.codex/skills/dev-writing-plans/SKILL.md` | [`01-主线工作流/writing-plans.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/writing-plans.md) |
| `dev-executing-plans` | `core` | 主线工作流 | 执行计划并驱动 formal verification gate、架构审视、测试、归档 | `.codex/skills/dev-executing-plans/SKILL.md` | [`01-主线工作流/executing-plans.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/executing-plans.md) |
| `dev-subagent-driven-development` | `core` | 主线工作流 | 子 agent 协同执行计划，并与顺序主线共享同一 formal verification gate | `.codex/skills/dev-subagent-driven-development/SKILL.md` | [`01-主线工作流/subagent-driven-development.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/subagent-driven-development.md) |
| `dev-finishing-a-development-branch` | `core` | 主线工作流 | 归档后的本地合并、提交与收尾 | `.codex/skills/dev-finishing-a-development-branch/SKILL.md` | [`01-主线工作流/finishing-a-development-branch.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/finishing-a-development-branch.md) |
| `dev-using-git-worktrees` | `project` | 主线工作流 | 隔离工作区与 git 工作流 | `.codex/skills/dev-using-git-worktrees/SKILL.md` | [`01-主线工作流/using-git-worktrees.md`](./01-%E4%B8%BB%E7%BA%BF%E5%B7%A5%E4%BD%9C%E6%B5%81/using-git-worktrees.md) |
| `dev-project-wiki-maintenance` | `core` | 项目知识维护 | 长期知识落库、更新 `INDEX.md` | `.codex/skills/dev-project-wiki-maintenance/SKILL.md` | [`03-项目知识维护/project-wiki-maintenance.md`](./03-%E9%A1%B9%E7%9B%AE%E7%9F%A5%E8%AF%86%E7%BB%B4%E6%8A%A4/project-wiki-maintenance.md) |
| `dev-test-driven-development` | `project` | 质量保障 | TDD 纪律与失败测试优先，不持有 owner / obligation authority | `.codex/skills/dev-test-driven-development/SKILL.md` | [`04-质量保障/test-driven-development.md`](./04-%E8%B4%A8%E9%87%8F%E4%BF%9D%E9%9A%9C/test-driven-development.md) |
| `dev-project-test-governance` | `project` | 质量保障 | 统一 `subjects/test/Chaos.TestFramework/collection/runner/codegen` 主线治理，冻结 authority / obligation intake，约束 IL2CPP / Python codegen 优先 Scriban，并包含 CodeGen 快照测试强制规则 | `.codex/skills/dev-project-test-governance/SKILL.md` | [`04-质量保障/project-test-governance.md`](./04-%E8%B4%A8%E9%87%8F%E4%BF%9D%E9%9A%9C/project-test-governance.md) [`../../06-测试验证/CodeGen快照测试规范.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/CodeGen%E5%BF%AB%E7%85%A7%E6%B5%8B%E8%AF%95%E8%A7%84%E8%8C%83.md) |
| `dev-systematic-debugging` | `project` | 质量保障 | 根因分析与调试收口 | `.codex/skills/dev-systematic-debugging/SKILL.md` | [`04-质量保障/systematic-debugging.md`](./04-%E8%B4%A8%E9%87%8F%E4%BF%9D%E9%9A%9C/systematic-debugging.md) |
| `dev-verification-before-completion` | `project` | 质量保障 | 完成前 formal object 选择与验证证据收口，含 CodeGen 快照测试专项检查 | `.codex/skills/dev-verification-before-completion/SKILL.md` | [`04-质量保障/verification-before-completion.md`](./04-%E8%B4%A8%E9%87%8F%E4%BF%9D%E9%9A%9C/verification-before-completion.md) [`../../06-测试验证/CodeGen快照测试规范.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/CodeGen%E5%BF%AB%E7%85%A7%E6%B5%8B%E8%AF%95%E8%A7%84%E8%8C%83.md) |
| `dev-requesting-code-review` | `project` | 质量保障 | 结构化代码审查请求 | `.codex/skills/dev-requesting-code-review/SKILL.md` | `-` |
| `dev-receiving-code-review` | `project` | 质量保障 | 接收并处理代码审查 | `.codex/skills/dev-receiving-code-review/SKILL.md` | `-` |
| `dev-dispatching-parallel-agents` | `project` | 质量保障 | 并行 agent 任务调度 | `.codex/skills/dev-dispatching-parallel-agents/SKILL.md` | `-` |
| `dev-writing-skills` | `general` | 通用能力 | skill 编写方法论 | `.codex/skills/dev-writing-skills/SKILL.md` | `-` |
| `tools-feishu-docs` | `general` | 工具能力 | 飞书公开文档读取与导出 | `.codex/skills/tools-feishu-docs/SKILL.md` | `-` |
| `tools-find-skills` | `general` | 工具能力 | skill 发现与安装 | `.codex/skills/tools-find-skills/SKILL.md` | `-` |
| `profile-code-optimization` | `general` | 分析能力 | Tracy 热点代码优化分析 | `.codex/skills/profile-code-optimization/SKILL.md` | `-` |
| `profile-tracy-profile` | `general` | 分析能力 | Tracy 性能分析工作流 | `.codex/skills/profile-tracy-profile/skill.md` | `-` |
| `dev-il2cpp-core-agent` | `project` | 主线工作流 | il2cpp 核心开发统一入口 — 分类任务并路由到 Expert Agent | `skills/library/skills/dev-il2cpp-core-agent/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-translation-expert` | `project` | il2cpp | IL→C++ 翻译路径专家 | `skills/library/skills/dev-il2cpp-translation-expert/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-runtime-expert` | `project` | il2cpp | runtime-core/interpreter/bootstrap 专家 | `skills/library/skills/dev-il2cpp-runtime-expert/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-debug-expert` | `project` | il2cpp | il2cpp 领域系统性调试专家 | `skills/library/skills/dev-il2cpp-debug-expert/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-gc-expert` | `project` | il2cpp | CRAG GC 架构、分代策略、写屏障专家 | `skills/library/skills/dev-il2cpp-gc-expert/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-codegen-expert` | `project` | il2cpp | C# codegen 管线、Scriban 模板、snapshot 专家 | `skills/library/skills/dev-il2cpp-codegen-expert/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-hotupdate-expert` | `project` | il2cpp | Hotpatch/PatchLoader/.patchdata 热更新专家 | `skills/library/skills/dev-il2cpp-hotupdate-expert/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-foundation-dll-optimizer` | `project` | il2cpp | foundation-dll 深度优化专家（全平台对比、瓶颈分析、迭代优化） | `skills/library/skills/dev-il2cpp-foundation-dll-optimizer/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |
| `dev-il2cpp-build-fixer` | `project` | il2cpp | 编译失败与 codegen stub 修复专家（四层分层诊断） | `skills/library/skills/dev-il2cpp-build-fixer/SKILL.md` | [`05-il2cpp核心开发Agent体系.md`](./05-il2cpp核心开发Agent体系.md) |

## 规则

- `core`：主线入口级技能，负责分流或推进正式开发流程。
- `project`：本仓库长期使用的项目级技能，负责测试、验证、文档治理等仓库特有规则。
- `general`：通用能力或工具能力，只做登记和索引。

## 最近更新

- `2026-04-17`：更新 `dev-writing-plans`、`dev-executing-plans`、`dev-subagent-driven-development` 与 `dev-finishing-a-development-branch`，固定 plan 收尾链路为“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”。
- `2026-04-17`：更新 `dev-systematic-debugging`、`dev-test-driven-development`、`dev-verification-before-completion` 与 `dev-project-test-governance`，新增测试阶段 `dotnet` 编译崩溃必须追根因并修复的规则。
- `2026-04-17`：新增 `dev-project-test-governance`，并把 `subjects/test/Chaos.TestFramework/collection/runner/codegen` 相关任务正式接入主技能入口。
- `2026-04-18`：补充 AOT obligation intake 字段、formal verification gate 与 authority 分工；`dev-verification-before-completion` 成为 formal object selector，`dev-test-driven-development` 只保留 TDD 纪律。
- `2026-04-19`：更新 `dev-project-test-governance` 的项目说明，明确 IL2CPP / Python codegen 默认优先 Scriban，能力不足时先扩展 Scriban。
- `2026-04-23`：更新 `dev-using-booming`、`dev-brainstorm`、`dev-roadmap`、`dev-writing-plans`、`dev-executing-plans`、`dev-subagent-driven-development`、`dev-project-test-governance`、`dev-verification-before-completion` 与 `dev-project-wiki-maintenance`，统一“问题先清零再规划/执行”的主线，并要求命中新测试流程打通任务时在 formal object 通过后刷新 `verification-v1` 正式产物、记录结构化新数据路径。

- `2026-04-24`：补充 verification-v1 public command surface 口径，明确 `run test inventory` 已内部化，以及 `benchmark --record` 只写 raw benchmark records，不等于 formal refresh。
- `2026-04-25`：更新 `dev-project-test-governance`，补充 DLL-first reporting / projection contract 的数据刷新约束；命中报告对象、formal reports、projection 字段或证据链接规则改动时，必须同步刷新 `verification-v1` 正式数据与对应 projection。
- `2026-04-26`：更新 `dev-project-test-governance`，补充 foundation-dll-audit 双轴进度口径、capability-family ledger 分母 authority、completion-certification 的 DLL 级定位，以及 workspace 只作为 proof container 的规则。

