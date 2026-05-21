# Progress v1.02

日期：2026-04-13 01:27:27 +08:00

## 本轮完成

- 将 `subjects/MainlineFeaturePack/subject.manifest.json` 切换到 `testDeclarationMode=require`。
- 在 `subjects/MainlineFeaturePack/source/MainlineFeaturePack.csproj` 中引入 `src/reference/Chaos.TestFramework/Chaos.TestFramework.csproj`。
- 为 `MainlineFeaturePack` 的 7 个 proof entry methods 增加真实 `ChaosUnitTest(...)` 标注：
  - `ProofEntry`
  - `DispatchProofEntry`
  - `GenericLayoutProofEntry`
  - `ArrayBoxingProofEntry`
  - `DelegateProofEntry`
  - `ExceptionProofEntry`
  - `ReflectionInteropClosureEntry`
- 修复 `subjects/Directory.Build.props` 的中间目录冲突问题，引入 `ChaosTempIntermediateRoot` 支持，避免 `MainlineFeaturePack` 与 `Chaos.TestFramework` 在命令行显式属性下共用同一 `obj` 目录。
- 对齐声明发现相关 tests，冻结 `MainlineFeaturePack` 的 declared metadata payload、manifest contract 与 planner/schema 兼容行为。

## 验证结果

- `python -m pytest tests/unit/run/test_phase4_mainline_feature_pack_onboarding.py -q` -> `4 passed`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q` -> `4 passed`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q` -> `13 passed`
- `python -m pytest tests/unit/planning/test_subject_planner.py -q` -> `25 passed`

## 下一步提示

- 归档当前 Phase 5 Batch 1。
- 在父 roadmap 下创建 Phase 5 Batch 2，优先处理一组低耦合的 legacy opcode/object proof roots，把它们并回 `MainlineFeaturePack` 并退化为 compatibility shell。
