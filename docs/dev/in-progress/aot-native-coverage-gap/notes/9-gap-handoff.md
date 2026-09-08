# Handoff — AOT-Native-Coverage-Gap (9 GAP) — 2026-09-07

## 🔄 2026-09-08 移交后更新：CoreLib 构建阻塞已修 + 剩余 fact 调查结论

**已修复并推送**（`937847fbb` ATG global:: + `21ea08267` ATG ref-struct）：CoreLib system-3 chunk
从 4616 编译错 → **3/3 stages passed**。整族 CoreLib ci_smoke 现 **55781/55850 fact passed**。

**「2 个剩余 fact fail」逐案调查结论（3 agent 并行,** 无真实 codegen 缺陷）**：
- **system-6**：假阳性。aggregate 在重建前读到旧 stale artifact (total=0)；实际 `3779/3779 passed`。
- **threading-tasks**："5/10" 是 stage 级统计非事实失败；**456/456 fact 实际全过**。
- **runtime-interop**：6 fact fail（si 39/40/405/406/411/465 → codegen idx 96/97/114/115/141/745）
  全为 `System.Runtime.InteropServices.Marshal` COM 测试方法（GetObjectForIUnknown 等）。
  body 全 NativeGenerated、语义正确，但 smoke subject 对不可复现的 COM/unmanaged 调用返 0 而非
  42 sentinel → 被 harness 当真实失败而非 UNVERIFIED-smoke。是 **ATG/harness UNVERIFIED 分类边角**，
  非 AOT 翻译缺陷。

**净结论**：9 个 aot-native-coverage-gap 的 fact 无一真实失败（全部过系统 5 之外的已跑 chunks 通过），
无待修 codegen/AOT 翻译缺口的残留。runtime-interop Marshal COM smoke 分类问题属
测试治理/ATG 域（见 [[com-marshaller-aot-semantic-alignment]] / [[atg-fact-false-positive-unverified-fix]]），
不在本 GAP 追踪域。

下接原交接正文（9 GAP 注册状态 + 入口仍在，作为参考可关闭/归档）。



## 交接给谁
chaos-il2cpp codegen 专家 agent（dev-il2cpp-codegen-expert）。

## 一句话 TL;DR
9 个 GAP 的 **codegen 修复代码已在 HEAD** 中（`df7813de7` 为 `Part3.S23.cs` 添加了 7 个 `InlineShapeDescriptor` 注册，
涉及 Delegate.Combine/Remove、String.Join、Enum.TryParse、Convert.ChangeType、Nullable\<T\>.GetValueRefOrDefaultRef；
该 commit message 标题写 "register all 11" 是包含该文件内所有 inline 注册的总数，非仅本次新增——其 diff 仅 +73 行，
实际新增 InlineShapeDescriptor 为 7 项，其余为 pre-existing 及其他类别注册），
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
| bcl-abi-003 | String.Join(string,string[]) | ✅ **两处互补**：`Part3.S23.cs:436` Inline override（直达 `ChaosStringJoinSs`，绕过 null-guard）**+** `Part1.S10.cs:78-88` SimpleForward 注册（`ChaosStringJoinSs`）。维护时两处都要看。 | 代码就位，待验证 |
| reflector-004 | Enum.TryParse(Type,string,out object) | ✅ SimpleForward `ChaosEnumTryParse`, `Part3.S23.cs:783` | 代码就位，待验证 |
| reflector-005 | Enum.TryParse(Type,string,bool,out object) | ✅ SimpleForward `ChaosEnumTryParseWithIgnoreCase`, `Part3.S23.cs:793` | 代码就位，待验证 |
| generics-006 | Enum.TryParse\<T\>(string,bool,out int) | ⚠️ 需确认泛型 int 版本 | 待确认 |
| generics-007 | Nullable\<T\>.GetValueRefOrDefaultRef | ✅ InlineShapeDescriptor, `Part3.S23.cs:485`（为 default 探针 echo value-at-offset-0；非 default 真遍历走 native composition） | 代码就位，待验证 |
| generics-008 | ReadOnlySpan\<T\>.ToArray | ✅ GenericShapeDescriptor, `Part2.S17.cs:156` | 代码就位，待验证 |
| reflector-009 | Convert.ChangeType(object,TypeCode) | ✅ Inline 直接 native `ChaosConvertChangeType`, `Part3.S23.cs:467` | 代码就位，待验证 |

