# Progress v1.01

日期：2026-04-13 05:58:37 +08:00

## 本轮启动

- 承接 Phase 7 Batch 1 的归档结果，开始 Phase 8。
- 本批先冻结三类残量：
  - `tests/` legacy 顶层目录
  - `tests/unit/run` 中的 phase/stage 命名真实测试
  - legacy proof / benchmark / golden shell roots 中仍存活的 expected / baseline / redirect 资产

## 首轮 inventory

- `tests/proof` 当前只剩 guidance 文件：
  - `README.md`
  - `Directory.Build.props`
- `tests/registry` 当前只剩 `README.md`
- `tests/platform` 与 `tests/gate` 当前只剩 3 个 `CMakeLists.txt` wrapper
- `tests/perf` 仍有 active 内容：
  - `convert-golden-multi-project/check.py`
  - `convert-golden-multi-project/baselines/windows.json`
- `build/toolchains/run/testing/perf.py` 仍直接读取 `tests/perf/<suite>/baselines/<platform>.json`
- `tests/unit/run` 仍有 30+ 个非 shim 的 `test_phase*` / `test_stage*` 真实测试文件
- `subjects/` 顶层仍有大量 legacy proof / benchmark / golden shell roots；其中多项仍保留 `expected/`、`baselines/` 或 `compatibility.redirectToSubject`

## 批次边界判断

- Batch 1 可优先处理：
  - `tests/unit/run` phase/stage 文件重命名与分桶
  - 纯 wrapper / 纯 guidance 的 legacy test roots
- 后续 batch 处理：
  - `tests/perf` 基线入口迁移
  - proof / benchmark / golden shell roots 的 expected / baseline 资产迁移
  - compatibility redirect 生命周期最终退役
