---
task_id: 20260424-01-c3-verification-topology-and-unified-entry
title: C3 Verification Topology And Unified Entry
task_type: roadmap
lifecycle_status: in-progress
phase: roadmap
created_at: 2026-04-24 00:00:00 +08:00
updated_at: 2026-04-24 01:30:00 +08:00
latest_stop_point: roadmap-v1-01.md 已创建；subjects/tests/wiki/skill 的最终迁移与 hard cut 清单已冻结，任务已从 brainstorming 正式推进到 roadmap
current_dir: docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry
parent_task_id:
source_task_id: 20260423-01-verification-v1-roadmap
source_relation: follow-up architecture roadmap after verification-v1 completion
design_doc: docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry/design-v1-01.md
roadmap_or_plan: docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: 20260424-02-c3-phase-0-contract-freeze-and-layout-skeleton
active: true
preflight_review: pass
preflight_summary: blocking questions 已清零；design、subject cutover mapping 与 roadmap 已对齐，下一步可直接进入 Phase 0 contract freeze child
auto_execution_decision: continue
---

# 20260424-01 C3 Verification Topology And Unified Entry

## Roadmap Update 2026-04-24

- 当前状态: `in-progress`
- 当前阶段: `roadmap`
- roadmap: `docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry/roadmap-v1-01.md`
- recommended_next_child: `20260424-02-c3-phase-0-contract-freeze-and-layout-skeleton`

## Current Conclusion 2026-04-24

- 方案已冻结为 `Full Authority Migration`
- cutover 模式已冻结为 `one-shot hard cut`
- `verification/` 将成为唯一正式 verification root
- `run.py` 继续是唯一真实执行入口
- `verification-all.sln` 只是 IDE facade，不是执行 authority
- `subjects/*` 不再作为长期测试 authority root
- verification 专属自动化测试统一迁到 `tests/verification/*`
- wiki、skill、CLI、TUI、workspace、projection、archive 必须同批切到新根

## Key Documents 2026-04-24

- `docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry/design-v1-01.md`
- `docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry/subject-cutover-mapping-v1-01.md`
- `docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry/roadmap-v1-01.md`
- `docs/architecture/verification-v1/spec.md`

## Historical Snapshot Notice

- 本文件前半部分与 frontmatter 是当前有效状态。
- 下方旧的 brainstorming 记录仅作历史保留，恢复任务时忽略。

## New Decisions 2026-04-24

- total `.sln` remains a human-facing / IDE-facing facade only
- `build/toolchains/run/run.py` remains the only real execution entry
- the total `.sln` uses a stable generated path, not a hand-maintained static file
- the generated total `.sln` and its workspace manifest must live under the new unified verification root
- roadmap direction is `Full Authority Migration` with one-shot `hard cut`
- no dual-write, no shadow path, no legacy public path retention after cutover
- `subjects/` is no longer treated as the long-term test-system authority root; its test-bearing content becomes migration input and must exit the mainline by hard cut completion

## Question Clearance 2026-04-24

- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true
- design_ready: true
- next_downstream: roadmap

## Key Documents

- `docs/dev/in-progress/20260424-01-c3-verification-topology-and-unified-entry/design-v1-01.md`
- `docs/architecture/verification-v1/spec.md`
- `docs/dev/completed/20260423-01-verification-v1-roadmap/STATUS.md`

## 当前状态

- 状态: `in-progress`
- 当前阶段: `brainstorming`
- 当前目标: 把 C3 测试结构讨论正式收束成可执行的开发计划输入

## 当前结论

- 当前 `verification-v1` 的 formal archive authority 已稳定到：
  - `docs/testing-inventory/verification/*`
  - `subjects/<OwnerPack>/verification/codegen-stubs/*`
- 当前长期统一命令入口是 `build/toolchains/run/run.py`，并且已经提供：
  - `run generate project all`
  - `run generate project subject`
  - `run test *`
  - `run verify *`
