# 架构文档索引

## 目录

- `il2cpp-hotupdate-capability-matrix.md`
  - 定义 IL2CPP、热更与 mixed execution 的能力矩阵、support-state 分层与产品化维度。
- `solution-archetype-matrix.md`
  - 定义 canonical solution archetype、owner subject、验证目标与新增准入条件。
- `full-project-hotupdate-workflow.md`
  - 定义 `HostApp + SharedContracts + PatchModules` 的完整热更工程 canonical 工作流。
- `reference-assembly-and-corelib-policy.md`
  - 定义受控 reference assembly / corelib foundation bundle 的来源策略与 archetype 选型边界。
- `unsupported-feature-policy.md`
  - 定义 `supported / partial / unsupported / not-applicable` 的统一表达与 reason code 规则。
- `native-code-conventions.md`
  - 定义 native C++ 代码规范、命名空间、include 等约定。
- `ios-distribution-policy.md`
  - 记录 iOS 热更新的分发边界、数据热更新与逻辑热更新建议。
- `release-checklist.md`
  - 记录发布版本必须通过的 productization gate。
- `version-compatibility-matrix.md`
  - 记录 `AOT 主包版本` 与 `hot update patch` 的版本兼容规则。
- `foundation-dll-capability-verification-closure-v1.md`
  - foundation DLL 能力验证闭包的架构说明。

## 历史版本

旧版架构阶段性子文档（roadmap-0、verification-v1、subject-test-framework-v1、string-id-v1）已移入 [`archive/architecture/`](../archive/architecture/)。
