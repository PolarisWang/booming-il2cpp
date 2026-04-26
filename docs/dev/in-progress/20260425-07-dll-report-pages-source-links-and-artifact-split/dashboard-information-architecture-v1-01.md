# Dashboard Information Architecture v1.01

Date: 2026-04-26 00:30:00 +08:00
Status: draft
Phase: phase-3 (20260425-07)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 foundation DLL audit dashboard（dashboard.html）的正式信息架构，使其展示三轴进度并拆分 Artifact Index。

核心原则：
- **总报告看全局进度**，不看原始 artifact dump
- **三轴并行**：DLL Progress / Capability Closure / Workflow Progress 各有独立的可视区域
- **Artifact Index 退到次级入口**，不再占据 dashboard 主视觉

## 2. 页面布局

```
┌──────────────────────────────────────────────────────────┐
│ Header                                                    │
│  Eyebrow: "Verification Program"                         │
│  Title: "Foundation DLL Audit"                           │
│  Description: 一句话描述                                 │
│  Top Nav: [Program Overview] [DLL Matrix] [DLL Detail]   │
│  [Artifact Index] ← 移到最后，作为次级入口                │
├──────────────────────────────────────────────────────────┤
│ Section: Program Overview                                 │
│  ┌──────┬──────┬──────┬──────┬──────┬──────┐            │
│  │DLL   │      │      │      │      │Active│            │
│  │Count │...   │...   │...   │...   │Assembly           │
│  └──────┴──────┴──────┴──────┴──────┴──────┘            │
│                                                          │
│  ┌─ Progress Bars ─────────────────────────────────┐     │
│  │  DLL Completion:  [████░░░░░░] 0/14 (0%)        │     │
│  │  Capability Closure: [████████░░] 18/120 (15%)   │     │
│  │  Workflow Progress:  [██████░░░░] 45/360 (12.5%) │     │
│  └──────────────────────────────────────────────────┘     │
│                                                          │
│  ┌─ Authority Snapshot ────────────────────────────┐     │
│  │  Snapshot: snap-20260426-001                    │     │
│  │  Snapshot At: 2026-04-26T00:00:00+08:00         │     │
│  │  [View Ledger Authority Details]                │     │
│  └──────────────────────────────────────────────────┘     │
├──────────────────────────────────────────────────────────┤
│ Section: DLL Matrix                                       │
│  ┌─ Filter / Toggle ───────────────────────────────┐     │
│  │  [All] [In Progress] [Blocked] [Completed]      │     │
│  └──────────────────────────────────────────────────┘     │
│                                                          │
│  Table:                                                  │
│  ┌──────────┬──────┬──────────┬──────────┬───┬───┬───┐   │
│  │Assembly  │State │Closure   │Progress  │...│...│...│   │
│  ├──────────┼──────┼──────────┼──────────┼───┼───┼───┤   │
│  │CoreLib   │▶ IP  │████ 90%  │████ 75%  │...│...│...│   │
│  │Immutable │○ NS  │░░░░  0%  │░░░░  0%  │...│...│...│   │
│  │Interop   │○ NS  │░░░░  0%  │░░░░  0%  │...│...│...│   │
│  └──────────┴──────┴──────────┴──────────┴───┴───┴───┘   │
├──────────────────────────────────────────────────────────┤
│ Section: DLL Detail (Collapsible Cards)                   │
│  (保留现有 per-DLL cards，但移除重复的 artifact dump)     │
│  ┌─ System.Private.CoreLib ─────────────────────────┐     │
│  │  State: ● In Progress  │  Closure: 90%           │     │
│  │  Progress: 75%          │  Families: 18/20 closed │     │
│  │  [Detail Page] [JSON]                              │     │
│  │  (精简为 summary + key links，细节去 DLL 详情页)  │     │
│  └──────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────┘
```

## 3. 各区域详细设计

### 3.1 Header

与当前 header 一致，但：
- **Top Nav** 顺序改为：`[Program Overview] [DLL Matrix] [DLL Detail] [Artifact Index]`
- Artifact Index 排到最后，弱化为次级入口

