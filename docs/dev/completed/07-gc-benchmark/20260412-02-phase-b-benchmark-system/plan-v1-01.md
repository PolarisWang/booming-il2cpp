# Phase B — 三方 Benchmark 报告体系 实现计划

> **面向执行 Agent：** 必须使用 `dev:executing-plans` 来执行本计划。每完成一个 checkbox 立即标记。每个 Batch 完成后更新 `STATUS.md` 和 `notes/progress-v1-NN.md`。

**目标：** 构建增量 benchmark 数据记录管道，支持 C# (.NET) / AOT (native) / HotUpdate (interpreter) 三方对比，生成自包含 HTML dashboard，每次跑完自动更新 `docs/benchmark/`。

**架构：** 分 4 批推进。Batch 1 新增 Python stage worker 和比较聚合层；Batch 2 新增 8 个 benchmark subjects + 三 pipeline 结构；Batch 3 新增 device 检测 + JSON Lines 记录层 + dashboard 生成器；Batch 4 新增 CLI 命令 + docs/benchmark 初始化。

**技术栈：** Python 3.11+, JSON Lines, Chart.js (CDN), dotnet CLI, CMake

**设计文档：** `docs/dev/in-progress/20260411-10-post-phase7-development-roadmap/benchmark-design-v1-01.md`

**预期知识沉淀：** `wiki/testing/benchmark-system.md`

---

## 涉及文件清单

| 文件 | 操作 | 所属步骤 |
|------|------|---------|
| `build/toolchains/run/testing/subject_workers.py` | 修改 — 新增 2 个 stage worker + DEFAULT_STAGE_WORKERS | B.1, B.2 |
| `build/toolchains/run/testing/benchmark_comparison.py` | **创建** | B.2 |
| `subjects/Bench*/` × 8 | **创建** — subject.manifest.json + source | B.3, B.4 |
| `build/toolchains/run/testing/device_detector.py` | **创建** | B.5 |
| `build/toolchains/run/testing/benchmark_records.py` | **创建** | B.6 |
| `build/toolchains/run/testing/benchmark_dashboard_generator.py` | **创建** | B.7 |
| `build/toolchains/run/testing/templates/benchmark-dashboard.html` | **创建** | B.7 |
| `build/toolchains/run/commands/benchmark.py` | **创建** | B.8 |
| `build/toolchains/run/run_manifest.json` | 修改 — 注册 benchmark 命令 | B.8 |
| `contracts/artifacts/v0/schemas/benchmark-record.schema.json` | **创建** | B.6 |
| `docs/benchmark/README.md` | **创建** | B.9 |
| `docs/benchmark/dashboard.html` | **创建** (初始空状态) | B.9 |
| `docs/benchmark/overview.json` | **创建** (初始空) | B.9 |

---

## 步骤

### Batch 1: interpreter-runtime-perf stage worker (B.1)

- [ ] **1.1** 在 `subject_workers.py` 末尾（DEFAULT_STAGE_WORKERS 之前）添加 `run_interpreter_runtime_perf` worker:
  - 模式: `host-input` upstream → 加载 IR → dotnet run interpreter harness → 采集 timing
  - 参数从 `request["selection"]["executionContext"]["runtimeProfile"]` 读取 sample count / iterations
  - 输出格式与 `run_runtime_perf_collect` 完全一致 (samples, summaryMetrics, baseline 比较)
  - 对 interpreter harness 路径的处理: 从 manifest validation spec 的 `"driver": "interpreter-runtime-perf"` 读取项目路径

- [ ] **1.2** 在 `DEFAULT_STAGE_WORKERS` (line 1971) 添加注册:
  ```python
  "interpreter-runtime-perf": run_interpreter_runtime_perf,
  ```

- [ ] **1.3** 创建最小 `subjects/InterpreterPerfSmoke/` subject 验证 worker 可以被调用 (不需要真实 interpreter 执行，只需要 stage 注册正确)

- [ ] **1.4** 验证: `python build/toolchains/run/run.py test subject --subject InterpreterPerfSmoke` 不报 "unknown stage kind"

**Batch 1 验收**: stage 注册成功，不报 "unknown stage kind" 错误

---

### Batch 2: benchmark-comparison-aggregate worker + comparison 模块 (B.2)

- [ ] **2.1** 创建 `build/toolchains/run/testing/benchmark_comparison.py`:
  ```python
  def compute_comparison(managed_metrics, native_metrics, interpreter_metrics) -> dict
  def evaluate_targets(comparison, targets=None) -> dict  # pass/fail verdict
  ```
  - `compute_comparison`: 对每个共同 metric 计算 ratio = mode_a / mode_b (除零 → "N/A", 缺失 → None)
  - `evaluate_targets`: 默认目标 AOT/C# ≥ 2.0, HotUpdate/C# ≤ 50.0
  - ratio 保留 2 位小数

