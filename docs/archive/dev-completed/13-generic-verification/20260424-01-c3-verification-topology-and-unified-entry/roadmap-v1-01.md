# C3 Verification Topology And Unified Entry Roadmap v1.01

Date: 2026-04-24 00:00:00 +08:00
Status: completed

## 1. 目标

把当前已确认的 C3 设计收口成一次性的 `hard cut` 落地路线图，完成以下结果：

- 顶层 `verification/` 成为唯一正式 verification root。
- `docs/testing-inventory/verification/*` 与 `subjects/*/verification/*` 全部退出长期 authority。
- `subjects/*` 中承载测试体系 authority 的内容全部迁出，当前 3 套 owner pack 默认整包退出主线。
- `tests/` 中 verification-framework 专属测试统一收口到 `tests/verification/*`。
- `wiki`、`skill`、`CLI`、`TUI`、workspace、projection、archive 一次性切到新根，不保留长期兼容层。

## 2. 范围边界

本 roadmap 覆盖：

- `verification/catalog/*`
- `verification/archive/*`
- `verification/evidence/owners/*`
- `verification/workspaces/*`
- `verification/projections/*`
- `build/toolchains/run/*` 中与 verification root、workspace、public entry、projection、reporting 相关的实现
- `subjects/*` 中属于 verification-framework 的 manifest / features / proofs / benchmarks / engineering scenarios / support source / baselines / verification stubs
- `tests/` 中 verification 专属测试的物理迁移与断言切换
- `wiki/06-测试验证/*`
- `wiki/02-Skill体系/*`
- `.codex/skills/*` 中与 verification formal source、completion gate、test governance 直接相关的技能

本 roadmap 不覆盖：

- 新增新的 capability 家族
- 新增新的 route lane
- 重新设计 `verification-v1` 的 claim / evidence 基本语义
- 保留旧 public path 的长期兼容层

## 3. 非目标

- 不做 dual-write。
- 不做 shadow path。
- 不保留 `subjects/*` 作为长期测试 authority root。
- 不手工维护总 `.sln`。
- 不把 `.artifact/` 升格为正式 archive 目录。

## 4. 阶段列表

### Phase 0: Contract Freeze And Skeleton

- `goal`
  - 冻结 `verification/` 根结构、正式路径、目录职责、子任务切分和硬切边界。
- `exit_criteria`
  - `roadmap-v1-01.md`、`design-v1-01.md`、`subject-cutover-mapping-v1-01.md` 三者对齐。
  - `subjects/`、`tests/`、`wiki/skill` 的最终落点清单已固定。
  - 生成总 `.sln`、formal source、owner evidence、projection 的稳定路径已固定。
- `deliverables`
  - 本 roadmap
  - `subject-cutover-mapping-v1-01.md`
  - Phase checklist 冻结
- `dependencies`
  - `design-v1-01.md`
- `resolved_decisions`
  - 方案为 `Full Authority Migration`
  - cutover 模式为 `one-shot hard cut`
  - `run.py` 是唯一真实执行入口
  - `verification-all.sln` 只是 IDE facade
- `watch_items`
  - 不允许在 roadmap 中重新引入“兼容保留旧路径”的描述

### Phase 1: Catalog And Subject Authority Migration

- `goal`
  - 把 `subjects/*` 中承载 verification authority 的内容迁到 `verification/catalog/*`。
- `exit_criteria`
  - `owner.manifest.json`
  - `owner.features.json`
  - `proofs/**`
  - `benchmarks/**`
  - `support/**`
  - `benchmark-baselines/**`
  - `scenarios/**`
  - 全部能从 `verification/catalog/*` 恢复。
- `deliverables`
  - `verification/catalog/owners/*`
  - `verification/catalog/scenarios/*`
  - subject migration adapters
- `dependencies`
  - Phase 0
- `resolved_decisions`
  - `subject.features.json` 的 authority 迁入 `verification/catalog/owners/<owner>/owner.features.json`
  - `EngineeringScenarios` 迁入 `verification/catalog/scenarios/<owner>/*`
- `watch_items`
  - 不允许新实现继续从 `subjects/*` 扫描 owner / proof / benchmark / scenario

### Phase 2: Archive, Evidence, Projection Writer Cutover

- `goal`
  - 把 formal archive、owner evidence、benchmark records merge、projection writer 全部切到 `verification/`。
- `exit_criteria`
  - `verification/archive/latest/*`
  - `verification/archive/master/*`
  - `verification/archive/reports/*`
  - `verification/evidence/owners/*/codegen-stubs/*`
  - `verification/projections/*`
  - 全部可稳定生成。
- `deliverables`
  - archive writer cutover
  - evidence writer cutover
  - projection writer cutover
  - benchmark records merge path cutover
