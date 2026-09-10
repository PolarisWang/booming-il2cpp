# Handoff — `OverrideUnresolvedExternalRuntimeEntries` segfault (CoreLib system chunk)

> 交接给：**async Task 工业化 agent**（`async-task-industrialization` 线）或 **codegen 域专家**。
> 交接日期：2026-09-10
> 交接人：nightly-runner-refactor / ATG / gating 线 session

## 一句话 TL;DR

CoreLib `system` chunk 用当前 HEAD 的 Generator 重建后，`entry.exe --fact-json`
**在 `ChaosRuntimeHost::OverrideUnresolvedExternalRuntimeEntries()` 内 SIGSEGV**，
零 stdout 输出，pipeline 报 `2181 methods dropped vs metadata — SEVERE (100%)`。
**根因指向 async 工业化 commit 组合**把 `kChaosExternalRuntimeCount` 从 1616 推到 2147，
使该函数的 hotpatch 修复循环读到了非法 subject-id 指针。**与 ATG / gating / sidecar 改动无关**
（这些已在小 chunk 上独立验证通过）。

## 复现

```bash
cd tests/e2e
rm -rf artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native/{build,codegen,subjects} \
       artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native/entry.exe
CHAOS_FOUNDATION_DLL="$PWD/translation" python -m verification.chunk_pipeline \
    --assembly System.Private.CoreLib --chunk system --stages build,fact
# => TPG build OK, entry.exe 生成, fact 阶段 "2181 methods dropped — SEVERE (100%)"
```

直接运行取证：

```bash
cd artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native
./entry.exe --fact-json    # 无 stdout，exit 139 (SIGSEGV)
```

## 崩溃定位（cdb 实测）

```
Access violation - code c0000005
ExceptionAddress: chaos_entry!ChaosRuntimeHost::OverrideUnresolvedExternalRuntimeEntries+0x1b5
NumberParameters: 2
   Parameter[0]: 0000000000000000
   Parameter[1]: 00007ff4ea069df0        ← "Attempt to read from address 00007ff4ea069df0"
Call Site:
  chaos_entry!ChaosRuntimeHost::OverrideUnresolvedExternalRuntimeEntries+0x1b5
  chaos_entry!main+0x29d
```

（cdb 路径：`C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe`；
命令：`cdb -c 'g; .exr -1; kb 30; q' ./entry.exe --fact-json`）

`0x7ff4...` 不在 exe 映射区（exe 在 `0x7ff6...`），是一次**野指针读取**。

## 关键数据对比（新旧构建）

| 符号 | 旧（working，Sep-8） | 新（segfault，当前 HEAD） | Δ |
|------|:---:|:---:|:---:|
| `kChaosExternalRuntimeCount` | 1616 | **2147** | **+531** |
| `kSubjectEntryCount` | 3779 | 2885 | −894 |
| `kAotMethodCount` | 8500 | 9452 | +952 |
| `ChaosGeneratedModuleActivate` | ✅ 存在且一致 | ✅ 存在且一致 | — |
| `kChaosExternalRuntimeFnTable[N]` / `Subjects[N]` | 1616 | 2147（数组容量与 Count 一致 ✅） | — |
| `kSubjectSlotMap` 越界检查 | — | max=9451 < kAotMethodCount=9452，**0 越界** | — |

外部条目数据本身是干净的（2147 条 subject-id，无空/畸形条目）：
```
System.Private.CoreLib      1989
Chaos.TestFramework.Sdk       67
CombinedSubjects              55
System.Memory                 33
System.Linq                    2
System.Console                 1
```

## 新增 558 条 external entry 的类型分布（旧→新的差集）

```
 91  System.Type
 90  System.String
 43  System.IO.TextWriter
 34  System.Attribute
 32  System.Enum            ← 含本会话的 Enum.TryParse inline 改动贡献
 27  System.AppDomain
 22  System.Char
 17  Chaos.TestFramework.SubjectInstanceFactory
 13  System.Delegate
 12  System.DateTime
 11  System.TimeZoneInfo
  9  System.ModuleHandle
  8  System.TimeSpan
  8  System.MemoryExtensions+TryWriteInterpolatedStringHandler
  7  System.TimeOnly
```

