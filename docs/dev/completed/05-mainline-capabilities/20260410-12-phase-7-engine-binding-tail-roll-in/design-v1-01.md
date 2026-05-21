# Phase 7 Engine Binding Tail Roll In 设计 v1.01

Date: 2026-04-10 00:45:00 +08:00

## 1. 目标

把 `engine binding` 正式接入现有 mainline：

- 作为独立 contract 边界进入 analysis -> generated -> runtime -> report
- 以三个最小 engine subject 形成可执行验证入口
- 保持 Windows-only 首轮收口，不提前引入移动平台与新平行系统

## 2. 结构

### 2.1 Subject 结构

新增三个 subject：

- `subjects/EngineLogWriteLite`
- `subjects/EngineObjectHandleLite`
- `subjects/EngineLifecycleCallbackLite`

三者共用规则：

- `category = canonical`
- `defaultGoal = correctness.dev`
- `defaultMatrix = windows-dev-output`
- 首轮只定义 `windows-dev-output` 与 `windows-reference-trace`
- `subject.manifest.json` 新增轻量字段 `engineProofProfile`

### 2.2 Engine Contract 结构

在 `LoweringPlan` 中新增两层：

- `EngineBindings`
- `HostBindings`

职责边界：

- `SubjectRoles`：managed facade 的 type/method 身份
- `RuntimeRoles`：通用 runtime/helper/icall
- `EngineBindings`：engine-facing semantic contract
- `HostBindings`：宿主 / 平台约束与落地方式

### 2.3 Evidence / Report 结构

保持既有文件名与 bucket 不变，只增量增加 engine summary：

- `subject.manifest.json.engineProofProfile`
- `analysis.manifest.json.engineContractSummary`
- `generated.manifest.json.engineEmissionSummary`
- `runtime.manifest.json.engineObservationSummary`
- `report.json.engineProofSummary`

这些 summary 只服务 reporting / diagnostics，不得反向成为 planner 或 executor 的执行真源。

### 2.4 Stage 结构

首轮允许最小 engine-aware stage kind：

- `generated-engine-proof`
- `runtime-engine-observe`
- `runtime-engine-trace-compare`

其他 bucket 继续复用：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `build-target`
- `report-assemble`

## 3. 具体能力范围

### 3.1 EngineLogWriteLite

- focusArea: `service-call`
- 最小能力：
  - `engine.log.write`
- evidence：
  - log message output

### 3.2 EngineObjectHandleLite

- focusArea: `object-handle`
- 最小能力：
  - `engine.object.handle.create`
  - `engine.object.handle.resolve`
- evidence：
  - handle roundtrip
  - identity roundtrip

### 3.3 EngineLifecycleCallbackLite

- focusArea: `lifecycle-callback`
- 最小能力：
  - `engine.lifecycle.callback.register`
  - `engine.lifecycle.dispatch`
  - `engine.thread.main-lane`
- evidence：
  - callback order
  - thread lane

## 4. 关键约束

- 不允许让 `LinkedWorld.Dependencies` 承载 engine binding 语义。
- 不允许让 executor / planner 读取 engine summary 来决定调度行为。
- 不允许为 engine binding 新造 `engine-*` 平行 artifact bucket。
- 不提前把 Android / iOS buildable/runtime subset 拉入本阶段。

## 5. 验证目标

- unit：
  - manifest / planner / reporting / worker / command 的 engine surface 冻结
  - analysis/codegen/runtime 的 engine binding contract 冻结
- end-to-end：
  - 三个 subject 的 `correctness.dev`
  - 三个 subject 的 `correctness.platform`

## 6. 非目标

- 完整引擎 SDK 自动生成
- 移动平台 buildable / runtime subset
- 将 engine binding 并回 `MainlineFeaturePack`
- 把当前路线表述为完整 `C# -> C++ total solution`
