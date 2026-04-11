# 共享 Contract v0

## 定位

- `contracts/shared/v0/` 是 AOT 主线与热更运行时共享 contract 的唯一真源。
- 本目录只冻结跨阶段必须共享、且当前仓库已经有实现事实支撑的边界。
- 执行日志、阶段推进记录仍留在 `docs/dev/**`，不写入本页。

## 冻结范围

- `identity-model.md`
- `object-model.md`
- `handle-model.md`
- `abi-calling-convention.md`
- `exception-boundary.md`
- `delegate-abi.md`
- `metadata-token-mapping.md`
- `version-policy.md`
- `interpreter-ir-decision.md`
- `package-manifest.schema.json`
- `README.md`

## v0 规则

- 类型、成员、跨版本引用的稳定标识以 `SubjectId` 为准，不能依赖 PE metadata token 做跨版本 join。
- `metadata token` 只在“同版本、同程序集”的局部上下文里可当作快速索引使用。
- `package-manifest.schema.json` 中的 `supplementalMetadata` 在 v0 固定为字符串路径，不提前扩成更复杂对象。
- `version-policy.md` 规定 v0 只允许前向兼容式追加，不允许删除或重命名既有字段。

## 事实边界

- `object-model.md` 以当前 `src/native/runtime-core/runtime_core.cpp` 的可验证实现为准，不写未来理想语义。
- 当前字符串对象事实口径是 `UTF-8 + byte_count`，不是 `UTF-16` 布局承诺。
- GC bits、sync block 等更强运行时语义尚未有实现支撑，因此明确不属于 v0 保证面。
- `delegate-abi.md`、`handle-model.md` 与 `abi-calling-convention.md` 都以 `contracts/native/v0/*.h` 和当前 bootstrap/runtime proof 为锚点。

## 消费约定

- Phase 1 之后的新模块如果需要 identity、handle、ABI、package manifest 规则，必须先引用 `contracts/shared/v0/`，不能再各自发明局部 contract。
- 如果未来实现突破了 v0 事实边界，先更新实现与验证，再通过新的版本目录升级，不在 `v0/` 内直接破坏兼容。
- 文档说明和 schema 必须同步更新；只改其中一侧会导致 contract 漂移。
