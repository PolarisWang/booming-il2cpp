# Phase 0 Capability Taxonomy And Archetype Freeze 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。步骤使用复选框语法跟踪。

**目标：** 冻结本轮 roadmap 的 level-2 capability taxonomy、canonical solution archetype 清单、retained subject 目标结构与关键命名/依赖边界。

**架构：** 本阶段不改生产代码，只输出冻结文档和执行边界。输出将直接作为 `Phase 1` 到 `Phase 6` 的唯一 vocabulary 与结构输入。

**技术栈：** Markdown、YAML 状态文件、PowerShell、仓库目录盘点。

**设计文档：** `docs/dev/in-progress/20260414-01-il2cpp-hotupdate-capability-closure/design-v1-01.md`

**父 roadmap：** `docs/dev/in-progress/20260414-01-il2cpp-hotupdate-capability-closure/roadmap-v1-01.md`

**预期知识沉淀：** 先沉淀在本子任务目录；阶段完成后，仅把稳定 contract 抽入 `docs/architecture/`，过程性迁移信息继续留在 dev task archive。

---

## 计划任务

- [x] Task 1: 建立 Phase 0 child task 执行骨架并切换 `docs/dev/ACTIVE.md`
  - 目标文件：
    - `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/STATUS.md`
    - `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/plan-v1-01.md`
    - `docs/dev/ACTIVE.md`
    - `docs/dev/in-progress/INDEX.md`
  - 验收：
    - `ACTIVE.md` 指向本 child task
    - `in-progress/INDEX.md` 可见 parent roadmap 与本 child task

- [x] Task 2: 输出 capability taxonomy freeze
  - 目标文件：
    - `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/capability-taxonomy-freeze-v1-01.md`
  - 内容要求：
    - 冻结 level-1 / level-2 capability vocabulary
    - 明确历史语法点“单文件，但尽量合在同一项目”的编排规则
    - 明确每个 capability family 的 subject owner
    - 冻结 support-state vocabulary：`supported / partial / unsupported / not-applicable`
  - 验收：
    - 任意一个历史 proof/benchmark 文件都能找到 level-2 capability 归属

- [x] Task 3: 输出 solution archetype freeze
  - 目标文件：
    - `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/solution-archetype-freeze-v1-01.md`
  - 内容要求：
    - 冻结 canonical archetype 清单
    - 明确 owner subject、最小 graph、依赖模型与当前 seed
    - 固化 `repository-owned reference bundle` 作为 `CoreLibReferenceSolution` 基础
  - 验收：
    - 任意一个完整 solution 验证目标都能映射到一个 archetype

- [x] Task 4: 输出 retained subject target structure freeze
  - 目标文件：
    - `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/subject-target-structure-freeze-v1-01.md`
  - 内容要求：
    - 冻结三类 retained subject 的目标目录结构
    - 冻结现有目录到目标结构的迁移映射
    - 明确不再扩大 string 内部调度面
  - 验收：
    - 任意一个新增能力点都能回答“放在哪个目录、哪个项目、哪个 archetype”

- [x] Task 5: 同步 parent roadmap / STATUS，并准备切到 Phase 1
  - 目标文件：
    - `docs/dev/in-progress/20260414-01-il2cpp-hotupdate-capability-closure/roadmap-v1-01.md`
    - `docs/dev/in-progress/20260414-01-il2cpp-hotupdate-capability-closure/STATUS.md`
    - `docs/dev/in-progress/20260414-02-phase-0-capability-taxonomy-and-archetype-freeze/notes/progress-v1-01.md`
  - 验收：
    - parent roadmap 的 `Q0-Q8` 冻结与 Phase 0 输出一致
    - parent STATUS 明确下一步切到 Phase 1

## 验证

- 文档交叉检查：
  - `design-v1-01.md`
  - `roadmap-v1-01.md`
  - 本阶段三份 freeze 文档
- 状态检查：
  - `docs/dev/ACTIVE.md`
  - parent `STATUS.md`
  - child `STATUS.md`

## 执行备注

- 本阶段不修改生产代码。
- capability taxonomy 先冻结 vocabulary 与归属，不在本阶段追求实现覆盖率。
- archetype 名称要以真实 graph 为准，不为了“好听”而牺牲准确性。
