---
task_id: 20260425-07-dll-report-pages-source-links-and-artifact-split
title: DLL Report Pages Source Links And Artifact Split
task_type: plan
lifecycle_status: completed
phase: phase-3
created_at: 2026-04-26 00:30:00 +08:00
updated_at: 2026-04-27 02:20:00 +08:00
current_dir: docs/dev/completed/20260425-07-dll-report-pages-source-links-and-artifact-split
parent_task_id: 20260425-03-foundation-dll-capability-closure-reporting-roadmap
parent_task_phase: phase-3
parent_task_relation: child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
active: false
---

# 20260425-07 DLL Report Pages Source Links And Artifact Split

## 目标

重构 HTML 报告信息架构，使总报告看全局进度，DLL 页看 capability 细节和源码入口。

## 退出条件

- [x] 总报告同时显示 `DLL Progress / Capability Closure / Workflow Progress`
- [x] DLL 页显示 capability matrix、source links、evidence、waivers
- [x] Artifact Index 从总 dashboard 主路径拆分到独立页面或次级入口

## 交付物

1. `dashboard-information-architecture-v1-01.md` — 总报告页面的信息架构
2. `dll-detail-information-architecture-v1-01.md` — DLL 详情页的信息架构
3. `source-links-rendering-rules-v1-01.md` — Source Links 渲染规则

## 依赖

- Phase 2 完成的 3 个文档（projection schema、generator contract、payload examples）
- `build/toolchains/run/testing/foundation_dll_audit_generator.py`（当前的 HTML 渲染实现）
- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/design-v1-01.md`（Artifact 规则）

## 当前状态

- 三个设计文档均已完成草稿（draft 状态）：
  - `dashboard-information-architecture-v1-01.md` — 定义三轴进度条（DLL Completion / Capability Closure / Workflow Progress），DLL Matrix 新增 Closure/Progress 列和筛选器，DLL Detail 卡片精简为摘要，Artifact Index 拆分到独立 `artifact-index.html`
  - `dll-detail-information-architecture-v1-01.md` — 定义 DLL 详情页的七大区域布局（Header → Dual Axis → Source Links → Capability Families 表格 → Waiver Records → Verification Projects 精简版 → Blockers），支持响应式三档断点
  - `source-links-rendering-rules-v1-01.md` — 定义五种 source link 类型（Subject/Generated/Evidence/Verification/Authority）的图标、渲染规则、路径截断规则、存在性检查、family 级方法链接展开

## 验证

- `dashboard-information-architecture-v1-01.md` 经用户 review 确认
- `dll-detail-information-architecture-v1-01.md` 经用户 review 确认
- `source-links-rendering-rules-v1-01.md` 经用户 review 确认
