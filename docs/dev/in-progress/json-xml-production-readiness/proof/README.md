# Proof Directory — JSON / XML Production Readiness

## 规范

每个证据文件遵循以下命名结构：

```
proof/{assembly}/{benchmark-or-test-name}/evidence-{N}.md
```

或对于独立断言：

```
proof/{assembly}/{method-subject-id-slug}.md
```

## 证据等级

| 等级 | 定义 | 必须包含 |
|---|---|---|
| P0 | 分类器辅助（人工复核过） | 日期、复核人、断言位置 |
| P1 | 人工编写的单元测试 | 测试代码路径、期望值 |
| P2 | 自动化 fact 生成 + 断言 | fact id、build 编号、日志 |

最低验收要求：每个 `real` 方法必须有至少一条 P1 或 P2 证据。

## 证据文件内容模板

```markdown
# Evidence: {method-subject-id}

- **Assembly**: {assembly}
- **Verdict**: real ｜ not-supported ｜ not-applicable
- **Level**: P1 ｜ P2
- **Date**: YYYY-MM-DD

## 断言内容

{简洁描述断言了什么、期望值是什么}

## 复现方式

{如何从基础 repository 重跑}

## 测试/构建编号

{fact id / test run id / build number}
```

## 当前证据

（Phase 0：骨架已就位，占位文件标注目标断言；Phase 1 起填入 fact id / 构建编号。）

### System.Xml.ReaderWriter — NameTable.Add/Get

- `proof/System.Xml.ReaderWriter/NameTable-Add-Get.md` — TBD（由 P0-A #4 的 NameTable 单元测试产出）

### System.Text.Json — 已定位的 real

- `proof/System.Text.Json/Utf8JsonWriter-WriteString.md` — TBD
- `proof/System.Text.Json/JsonElement-EnumerateObject.md` — TBD