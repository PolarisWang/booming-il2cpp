# AOT-Native-Coverage-Gap — AOT 原生翻译覆盖缺口闭合(工业化命名)

> 原名为"fact-266"(内部 pipeline 序号代号,已完成大量但语义不可读、外部协作看不懂)。
> 本任务以**能力语义**命名并作为长期追踪基线,用翻译追踪(见 wiki 的 capabilitySummary/
> translation_tracker.py)对每个剩余缺口做量化闸门:修复一个 → `capabilityAggregate.NativeGenerated`
> 占比应单调上升 / `NoCanonicalBody` 下降。

## 命名规则

- **AOT-Native-Coverage-Gap**:一个方法走了 interpreter/ExternalRuntime-fallback 而没有真实 AOT C++
  翻译。等同旧的"翻译墙"概念。
- 每缺口有 稳定 ID:`GAP-<domain>-<NNN>`(domain ∈ {bcl-abi, reflector, generics, interop, enum,
  nullable})。
- 生命周期:open(登记)→ fixing(提交中)→ flaky/known-interim(噪声)→ closed(native 实现 +
  fact 绿)。
- 关闭判定 = 该 subject 在 foundation-dll full 下 fact passed + translation_tracker.py 显示该
  方法已记 NativeGenerated / 不再出现在 NoCanonicalBody 中。

## 剩余缺口清单(GAP-ID 登记,合并自原 fact-266 11-remain 基线 fact 3522/3533)

根因在 codegen planner + registry inline/native stub,修复需 codegen 域 + 每次 ~10min pipeline 验证。

### 类 A — reference-type null-guard 墙 (已注册 native,差 inline 绕过) 3
- GAP-bcl-abi-001 Delegate.Combine — native ChaosDelegateCombine 已就位, codegen null-guard 先 raise_NRE
- GAP-bcl-abi-002 Delegate.Remove  — 同上 ChaosDelegateRemove
- GAP-bcl-abi-003 String.Join(string,string[]) — null 数组/参数 ABI

### 类 B — SimpleForward 泛型匹配 3
- GAP-reflector-004 Enum.TryParse(Type,string,out object)
- GAP-reflector-005 Enum.TryParse(Type,string,bool,out object)
- GAP-generics-006 Enum.TryParse<T>(string,bool, out int) 泛型,未注册 int 版

### 类 C — 泛型 value-type 0-arg/ref-struct 墙 2
- GAP-generics-007 Nullable<int>.GetValueRefOrDefaultRef (value type ABI 16B → 0-arg)
- GAP-generics-008 ReadOnlySpan<int>.ToArray (ref struct 特殊 value type)

### 类 D — 类型系统大分发 2
- GAP-reflector-009 Convert.ChangeType(object, TypeCode[, IFormatProvider]) — TypeCode 映射大复杂度

## 进度
- (继承自原 fact-266) 已修 902 个, 基底 fact 3522/3533。BitConverter (原 2 个) native+inline
  已落 (ChaosBitConverterGetBytesFromSingle/Double 在 array_stubs.cpp:172/187), 若仍 fail 查 stale lib。
- 当前不在本 working-tree 修复(需 codegen session + 全 pipeline)。**执行移交点见本文件同目录
  notes/execution-handoff-...md**,或对每个 GAP 建独立 codegen 任务。

## 架构映射
- 修复分层: codegen 翻译路径 (NativeAotLoweringPlanner / registry InlineShapeDescriptor) +
  native runtime stub + CodeGenStage.Planning BodyAvailabilityCode。
- 追踪: 本篇命名是本会话的翻译追踪工作命名落点;capabilitySummary +
  translation_tracker.py 作为收敛度量(修复一个 → NativeGenerated 占比上升)。

## 检查点 (2026-09-07)
- 本目录 = 原 "fact-266" 的工业化重命名落点。重命名本身是纯文档/追踪语义变更,无代码行为改动。
- 实际 GAP 修复需 codegen 域会话 + 每次 ~10min foundation-dll 验证,**不在本 working-tree 直接改**(否则改了一半无法验证)。
- 并行 TPG agent 在本会话活动频繁(git lock 反复争用),重命名提交若与并行线交错,以后续 memory
  手记为准对齐。
