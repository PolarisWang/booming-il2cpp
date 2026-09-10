# DLL Capability Verification Closure Roadmap v1

Date: 2026-04-27 04:25:00 +08:00
Updated: 2026-04-27 15:20:00 +08:00
Status: in-progress

## 1. 目标

建立一套完整的 DLL 翻译验证闭环体系，使每个进入翻译验证范围的 DLL 都具备：

- 完整的 capability / family / method authority
- 完整的功能、性能、热更验证闭环
- 可归档、可 review、可 solution 执行的生成结果
- 可被 dashboard 稳定消费的正式 projection

本路线图的最终完成标准不是“页面上有数据”，而是：

- Truth、Evidence、Execution、Projection 四层全部正式落地
- 新流程可以为新增 DLL 一次性生成完整 `case / project / bundle / projection skeleton`
- 旧流程和旧逻辑被清理干净
- 全部讨论内容已体现在 checklist 中
- 所有阶段都具备用户可直接验收的结果

## 2. 范围边界

本 roadmap 负责：

- 设计并实现 `Hybrid Layer` 四层对象模型与目录结构
- 为 DLL onboarding 生成：
  - `DllCapabilityManifest`
  - `CapabilityFamilyVerificationContract`
  - `MethodCapabilityContract`
  - generated `test/native/benchmark/host/patch` 项目
  - annotation source
  - formal case index
  - review bundle skeleton
  - aggregate / projection skeleton
- 实现 `A2 Source-Annotation-First`
- 实现单一正式 `.sln` 主入口及 family 局部执行加速入口
- 重构 dashboard 为 `DLL -> family -> method -> method tooltip`
- 完成旧逻辑清理、formal refresh、可 review 归档与用户验收

本 roadmap 不负责：

- 立即把所有 DLL 的所有 family 全量翻译完成
- 为所有 DLL 一次性补齐人工 review sign-off
- 扩展 foundation-dll scope 之外的其他业务线

## 3. 非目标

- 不允许只增加 dashboard 字段而不建立 formal truth
- 不允许只生成一批半成品测试工程而没有 solution 接线与执行链
- 不允许把 family 局部加速入口当成新的 formal 主入口
- 不允许引入新流程后保留旧逻辑双轨长期共存
- 不允许把“后续 follow-up 再补”作为阶段退出条件

## 4. 阶段列表

### Phase 0: Authority Freeze And Checklist Freeze

- goal:
  - 冻结四层对象模型、目录结构、annotation 规则、completed-before gate 与统一 checklist
- exit_criteria:
  - `Hybrid Layer` 的四层职责与边界已落成正式文档
  - A2 case generation authority 已冻结
  - 正式目录结构已冻结
  - `Review Bundle = G2 reviewable-bundle` 已写入正式 gate 规则
  - 全路线图 checklist 完整，无 follow-up 尾项
- deliverables:
  - `phase-0` authority 文档
  - 四层对象模型章节
  - 全量 checklist
- dependencies:
  - `20260427-03` brainstorm 已 cleared
- resolved_decisions:
  - `Hybrid Layer`
  - `A2 Source-Annotation-First`
  - method 级全映射
  - 单一正式 `.sln` 主入口
  - family 局部入口仅作开发加速入口
  - `Review Bundle` gate = `G2 reviewable-bundle`
- watch_items:
  - checklist 不允许遗漏旧逻辑清理与 formal refresh 验收项

### Phase 1: Truth Layer Buildout

- goal:
  - 建立 DLL / family / method authority，确保新增 DLL 进入 scope 时 capability 信息一次性冻结
- exit_criteria:
  - `DllCapabilityManifest` 正式落地
  - `CapabilityFamilyVerificationContract` 正式落地
  - `MethodCapabilityContract` 正式落地
  - 新增 DLL 时可自动生成 truth skeleton
  - 目录结构 `verification/foundation-dll/<assembly>/<family>/...` 可自动生成
- deliverables:
  - truth schema / generator
  - per-DLL capability manifest skeleton
  - per-family contract skeleton
  - per-method contract skeleton
- dependencies:
  - Phase 0
- resolved_decisions:
  - Truth 只定义 obligation，不执行、不聚合
- watch_items:
  - denominator authority 不能与现有 family ledger 漂移

### Phase 2: Evidence Layer Buildout

- goal:
  - 建立 method 级 case annotation、formal case index、review bundle 与功能/性能/热更证据对象
- exit_criteria:
  - generated source 具备：
    - `[MethodSubjectId]`
    - `[BenchmarkSubjectId]`
    - `[HotUpdateSubjectId]`
  - annotation scanner 只从 generated source / metadata 产出 formal index
  - 三份 formal index 落地：
    - `method-test-case-index.json`
    - `method-benchmark-case-index.json`
    - `method-hotupdate-case-index.json`
  - `ReviewBundle` 可生成且满足 G2 所需字段
  - 新增 DLL 时 evidence skeleton 同步生成
- deliverables:
  - annotation attribute / enum contract
  - scanner
  - formal indexes
  - review bundle skeleton / schema
- dependencies:
  - Phase 1
