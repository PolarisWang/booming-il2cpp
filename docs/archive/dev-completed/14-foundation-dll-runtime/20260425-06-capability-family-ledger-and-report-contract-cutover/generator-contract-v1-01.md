# Generator Contract v1.01

Date: 2026-04-26 00:00:00 +08:00
Status: draft
Phase: phase-2 (20260425-06)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 `foundation_dll_audit_generator.py` 与数据源、输出产物之间的正式合约。

核心原则：
- **报告 authority 由 projection payload 明示**，不是由 HTML 模板隐式推断
- **capability family ledger 是双轴数据的唯一来源**，generator 不应自行推导 family 闭包状态
- **generator 是数据编排器，不是数据生产者**——所有 authoritative 数据必须来自输入 artifact

## 2. 输入合约

### 2.1 输入来源

| # | 输入 | 路径 | 用途 | Authority 层级 |
|---|------|------|------|---------------|
| 1 | Program Manifest | `verification/catalog/programs/foundation-dll-translation-audit.program.json` | DLL 列表、project template 定义、policy 配置 | program authority |
| 2 | Capability-Family Ledger | `verification/projections/foundation-dll-audit/capability-family-ledger.json` | 每个 DLL 的 semantic family 定义、closure 状态、implementation family、source group | 报告 authority |
| 3 | Roadmap STATUS | `docs/dev/in-progress/20260425-03-.../STATUS.md` | 任务状态、active child | workflow authority |
| 4 | Design doc | `docs/dev/in-progress/20260419-01-.../design-v1-01.md` | Verification project 模板、状态模型 | design authority |
| 5 | ACTIVE.md | `docs/dev/ACTIVE.md` | 当前 active task ID | workflow authority |
| 6 | Task STATUS files | `docs/dev/{in-progress,completed}/*/STATUS.md` | Evidence path 提取、blocker 信息 | evidence source |
| 7 | Audit artifacts | `artifacts/subjects/SolutionCorePack/runs/*/` | Evidence 的存在性检查 | evidence source |

### 2.2 新增核心输入：Capability-Family Ledger

这是 Phase 2 新增的核心输入。Ledger 文件的预期位置：

```
verification/projections/foundation-dll-audit/capability-family-ledger.json
```

Ledger 文件由**外部工具**维护（不是由 generator 生成）。Generator 只读取它。

Ledger 的最小结构：

```json
{
  "schemaVersion": "1.0.0",
  "ledgerId": "foundation-dll-capability-family-v1",
  "authoritySnapshotOf": "2026-04-25T22:00:00+08:00",
  "originalAuthorityDigest": {
    "inputManifestVersion": "sha256:<hash>",
    "surfaceLedgerVersion": "sha256:<hash>",
    "semanticLedgerVersion": "sha256:<hash>",
    "nativeizationPlanVersion": "sha256:<hash>"
  },
  "dlls": [
    {
      "assemblyName": "System.Private.CoreLib",
      "families": [
        {
          "familyId": "family/System.Private.CoreLib/convert/to-char",
          "displayName": "Convert.ToChar",
          "closureStatus": "closed",
          "closureEvidence": { ... },
          "methodCount": 18,
          "verificationGates": { ... },
          "waiverRecords": [],
          "implementationFamilies": [ ... ],
          "sourceGroups": [ ... ]
        }
      ],
      "sourceLinks": { ... }
    }
  ]
}
```

### 2.3 Ledger 不存在时的降级行为

当 `capability-family-ledger.json` 不存在时：
- `program.json` 不输出 `capabilityClosure` / `workflowProgress` / `authoritySnapshot` 字段（向后兼容）
- `dll-matrix.json` 不输出行级 `capabilityClosure` / `workflowProgress`
- `dlls/<assembly>.json` 不输出 `capabilityFamilies[]` / `sourceLinks` / `waiverSummary`
- Generator 正常输出其他已有字段，不抛异常

当 ledger 存在但某个 DLL 条目缺失时：
- 该 DLL 的 `capabilityFamilies` = `[]`
- `capabilityClosure.totalFamilies` = `0`
- 不阻塞其他 DLL 的处理

## 3. 处理合约

### 3.1 数据流

```
Program manifest ─┐
                   ├──→ DLL 列表、project templates、policy
                   │
Capability ledger ─┤
                   ├──→ Family 数据、closure 状态、gate 状态
                   │
Roadmap status ────┤
                   ├──→ DLL state、active task、blocker
                   │
Task STATUS files ─┤
                   ├──→ Evidence paths、support refs
                   │
ACTIVE.md ─────────┘
                   │
                   ↓
          Generator (数据编排)
                   │
                   ↓
          Projection bundle:
            program.json           ← 含双轴进度
            dll-matrix.json        ← 含双轴列
            dlls/<assembly>.json   ← 含 capabilityFamilies[]
            artifact-index.json    ← 不变
            dashboard.html         ← 渲染双轴
            dashboard.css          ← 不变
            summary.md             ← 含双轴摘要
```

### 3.2 核心处理步骤

