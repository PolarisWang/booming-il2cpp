# Reporting Authority Sync And Formal Refresh v1.01

Date: 2026-04-26 00:45:00 +08:00
Status: draft
Phase: phase-4 (20260425-08)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

Phase 0-3 定义了 foundation DLL 报告的能力闭包治理全貌。治理规则的 authority 文档（`spec.md`）和知识入口（`INDEX.md`）尚未同步。本阶段的目标：

1. 把 capability-family-ledger、dual-axis 进度模型、waiver/exclusion/blocked 规则、source links 渲染规则同步到 `docs/archive/architecture/verification-v1/spec.md`
2. 在 `wiki/06-测试验证/INDEX.md` 创建新口径的正式条目
3. 定义 verification-v1 formal refresh 的执行计划
4. 定义 docs/projection/archive 三层产物一致性的校验规则

## 2. spec.md 更新方案

### 2.1 §6 Projection 模型 — 新增 foundation-dll-audit 投影

在 §6.5「下游派生视图」的表格末尾追加以下条目：

| 文件/页面 | 角色 | 推荐粒度 |
| --- | --- | --- |
| `verification/projections/foundation-dll-audit/program.json` | foundation DLL 报告投影入口 | 三轴进度（DLL Completion / Capability Closure / Workflow Progress）+ DLL Matrix |
| `verification/projections/foundation-dll-audit/dll-matrix.json` | DLL 矩阵投影 | 每 DLL 一行，含 gate 列 + Closure/Progress 双轴列 |
| `verification/projections/foundation-dll-audit/dlls/<assembly>.json` | DLL 详情投影 | 每 DLL 一个，含 capability families、waiver records、source links |
| `docs/verification/foundation-dll-audit/dashboard.html` | 人工浏览入口（HTML） | 从 projection JSON 派生 |
| `docs/verification/foundation-dll-audit/dlls/<assembly>.html` | DLL 详情浏览入口（HTML） | 从 projection JSON 派生 |
| `docs/verification/foundation-dll-audit/artifact-index.html` | artifact 索引（HTML，独立次级入口） | 每 artifact 一行 |

同时，在 §6.5 之前增加一段说明 §6.5.1「Capability-Family Ledger 投影」：

```
### 6.5.1 Capability-Family Ledger 投影

foundation-dll-audit 投影系列是 evidence-driven closure 投影的一个实例，遵循以下权威链：

1. **分母 authority**：`capability-family ledger`（由 audit input manifest + surface ledger + semantic ledger + nativeization plan 合成）
2. **进度 authority**：`workflow gate contract`（family 级 gate 的 policyState 和 executionState 决定 passedGates / requiredGates）
3. **闭包 authority**：`DLL completed contract`（6 条硬规则：family 全 closed、无 in-progress、所有 required gates passed、completion-certification passed、waiver authority 有效、evidence 全部 exists）
4. **豁免 authority**：`waiver contract`（waiver / exclusion / platform-blocked 三类，必须带正式 authorityRef，不允许引用 task STATUS.md）

该投影的 schemaVersion 当前为 2，采用 CapabilityClosure + WorkflowProgress 双轴字段结构。
```

### 2.2 §15 上传与清理 — 增加 capability-family-ledger 刷新触发规则

在 §15.1 列举的 formal report / projection contract 改动触发条件中，在现有四条后面追加第五条：

```
- `capability-family ledger` 的分母条目或 family 状态
```

同时，在 §15.1 末尾增加一段注释：

```
foundation-dll-audit 投影的 refresh 顺序：

1. 确认 capability-family-ledger.json 是最新冻结快照
2. 读取所有 audit input / surface ledger / semantic ledger 的最新版本
3. 生成 program.json（含三轴进度）
4. 生成 dll-matrix.json（含 gate 列 + Closure/Progress 列）
5. 生成 dlls/<assembly>.json（含 capability families、waiver records、source links）
6. 从 projection JSON 派生 HTML 页面（dashboard.html + DLL detail pages + artifact-index.html）
7. 提交正式产物并更新 archive

其中 step 6 由 generator 在 projection JSON 就绪后自动执行。
```

这一改动可以直接在最终落地编辑 spec.md 时应用。

## 3. INDEX.md 同步方案