- resolved_decisions:
  - truth 不从运行结果反推
- watch_items:
  - scanner 输出必须可重复、稳定、可审计

### Phase 3: Execution Layer Buildout

- goal:
  - 生成完整测试工程、benchmark 工程、hotupdate host/patch 工程，并统一挂进正式主 solution
- exit_criteria:
  - `FoundationDllTranslationSolution.sln` 正式存在
  - generated family projects 自动接入：
    - `test/`
    - `native/`
    - `benchmark/`
    - `host/`
    - `patch/`
  - 支持按 DLL / family 局部执行
  - 生成结果可直接执行，不是半成品
- deliverables:
  - `.sln`
  - generated `.csproj`
  - family filter runner / command entry
  - execution contract
- dependencies:
  - Phase 2
- resolved_decisions:
  - 单一正式 `.sln` 主入口
  - family 局部入口只作开发加速
- watch_items:
  - solution 规模增长后仍需保证局部执行成本可控

### Phase 4: Projection Layer And Dashboard Cutover

- goal:
  - 重构 dashboard 与 projection，使之只消费四层 formal object
- exit_criteria:
  - 一级 dashboard 只显示 DLL aggregate
  - 二级详情页显示 family aggregate
  - 三级页面显示 method aggregate
  - tooltip 显示 method 测试详情
  - 最新 projection 可从 formal object 完整重建
- deliverables:
  - DLL aggregate projection
  - family aggregate projection
  - method aggregate projection
  - dashboard / detail / method page cutover
- dependencies:
  - Phase 3
- resolved_decisions:
  - dashboard 不再直接消费 run 层临时数据
- watch_items:
  - tooltip 只展示局部细节，不能承载主判断

### Phase 5: Legacy Retirement And Formal Verification Closeout

- goal:
  - 清理旧流程、旧结构、旧双轨逻辑，并完成 formal refresh 与用户验收
- exit_criteria:
  - 旧目录、旧 case 路径、旧推断逻辑全部清理
  - formal refresh 完整通过
  - 最新 generated result 已归档、可 review、可执行
  - checklist 全部勾完
  - 用户可以直接按验收步骤验证
- deliverables:
  - legacy cleanup batch
  - final review bundle
  - final formal projection refresh
  - user acceptance notes
- dependencies:
  - Phase 4
- resolved_decisions:
  - 不留 follow-up 尾项
- watch_items:
  - 避免遗漏旧入口导致 authority 分裂

## 5. 每阶段完成定义

### 5.1 用户可验收优先

每个阶段都必须给出用户可直接验证的结果，而不是只给内部代码状态。

### 5.2 统一退出标准

每个阶段只有在同时满足以下条件时才算完成：

- 本阶段 checklist 全勾完
- 本阶段 deliverables 全部存在
- 对应自动化验证通过
- 用户可执行验收步骤存在且可操作
- 没有“留到后续 follow-up”才能成立的条件

### 5.3 completed-before gate 统一约束

所有进入 DLL 翻译验证的 family，最终都必须满足：

- generated source snapshot 已归档
- case index 已生成
- latest execution results 已归档
- review bundle 已生成
- solution 可执行

## 6. Master Checklist

### 6.1 总体冻结

- [ ] `Hybrid Layer` 四层对象模型冻结
- [ ] 单一正式 `.sln` 主入口规则冻结
- [ ] family 局部入口“仅作开发加速”规则冻结
- [ ] `Review Bundle = G2 reviewable-bundle` 规则冻结
- [ ] 全量目录结构冻结
- [ ] annotation 枚举策略冻结

### 6.2 Truth Layer

- [ ] `DllCapabilityManifest` schema 落地
- [ ] `CapabilityFamilyVerificationContract` schema 落地
- [ ] `MethodCapabilityContract` schema 落地
- [ ] 新增 DLL 时 truth skeleton 自动生成
- [ ] capability inventory / family / method denominator 一致性校验存在

### 6.3 Evidence Layer

- [ ] `[MethodSubjectId]` annotation 落地
- [ ] `[BenchmarkSubjectId]` annotation 落地
- [ ] `[HotUpdateSubjectId]` annotation 落地
- [ ] annotation 尽量枚举化完成
- [ ] annotation scanner 落地
- [ ] `method-test-case-index.json` 落地
- [ ] `method-benchmark-case-index.json` 落地
- [ ] `method-hotupdate-case-index.json` 落地
- [ ] `ReviewBundle` schema / generator 落地
- [ ] 新增 DLL 时 evidence skeleton 自动生成

### 6.4 Execution Layer

- [ ] `FoundationDllTranslationSolution.sln` 落地
- [ ] family `test/` project 自动生成
- [ ] family `native/` project 自动生成
- [ ] family `benchmark/` project 自动生成
- [ ] family `host/` project 自动生成
- [ ] family `patch/` project 自动生成
- [ ] 新增 DLL 时 execution layer 工程直接生成完整成品
- [ ] 支持按 DLL 局部执行
- [ ] 支持按 family 局部执行
- [ ] 局部执行不依赖新的 formal 主入口

### 6.5 Projection Layer

