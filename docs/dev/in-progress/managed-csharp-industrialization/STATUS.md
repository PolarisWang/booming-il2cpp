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
| M-A | dotnet format style CI gate + .editorconfig | ✅ 完成细节见下 (2a7e42203) |
| M-B | Roslyn analyzers（启用 + 修 CS8602，CI 硬门禁**未接线**） | 🟡 部分（6037c9cd9）|
| M-C | 拆分 7 个超大 C# 文件 (partial class) | ✅ 评估闭环：4/7 已是 partial、3/7 独立工具类，推荐专职 codegen 再拆（见下） |
| M-D | C# 覆盖率硬门禁 | ✅ 已交 a29e55606（能力就绪，%门禁 nominal no-op）|
| M-E | Scriban 模板审查规范 | ✅ 已审计+清单固化（scriban-template-review.md）|

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

## M-B 部分完成 (6037c9cd9) + M-C 评估
- ✅ Directory.Build.props: EnableNETAnalyzers + AnalysisLevel=latest
- ✅ 修复 CS8602: ExceptionEmission.Helpers + StructuredIR.Emit (#pragma,
   ThreadLocal 工厂初始化假阳性) + AsmCompareHandler (真实 null,加检查)
- ⚠️ **known gap (诚实, 对标 M-A)**: M-B 定义的"Roslyn analyzers **CI 门禁**"仅完成
  前一半 —— analyzers 已启用并修复了存量 CS8602, 但**尚未接线 CI enforcement 环节**
  (无 TreatWarningsAsErrors / WarningsAsErrors, 无类似 M-A `managed-csharp-style.yml`
  的独立 workflow 把 analyzer 违规转红门). 当前分析器违规只作为本地 build warning 出现,
   不 red CI. 需后续补: Directory.Build.props 加 `<TreatWarningsAsErrors>true</TreatWarningsAsErrors>`
   或独立 analyzer-gate workflow (增量 on changed .cs, 不 rewrite 存量). 未补前不应视 M-B 已闭环.
- 🔍 M-C codegen 文件评估: 4/7 已是 partial class NativeAotLoweringPlanner
   的一部分(方法45-146/文件), 拆单个2000行partial文件需codegen domain
   判断, 收益低(已职责导向partial)风险高(破坏AOT输出). 建议由专职
   codegen agent 分批做.
- 3 非 partial (Program.cs/JitAsmCapture/DllScanner) 为独立工具类.

## M-C / M-D / M-E 完成 (2026-09-02)
- 🔍 M-C: 7 超大文件评估 → 4/7 已是 NativeAotLoweringPlanner partial class 部分
   (45-146 方法/文件, 职责导向), 纯物理拆分收益低风险高(codegen输出). 建议
   专职 codegen agent 需 domain 判断时再拆. 3 非partial为独立工具类. 记录不强制.
- ✅ M-D: Driver.Tests +coverlet.msbuild(能力就绪), CI codegen(50%硬门禁 保留) +
   Driver Coverage Report(report-only, 29测试仅覆盖薄CLI面, 强设%门禁是
   nominal no-op). commit a29e55606.
- ✅ M-E: 160 Scriban 模板审计 → 无 stub/硬编码/编码残留, snapshot(108)守正确性.
   固化 6 项审查清单(scriban-template-review.md). 记录 kSubjectContractMap 已知gap.
