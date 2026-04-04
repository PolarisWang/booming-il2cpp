objectId: module/hosted-runtime/basic
objectType: module
primaryModuleId: hosted-runtime

# hosted-runtime

## 对象

- `module/hosted-runtime/basic`

## 目的

- 为宿主启动、托管入口绑定和基础生命周期守卫相关改动提供默认完成前模块验证。
- 把 `HostEmbeddingLite` 从单独 code suite 提升为可被 skill 和 harness 直接引用的项目级模块对象。

## 覆盖

- `smoke/HostEmbeddingLite`

## 适用场景

- host embedding 启动链路发生改动。
- attach/detach、session 生命周期或托管入口守卫发生改动。

## 正式入口

- `run test module --id module/hosted-runtime/basic`
- `run test module --module hosted-runtime --profile basic`