- `dependencies`
  - Phase 1
- `resolved_decisions`
  - 原始 benchmark records 只进 `.artifact/verification/benchmark-records/*`
  - 正式归并结果只进 `verification/archive/*`
- `watch_items`
  - 不允许 `artifacts/**` 被误当 formal source

### Phase 3: Workspace And Public Entry Cutover

- `goal`
  - 把 workspace、manifest、CLI/TUI public output、首页入口全部切到 `verification/`。
- `exit_criteria`
  - `verification/workspaces/verification-all.sln`
  - `verification/workspaces/subjects/*`
  - `verification/workspaces/routes/*`
  - `verification/workspaces/core/*`
  - `verification/INDEX.md`
  - `verification/verification.manifest.json`
  - 全部可生成且旧 `solutions/*` 不再作为 verification public entry。
- `deliverables`
  - workspace generators
  - public entry cutover
  - manifest/index cutover
- `dependencies`
  - Phase 2
- `resolved_decisions`
  - 统一可见入口固定为 `verification/INDEX.md`、`verification/workspaces/verification-all.sln`、`run.py`
- `watch_items`
  - 不允许把非 .NET route/platform 资产重新压回 `subjects/*`

### Phase 4: Tests Relayout And Discovery Cutover

- `goal`
  - 把 verification 专属测试物理迁到 `tests/verification/*`，并把扫描根切到 `verification/*`。
- `exit_criteria`
  - `tests/verification/unit/*`
  - `tests/verification/integration/*`
  - `tests/verification/tooling/run/*`
  - `tests/verification/contracts/*`
  - 已建立且主要脚本迁移完成。
  - 保留在旧目录的通用测试不再引用旧 verification public path。
- `deliverables`
  - new tests tree
  - discovery cutover
  - negative assertions for old public path retirement
- `dependencies`
  - Phase 1
  - Phase 2
  - Phase 3
- `resolved_decisions`
  - verification-framework 专属测试统一收口到 `tests/verification/*`
  - 纯 legacy 入口存在性测试直接删除
- `watch_items`
  - 不允许为兼容旧路径保留双断言

### Phase 5: Wiki And Skill Cutover

- `goal`
  - 把长期知识入口与 skill 约束同步切到新框架口径。
- `exit_criteria`
  - `wiki/06-测试验证/*` 不再宣称旧 formal path
  - `wiki/02-Skill体系/*` 不再宣称旧 formal path
  - `.codex/skills/*` 中 formal source / completion gate / governance 路径切到 `verification/*`
- `deliverables`
  - wiki cutover
  - skill cutover
  - skill registry refresh
- `dependencies`
  - Phase 2
  - Phase 3
  - Phase 4
- `resolved_decisions`
  - wiki 只认 `verification/*` 为正式路径
  - skill 只认 `verification/*` 为 formal source
- `watch_items`
  - 不允许保留“旧路径也可接受”的兼容描述

### Phase 6: Legacy Hard Cut And Repo Cleanup

- `goal`
  - 删除旧 authority、旧 public path、旧 tests、旧 subject pack 目录。
- `exit_criteria`
  - `docs/testing-inventory/verification/*` 删除
  - `subjects/*/verification/*` 删除
  - 当前 3 套 owner pack 如无非 verification 职责残留则整包删除
  - 旧 `solutions/*` verification public entry 删除
  - 旧 alias / old path constants / legacy TUI menu / old help text 删除
- `deliverables`
  - hard cut cleanup commit
  - legacy path scan result
- `dependencies`
  - Phase 4
  - Phase 5
- `resolved_decisions`
  - 不保留长期 compatibility shim
- `watch_items`
  - 删除前必须确认新根已完整可用

### Phase 7: Acceptance And Archive

- `goal`
  - 完成最终验证、报告、归档和 roadmap closeout。
- `exit_criteria`
  - formal archive 能在新根下稳定重建
  - owner evidence 能在新根下稳定重建
  - workspace / CLI / TUI / wiki / skill 全部对齐
  - 仓库内不存在旧 verification public path 主引用
- `deliverables`
  - closeout verification evidence
  - final report
  - updated `STATUS.md`
- `dependencies`
  - Phase 6
- `resolved_decisions`
  - closeout 必须以正式新路径为准，而不是以过程日志为准
- `watch_items`
  - 不允许只凭运行日志宣称“新框架已打通”

## 5. 每阶段完成定义

### Phase 0

- `roadmap-v1-01.md` 已生成
- `STATUS.md` 已切到 roadmap 阶段
- 下游唯一下一子任务已明确

### Phase 1

- 当前 3 套 owner pack 的 authority 映射都能在 `verification/catalog/*` 找到对应物
- registry / planner 不再依赖 `subjects/*` discovery

