# Shared Handle Model v0

## 目标

冻结 shared runtime / reflection handle 的最小约束，避免 AOT 与热更各自发明不同的 handle 语义。

## 冻结 handle 类型

- `TypeInfoHandle`
- `MethodInfoHandle`
- `FieldInfoHandle`
- `PropertyInfoHandle`
- `EventInfoHandle`
- `ParameterInfoHandle`
- `GenericContextHandle`
- `AssemblyHandle`
- `ImageHandle`
- `ExceptionHandle`

## 语义

- 所有 handle 都是 opaque pointer typedef。
- handle 只允许通过 ABI / bridge helper 使用，不允许调用方自行解引用。
- handle 不可序列化，不可跨进程，不可跨 runtime 实例缓存。
- runtime shutdown 后，既有 handle 一律视为失效。
- metadata token 可以帮助解析 handle，但 token 本身不是 handle。

## 非目标

- v0 不定义稳定的“handle 数值编码”。
- v0 不承诺 handle 能在不同进程、不同 build、不同 package 之间直接复用。

## 事实来源

- `contracts/native/v0/runtime_abi.h`
