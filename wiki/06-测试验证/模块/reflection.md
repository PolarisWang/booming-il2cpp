objectId: module/reflection/basic
objectType: module
primaryModuleId: reflection

# reflection

## 对象

- `module/reflection/basic`

## 目的

- 为反射相关改动提供默认完成前模块验证。
- 把 `ReflectionLite` 从单独 code suite 提升为可被 skill 和 harness 直接引用的项目级模块对象。

## 覆盖

- `smoke/ReflectionLite`

## 适用场景

- 反射字段、方法、元数据访问链路发生改动。
- 需要确认反射能力已达到当前开发范围内的完成门槛。

## 正式入口

- `run test module --id module/reflection/basic`
- `run test module --module reflection --profile basic`
