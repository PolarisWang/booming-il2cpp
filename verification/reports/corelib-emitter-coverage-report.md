# CoreLib Emitter Coverage 摸底报告

Date: 2026-05-02
Source: `assets/framework-packs/dotnet-foundation/net10/runtime/System.Private.CoreLib.dll`

## 扫描结果

| 指标 | 数值 |
|------|------|
| Total methods | 42,371 |
| With body | 40,082 |
| Unique opcodes scanned | 197 |
| Emitter known opcodes | 97 (base) + 43 (short forms) = 140 |
| Coverage | 71.1% (140/197) |

## 缺失 Opcode 分类

### A: 假阳性（IR 层已归一化）— 16 个
`ldelem.i`, `ldelem.i1`, `ldelem.i4`, `ldelem.u1`, `ldelem.u2`, `ldelem.u4`, `ldelem.r8`, `stelem.i`, `stelem.i1`, `stelem.i2`, `stelem.i4`, `stelem.i8`, `stelem.r8`, `ldind.i`, `stind.i`

→ emitter 处理通用的 `ldelem`/`stelem`/`ldind`/`stind`，不关注类型变体

### B: 无符号变体 — 3 个
`bgt.un`, `ble.un`, `blt.un` → emitter 已覆盖

### C: 溢出检查变体 — 22 个
`conv.ovf.*`, `add.ovf.un`, `mul.ovf.un`, `sub.ovf.un`
→ 部分 emitter 已有（如 `conv.ovf.i1`, `conv.ovf.u1`），部分缺失（如 `conv.ovf.i`, `conv.ovf.i.un` 系列）

### D: 真实缺失（需要 emitter 扩展）— 16 个
| Opcode | 重要性 | 说明 |
|--------|--------|------|
| `constrained` | 高 | IL 前缀，用于值类型上的虚方法调用 |
| `endfinally` | 高 | finally 块结束 |
| `rethrow` | 高 | 异常重抛 |
| `ldvirtftn` | 中 | 加载虚函数指针 |
| `nop` | 低 | 无操作（调试占位） |
| `volatile` | 中 | 易失内存前缀 |
| `readonly.` | 中 | 只读数组前缀 |
| `sizeof` | 中 | 值类型大小 |
| `neg` | 低 | 算术取反 |
| `div.un` | 低 | 无符号除法 |
| `rem.un` | 低 | 无符号取模 |
| `clt.un` | 低 | 无符号小于比较 |
| `conv.r.un` | 低 | 无符号转浮点 |
| `endfilter` | 低 | filter 子句结束 |
| `starg.s` | 低 | 存储到参数 |
| `refanytype` | 低 | 获取类型化引用类型 |

## 结论

CoreLib emitter 扩展现状：**真实缺失约 16 个 opcodes，其中高优先级约 3 个（constrained、endfinally、rethrow）**。Phase 1 将优先补齐这些缺失，然后走完整验证闭环。
