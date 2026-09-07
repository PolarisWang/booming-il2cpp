# Handoff — AOT-Native-Coverage-Gap (9 GAP) — 2026-09-07

## 交接给谁
chaos-il2cpp codegen 专家 agent（dev-il2cpp-codegen-expert）。

## 一句话 TL;DR
9 个 GAP 的 **codegen 修复代码已在 HEAD** 中（`df7813de7` 为 7 项添加了 InlineShapeDescriptor 绕过 null-guard），
但 foundation-dll full 验证被 **CoreLib `system-3` chunk 的 managed-combined CS1525 生成 bug** 阻塞，
无法在 fact 层面确认这些修复是否真实生效。需要先修好这个生成 bug，然后跑 CoreLib 的 fact 逐项验证。

## 已修复并合入 main（本会话产出）

| Commit | 修复 | 效果 |
|--------|------|------|
| `a46d4be49` | PatchDataExtractor subject-only mode 产出 AotCoreIr section + 真 sentinel operand | hotupdate semantic_changed 0→30 |
| `58b6d05ef` | 交接文档 E2E 验证记录 | 闭环记录 |
| `3d1683e8c` | aot-native-coverage-gap STATUS.md 更新 | 反映当前 9 GAP 真状态 |

## 9 GAP 真实状态（2026-09-07 调查结论）

| GAP | 方法 | HEAD 中的注册 | 当前状态 |
|-----|------|:---|:---:|
| bcl-abi-001 | Delegate.Combine | ✅ InlineShapeDescriptor, `Part3.S23.cs:419` | 代码就位，待验证 |
| bcl-abi-002 | Delegate.Remove | ✅ InlineShapeDescriptor, `Part3.S23.cs:427` | 代码就位，待验证 |
| bcl-abi-003 | String.Join(string,string[]) | ✅ SimpleForward `ChaosStringJoinSs` + Inline, `Part3.S23.cs:436` | 代码就位，待验证 |
| reflector-004 | Enum.TryParse(Type,string,out object) | ✅ SimpleForward `ChaosEnumTryParse`, `Part3.S23.cs:783` | 代码就位，待验证 |
| reflector-005 | Enum.TryParse(Type,string,bool,out object) | ✅ SimpleForward `ChaosEnumTryParseWithIgnoreCase`, `Part3.S23.cs:793` | 代码就位，待验证 |
| generics-006 | Enum.TryParse\<T\>(string,bool,out int) | ⚠️ 需确认泛型 int 版本 | 待确认 |
| generics-007 | Nullable\<T\>.GetValueRefOrDefaultRef | ✅ InlineShapeDescriptor, `Part3.S23.cs:485` | 代码就位，待验证 |
| generics-008 | ReadOnlySpan\<T\>.ToArray | ✅ GenericShapeDescriptor, `Part2.S17.cs:156` | 代码就位，待验证 |
| reflector-009 | Convert.ChangeType(object,TypeCode) | ✅ Inline 直接 native, `Part3.S23.cs:467` | 代码就位，待验证 |

修复代码在 `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part3.S23.cs`。

## 下一会话入口

### 第一优先级：分诊 CoreLib `system-3` chunk 的 CS1525 生成 bug

**症状**：`ci_smoke --mode full` 在 CoreLib system-3 chunk 的 managed-combined 编译时
抛出数千 CS1525/CS1003/CS1001 语法错。错误位置集中在 `CombinedSubjects.cs` 的 `UIntPtr.TryParse`
相关行（line ~72500+），`string` 关键字出现在语法错误位置。

**根本原因悬而未决**: 可能是 ATG subject generator 在为 `UIntPtr.TryParse` 生成 subject 时 C# 措辞问题
（`string` 关键字在 `out` 参数位置生成无效），或 emitter 模板问题。

**修复入口**：`artifacts/foundation-dll/System.Private.CoreLib/chunks/system-3/managed/combined/CombinedSubjects.cs`
（生成源，非 tracked）；检查 ATG subject generator 的 `UIntPtr.TryParse` 模板。

**验证**：修好后跑 `ci_smoke --families System.Private.CoreLib --mode full`（或子集 stages=build,fact）。

### 第二优先级：逐个验证 9 GAP 在 CoreLib fact 中的通过状态

一旦 system-3 修好，CoreLib fact 可以通过。在 `fact-summary.json` 的 `capabilityAggregate` 中检查
`NativeGenerated` 计数是否上升、`NoCanonicalBody` 是否下降。对每个 GAP 方法：
- 对应 subject fact passed → 标 `closed`
- 仍 fail → 需新 codegen 改动

### 代码索引（GAP 注册位置）

| 文件 | 内容 |
|------|------|
| `Part3.S23.cs` | 主要 InlineShapeDescriptor 注册（Delegate, String.Join, Convert.ChangeType, Nullable, Enum） |
| `Part1.S10.cs` | String.Join SimpleForward 注册 |
| `Part2.S17.cs` | ReadOnlySpan\<T\>.ToArray GenericShapeDescriptor |
| `delegate_helpers.cpp` | ChaosDelegateCombine/Remove 原生实现（null 安全） |
| `delegate_stubs.cpp` | extern "C" 包装 |
| `string_stubs.cpp` | ChaosStringJoinSs 原生实现 |