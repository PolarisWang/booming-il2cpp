# CRAG GC 可靠性验证补全 — 测试安置与框架结合设计

> 目标：回答"A+B+C+D 都要做时，测试放哪、怎么和现有测试框架结合"。
> 依据：已通读 根 `CMakeLists.txt`、`cmake/chaos_native_test.cmake`、`tests/contracts/native/runtime-core/CMakeLists.txt`、`tests/unit/runtime-native/.../gc/CMakeLists.txt`、`tests/suite_contract.yaml`、`tests/runner/adapters/native.py`、`tests/runner/test_driver.py`。

---

## 〇、先厘清：项目现有"三张 native 测试面"（这是安置的坐标系）

| 面 | 目录 | 框架 | 目标前缀 | 标签机制 | 层归属 |
|----|------|------|---------|---------|--------|
| **A: contracts 集成层** | `tests/contracts/native/runtime-core/` | **ad-hoc** TEST/CHECK 宏 + 自有 main（`WITHOUT_GTEST`） | `chaos_gc_*` | 文件名自动派生 `unit/stress/benchmark`；`RESOURCE_LOCK bgc` | `integration` 层（`test_driver --layer integration`） |
| **B: gtest 单元/压力/基准层** | `tests/unit/runtime-native/runtime-core/gc/` | **GoogleTest**（`gtest_main`/`GTEST_LIB_ONLY`），`gc_test_stubs.cpp` 提供 codegen 符号 stub、`gc_test_base.cpp` 共享 fixture | `test_gc_*` | 显式 `LABELS "unit;gc"` / `"stress;gc"` / `"benchmark;gc"` | 同 ctest build 目录（`windows-x64-reference`），并入 ctest 集合 |
| **C: e2e/foundation-dll 管线** | `tests/e2e/verification/` + `testing/foundation-dll` | Python 编排 + codegen 产物 | — | chunk pipeline | `e2e` 层 |

**关键事实**：
- 两张 native 面都注册进**同一个 ctest**（`artifacts/presets/windows-x64-reference`），共用 `chaos_native_add_test()` 工厂（`cmake/chaos_native_test.cmake`）。
- 工厂 `chaos_native_add_test(name SOURCES [LIBS] [LABELS] [RESOURCE_LOCK] [WITHOUT_GTEST] [GTEST_LIB_ONLY] [CXX_STANDARD] [INCLUDES])`：
  - 默认 `gtest_main`；`WITHOUT_GTEST`=裸 exe（ad-hoc 用）；`GTEST_LIB_ONLY`=自定义 main 但链 gtest（取 include/宏）。
  - 统一链接 `chaos_test_libs_v0`（= runtime-core/bootstrap/interpreter/jit/…全部），自动附 `gc_test_stubs.cpp`（codegen 符号 stub）。
- 压力区分：`LABELS "stress"/"benchmark"` 测试被 native adapter `-LE benchmark\|stress\|soak` **排除在普通门禁**；`CHAOS_GC_TEST_STRESS_ONLY=1` 切换为 `-L` **只跑压力层**（已有通道）。
- `tests/contracts/native/gc/` 是**空占位**（注释：真正 GC target 在 runtime-core 测试目录）；`tests/unit/runtime-native` 是 gtest 的承接根。

---

## 一、测试安置：A/B/C/D 各放哪里（明确目录 + 命名 + 目标）

### 方案 A — 可复现 native 门禁基线
- **改/修**（不新增目录）：
  - `tests/contracts/native/runtime-core/`：修 `chaos_gc_bgc_root_scan_test`（挂起）与 `gc_stress_test` 的路径/场景；这些是 ad-hoc 面。
  - 新增独立 **gtest 收敛性/可复现性**目录 `tests/unit/runtime-native/runtime-core/gc/baseline/`，目标 `test_gc_baseline_*`：把"每场景真实次数 + duration"固化为 gtest 断言 + 写 `baselines/gc.perf.yaml`（回归 diff 低线）。