- [ ] **2.2** 在 `subject_workers.py` 添加 `run_benchmark_comparison_aggregate` worker:
  - 从 upstream 读取 3 个 mode 的 manifest (managed/native/interpreter)
  - 调用 `benchmark_comparison.compute_comparison` + `evaluate_targets`
  - 输出 `comparison.json`

- [ ] **2.3** 在 `DEFAULT_STAGE_WORKERS` 注册:
  ```python
  "benchmark-comparison-aggregate": run_benchmark_comparison_aggregate,
  ```

- [ ] **2.4** 单元测试 `tests/unit/run/test_benchmark_comparison.py`:
  - 正常三方数据 → ratio 正确计算
  - 缺失一方 → graceful (None, 不 crash)
  - 除零 → "N/A"

**Batch 2 验收**: 单元测试通过；worker 注册成功

---

### Batch 3: 8 个 Benchmark Subjects + 三 Pipeline (B.3, B.4)

每个 subject 结构:
```
subjects/Bench{Name}/
├── subject.manifest.json          (3 pipelines + 3 matrices)
├── source/
│   ├── Bench{Name}.csproj
│   └── Program.cs                 (PerfHarness + 测试逻辑)
└── benchmark-records/             (JSON Lines 记录目录，初始空)
```

- [ ] **3.1** BenchArithmetic: 10 万次 int 加减乘除 + float sqrt 循环
- [ ] **3.2** BenchAllocation: 循环创建 1 万对象 + 测量 GC 压力
- [ ] **3.3** BenchDispatch: 通过基类引用调用虚方法 10 万次
- [ ] **3.4** BenchGeneric: `List<int>.Add` × 1 万次 + `Dictionary<string,int>` 查找
- [ ] **3.5** BenchDelegate: 单播调用 10 万次 + multicast (3 target) 调用
- [ ] **3.6** BenchException: try/catch 路径 10 万次 + throw/catch 1000 次
- [ ] **3.7** BenchMixed: AOT method 调用 interpreter method 1 万次
- [ ] **3.8** BenchGameLoop: 模拟 1000 帧 (Update × 100 对象 + GC safepoint)

每个 subject 的 `PerfHarness` 输出格式 (最后一行 JSON):
```json
{"elapsedMilliseconds": 12.3, "iterations": 1000, "opsPerSecond": 81300}
```

- [ ] **3.9** 每个 manifest 定义 3 个 executionPipelines:
  - `managed-benchmark`: source-resolve → host-input-build → runtime-perf-collect → report-assemble
  - `native-benchmark`: source-resolve → host-input-build → analysis-frontend → generated-native-proof → build-target → native-runtime-perf → report-assemble
  - `interpreter-benchmark`: source-resolve → host-input-build → interpreter-runtime-perf → report-assemble

- [ ] **3.10** 每个 manifest 定义 3 个 environmentMatrices:
  - `windows-managed-perf` (pipelineId=managed-benchmark, runtimeProfile=managed-perf-release)
  - `windows-native-perf` (pipelineId=native-benchmark, runtimeProfile=native-perf-profile)
  - `windows-interpreter-perf` (pipelineId=interpreter-benchmark, runtimeProfile=interpreter-perf-release)

**Batch 3 验收**:
- 8 × 3 = 24 combinations 全部在 manifest 中正确定义
- 每个 PerfHarness Program.cs 独立 `dotnet run` 时输出 JSON 最后一行
- managed pipeline: `run test subject --subject BenchArithmetic --matrix windows-managed-perf` 不报结构错误

---

### Batch 4: device_detector + benchmark_records + dashboard + CLI (B.5-B.9)

- [ ] **4.1** 创建 `build/toolchains/run/testing/device_detector.py`:
  ```python
  def detect_current_device() -> dict  # Windows/macOS
  def detect_android_device(serial: str) -> dict  # via adb
  def detect_ios_device(udid: str) -> dict         # via xcrun
  ```
  - `device.id` 格式: `{platform}-{arch}-{cpu_normalized}` (全小写, 空格→-)
  - `device.name` 格式: `{设备型号} ({CPU型号})`
  - Windows: `wmic cpu get Name` 或 `platform.processor()`
  - macOS: `sysctl -n machdep.cpu.brand_string`
  - 结果缓存 `artifacts/.device-cache.json`，TTL 1h