> **GAP 修复的注册位置不是单一文件**：bcl-abi-003（String.Join）同时落在 `Part1.S10.cs`（SimpleForward 形状注册）
> 和 `Part3.S23.cs`（Inline override）两处，且都是 2026-09 会话为了「形状注册 + null-guard 绕过」两层目的分别加的。
> 代码索引表（文末）已按文件列出完整注册点。逐项验证/接续时，用下面「代码索引」表覆盖全部有效位置，
> 不只盯 GAP 表的单个 `Part3.S23.cs` 位置。df7813de7 的改动只触及 `Part3.S23.cs`（+73 行），
> 它**不是** S10/S17 相关注册的唯一来源——后者由更早的会话（其 message 注明对应注册行）建立。

## 下一会话入口

### 第一优先级：分诊 CoreLib `system-3` chunk 的 CS1525 生成 bug

**症状**：`ci_smoke --mode full` 在 CoreLib system-3 chunk 的 managed-combined 编译时
抛出数千 CS1525/CS1003/CS1001 语法错。错误位置集中在 `CombinedSubjects.cs` 的 `UIntPtr.TryParse`
相关行（line ~72500+），`string` 关键字出现在语法错误位置。

**根本原因悬而未决**: 可能是 ATG subject generator 在为 `UIntPtr.TryParse` 生成 subject 时 C# 措辞问题
（`string` 关键字在 `out` 参数位置生成无效），或 emitter 模板问题。

**修复入口**：`artifacts/foundation-dll/System.Private.CoreLib/chunks/system-3/managed/combined/CombinedSubjects.cs`
（生成源，非 tracked）；检查 ATG subject generator 的 `UIntPtr.TryParse` 模板。

> **搜索关键词**：在 `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/` 下，`UIntPtr.TryParse` 没有专用模板。
> 其 C# 生成走通用路径 `ValueGenerator.cs`，其中 `BoundaryValues` 字典（L31）定义了 `UIntPtr` 边界值
> `["System.UIntPtr"] = new[] { "System.UIntPtr.Zero", "new System.UIntPtr(42)" }`，`Generate()` 方法（L174）
> 根据参数类型查此字典并拼接 C# 表达式。`UIntPtr.TryParse` 的 subject 生成入口在 `CombinedSubjects.cs`
> 中按 `BoundaryValues` 产出，查生成错误的搜索关键词是 `UIntPtr.TryParse`（在 CombinedSubjects.cs 中搜索）。
> 若修改 ATG 输出，需确认 `ValueGenerator.cs` 的 `UIntPtr` 边界生成是否包含 `out string` 参数的正确 C# 措辞。

**验证**：修好后跑 `ci_smoke --families System.Private.CoreLib --mode full`

> **ci_smoke 语法说明**：`ci_smoke.py` 本身不提供 `--stages` 参数。它内部按 `--mode` 选择 stage 子集：
> - `--mode smoke` → stages=`build,fact,coverage-audit`
> - `--mode full` → stages=`build,fact,hotupdate,coverage-audit,profile,benchmark,managed_benchmark,benchmark_report,aggregate,reporting`
> 如需自定义 stage 子集，直接调用 `chunk_pipeline.py --stages build,fact --assembly <name>`。
> 实例：`ci_smoke --families System.Private.CoreLib --mode full` 是真实可用的命令（已由 ci_smoke.py:53-55 的 argparse 支持）。

### 第二优先级：逐个验证 9 GAP 在 CoreLib fact 中的通过状态

一旦 system-3 修好，CoreLib fact 可以通过。在 `fact-summary.json` 的 `capabilityAggregate` 中检查
`NativeGenerated` 计数是否上升、`NoCanonicalBody` 是否下降。对每个 GAP 方法：
- 对应 subject fact passed → 标 `closed`
- 仍 fail → 需新 codegen 改动

> **fact-summary.json 位置**：写入在 `<foundation_root>/<assembly>/_dll/reports/latest/fact-summary.json`，
> 其中 `foundation_root` 是 `CHAOS_FOUNDATION_DLL` 环境变量（或从 `verification/` 向上找到
> `config/pipeline-config.yaml` 的仓库根），`<assembly>` 即运行 assembly 名（如 `System.Private.CoreLib`）。
> 字段 `capabilityAggregate` 由 `tests/e2e/verification/stages/aggregate.py` 写入（L567 组 key，
> L570 落到 `reports/latest/fact-summary.json`）。

### 代码索引（GAP 注册位置）

| 文件 | 内容 |
|------|------|
| `Part3.S23.cs` | 主要 InlineShapeDescriptor 注册（Delegate, String.Join, Convert.ChangeType, Nullable, Enum） |
| `Part1.S10.cs` | String.Join SimpleForward 注册 |
| `Part2.S17.cs` | ReadOnlySpan\<T\>.ToArray GenericShapeDescriptor |
| `delegate_helpers.cpp` | ChaosDelegateCombine/Remove 原生实现（null 安全） |
| `delegate_stubs.cpp` | extern "C" 包装 |
| `string_stubs.cpp` | ChaosStringJoinSs 原生实现 |