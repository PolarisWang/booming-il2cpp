# Dashboard 文档约束规范

## 状态: 起草中

本文档定义了 Foundation DLL Audit Dashboard 相关开发的数据约束规则和自检流程。

---

## 1. 新增字段流程

任何时候向输出工件添加新字段，开发者必须：

1. 更新 `foundation-dll-audit-data-dictionary.md` 的对应章节
2. 在 `foundation-dll-audit-provenance-registry.md` 中添加字段记录
3. 将新增字段对应的枚举值加入代码中对应的 `valid_*` 集合
4. 如新增 JSON 输出字段，同步更新对应的 `.schema.json` 文件
5. 运行 `verify-dashboard-field-coverage.py` 确认覆盖率 >= 95%
6. 运行 `validate-output-schemas.py` 确认输出符合 schema

## 2. 数据一致性规则

- **同一语义字段跨工件必须使用同一函数产生**（如 `dllState` 只能由 `_dll_state()` 产生）
- **渲染层不引入新计算逻辑** — HTML 模板只渲染已算好的字段，不做 denominator/numerator 变更
- 所有 denominator/numerator 更新必须在 `build_foundation_dll_audit_payload()` 的 `for family_record` 循环内完成
- `extra_keys` 在 matrix 中的注入必须与 payload 中同一字段的推导函数一致
- `schemaVersion` 变更时必须兼容旧版消费者

## 3. 显示到源代码的可追溯性

从 Dashboard 显示回溯到原始数据的完整链路格式：

```
(显示字段) -> (payload dict 路径) -> (生成函数) -> (函数在 .py 文件的行号) -> (原始数据文件)
```

每条链路必须在 `provenance-registry.md` 中记录。

## 4. 完成前自检清单

在声称 dashboard 相关开发完成前，开发者必须执行：

```
## Dashboard 数据约束检查
[ ] 运行 verify-dashboard-field-coverage.py — 字段覆盖率 >= 95%
[ ] 运行 validate-output-schemas.py — 输出工件符合 JSON Schema
[ ] 注册表 diff 检查 — provenance-registry.md 与 committed baseline 对比
[ ] 新增字段已在数据字典中登记
[ ] 新增 enum 值已在枚举值字典中登记
[ ] tooltip provenance 信息准确（如涉及 UI 变更）
```
