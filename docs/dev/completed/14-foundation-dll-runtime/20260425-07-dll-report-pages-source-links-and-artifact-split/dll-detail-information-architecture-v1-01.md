# DLL Detail Information Architecture v1.01

Date: 2026-04-26 00:30:00 +08:00
Status: draft
Phase: phase-3 (20260425-07)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 DLL 详情页（`dlls/<assembly>.html`）的正式信息架构，使其显示 capability matrix、source links、evidence、waivers。

核心原则：
- **DLL 页看 capability 细节和源码入口**，而不是重复 dashboard 的 project gate 视图
- **审核者可以在 DLL 页回答**："这个 DLL 的哪些能力已闭包？哪些 gate 通过了？证据在哪？源码在哪？"
- **Waiver / exclusion / platform-blocked 必须可见**，不允许隐藏在 JSON 中

## 2. 页面布局

```
┌──────────────────────────────────────────────────────────┐
│ Header                                                    │
│  Back Link: ← Back to Dashboard                          │
│  Eyebrow: "DLL Detail"                                   │
│  Title: "System.Private.CoreLib"                         │
│  Description: 一句话描述                                 │
│                                                          │
│  ┌─ Summary Cards ──────────────────────────────────┐    │
│  │ State │ Current  │ Phase │ Families │ Evidence   │    │
│  │ ● IP  │ comp-cer │ ph-1  │ 18/20    │ 42         │    │
│  └───────┴──────────┴───────┴──────────┴────────────┘    │
│                                                          │
│  ┌─ Dual Axis Cards ───────────────────────────────┐     │
│  │ Capability Closure: 18/20 (90%)                 │     │
│  │ Workflow Progress:  45/60 (75%)                 │     │
│  └──────────────────────────────────────────────────┘     │
│                                                          │
│  ┌─ Source Links ──────────────────────────────────┐     │
│  │ [Subject Source] [Verification] [Generated Code] │     │
│  │ [Evidence] [Authority Docs]                      │     │
│  └──────────────────────────────────────────────────┘     │
├──────────────────────────────────────────────────────────┤
│ Section: Capability Families                              │
│  ┌─ Family Table ───────────────────────────────────┐     │
│  │ Family ID    │ Status   │ Gates  │ Evidence  Src │     │
│  ├──────────────┼──────────┼────────┼───────────────┤     │
│  │ convert/     │ ● closed │ 5/5    │ [coverage]    │     │
│  │   to-char    │          │ passed │ [proof]       │     │
│  ├──────────────┼──────────┼────────┼───────────────┤     │
│  │ convert/     │ ● closed │ 5/5    │ [coverage]    │     │
│  │   to-string  │          │ passed │ [proof]       │     │
│  ├──────────────┼──────────┼────────┼───────────────┤     │
│  │ enum/        │ ○ IP     │ 2/5    │ [coverage]    │     │
│  │   has-flag   │          │ passed │               │     │
│  ├──────────────┼──────────┼────────┼───────────────┤     │
│  │ buffer/      │ ⚠ waived │ 3/5    │ —             │     │
│  │   memory-op  │ 2026-07  │ passed │               │     │
│  └──────────────┴──────────┴────────┴───────────────┘     │
│                                                          │
│  (每行可展开/跳转到 family 详情)                           │
├──────────────────────────────────────────────────────────┤
│ Section: Waiver Records                                   │
│  ┌─ Waiver Table ──────────────────────────────────┐     │
│  │ ID    │ Type │ Family   │ Expires   │ Authority  │     │
│  ├───────┼──────┼──────────┼───────────┼────────────┤     │
│  │ W-001 │ ⚠    │ buffer/  │ 2026-07   │ [nativei-  │     │
│  │       │ waiver│ mem-op   │ -25       │ zation-    │     │
│  │       │      │          │           │ plan#...]  │     │
│  ├───────┼──────┼──────────┼───────────┼────────────┤     │
│  │ E-001 │ ⊘    │ convert/ │ permanent │ [surface-  │     │
│  │       │ excl │ to-char  │           │ ledger#..] │     │
│  └───────┴──────┴──────────┴───────────┴────────────┘     │
├──────────────────────────────────────────────────────────┤
│ Section: Verification Projects (精简)                     │
│  ┌─ Project Cards (精简版) ─────────────────────────┐     │
│  │ 只显示 executionState 和 policyState，evidence    │     │
│  │ 链接到 path，不再展开完整 artifact list            │     │
│  └──────────────────────────────────────────────────┘     │
├──────────────────────────────────────────────────────────┤
│ Section: Blockers                                         │
│  （当有 blocker 时显示，无 blocker 时隐藏）               │
└──────────────────────────────────────────────────────────┘
```

## 3. 各区域详细设计

### 3.1 Header

