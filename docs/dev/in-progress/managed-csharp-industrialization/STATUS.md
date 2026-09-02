# Managed C# 管线工业化 — STATUS

## Phase
`executing-plans`（M-A~M-E 分阶段推进）

## task_type
`in-progress`

## 对标
native C++ 工业化（archive/dev-completed/17-native-industrialization）

## M 阶段清单

| Phase | 内容 | 状态 |
|-------|------|:----:|
| M-A | dotnet format style CI gate + .editorconfig | ✅ 提交 13c252936 |
| M-B | Roslyn analyzers CI 门禁 | ⏳ 待做 |
| M-C | 拆分 7 个超大 C# 文件 (partial class) | ⏳ 待做 |
| M-D | C# 覆盖率硬门禁 | ⏳ 待做 |
| M-E | Scriban 模板审查规范 | ⏳ 待做 |

## M-A 完成细节 (2a7e42203)
- `.editorconfig`: 补充 C# 规范 (using 位置/var/braces/import 排序), 跳过 generated/
- `managed-csharp-style.yml`: ubuntu CI 对 PR changed .cs 增量 run dotnet format
  仿 native clang-format 增量模式, 不重写存量
- 存量噪音 (Generator 46 IMPORTS) 不阻塞, 只 red 新改动

## 已识别现实 (诚实记录)
1. **CRLF vs LF**: git autocrlf=true → Windows checkout 是 CRLF, committed 是 LF.
   CI 用 ubuntu (无 autocrlf) 得 LF → dotnet format 无伪差. 本地 Windows 需注意.
2. **存量违规规模**: Generator 46 IMPORTS (import ordering), Contracts 干净.
   全量 reformat 会产生巨大 diff → M-A 用增量 gate.
3. **dotnet format --include** 支持单文件门禁 (已验证).
4. **workflow FAILED 子 shell 问题**: managed-csharp-style.yml 的 while 子 shell
   丢 FAILED 变量, 需后续精修为真正 red (用 marker 文件 + exit code).

## 关键文档
- 评估: `docs/dev/assessments/managed-csharp-industrialization-eval.md`

## M-B 完成 (6037c9cd9) + M-C 评估
- ✅ Directory.Build.props: EnableNETAnalyzers + AnalysisLevel=latest
- ✅ 修复 CS8602: ExceptionEmission.Helpers + StructuredIR.Emit (#pragma,
   ThreadLocal 工厂初始化假阳性) + AsmCompareHandler (真实 null,加检查)
- 🔍 M-C codegen 文件评估: 4/7 已是 partial class NativeAotLoweringPlanner
   的一部分(方法45-146/文件), 拆单个2000行partial文件需codegen domain
   判断, 收益低(已职责导向partial)风险高(破坏AOT输出). 建议由专职
   codegen agent 分批做.
- 3 非 partial (Program.cs/JitAsmCapture/DllScanner) 为独立工具类.
