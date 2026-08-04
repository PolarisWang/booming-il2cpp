# Family Verification Kernel Hard Cut Roadmap v1

Date: 2026-04-27 00:30:00 +08:00
Status: completed

## 1. 目标

执行 foundation-dll family verification kernel 的 hard cut。

这次 roadmap 必须一次把下面所有事情列清，并在完成时逐条回看确认：

- 引入新的 family verification kernel 对象模型
- 把 `testCode + nativeProof` 完整切到新链路
- 把 `managedProof / hotupdateProof / benchmark` 也纳入新内核迁移范围
- 完成新的 formal refresh / projection / dashboard 链路
- 清理所有旧逻辑，直到确认干净
- 做全面验证
- 保证 dashboard 上有真实数据；如果没有，要继续定位和修复
- 用 checklist 管理所有工作项、验证项、清理项、review 项

## 2. 范围边界

本 roadmap 负责：

- family verification kernel hard cut 的设计落实
- `verify verification-v1` 的 foundation-dll family verification 正式切换
- 新 snapshot、projection、dashboard 和 detail 的联动
- 旧逻辑清理与复查

本 roadmap 不负责：

- 把所有 `methodCount = 0` 的 family 一并补成新 authority
- 重写整个 verification-v1 的非 foundation-dll 领域

## 3. 非目标

- 不保留长期双轨
- 不允许“新字段已存在，但旧逻辑仍是主逻辑”
- 不允许“dashboard 有 UI，但没有真实数据”
- 不允许“旧逻辑大概率已删”这种口头完成态

## 4. 阶段列表

### Phase 0: Authority Freeze And Cutover Checklist Freeze

- goal:
  - 冻结 hard cut 所有 authority、schema、run 选择、清理标准和总 checklist
- exit_criteria:
  - `VerificationFact / Claim / Evaluator / Snapshot` 边界写入正式设计
  - family `methodSubjectIds` authority 规则冻结
  - 最新有效 native proof source 选择规则冻结
  - 总 checklist 成为正式执行准则
- deliverables:
  - `design-v1-01.md`
  - 本 roadmap
  - master checklist
- dependencies:
  - `20260426-01` 已完成的 family test-code-first 主线
- resolved_decisions:
  - 采用 hard cut，不做长期双轨
  - 完成时必须反复确认旧逻辑清理干净
- watch_items:
  - methodSubjectIds authority 仍有局部硬编码风险

### Phase 1: Kernel Introduction

- goal:
  - 引入 `VerificationFact / Claim / Evaluator / Snapshot`
- exit_criteria:
  - 新 kernel 对象落地到正式代码
  - foundation-dll 现有数据可以被新 kernel 表达
- deliverables:
  - fact builder
  - claim builder
  - evaluator interfaces
  - snapshot schema / builder
- dependencies:
  - Phase 0
- resolved_decisions:
  - generator 不再承担业务判断
- watch_items:
  - 不允许只建对象壳，不接真实数据

### Phase 2: NativeProof Hard Cut

- goal:
  - 把 family 级 `nativeProof` 完整切到新内核
- exit_criteria:
  - native proof facts 只由新 kernel 采集
  - family native proof progress/detail 只由 evaluator 产出
  - dashboard/detail 只消费 snapshot
- deliverables:
  - native proof facts
  - native proof claims
  - native proof evaluator
  - updated projection payload
- dependencies:
  - Phase 1
- resolved_decisions:
  - 不允许 generator 继续直接解析 coverage artifact
- watch_items:
  - 当前已存在的 family nativeProof 临时字段要替换，不是并存

### Phase 3: TestCode Hard Cut

- goal:
  - 把 `testCode` 从 gap analyzer 局部状态升级为 kernel snapshot 正式对象
- exit_criteria:
  - testCode facts / claims / snapshot 全部走新链路
  - `analyze-gaps` 只负责事实收集和 authority write-back，不再直接决定展示模型
- deliverables:
  - testCode evaluator
  - snapshot testCode section
  - updated analyze-gaps integration
