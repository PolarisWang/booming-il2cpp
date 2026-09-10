# Progress v1.02

日期：2026-04-13 00:59:47 +08:00

## 本轮完成

- 将 `subjects/golden/GoldenSimpleLib`、`subjects/golden/GoldenMultiProject`、`subjects/golden/GoldenWithPackage` 的 canonical source 迁移到新的 `subjects/Solution*` archetype subjects。
- 在 `build/toolchains/run/testing/path_resolver.py` 中补齐 legacy subject id alias 解析，让 `Golden*` 仍可映射到新的 `Solution*` canonical subject。
- 将 legacy `subjects/golden/*` 收敛为 compatibility shell，并补齐最小 manifest 契约字段，使其满足 repo layout 与 manifest 约束，不再污染 canonical 结构。
- 对齐与 archetype cutover 直接相关的 tests 与 workload 引用，确认 `convert-golden-multi-project` 已指向 `SolutionMultiProject`。

## 验证结果

- `python -m pytest tests/unit/run/test_repo_layout.py -q` -> `25 passed`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q` -> `13 passed`
- `python -m pytest -q` -> `712 passed, 40 skipped`

## 下一步提示

- 归档当前 Phase 4 Batch 1。
- 父 roadmap 切换到 Phase 5 Batch 1，优先把 `MainlineFeaturePack` 这类已合并 proof subject 切到 `ChaosUnitTest` 声明式发现。