1. **加载输入**:
   - 读取 program manifest
   - 尝试读取 capability-family ledger（可选）
   - 解析 roadmap STATUS.md
   - 提取 evidence paths

2. **计算 Capability Closure**:
   - 从 ledger 读取每个 DLL 的 families[]
   - 统计 `totalFamilies`、`closedFamilies`、`waivedFamilies` 等
   - 计算 `closurePercent = closedFamilies / totalFamilies * 100`

3. **计算 Workflow Progress**:
   - 从每个 family 的 `verificationGates` 读取 gate 状态
   - 统计 `totalRequiredGates`（`policyState != "not-required"` 的 gate 数）
   - 统计 `passedGates`（`executionState = "passed"` 的 gate 数）
   - 计算 `progressPercent = passedGates / totalRequiredGates * 100`

4. **计算 DLL Completion**:
   - 检查所有 family 的 `closureStatus` 是否满足 §3 的 6 条规则
   - 设置 `completion-certification` gate 的 executionState

5. **填充 projection payload**:
   - 按 schema 定义填充 program / matrix / DLL detail
   - 保留已有字段确保向后兼容

### 3.3 状态推导逻辑（无 ledger 时的回退）

当 ledger 不存在时，双轴数据不被推导。Generator 不猜测 family 划分。

## 4. 输出合约

### 4.1 输出文件清单

| # | 文件 | Schema 版本 | 用途 | 变更类型 |
|---|------|-------------|------|----------|
| 1 | `program.json` | v2 | Program 级总览 | 新增双轴字段 |
| 2 | `dll-matrix.json` | v2 | DLL Matrix | 新增双轴列 |
| 3 | `dlls/<assembly>.json` | v2 | DLL 详情 | 新增 capabilityFamilies[] |
| 4 | `artifact-index.json` | v1 | Artifact Index | 不变 |
| 5 | `dashboard.html` | — | 可视化报告 | 需渲染双轴 |
| 6 | `dashboard.css` | — | 样式 | 不变 |
| 7 | `summary.md` | — | 摘要 | 含双轴摘要 |

### 4.2 输出位置

```
verification/projections/foundation-dll-audit/     ← 主要输出（供工具使用）
  program.json
  dll-matrix.json
  artifact-index.json
  dlls/<assembly>.json
  dashboard.html
  dashboard.css
  summary.md

docs/verification/foundation-dll-audit/             ← 发布副本（供审核者浏览）
  (同上文件)

archive/report/completed/foundation-dll-audit/      ← 归档副本（formal archive）
  summary.md
```

### 4.3 输出约束

- 所有 JSON 文件必须是 valid JSON（不允许 trailing comma、comment）
- 所有路径使用反斜杠 `/`，不使用 `\`
- 每次生成必须原子写入（先写临时文件，再 rename）
- `schemaVersion` 从 `1` 升级到 `2`（新增双轴数据）
- `schemaVersion` 不升级当只有 evidence 变更时（gate 状态变更不升级 schema）

## 5. 与现有 generator 的关系

### 5.1 兼容性约束

| 约束 | 说明 |
|------|------|
| 向后兼容 | 现有 consumer（dashboard.html）必须能在无 ledger 时正常工作 |
| 增量升级 | 不要求一次性完成所有变更，允许 ledger 逐步填充 |
| 字段保留 | 不删除已有字段，只新增 |

### 5.2 现有 generator 的改动范围

| 函数 | 改动 | 风险 |
|------|------|------|
| `build_foundation_dll_audit_payload` | 新增 ledger 加载、双轴计算逻辑 | 中 |
| `_build_projects` | 新增 family-level gate 状态注入 | 低 |
| `_render_dashboard` | 新增双轴进度条渲染 | 中 |
| `_render_dll_detail_page` | 新增 capability families 表格、source links | 中 |
| `_summary_markdown` | 新增双轴摘要行 | 低 |
| 其他函数 | 不变 | 无 |

### 5.3 不改变的内容

以下逻辑保持不变：
- Evidence path 提取（`_extract_path_references`）
- Artifact record 构建（`_artifact_record`）
- DLL state 推导（`_dll_state`）
- Artifact index 构建
- Dashboard CSS 样式

## 6. Generator 与 Ledger 的数据一致性校验

Generator 必须在输出前执行以下校验：

| 校验 | 规则 | 失败处理 |
|------|------|----------|
| DLL 列表一致性 | Ledger 中的 DLL 列表与 program manifest 一致 | 记录 warning，以 program manifest 为准 |
| family ID 唯一性 | 同一 DLL 内的 family ID 不重复 | 记录 error，跳过重复 entry |
| gate 状态合法性 | gate 值必须是有效 executionState | 记录 warning，置为 `pending` |
| closure 证据存在性 | `closed` family 的 evidence 路径必须 `exists=true` | 记录 warning，不修改 closure 状态 |
| Total 计数一致性 | `totalFamilies = sum(closed+waived+...+inProgress)` | 记录 error，修正为实际值 |

## 7. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-26 | 1.0.0 | 初始版本 | Phase 2 初始创建 |
