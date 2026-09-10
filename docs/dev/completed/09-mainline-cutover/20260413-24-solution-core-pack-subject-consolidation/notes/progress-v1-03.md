# 进度记录 v1.03

Date: 2026-04-13 13:26:39 +08:00

## 本次完成

- 完成 `SolutionCorePack` archetype consolidation 收口：
  - `SolutionSimpleLib` -> `subjects/SolutionCorePack/source/Archetypes/SolutionSimpleLib`
  - `SolutionMultiProject` -> `subjects/SolutionCorePack/source/Archetypes/SolutionMultiProject`
  - `SolutionPackageReference` -> `subjects/SolutionCorePack/source/Archetypes/SolutionPackageReference`
- 删除已迁移的 3 个 legacy top-level subject 根：
  - `subjects/SolutionSimpleLib`
  - `subjects/SolutionMultiProject`
  - `subjects/SolutionPackageReference`
- 更新 `SolutionCorePack` 的 `.sln` / `Launcher` / `subject.manifest.json`，让 archetype matrix 直接落到 retained canonical subject
- 一次性删除 `build/toolchains/run/testing/path_resolver.py` 中的 compatibility alias 解析，不再保留 `legacySubjectIds` / `subjectAliases` / `aliases`
- 把依赖真实 legacy subject 根的 `tests/` 改为 synthetic fixture，避免 cleanup 后测试仍耦合被删除的 top-level subject 目录
- 定向回归通过：`34 passed`
- 扩展回归通过：`142 passed`
- 真实 retained subject 验证通过：
  - `python build/toolchains/run/run.py test registry refresh`
  - `subject/SolutionCorePack`
    - `windows-archetype-simple-lib-managed-output`
    - `windows-archetype-multi-project-managed-output`
    - `windows-archetype-package-reference-managed-output`
    - default proof
    - `perf.release`
  - `subject/HotUpdateHostPack`
    - default proof
    - `perf.release`
  - `subject/MixedExecutionFeaturePack`
    - default proof
    - `perf.release`

## 当前结论

- 父 roadmap 的 done definition 已满足：
  - retained solution-mode subject 已收敛完成
  - proof / benchmark 全量验证已完成
  - compatibility alias 已删除
  - archetype legacy subject 根已删除
- 这轮主任务已经从“重规划”切换为“真实落地并验收完成”。

## 下一步建议

- 无。本 roadmap 可以归档。
