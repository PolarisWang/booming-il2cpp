# Evidence: NameTable.Add(System.String)

- **Assembly**: System.Private.Xml (chunk xml, subject id `System.Private.Xml/System.Xml.NameTable::Add:System.String(System.String)`)
- **Verdict**: real
- **Level**: P1 (目标)
- **Date**: TBD (由 P0-A #4 的 NameTable 单元测试产出)

## 断言内容

调用 `NameTable.Add("root")` 返回 interned 字符串实例，与同值字符串引用相等：
- `nameTable.Add("root") == "root"`（值相等）
- `nameTable.Add("root")` 与先前 `nameTable.Add("root")` 的返回值**引用相等**（String.IsInterned 语义）
- 重复调用返回同一 interned 实例

## 复现方式

```csharp
var nt = new System.Xml.NameTable();
object a = nt.Add("root");
object b = nt.Add("root");
object c = nt.Add("ro" + "ot");  // 运行时拼接，但仍 interned
// Assert(a == b);
// Assert(a == c);
// Assert((string)a == "root");
```

## 测试/构建编号

TBD — 待 P0-A 落地后填入 fact id。