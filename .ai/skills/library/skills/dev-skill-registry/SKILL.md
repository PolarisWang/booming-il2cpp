---
name: dev-skill-registry
description: 技能注册中心 — 索引 .ai/skills/library/skills/ 下所有技能的目录、领域和用途
---

# dev-skill-registry

> ⚠️ **已弃用**：本 registry 是手工维护的旧版本，不再保证与发现系统同步。
> 请使用自动生成的 `.ai/skills/discovery/skill-index.md` + `.ai/skills/discovery/registries/<domain>.md` 作为权威来源。
>
> 自动生成命令: `python .ai/skills/tooling/catalog/generate_skill_catalog.py`
> 注册表文件: `.ai/skills/discovery/registries/` (7 个域)

## 用途

技能注册中心，索引 `.ai/skills/library/skills/` 下所有技能的目录、领域和用途。

当用户请求 `/dev-<name>` 时，Agent 通过此 registry 查找技能并加载对应的 `SKILL.md`。

## 注册表

### 核心开发技能

| 技能 | 领域 | 用途 |
|------|------|------|
| dev-architecture-first-development | il2cpp | 开发 il2cpp 翻译逻辑前必须执行的架构查询与路径匹配 |
| dev-systematic-debugging | il2cpp | 四阶段系统性调试：根因调查 → 模式分析 → 假设验证 → 实现修复 |
| dev-test-driven-development | testing | 先补失败测试，再改实现 |
| dev-il2cpp-core-agent | il2cpp | 核心开发统一入口，分类任务并路由到 Expert Agent |
| dev-il2cpp-codegen-expert | il2cpp | CodeGen 专家：C# codegen 管线、Planner、Emission、Scriban 渲染 |
| dev-il2cpp-gc-expert | il2cpp | GC 专家：CRAG GC 架构、分代分析、写屏障、stress test |
| dev-il2cpp-hotupdate-expert | il2cpp | 热更新专家：Hotpatch 双层分派、PatchLoader、解释器路由 |
| dev-il2cpp-debug-expert | il2cpp | 调试专家：集成 trace 系统和 il2cpp 故障模式索引 |
| dev-il2cpp-runtime-expert | il2cpp | 运行时专家：runtime-core/interpreter/bootstrap/support |
| dev-il2cpp-translation-expert | il2cpp | 翻译路径专家：IL→C++ 翻译路径选择、Planner/Emission 修改 |

### 质量保障技能

| 技能 | 领域 | 用途 |
|------|------|------|
| dev-verification-before-completion | testing | 完成前验证 |
| dev-requesting-code-review | workflow | 完成任务后请求代码审查 |
| dev-receiving-code-review | workflow | 接收代码审查反馈 |
| dev-trace-enforcement | testing | Trace 系统强制检查 |

### 设计规划技能

| 技能 | 领域 | 用途 |
|------|------|------|
| dev-brainstorm | planning | 创意发散和方案探索 |
| dev-roadmap | planning | 多阶段路线图规划 |
| dev-writing-plans | planning | 编写实施计划 |
| dev-executing-plans | planning | 执行已批准的计划 |
| dev-dispatching-parallel-agents | workflow | 并行任务分发 |

### 构建与优化技能

| 技能 | 领域 | 用途 |
|------|------|------|
| dev-optimization-campaign | optimization | 全自动 family 优化流水线 |
| dev-pressure-test-management | testing | 压力测试管理 |
| dev-project-test-governance | testing | 项目测试治理 |
| dev-native-debug-profiling | native | Native 调试与性能分析规范 |

### 工程工作流技能

| 技能 | 领域 | 用途 |
|------|------|------|
| dev-finishing-a-development-branch | workflow | 完成开发分支的合并与收口 |
| dev-using-git-worktrees | workflow | Git worktree 隔离开发 |
| dev-cpp-conventions | native | C++ 编码规范 |
| dev-unordered-map-selection | native | unordered_map 选型规则 |
| dev-subagent-driven-development | workflow | Subagent 驱动的开发模式 |
| dev-using-booming | workflow | 对话入口技能路由 |

### 元技能

| 技能 | 领域 | 用途 |
|------|------|------|
| dev-skill-evolution | meta | 技能健康检查和进化管理 |
| dev-writing-skills | meta | 创建和编辑技能 |
| dev-project-wiki-maintenance | meta | 项目 wiki 维护 |

## 注册表维护

技能列表与 `.ai/skills/library/skills/` 目录保持同步。新增/删除技能时：

1. 在 `.ai/skills/library/skills/<name>/` 下创建 SKILL.md + skill.manifest.json
2. 更新 `.ai/skills/discovery/skill-index.md`（运行 `python .ai/skills/tooling/catalog/generate_skill_catalog.py`）
3. 更新此注册表的 markdown 表格

> **注意**：`.claude/skills/` 现在是 `.ai/skills/library/skills/` 的符号链接，无需手动同步。
