# Capability Family Test-Code-First Pipeline Roadmap v1

Date: 2026-04-26 19:05:00 +08:00
Status: in-progress

## 1. 目标

把 foundation DLL 的 capability-family authority 从“报表分母 / 投影视图”继续推进到“测试代码优先”的执行主线：

- capability family 能显式声明测试归属与 gate 义务
- 新 family / widened family 能先被 gap analysis 发现，再生成 skeleton test code
- family 级测试元数据能进入 run pipeline，而不是停留在人工文档
- foundation-dll 报表能展示 family 的 test-code 状态，而不是只展示 capability closure / workflow progress

最终目标：

- `CapabilityTestAttribute + BenchmarkAttribute` 成为 family 级测试声明入口
- `foundation-dll analyze-gaps` / `check-family` 成为可执行命令面
- capability-family ledger 含正式 `testCode` 状态字段
- foundation-dll report / docs / archive 同步展示 family 级 test-code 缺口
- 命中新测试流程 / projection contract 时，formal `verification-v1` 刷新成为收口必选项

## 2. 范围边界

本 roadmap 负责：

- 定义并实现 family 级测试声明 contract
- 定义并实现 gap analyzer / skeleton generator / family status write-back
- 把 family test-code 状态接入 foundation-dll projection / docs
- 补齐与该主线直接相关的 CLI、测试、formal refresh

本 roadmap 不负责：

- 在本轮内完成所有 family 的真实测试填充
- 重写现有 subject registry / declaration scan 主架构
- 替换现有 `test subject` / `verify verification-v1` 主入口
- 把 CoreLib widening 主线并入此 roadmap 的实现范围

## 3. 非目标

- 不把当前任务伪装成“已接近完成”；现状是设计完成、实现只落了一部分
- 不以 docs/plan 代替自动化实现与验证
- 不把 `test inventory` 当作 public command surface
- 不跳过 formal refresh

## 4. 阶段列表

### Phase 0: Command Surface And SDK Contract Stabilization

- goal:
  - 冻结 family 级测试声明入口和最小 CLI 命令面
- exit_criteria:
  - `CapabilityTestAttribute` / `BenchmarkAttribute` 契约有自动化测试覆盖
  - `foundation-dll` 命令面与 manifest 一致
  - 不存在 CLI prefix-match / manifest parity 回归
- deliverables:
  - SDK contract tests
  - foundation-dll command/manifest tests
  - synced manifest / parser behavior
- dependencies:
  - existing `foundation-dll` command family
- resolved_decisions:
  - `CapabilityTestAttribute` 走 class-level
  - `BenchmarkAttribute` 保留 `ChaosBenchmarkAttribute` 兼容别名
- watch_items:
  - contract test 仍引用旧 `ChaosBenchmarkAttribute` 形态

### Phase 1: Gap Analysis And Candidate Test-Code Authority

- goal:
  - 实现 family 级 gap analysis，并把结果写回 capability-family ledger
- exit_criteria:
  - `gap_analyzer.py` 源码落地
  - `foundation-dll analyze-gaps` 命令可运行
  - ledger `testCode` 字段有 contract coverage
- deliverables:
  - gap analyzer implementation
  - command dispatch integration
  - unit tests for derive/write-back/status transitions
- dependencies:
  - Phase 0
- resolved_decisions:
  - `testCodeStatus` 至少支持 `no-coverage / needs-tests / coverage-widened / present`
- watch_items:
  - 当前 candidate/formal ledger 流程不能被 analyze-gaps 破坏

### Phase 2: Skeleton Test Generation

- goal:
  - 从 gap analysis 结果自动生成 family skeleton test code
- exit_criteria:
  - `test_code_generator.py` 源码落地
  - family skeleton 输出结构固定
  - auto-generate 与 analyze-gaps 串联
- deliverables:
  - generator implementation
  - generator tests
  - sample generated output contract
- dependencies:
  - Phase 1
