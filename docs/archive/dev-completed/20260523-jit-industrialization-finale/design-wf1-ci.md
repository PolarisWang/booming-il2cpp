# wf1-ci: JIT CI 性能基线设计

> task_id: wf1-ci
> parent: 20260523-jit-industrialization-finale
> status: design

## 目标

在现有 codegen-regression.yml 基础上，为 JIT 编译产物建立性能基线（per-commit 可对比），实现：

1. 每次 PR 自动运行 JIT benchmark 并记录耗时
2. 与历史基线对比，超过阈值自动标记回归
3. 基线数据可审计、可追溯

## 设计方案

### 现有 CI 状态

codegen-regression.yml 已覆盖：
- PR + push 触发
- RelWithDebInfo 构建
- 13 套 JIT 功能测试（199 tests）
- Foundation-DLL CI smoke

**缺失**：性能指标收集与基线对比。

### Benchmark 选型

使用现有 `test_jit_bench` 目标（在 `testing/src/native/jit/` 中），它是一个 Google Benchmark 套件。关键指标：

| 指标 | 来源 | 单位 | 说明 |
|------|------|------|------|
| JIT 编译耗时 | test_jit_bench | ms/method | 每条方法的 JIT 编译时间 |
| 生成代码大小 | code_buffer 统计 | bytes/method | 输出的 native code 大小 |
| TLAB 分配路径 | NewObj/Box/NewArr 内联 | cycles | 内联 TLS 访问延迟（RDTSC） |
| GC 槽映射大小 | gc_slot_map | entries/method | GcSlotMapV0 条目数 |
| 核心测试耗时 | test_jit_native 68 tests | ms | 完整回归耗时 |

### 基线存储策略

**方案推荐：GitHub Artifact + JSON 历史对比**

```
.github/baselines/
├── jit-compile-time.json      # 编译耗时基线（每个 method 的 P50/P95）
├── jit-code-size.json         # 代码大小基线
└── jit-test-timing.json       # 测试套件耗时基线
```

对比逻辑：
1. 每次 CI 运行在 `RelWithDebInfo` 下跑 `test_jit_bench` + `test_jit_native`
2. 输出 JSON 格式的性能报告
3. 与 `main` 分支的最新基线对比
4. 超过阈值（编译耗时 +10%，代码大小 +5%）时在 PR 注释中告警

### 阈值设定（初始值）

| 指标 | 告警阈值 | 严重阈值 |
|------|---------|---------|
| JIT 编译耗时 (mean) | +10% | +25% |
| 生成代码大小 | +5% | +15% |
| 测试套件耗时 | +15% | +30% |

### GitHub Actions 集成

在现有 `codegen-regression.yml` 中新增 job（或扩展步骤）：

```yaml
jit-baseline:
  runs-on: windows-latest
  needs: jit-regression  # 依赖构建完成

  steps:
    - run: ctest --test-dir testing/artifacts/run/native-test-build -R "test_jit_bench" --output-on-failure
    - run: python scripts/ci/collect-jit-metrics.py  # 解析 benchmark 输出 → JSON
    - run: python scripts/ci/compare-baseline.py --baseline .github/baselines/ --current jit-metrics.json
    # 如果超过严重阈值 → 创建 PR 注释 + label "perf-regression"
```

### 文件变更清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `.github/baselines/jit-compile-time.json` | 新增 | 初始基线（从 main 最新一次 CI 生成） |
| `scripts/ci/collect-jit-metrics.py` | 新增 | 解析 benchmark JSON 输出 → 指标报告 |
| `scripts/ci/compare-baseline.py` | 新增 | 与基线对比 → 回归告警 |
| `.github/workflows/codegen-regression.yml` | 修改 | 新增 jit-baseline job |

### 首次基线建立

1. 在 main 分支手动触发一次 full benchmark run
2. 输出保存为 `.github/baselines/` 下的 JSON
3. 以后每次 PR 自动对比该基线
4. 基线每季度或重大架构变更后手动更新

## 风险评估

| 风险 | 影响 | 缓解 |
|------|------|------|
| CI runner 性能波动导致误报 | 中 | 使用 P50 而非 mean，同一 runner 类型 |
| benchmark 耗时过长 | 中 | 只选关键子集，全量 nightly 运行 |
| 基线维护成本 | 低 | 自动化脚本管理，无需人工干预 |

## 退出条件

1. PR CI 自动运行 test_jit_bench 并输出 JSON 报告
2. 基线 JSON 文件已入库（`.github/baselines/`）
3. 回归告警脚本可正确检测 +10% 阈值
4. 回归告警以 PR comment 形式可见
