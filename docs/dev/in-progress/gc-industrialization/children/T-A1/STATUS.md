# T-A1 — Stress 测试进 CI 门禁

## Scope

将 GC stress/压力测试从「仅 nightly」提升为「per-PR 门禁」，移除 `-LE "benchmark|stress|soak"` 对关键并发正确性测试的排除，确保并发 bug 在合并前被拦截，而非推迟到 nightly 甚至被静默吞掉。

## Inputs

- `.github/workflows/gc-ci.yml` — 现行 per-PR 门禁（`-R "chaos_gc_|test_gc_" -LE "benchmark|stress|soak"`）
- `.github/workflows/gc-stress-nightly.yml` — nightly 压力层（`--stress-only`）
- `tests/suite_contract.yaml` `contracts-native-gc`（`ctest_exclude: benchmark|stress|soak`）
- `tests/runner/baselines/known-failures.integration.yaml` — 已知失败基线
- `tests/runner/test_driver.py` — 已含 `stale_known`/`ghost_known` 失效检测

## 现状调查结论（关键）

**部分名义范围已在仓库中完成**（review 报告部分过时）：
- ✅ `|| true` 已移除（注释明确 "no `|| true`"、"REDS on any failure"）
- ✅ A2b `chaos_gc_region_barrier_stress_test` 已提升进 fast unit gate（labels 覆盖为 "unit"）
- ✅ ASAN nightly 已存在（`gc-asan-nightly`）
- ✅ Server GC smoke 已存在（`gc-server-smoke`，失败即 RED）
- ✅ Nightly stress 已并入 `gc-stress-nightly.yml` + known-fail 对账（`stale_known` 过期检测已在 `test_driver.py`）
- ✅ known-fail 带失效检测已实现（`stale_known` / `ghost_known`）

**真实剩余缺口**：
1. **较重 stress 层仍被 per-PR 排除**——`test_gc_general_stress` / `test_gc_loh_stress` / `test_gc_delegate_stress` 仅 nightly 跑，合并前并发 bug 可能被静默放过
2. **ASAN 仍 nightly-only**（此缺口主要归 T-A2，但 T-A1 需确认能否低成本提升部分关键 stress 进 per-PR）
3. 已知 fail 的 stress（std::mutex arch limit）**不能**无差别 gate per-PR——需区分「真并发正确性 stress」（必须 gate）与「架构极限 stress」（保持 nightly）

## Expected Outputs

- 修改后的 `.github/workflows/gc-ci.yml`：把「并发正确性关键 stress」纳入 per-PR（低 SCALE 安全下限），保留「架构极限 stress」在 nightly
- 修改后的 `tests/suite_contract.yaml`（如 `ctest_exclude` 需调整）
- 新增/更新的 GC stress 目标 label 分类
- `T-A1` 子任务完成报告

## Exit Criteria

- 至少一个「真并发正确性 stress」测试在 per-PR 门禁中运行并 `REDS on failure`（非 `|| true`）
- 已知架构极限 stress 仍被正确归类在 nightly，不造成 per-PR 长期 flaky
- 跑通 `python tests/runner/test_driver.py --layer unit` 确认无回归
- 提交包含 `root_cause` / `fix_strategy` / `regression_check` 三段式 message

## Terminal Notes

（执行终端填写）

## 元信息

- `parent_task_id`: gc-industrialization
- `source_relation: roadmap-child`
- `clearance_source: parent-roadmap`
- `entry_skill`: `dev-writing-plans`（先最小 preflight，再执行）
