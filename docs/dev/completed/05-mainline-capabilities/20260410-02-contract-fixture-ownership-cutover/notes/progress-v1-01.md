# Progress v1.01 — Contract Fixture Ownership Cutover 完成

Date: 2026-04-10

## 完成项

- [x] 移动 `contracts/artifacts/v0/samples/` (8 files) → `tests/contracts/analysis/v0/samples/`
- [x] 移动 `contracts/artifacts/v0/snapshots/` (7 files) → `tests/contracts/analysis/v0/snapshots/`
- [x] 移动 `contracts/native/v0/samples/` (5 files) → `tests/contracts/native/v0/samples/`
- [x] 删除 `contracts/examples/` (6 files)
- [x] 删除 `contracts/native/examples/` (5 files)
- [x] 删除 `contracts/native/docs/` (2 files, content already exists in contracts/docs/v0/)
- [x] 删除 `tests/contract/` (13 files, legacy alias)
- [x] 更新 `path_resolver.py`: 新路径 + 移除 compatibility keys
- [x] 更新 `subject_workers.py`: snapshotPath
- [x] 更新 `run_manifest.json`: artifacts 路径
- [x] 更新 `test_path_resolver.py`: 新 expected paths + 新 assertNotIn
- [x] 更新 `test_repo_layout.py`: 新 snapshot root + 新 filesystem assertions
- [x] 更新 `test_python_unittest_command.py`: 新 input data + 新 expected assertion
- [x] 更新 `test_subject_workers.py`: 新 expected snapshotPath
- [x] 更新 `.gitignore`: 移除旧 include 行
- [x] 更新 7 个 active docs（overview、schema-pack、hello-world-proof-spec、local-verification、foundation-cutover、wiki analysis、wiki 统一测试框架）

## 验证结果

- `test_path_resolver.py`: 3/3 passed
- `test_repo_layout.py`: 12/12 passed
- `test_python_unittest_command.py`: 1/1 passed
- `test_subject_workers.py`: 9/10 passed (1 pre-existing failure, unrelated to contract migration)
- Filesystem: contracts/artifacts/v0/samples — DOES NOT EXIST (correct)
- Filesystem: contracts/artifacts/v0/snapshots — DOES NOT EXIST (correct)
- Filesystem: contracts/examples — DOES NOT EXIST (correct)
- Filesystem: contracts/native/examples — DOES NOT EXIST (correct)
- Filesystem: tests/contract — DOES NOT EXIST (correct)

## 无 Wiki 更新需求

wiki 内容已在本轮直接更新（analysis.md 和 统一测试框架.md）。
