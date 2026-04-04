objectId: module/interop/basic
objectType: module
primaryModuleId: interop

# interop

## 对象

- `module/interop/basic`

## 目的

- 为 P/Invoke 与基础 native bridge 相关改动提供默认完成前模块验证。
- 把 `PInvokeLite` 从单独 code suite 提升为可被 skill 和 harness 直接引用的项目级模块对象。

## 覆盖

- `contract/native-abi`
- `contract/native-bridge`
- `smoke/PInvokeLite`

## 适用场景

- 托管到 native 的调用边界发生改动。
- native ABI 头文件、布局约束或 compile-only smoke 发生改动。
- 编组、符号绑定或基础导出调用链路发生改动。

## 正式入口

- `run test module --id module/interop/basic`
- `run test module --module interop --profile basic`
