---
task_id: 20260523-metadata-writer-industrialization
task_type: roadmap
phase: roadmap
lifecycle_status: in-progress
roadmap_or_plan: roadmap-v1-01.md
design_doc: n/a（设计摘要见 STATUS.md）
arch_review_mode: normal
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
clearance_source: direct user confirmation（综合评估已确认全部缺口）
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
recommended_next_child: M1-test-core
---

# Metadata Writer 工业化收官

> **父任务:** 无（顶级 roadmap）
> **创建日期:** 2026-05-23
> **下游执行:** roadmap-v1-01.md

---

## Phase

`roadmap`

## Type

`roadmap`

## Roadmap or Plan

`roadmap-v1-01.md`

## 关键文档

- [Roadmap 路线图](roadmap-v1-01.md) — 阶段定义、子任务映射、退出标准

---

## 设计摘要

### 边界拍板

- **范围**: MetadataWriterStage.cs（单文件 339 行）的全面工业化，涵盖测试覆盖、CI/CD 门禁、文档、架构审视
- **非目标**: 不重新设计架构、不改造 artifact 格式、不改 PipelineResult 契约、不修改 Contracts 模型层
- **纳入产出 artifact**: AotManifest / MetadataRegistration / SupplementalMetadataTemplate（全部 3 个 artifact）

### 缺口总清单

| # | 缺口 | 当前状态 | 目标 |
|---|------|---------|------|
| MW-0 | 架构审视 | 单文件 339 行，职责混入 | 错误处理细化、文件拆分评估 |
| MW-1 | `Write()` 核心路径测试 | 无直接测试，仅 21 个辅助方法测试 | 全覆盖 artifact 产出验证 |
| MW-2 | 契约集成测试 | 无 | 验证 MetadataWriter→CodeGen 契约一致性 |
| MW-3 | CI 门禁 | 无 codegen-regression 覆盖 | GitHub Actions 触发 |
| MW-4 | 覆盖率门禁 | 不存在 | coverlet 基线 + 阈值 |
| MW-5 | 文档 | 无独立 wiki 页面 | API 文档 + 故障诊断指南 |

### 风险评估摘要

| 风险 | 级别 | 缓解 |
|------|------|------|
| `Write()` 测试需要构造复杂 LinkedWorldModel | 中 | 复用已有 test builder（test_metadata_supplement_bridge.py / project_workspace.py） |
| CI 配置可能受当前 CI 基础设施限制 | 低 | 沿用 codegen-regression.yml 现有模式 |
| artifact 契约变更是隐式的 | 低 | 契约测试锁定字段名和类型，变更时强制更新 |
| 单文件规模增长 | 低 | 控制在 500 行以内，超过则拆分 |

### Authority 决策

- MetadataWriterStage.cs 保持为单文件，超过 500 行后拆分 helper 类
- 测试放在 `Chaos.IL2CPP.CodeGen.Tests` 项目（沿袭现有模式）
- Wiki 页面放在 `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/` 目录

---

## 最近摘要

2026-05-23（初始化）: 在 codegen 工业化全部收官后，对 Metadata Writer 模块进行工业化成熟度评估。确认 6 项缺口（零测试覆盖核心路径、无 CI 门禁、无覆盖率门禁、无文档、无契约测试、架构需审视），总预估 3-5 天 FT。

## 调度状态

```yaml
dispatch_doc: n/a（sequential 模式，不创建 DISPATCH.md）
dispatch_model: sequential
active_batches: []
completed_batches: []
pending_batches: [Phase 1, Phase 2, Phase 3]
```

## 子任务状态

| task_id | phase | status | owner | purpose |
|---------|-------|--------|-------|---------|
| M1-test-core | P1 | **ready** | FT | Write() 核心路径测试 + 契约集成测试 |
| M1-arch-review | P1 | **ready** | FT | 架构审视 + 错误处理细化 |
| M2-ci-gate | P2 | **planned** | FT | CI 门禁 + 覆盖率门禁 |
| M3-docs | P3 | **planned** | FT | Wiki 文档 + 故障诊断指南 |

## 执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

## Latest Stop Point

Metadata Writer 工业化 roadmap 初始化完成。所有 6 项缺口已确认，3 个阶段 4 个子任务已定义。

## 下一步

Phase 1（P1）: 执行子任务 M1-test-core（Write() 核心路径测试 + 契约集成测试）和 M1-arch-review（架构审视）。

---

## 入口

`entry_skill` = 按子任务类型分配（详见 roadmap-v1-01.md §10）

## Question Clearance

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## Child Execution Mode

`auto`

## Clearance Source

`direct user confirmation`

## Recommended Next Child

`M1-test-core`
