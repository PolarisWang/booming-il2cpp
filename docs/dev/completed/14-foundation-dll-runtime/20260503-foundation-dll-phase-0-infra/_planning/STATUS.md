---
task_id: foundation-dll-phase-0-infra
title: Phase 0 基础设施对齐与 CoreLib 摸底
task_type: plan
lifecycle_status: in-progress
phase: planning
created_at: 2026-05-02 01:00:00 +08:00
updated_at: 2026-05-02 02:00:00 +08:00
current_dir: docs/dev/in-progress/foundation-dll-phase-0-infra
parent_task_id: foundation-dll-full-development
parent_task_phase: phase-0
parent_task_relation: child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
design_doc: n/a
plan_doc: n/a
arch_review_mode: critical
---

# Phase 0 基础设施对齐与 CoreLib 摸底

## Scope
建立从 DLL → IL body → emitter 覆盖的自动化分析工具链，并对 System.Private.CoreLib 24 families 做完整摸底。

## 调研结论

### IL 分析源
- **DLL 位置**: `assets/framework-packs/dotnet-foundation/net10/runtime/` 包含所有 165+ 托管 DLL 的完整 IL body
- **Loader 已解码 IL**: 但嵌入在完整 pipeline 中，非独立工具，不适合直接复用

### Emitter 现状
- `NativeAotLoweringPlanner.MethodEmission.cs` 中 switch-case 已有 **97 个 IL opcode** 的分派
- opcode 使用 `string` 类型（非 enum）
- 缺失 opcode 直接抛 `NotSupportedException`

### Mono.Cecil 可用性
- `third_party/mono.cecil/netstandard/Mono.Cecil.dll` 已 vender，可直接使用

## 实现方案

### 方案设计

选用 **C# Mono.Cecil 扫描器 + Python 协调** 的双层架构：

**C# 扫描器 (`src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/ILScanner/`)**
- 使用 Mono.Cecil 读取 DLL
- 对每个 method 提取 IL opcode 序列
- 输出 JSON（per-method 的 opcode 列表 + 签名）

**Python 分析器 (`build/toolchains/run/testing/foundation_dll/il_coverage_analyzer.py`)**
- 调用 C# 扫描器获取 IL 数据
- 读取 family contract 文件（methodSubjectIds）
- 读取 emitter switch-case（解析 MethodEmission.cs 中的 case 语句）
- 对比产出缺口报告

### 文件列表

| 文件 | 职责 |
|------|------|
| `src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/ILScanner/Program.cs` | 主入口：加载 DLL，扫描 IL |
| `src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/ILScanner/ILScanner.csproj` | 项目文件，引用 Mono.Cecil |
| `build/toolchains/run/testing/foundation_dll/il_coverage_analyzer.py` | Python 分析器：对比 emitter 覆盖 |
| `build/toolchains/run/testing/foundation_dll/emitter_coverage_known.py` | 已知 emitter 覆盖的常量定义 |

### 数据结构

```json
// IL Scanner 输出 (per DLL)
{
  "assemblyName": "System.Private.CoreLib",
  "scannedAt": "...",
  "methods": [
    {
      "name": "System.String.IsNullOrEmpty",
      "methodSubjectId": "...",
      "ilOpcodes": ["ldarg.0", "ldnull", "ceq", "ldarg.0", "ldlen", "brfalse", ...],
      "uniqueOpcodes": ["ldarg.0", "ldnull", "ceq", "ldlen", "brfalse"],
      "parameterCount": 1,
      "returnType": "bool",
      "hasBody": true
    }
  ]
}

// Emitter Coverage 已知
{
  "knownOpcodes": ["ldc.i4", "ldc.i8", ..., "ret"],  // 97 opcodes
  "missingOpcodes": []
}

// Gap Report (per DLL)
{
  "assemblyName": "System.Private.CoreLib",
  "totalMethods": 50000,
  "scannedMethods": 48000,
  "methodsNoBody": 2000,
  "emitterKnownOpcodes": 97,
  "opcodesNeededButMissing": ["arglist", "mkrefany", ...],
  "familyGaps": [
    {
      "familyId": "family/System.Private.CoreLib/convert-char",
      "methodsNeedingNewOpcodes": [...]
    }
  ]
}
```

## Exit Criteria
- [ ] IL 扫描器可对任意 DLL 输出 per-method IL opcode JSON
- [ ] Emitter coverage 分析可自动输出 gap 报告
- [ ] CoreLib 24 families 扫描完成，缺口报告已归档
- [ ] CoreLib 摸底报告包含：现有验证状态、emitter 缺口、各 family 复杂度评估

## 关键路径依赖
- `assets/framework-packs/dotnet-foundation/net10/runtime/` 作为 IL 源
- `third_party/mono.cecil/netstandard/Mono.Cecil.dll` 作为 IL 读取库
- `src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` 作为 emitter 覆盖基线
- `verification/foundation-dll/System.Private.CoreLib/*/method-capability-contracts.json` 作为 family 分组依据
