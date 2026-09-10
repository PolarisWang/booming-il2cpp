# Metadata Writer 工业化收官 Roadmap

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** MetadataWriterStage.cs（339 行，单文件）全面工业化收官，弥合测试覆盖、CI/CD 门禁、文档、架构审视全部缺口，达到与 codegen/linker-driver 同等的工业化标准。

**架构：** 不改变 MetadataWriterStage 的职责边界和 artifact 格式。测试层新增单元测试 + 契约测试，CI 层沿袭 codegen-regression.yml 模式，文档层在 wiki 翻译管线目录下新增独立页面。

**技术栈：** C# (.NET 8), xUnit, coverlet, GitHub Actions (YAML)

**架构审核模式：** normal

**结构告警重点：** MetadataWriterStage.cs 当前 339 行混入 3 artifact 构建逻辑 + 泛型辅助方法。超过 500 行时考虑拆分 helper 类。

**权责图审核主题：** n/a

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: MetadataIndustrialization
- capabilityItem: M1-test-core, M1-arch-review, M2-ci-gate, M3-docs
- ownerSubjectId: 20260523-metadata-writer-industrialization
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: [MetadataWriterStage.Write() core path tests, contract integration tests]
- requiredGates: test-pass -> ci-pass -> docs-archived

**设计文档：** n/a（设计摘要汇总在 STATUS.md）

**问题清零来源：** direct user confirmation（综合评估已确认全部缺口和边界）

**计划来源：** direct-plan（评估完成 → 直接创建 roadmap）

**预期知识沉淀：** wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/ → MetadataWriter 独立页面

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed"固定链路。

---

## 为什么需要 Roadmap

Metadata Writer 工业化涉及 4 个子任务、3 个阶段，有依赖关系（CI 门禁依赖测试就绪），需要串行阶段编排。子任务之间存在明确的 phase gate，测试→CI→文档的推进顺序决定了需要 roadmap 而非单一 plan。

---

## 阶段列表

### Phase 1: 核心测试 + 架构审视

| 字段 | 值 |
|------|-----|
| goal | 弥合最大风险缺口：`Write()` 无直接测试、架构无审查、错误处理过粗 |
| exit_criteria | Write() 核心路径全覆盖、契约测试通过、架构审视完成、错误处理细化 |
| dependencies | 无（初始阶段） |
| resolved_decisions | 测试放在 Chaos.IL2CPP.CodeGen.Tests 项目；沿袭 xUnit 模式；复用已有 test builder 构造 LinkedWorldModel |
| watch_items | 构造 LinkedWorldModel 的复杂性；如果原生构造过于复杂，考虑引入 builder 模式 |

### Phase 2: CI/CD + 质量门禁

| 字段 | 值 |
|------|-----|
| goal | 新增测试能在 CI 中自动触发，覆盖率基线建立 |
| exit_criteria | GitHub Actions workflow 触发 MetadataWriter 测试，coverlet 覆盖率基线建立且阈值生效 |
| dependencies | Phase 1 完成（需要测试存在才能 CI） |
| resolved_decisions | 沿袭 codegen-regression.yml 模式；覆盖率阈值参考现有 codegen 基线 |
| watch_items | CI runner 环境是否满足测试依赖 |

### Phase 3: 文档收官

| 字段 | 值 |
|------|-----|
| goal | Metadata Writer 文档从"被引用但无独立页面"达到"功能完整文档化" |
| exit_criteria | wiki 独立页面创建、API 文档覆盖全部公开成员、故障诊断指南覆盖至少 3 种典型失败场景 |
| dependencies | 无（可独立推进，也可与 Phase 2 并行执行） |
| resolved_decisions | 文档放在 wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/；故障诊断指南作为同一页面中的独立章节 |
| watch_items | 无 |

---

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|-------------|-------------|---------------|----------------|-----------------|
| **M1-arch-review** | P1 | ready | FT | 架构审视 + 错误处理细化 | 无 | 审阅 MetadataWriterStage.cs 职责边界、错误分类、文件拆分决策 | STATUS.md 架构审视结论 | 审视结论写入 STATUS.md、错误处理从单一 METADATA_WRITER_FAILED 细化到至少 3 种错误码、文件拆分决策已记录 | `src/managed/Chaos.IL2CPP.MetadataWriter/` | 0.5 天 |
| **M1-test-core** | P1 | ready | FT | Write() 核心路径测试 + 契约测试 | 无 | 构造 LinkedWorldModel、验证 3 artifact 产出、验证 CodeGen 契约 | MetadataWriterStageTests.cs 新增测试 | 全部 3 个 artifact 的字段映射被验证、CodeGen 消费路径被契约测试锁定 | `tests/managed/Chaos.IL2CPP.CodeGen.Tests/` | 2 天 |
| **M2-ci-gate** | P2 | planned | FT | CI 门禁 + 覆盖率门禁 | M1-test-core | 将 MetadataWriter 测试纳入 CI workflow、配置 coverlet 基线 | .github/workflows/ 更新 + coverlet.runsettings 更新 | PR 修改 MetadataWriterStage.cs 自动触发 CI、覆盖率阈值生效 | `.github/workflows/` + `tests/managed/Chaos.IL2CPP.CodeGen.Tests/` | 1 天 |
| **M3-docs** | P3 | planned | FT | Wiki 文档 + 故障诊断指南 | 无 | 编写独立 wiki 页面 | wiki/ 新页面 + API 文档 | 页面包含职责/输入输出/artifact 格式/3 种故障场景 | `wiki/` | 1 天 |

