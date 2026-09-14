# Evidence: Utf8JsonWriter.WriteString

- **Assembly**: System.Text.Json (subject id `System.Text.Json/System.Text.Json.Utf8JsonWriter::WriteString:System.Void(System.String,System.String)`)
- **Verdict**: real
- **Level**: P1 (目标)
- **Date**: TBD (由 Phase 2 止血 / Phase 3 核心 API 产出)

## 断言内容

以 `Utf8JsonWriter` 写入单个对象属性，输出字节序列与 .NET 8 参照逐字节一致：
- `writer.WriteStartObject(); writer.WriteString("name", "value"); writer.WriteEndObject();`
- 输出 UTF-8 字节 == `{"name":"value"}` 的 UTF-8 编码
- 转义规则验证：`\"`、`\\`、`\uXXXX`（控制字符）按 JSON 规范转义

## 复现方式

```csharp
using var ms = new MemoryStream();
using (var w = new Utf8JsonWriter(ms))
{
    w.WriteStartObject();
    w.WriteString("name", "value");
    w.WriteEndObject();
}
// Assert(Encoding.UTF8.GetString(ms.ToArray()) == "{\"name\":\"value\"}");
```

## 测试/构建编号

TBD — 待核心 API 落地后填入 fact id。