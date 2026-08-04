# Progress v1.01 — Run Tooling Domain Reorg 完成

Date: 2026-04-10

## 完成项

- [x] 创建 `build/toolchains/run/core/` 子目录
- [x] 移动 common.py, manifest.py, result.py, operation_reporting.py, tooling.py → core/
- [x] 修复 core/ 内模块的 import（manifest.py → try/except pattern, tooling.py → parents[1], operation_reporting.py → ..testing.events）
- [x] 更新 ~25 个文件的 relative import（commands/, testing/, root level）
- [x] 更新 ~25 个文件的 fallback import（commands/, testing/, root level）
- [x] 更新 ~20 个测试文件的动态加载路径
- [x] 创建 `build/toolchains/run/subject/` 子目录
- [x] 移动 project_workspace.py, deploy_layout.py → subject/
- [x] 修复 subject/ 内模块和所有 consumer 的 import
- [x] 修复 Phase 1 遗留的测试路径断言（test_contract_paths, test_subject_contracts_source_cutover, test_managed_closure_contract_bundle）

## 验证结果

- 276 passed, 8 pre-existing failures, 32 skipped
- 0 regressions introduced

## 未完成项（留给后续阶段）

- testing/ 子目录重组（contracts/, registry/, subject/, session/）
- tui.py 拆分为 ui/ 子目录
- commands/test.py 拆分为 commands/test/ 子目录
- subject_workers.py 拆分为 testing/subject/workers/ 子目录
- manifest 分片

## 无 Wiki 更新

本任务是 roadmap Phase 2 的结构重组，不涉及 wiki 更新。
