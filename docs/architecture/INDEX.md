# 架构文档索引

## 目录

- `runtime-baseline/`
  - 记录当前 runtime baseline 主线的长期架构、contract、gate 与本地验证规则。
- `subject-test-framework-v1/`
  - 记录 `subject` 测试框架在 Stage A+B+C 落地后的 canonical source、artifact bucket、execution spine 与 Windows cutover 边界。
- `il2cpp-hotupdate-capability-matrix.md`
  - 定义 IL2CPP、热更与 mixed execution 的能力矩阵、retained subject ownership、support-state 分层与产品化维度。
- `solution-archetype-matrix.md`
  - 定义所有 canonical solution archetype 的工程形态、owner subject、验证目标与新增准入条件。
- `full-project-hotupdate-workflow.md`
  - 定义 `HostApp + SharedContracts + PatchModules` 的完整热更工程 canonical 工作流。
- `reference-assembly-and-corelib-policy.md`
  - 定义仓库受控 reference assembly / corelib foundation bundle 的来源策略和 archetype 选型边界。
- `unsupported-feature-policy.md`
  - 定义 `supported / partial / unsupported / not-applicable` 的统一表达与 reason code 规则。
- `ios-distribution-policy.md`
  - 记录 iOS 热更新的 App Store 2.5.2 边界、配置/数据热更新与逻辑热更新的建议分层。
- `release-checklist.md`
  - 记录每个发布版本必须通过的 productization gate，包括版本兼容、rollback、mobile runtime host、iOS distribution policy、perf regression、soak test 与 unsupported feature report。
- `version-compatibility-matrix.md`
  - 记录 `AOT 主包版本` 与 `hot update patch` 的版本编号和 `major.minor` compatibility 规则。
