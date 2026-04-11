# Metadata Token Mapping v0

## 目标

冻结 metadata token 与 stable identity 的边界，避免热更链路错误地把 PE token 当成跨版本 stable key。

## v0 规则

- PE metadata token 只在“同一版本、同一 assembly”的 metadata image 内有效。
- token 不能跨 package、跨 build、跨版本直接持久化为公共 contract。
- 跨版本稳定身份必须使用 `SubjectId`。

## 查找策略

热更引用 AOT 类型/方法时，v0 推荐流程是：

1. 热更包记录 `SubjectId`
2. runtime 根据当前主包 metadata 建立 `SubjectId -> token / handle` 映射
3. 运行时按当前版本映射解析真正的 token / handle

## 为什么不能直接复用 token

- 同一个方法在重新 build 后 token 可能变化。
- 不同 assembly 的 token 空间天然独立。
- 热更补丁需要面对“主包升级但逻辑 identity 不变”的场景。

## v0 冻结结论

- token = version-local lookup key
- `SubjectId` = cross-version stable identity

## 事实来源

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `contracts/native/v0/codegen_bridge.h`