- **接线脚本**：`scripts/ci_test.py` 增加 `--preset native-gc`（调 driver `--layer integration --group <gc 新组>`，开 `--cases`），报告回本仓库。

### 方案 B — 修复并强化压力层（声明=实现）
- **都在现有面改**，不新建目录：
  - contracts 面 `gc_stress_test.cpp` / `gc_bgc_stress_test.cpp`：改参数（`kFWorkers`/`kFAllocsPerThread` → `hardware_concurrency` 动态核数）、恢复 Scenario G 被注掉的后台 GC 线程、去 `--new-only` 规避、报告路径改回本仓。
  - gtest 面 `gc_bgc_smoke_test.cpp` / `gc_poh_test.cpp` / `gc_handle_test.cpp` / `gc_finalizer_test.cpp`：去掉"ForceComplete 跳过/避免直接收集/软断言"，升级为真实 interleaved + 语义硬断言。
- **压力门禁**：`suite_contract.yaml` 增一个 `gc-stress` group（`ctest_exclude` 置空、`ctest_config` 指定）或用现有 `CHAOS_GC_TEST_STRESS_ONLY`，接 CI nightly workflow。

### 方案 C — 独立性/交叉验证（.NET 语义对齐 + 可追审验收）
- **新增纯 gtest 目录**：`tests/unit/runtime-native/runtime-core/gc/semantics/`，目标 `test_gc_semantics_*`（WeakReference/DependentHandle/Ephemeron/GCHandle pin/finalize 顺序的 .NET 语义 oracle）。
- **新增独立可执行验收包**：`tests/contracts/native/gc/`（当前是空占位，正好用它）建 `gc_acceptance_*`，用 `chaos_native_add_test(... WITHOUT_GTEST)` 产出自带 main 的裸可执行验收器 + 一份 `acceptance-checklist.md`（判定口径）。
- **兼容矩阵**：`tests/e2e/verification/gc/`（或 `docs/gc-semantic-compat.md`）固化差异判定记录。

### 方案 D — 真实负载 + 可观测基线
- **新增**：`tests/unit/runtime-native/runtime-core/gc/load/`，目标 `test_gc_load_*`（贴近生产负载的单一长跑 gtest）+ `test_gc_observability_*`（低内存/堆上限/emergency reserve/暂停直方图断言）。
- **基准（benchmark 面）**：在 `tests/unit/runtime-native/runtime-core/gc/` 扩 `test_gc_throughput_benchmark` / `test_gc_regression_benchmark`（已有），加 perf P50/P95/P99 落库；补 `GCMemoryInfo` 字段真实赋值后校验。

---

## 二、怎么和现有框架结合（精确到工厂函数 / 契约字段）

### 结合点 1：新增测试 = 写 `.cpp` + 在对应 CMakeLists 加一行 `chaos_native_add_test(...)`，**不改工厂**
- 统一走 `cmake/chaos_native_test.cmake` 的 `chaos_native_add_test`（自动链 `chaos_test_libs_v0` + `gc_test_stubs.cpp` + 统一 include/flags）。
- 需要共享 fixture → `gc_test_base.cpp`（已在 `tests/unit/runtime-native/runtime-core/gc/`）；需要真实源而非 prebuilt → 直接 `chaos_native_add_test(... ; ../../../../src/native/.../gc_xxx.cpp)` + `/FORCE:MULTIPLE`（项目已有此模式）。
- gtest（推荐给 C/D）用默认 `gtest_main`；裸可执行验收器（A 的部分 + C 的 acceptance）→ `WITHOUT_GTEST` 或 `GTEST_LIB_ONLY`。

### 结合点 2：标签驱动"门禁/门槛"分层（不靠 skip）
| 标签 | 放哪些 | 何时跑 |
|------|-------|-------|
| `unit;gc` | A 收敛性、C 语义、D 可观测 | **PR/普通门禁**（`-LE` 之外） |
| `stress;gc` | B 全部压力（含 BGC/交叠） | **nightly + on-demand**（`-L` 或 `--preset gc-stress`） |
| `benchmark;gc` | D perf 基线 | nightly，落 `gc.perf.yaml` |
| `soak` | 72h 长跑 | 夜间/周末，独立 |