### Phase 2

- `run verify verification-v1 --json` 刷新后，正式产物只落到 `verification/archive/*` 与 `verification/evidence/*`
- projection 只从新 formal source 派生

### Phase 3

- `verification-all.sln` 和 subject/route/core workspace 都在 `verification/workspaces/*`
- CLI/TUI/manifest/index 只公开新入口

### Phase 4

- verification 专属测试完成物理迁移
- `tests/verification/*` 已成为唯一 verification framework 测试主树

### Phase 5

- wiki 和 skill 文案全部切到新 authority
- skill registry 中不再描述旧框架为主线

### Phase 6

- 仓库不再保留旧 public path 主引用
- 当前 3 套 `subjects/<owner>/` 目录在无残余职责前提下整包退出主线

### Phase 7

- 验证命令、formal source、projection、wiki、skill、legacy scan 全部通过
- roadmap 可以正式 closeout

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260424-02-c3-phase-0-contract-freeze-and-layout-skeleton` | `Phase 0` | `completed` | `codex` | 冻结最终路径、目录职责、切换边界、实施清单 | `-` |
| `20260424-03-c3-phase-1-catalog-and-subject-authority-migration` | `Phase 1` | `completed` | `codex` | 把 `subjects/*` authority 迁到 `verification/catalog/*` | `20260424-02-c3-phase-0-contract-freeze-and-layout-skeleton` |
| `20260424-04-c3-phase-2-archive-evidence-and-projection-writer-cutover` | `Phase 2` | `completed` | `codex` | 切换 archive / evidence / projection writer | `20260424-03-c3-phase-1-catalog-and-subject-authority-migration` |
| `20260424-05-c3-phase-3-workspace-and-public-entry-cutover` | `Phase 3` | `completed` | `codex` | 切换 workspace、manifest、CLI/TUI、统一首页 | `20260424-04-c3-phase-2-archive-evidence-and-projection-writer-cutover` |
| `20260424-06-c3-phase-4-tests-verification-tree-and-discovery-cutover` | `Phase 4` | `completed` | `codex` | 建立 `tests/verification/*` 并迁移 verification 专属脚本 | `20260424-05-c3-phase-3-workspace-and-public-entry-cutover` |
| `20260424-07-c3-phase-5-wiki-and-skill-cutover` | `Phase 5` | `completed` | `codex` | 切换 wiki/skill 到新 authority | `20260424-06-c3-phase-4-tests-verification-tree-and-discovery-cutover` |
| `20260424-08-c3-phase-6-legacy-hard-cut-and-repo-cleanup` | `Phase 6` | `completed` | `codex` | 删除旧 authority、旧路径、旧 subject pack | `20260424-07-c3-phase-5-wiki-and-skill-cutover` |
| `20260424-09-c3-phase-7-acceptance-and-archive` | `Phase 7` | `completed` | `codex` | 完成最终验证、报告、归档与 closeout | `20260424-08-c3-phase-6-legacy-hard-cut-and-repo-cleanup` |

## 7. 依赖

- authority 输入：
  - `design-v1-01.md`
  - `subject-cutover-mapping-v1-01.md`
  - `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`
- 当前实现入口：
  - `build/toolchains/run/run.py`
  - `build/toolchains/run/run_manifest.json`
  - `build/toolchains/run/subject/project_workspace.py`
- 下游规范入口：
  - `wiki/06-测试验证/INDEX.md`
  - `wiki/02-Skill体系/skill-registry.md`

## 8. 风险

- 如果 writer cutover 与 public entry cutover 分批过久，会形成新旧 authority 双轨。
- 如果 tests 只改断言不做物理迁移，会继续保留旧目录心智。
- 如果 wiki/skill 文案滞后，会导致执行口径和仓库实际结构长期不一致。
- 如果 `subjects/*` 只删 `verification/*` 不删整包，会留下“伪 authority root”。

## 9. 备选路径

### 方案 A：保留旧 authority，只加统一入口

不采用。会继续保留双 authority。

### 方案 B：dual-write 过渡一段时间

不采用。与用户确认的 hard cut 冲突。

### 方案 C：先 catalog，再 writer，再 public entry，再 tests/wiki/skill，最后 hard cut

采用。这是当前唯一执行路径。

## 10. 当前建议推进顺序

1. `20260424-02-c3-phase-0-contract-freeze-and-layout-skeleton`
2. `20260424-03-c3-phase-1-catalog-and-subject-authority-migration`
3. `20260424-04-c3-phase-2-archive-evidence-and-projection-writer-cutover`
4. `20260424-05-c3-phase-3-workspace-and-public-entry-cutover`
5. `20260424-06-c3-phase-4-tests-verification-tree-and-discovery-cutover`
6. `20260424-07-c3-phase-5-wiki-and-skill-cutover`
7. `20260424-08-c3-phase-6-legacy-hard-cut-and-repo-cleanup`
8. `20260424-09-c3-phase-7-acceptance-and-archive`

## 11. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child: 20260424-02-c3-phase-0-contract-freeze-and-layout-skeleton`

自动推进前的最小 `preflight self-review` 固定检查：

- authority 是否发生漂移
- 是否重新引入旧 public path
- 当前 phase 的输入依赖是否满足
- 是否出现新的 open question
- 是否会破坏 `verification/` 唯一 authority root 的结论

`preflight self-review` 结果：

- `pass`
  - 直接继续
- `warn`
  - 写入 child `STATUS.md` 后继续
- `stop`
  - 停下并回到用户确认

## 12. 详细 Checklist

### Phase 0 Checklist

- [x] 冻结 `verification/catalog/archive/evidence/workspaces/projections` 目录职责
- [x] 冻结 `subjects/*` 迁移矩阵
- [x] 冻结 `tests/verification/*` 最终树
- [x] 冻结 wiki/skill 切换清单
- [x] 冻结 legacy 清理清单

### Phase 1 Checklist

- [x] 迁移 `subject.manifest.json`
- [x] 迁移 `subject.features.json`
- [x] 迁移 proofs
- [x] 迁移 benchmarks
- [x] 迁移 engineering scenarios
- [x] 迁移 support source
- [x] 迁移 perf baselines

### Phase 2 Checklist

- [x] formal archive writer -> `verification/archive/*`
- [x] owner evidence writer -> `verification/evidence/owners/*`
- [x] projection writer -> `verification/projections/*`
- [x] benchmark records merge -> `.artifact/*` + `verification/archive/*`
- [x] `run verify verification-v1 --json` 输出切到新 formal source

### Phase 3 Checklist

- [x] 生成 `verification/verification.manifest.json`
- [x] 生成 `verification/INDEX.md`
- [x] 生成 `verification/workspaces/verification-all.sln`
- [x] 生成 `verification/workspaces/subjects/*`
- [x] 生成 `verification/workspaces/routes/*`
- [x] 生成 `verification/workspaces/core/*`
- [x] CLI/TUI/help/manifest 切到新入口

### Phase 4 Checklist

- [x] 创建 `tests/verification/unit/archive/*`
- [x] 创建 `tests/verification/unit/catalog/*`
- [x] 创建 `tests/verification/unit/evidence/*`
- [x] 创建 `tests/verification/unit/projections/testing-inventory/*`
- [x] 创建 `tests/verification/unit/projections/benchmark/*`
- [x] 创建 `tests/verification/unit/workspaces/*`
- [x] 创建 `tests/verification/integration/pipeline/*`
- [x] 创建 `tests/verification/integration/catalog/*`
- [x] 创建 `tests/verification/integration/workspaces/*`
- [x] 创建 `tests/verification/tooling/run/*`
- [x] 创建 `tests/verification/contracts/*`
- [x] 删除纯 legacy 入口存在性测试
- [x] 增加旧 public path 不存在的负向断言

### Phase 5 Checklist

- [x] 更新 `wiki/06-测试验证/INDEX.md`
- [x] 更新 `wiki/06-测试验证/Verification-V1测试流程规范.md`
- [x] 更新 `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- [x] 更新 `wiki/06-测试验证/新增测试接入规范.md`
- [x] 更新 `wiki/06-测试验证/测试分层.md`
- [x] 更新 `wiki/06-测试验证/模块映射.md`
- [x] 更新 `.codex/skills/dev-project-test-governance/SKILL.md`
- [x] 更新 `.codex/skills/dev-executing-plans/SKILL.md`
- [x] 更新 `.codex/skills/dev-subagent-driven-development/SKILL.md`
- [x] 更新 `.codex/skills/dev-verification-before-completion/SKILL.md`
- [x] 更新 `wiki/02-Skill体系/skill-registry.md`
- [x] 更新 `wiki/02-Skill体系/04-质量保障/project-test-governance.md`

### Phase 6 Checklist

- [x] 删除 `docs/testing-inventory/verification/*`
- [x] 删除 `subjects/*/verification/*`
- [x] 删除当前 3 套 owner pack 目录
- [x] 删除旧 `solutions/*` verification public entry
- [x] 删除旧 alias / old path constants / old menu / old help text
- [x] 删除旧 run-oriented reporting sink

### Phase 7 Checklist

- [x] formal archive 在新根下可重建
- [x] owner evidence 在新根下可重建
- [x] workspace 在新根下可重建
- [x] CLI/TUI/wiki/skill 不再引用旧路径
- [x] legacy path scan 通过
- [x] roadmap closeout 完成

