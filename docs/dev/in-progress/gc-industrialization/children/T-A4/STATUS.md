# T-A4 — Release 性能基线（P99/阶段分解/对比 CoreCLR）

## Scope

建立 GC 性能基线。当前基线（`gc.perf.yaml`）全部为 `null`（`pending-first-green-run`），性能回归比较器处于惰性状态。本任务产生第一个绿色运行的可信基线。

## 现状调查（结论：基建已 100% 就绪）

**`gc-stress-nightly.yml` 已完整实现所有基线捕获基建**：
- ✅ `collect-gc-metrics.py`（解析 `BENCH,` 行 → 桥接至 `gc.perf.yaml`）
- ✅ `compare-baseline.py`（JIT 已有，GC 复用，阈值 compare）
- ✅ opt-in baselining 步骤（`update_baseline=true` 手动 dispatch 时填充 + 提交基线）
- ✅ 自动 nightly 从不 mutate 基线（只有手动 dispatch 才写）
- ✅ `machine: "pending-first-green-run"` 待填充

**T-A4 唯一剩余缺口**：一次真实的绿色 run 来填充基线值。

## ⚠️ 无法在沙箱完成

填充基线需要**完整 Release 构建 + CI 环境**（释放真实 benchmark 数字），无法在本会话沙箱中可靠执行。这是一个「待真实 CI 捕获」的任务，非代码实现任务。

## 手动执行指令（供真实 CI 触发）

1. **手动 dispatch** `gc-stress-nightly` workflow
2. 设置 input `update_baseline: true`
3. 该 run 会：
   - 开启基准构建 + 运行 `test_gc_gen1_benchmark` / `test_gc_memory_domain_benchmark`
   - `collect-gc-metrics.py --update-baseline` 填充 `gc.perf.yaml`
   - `git commit + push` 填好的基线
4. 验证 `gc.perf.yaml` 中 `machine:` / `captured_at:` 已设置，`p50/p95/p99` 非 null

## 关键决策

- **对比 CoreCLR**：推荐「自我基线 + 定性对齐」，不强制数值内联 CoreCLR 基准（P4 阶段再做精确对标）
- **Release 构建**：Debug 的 ~130ms Full GC 不具代表性；基线应为 Release 数值

## Expected Outputs

- `gc.perf.yaml` 基线已填充（非 null，`machine`/`captured_at` 设置）— 待真实 CI
- 回归比较器已在 nightly 中运行（continue-on-error）

## Exit Criteria

- 一次手动 dispatch 已填充 `gc.perf.yaml`（非 null）
- 回归比较器正确运行

## 状态
**READY-FOR-CI**：基建就绪，等待真实 CI 手动 dispatch `update_baseline=true`。非代码阻塞，无本地可执行项。

## 元信息

- `parent_task_id`: gc-industrialization
- `source_relation: roadmap-child`
- `clearance_source: parent-roadmap`
- `entry_skill`: `dev-writing-plans`