- 当前仓库缺少一个稳定、长期、可直接作为“全项目可视入口”的正式 `.sln`；现有 `.sln` 主要在临时或生成产物中
- 进入 C3 后，测试结构希望进一步纯化为正交轴，而不是继续让 `subject` 同时承载 owner / scenario / route / host / platform responsibilities
- 后续每个 open question 都必须先和 `verification-v1` 一起重审合理性、可行性与 authority 冲突，再对用户发问
- 已拍板：
  - 总 `.sln` 只做人类可见与 IDE facade
  - `build/toolchains/run/run.py` 继续是唯一真实执行入口
- 当前冲突的根因不是“目录名不统一”，而是三类职责仍然混在一起：
  - formal archive authority
  - owner-local verification evidence
  - human navigation / IDE workspace entry
- 基于 `verification-v1` 的更干净方向，应优先做三层分离，而不是直接物理迁移 formal archive

## 候选方案

### 方案 A: Overlay Gateway

- 保留 `verification-v1` formal authority 不变：
  - `docs/testing-inventory/verification/*`
  - `subjects/<OwnerPack>/verification/codegen-stubs/*`
- 新增一个统一的人类入口根，例如 `verification/`
- `verification/` 只放：
  - `INDEX.md`
  - `verification.manifest.json`
  - `workspaces/verification-all.sln`
  - `owners/<subject-id>.json`
  - 指向 formal archive / owner stub / run command 的聚合索引
- `run.py` 继续是真实执行入口；总 `.sln` 只是 IDE facade

### 方案 B: Docs-First Hub

- 继续把 `docs/testing-inventory/` 当作总入口
- 在 `docs/testing-inventory/` 下新增：
  - `INDEX.md`
  - `verification.manifest.json`
  - `workspaces/verification-all.sln`
  - `owners/`
- formal archive 与人类入口都留在 `docs/` 下
- owner codegen stubs 仍留在 `subjects/*/verification/`

### 方案 C: Full Authority Migration

- 新建顶层 `verification/` 作为新的 formal authority 根
- 迁移：
  - `docs/testing-inventory/verification/* -> verification/archive/*`
  - `subjects/*/verification/codegen-stubs/* -> verification/owners/*/codegen-stubs/*`
- 同时把总 `.sln`、reports、projection、index 全部收进 `verification/`
- `run.py` 继续为真实入口，但 formal path 全部切换

## 当前推荐

- 用户已拍板: `方案 C`
- 影响:
  - 本任务不再以“保留 verification-v1 formal path 不变”为前提
  - 本任务必须显式设计 `verification-v1` authority migration 的 cutover、兼容期和回收策略
  - 后续开发计划默认按 multi-phase roadmap 组织，而不是单次 plan
  - 用户已进一步拍板:
    - 采用一次性 `hard cut`
    - 不保留旧路径
    - 旧路径在 cutover 完成后要被清理干净

## hard cut 联审

- review_mode: `critical`
- 结论: `warning-but-acceptable`
- 与 verification-v1 的关系:
  - 这是一次明确的 authority rewrite，不再是 verification-v1 内部的小修
  - 必须把 `docs/testing-inventory/verification/*` 与 `subjects/*/verification/*` 的旧 authority 一次性迁走并回收
- 可行性:
  - 可行，但必须保证生成器、CLI、TUI、wiki、tests、索引、workspace 入口在同一批次一起切换
  - 不能接受长期 alias、shadow path、dual-write 或回落到旧 formal path
- cutover 约束:
  - 新 authority 根必须一次性完整可用
  - 旧 authority 根在 hard cut 后必须全部退出 public/mainline
  - 若 owner-local compare 需求仍保留，需要在新 authority 根内提供 owner 分区，而不是继续保留旧 `subjects/*/verification/*`

## 边界拍板

- 本任务只讨论并冻结以下内容：
  - C3 架构下验证目录的正式收束方案
  - “人类可见 + 可自运行”的统一入口方案
  - 这些决策如何进入后续 design / roadmap