---

## 依赖关系图

```
M1-arch-review ──────────────────── 无下游依赖（独立审查）
                                     ↘
M1-test-core ──────────→ M2-ci-gate ──→ M3-docs
                              ↑              ↑
                              └── 依赖测试    └── 可独立并行（与 Phase 2 并行）
```

---

## 风险

| 风险 | 级别 | 概率 | 影响 | 缓解 |
|------|------|------|------|------|
| LinkedWorldModel 构造复杂度过高 | 中 | 中 | 高 | 先评估现有 test builder（project_workspace.py），必要时引入 Builder 模式 |
| CI workflow 权限/环境不足 | 低 | 低 | 中 | 沿袭 codegen-regression.yml 模式，已验证可行 |
| 覆盖率阈值设置不合理 | 低 | 低 | 低 | 参考现有 codegen 基线（38.5%），按 MetadataWriter 实际可测比例设定 |

---

## 备选路径

如果 M1-test-core 遇到 LinkedWorldModel 构造不可逾越的复杂性：
- **备选 A**：退而求其次，将 Write() 拆为多个可独立测试的内部方法（如 `BuildAotManifest`/`BuildMetadataRegistration`/`BuildSupplementalTemplate`），分别测试
- **备选 B**：先做 M3-docs 和 M2-ci-gate（无阻塞），等基础设施成熟后再补齐 M1

---

## 当前建议推进顺序

```
Phase 1 (并行):
  M1-arch-review ─┐
                  ├── 并行执行
  M1-test-core   ─┘

Phase 2 (串行, 依赖 Phase 1):
  M2-ci-gate

Phase 3 (可并行/串行):
  M3-docs
```

---

## 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`（除非子任务失败或出现 blocker）
- `auto_stop_policy: blocking-only`
- `recommended_next_child: M1-test-core`

### 子任务启动规则

- M1-arch-review 和 M1-test-core 无依赖关系，**可并行启动**
- 先启动 M1-test-core（最高风险项），M1-arch-review 可同步或稍后启动
- Phase 1 全部完成后自动推进 Phase 2

### entry_skill 规则

| 子任务 | entry_skill | 原因 |
|--------|------------|------|
| M1-test-core | dev-writing-plans | 需要 plan 来规划测试用例和测试策略 |
| M1-arch-review | dev-writing-plans | 需要 plan 来规划审视范围和错误码设计 |
| M2-ci-gate | dev-writing-plans | CI 配置变更需要 plan 确保不破坏现有 workflow |
| M3-docs | dev-executing-plans | 边界清晰，直接执行即可 |

---

## 退出条件

1. **M1-arch-review**: 架构审视结论写入 STATUS.md、错误码细化（至少 3 种）、文件拆分决策已记录
2. **M1-test-core**: 全部 3 个 artifact 的字段映射被测试锁定、CodeGen 契约一致性已验证
3. **M2-ci-gate**: MetadataWriter 测试在 CI 中自动触发、coverlet 基线已建立
4. **M3-docs**: wiki 独立页面创建、API 文档完整、故障诊断指南覆盖 3 种场景
5. **全量验证**: `dotnet build Chaos.IL2CPP.CodeGen.Tests` 零 warning、所有测试通过

## 验证命令

```bash
# 1. 构建
cd src/managed && dotnet build Chaos.IL2CPP.CodeGen.Tests

# 2. 运行 MetadataWriter 测试
dotnet test --filter "FullyQualifiedName~MetadataWriter"

# 3. 覆盖率
dotnet test --collect:"XPlat Code Coverage" --settings coverlet.runsettings

# 4. 全管线集成验证（可选）
cd testing/foundation-dll && python run_all.py --quick
```

---

## 完成归档

完成时移动 `docs/dev/in-progress/20260523-metadata-writer-industrialization/` 到 `docs/dev/completed/`。
