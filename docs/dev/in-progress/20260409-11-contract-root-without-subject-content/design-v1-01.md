# Contract 根目录去 Subject 内容化设计

## 1. 设计目标

本次设计只解决一个问题:

- `contracts/` 目录不再承载任何 concrete subject 内容

这里的 concrete subject 内容包括:

- concrete analysis sample
- concrete snapshot baseline
- concrete native proof sample
- 以 `HelloWorldObject` 等具体 proof shape 为默认语义的 example 资产

本次不改:

- schema 中的 `subjectId` 命名
- `subjects/<subject-id>/source|validation|expected|baselines` 的既有所有权
- trace contract 的现有 `tests/contracts/trace/**` 布局

## 2. 现状问题

### 2.1 `contracts/` 当前混入了 fixture

当前 `contracts/` 同时扮演了三种角色:

1. formal contract definition
2. canonical validation fixture
3. 遗留兼容 example 根

这三种角色被混在同一棵树里，导致 “contract 根目录里出现具体 subject 内容” 成为必然结果。

### 2.2 当前污染路径

需要迁出的重点路径:

- `contracts/artifacts/v0/samples/*.min.json`
- `contracts/artifacts/v0/snapshots/*.snapshot.json`
- `contracts/native/v0/samples/**/*`
- `contracts/examples/v0/artifacts/*.json`
- `contracts/native/examples/v0/**/*`

其中:

- `contracts/examples/v0/**` 与 `contracts/native/examples/v0/**` 本质上是旧兼容根
- `contracts/artifacts/v0/samples` 和 `contracts/artifacts/v0/snapshots` 当前已经被当成 canonical 根消费

### 2.3 活跃 consumer 已绑定旧路径

本次必须同步改动的 active consumer 包括:

- `build/toolchains/run/testing/path_resolver.py`
- `build/toolchains/run/testing/contracts.py`
- `build/toolchains/run/testing/subject_workers.py`
- `build/toolchains/run/run_manifest.json`
- `tests/tooling/run/test_python_unittest_command.py`
- `tests/unit/run/test_subject_workers.py`
- `tests/unit/run/test_path_resolver.py`
- `tests/unit/run/test_subject_contracts_source_cutover.py`
- `tests/unit/run/test_repo_layout.py`
- 多份 active architecture 文档

## 3. 推荐目录结构

推荐把 formal definition 和 shared fixture 彻底分开。

### 3.1 `contracts/` 的最终职责

```text
contracts/
  artifacts/
    v0/
      schemas/
  native/
    v0/
      *.h
  docs/
    v0/
      *.md
```

这里不再保留:

- `samples/`
- `snapshots/`
- `examples/`
- `native/.../samples/`
- `native/examples/`

### 3.2 analysis contract fixture 的新根

```text
tests/contracts/
  analysis/
    v0/
      samples/
        *.min.json
      snapshots/
        *.snapshot.json
      native/
        abi/
          *.cpp
        bridge/
          *.cpp
  trace/
    ...
```

语义解释:

- `tests/contracts/analysis/v0/samples`: analysis contract 最小可读 fixture
- `tests/contracts/analysis/v0/snapshots`: analysis contract canonical snapshot baseline
- `tests/contracts/analysis/v0/native/**`: analysis/codegen/native boundary proof fixture

### 3.3 subject 自有内容继续留在 subject 树

这次不迁移这些内容:

- `subjects/<subject-id>/source/**`
- `subjects/<subject-id>/validation/**`
- `subjects/<subject-id>/expected/**`
- `subjects/<subject-id>/baselines/**`

原则是:

- subject 自己的输入、验证项目、expected、baseline 继续由 subject 拥有
- shared contract fixture 归 `tests/contracts/**`
- formal contract definition 归 `contracts/**`

## 4. 为什么不推荐迁到 `subjects/<subject-id>/...`

不推荐把当前这批 sample/snapshot/native example 直接挪到 `subjects/HelloWorldObject/...`，原因有三点:

1. 当前这些资产的主要消费者是 generic contract validator 和 contract test，不是某一个 subject 的私有执行路径。
2. 如果 canonical contract fixture 绑定在 `HelloWorldObject` 下，之后切换主 proof subject 时，contract 根语义会跟着漂移。
3. `subjects/<subject-id>/...` 已经有明确职责: source、validation、expected、baselines。再塞 shared fixture，会重新把边界弄混。

换句话说:

- subject-specific runtime expectation 放 `subjects/<subject-id>/...`
- shared contract fixture 放 `tests/contracts/...`

## 5. 具体迁移方案

### 5.1 目录迁移

推荐直接做实迁移，不保留新的长期兼容目录。

迁移映射:

```text
contracts/artifacts/v0/samples/*.min.json
  -> tests/contracts/analysis/v0/samples/*.min.json

contracts/artifacts/v0/snapshots/*.snapshot.json
  -> tests/contracts/analysis/v0/snapshots/*.snapshot.json

contracts/native/v0/samples/abi/*.cpp
  -> tests/contracts/analysis/v0/native/abi/*.cpp

contracts/native/v0/samples/bridge/*.cpp
  -> tests/contracts/analysis/v0/native/bridge/*.cpp

contracts/examples/v0/artifacts/*
  -> 删除；其语义并入 tests/contracts/analysis/v0/samples/*

contracts/native/examples/v0/*
  -> 删除；其语义并入 tests/contracts/analysis/v0/native/*
```

