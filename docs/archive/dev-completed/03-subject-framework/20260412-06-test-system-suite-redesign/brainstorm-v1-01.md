# 测试体系 Suite 化重设计 Brainstorm

**日期：** 2026-04-12 15:22 +08:00

## 用户目标

用户希望围绕当前测试体系做一次更大尺度的清理与重设计，目标包括：

- 把 `tests/` 下仍然夹带的 subject-specific 逻辑抽成通用机制，让 benchmark 与 proof 共用。
- 清理 `tests/` 中仍然残留的 `phase*` 命名，改成按通用功能分类。
- 收敛 `subjects/` 根目录，不再继续堆出大量 proof subject 与 benchmark subject。
- 让未来新增测试项时，不需要再在根目录新开一个又一个顶层 subject。
- 站在“多平台 IL2CPP + 热更系统”的角度，补齐测试能力覆盖模型，而不只看 proof/benchmark 两条局部主线。

## 当前仓库暴露出来的问题

### 结构问题

- `tests/unit/run/` 里仍有 30 个 `test_phase*` 文件，说明测试关注点还在跟历史阶段耦合。
- `tests/integration/run/` 与 `tests/tooling/run/` 存在同名测试文件，说明职责边界并不清晰，例如：
  - `test_command_manifest.py`
  - `test_json_output.py`
  - `test_tui.py`
  - `test_tui_routing.py`
  - `test_unified_test_command.py`
- `subjects/` 下已有 11 个 `Bench*` subject 与 40 个非 `Bench*` subject；继续“一项一 subject”会持续膨胀。

### 语义问题

- 当前把 `subject`、`phase`、`proof/benchmark`、`平台差异`、`能力差异` 混在同一层表达。
- 未来如果继续把 correctness / perf / hot-update / mobile 差异都建成新顶层 subject，根目录会比现在更乱。
- 当前“新增测试项只要新增 subject 就行”的想法，短期省事，但长期会把 registry、artifact、TUI、文档与 owner 全部拖向碎片化。

## 已对齐的核心概念

### `subject` 的长期职责

- `subject` 不应该再等于“单个 proof”或“单个 benchmark”。
- `subject` 应该回到“稳定的 suite root”角色，承载一组共享 source、validation、matrix、baseline 与 artifact contract。

### `scenario` 的职责

- `scenario` 是 `ProofSuite` 里的最小 correctness 扩展单元。
- 它描述一个具备明确预期证据的验证场景，而不是一个新的顶层 subject。
- 例子：
  - `dispatch`
  - `generic-layout`
  - `array-boxing`
  - `delegate`
  - `exception`
  - `reflection-interop-closure`
  - `hot-update-rollback`
  - `mobile-host-startup`

### `workload` 的职责

- `workload` 是 `BenchmarkSuite` 里的最小性能扩展单元。
- 它描述一个可以独立测量的工作负载、指标与基线，不是新的顶层 benchmark subject。
- 例子：
  - `arithmetic`
  - `allocation`
  - `dispatch`
  - `generic`
  - `exception`
  - `game-loop`
  - `hot-update-load`
  - `hot-update-dispatch`
  - `hot-update-roundtrip`
  - `mixed`

### 更大维度上的能力模型

对于多平台 IL2CPP + 热更系统，proof 与 benchmark 只是两种执行方式，不是全部测试能力。长期至少要有以下 8 类 capability：

- semantic correctness
- metadata/artifact correctness
- execution-model consistency
- platform host/packaging
- stability/recovery
- performance
- observability
- compliance/security

这些 capability 应该是横切维度，挂在 scenario/workload/pipeline 上，而不是继续扩展顶层 subject。

## 方案对比

### 方案 A：最小改动，只重命名 tests，不重做 subject 模型

做法：

- `tests/` 去掉 `phase*` 命名
- 维持当前大量 proof/benchmark subject
- 后续仍允许按测试项新增顶层 subject

优点：

- 迁移成本最低
- 短期最少碰 manifest 和 registry

缺点：

- `subjects/` 根目录继续膨胀
- subject-specific 配置仍会回流到 `tests/`
- 未来 mobile / hot-update / release gate 这些能力很难干净落位

结论：

- 不推荐。它只处理命名，不处理结构。

### 方案 B：少量 suite subject + `scenario/workload` 内部扩展

做法：

- 顶层先收敛成 `ProofSuite` 与 `BenchmarkSuite`
- correctness 测试点变成 `scenario`
- perf 测试点变成 `workload`
- capability / matrix / availability / pipeline 作为横切维度管理
- `tests/` 只保留通用机制；subject-specific 配置、baseline、fixture 回归 `subjects/`

优点：

- 结构最干净
- 能兼容 proof 与 benchmark 的共性机制
- 容易扩展到 hot-update、移动端、release gate、device lab
- 新增测试点的成本真正下降，且不会污染根目录

缺点：

- 需要一次明确的迁移设计
- manifest、selector、registry 命名要一起收敛

结论：

- **推荐方案。**

### 方案 C：按 capability 建多个顶层 suite

做法：

- 顶层拆成 `CorrectnessSuite`、`PerformanceSuite`、`StabilitySuite`、`PlatformSuite`、`ComplianceSuite`
- 每类能力各自承载 proof/benchmark/hot-update/mobile 测试

优点：

- 治理视角清晰
- release gate 容易按能力维度拼接

缺点：

- 同一 source 与 baseline 会在多个 suite 重复表达
- 同一 workload/scenario 会因为能力标签不同而多地复制
- 对当前仓库的 subject 演化路径不自然，实施成本比方案 B 更高

结论：

- 不推荐作为第一阶段终态；可把 capability 作为正交标签保留，不必先升成顶层 suite。

## 收敛结论

本次设计采用 **方案 B**，并明确以下长期约束：

1. 顶层 `subject` 应该是少量、稳定、可长期维护的 suite root，不再等于单个测试点。
2. 新增 correctness 项时新增 `scenario`；新增 perf 项时新增 `workload`；默认不再新增顶层 subject。
3. capability 是横切维度，不是顶层目录。
4. `tests/` 只验证 generic mechanism；subject-specific source、baseline、fixture、配置回到 `subjects/`.
5. 平台是否已实现，不通过新 subject 表达，而通过 `availability` 与 matrix capability 表达。

下一步进入正式设计文档，并在用户审查后决定是否进入 roadmap 分阶段落地。
