# Golden Record 验证机制

## 概述

Golden Record 是 foundation-dll 验证管线中的一个阶段（`managed_record`），用于解决 subjects DLL 中 handwritten 方法在 managed 侧的验证遗漏问题。

核心思路：**直接运行 subjects DLL（包含 Custom.cs handwritten 代码）** 在 managed .NET 下，记录每个 `Subject_N()` / `CustomEntrySubject_N()` 方法的 pass/fail 状态，生成 golden-record.json 供后续 cross_verify 与 AOT entry.exe --fact-json 严格对比。

## 解决的问题

原有 `managed_fact` 阶段从 contract 重新生成 C# harness，**不包含 handwritten Custom.cs 代码**，导致：

| 问题 | 说明 |
|------|------|
| CustomEntrySubject 方法缺失 | handwritten 方法完全不在 managed harness 覆盖范围内 |
| cross_verify 过于宽松 | 认为 "managed fail 但 native pass 是方法论差异"，掩盖真实回归 |
| 验证链路不完整 | 无法判断 AOT 翻译是否改变了 handwritten 代码的行为 |

## 设计

### 架构

```
managed/subjects/*Subjects.csproj
  │
  │ ProjectReference
  │
  ▼
managed_test/subject_record/ManagedSubjectRecord.cs   ← 自动生成 harness
  │
  │ dotnet run
  │
  ▼
native/golden-record.json                              ← Golden Record
  │
  │ cross_verify 读取 + entry.exe --fact-json
  │
  ▼
  逐方法严格对比 pass/fail
```

### 关键设计决策

1. **直接使用 subjects DLL（ProjectReference）**
   - harness 通过 `<ProjectReference>` 引用 subjects .csproj
   - MSBuild 自动解析所有传递依赖（Chaos.TestFramework.Sdk 等）
   - 包含 Custom.cs 全部 handwritten 代码

2. **Managed vs AOT 语义对齐**
   - Subject 方法内部 catch 异常并设置 `_exitCode=1`，这是**预期行为**（API call failed but was handled）
   - Managed harness 的 `Passed = true` 表示"方法执行完毕，未发生未处理异常"
   - AOT fact-json 的 `passed = true` 表示"CHAOS_EH_TRY/CATCH 未捕获异常"
   - 两者语义等价

3. **Harness 模式**
   - 新建独立 EXE，不修改 subjects DLL
   - 逐个方法调用，每个方法 reset `_exitCode=0`
   - 外层 try/catch 仅捕获未处理异常，不影响后续方法

### 输出格式

```json
{
  "family": "convert-char",
  "assembly": "System.Private.CoreLib",
  "methodCount": 18,
  "results": [
    {
      "methodIndex": 0,
      "subjectName": "Subject_0",
      "isCustom": false,
      "passed": true,
      "exitCode": 1,
      "exceptionMessage": null
    }
  ]
}
```

## 管线位置

`managed_record` 阶段位于 `managed_fact` 之后、`cross_verify` 之前：

```
preflight → codegen → jit_codegen → managed_fact → managed_record → cross_verify → ...
```

## cross_verify 增强

`cross_verify` 阶段做了严格化改造：

- **Primary 数据源**：golden-record.json（`managed_record` 阶段生成）
- **Fallback 数据源**：golden-values.json（`managed_fact` 阶段生成，向后兼容）
- **严格对比**：managed `passed` 必须 == AOT `passed`
- 移除了 "methodology difference" 宽松逻辑
- 输出详细 mismatch 报告（含 methodIndex、subjectName、managedPassed、nativePassed、exitCodes）

## 边界情况处理

| 情况 | 处理 |
|------|------|
| subjects 目录不存在 | skipped |
| subjects DLL 构建失败 | failed + 错误信息 |
| 部分方法抛出未处理异常 | catch, 记录 exceptionMessage, passed=false |
| non-auto-callable + 不在 customEntryIndices | 对应方法不在 subjects DLL 中（由 codegen 保证） |
| 0-method family | n/a |

## 文件清单

| 文件 | 说明 |
|------|------|
| `tests/e2e/verification/stages/managed_subject_record.py` | managed_record 阶段（新建） |
| `tests/e2e/verification/stages/fact.py` | cross_verify 增强（修改） |
| `tests/e2e/verification/orchestration/engine.py` | 管线集成（修改） |