- dependencies:
  - Phase 2
- resolved_decisions:
  - `testCode` 与 `nativeProof` 必须能在同一 family snapshot 中协同展示
- watch_items:
  - write-back authority 和 snapshot 生成顺序不能打架

### Phase 4: Managed/HotUpdate/Benchmark Expansion

- goal:
  - 把剩余 family gate 全部并入同一验证内核
- exit_criteria:
  - `managedProof`
  - `hotupdateProof`
  - `benchmark`
  都进入 snapshot，并在 dashboard/detail 有数据位
- deliverables:
  - 3 个 evaluator
  - projection schema expansion
  - dashboard/detail expansion
- dependencies:
  - Phase 3
- resolved_decisions:
  - 一旦进入新 snapshot，就不再允许单独走 generator 私有逻辑
- watch_items:
  - benchmark/hotupdate 的事实源可能与 native proof 不同

### Phase 5: Projection Hard Cut And Dashboard Verification

- goal:
  - 让 `program.json / dll-matrix.json / dlls/*.json / html` 全部只读新 snapshot
- exit_criteria:
  - dashboard 上有真实 family verification 数据
  - tooltip/detail 不是空壳
  - 不再依赖旧 generator 内联计算
- deliverables:
  - new projection readers
  - dashboard render cutover
  - dll detail cutover
- dependencies:
  - Phase 4
- resolved_decisions:
  - dashboard 无数据时，任务不能 completed
- watch_items:
  - 页面存在但全是默认值，视为失败

### Phase 6: Legacy Retirement, Full Verification, And Review Closeout

- goal:
  - 清理所有旧逻辑并做全面验证与最终 review
- exit_criteria:
  - 旧逻辑 grep / review 清零
  - checklist 全勾选
  - design / roadmap / implementation / dashboard 全部一致
- deliverables:
  - legacy cleanup commit batch
  - full verification evidence
  - final review notes
- dependencies:
  - Phase 5
- resolved_decisions:
  - completed 前必须 review roadmap 全部任务
- watch_items:
  - 任何一处旧逻辑残留都不能放过

## 5. Master Checklist

### 5.1 范围与设计

- [x] `VerificationFact` 结构定义完成
- [x] `VerificationClaim` 结构定义完成
- [x] `VerificationEvaluator` 结构定义完成
- [x] `VerificationSnapshot` 结构定义完成
- [x] family `methodSubjectIds` authority 冻结
- [x] latest valid native proof source 选择规则冻结
- [x] run 选择规则冻结
- [x] dashboard/detail 数据需求冻结

### 5.2 Native Proof Hard Cut

- [x] native proof facts builder 落地
- [x] native proof claims builder 落地
- [x] native proof evaluator 落地
- [x] family progress/detail 不再由 generator 私有逻辑计算
- [x] Convert.Char Conversions 有真实 native proof detail
- [x] Buffer.Memory Operations 有真实 native proof detail
- [x] Enum Parsing And Formatting 有真实 native proof detail

### 5.3 TestCode Hard Cut

- [x] testCode facts builder 落地
- [x] testCode evaluator 落地
- [x] analyze-gaps 改为新链路消费者/写回器
- [x] family snapshot 中 testCode 与 nativeProof 协同存在

### 5.4 Managed/HotUpdate/Benchmark

- [x] managed proof evaluator 落地
- [x] hotupdate proof evaluator 落地
- [x] benchmark evaluator 落地
- [x] family snapshot 中 5 条 gate 齐全

### 5.5 Projection / Dashboard

- [x] `program.json` 只读 snapshot
- [x] `dll-matrix.json` 只读 snapshot
- [x] `dlls/*.json` 只读 snapshot
- [x] dashboard HTML 只读 snapshot
- [x] DLL detail HTML 只读 snapshot
- [x] dashboard 上确认有真实 family verification 数据
- [x] tooltip/detail 确认有真实内容
- [x] dashboard 无数据问题若出现，已定位并修复

### 5.6 全面验证

