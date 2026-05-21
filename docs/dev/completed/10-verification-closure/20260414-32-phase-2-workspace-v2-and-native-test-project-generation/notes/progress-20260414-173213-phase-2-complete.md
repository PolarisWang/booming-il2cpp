# Phase 2 Progress 2026-04-14 17:32:13 +08:00

- 完成 `workspace.manifest.json` v2 的主写路径：
  - `workspaceVersion`
  - `defaultMatrixId`
  - `managedProjects`
  - `managedTestProjects`
  - `nativeProjects`
  - `nativeTestProjects`
  - `matrices`
- generated managed proof/benchmark host 已被纳入 subject workspace，并写入 `solutions/subjects/<SubjectId>/<SubjectId>.sln`。
- native test project 默认构建目标恢复为 proof host；v2 manifest 构建路由会优先选择 `nativeTestProjects`。
- 新 manifest 不再继续写入 `generatedRoot`、`defaultBuildNativeProject`、`nativeProjectPath`、`targetId` 等 legacy/冗余字段；读取侧仍保持兼容推导。
- Visual Studio 解决方案现在只保留 subject-facing 顶层 native project；跨平台 packaging stub 不再混入 Windows `.sln`。
- 验证：
  - `python -m pytest tests/unit/planning/test_project_workspace.py`
  - `python -m pytest tests/unit/planning/test_generated_managed_hosts.py tests/integration/registry/test_declared_metadata_discovery.py tests/unit/compatibility/test_compiled_subject_catalog.py`
- 下一步：切入 `20260414-33-phase-3-unified-test-entry-and-registry-cutover`，把 `run test` / registry 主链切到 workspace manifest v2。