对每条新增测试**必须写 `LABELS`**，并遵守 no-skip：失败登记进 `tests/runner/baselines/known-failures.integration.yaml`（修后删除）。**修掉 `chaos_gc_bgc_root_scan_test` 前，全局门不会全绿**——这是 A 的第一件事。

### 结合点 3：driver / CI / daily 接线
- 报告落库：driver `--cases` 产出 per-case，写 `tests/runner/test-report.json`（native 层自此有明细）。
- 新增 CI preset（`scripts/ci_test.py`）：
  - `--preset native-gc`（快速 GC 组，`unit;gc`）
  - `--preset gc-stress`（`CHAOS_GC_TEST_STRESS_ONLY=1` + `-L stress|soak|benchmark`）
  - nightly workflow 调 `test_driver.py --layer integration --group <gc>` 落 `artifacts/gc-stress-reports/<date>.json`。
- 基线：新 `tests/runner/baselines/gc.perf.yaml`；加 `gc_semantic_compat.yaml` 存 C 的兼容判定。

---

## 三、推荐"安置-框架"对应总表（交付即落地）

| 方案/阶段 | 目录 | 目标名 | 框架 | 标签 | 接线 |
|------|------|--------|------|------|------|
| A1 修挂起/路径 | `tests/contracts/native/runtime-core/` | 改 `chaos_gc_*` | ad-hoc | unit/stress | 现有 |
| A2 收敛性+基线 | `tests/unit/runtime-native/runtime-core/gc/baseline/` | `test_gc_baseline_*` | gtest | `unit;gc` | `native-gc` preset |
| B 压力+断言修复 | 现有两文件 | 改 `chaos_gc_stress`/`test_gc_*` | ad-hoc/gtest | `stress;gc` | `gc-stress` preset + nightly |
| C 语义/验收 | `…/gc/semantics/` + `tests/contracts/native/gc/` | `test_gc_semantics_*` + `gc_acceptance_*` | gtest + bare | `unit;gc` | 普通门禁 + 文档 |
| D 负载/可观测/基准 | `…/gc/load/` + 现有 benchmark | `test_gc_load_*`/`test_gc_observability_*` | gtest | `unit;gc`/`benchmark;gc` | nightly + `gc.perf.yaml` |

---

## 四、关键决策点
1. **优先用 gtest 面（`tests/unit/runtime-native/runtime-core/gc/`）**作为 A2/C/D 的承接：它有 `gc_test_base.cpp`/`gc_test_stubs.cpp` 基础设施、成熟 /FORCE:MULTIPLE 源码编译模式、标签体系完整。ad-hoc 面（contracts）只用来**修**（A1/B 的存量压力与已知挂起）。
2. **不改 `chaos_native_test.cmake` 工厂**——一切通过四个参数（`LABELS`/`RESOURCE_LOCK`/`WITHOUT_GTEST`/`LIBS`）+ 行级 `chaos_native_add_test(...)` 完成，避免侵入共享机制。
3. **压力必须走独立门禁**（`stress` 标签 + nightly），不能塞回普通 PR 门禁——否则 PR 被数小时压力拖垮。
4. **先做 A1**（修 `chaos_gc_bgc_root_scan_test` 挂起 + `gc_stress` 报告路径），否则后续任何"全绿/可复现"声明都不成立。用 `native-contracts` preset 实跑一次拿到第一份含 GC 的 native 报告。

---

> 一句话：**A1 在 contracts ad-hoc 面修，A2/C/D 新测试落在 `tests/unit/runtime-native/runtime-core/gc/` 的 gtest 面（新建 baseline/、semantics/、load/ 子目录），B 在两处存量面原位修复；全部经 `cmake/chaos_native_test.cmake` 工厂 + `LABELS` 标签 + `suite_contract.yaml`/`ci_test.py` 接线，做到"新增测试不改工厂、压力独立门禁、基线可复现、报告落本仓"。**
