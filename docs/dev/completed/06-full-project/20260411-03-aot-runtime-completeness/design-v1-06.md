# Design v1-06

## Batch 6 最小实现设计

### 目标

- 补齐 Batch 6.3：`InterfaceDispatchProof` 真实 native perf baseline，指标为 `dispatch <= 5ns`
- 补齐 Batch 6.4：`GoldenMultiProject` convert time suite baseline，阈值为 `<= 10s`
- 保持 Batch 5 的事实边界：仅完成 analysis 侧 stripping/preserve proof

### 设计范围

#### 1. perf worker 自定义指标协议

- 扩展 `build/toolchains/run/testing/subject_workers.py`
- 约定：
  - perf harness/native perf 二进制可在 stdout 最后一行输出 JSON payload
  - `elapsedMilliseconds` 继续作为 `durationMs`
  - `metrics` 对象中的数值字段会写入 sample
- 汇总规则：
  - 对 `countedInSummary = true` 的 sample 计算
  - 固定输出 `sampleCount` / `mean|min|maxDurationMs`
  - 对自定义数值字段额外输出：
    - `meanDispatchNanoseconds`
    - `minDispatchNanoseconds`
    - `maxDispatchNanoseconds`
  - 命名按 `mean|min|max` + 字段名首字母大写拼接

#### 2. InterfaceDispatchProof native perf proof

- `subjects/InterfaceDispatchProof/subject.manifest.json`
  - 新增 `validationProfiles.perf-profile = ["proof", "perf"]`
  - 新增 `validation.perf.driver = "native-runtime-perf"`
  - 新增 `baselines.perf`
  - 新增 `native-runtime-perf` pipeline
  - 新增 `windows-native-profile` matrix
- 新增 baseline：
  - `subjects/InterfaceDispatchProof/baselines/perf/windows-native-profile/windows.json`
  - 阈值：`meanDispatchNanoseconds = 5.0`
- 模板：
  - `NativeReferenceProof.InterfaceDispatchMessage.cpp.scriban`
  - `CHAOS_VARIANT_PROFILE` 下：
    - instance method 简化为返回 captured string，避免把字符串拼接算进 dispatch
    - 先做 warmup
    - 分别测量 direct call 与 `resolve_virtual_method + invoke_virtual` steady-state 路径
    - 用 `(virtual_total - direct_total) / iterations` 估算 dispatch overhead
    - 最后一行输出：
      - `elapsedMilliseconds`
      - `metrics.dispatchNanoseconds`

#### 3. GoldenMultiProject convert perf suite

- 保持 suite 级入口：
  - `tests/perf/convert-golden-multi-project/check.py`
  - `tests/perf/convert-golden-multi-project/baselines/windows.json`
- 脚本行为：
  - 真实执行 `dotnet run --project src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -- convert subjects/golden/GoldenMultiProject --output <temp>`
  - 通过 `evaluate_perf_suite(...)` 比较 `convertDurationMs`
- 模块加载：
  - 使用 `importlib.util.spec_from_file_location(...)` 加载 `perf.py`
  - 避免直接 `exec(...)` 破坏相对导入

### 验证策略

- RED：
  - `python -m pytest tests/unit/run/test_subject_workers_perf.py -v`
  - `python -m pytest tests/unit/run/test_phase2_perf_governance.py -v`
- GREEN：
  - `python -m pytest tests/unit/run/test_subject_workers_perf.py -v`
  - `python -m pytest tests/unit/run/test_phase2_perf_governance.py -v`
  - `python -m pytest tests/unit/run/test_subject_manifest_schema.py -v`
  - `python -m pytest tests/unit/run/test_subject_planner.py -v`
  - `python -m pytest tests/unit/run/test_subject_reporting.py -v`
  - `python -m pytest tests/unit/run -v`
  - `dotnet build subjects/InterfaceDispatchProof/source/InterfaceDispatchProof.csproj -c Debug`
  - `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`
  - `python build/toolchains/run/run.py --json test subject --id subject/InterfaceDispatchProof --goal perf.profile --validation-profile perf-profile --variant PROFILE`
  - `python tests/perf/convert-golden-multi-project/check.py`

### 风险

- `dispatchNanoseconds` 仍是 proof-first 指标，依赖当前 PROFILE 模板的“direct call vs invoke_virtual”差值模型；它是 steady-state 近似值，不是完整产品 perf dashboard。
- `GoldenMultiProject` convert perf 仍停留在 suite 级入口；如后续要统一到 subject pipeline，需要单独处理 `subjects/golden/*` 的发现与路径规则。
