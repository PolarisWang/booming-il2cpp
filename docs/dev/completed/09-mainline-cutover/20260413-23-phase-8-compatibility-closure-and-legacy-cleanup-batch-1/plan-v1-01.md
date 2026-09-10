# Phase 8 Compatibility Closure And Legacy Cleanup Batch 1 实施计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development` 与 `dev:verification-before-completion` 执行本计划。  
> 目标：冻结 Phase 8 cleanup inventory，优先收口 `tests/` taxonomy 与低风险 legacy wrapper，再为 subject shell 资产迁移建立稳定批次边界。  
> 设计文档：`docs/dev/in-progress/20260412-06-test-system-suite-redesign/design-v1-03.md`

---

## 计划任务

- [ ] Task 1: 冻结 cleanup inventory 与批次边界
  - 范围：
    - 盘点 `tests/proof`、`tests/perf`、`tests/platform`、`tests/gate`、`tests/registry` 的真实内容与运行期依赖
    - 盘点 `tests/unit/run` 中所有 `test_phase*` / `test_stage*` 文件，区分：
      - 纯 shim
      - 真实测试
    - 盘点 legacy proof / benchmark / golden shell roots 中仍然存活的：
      - `expected/`
      - `baselines/`
      - `compatibility.redirectToSubject`
    - 记录 `tests/perf` 与 shell subject 的 active runtime usage
  - 验收：
    - 明确哪些目录可在 Batch 1 直接收口
    - 明确哪些目录必须延后到后续 batch

- [ ] Task 2: 重命名并迁移 `tests/unit/run` 中的 phase/stage 实测文件
  - 范围：
    - 把非 shim 的 `test_phase*` / `test_stage*` 文件迁移到功能分桶目录
    - 文件名改为按功能命名，不再带 phase/stage 前缀
    - 更新 `tests/unit/run/test_repo_layout.py` 的布局断言
    - 保持 `tests/unit/run/` 只承载：
      - `test_repo_layout.py`
      - 必要的 legacy shim 入口
  - 验收：
    - `tests/unit/run/` 不再保留真实 phase/stage 测试实现
    - pytest collection 与相对路径计算保持稳定

- [ ] Task 3: 收口低风险 legacy test roots
  - 范围：
    - 清理只剩 wrapper / guidance 的顶层 legacy 目录
    - 若 `tests/gate` / `tests/platform` 仅为 CMake wrapper，则迁入 canonical doc/fixture 路径或直接退役
    - 若 `tests/proof` / `tests/registry` 只剩 README / props，则迁入更合适的 canonical 位置后退役
    - 对 `tests/perf` 保持审慎：只有在 active runtime dependency 消除后才允许退役
  - 验收：
    - 已收口的 legacy test roots 不再出现在 repo layout 合同中
    - 未收口的根目录会在 STATUS/notes 中明确说明原因与后续批次归属

- [ ] Task 4: 冻结后续 batch 的 subject shell 资产迁移计划并完成验证
  - 范围：
    - 记录 legacy proof / benchmark / golden shell roots 的剩余资产类型
    - 识别需要迁移到 canonical subject 的 expected / baselines / alias 资产
    - 运行定向测试与全量测试
    - 更新 STATUS / ACTIVE / 索引 / 父 roadmap 状态
  - 验收：
    - Phase 8 后续工作不再是“盲删目录”
    - 本批验证通过，父 roadmap 的剩余工作边界清晰可执行

## 当前冻结的 inventory 结论

- `tests/proof` 当前只有：
  - `README.md`
  - `Directory.Build.props`
- `tests/registry` 当前只有：
  - `README.md`
- `tests/platform` / `tests/gate` 当前各保留 3 个 `CMakeLists.txt` wrapper
- `tests/perf` 当前仍有 active 内容：
  - `convert-golden-multi-project/check.py`
  - `convert-golden-multi-project/baselines/windows.json`
- `build/toolchains/run/testing/perf.py` 仍直接从 `tests/perf/<suite>/baselines/<platform>.json` 读取 perf baseline
- `tests/unit/run` 当前仍有 30+ 个非 shim 的 `test_phase*` / `test_stage*` 真实测试文件
- `subjects/` 顶层仍保留大量 legacy proof / benchmark / golden shell roots，且其中不少仍有 `expected/` 或 `baselines/` 资产

## 关键改动文件

- `tests/unit/run/test_repo_layout.py`
- `tests/unit/run/*.py`
- `tests/unit/compatibility/*.py`
- `tests/unit/planning/*.py`
- `tests/unit/performance/*.py`
- `tests/integration/platform-hosts/**`
- `tests/proof/**`
- `tests/platform/**`
- `tests/gate/**`
- `tests/registry/**`
- `docs/dev/**`

## 验证

- `python -m pytest tests/unit/run/test_repo_layout.py -q`
- `python -m pytest tests/unit/planning/test_subject_planner.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/compatibility/test_subject_manifest_schema.py -q`
- `python -m pytest tests/unit tests/integration -q`
- `python -m pytest -q`
