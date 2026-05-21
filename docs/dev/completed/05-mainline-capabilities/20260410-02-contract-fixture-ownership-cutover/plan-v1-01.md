# Contract Fixture Ownership Cutover 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 让 `contracts/` 回到 formal contract root，只保留定义（schemas、headers、docs），不再承载 concrete fixture（samples、snapshots、examples）。

**架构：** 直接硬切旧路径，不保留 compatibility shim。先移动文件，再更新所有 consumer，最后清理 legacy 路径。

**技术栈：** Python, JSON, Git, Shell

**设计文档：** `docs/dev/completed/20260410-01-layout-boundary-freeze/layout-target-v1.md`

**路径映射：** `docs/dev/completed/20260410-01-layout-boundary-freeze/path-mapping-v1.md`

**预期知识沉淀：** 按任务决定

---

## 步骤

### Batch 1: 移动 artifact fixtures 到 tests/contracts/analysis/

- [ ] **1.1** 创建目标目录 `tests/contracts/analysis/v0/samples/` 和 `tests/contracts/analysis/v0/snapshots/`
- [ ] **1.2** 移动 `contracts/artifacts/v0/samples/*.min.json` (8 个文件) → `tests/contracts/analysis/v0/samples/`
- [ ] **1.3** 移动 `contracts/artifacts/v0/snapshots/*.snapshot.json` (7 个文件) → `tests/contracts/analysis/v0/snapshots/`
- [ ] **1.4** 删除空目录 `contracts/artifacts/v0/samples/` 和 `contracts/artifacts/v0/snapshots/`

### Batch 2: 移动 native samples 到 tests/contracts/native/v0/

- [ ] **2.1** 创建目标目录 `tests/contracts/native/v0/samples/abi/` 和 `tests/contracts/native/v0/samples/bridge/`
- [ ] **2.2** 移动 `contracts/native/v0/samples/abi/*.cpp` (2 个文件) → `tests/contracts/native/v0/samples/abi/`
- [ ] **2.3** 移动 `contracts/native/v0/samples/bridge/*.cpp` (3 个文件) → `tests/contracts/native/v0/samples/bridge/`
- [ ] **2.4** 删除空目录 `contracts/native/v0/samples/`

### Batch 3: 删除 legacy 路径

- [ ] **3.1** 删除 `contracts/examples/` (6 个重复文件)
- [ ] **3.2** 删除 `contracts/native/examples/` (5 个重复文件)
- [ ] **3.3** 如果 `contracts/native/docs/v0/` 中的内容不在 `contracts/docs/v0/` 中已存在，则合并；否则直接删除
- [ ] **3.4** 删除 `tests/contract/` (legacy 别名目录)

### Batch 4: 更新 path_resolver 和所有 code consumer

- [ ] **4.1** 更新 `build/toolchains/run/testing/path_resolver.py`：
  - `artifactSampleRoot` → `repo_root / "tests" / "contracts" / "analysis" / version / "samples"`
  - `artifactSnapshotRoot` → `repo_root / "tests" / "contracts" / "analysis" / version / "snapshots"`
  - `nativeSampleRoot` → `repo_root / "tests" / "contracts" / "native" / version / "samples"`
  - 删除 `artifactSampleCompatibilityRoot` 和 `nativeSampleCompatibilityRoot`
- [ ] **4.2** 更新 `build/toolchains/run/testing/subject_workers.py:382`：
  - `"snapshotPath": "contracts/artifacts/v0/snapshots"` → `"snapshotPath": "tests/contracts/analysis/v0/snapshots"`
- [ ] **4.3** 更新 `build/toolchains/run/run_manifest.json:838`：
  - `"artifacts": ["contracts/artifacts/v0/samples"]` → `"artifacts": ["tests/contracts/analysis/v0/samples"]`
- [ ] **4.4** 更新 `tests/unit/run/test_path_resolver.py`：
  - 修正 `artifactSampleRoot` 预期路径
  - 修正 `nativeSampleRoot` 预期路径
  - 删除 `artifactSampleCompatibilityRoot` 和 `nativeSampleCompatibilityRoot` 断言
  - 添加 `artifactSnapshotRoot` 预期路径断言
- [ ] **4.5** 更新 `tests/unit/run/test_subject_workers.py:283`：
  - `"contracts/artifacts/v0/snapshots"` → `"tests/contracts/analysis/v0/snapshots"`
- [ ] **4.6** 更新 `tests/tooling/run/test_python_unittest_command.py:42`：
  - `"artifacts": ["contracts/artifacts/v0/samples"]` → `"artifacts": ["tests/contracts/analysis/v0/samples"]`
- [ ] **4.7** 更新 `tests/unit/run/test_repo_layout.py`：
  - `test_contract_snapshot_baselines_live_under_contracts_tree()` 的 `snapshot_root` 改为 `tests/contracts/analysis/v0/snapshots`
  - 确认负面断言仍然正确（`tests/contracts/schema` 不存在、`tests/contract/schema` 不存在）

### Batch 5: 更新 .gitignore

- [ ] **5.1** 更新 `.gitignore`：
  - 删除 `!contracts/artifacts/v0/samples/` 及 `**` 行
  - 删除 `!contracts/examples/` 系列行
  - 删除 `!contracts/native/v0/samples/` 系列行
  - 添加 `!tests/contracts/` 系列 include 行（如果需要）

### Batch 6: 更新 active docs

- [ ] **6.1** 更新 `contracts/docs/v0/overview.md`：所有 `contracts/artifacts/v0/samples` → `tests/contracts/analysis/v0/samples`，同理 snapshots
- [ ] **6.2** 更新 `docs/architecture/runtime-baseline/schema-pack-v0.md`
- [ ] **6.3** 更新 `docs/architecture/runtime-baseline/hello-world-proof-spec-v0.md`
- [ ] **6.4** 更新 `docs/architecture/runtime-baseline/local-verification.md`
- [ ] **6.5** 更新 `docs/architecture/subject-test-framework-v1/foundation-and-windows-cutover-v1.md`
- [ ] **6.6** 更新 `wiki/06-测试验证/模块/analysis.md`
- [ ] **6.7** 更新 `wiki/04-工具与集成/统一测试框架.md`：替换 `tests/contract/` → `tests/contracts/`

### Batch 7: 验证

- [ ] **7.1** 运行 `python -m pytest tests/unit/run/test_path_resolver.py -v`
- [ ] **7.2** 运行 `python -m pytest tests/unit/run/test_repo_layout.py -v`
- [ ] **7.3** 运行 `python -m pytest tests/unit/run/test_subject_workers.py -v`
- [ ] **7.4** 运行 `python -m pytest tests/tooling/run/test_python_unittest_command.py -v`
- [ ] **7.5** 确认 `contracts/` 下不存在 `samples/`、`snapshots/`、`examples/` 目录
- [ ] **7.6** 确认 `tests/contract/` 不存在
