# Brainstorm v1-06

## 背景

Batch 5 已经收敛为 analysis 侧的 preserve/stripping proof：

- `LinkerStrippingProof` 可真实 convert
- `preserve-descriptor.json`、`metadata-registration.json`、`native-reference.lowering-plan.json` 已能稳定产出
- 但 native reference 仍会对 `generic-analysis-only` 给出 warning，因此不能把 Batch 5 误报为 native/runtime 闭环

剩余未闭环项集中在 Batch 6 perf governance：

- `6.3` `InterfaceDispatchProof` 需要真实的 steady-state dispatch 指标，而不是整个 proof 进程 wall-clock
- `6.4` `GoldenMultiProject` 需要 convert baseline，但 golden subject 目前不适合硬塞进标准 subject pipeline

## 候选方案

### 方案 A：直接量整程序 wall-clock

- 内容：
  - 跑 `InterfaceDispatchProof` PROFILE 二进制
  - 用整个进程耗时近似 dispatch overhead
- 优点：
  - 接线最少
- 缺点：
  - 不诚实；会混入 bootstrap、对象构造、字符串拼接与 I/O
  - 不能代表 steady-state interface dispatch

### 方案 B：PROFILE native proof 自产数值指标 + suite 级 convert perf

- 内容：
  - `InterfaceDispatchProof` 的 PROFILE 模板在 native proof 内部输出 `dispatchNanoseconds`
  - perf worker 从 JSON payload 吸收自定义数值指标并汇总 `mean/min/max`
  - `GoldenMultiProject` 走 `tests/perf/convert-golden-multi-project/check.py` + `evaluate_perf_suite(...)`
- 优点：
  - dispatch 指标可直接绑定到 proof 语义
  - convert baseline 不需要重写 subject discovery/path resolver
  - 边界清晰，适合当前 proof-first 范围
- 缺点：
  - 需要扩展 perf worker 的 payload 协议

### 方案 C：把 golden subject 正式纳入 subject pipeline 后再做 perf

- 优点：
  - 入口统一
- 缺点：
  - scope 膨胀到 path resolver / planner / discovery
  - 与本轮 Batch 6 的最小落地目标不匹配

## 选择

选择方案 B。

## 关键边界

- `6.3` 的 dispatch 指标必须来自 steady-state native proof，不接受整进程 wall-clock 冒充 dispatch overhead。
- `6.4` 的 convert perf 是 suite 级 baseline，不等于把 `subjects/golden/*` 全部并入现有 subject pipeline。
- Batch 5 仍然只完成 analysis 侧 stripping/preserve proof，未闭环 debug baseline，也未闭环 native/runtime stripping 证明。
