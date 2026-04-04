objectId: module/analysis/basic
objectType: module
primaryModuleId: analysis

# analysis

## 对象

- `module/analysis/basic`

## 目的

- 为 `analysis/contracts` 下的 schema、example、snapshot 一致性提供默认完成前模块验证。
- 把原先只藏在 roadmap-0 底层脚本里的 analysis contract 校验升格为正式对象，供 harness、registry 和 skill 直接调用。

## 覆盖

- `contract/analysis-schema`

## 适用场景

- 修改 `analysis/contracts/schemas/*.schema.json`
- 修改 `analysis/contracts/examples/*.min.json`
- 修改 `tests/contract/schema/*.snapshot.json`

## 正式入口

- `run test module --id module/analysis/basic`
- `run test module --module analysis --profile basic`
