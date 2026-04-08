objectId: module/analysis/basic
objectType: module
primaryModuleId: analysis

# analysis

## 对象

- `module/analysis/basic`

## 目的

- 为 analysis contract 的 schema / sample / snapshot 一致性提供正式验证入口
- 把底层脚本里的 analysis contract 校验升级成可复用的模块对象

## 覆盖

- `contract/analysis-schema`

## 适用场景

- 修改 `contracts/artifacts/v0/schemas/*.schema.json`
- 修改 `contracts/artifacts/v0/samples/*.min.json`
- 修改 `contracts/artifacts/v0/snapshots/*.snapshot.json`

## 正式入口

- `run test module --id module/analysis/basic`
- `run test module --module analysis --profile basic`
