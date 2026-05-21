# Schema Pack v0 冻结说明

## 目标

本文定义 `Roadmap 0` 的 schema pack v0 冻结边界。

## 通用约定

- 顶层 `formatVersion` 固定为 `"v0"`
- `artifactKind` 是稳定产物标识
- `contracts/artifacts/v0/schemas/*.schema.json` 是 canonical schema
- `contracts/artifacts/v0/samples/*.min.json` 是最小可读 sample
- `contracts/artifacts/v0/snapshots/*.snapshot.json` 是 canonical baseline
- 根目录 `analysis/` 已退役；run 期 analysis 产物只存在于 `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/...`

## Example 与 Snapshot 的分工

- example 强调“最小可读”，服务文档和人工维护
- snapshot 强调“稳定基线”，服务回归校验和冻结演进
- 二者不一致时，以 schema 加 snapshot 定义的稳定边界为准

## 版本演进原则

- v0 允许在不破坏最小冻结字段命名的前提下追加字段
- 如果需要删除、重命名或改变冻结字段语义，必须提升 `formatVersion`