## 嫌疑代码

`src/native/runtime-core/chaos_runtime_host.h:217` `OverrideUnresolvedExternalRuntimeEntries()`

该函数的安全设计（注释 `h:213-230`）：
- 栈上 `const char* overridden_subjects[4096]` / `int32_t overridden_indices[4096]`
- 只对**非白名单 assembly**（非 `CombinedSubjects/`、`System.Private.CoreLib/`、
  `Chaos.TestFramework.Sdk/`）的条目做 `VirtualProtect` + 置 `nullptr`
- 之后在 hotpatch 修复循环里解引用 `overridden_subjects[oi]` 解析 subject-id

**需要查的点**：

1. **`kMaxOverrides = 4096` 是否仍安全**？当前 2147 条 external entry，但白名单过滤前
   会先遍历全部；若未来增长到接近 4096 会栈溢出（32KB×2 数组）。
2. **`overridden_subjects[]` 里存的 `sid` 指针生命周期**：它指向
   `kChaosExternalRuntimeSubjects[i]`（const 字符串字面量，应在 .rdata）——确认新的
   codegen 是否把某些 subject-id 生成为**非字面量**（例如拼接的 `std::string` 或
   指向已析构临时对象的指针）。这是最可能的野指针来源。
3. **hotpatch 修复循环里的 registry 查找**（`GetHotpatchNameRegistry()`）在 2147 条
   规模下是否仍正确（遍历/哈希表容量假设）。
4. **新增的 async 方法**（`Task.Delay` / `TaskCompletionSource` / `WhenAll` 等，
   见 commit `3cf2e453` `4bb25287` `feccdb16`）是否注册了 direct_ptr 指向
   bridge thunk 而非真实函数，导致后续调用路径跳进无效地址。

## 推荐排查步骤

```bash
# 1. 用 cdb 在崩溃点看 RSI/RDI 等寄存器，确认是哪个数组、哪个索引
cdb -c 'g; .exr -1; r; kb; q' ./entry.exe --fact-json

# 2. 打印 overridden_subjects[oi] 与 overridden_indices[oi] 的实际值
#    （建议临时加 fprintf 到 chaos_runtime_host.h:310 附近的循环）

# 3. 二分：逐个 revert async commit 重建，确认哪一个引入
git log --oneline --since=2026-09-08 -- src/managed/Chaos.IL2CPP.Generator/ src/native/runtime-core/
#   候选：3cf2e453(TCS) 4bb25287(Task.Delay) feccdb16(WhenAll/WhenAny) 6bdc6ea6(Task.Delay native)
```

## 重要：这不是什么

- ❌ 不是 ATG / gating / honest_report / sidecar 改动引起 —— 这些改动只触及
  `src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/`、`tests/e2e/verification/`，
  已在小 chunk（`System.Collections.Immutable`、`System.ObjectModel` 等）上独立验证
  `build+fact` 2/2 通过。
- ❌ 不是 `kSubjectSlotMap` 越界（max=9451 < 9452）。
- ❌ 不是 `ChaosGeneratedModuleActivate` 缺失（新旧一致存在）。
- ❌ 不是 `kChaosExternalRuntimeFnTable` / `Subjects` 数组容量不匹配（均 2147 = Count）。

## 影响面

**所有含大量 external-runtime 条目的 chunk 的原生重建都会 segfault**。
小 chunk（如 Immutable，external 条目少）不受影响，仍可正常 `build+fact`。
在修复前，**全量 nightly 的 CoreLib 相关 chunk 会持续失败**。

## 本会话已完成并推送的产物（参考，勿重做）

```
4df92cc2  feat(build): promote inline-native wrappers to NativeGenerated via sidecar
8a980bda  feat(codegen): emit inline-natives sidecar for InlineShapeDescriptor
48203260  fix(gate): unify real-vs-smoke classification at the source (fact_chunk)
1cd1c02a  fix(atg): ZipFile chunk — real ZipArchive fixture
c597fa67  fix(atg): add real-instance construction for 6 smoke chunks
1c37336f  fix(codegen): Enum.TryParse inline→ChaosEnumTryParse
```
