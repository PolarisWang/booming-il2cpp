# Productization Gates Design v1.01

## 本轮目标

- 为 hot update package 建立最小可执行的版本兼容规则。
- 将规则接入 `PackageValidator`，替代当前“必须完全等于 target AOT version”的硬编码。
- 补齐与产品发布相关的两份架构文档：
  - `docs/architecture/ios-distribution-policy.md`
  - `docs/architecture/version-compatibility-matrix.md`

## 设计约束

- 不引入依赖真实 Android/iOS 设备运行的验证。
- 不破坏 Phase 5/6/7 现有 proof 的 package 形状。
- 尽量复用现有 `TargetAotVersion` 字段，只在需要时新增最小辅助类型。

## 兼容规则

- AOT 版本采用 `major.minor.patch` 语义。
- hot update package 的 `TargetAotVersion` 表示它面向的 AOT compatibility band。
- 默认规则：
  - `major` 不同：不兼容
  - `major` 相同、`minor` 不同：不兼容
  - `major.minor` 相同、`patch` 不同：兼容
- 当版本字符串不合法时，`PackageValidator` 直接拒绝。

## 代码改动

- 在 `Chaos.IL2CPP.HotUpdate` 内新增一个轻量版本解析/比较 helper。
- `PackageValidator.ValidateCompatibleTargetAotVersion(...)` 改为：
  - 解析 runtime AOT version
  - 解析 package target AOT version
  - 按 `major.minor` compatibility band 验证
  - 继续保留 signature 非空校验

## 测试策略

- 新增 Phase 8 定向 pytest，先验证以下失败场景：
  - `PackageValidator` 仍然只做精确相等检查
  - 缺少版本解析 helper
  - 缺少两份架构文档
- 之后实现代码并让测试转绿。

## 非目标

- 本轮不实现 compatibility matrix runner。
- 本轮不实现 rollback / integrity drill。
- 本轮不实现 24h soak test。
