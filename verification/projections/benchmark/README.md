# Benchmark Dashboard

`verification/projections/benchmark/` 由统一入口自动维护。不要手动编辑 `overview.json`、`subjects/*.json`、`dashboard.html`。

当前 dashboard 的统一基线是 `managed` 模式的 `meanDurationMs`。所有 subject 和 case 的对比都以这个 managed latency baseline 为中心展开。

## Retained Subjects

- `SolutionCorePack`
  - 目标：验证 solution 级核心 runtime/AOT 性能面。
  - `arithmetic-bench` 是主对比 workload，覆盖 `managed + native`。
  - `allocation-bench`、`dispatch-bench`、`generic-bench` 保留为 `managed-only` 诊断 case，用来暴露当前 native AOT 还未覆盖的能力边界。

- `MixedExecutionFeaturePack`
  - 目标：验证 mixed execution 的 `managed / native / interpreter` 切换成本。
  - `mixed-execution-native-bench` 覆盖 `managed + native + interpreter`，是主对比 case。
  - `mixed-execution-bench` 覆盖 `managed + interpreter`，聚焦解释器参与的混合路径。

- `HotUpdateHostPack`
  - 目标：验证 host 侧 hot-update 的 load / dispatch / roundtrip 成本。
  - 当前 benchmark case 全部是 `managed-only`，因为它们测试的是托管 host 能力，不是 native AOT 对比面。

## Dashboard Semantics

- `recorded`
  - 已成功生成 benchmark 数据。

- `missing`
  - manifest 或 case contract 声明支持该 mode，但这次没有录到数据。

- `unsupported`
  - subject 或 case contract 明确不支持该 mode，不应被当成缺数。

- `regression verdict`
  - 数据已经生成成功，但和 baseline 对比没有达标。
  - 这不是执行失败；执行失败会表现为 benchmark 流程报错或没有产出记录。

## Update Commands

```bash
# 跑单个 retained subject 的 benchmark，写入原始记录
run benchmark --subject SolutionCorePack --record

# 跑全部 retained benchmark subjects，写入原始记录
run benchmark --all --record

# 合并最新 benchmark 记录并刷新 formal archive / projection
run verify verification-v1 --json

# 仅重建当前 benchmark dashboard 视图
run benchmark --dashboard --open
```

## Generated Files

- `dashboard.html`
  - 静态 dashboard 页面。

- `overview.json`
  - retained benchmark subjects 的总览数据。

- `subjects/*.json`
  - 每个 retained subject 的完整 benchmark 数据，包含 case 级明细。