- 本任务暂不直接改动实现代码、runner、generator、manifest 或 wiki
- 本任务暂不重写已有 `verification-v1` formal archive，只定义下一步迁移目标和切换路径

## Authority 决策

- formal verification archive authority:
  - `docs/testing-inventory/verification/*`
- owner-side codegen stub authority:
  - `subjects/<OwnerPack>/verification/codegen-stubs/*`
- current unified command authority:
  - `build/toolchains/run/run.py`
  - `build/toolchains/run/run_manifest.json`
  - `build/toolchains/run/subject/project_workspace.py`
- upstream reference roadmap:
  - `docs/dev/completed/20260423-01-verification-v1-roadmap/STATUS.md`
  - `docs/architecture/verification-v1/spec.md`

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `verification-topology-and-unified-entry`
- capabilityItem: `c3-architecture-brainstorm`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `false`
- benchmarkRequired: `false`
- hotupdateImpact: `None`
- formalVerificationObjects:
  - `docs/testing-inventory/verification/*`
  - `subjects/*/verification/codegen-stubs/*`
- requiredGates:
  - `collector`
  - `registry`
  - `workspace`

## Blocking Questions

- C3 下“验证目录的统一入口”应以哪一层为正式锚点：
  - 已拍板为 authority migration；下一步只剩新根内部如何分层
- “人类可见 + 可自运行”的统一入口，最终应以哪种对象为主：
  - 已拍板：CLI/TUI 为唯一真实入口，`.sln` 仅做人类浏览和 IDE workspace
- C3 是否允许保留“生成型入口”，还是要求保留一个稳定、常驻、提交到仓库的总入口

## Watch Items

- 如果总入口过度绑定到 `.sln`，route lane / platform-device lab 的非 .NET 资产可能再次被迫回流到 subject 或 host 层
- 如果 formal archive 与人类入口完全分离，认知路径可能仍然分散，用户会继续在 `docs/`、`subjects/`、`solutions/`、`run.py` 之间跳转
- 如果 C3 为了“纯净”牺牲自运行体验，迁移后会降低人工排障和 IDE 调试效率

## verification-v1 预审

- review_mode: `critical`
- 初步结论: `warning`
- 合理性:
  - C3 把 `subject / scenario / route / platform-device` 做正交分层，和 `verification-v1` 的 `Authority Graph / Claim Graph / Execution Facts / Projection` 总体方向一致
  - C3 想把验证目录做统一入口，也符合 `verification-v1` 对 formal archive 可读性和聚合入口的目标
- 可行性:
  - C3 可行，但不能直接把现有 `docs/testing-inventory/verification/*` formal hub 废掉，否则会与已完成的 `verification-v1` authority 冲突
  - C3 可把“统一入口”做在 formal hub 之上，或做成覆盖 formal hub 的更高层入口，但不能绕开 formal source 自行定义另一套 archive authority
  - “总 .sln” 作为人类入口可行，但更适合作为 workspace / IDE facade，而不是 formal verification authority 或唯一真实执行入口
- 主要冲突点:
  - 已关闭：总 `.sln` 不再争论为唯一主入口，避免与当前 `run.py` 的统一命令 authority 冲突
  - 若把验证目录统一到新的根，同时迁走 formal archive，会与 `verification-v1` 已批准的 formal path 冲突，除非后续显式开新任务修改 authority
  - 若把 route/platform 资产强制压回 `.sln`，会损害 C3 追求的正交结构
- 重审规则:
  - 每个 open question 发出前，必须先写明：
    - 与 `verification-v1` 是否一致
    - 是否需要改动 `verification-v1` authority
    - 若不改 authority，推荐的可行落位是什么

## 问题清理状态

- blocking_questions: non-empty
- question_clearance: pending
- clearance_confirmed_by_user: false

## 下一步

- 按 brainstorming 流程先清理 blocking questions
- blocking questions 清零后再写 `design-v1-01.md`
- 设计拍板后进入正式开发计划文档