### 5.2 路径解析层调整

`build/toolchains/run/testing/path_resolver.py` 需要把 contract root 拆成两类:

- formal contract roots
- analysis fixture roots

建议的新返回字段:

- `artifactSchemaRoot` -> `contracts/artifacts/v0/schemas`
- `analysisFixtureSampleRoot` -> `tests/contracts/analysis/v0/samples`
- `analysisFixtureSnapshotRoot` -> `tests/contracts/analysis/v0/snapshots`
- `analysisFixtureNativeRoot` -> `tests/contracts/analysis/v0/native`

建议移除的旧字段:

- `artifactSampleRoot`
- `artifactSnapshotRoot`
- `nativeSampleRoot`
- `artifactSampleCompatibilityRoot`
- `nativeSampleCompatibilityRoot`

原因:

- 旧字段名默认把 fixture 伪装成 contract root
- 这正是当前目录语义混乱的根源

### 5.3 consumer 调整

`build/toolchains/run/testing/contracts.py`

- `analysis_contract_json_paths()` 改为读取:
  - `artifactSchemaRoot`
  - `analysisFixtureSampleRoot`
  - `analysisFixtureSnapshotRoot`
- `validate_analysis_contracts()` 用新 fixture 根进行 sample/snapshot 校验

`build/toolchains/run/testing/subject_workers.py`

- contract validate report 中:
  - `schemaPath` 继续指向 `contracts/artifacts/v0/schemas`
  - `snapshotPath` 改为 `tests/contracts/analysis/v0/snapshots`

`build/toolchains/run/run_manifest.json`

- `test-contract-managed-closure-bundle` 的 `artifacts` 从:
  - `contracts/artifacts/v0/samples`
  改为:
  - `tests/contracts/analysis/v0/samples`

### 5.4 文档口径调整

`contracts/docs/v0/overview.md` 需要明确写成:

- `contracts/` 是 formal contract 定义根
- analysis shared fixture 位于 `tests/contracts/analysis/v0/**`

以下 active 文档也需要同步改路径:

- `contracts/docs/v0/codegen-bridge-proof-boundary.md`
- `docs/architecture/roadmap-0/hello-world-proof-spec-v0.md`
- `docs/architecture/roadmap-0/local-verification.md`
- `docs/architecture/roadmap-0/schema-pack-v0.md`
- `docs/architecture/runtime-baseline/hello-world-proof-spec-v0.md`
- `docs/architecture/runtime-baseline/local-verification.md`
- `docs/architecture/runtime-baseline/schema-pack-v0.md`
- `docs/architecture/subject-test-framework-v1/foundation-and-windows-cutover-v1.md`

文档需要统一成一句清楚的话:

- “schema 在 `contracts/`，fixture 在 `tests/contracts/analysis/v0/**`”

### 5.5 repo layout 与测试清理

`tests/unit/run/test_repo_layout.py` 需要改成新的约束:

- `contracts/` 下不再要求存在 `snapshots/`
- `contracts/` 下不再允许出现 `examples/`
- `contracts/` 下不再允许出现 `native/.../samples`
- `tests/contracts/analysis/v0/snapshots` 必须存在
- `tests/contract/**` 不再作为 active canonical 根

同步要改的单元测试:

- `tests/unit/run/test_path_resolver.py`
- `tests/unit/run/test_subject_contracts_source_cutover.py`
- `tests/unit/run/test_subject_workers.py`
- `tests/tooling/run/test_python_unittest_command.py`

## 6. 实施顺序建议

推荐按下面顺序做，避免中间状态太久:

1. 先新建 `tests/contracts/analysis/v0/**` 并迁移文件
2. 更新 `path_resolver.py`
3. 更新 `contracts.py`、`subject_workers.py`、`run_manifest.json`
4. 更新活文档
5. 更新 repo layout test 和相关单元测试
6. 删除 `contracts/examples/**`、`contracts/native/examples/**`、`contracts/.../samples|snapshots`
7. 如有必要，顺手清理 `tests/contract/**` 的遗留并行树

## 7. 验收口径

实现完成后，至少满足以下条件:

### 7.1 目录边界

- `contracts/` 下不存在任何 concrete sample / snapshot / native sample / examples
- `tests/contracts/analysis/v0/**` 成为 analysis contract fixture 的唯一 canonical 根

### 7.2 运行时与测试口径

- `validate_analysis_contracts()` 只从:
  - `contracts/artifacts/v0/schemas`
  - `tests/contracts/analysis/v0/samples`
  - `tests/contracts/analysis/v0/snapshots`
  读取数据
- `run test contract managed-closure-bundle` 返回的新 artifact 路径指向 `tests/contracts/analysis/v0/samples`
- subject contract validate report 的 `snapshotPath` 指向新 fixture 根

### 7.3 文档口径

- active 文档不再把 `contracts/.../samples|snapshots` 写成 canonical fixture 根
- active 文档不再引用 `contracts/examples/v0` 或 `contracts/native/examples/v0`

## 8. 结论

这次改动不建议做成“在 `contracts/` 里继续保留 sample，只是尽量少放 subject 内容”。

推荐一次性把边界拉直:

- `contracts/` = formal definition
- `tests/contracts/analysis/v0/**` = shared validation fixture
- `subjects/<subject-id>/...` = subject-owned content

这样后面再新增 subject、再扩 analysis contract、再做更多 proof fixture 时，不会继续把 `contracts/` 变成“大杂烩根目录”。
