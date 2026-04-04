objectId: module/managed-smoke/basic
objectType: module
primaryModuleId: managed-smoke

# managed-smoke

## 对象

- `module/managed-smoke/basic`

## 目的

- 为托管 smoke 相关改动提供默认完成前模块验证。

## 覆盖

- `smoke/HelloWorld`
- `smoke/GenericEcho`

## 正式入口

- `run test module --id module/managed-smoke/basic`
- `run test module --module managed-smoke --profile basic`
