# EH P2 诊断工具

P2 的目标是「确保 C# `throw` 的每个异常类型都能被 `catch` 正确匹配」。
以全仓 **1614 条 EUR**（Exception-Uncaught-Raise，即 fact 记录中
`caught=true && assertFailed=false`）为入口做诊断时，单看总数无法驱动修复 ——
同一签名由多种互不相关的原因产生。这套工具用于把它们分开。

## 脚本

| 脚本 | 用途 |
|---|---|
| `classify_eur.py` | 按 `resultKind` / `bodyAvailability` 把 EUR 分桶 |
| `classify_eur2.py` | 进一步交叉引用 **subject 源码标记**（`AOT-THROWS-ASSERT` / `AOT-STUB-GAP` / `skipping Throws`） |
| `sample_eur.py` | 按 chunk + resultKind 抽样，并把同一方法族的失败折叠成一行 |

## 用法

```bash
python tools/eh/classify_eur.py  --root artifacts/foundation-dll
python tools/eh/classify_eur2.py --root artifacts/foundation-dll \
                                 --subjects artifacts/foundation-dll
python tools/eh/sample_eur.py    --min 30
```

## 两个必须知道的坑（都实际踩过）

1. **subject 源的真实位置是 `artifacts/.../managed/combined/CombinedSubjects.cs`，
   不是 `testing/foundation-dll/...`** —— 后者可能是很久以前的副本。
   用错目录会得到「fact 里的 subject 编号在源码里找不到」的假象。
   使用前请核对两个文件的 mtime 是否同批。

2. **标记注释在方法体【内】，不在签名上方**：

   ```csharp
   public long AppendChild_6_XmlNode_0() {
       // [AOT smoke] ... skipping Throws      ← 在这里
       return 42L;
   }
   ```

   最初的匹配器按「签名前一行」扫描，命中数为 0。

## 产物

`P2-L3-CONCLUSION.md` —— 诊断结论：**EH 机制本身正确**，
EUR 的主导根因是 codegen 的 `chaos_external_runtime_*` 调用丢失实参
（不属 EH 域，已列移交建议）。