- [x] unit tests 通过
- [x] integration tests 通过
- [x] family test projects 通过
- [x] `foundation-dll analyze-gaps` 通过
- [x] `verify verification-v1 --json` 通过
- [x] formal artifacts 刷新成功
- [x] dashboard 刷新成功

### 5.7 旧逻辑清理

- [x] 旧 generator 内联 coverage 选择逻辑删除
- [x] 旧 generator 内联 family progress 计算删除
- [x] 旧启发式 fallback 删除
- [x] 旧临时字段删除
- [x] grep 检查通过
- [x] 第二轮 grep 复查通过
- [x] 第三轮 review 确认无残留

### 5.8 完成前复核

- [x] review roadmap 所有任务均为 completed
- [x] review design 与最终实现一致
- [x] review dashboard 与正式 JSON 一致
- [x] review cleanup 结果确认旧逻辑已干净退场
- [x] review 全链路验证证据齐全

## 6. 每阶段完成定义

每个阶段只有满足以下条件才允许标记 completed：

- 该阶段 checklist 对应条目全部勾选
- 该阶段改动有自动化验证
- 若涉及 projection / refresh，则必须有 formal refresh 证据
- 若涉及旧逻辑清理，则必须有 grep + 人工复查双重证据

## 7. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260427-01` | `phase-0` | `completed` | `codex` | 冻结 hard cut 边界、authority 和全量 checklist | - |
| `20260427-01` | `phase-1` | `completed` | `codex` | 引入新的 family verification kernel 对象模型 | `phase-0` |
| `20260427-01` | `phase-2` | `completed` | `codex` | nativeProof hard cut | `phase-1` |
| `20260427-01` | `phase-3` | `completed` | `codex` | testCode hard cut | `phase-2` |
| `20260427-01` | `phase-4` | `completed` | `codex` | managed/hotupdate/benchmark expansion | `phase-3` |
| `20260427-01` | `phase-5` | `completed` | `codex` | projection/dashboard hard cut | `phase-4` |
| `20260427-01` | `phase-6` | `completed` | `codex` | legacy retirement + full verification + review closeout | `phase-5` |

## 8. 依赖

- `docs/dev/completed/20260426-01-capability-family-test-code-first-pipeline/design-v1-01.md`
- `docs/dev/completed/20260426-01-capability-family-test-code-first-pipeline/STATUS.md`
- `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/verification-v1/spec.md`
- `build/toolchains/run/testing/foundation_dll_audit_generator.py`
- `build/toolchains/run/testing/foundation_dll/*`
- `verification/projections/foundation-dll-audit/*`
- `artifacts/subjects/SolutionCorePack/runs/*/analysis/generated/*coverage.json`

## 9. 风险

- methodSubjectIds authority 仍存在局部经验映射，hard cut 过程中如果不先冻结，会反复返工
- generator 目前已经承载太多逻辑，切换时若不分批验证，容易把 dashboard 整体打坏
- 删除旧逻辑时很容易漏掉“看似无害”的 fallback，必须多轮 grep + review

## 10. 备选路径

### 路径 A：继续局部修补 generator

- 不推荐
- 原因：维护成本只会继续上升

### 路径 B：保留双轨长期并存

- 不推荐
- 原因：很难证明哪条链路才是 authority

### 路径 C：执行本 roadmap 的 hard cut

- 推荐
- 原因：最干净，且与当前用户目标一致

## 11. 当前建议推进顺序

1. 先完成 authority freeze 和 master checklist
2. 立刻引入新 kernel 对象
3. 先硬切 nativeProof
4. 再硬切 testCode
5. 然后扩剩余 gate
6. 最后做 projection hard cut、legacy retirement 和 full verification

## 12. 子任务执行策略

- `child_execution_mode: manual`
- `auto_continue: false`
- `auto_stop_policy: manual-review`
- 这是一次高风险 hard cut，不在 roadmap 创建后自动进入实现
- `recommended_next_child`: `phase-1-kernel-introduction`