**保留** 当前 header 元素：
- Back Link: `← Back to Dashboard`
- Updated summary cards:
  - State badge
  - Current Project
  - Phase
  - Roadmap Task ID
  - `Families: 18/20 closed`（新增）
  - Evidence count（保留现有）

**新增** Dual Axis Cards：
```
Capability Closure:  [████████████████░░] 18/20 (90%)
Workflow Progress:   [██████████████░░░░] 45/60 (75%)
```

**新增** Source Links（见 §4 / source-links-rendering-rules）。

### 3.2 Capability Families 表格

这是 DLL 详情页的核心新增内容。每行是一个 semantic family：

| 列 | 内容 | 格式 |
|----|------|------|
| Family | `familyId` 的短名称 + 描述 tooltip | 链接到 JSON 锚点 |
| Status | closureStatus + 豁免图标 | badge + waiver tooltip |
| Gates | `passedGates / totalRequiredGates` | 微型进度条 + 数字 |
| Evidence | 证据文件链接 | `coverage` / `proof` / `review` 三个图标按钮 |
| Source | source group 链接 | 跳转到源码目录 |

**Family 表头展开**：
- 点击 family 行时展开显示：
  - implementation families 列表（handler 名 + 方法数）
  - source groups 列表
  - waiver records（如果有）

### 3.3 Waiver Records 表格

| 列 | 内容 | 格式 |
|----|------|------|
| ID | waiverId | `W-001` 链接到 authorityRef |
| Type | waiver / exclusion / platform-blocked | 图标 + 文字 |
| Family | familyId 短名称 | 链接到 family |
| Scope | 影响范围 | `partial` / `full` + 方法数摘要 |
| Expires/Status | waiver 到期时间或 exclusion 永久标记 | 日期或 "permanent" |
| Authority | authorityRef 跳转 | 外部链接 |

**空状态**：当 waiverRecords 为空时，显示 "No waivers or exclusions for this DLL." 并隐藏该 section。

### 3.4 Verification Projects（精简版）

当前 DLL detail 页的 projects section 渲染了完整的 project cards（含 artifacts 列表、support refs、blockers）。

Phase 3 精简为：
- 每个 project 一行 table：| Project | Policy | State | Evidence Count | Support Refs |
- evidence 点击跳转到 path
- 不再渲染 `_render_project_card` 的完整 HTML（在 DLL 详情页中重复太多了）

### 3.5 Blockers

只有当 `blockers` 非空时显示。每个 blocker：
- Title + description
- 类型标签（technical / authority / dependency）
- 受影响的 family 列表
- Resolution plan（如果有）

## 4. 页面布局约束

### 4.1 响应式设计

| 断点 | 布局 | 行为 |
|------|------|------|
| >1200px | 三列布局：family table + waiver + projects 并排 | 充分利用宽屏 |
| 768-1200px | 两列布局 | family table 独占一行 |
| <768px | 单列布局 | 全部纵向排列 |

### 4.2 加载顺序

```
1. Header + Summary Cards（首屏可见）
2. Dual Axis Progress（首屏可见）
3. Source Links（首屏可见）
4. Capability Families Table（核心内容）
5. Waiver Records（如果存在）
6. Verification Projects（精简版）
7. Blockers（如果存在）
```

### 4.3 导航锚点

每个 section 设置 `id` 属性，允许 URL 直接跳转：
- `#capability-families`
- `#waivers`
- `#projects`
- `#blockers`

## 5. 与当前 DLL detail 页的差异

| 区域 | 当前 | Phase 3 |
|------|------|---------|
| Header | State / Current / Phase / Task | + Families count + Dual Axis + Source Links |
| Source Links | 不存在 | 新增 source links section |
| Capability Families | 不存在 | 新增 family table（核心新增） |
| Waiver Records | 不存在 | 新增 waiver table |
| Verification Projects | 完整 project cards + artifact 列表 | 精简为 summary table + 路径链接 |
| Blockers | 在 Blocking Context section 中 | 保持，放在页面底部 |

## 6. 与 JSON payload 的对应关系

DLL Detail 页的每个可视化元素必须直接对应 JSON payload 中的一个字段：

| 页面元素 | JSON payload 字段 |
|----------|-------------------|
| Dual Axis Progress | `capabilityClosure.closurePercent`, `workflowProgress.progressPercent` |
| Family Table 行 | `capabilityFamilies[]` |
| Family Status badge | `capabilityFamilies[].closureStatus` |
| Family Gates | `capabilityFamilies[].verificationGates` |
| Family Evidence | `capabilityFamilies[].closureEvidence` |
| Waiver Table 行 | `capabilityFamilies[].waiverRecords[]` |
| Source Links | `sourceLinks.*` |
| Verification Projects | `projects[]` |

## 7. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-26 | 1.0.0 | 初始版本 | Phase 3 初始创建 |