### 3.2 Program Overview

**保留** 现有的 summary cards（DLL Count / Completed / In Progress / Blocked / Not Started / Active Assembly）。

**新增** 三轴进度条（见 §3.3）。

**新增** Authority Snapshot 卡片，显示当前 ledger snapshot 信息。

### 3.3 三轴进度条

三个进度条并列展示，视觉样式统一：

```
DLL Completion:     [████░░░░░░░░░░░░]  0/14  (0%)
Capability Closure: [████████████░░░░]  18/120 (15%)
Workflow Progress:  [██████████░░░░░░]  45/360 (12.5%)
```

进度条属性：

| 属性 | DLL Completion | Capability Closure | Workflow Progress |
|------|---------------|-------------------|-------------------|
| 分母 | 14（总 DLL 数） | totalFamilies | totalRequiredGates |
| 分子 | completedDllCount | closedFamilies | passedGates |
| 颜色 | 蓝色 | 绿色 | 橙色 |
| 单位 | DLL 数 | capability family 数 | gate 数 |

### 3.4 DLL Matrix

**保留** 现有的 gate 列（audit-input-and-ledger / managed-proof / ... / completion-certification）。

**新增** 两列作为双轴视图：
- `Closure` 列：显示 `closurePercent` + 微型进度条
- `Progress` 列：显示 `progressPercent` + 微型进度条

**新增** 筛选器：按 DLL state 过滤行（All / In Progress / Blocked / Completed）。

### 3.5 DLL Detail 卡片（精简版）

当前 dashboard 的 DLL Detail section 渲染了每个 DLL 的完整 project cards（含 artifacts、support refs、blockers）。这导致两个问题：
1. **Dashboard 过于冗长**：14 个 DLL × 7 projects = 大量 DOM
2. **与 DLL 详情页重复**：detail 页已经包含相同信息

Phase 3 精简方案：

```
┌─ System.Private.CoreLib ──────────────────────────────┐
│ ● In Progress  │  Closure: 90%  │  Progress: 75%     │
│ Families: 18/20 closed (1 waived, 1 excluded)         │
│ Current: completion-certification                     │
│ [Detail Page →] [JSON ↓] [Evidence] [Source Links]    │
│ ┌─ Waiver Summary ───────────────────────────────┐    │
│ │ ⚠ 1 active waiver  (expires 2026-07-25)       │    │
│ │ ⊘ 1 permanent exclusion                        │    │
│ └────────────────────────────────────────────────┘    │
└──────────────────────────────────────────────────────┘
```

变化：
- 不再展开 project cards（删除 `_render_project_card` 的 dashboard 调用）
- 精简为 2-3 行摘要 + 关键操作链接
- 新增 waiver summary
- 细节内容迁移到独立的 DLL detail page

### 3.6 Artifact Index（拆分到次级入口）

**当前**：Artifact Index 是 dashboard 的最后一个 section，包含一个巨大的 table（所有 DLL 的所有 artifact 行）。

**Phase 3 变更**：
- Dashboard 顶部 nav 保留 Artifact Index 链接
- Dashboard **不再直接渲染** Artifact Index table
- 新增独立页面 `artifact-index.html`，包含完整的 artifact table
- 保留 `artifact-index.json` 作为 machine-readable 数据

```
dashboard.html:
  - Artifact Index 导航链接 → artifact-index.html

artifact-index.html:
  - 独立的 Index 页面
  - 完整的 artifact table（与当前一致）
  - 筛选器（按 DLL、按 role、按 exists）
  - Back to Dashboard 链接
```

## 4. 视觉设计指引

### 4.1 三轴颜色方案

