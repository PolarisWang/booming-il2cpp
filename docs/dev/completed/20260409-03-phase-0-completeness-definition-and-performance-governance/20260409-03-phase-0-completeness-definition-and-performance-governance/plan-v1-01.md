# Phase 0 完整性定义与性能治理实施计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。
>
> **目标：** 冻结 `DefinitionOfComplete` 与 `PerformanceGovernance` 两份正式文档，解除 `20260409-02-phase-1-optimization-facts-and-native-perf-contracts` 的前置歧义。
>
> **架构：** 先写两份冻结文档，再同步父 roadmap / status / index 与 `20260409-02` child 状态，最后做一致性校验。
>
> **技术栈：** Markdown、`docs/dev` workflow、roadmap/status/index maintenance
>
> **设计文档：** `docs/dev/in-progress/20260409-03-phase-0-completeness-definition-and-performance-governance/design-v1-01.md`
>
> **预期知识沉淀：** 暂不写 wiki；待 `Phase 1/2/3` 稳定后，再统一沉淀到 architecture/wiki 页面。

---

## 目标文件与职责

- `docs/dev/in-progress/20260409-03-phase-0-completeness-definition-and-performance-governance/definition-of-complete-v1-01.md`
  - 冻结当前主线的语义完整度、平台完整度、工程完整度
- `docs/dev/in-progress/20260409-03-phase-0-completeness-definition-and-performance-governance/performance-governance-v1-01.md`
  - 冻结性能目标函数、证据链、冲突裁决与默认优先级
- `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/roadmap-v1-02.md`
  - 把 `Phase 0` placeholder 替换成正式 child task id，并同步状态
- `docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/STATUS.md`
  - 记录 `Phase 0` child 已派生
- `docs/dev/in-progress/20260409-02-phase-1-optimization-facts-and-native-perf-contracts/STATUS.md`
  - 从“等待父 roadmap 重规划”切换到“等待 `Phase 0` 完成”
- `docs/dev/ACTIVE.md`
  - 切换当前活动任务到 `Phase 0`
- `docs/dev/INDEX.md`
  - 同步当前 active task 与任务摘要
- `docs/dev/in-progress/INDEX.md`
  - 同步当前 active task 与任务摘要

## Chunk 1：任务初始化

- [ ] 创建 `Phase 0` 子任务目录与 `STATUS.md`
- [ ] 写入 child `brainstorm-v1-01.md`、`design-v1-01.md`、`plan-v1-01.md`
- [ ] 切换 `docs/dev/ACTIVE.md` 与 in-progress index 指向当前 child

## Chunk 2：冻结核心文档

- [ ] 写 `definition-of-complete-v1-01.md`
- [ ] 写 `performance-governance-v1-01.md`
- [ ] 把 `exceptions` 强制交付、`engine binding` 最后收口、Windows-only 主线收口写死

## Chunk 3：同步主线状态

- [ ] 回写父 roadmap 的 `Phase 0` child task id 与状态
- [ ] 更新父 `STATUS.md` 最近摘要与下一步
- [ ] 更新 `20260409-02` child `STATUS.md` 的依赖表述

## Chunk 4：一致性校验

- [ ] 检查 `roadmap-v1-02.md`、父 `STATUS.md`、child `STATUS.md`、`ACTIVE.md`、两份 index 是否都指向同一条主线
- [ ] 检查 `Phase 0` 文档中对 complete/perf governance 的默认拍板是否一致
- [ ] 记录进度到 `notes/progress-v1-01.md`

## 验证命令

- [ ] `rg --files docs/dev/in-progress/20260409-03-phase-0-completeness-definition-and-performance-governance`
- [ ] `Select-String -Path 'docs/dev/in-progress/20260409-01-il2cpp-mainline-performance-core-and-full-subject-roadmap/roadmap-v1-02.md' -Pattern '20260409-03-phase-0-completeness-definition-and-performance-governance|Phase 0'`
- [ ] `Select-String -Path 'docs/dev/ACTIVE.md' -Pattern '20260409-03-phase-0-completeness-definition-and-performance-governance'`

## 完成标准

- [ ] `definition-of-complete-v1-01.md` 已存在且冻结当前主线的 complete 定义
- [ ] `performance-governance-v1-01.md` 已存在且冻结性能治理优先级与证据链
- [ ] 父 roadmap 已引用正式 `Phase 0` child task id
- [ ] `20260409-02` 已明确转为等待 `Phase 0` 完成，而不是等待泛化的“父 roadmap 重规划”
- [ ] `docs/dev/ACTIVE.md`、`docs/dev/INDEX.md`、`docs/dev/in-progress/INDEX.md` 一致
