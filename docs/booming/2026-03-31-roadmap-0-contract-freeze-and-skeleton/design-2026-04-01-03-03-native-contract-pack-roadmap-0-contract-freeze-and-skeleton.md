# 第 3 段：Native Contract Pack（ABI / Bridge）

## 包结构

公共 native contract pack 采用：

- 根路径：`contracts/native/`
- 版本目录：`contracts/native/v0/`
- 文档目录：`contracts/native/docs/v0/`
- example 目录：`contracts/native/examples/v0/`
- 测试目录：`tests/contracts/native/abi/`、`tests/contracts/native/bridge/`

header 文件名不带版本，版本由目录层承载。
例如：

- `contracts/native/v0/boom_runtime_abi.h`
- `contracts/native/v0/boom_codegen_bridge.h`

## ABI 与 Bridge 的职责分层

### ABI

`ABI` 是 runtime 对外暴露的低层基础操作面。
直接调用者通常是：

- bridge
- host embedding
- 手写 smoke
- 少量平台 glue

它重点承担：

- runtime init / shutdown
- thread attach / detach
- object / array / string allocation
- class init
- GC handle / root 基础操作
- 托管异常抛出入口
- 通过句柄/描述符承载的字段访问、方法调用、反射查询

错误边界采用更底层、更显式的模式：

- 基础操作优先返回状态 / 空值
- 托管异常通过专门入口显式触发

### Bridge

`bridge` 是 generated C++ 更直接依赖的高层 helper 面。
它承担：

- code / metadata registration
- runtime metadata helper
- boxing / unboxing helper
- virtual invoke helper
- delegate helper
- icall resolve helper

并且这些高阶 helper 在 `v0` 就冻结到全细节级：

- 名称
- 签名
- 参数语义
- 调用约定
- 错误处理模式

`bridge` 的错误模型采用混合方案：

- 查询 / 解析类 helper：返回状态码或空指针
- 托管语义失败：走受控异常出口

## ABI v0 的句柄与覆盖范围

`ABI v0` 的句柄集合采用扩展版，包含：

- `TypeInfo`
- `MethodInfo`
- `FieldInfo`
- `Assembly/Image`
- `Exception`
- `PropertyInfo`
- `EventInfo`
- `ParameterInfo`
- `GenericContext`

这些高阶句柄在 `v0` 中按完整语义承诺处理，而不是占位句柄。

但进入 `ABI` 的方式仍以不透明句柄 + 通用操作函数为主，而不是铺开大量直接 API。

## 文档与 example

native contract pack 不是只放 header。
它必须至少包含：

- 正式 header
- 人读文档
- ABI example 2-3 个
- bridge example 2-3 个
- compile-only smoke

example 覆盖范围不仅是基础动作，也包括：

- virtual invoke
- delegate
- icall resolve

## 当前结论

这意味着 `Roadmap 0` 中的 native contract pack 已经不是“占位式接口”，而是一套偏重、可读、可验证、可被后续实现直接对齐的正式 contract 包。
