# Version Compatibility Matrix

> Compatibility shorthand: `major 不兼容`, `major.minor` 相同视为同一 compatibility band。

## 版本对象

- `AOT 主包版本`：宿主二进制与内置 metadata / bridge / runtime 的版本。
- `hot update patch`：运行时加载的补丁包版本。

## 关联文档

- `full-project-hotupdate-workflow.md`
- `release-checklist.md`
- `unsupported-feature-policy.md`

## 版本编号规则

### AOT 主包版本

- 使用 `major.minor.patch`。
- `major` 表示破坏性架构变更。
- `minor` 表示兼容面扩展或某类 metadata 变化。
- `patch` 表示不改变 compatibility band 的修复。

### Hot update patch

- 同样使用 `major.minor.patch`。
- patch 自身可以独立递增，但它面向的宿主 compatibility band 由 `TargetAotVersion` 决定。

## 兼容判定规则

- 默认按 `major.minor` 判定 compatibility band。
- `major.minor` 相同：兼容。
- `patch` 不同：仍兼容。
- `major` 不同：不兼容。
- `minor` 不同：默认不兼容，除非后续引入显式兼容矩阵放宽规则。

## PackageValidator 落地规则

- `PackageValidator` 负责校验 runtime AOT version 与 package `TargetAotVersion`。
- 当前实现要求两者都能解析成 `major.minor.patch`。
- 当前实现要求两者至少落在同一个 `major.minor` compatibility band。

## 后续扩展

- 若后续需要支持跨 minor 兼容，必须在 manifest 中显式引入兼容矩阵，而不是继续堆叠字符串约定。
- rollback、compatibility runner 与 release checklist 应复用同一套 compatibility 定义，避免出现多份规则。
