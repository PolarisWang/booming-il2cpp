# Planning Tests Genericization Follow-up Design v1.01

Date: 2026-04-14
Status: proposed

## 1. 目标

把 planning 层测试重新分层，让 generic contract、fixture repo smoke、真实 retained subject 回归各自归位。

## 2. 非目标

- 不改 `subject_planner.py` 的对外 contract，除非在测试迁移过程中发现真实 generic 缺口。
- 不处理 registry / dashboard / benchmark command 的额外重构。
- 不在这次 follow-up 里继续扩展新的 subject 能力或 matrix。

## 3. 目标结构

### 3.1 Generic contract

- 文件：
  - [test_subject_planner.py](D:/agent/booming-il2cpp/tests/unit/planning/test_subject_planner.py)
- 只保留：
  - synthetic manifest
  - temp repo materialization
  - stagePlan / fingerprint / selection / artifact path contract
  - 不依赖真实 `subjects/*` 布局的 generic 行为

### 3.2 Fixture repo smoke

- 文件：
  - 新增 `tests/unit/planning/test_fixture_subject_planner.py`
- 只保留：
  - `clone_fixture_subject_repo(...)`
  - `MobileHelloWorldProof`
  - `EngineHostProof`
  - 这类通过 fixture subject repo 验证 planner 能消费“类真实 subject 目录”的 smoke

### 3.3 Retained real-subject planning integration

- 文件：
  - [test_solution_core_pack_planner.py](D:/agent/booming-il2cpp/tests/unit/planning/test_solution_core_pack_planner.py)
- 集中承载：
  - `SolutionCorePack` 默认 proof entry
  - benchmark entry / workload entry
  - declared-unit-test / declared-benchmark 选择
  - thin-manifest / sourceModel / entrySelection
  - archetype matrix -> `primaryProjectPath` / `entry` 映射

## 4. 建议的文件调整

### 4.1 测试文件

- [test_subject_planner.py](D:/agent/booming-il2cpp/tests/unit/planning/test_subject_planner.py)
  - 删除所有真实 `SolutionCorePack` 依赖
  - 如有必要，删除 fixture repo smoke
- `tests/unit/planning/test_fixture_subject_planner.py`
  - 新建，承载 fixture repo smoke
- [test_solution_core_pack_planner.py](D:/agent/booming-il2cpp/tests/unit/planning/test_solution_core_pack_planner.py)
  - 扩充，吸收当前散落在 `test_subject_planner.py` 的 retained real-subject planning 断言

### 4.2 测试支持

- [tests/support.py](D:/agent/booming-il2cpp/tests/support.py)
  - 保持 `make_temp_repo_root` / `materialize_subject_manifest` / `clone_fixture_subject_repo`
  - 如果迁移后出现重复 helper，再评估是否抽出 `tests/support/planning_fixtures.py`

### 4.3 生产代码

- [subject_planner.py](D:/agent/booming-il2cpp/build/toolchains/run/testing/subject_planner.py)
  - 预期不需要结构性修改
  - 仅在测试迁移暴露真正 contract 缺口时做最小修复

## 5. 迁移原则

1. generic file 不出现真实 retained subject 名字。
2. 真实 subject 行为断言只留在 dedicated retained planning suite。
3. fixture repo smoke 不冒充 generic contract test。
4. 优先移动测试，不主动改 planner 语义。
5. 如果要抽 helper，先抽“构造 fixture”而不是抽“断言语义”。

## 6. 验证策略

### 6.1 局部

- `python -m pytest tests/unit/planning/test_subject_planner.py`
- `python -m pytest tests/unit/planning/test_fixture_subject_planner.py`
- `python -m pytest tests/unit/planning/test_solution_core_pack_planner.py`

### 6.2 汇总

- `python -m pytest tests/unit/planning`
- `python -m pytest`

## 7. 风险与控制

### 风险 1

- 把 `SolutionCorePack` 断言从 generic file 搬走后，遗漏某些 archetype / declared-entry 回归。

控制：

- 在迁移前先列清单，迁移后由 `test_solution_core_pack_planner.py` 全量覆盖。

### 风险 2

- 新增 `test_fixture_subject_planner.py` 但 helper 仍混乱，结果只是把耦合换了文件。

控制：

- 文件职责以“数据来源”划分，不以“断言数量”划分。

### 风险 3

- 为了测试整理去改 planner 逻辑，导致功能回归。

控制：

- 默认不改生产代码；只有测试证明 planner contract 真有缺口时才做最小修复。

## 8. 设计结论

- 采用“三层分离”：
  - generic contract
  - fixture repo smoke
  - retained real-subject planning integration
- 下一步进入 `writing-plans`，写一份单次执行计划，然后按 TDD 迁移 tests 并回跑全量验证。
