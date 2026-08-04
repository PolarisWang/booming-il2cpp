# SYSTEMIC_ISSUES.md — 系统性 Bug 清单

> 基于 Phase 1 基线扫描识别。每个 SYS 具有公共根因，修复后会影响多个 family。

## SYS-001: Codegen — missing chaos_* macros in runtime headers

**类型**: codegen 基础设施
**影响范围**: 全量 family（~164）
**严重性**: 阻塞（Blocking）
**FAMILY_ORDER 中的优先级**: P0

**根因分析**:
Codegen 生成的 `native-aot.generated.cpp` 使用了以下标识符：
- `chaos_array`
- `chaos_value`
- `chaos_managed_pointer_local_slot_tag`
- `chaos_is_type_compatible`

这些标识符在 verification entry 项目的运行时头文件中未定义。现有的 `entry-aot.exe` 是从之前 codegen 版本构建的（当时未发射这些宏），因此当前 pre-built 二进制文件仍可运行事实验证。

**已影响的 family**: tag-model, decoder, convert-char, custom-generics, triple-composite, 以及任何需要重建 entry.exe 的 family

**建议修复方案**:
1. 在 `src/native/common/chaos/` 或 verification 包含的通用头中添加这些宏定义
2. 或者在 codegen 发射侧将这些宏替换为已存在的等效标识

## SYS-002: Missing capability-family-contract.json for 4 families

**类型**: contract 配置
**影响范围**: 4 families
**严重性**: 低
**FAMILY_ORDER 中的优先级**: P3

**根因分析**:
这些 family 有 `unified-verification-report.json` 但缺少 `contract.json` 和 `capability-family-contract.json`。预检阶段因找不到 capability 合同而失败。

**已影响的 family**:
- `--family/pinvoke-dllimport`
- `--family/threading-monitor-interlocked`
- `System.Runtime.Serialization/surrogates-selection`
- `System.Security.Claims/windows-identity`

**建议修复方案**:
为每个 family 创建最小的 `contract.json` 和 `capability-family-contract.json`，或确认这些 stub family 是否需要保留。

## SYS-003: GC.LatencyMode not available in target runtime

**类型**: managed subjects 兼容性
**影响范围**: 1 family (garbage-collection)
**严重性**: 中
**FAMILY_ORDER 中的优先级**: P0

**根因分析**:
`GarbageCollectionSubjects.cs` 引用了 `GC.LatencyMode` 属性，该属性是 CoreCLR GC API 的一部分。我们的 CRAG GC 实现没有暴露此 API。MSBuild 编译 subjects DLL 时因找不到 `GC.LatencyMode` 而失败。

**已影响的 family**:
- `System.Private.CoreLib/garbage-collection`

**建议修复方案**:
1. 在 CRAG GC 中添加 `GC.LatencyMode` 兼容 API（推荐）
2. 或修改 subjects 代码移除对 `GC.LatencyMode` 的引用（回避，可能导致测试覆盖不足）

## SYS-004: p1_lowering principle VIOLATION

**类型**: codegen 优化覆盖
**影响范围**: 1 family (document-element)
**严重性**: 中
**FAMILY_ORDER 中的优先级**: P0

**根因分析**:
`document-element` family 有 69 个方法（含 custom entries），但 codegen 对所有这些方法都只生成了 simple forwards 或 stubs，没有任何 lowering（IR lowering 优化）。导致 audit 阶段 `p1_lowering` 检查判为 `VIOLATION`。

这是 codegen 优化覆盖面的问题 —— 某些类型的方法（特别是 JSON document/element 操作）可能未被优化管线的 lowering pass 覆盖。

**已影响的 family**:
- `System.Text.Json/document-element`

**建议修复方案**:
1. 在 codegen lowering pass 中增加对 document-element 相关操作的支持
2. 或确认该 VIOLATION 是否可以接受为已知限制（升级为 CONCERN 而非 VIOLATION）