在 `wiki/06-测试验证/INDEX.md` 的「最近更新」区域追加以下三条：

```
- `2026-04-26`：新增 Capability Closure 双轴报告治理框架；capability-family ledger 成为 foundation DLL 进度报告的正式分母 authority；completion-certification 改为 DLL 级 gate，不再混入 family workflow denominator；waiver / exclusion / platform-blocked 三类豁免必须带正式 authorityRef，不再允许引用 task STATUS.md。
- `2026-04-26`：新增 foundation-dll-audit 投影系列的 authority 分层；`verification/projections/foundation-dll-audit/*` 成为正式 projection contract，HTML 报告从 projection JSON 派生；source links 五类（Subject / Verification / Generated / Evidence / Authority）的渲染规则固定。
- `2026-04-26`：新增 DLL 详情页的信息架构规范；DLL 页头显示双轴进度条 + source links，正文显示 capability families 表格 + waiver records 表格 + verification projects 精简摘要。Dashboard 页头显示三轴进度条（DLL Completion / Capability Closure / Workflow Progress），DLL Matrix 新增 Closure/Progress 列和筛选器。Artifact Index 拆分到独立 artifact-index.html 次级入口。
```

在「正式主线」区域的第 21 行之后追加：

```
- foundation DLL 详情页入口：[`docs/verification/foundation-dll-audit/dlls/<assembly>.html`](docs/verification/foundation-dll-audit/dlls/<assembly>.html)
- artifact 索引入口：[`docs/verification/foundation-dll-audit/artifact-index.html`](docs/verification/foundation-dll-audit/artifact-index.html)
- capability-family ledger 是 foundation DLL 报告的正式分母 authority。
- workflow progress 是辅助流程条，不代表 capability 覆盖本身。
- waiver / exclusion / platform-blocked 三类豁免必须带正式 authorityRef 且记录在 capability-family-ledger.json 中，不允许只写在 task STATUS.md。
- DLL 详情页中的 source links 遵循 `subject-source / verification-source / generated-code / evidence / authority-docs` 五种类型、优先级顺序和路径截断规则。

## 4. Formal Refresh 计划

verification-v1 formal refresh 需要在 generator 实现 Phase 2-3 的 projection 和 HTML 变化之后执行。以下步骤文档化当前已知的正式 refresh 计划。

### 4.1 前置条件

- [ ] generator 已支持从 capability-family-ledger.json 读取数据
- [ ] generator 已输出 schemaVersion=2 的 projection JSON（含 CapabilityClosure / WorkflowProgress 字段）
- [ ] generator 已输出三轴进度条的 dashboard.html
- [ ] generator 已输出带 capability families、waiver records、source links 的 DLL detail 页
- [ ] generator 已拆分 artifact-index.html 为独立页面
- [ ] capability-family-ledger.json 已正式就位（包含所有 14 个 DLL 的 family 定义）

### 4.2 Refresh 执行步骤

```
1. 冻结 capability-family-ledger.json 为当前 snapshot
   → 记录 snapshotId + snapshotAt 到 program.json 的 authoritySnapshot 字段

2. 运行 generator：
   python build/toolchains/run/testing/foundation_dll_audit_generator.py

3. 校验输出完整性：
   - verification/projections/foundation-dll-audit/program.json 存在
   - verification/projections/foundation-dll-audit/dll-matrix.json 存在
   - verification/projections/foundation-dll-audit/dlls/*.json 存在（每 DLL 一个）
   - docs/verification/foundation-dll-audit/dashboard.html 存在
   - docs/verification/foundation-dll-audit/dlls/*.html 存在
   - docs/verification/foundation-dll-audit/artifact-index.html 存在

4. 运行三层一致性校验（见 §5）

5. 提交正式产物：
   - git add verification/projections/foundation-dll-audit/
   - git add docs/verification/foundation-dll-audit/
   - git commit -m "chore: refresh foundation-dll-audit projections and reports"
```

### 4.3 Refresh 触发条件

以下情况必须触发 formal refresh：

| 触发条件 | 影响范围 | 紧急程度 |
|----------|----------|----------|
| capability-family-ledger 更新（新增/删除/修改 family） | program.json, dll-matrix.json, 受影响 DLL 的 HTML | 高 |
| 某个 gate 的 executionState 变化 | 对应 DLL 的 capabilities, progress, dashboard | 高 |
| waiver 新增/到期/更新 | 对应 DLL 的 waiver records, closure status | 高 |
| evidence 路径变化或新增 | 对应 DLL 的 source links, evidence count | 中 |
| 仅 HTML 样式/排版调整 | HTML 文件 | 低（可不触发 JSON 刷新） |

### 4.4 增量刷新 vs 全量刷新

- **全量刷新**：generator 必须处理全部 14 个 DLL。适用于 ledger 版本升级、schema version 变化或首次创建。
- **增量刷新**：允许 generator 只重刷变更的 DLL 的 JSON 和 HTML。需要在 toolchain 层面支持按 DLL 名称过滤。

## 5. 三层产物一致性校验规则

docs / projection / archive 三层产物必须保持一致性。定义以下校验规则。

### 5.1 三层定义

| 层 | 路径 | 角色 | 更新者 |
|----|------|------|--------|
| Archive | `verification/archive/latest/` | formal evidence 原始层 | 验证 pipeline |
| Projection | `verification/projections/foundation-dll-audit/*` | 结构化数据层 | generator |
| Docs | `docs/verification/foundation-dll-audit/*` | 人工浏览层 | generator（从 projection 派生）|

### 5.2 一致性校验矩阵

| 校验项 | 方法 | 失败含义 |
|--------|------|----------|
| projection JSON 与 archive 中的 evidence path 一致 | 对比 evidence `path` 在 archive 中存在 | HTML 中的 evidence 链接可能断裂 |
| HTML 中的 capability family 数量与 JSON 一致 | 对比 HTML 渲染的 family 行数与 JSON 的 `capabilityFamilies[]` 长度 | HTML 渲染逻辑遗漏了某些 family |
| HTML 中的 closurePercent 与 JSON 一致 | 对比 HTML 进度条宽度和文本与 JSON 的 `closurePercent` | 进度条渲染公式不匹配 |
| HTML 中的 waiver 记录与 JSON 一致 | 对比 HTML waiver 行数与 JSON 的 `waiverRecords[]` 长度 | waiver 渲染遗漏 |
| projection JSON 的 schemaVersion 与 docs 架构规格一致 | `schemaVersion` 字段值与 spec.md 定义的版本号一致 | schema 版本未同步 |
| 所有 source link 的目标 path 存在 | 检查 subjectSource / verificationSource 等的 path 在文件系统中存在 | source link 断裂 |

### 5.3 校验时机

- **generator 每次执行后**：自动校验 1-3 项（快速校验）
- **formal refresh 提交前**：手动或脚本校验 1-6 项（完整校验）
- **Schema 版本变更后**：校验第 5 项（版本一致性）

### 5.4 校验脚本接口

建议在 `build/toolchains/run/testing/` 下新增校验脚本 `verify_projection_consistency.py`，接口：

```
python build/toolchains/run/testing/verify_projection_consistency.py \
  --projection-dir verification/projections/foundation-dll-audit \
  --docs-dir docs/verification/foundation-dll-audit \
  --archive-dir verification/archive/latest \
  --mode quick|full
```

- `--mode quick`：只校验 1-3 项（generator 后自动调用）
- `--mode full`：校验 1-6 项（提交前手动调用）

## 6. 具体编辑操作指导

### 6.1 spec.md 编辑

在 `docs/archive/architecture/verification-v1/spec.md` 中：

1. **§6.5 表格末尾追加**：6 行 foundation-dll-audit 投影条目（见 §2.1）
2. **§6.5 之前插入**：§6.5.1 新节（见 §2.1）
3. **§15.1 列举末尾追加**：第 5 条触发条件（见 §2.2）
4. **§15.1 末尾追加**：foundation-dll-audit refresh 顺序注释（见 §2.2）

### 6.2 INDEX.md 编辑

在 `wiki/06-测试验证/INDEX.md` 中：

1. **「正式主线」区域**：在 foundation DLL 总览页条目后追加 3 行新入口
2. **「正式主线」区域末尾**：追加 6 行本层规则
3. **「最近更新」区域**：追加 3 条 2026-04-26 记录

## 7. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-26 | 1.0.0 | 初始版本 | Phase 4 初始创建 |

