# Managed C# 管线工业化 — 评估 (Native-工业化后续方向)

> 日期：2026-09-02
> 域：构建(7) + CodeGen(4)
> 状态：评估完成，待立项
> 对标：native C++ 工业化（已完成，见 archive/dev-completed/17-native-industrialization）

---

## 1. 为什么做

Native C++ 工业化已完成（工具链/模块化/文档/测试全链路）。
Managed codegen 管线（C# + Scriban 模板）是**另一半代码库**，仍处于工业化初期，
缺 native 侧已建立的同类质量门禁。两者共同构成完整的生产 il2cpp。

## 2. 当前现状（评估 2026-09-02）

### 2.1 代码规模
| 区域 | 行数 | 备注 |
|------|:---:|------|
| src/managed | 50,278 | 20 个项目 |
| src/tools | 16,560 | 5 个项目 |
| **合计** | **~66,838** | |

### 2.2 超大文件（拆分候选）
| 文件 | 行数 |
|------|:---:|
| NativeAotLoweringPlanner.StructuredIR.Emit.cs | 2447 |
| FoundationPack.Analysis/Program.cs | 2400 |
| ExceptionEmission.EmitInstruction.cs | 2061 |
| JitAsmCapture.cs | 2041 |
| AutoTestGenerator/DllScanner.cs | 2035 |
| ReflectionObjectEmission.cs | 2000 |
| Methods.cs | 1935 |

### 2.3 质量门禁缺口（对照 native 已完成项）
| 门禁 | native 状态 | managed 状态 |
|------|:---:|:---:|
| 代码格式化 CI | ✅ clang-format | ❌ 无 dotnet format |
| 静态分析 | ✅ clang-tidy | ❌ 无 Roslyn analyzers |
| 覆盖率硬门禁 | ✅ Coverlet 50% | ⚠️ 有 tool 无硬门禁 |
| 模块化 | ✅ 12 文件拆分 | ❌ 7 个超 2000 行文件 |
| API 文档 | ✅ Doxygen | ⚠️ 有 `///` 但无自动生成 |
| 运行时间 | ✅ ASan/UBSan | N/A (但需 value-tuple/alloc 审查) |

## 3. 建议方向（对照 native 工业化分阶段）

| Phase | 内容 | 对标 |
|-------|------|------|
| M-A | dotnet format CI 强制 + .editorconfig | D1 |
| M-B | Roslyn analyzers (StyleCop/Analyzer) + CI error | D1 |
| M-C | 拆分 7 个超大 C# 文件 (partial class / 职责) | D3 |
| M-D | C# 覆盖率硬门禁 (Coverlet) 提升至全项目 | D1 |
| M-E | Scriban 模板代码审查规范 | D2 |

## 4. 关键差异（vs native）

- **C# partial class** 天然支持把一个 class 拆到多文件而无需跨 TU static 处理——
  拆分成本远低于 native C++（native 需 internal header + extern）。
- **.editorconfig + format** 是声明式, 比 clang-format CI 更易统一。
- codegen 输出的正确性由 snapshot 测试(csharp)守, 已部分就位.

## 5. 结论

Managed C# 管线工业化是 native 工业化的**自然延续**, 填补另一半代码库的质量门禁。
`partial class` 让拆分比 native 更安全。建议按 M-A~M-E 立项，可与 GC-工业化并行。
