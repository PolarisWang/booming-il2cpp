# Progress v1-01: Batch 1-4 完成 (Phase B 全部任务)

**时间**: 2026-04-12
**Batch**: 1-4 全部完成

## 产出

### Batch 1: interpreter-runtime-perf worker
- `subject_workers.py` 新增 `run_interpreter_runtime_perf()` worker
- `DEFAULT_STAGE_WORKERS` 注册 `interpreter-runtime-perf`
- 支持: 有 harness project 时运行, 无时返回 skip 结果 (graceful degradation)

### Batch 2: comparison aggregate
- `benchmark_comparison.py` 新增 (compute_comparison, evaluate_targets)
- `subject_workers.py` 新增 `run_benchmark_comparison_aggregate()` + 注册
- `tests/unit/run/test_benchmark_comparison.py` 9 个单元测试全部通过

### Batch 3: 8 benchmark subjects
- `subjects/Bench{Arithmetic,Allocation,Dispatch,Generic,Delegate,Exception,Mixed,GameLoop}/`
- 每个 subject: source/Program.cs + 3 pipelines + 3 matrices in manifest
- 全部 8 个 Program.cs 独立 `dotnet run` 验证输出正确 JSON

### Batch 4: device + records + dashboard + CLI
- `device_detector.py` — Windows/macOS/Linux/Android/iOS 自动检测，Windows 验证 OK
- `benchmark_records.py` — JSON Lines append/query/history，文件锁，Windows 兼容
- `benchmark_dashboard_generator.py` — 扫描所有 subjects, 生成 HTML, update_docs
- `templates/benchmark-dashboard.html` — 3 Tab (Overview/Detail/DeviceComparison), Chart.js
- `commands/benchmark.py` — CLI dispatch (record/dashboard/status/all)
- `manifests/run/commands.test-and-verify.json` 注册 benchmark 命令
- `run.py` 添加 benchmark handler + 正确传递 --options
- `docs/benchmark/` 初始化 (README.md + overview.json + dashboard.html)

## 端到端验证

```
run benchmark --subject BenchArithmetic --mode managed --record
→ ✓ BenchArithmetic / managed / Windows PC (...)
→ docs/benchmark/ updated
→ docs/benchmark/overview.json 包含 BenchArithmetic managed 数据 ✓
```

## 遇到的问题

1. `fcntl` Windows 不可用 → 条件导入 + Windows 用 lock file 策略
2. run_manifest.json 是 sharded 格式 → 需在 manifests/run/ shard 文件中注册
3. `--subject --mode --record` 被 manifest parser 解析为 options → 在 run.py handler 中重构 args list

## Phase B 退出标准验证

- [x] 8 个 benchmark subjects 全部存在，manifest 合法
- [x] interpreter-runtime-perf + benchmark-comparison-aggregate 注册
- [x] device_detector.py Windows 返回有效 device info
- [x] benchmark_records.py append/query 测试通过
- [x] run benchmark --dashboard 生成 HTML (18KB)
- [x] docs/benchmark/ 初始化完成
