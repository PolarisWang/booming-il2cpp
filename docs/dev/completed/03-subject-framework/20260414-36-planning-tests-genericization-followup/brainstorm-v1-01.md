# Planning Tests Genericization Follow-up Brainstorm

Date: 2026-04-14
Status: proposed

## 用户目标

在上一条 workspace/test-entry roadmap 完成后，继续推进下一步，把 `planning/tests` 这一层和真实 retained subject 脱钩，避免 `test_subject_planner.py` 继续承担真实 `SolutionCorePack` 行为回归。

## 当前观察

### 代码现实

- [subject_planner.py](D:/agent/booming-il2cpp/build/toolchains/run/testing/subject_planner.py) 本身已经基本是 generic contract：
  - 输入是 `repo_root + subject.manifest.json`
  - 输出是 `selection/artifactsRoot/stagePlan`
  - 不包含对 `SolutionCorePack` 的硬编码
- [test_subject_planner.py](D:/agent/booming-il2cpp/tests/unit/planning/test_subject_planner.py) 目前混了三类测试：
  - synthetic manifest contract tests
  - fixture subject repo smoke，例如 `MobileHelloWorldProof` / `EngineHostProof`
  - 真实 `SolutionCorePack` / archetype / declared-entry / thin-manifest 断言
- [test_solution_core_pack_planner.py](D:/agent/booming-il2cpp/tests/unit/planning/test_solution_core_pack_planner.py) 已经是一个真实 retained subject planning suite，但覆盖范围还不够，导致 `test_subject_planner.py` 里继续夹带真实 solution 断言。

### 现在的问题

1. `test_subject_planner.py` 名义上是 unit/planning generic suite，实际上仍对真实 retained subject 有强耦合。
2. 真实 `SolutionCorePack` 行为断言散落在两个 planning 文件里，职责边界不清楚。
3. synthetic manifest helper 很多，但没有清晰的“generic contract test”与“fixture/integration smoke”分层。

## 方案对比

### 方案 A：仅做最小搬运

- 做法：
  - 只把 `SolutionCorePack` 相关测试从 `test_subject_planner.py` 搬到 `test_solution_core_pack_planner.py`
  - 其余 fixture smoke 继续留在 `test_subject_planner.py`
- 优点：
  - 改动最小，见效最快
- 缺点：
  - `test_subject_planner.py` 仍然混杂 synthetic manifest 与 fixture repo smoke
  - generic suite 的语义仍不干净

### 方案 B：按测试层级重排，明确三层职责

- 做法：
  - `test_subject_planner.py` 只保留 synthetic manifest contract tests
  - 新增 `test_fixture_subject_planner.py`，承载 fixture subject repo smoke
  - 扩充 `test_solution_core_pack_planner.py`，承载全部真实 `SolutionCorePack` / archetype / declared-entry / thin-manifest planning 断言
  - 如有必要，抽 `tests/support/planning_fixtures.py` 复用 builder/helper
- 优点：
  - generic / fixture / retained-integration 三层边界清晰
  - 最符合这次 follow-up 的目标
  - 以后继续 generic 化时，不会再把 retained subject 行为回填进 generic suite
- 缺点：
  - 需要同步整理 helper 与文件命名
  - 需要一次性改多份 planning tests

### 方案 C：把真实 retained subject 断言全部移出 planning 层

- 做法：
  - `tests/unit/planning/*` 只留 generic manifest tests
  - 真实 `SolutionCorePack` 规划回归全部依赖更高层的 integration/tooling tests
- 优点：
  - planning 层最“纯”
- 缺点：
  - 会损失 planner 这一层对真实 solution/archetype 映射的直接回归
  - 出问题时定位会更差

## 推荐

- 选方案 B。

理由：

1. 它既能把 `test_subject_planner.py` 变回真正的 generic contract suite，又不会丢掉 `SolutionCorePack` 这一层真实 planner 回归。
2. 它和当前仓库已经形成的测试分层一致：
   - generic unit contract
   - fixture smoke
   - retained real-subject integration
3. 这条 follow-up 更像一次单次收口的测试结构整理，不需要再开大 roadmap，后续直接进入 `writing-plans` 即可。

## 分流判断

- 本任务建议进入 `writing-plans`，不再开新 roadmap。

理由：

- 范围集中在 `tests/unit/planning/*`、`tests/support.py` 以及少量 planner 测试 helper；
- 不涉及新的多阶段产品结构改造；
- 目标是一次性完成分层、迁移、验证和收口。