- [ ] **4.2** 创建 `contracts/artifacts/v0/schemas/benchmark-record.schema.json`:
  ```json
  { "required": ["runId","subject","mode","platform","device","recordedAt","gitCommit","metrics"] }
  ```

- [ ] **4.3** 创建 `build/toolchains/run/testing/benchmark_records.py`:
  ```python
  def append_record(repo_root, record: dict) -> None  # JSON Lines append
  def query_latest(repo_root, subject_id, mode, device_id) -> dict | None
  def query_latest_all_modes(repo_root, subject_id, device_id) -> dict
  def query_history(repo_root, subject_id, mode, device_id, limit=20) -> list
  def query_all_devices(repo_root, subject_id, mode) -> dict
  ```
  - 文件路径: `subjects/{id}/benchmark-records/records.jsonl`
  - 反向扫描取最新记录
  - 文件锁防并发写

- [ ] **4.4** 创建 `build/toolchains/run/testing/benchmark_dashboard_generator.py`:
  ```python
  def generate(repo_root, output_path) -> None
  def update_docs(repo_root, subject_id=None) -> None
  ```
  - 扫描 `subjects/*/benchmark-records/records.jsonl`
  - 计算 latest per (subject, mode, device)
  - 生成 overview.json + subjects/{id}.json
  - 将数据嵌入 HTML: `window.BENCHMARK_DATA = {...};`

- [ ] **4.5** 创建 `build/toolchains/run/testing/templates/benchmark-dashboard.html`:
  - Tab 1: 总览表格 (subjects × modes, 默认 Windows 数据)
  - Tab 2: Subject 详情 (模式对比柱状图 + 趋势折线图 + 设备横向对比)
  - Tab 3: 设备对比 (雷达图, 相对 Windows 归一化)
  - Chart.js from CDN `cdn.jsdelivr.net/npm/chart.js`
  - 新鲜度: >7 天 ⚠, 模式间差 >3 天 标注 `~`

- [ ] **4.6** 创建 `build/toolchains/run/commands/benchmark.py` with `dispatch(args, repo_root, host_platform)`:
  ```
  run benchmark --subject <id> --mode <managed|native|interpreter> --record
  run benchmark --all --mode <mode> --record
  run benchmark --dashboard [--open]
  run benchmark status [--subject <id>] [--all]
  ```
  - `--record`: 跑对应 pipeline → device_detector → append_record → update_docs
  - `--dashboard`: 调用 generate → 可选 webbrowser.open

- [ ] **4.7** 修改 `build/toolchains/run/run_manifest.json`:
  在 commands 数组中添加 benchmark 命令组 (4 个条目: record, dashboard, status, all):
  ```json
  {
    "id": "benchmark-record",
    "title": "Run benchmark and record result",
    "group": "Test And Verify",
    "handler": "benchmark.dispatch",
    "tokens": ["benchmark"],
    "show_in_menu": true,
    "examples": ["run benchmark --subject BenchArithmetic --mode native --record"]
  }
  ```

- [ ] **4.8** 创建 `docs/benchmark/README.md`:
  说明此目录自动维护，更新方式，各文件含义

- [ ] **4.9** 创建初始 `docs/benchmark/dashboard.html` (空状态) 和 `docs/benchmark/overview.json` (空数据):
  ```json
  { "generatedAt": "...", "defaultPlatform": "windows-x64", "subjects": {} }
  ```

- [ ] **4.10** 端到端验证: `python build/toolchains/run/run.py benchmark --dashboard` 成功生成 HTML

**Batch 4 验收**:
- `run benchmark --subject BenchArithmetic --mode native --record` 成功 (即使 pipeline 跑出 fail，records.jsonl 也要追加)
- `run benchmark --dashboard` 生成 HTML，能用浏览器打开，3 个 Tab 正常渲染
- `docs/benchmark/dashboard.html` 在命令后自动更新

---

## Phase B 退出标准

- [ ] 8 个 benchmark subjects 全部存在，manifest 结构合法
- [ ] `interpreter-runtime-perf` + `benchmark-comparison-aggregate` stage 注册
- [ ] `device_detector.py` 在 Windows 上返回有效 device info
- [ ] `benchmark_records.py` append/query 单元测试通过
- [ ] `run benchmark --dashboard` 生成 HTML，浏览器可打开，无 JS error
- [ ] `docs/benchmark/` 目录初始化完成

---

## 执行时的任务目录维护

每个 Batch 完成后:
1. 勾选 checkbox
2. 更新 `STATUS.md` current_focus + 最近摘要
3. 写 `notes/progress-v1-NN.md`
4. 全部完成后 INDEX.md 标记 completed，删除 ACTIVE.md
