# Contract 根目录去 Subject 内容化 Brainstorm

## 背景

用户要求很明确:

- `contracts/` 目录中不允许再放 subject 下的具体内容

当前仓库里真实冲突点不是 schema 中存在 `subjectId` 字段，而是 `contracts/` 已经混入了 concrete fixture，尤其是 `HelloWorldObject` 一类 first-proof 内容。当前污染路径主要包括:

- `contracts/artifacts/v0/samples/*.min.json`
- `contracts/artifacts/v0/snapshots/*.snapshot.json`
- `contracts/examples/v0/artifacts/*.json`
- `contracts/native/v0/samples/**/*`
- `contracts/native/examples/v0/**/*`

同时，这些路径已经被多处 active consumer 依赖:

- `build/toolchains/run/testing/contracts.py`
- `build/toolchains/run/testing/path_resolver.py`
- `build/toolchains/run/testing/subject_workers.py`
- `build/toolchains/run/run_manifest.json`
- `tests/unit/run/test_repo_layout.py`
- `tests/unit/run/test_path_resolver.py`
- `tests/unit/run/test_subject_contracts_source_cutover.py`
- `tests/tooling/run/test_python_unittest_command.py`

所以这不是一次“只改文档说法”的调整，而是 contract root 边界重画。

## 目标

把仓库里的三类东西彻底分开:

1. 正式 contract 定义
2. shared validation fixture
3. subject 自己的输入 / expected / baseline / validation

其中:

- `contracts/` 只承担第 1 类
- 当前混在 `contracts/` 里的 concrete proof/sample/snapshot/native example 要迁出
- 迁移后目录语义必须能被路径解析器、测试命令、活文档和 repo layout test 一致表达

## 备选方案

### 方案 A: 把 concrete fixture 迁到 `tests/contracts/analysis/v0/**`

建议布局:

```text
contracts/
  artifacts/v0/schemas/
  native/v0/*.h
  docs/v0/*.md

tests/contracts/
  analysis/v0/samples/*.min.json
  analysis/v0/snapshots/*.snapshot.json
  analysis/v0/native/abi/*.cpp
  analysis/v0/native/bridge/*.cpp
  trace/...
```

优点:

- `contracts/` 语义最干净，只保留 formal definition
- fixture 归属于测试与验证侧，符合其真实用途
- `run test contract ...`、`validate_analysis_contracts()`、文档示例都可以共享同一套 test-owned canonical fixture
- 与现有 `tests/contracts/trace/**` 方向一致，便于后续统一 contract validation 资产

缺点:

- 要同步改 `path_resolver`、测试、活文档和 run manifest
- 需要重新定义 “canonical snapshot baseline” 的口径，不再写成 `contracts/...`

### 方案 B: 把 concrete fixture 迁到 `subjects/<subject-id>/...`

示例:

```text
subjects/HelloWorldObject/contracts/v0/...
```

优点:

- 目录所有权很直观，谁的 fixture 谁负责
- 对单 subject proof 研发来说容易理解

缺点:

- 当前这些资产并不只是 subject 私有输入，而是 shared contract validation fixture
- generic contract check 会被某个 subject 目录结构绑死
- 以后如果主线 subject 从 `HelloWorldObject` 切到别的 subject，contract canonical 路径会变得不稳定

### 方案 C: 继续保留在 `contracts/`，但改成“只允许 generic sample，不允许 subject sample”

优点:

- 迁移成本最低

缺点:

- 规则执行成本高，边界模糊
- 当前 sample/snapshot/native sample 本身已经是 concrete proof 资产，不适合继续留在 `contracts/`
- 这会把“定义”和“夹具”继续混在同一棵树里，后续还会反复污染

## 推荐

选择方案 A。

核心理由:

- 这次真正要修的是“contract 定义根被 fixture 污染”，不是换个名字继续共存。
- 当前 sample/snapshot/native example 的真实职责是 shared validation fixture，不是 subject source，也不是 formal contract definition。
- `tests/contracts/analysis/v0/**` 能稳定表达 “analysis contract 的 canonical fixture 根”，同时让 `contracts/` 回到单一职责。

## 冻结结论

- `contracts/` 后续只保留:
  - `contracts/artifacts/v0/schemas`
  - `contracts/native/v0/*.h`
  - `contracts/docs/v0/*.md`
- 下列路径不再作为 canonical 根:
  - `contracts/artifacts/v0/samples`
  - `contracts/artifacts/v0/snapshots`
  - `contracts/examples/v0`
  - `contracts/native/v0/samples`
  - `contracts/native/examples/v0`
- analysis contract 的 shared fixture 新 canonical 根改为:
  - `tests/contracts/analysis/v0/samples`
  - `tests/contracts/analysis/v0/snapshots`
  - `tests/contracts/analysis/v0/native/abi`
  - `tests/contracts/analysis/v0/native/bridge`
- `subjectId` 字段不需要因为这次目录迁移而删除；问题在于存储位置，不在于 schema 内容。
- `tests/contract/**` 作为遗留别名不再继续扩散，实现时应优先统一到 `tests/contracts/**`。