- [ ] DLL aggregate projection 落地
- [ ] family aggregate projection 落地
- [ ] method aggregate projection 落地
- [ ] 一级 dashboard 只显示 DLL
- [ ] 二级详情页只显示 family
- [ ] 三级页面显示 method
- [ ] tooltip 显示 method 测试详情
- [ ] projection 只消费 formal object，不从 runs 反推 truth

### 6.6 Legacy Cleanup

- [ ] 旧双轨 case 逻辑识别完成
- [ ] 旧目录 / 旧入口 / 旧推断逻辑清理完成
- [ ] 旧 annotation / 旧 scanner / 旧 projection consumer 清理完成
- [ ] 旧逻辑 grep 复查通过
- [ ] 第二轮 grep 复查通过
- [ ] 第三轮人工 review 确认无残留

### 6.7 Formal Verification

- [ ] Truth Layer formal validation 通过
- [ ] Evidence Layer formal validation 通过
- [ ] Execution Layer solution build/test 通过
- [ ] Projection Layer regression 通过
- [ ] `run verify verification-v1 --json` 通过
- [ ] latest/master/reports 刷新成功
- [ ] generated result 已归档
- [ ] review bundle 可 review
- [ ] solution 可执行

### 6.8 用户验收

- [ ] 用户可查看新增 DLL 的全部 capability / family / method 信息
- [ ] 用户可查看每个 family 的功能 / 性能 / 热更闭环进度
- [ ] 用户可点入 method 页面查看测试详情
- [ ] 用户可在 tooltip 查看 method 测试详情
- [ ] 用户可在仓库中 review 最新生成结果
- [ ] 用户可通过正式 `.sln` 执行
- [ ] 用户可按 family 局部执行验证
- [ ] 用户确认无 follow-up 尾项

## 7. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260427-05-dll-capability-verification-phase-0-authority-freeze` | `phase-0` | `completed` | `codex` | 已冻结四层对象模型、目录结构、gate 与全量 checklist，并将 authority 写入正式文档 | - |
| `20260427-06-dll-capability-verification-phase-1-truth-layer` | `phase-1` | `in-progress` | `codex` | 落地 Truth Layer authority 与新增 DLL truth skeleton | `20260427-05-dll-capability-verification-phase-0-authority-freeze` |
| `20260427-07-dll-capability-verification-phase-2-evidence-layer` | `phase-2` | `in-progress` | `codex` | 落地 A2 annotation、scanner、formal case index 与 review bundle | `20260427-06-dll-capability-verification-phase-1-truth-layer` |
| `20260427-08-dll-capability-verification-phase-3-execution-layer` | `phase-3` | `ready` | `codex` | 生成完整测试/benchmark/hotupdate 工程与正式 `.sln`，支持局部执行 | `20260427-07-dll-capability-verification-phase-2-evidence-layer` |
| `20260427-09-dll-capability-verification-phase-4-projection-layer` | `phase-4` | `planned` | `codex` | 重构 projection 与 dashboard 为 DLL/family/method 三层导航 | `20260427-08-dll-capability-verification-phase-3-execution-layer` |
| `20260427-10-dll-capability-verification-phase-5-closeout` | `phase-5` | `planned` | `codex` | 清理旧逻辑、formal refresh、归档 review bundle 与用户验收 | `20260427-09-dll-capability-verification-phase-4-projection-layer` |

## 8. 依赖

- `docs/dev/completed/20260427-03-dll-capability-verification-closure-brainstorm/brainstorm-v1-01.md`
- `docs/architecture/verification-v1/spec.md`
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
- `docs/dev/in-progress/20260419-01-foundation-dll-translation-audit-roadmap/STATUS.md`
- 当前 foundation-dll projection / audit generator / family verification 链路

## 9. 风险

- method 级 case 全映射会显著增加 generated project 规模
- annotation scanner 是关键设施，若 authority 边界不严会污染 truth
- 目录与 solution 结构大改时，容易留下旧双轨逻辑残留
- 如果用户验收步骤没有写清楚，阶段完成会重新退回“内部自证”

## 10. 备选路径

### 路径 A：只改 dashboard，不改四层闭环

- 不推荐
- 原因：无法满足你对闭环、review、solution 执行、旧逻辑清理的要求

### 路径 B：采用弱 authority，后续再补 method 级

- 不推荐
- 原因：你已经明确要求一步到位 method 全映射

### 路径 C：采用当前 roadmap

- 推荐
- 原因：它把 authority、case、solution、review、projection、cleanup 和用户验收统一到一条可验证主线中

## 11. 当前建议推进顺序

1. 先完成 Phase 0，把所有标准、目录、gate、checklist 冻住
2. 再完成 Truth Layer，建立 capability / family / method authority
3. 再完成 Evidence Layer，落地 A2 annotation + scanner + indexes + review bundle
4. 再完成 Execution Layer，直接生成完整工程和主 `.sln`
5. 再做 Projection Layer 重构
6. 最后做旧逻辑清理与 formal closeout

## 12. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child`: `20260427-08-dll-capability-verification-phase-3-execution-layer`
