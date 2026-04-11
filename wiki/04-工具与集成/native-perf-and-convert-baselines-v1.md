# native perf 与 convert baseline v1

## 适用范围

本页记录仓库当前稳定使用的两类性能基线入口：

- subject 级 native perf baseline
- suite 级 convert perf baseline

它描述的是长期协议，不记录单次运行结果。

## 1. subject 级 native perf baseline

### 入口

- subject manifest 中声明：
  - `validation.perf.driver = "native-runtime-perf"`
  - `validationProfiles.perf-profile`
  - `executionPipelines[].pipelineId = "native-runtime-perf"`
  - 对应 `windows-native-profile` 等 perf matrix
- baseline 文件位置：
  - `subjects/<SubjectId>/baselines/perf/<matrixId>/<host>.json`

### payload 协议

`native-runtime-perf` worker 约定 native proof 二进制可以在 stdout 最后一行输出 JSON：

```json
{
  "elapsedMilliseconds": 10.430,
  "metrics": {
    "dispatchNanoseconds": 2.079
  }
}
```

规则：

- `elapsedMilliseconds` 会覆盖 sample 的 `durationMs`
- `metrics` 中的数值字段会被写入 sample
- warmup sample 仍会记录，但不会进入 summary

### 汇总规则

worker 会对 `countedInSummary = true` 的 sample 生成：

- 固定字段：
  - `sampleCount`
  - `meanDurationMs`
  - `minDurationMs`
  - `maxDurationMs`
- 自定义数值字段：
  - `mean<Metric>`
  - `min<Metric>`
  - `max<Metric>`

例如 `dispatchNanoseconds` 会汇总成：

- `meanDispatchNanoseconds`
- `minDispatchNanoseconds`
- `maxDispatchNanoseconds`

### 当前参考 subject

- `subjects/InterfaceDispatchProof/`
  - 通过 PROFILE native proof 输出 `dispatchNanoseconds`
  - 当前基线为 `meanDispatchNanoseconds <= 5.0`

## 2. suite 级 convert perf baseline

### 适用场景

当验证对象不是标准 subject pipeline，而是仓库级流程或特殊目录布局时，使用 suite 级 baseline：

- baseline 放在 `tests/perf/<suite>/baselines/<host>.json`
- 校验脚本放在 `tests/perf/<suite>/check.py`

### 当前参考 suite

- `convert-golden-multi-project`
  - 真实执行：
    - `dotnet run --project src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -- convert subjects/golden/GoldenMultiProject --output <temp>`
  - 通过 `evaluate_perf_suite(...)` 比较 `convertDurationMs`
  - 当前基线为 `convertDurationMs <= 10000.0`

### 模块加载约定

`check.py` 应通过 `importlib.util.spec_from_file_location(...)` 加载 `build/toolchains/run/testing/perf.py`，不要直接 `exec(...)` 文件内容。

原因：

- `perf.py` 依赖相对导入
- 直接 `exec(...)` 会绕开模块初始化，破坏导入语义

## 3. 边界

- suite 级 convert perf baseline 不等于把 `subjects/golden/*` 正式纳入 subject discovery / planner / executor。
- `dispatchNanoseconds` 是 proof-first steady-state 指标；它适合当前 `InterfaceDispatchProof` 的 contract，不能自动外推成完整产品 perf dashboard。
