# Schema Pack v0 冻结说明

## 目标

本文定义 Roadmap 0 的 schema pack v0 冻结边界。它只负责稳定 JSON 资产的最小骨架，避免过早冻结分析细字段，同时为 example 与 snapshot 提供统一解释。

## 通用约定

- 所有 schema 顶层 `formatVersion` 固定为 `"v0"`。
- `artifactKind` 作为稳定产物标识，一旦对外使用，不应随意改名。
- `analysis/contracts/examples/*.min.json` 是人维护、可读、最小样例，用来表达 contract 意图，不追求覆盖全部字段。
- `tests/contracts/schema/*.snapshot.json` 是 canonical baseline，用来承载稳定基线；它与 example 分离存放，后续可独立演进。
- 除顶层稳定字段和各 schema 明确列出的最小冻结字段外，其余字段默认允许追加；v0 目标是保守冻结，不抢先定义未来细节。

## Schema 清单

## 共享 `subjectId` 约定

- 凡是将 `subjectId` 作为最小冻结字段的 schema，都使用同一套人读 canonical 规则。
- `subjectId` 至少包含程序集，以及适用的类型名 / 成员签名；不得使用指针、随机 UUID 或仅本地有效的临时编号。
- 类型示例：`Game.Core/Game.Player`
- 方法示例：`Game.Core/Game.Player::TakeDamage(System.Int32)`
- 泛型场景必须带出实例化形态，例如：`Game.Core/Game.Inventory::AddItem<System.String>(System.String)`
- 如果是委托绑定或组合主体，可在保留核心签名稳定性的前提下追加 `=>` 等连接语法。

### `metadata-graph`

- 目的：表达按 `assembly -> type -> member` 组织的元数据关系骨架。
- 最小冻结字段：`formatVersion`、`artifactKind`、`assemblies[]`、`assemblies[].assemblyName`、`assemblies[].types[]`、`assemblies[].types[].typeName`。如果出现 `members[]`，则其最小冻结字段为 `memberKind`、`memberName`。
- 允许追加：类型可继续追加 token、attribute、base type、interface、nested graph edge；成员可追加签名、slot、visibility、custom attribute 等。
- 禁止随意改动字段名：`assemblies`、`assemblyName`、`types`、`typeName`、`members`、`memberKind`、`memberName`。
- 说明：v0 只冻结可稳定消费的骨架，不冻结完整 metadata graph 的全部细字段。

### `typed-il-ir`

- 目的：表达方法级 typed IL IR 的最小稳定骨架。
- 最小冻结字段：`formatVersion`、`artifactKind`、`methods[]`、`methods[].methodId`、`methods[].signature`、`methods[].blocks[]`、`methods[].blocks[].blockId`、`methods[].blocks[].instructions[]`、`methods[].blocks[].instructions[].op`。
- 允许追加：参数注解、SSA 名称、常量值、控制流边、异常流、优化标签、平台特定 lowering 信息。
- 禁止随意改动字段名：`methods`、`methodId`、`signature`、`parameters`、`blocks`、`blockId`、`instructions`、`op`。
- 说明：v0 只冻结方法、基础块、指令这三层骨架，不冻结完整 IR 细节。

### `aot-manifest`

- 目的：声明需要预先保留或生成的 AOT 主体。
- 最小冻结字段：`formatVersion`、`artifactKind`、`entries[]`、`entries[].assemblyName`、`entries[].subjectKind`、`entries[].subjectId`、`entries[].reason`。
- 允许追加：warmup 分类、来源组件、平台、优先级、泛型实例说明、去重信息。
- 禁止随意改动字段名：`entries`、`assemblyName`、`subjectKind`、`subjectId`、`reason`。

### `code-registration`

- 目的：表达 code registration 结果中最小可验证的模块与注册条目。
- 最小冻结字段：`formatVersion`、`artifactKind`、`modules[]`、`modules[].moduleName`、`modules[].registrations[]`、`registrationKind`、`slot`、`symbol`、`subjectId`。
- 允许追加：指针地址、节区来源、平台差异标签、native symbol 备注、排序来源。
- 禁止随意改动字段名：`modules`、`moduleName`、`registrations`、`registrationKind`、`slot`、`symbol`、`subjectId`。

### `metadata-registration`

- 目的：表达 metadata registration 中最小可验证的注册主体与稳定槽位。
- 最小冻结字段：`formatVersion`、`artifactKind`、`registrations[]`、`registrationKind`、`slot`、`subjectId`。
- 允许追加：token、owner type、source module、generic sharing 标签、布局偏移。
- 禁止随意改动字段名：`registrations`、`registrationKind`、`slot`、`subjectId`。

### `preserve-descriptor`

- 目的：声明在裁剪、AOT 或反射路径下需要保留的主体。
- 最小冻结字段：`formatVersion`、`artifactKind`、`entries[]`、`subjectKind`、`subjectId`、`preserve`、`reason`。
- 允许追加：平台条件、条件表达式、来源规则、补充说明、粒度覆盖。
- 禁止随意改动字段名：`entries`、`subjectKind`、`subjectId`、`preserve`、`reason`。

## Example 与 Snapshot 的角色分离

- example 强调“最小可读”，用于人工维护和文档引用。
- snapshot 强调“规范基线”，用于后续验证、对比和冻结演进。
- 当二者内容不一致时，以 schema 与 snapshot 的稳定边界为准；example 不承担完整基线职责。

## 版本演进原则

- v0 允许在不破坏最小冻结字段命名的前提下向对象追加字段。
- 如果未来需要移除、重命名或改变最小冻结字段语义，应提升 `formatVersion`，而不是在 v0 内静默修改。
