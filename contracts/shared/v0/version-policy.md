# Shared Version Policy v0

## 目标

冻结 shared contract 的演进规则，避免后续 phase 在没有升级版本号的前提下破坏兼容。

## 顶层版本

- `formatVersion` 固定为 `"v0"`。
- v0 是当前 proof baseline 的冻结版本。

## v0 允许的变更

- 在 JSON object 末尾追加可选字段。
- 在 C struct 末尾追加新字段，并继续保留旧前缀布局。
- 在文档中补充解释、限制和示例，只要不改变已冻结语义。

## v0 禁止的变更

- 删除冻结字段
- 重命名冻结字段
- 改变冻结字段语义
- 改变已冻结 enum 值
- 重排已有 C struct 字段顺序

## struct_size 规则

- 任何跨边界 C struct 都必须把 `struct_size` 作为第一个字段。
- 新实现读取旧调用方输入时，只能访问 `struct_size` 覆盖范围内的字段。
- 调用方不能假设新字段在旧实现中存在。

## 何时升级到 v1

以下任一条件成立都必须升级：

- 需要删除或重命名 v0 字段
- 需要改变对象 / handle / delegate 的冻结语义
- 需要改变 hot update package manifest 的 required 字段集合
- 需要让旧版 runtime 无法安全忽略的新行为成为必需

## 结论

- additive only = v0 内允许
- breaking change = 必须升版本
