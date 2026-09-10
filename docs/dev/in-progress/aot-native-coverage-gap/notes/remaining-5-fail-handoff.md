# Handoff — 剩余 5 个 FAIL chunk 解析

> 交接给: codegen 域 / runtime 域 agent
> 日期: 2026-09-10
> 上游: ATG+gate+honest_report 会话（commit `c7bd08b97`）

## 统一根因

5 个 chunk 的每一条 smoke-unknown 记录都经过同一代码路径：

```
TestEmitter.cs:234  hasException=true && isExternalAssembly=true
  → 不执行调用
  → AppendAssert 输出 // [UNVERIFIED]
  → return 42L
```

ATG 不能突破的原因是 `d512accfa`（Sep 3）确立的设计红线：对外部 AOT stub，
**不能执行 hasException 调用，不能插入 Assert.Throws，也不能插入 default 断言**。
否则 AOT stub 的异常行为不匹配 managed → 假阴性 FAIL。

## 各 chunk 的所需修复类型

| Chunk | 缺口方法数 | 所需域 |
|-------|:---------:|--------|
| **System.IO.Compression.ZipFile** | 17 个 assertable smoke | 需要 codegen 为 `ZipFile.OpenRead/Open/CreateFromDirectory` 生成真实 native 体 |
| **System.Net.Sockets** | 358 个 assertable smoke | 需要真实 native 翻译 + 异常抛出桥 |
| **System.Private.Xml system-xml-xsl** | 164 个 assertable smoke | ATG 已搭建 `TestXPathExpression`/`TestXsltContext` 子类但每个 .NET 版本的抽象成员不同 → 不可行；需要 codegen 域翻译 |
| **System.Text.Json/global-ns** | 51 个 assertable smoke | `JsonMetadataServices.Create*Info<T>` 需要完整 `JsonTypeInfo` 对象图 |
| **System.Text.Json/text-json** | 162 个 assertable smoke | `JsonDocument.Parse` 等在 AOT external-runtime stub 中无法复现 managed 的 `JsonReaderException`™ |

### 已排除的路径（不用重做）

- ❌ 实例构造登记 —— `CSharpExpressionBuilder.KnownInstances` 已覆盖全部常见类型
- ❌ 抽象子类合成 —— `AbstractSubclassMap` + `SynthesizedSubclasses` 已覆盖可覆盖的
- ❌ fixture 机制 —— `FixtureValues` 表已登记文本/字节/地址/XPath fixture
- ❌ 门禁分母排除 unassertable —— `gating.py` 已改为 `(total - unassertable)`
- ❌ resultKind 分类 —— `fact_chunk.py` 已按运行时值 + 返回类型分类

## 代码索引

| 职责 | 位置 |
|------|------|
| `hasException` 拦截 | `TestEmitter.cs:234-241` |
| `[UNVERIFIED]` 发射 | `TestEmitter.cs:313-317`（isExternalAssembly 分支） |
| 42L 返回 | `TestEmitter.cs:257-260` |
| fixture 登记表 | `ValueGenerator.cs FixtureValues` |
| 抽象子类表 | `CSharpExpressionBuilder.cs AbstractSubclassMap / SynthesizedSubclasses` |