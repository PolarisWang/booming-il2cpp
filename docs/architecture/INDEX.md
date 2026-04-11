# 架构文档索引

## 目录

- `runtime-baseline/`
  - 记录当前 runtime baseline 主线的长期架构、contract、gate 与本地验证规则。
- `subject-test-framework-v1/`
  - 记录 `subject` 测试框架在 Stage A+B+C 落地后的 canonical source、artifact bucket、execution spine 与 Windows cutover 边界。
- `ios-distribution-policy.md`
  - 记录 iOS 热更新的 App Store 2.5.2 边界、配置/数据热更新与逻辑热更新的建议分层。
- `release-checklist.md`
  - 记录每个发布版本必须通过的 productization gate，包括版本兼容、rollback、mobile runtime host、iOS distribution policy、perf regression、soak test 与 unsupported feature report。
- `version-compatibility-matrix.md`
  - 记录 `AOT 主包版本` 与 `hot update patch` 的版本编号和 `major.minor` compatibility 规则。