- resolved_decisions:
  - 自动生成文件与手写文件必须分离
- watch_items:
  - 不要覆盖手写 `.cs`

### Phase 3: Family Test Discovery And Pipeline Read Model

- goal:
  - 把 family test metadata 变成 pipeline 可消费对象
- exit_criteria:
  - `test_discovery.py` 源码落地
  - 能从 family test project 产出 stable metadata
  - foundation-dll command / report 有最小 read-model 接口
- deliverables:
  - discovery implementation
  - discovery tests
  - family test metadata contract
- dependencies:
  - Phase 2
- resolved_decisions:
  - 本阶段先构建 read-model，不强制完整 plugin execution
- watch_items:
  - 不要在 discovery 阶段偷渡完整新 runner

### Phase 4: Projection / Report / Formal Refresh Cutover

- goal:
  - 把 family test-code 状态接入 projection、docs 和 formal refresh
- exit_criteria:
  - foundation-dll report 展示 test-gap / test-code 状态
  - `verification-v1` formal refresh 通过
  - docs / projection / archive 一致
- deliverables:
  - generator/report updates
  - refreshed formal outputs
  - authority/wiki/spec sync if rule changed
- dependencies:
  - Phase 3
- resolved_decisions:
  - report 中的 family test-code 状态属于 projection，不反向定义 authority
- watch_items:
  - report contract 变更后必须跑 `run verify verification-v1 --json`

## 5. 每阶段完成定义

每个阶段只有同时满足以下条件才算完成：

- 阶段目标对应的源码真实存在，不是只有 `__pycache__`
- 至少一组自动化测试覆盖该阶段主 contract
- 若改动命中 command surface / projection contract，则对应集成测试同步更新
- 不把 open question 留给下一阶段实现时临场拍板

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260426-01` | `phase-0` | `in-progress` | `codex` | 先修 CLI/manifest 回归，并稳定 SDK 与命令面 contract | - |
| `20260426-01` | `phase-1` | `ready` | `codex` | 落地 gap analyzer 和 ledger testCode authority | `phase-0` |
| `20260426-01` | `phase-2` | `planned` | `codex` | 落地 skeleton test generation | `phase-1` |
| `20260426-01` | `phase-3` | `planned` | `codex` | 落地 family test discovery read-model | `phase-2` |
| `20260426-01` | `phase-4` | `planned` | `codex` | 把 family test-code 状态接入 projection / formal refresh | `phase-3` |

## 7. 依赖

- `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/verification-v1/spec.md`
- `wiki/06-测试验证/INDEX.md`
- `build/toolchains/run/commands/foundation_dll.py`
- `build/toolchains/run/testing/foundation_dll/*`
- `verification/projections/foundation-dll-audit/capability-family-ledger.json`
- `src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.cs`

## 8. 风险

- 当前 `tests/unit/compatibility/test_chaos_test_framework_contracts.py` 仍绑定旧 contract 断言，后续 SDK 变更若不同步测试，会持续给出假阴性
- family pipeline 的设计跨度大，若一次性把 discovery / generator / plugin 全部硬接，容易把 task 拖成未验证的大杂烩
- report contract 已是 formal authority 消费面，任何 test-code 字段接入都必须同步 formal refresh

## 9. 备选路径

### 路径 A: 只保留 SDK attribute，不做 gap analyzer / generator

- 不推荐
- 原因: 只能写声明，不能形成 code-first pipeline

### 路径 B: 先做 gap analyzer + generator，再逐步接 discovery / projection

- 推荐
- 原因: 能最快形成可验证闭环

## 10. 当前建议推进顺序

1. 先完成 Phase 0 收口
2. 立即进入 Phase 1，补真实缺失的 `gap_analyzer.py`
3. 再落地 `test_code_generator.py`
4. 然后补 discovery read-model
5. 最后统一做 projection / formal refresh

## 11. 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child`: `phase-1-gap-analysis-and-candidate-test-code-authority`