| 轴 | 用途 | 色值 |
|----|------|------|
| DLL Completion | 整体进度 | `--blue` (#24527a) |
| Capability Closure | 能力覆盖 | `--green` (#205b35) |
| Workflow Progress | 流程推进 | `--orange` (#8a4f14) |

### 4.2 微型进度条

DLL Matrix 中的微型进度条渲染规则：
- 宽度 80px，高度 8px
- 显示为 `<span class="mini-bar"><span class="mini-fill" style="width: 90%"></span></span>`
- `closurePercent >= 100` 时填充绿色
- `closurePercent >= 50` 时填充蓝色
- `closurePercent < 50` 时填充橙色
- hover 时显示 tooltip: "18/20 families closed"

### 4.3 Filter Bar

Filter bar 渲染规则：
- 四个按钮：`All` | `In Progress` | `Blocked` | `Completed`
- 默认选中 `All`
- 点击时通过 JavaScript 或 CSS class 隐藏/显示矩阵行
- 选中状态高亮

## 5. 页面加载策略

### 5.1 No-JS Fallback

Dashboard 是静态 HTML。筛选器和交互功能需要 JavaScript。但必须提供 No-JS fallback：

| 功能 | JS 启用 | JS 禁用 |
|------|---------|---------|
| DLL 筛选 | 点击按钮切换 | 全部显示（不筛选） |
| 进度条 tooltip | hover tooltip | 直接显示文字 "18/20" |
| 折叠/展开 | 点击折叠 | 全部展开 |
| Artifact Index | 独立的 artifact-index.html | 同左（纯静态页面） |

### 5.2 页面间导航

```
dashboard.html ──→ DLL detail pages (dlls/<assembly>.html)
         │
         └──→ artifact-index.html
```

所有页面间导航使用静态 `<a>` 链接，不依赖 JavaScript。

## 6. 与现有 dashboard 的差异总表

| 区域 | 当前 | Phase 3 |
|------|------|---------|
| Header Top Nav | 4 个链接 | Artifact Index 排到最后 |
| Program Overview | 只有 summary cards | + 三轴进度条 + Authority Snapshot |
| DLL Matrix | gate 列 | + Closure/Progress 列 + 筛选器 |
| DLL Detail | 完整 project cards | 精简摘要 + waiver summary |
| Artifact Index | dashboard 最后一个 section | 独立 artifact-index.html 页面 |

## 7. CSS 新增样式

在现有 dashboard.css 基础上新增：

```css
/* 三轴进度条 */
.progress-axis { display: flex; gap: 24px; flex-wrap: wrap; }
.progress-bar-container { flex: 1; min-width: 200px; }
.progress-bar { height: 20px; background: var(--pending-bg); border-radius: 999px; overflow: hidden; }
.progress-bar-fill { height: 100%; border-radius: 999px; transition: width 0.3s; }
.progress-bar-fill.dll-completion { background: #24527a; }
.progress-bar-fill.capability-closure { background: #205b35; }
.progress-bar-fill.workflow-progress { background: #8a4f14; }
.progress-label { display: flex; justify-content: space-between; margin-top: 4px; font-size: 12px; }

/* 微型进度条 */
.mini-bar { display: inline-block; width: 80px; height: 8px; background: var(--pending-bg); border-radius: 999px; overflow: hidden; vertical-align: middle; }
.mini-fill { height: 100%; border-radius: 999px; }
.mini-fill.high { background: #205b35; }
.mini-fill.medium { background: #24527a; }
.mini-fill.low { background: #8a4f14; }

/* Filter bar */
.filter-bar { display: flex; gap: 8px; margin-bottom: 12px; }
.filter-btn { padding: 6px 14px; border: 1px solid var(--line); background: var(--panel); cursor: pointer; }
.filter-btn.active { background: var(--accent); color: white; border-color: var(--accent); }

/* Waiver summary badge */
.waiver-badge { display: inline-flex; align-items: center; gap: 4px; padding: 2px 8px; border-radius: 999px; font-size: 11px; }
.waiver-badge.active { background: var(--warn-bg); color: var(--warn-ink); }
.waiver-badge.exclusion { background: var(--pending-bg); color: var(--pending-ink); }

/* Authority snapshot card */
.snapshot-card { border: 1px dashed var(--line); background: var(--panel); padding: 12px; font-size: 12px; }
```

## 8. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-26 | 1.0.0 | 初始版本 | Phase 3 初始创建 |
