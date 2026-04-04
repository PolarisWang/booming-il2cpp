objectId: system/hosted-runtime-smoke
objectType: system
primaryModuleId: hosted-runtime

# hosted-runtime-smoke

## 对象

- `system/hosted-runtime-smoke`

## 目的

- 验证宿主初始化、托管入口和基础 smoke 模块的协同可运行。

## 覆盖

- `module/managed-smoke/basic`
- `module/hosted-runtime/basic`

## 正式入口

- `run test system --id system/hosted-runtime-smoke`
- `run test system --scenario hosted-runtime-smoke`
