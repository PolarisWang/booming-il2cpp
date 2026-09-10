# ASM-COMPARE 诊断工具

## 概述

`asm-compare` 是 chaos-il2cpp CLI 下的一个诊断工具，用于对比同一个 managed 方法在 CoreCLR JIT 编译结果和 Chaos IL2CPP 管线翻译结果之间的差异。输出包含 10 个可选 section，覆盖 IL 级、AOT IR 级、Native Asm 级的逐层对比，以及量化指标和自动优化建议。

## 使用方式

```bash
chaos-il2cpp asm-compare <managed.dll|exe> --method <name> [options]
```

### 参数

| 参数 | 说明 |
|------|------|
| `<managed.dll\|exe>` | 托管程序集路径（必需） |
| `--method <name>` | 方法名，支持 `TypeName::MethodName` 或 `TypeName.MethodName` 格式 |
| `--methods <names>` | 逗号分隔的方法列表，批量模式（如 `Subject_0,Subject_1,Subject_2`） |
| `--method-subject-ids <ids>` | 逗号分隔的完整 methodSubjectId 列表，每个 id 格式为 `AssemblyName/TypeName::MethodName:ReturnType(Params)`。支持括号内逗号（如 `System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean,System.IFormatProvider)` 不会被错误分割）。用于 JIT 捕获时解析框架中的真实目标方法，而非 Subjects DLL 的包装器。 |
| `--output, -o <file>` | 输出到文件（默认 stdout） |
| `--sections <list>` | 逗号分隔的 section 列表（默认全部） |
| `--format <fmt>` | 输出格式：`text`（默认）或 `json` |
| `--keep-temp` | 保留临时管线输出（用于调试） |
| `--help, -h` | 显示帮助 |

### Section 列表

| Section | Phase | 内容 |
|---------|-------|------|
| header | 1 | 基本信息（binary, method, 地址, 大小, 指令数, signature 等） |
| il-dump | 1 | 原始 IL 字节码反汇编 |
| raw-jit | 1 | CoreCLR JIT 生成的原生 x86-64 指令（含 hex dump） |
| raw-cpp | 1 | IL2CPP 生成的 C++ 源码 |
| side-by-side | 2 | 语义块级对齐对比（SemanticAligner: entry/call/guard-null/box/return 等标签） |
| ir-trace | 2 | IL → AotCoreIR 逐指令映射，含 callee/operand/symbol/dispatch 详情 |
| metrics | 2 | 量化指标对比表，含指令数/代码体积/调用数/分支数/内存访问/基本块数 + IL offset 覆盖率 + opcode 分布 |
| inline-map | 2 | AOT IR 中所有 call 指令的详细展开，含 dispatch kind/nparams/return_type/constrained type |
| raw-aot | 3 | AOT 编译后原生 asm（MSVC 编译 .cpp → .obj → .text section 反汇编；需安装 VS2022 C++ 工作负载） |
| side-by-side | 2/3 | Phase 2: IR 级语义块对齐对比；Phase 3 升级: AOT native asm 可用时切换到 native-vs-native 分块对齐 |
| analysis | 2 | 自动优化机会检测（ExternalRuntime call / virtual dispatch / inline candidate / boxing / null guard / IR expansion / branch-heavy） |

## 实现架构

### 管线流程

```
输入: managed.dll + --method / --methods
    │
    ├─[0] 解析 methodSubjectId →
    │    提取 AssemblyName，通过 System.Reflection.Assembly.Load 定位
    │    目标程序集路径（如 System.Private.CoreLib.dll），
    │    附加到 AdditionalAssemblyPaths
    │
    ├─[1] IL2CPP PipelinePlan.Execute()
    │    └─ 加载入口 DLL + AdditionalAssemblyPaths → ManagedClosureResult
    │
    ├─[2] NativeAotEmitter.Generate()
    │    └─ NativeAotResult (含 GeneratedSources — C++ 文件)
    │
    ├─[3] JitAsmCapture.Capture()
    │    └─ JitCaptureResult (含反汇编指令列表 + IL body)
    │
    ├─[3b] NativeCompile.Compile() — Phase 3 MSVC 编译管线
    │    ├─ FindMsvc(): 发现 VS2022 vcvarsall.bat → 配置环境
    │    ├─ cl.exe /std:c++20 /c /EHsc: 编译 .cpp → .obj
    │    ├─ ExtractTextSection(): COFF .obj 解析器, 拼接 .text$mn 段
    │    ├─ ExtractFunctionBytes(): COFF 符号表解析, 函数级字节提取
    │    ├─ ExtractFunctionDisassembly(): dumpbin /DISASM 输出解析
    │    └─ JitAsmCapture.DisassembleRaw(): 内建解码器统一格式反汇编
    │
    └─[4] AsmCompareReport.Generate()
         └─ 分段文本报告 (10 sections)
              ├─ AsmSemanticAligner.Align() — IR 级语义块对齐 (Phase 2)
              └─ AsmSemanticAligner.AlignNativeVsNative() — native 级分块对齐 (Phase 3)
```

### 核心文件

| 文件 | 职责 |
|------|------|
| `AsmCompareConfig.cs` | CLI 参数解析，section 白名单，`SplitRespectingParens()` 处理括号内逗号 |
| `AsmCompareHandler.cs` | 主编排（methodSubjectId → AdditionalAssemblyPaths 解析 → PipelinePlan → NativeAotEmitter → JIT asm capture → MSVC native compile → report） |
| `JitAsmCapture.cs` | JIT 原生代码捕获 + x86-64 反汇编 + IL body 读取 + `DisassembleRaw()` + `ResolveMethodFromSubjectId()` 按参数类型匹配过载解析 |
| `NativeCompile.cs` | Phase 3 MSVC 编译管线：`FindMsvc()` 发现 VS2022 工具链、`Compile()` 编译 .cpp → .obj、COFF 解析 |
| `AsmCompareReport.cs` | 全 section 报告生成器（含 metrics/analysis 引擎） |
| `AsmSemanticAligner.cs` | IL offset 锚定的语义块对齐引擎 |

### Framework 方法解析

asm-compare 的 Subjects DLL 包含 `Subject_N` 包装器方法（每个包装器调用一个 framework 目标方法）。AOT 侧如果只处理 Subjects DLL，生成的代码只有包装器逻辑（约 9 条 IR 指令），而非 framework 中的真实方法逻辑。

通过 `--method-subject-ids` 传入完整 methodSubjectId（格式 `AssemblyName/TypeName::MethodName:ReturnType(Params)`），handler 从中提取 `AssemblyName` 部分，通过 `System.Reflection.Assembly.Load(assemblyName).Location` 解析目标程序集路径，传入 `ManagedClosureRequest.AdditionalAssemblyPaths`，使 pipeline 同时加载 Subjects DLL 和 framework 程序集。

这样 AOT 侧就能生成 framework 真实方法的 C++ 代码，与 JIT 侧的对比数据才有意义。

### JIT 过载解析

`JitAsmCapture.ResolveMethodFromSubjectId()` 从 methodSubjectId 解析真实目标方法。由于 `Convert.ToChar` 等框架方法有多达 18 个过载，仅按方法名匹配会始终返回第一个过载。

解析算法：
1. 从 SubjectId 提取 `MethodName:ReturnType(ParamType1,ParamType2,...)`
2. 通过 `Type.GetMethod()` 按名称找到候选方法列表
3. 逐个匹配参数类型（从 SubjectId 解析的参数字符串与 `MethodInfo.GetParameters()` 对比）
4. 只返回参数类型完全匹配的方法

### 参数解析

`AsmCompareConfig.SplitRespectingParens()` 用于解析 `--method-subject-ids` 的逗号分隔列表。由于 methodSubjectId 中包含方法签名括号（如 `ToChar:System.Char(System.Object,System.IFormatProvider)`），括号内也可能有逗号。

实现方式是跟踪括号深度：只在深度为 0 时按逗号分割，括号内的逗号被跳过。确保 `Subject_N` 索引与 methodSubjectId 列表一一对应，不会因错误分割导致偏移错位。

### JIT 捕获原理

1. `Assembly.LoadFrom` 加载托管 DLL
2. `RuntimeHelpers.PrepareMethod` 强制 Tier1 JIT
3. `MethodHandle.GetFunctionPointer()` 获取 native 入口地址
4. `VirtualQuery` Win32 API 获取代码内存区域
5. `Marshal.Copy` 读取原始字节
6. `FindMethodBoundary` 通过指令长度遍历 + `ret` 检测确定方法边界
7. 内建 x86-64 反汇编器（opcode-length table + group opcode dispatch）生成可读指令列表

### DisassembleRaw 公共方法

`JitAsmCapture.DisassembleRaw()` 是 Phase 3 新增的公共方法，使用同一套内建 x86-64 解码器反汇编原始字节（如从 .obj .text 段提取的代码）。输出格式与 RAW-JIT 一致，便于 SAIDE-BY-SIDE 对比。

```csharp
public static JitCaptureResult DisassembleRaw(byte[] code, ulong baseAddress, string? methodFullName = null)
```

- **code**: 原始字节数组（如 .text 段内容）
- **baseAddress**: 反汇编基地址（用于 rel32 目标计算）
- **methodFullName**: 可选，方法全名标记
- **返回**: `JitCaptureResult`，包含 `HexDump`、`Instructions` 等字段，与 JIT 捕获结果格式一致

### IL 读取

使用 `System.Reflection.Metadata` (`PEReader` + `MetadataReader`) 直接从 PE 文件读取 IL 字节码，不加载托管程序集。支持解析常见 IL 指令（包括 switch、短/长分支、metadata token 等）。

### NativeCompile — Phase 3 MSVC 编译管线

`NativeCompile` 是 Phase 3 的核心组件，负责将 IL2CPP 生成的 C++ 代码编译为原生 .obj 文件并反汇编。

#### NativeCompileResult 记录

```csharp
public sealed record NativeCompileResult
{
    public bool FoundMsvc { get; init; }
    public bool CompileSuccess { get; init; }
    public string? MsvcVersion { get; init; }
    public string? ObjectPath { get; init; }
    public string? ObjectSize { get; init; }
    public string? Disassembly { get; init; }       // dumpbin /DISASM 输出
    public JitCaptureResult? AotDisasm { get; init; } // 内建解码器输出
    public byte[]? TextSectionBytes { get; init; }
    public string? TextSectionName { get; init; }
}
```

#### Compile() 流程

1. **FindRepoRoot()**: 向上遍历目录寻找 CLAUDE.md + src/native 标记仓库根目录
2. **FindMsvc()**: 在 VS2022 候选路径中搜索 vcvarsall.bat：
   - Professional → Community → Enterprise
   - 运行 `vcvarsall.bat x64 && set` 捕获环境变量
   - 从 PATH 中定位 cl.exe
3. **GetIncludeDirs()**: 镜像 CMakeLists.txt include 路径（common、runtime-core、interpreter 等 12 个目录）
4. **编译**: `cl.exe /nologo /std:c++20 /c /EHsc /W3 /utf-8 /DCHAOS_IL2CPP_CONFIG_CHECK ...`
5. **ExtractTextSection()**: COFF .obj 解析器，读取 section headers 后拼接 `.text$mn` 等代码段
6. **ExtractFunctionBytes()**: COFF 符号表解析器，定位目标函数偏移量和尺寸，精确提取函数字节
7. **ExtractFunctionDisassembly()**: 解析 dumpbin /DISASM 输出，提取目标函数的反汇编行
8. **DisassembleRaw()**: 使用内建解码器对提取的字节做统一格式反汇编

#### MSVC 发现

```csharp
var candidates = new[]
{
    @"C:\Program Files\Microsoft Visual Studio\2022\Professional",
    @"C:\Program Files\Microsoft Visual Studio\2022\Community",
    @"C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
    @"C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional",
};
```

支持 Professional/Community/Enterprise 版本，通过 `ExtractMsvcVersion()` 从 cl.exe 路径中提取 MSVC 工具集版本号（如 `14.4x`）。

### 语义对齐引擎 (AsmSemanticAligner)

Phase 2/3 核心组件。Phase 2 负责将 JIT asm 和 AOT IR 在 IL offset 层面做语义级对齐；Phase 3 新增 native-vs-native 对齐：

- **输入 (Phase 2 IR 对齐)**: JitCaptureResult (含 IL body + JIT 指令) + AotCoreIrMethodArtifact
- **输入 (Phase 3 native 对齐)**: JitCaptureResult (JIT asm) + JitCaptureResult (AOT .text 反汇编)
- **对齐策略 (IR)**: 按 IL offset 分组 AOT IR → JIT 指令按比例映射到各 IL 指令 → 相邻低价值块合并
- **对齐策略 (native)**: 按 ret/jmp/jcc 边界分块检测 → 按位置对齐（第 N 个 JIT 块 ↔ 第 N 个 AOT 块）
- **语义标签**: `entry`, `call`, `return`, `guard-null`, `box`, `unbox`, `range-check`, `array-length`, `dispatch`
- **输出**: AlignedBlock 列表，每块含 IL 上下文 + AOT IR/asm + JIT asm + 语义标签

#### Phase 3 新增方法

| 方法 | 说明 |
|------|------|
| `AlignNativeVsNative()` | 将 JIT 和 AOT 的 asm 按基本块级别对齐（在 ret/jmp/jcc 处分块） |
| `DetectNativeBlocks()` | 检测 asm 指令流中的基本块边界 |
| `RenderNativeBlocks()` | 渲染 JIT vs AOT 双栏对比文本 |

### 分析引擎 (ANALYSIS section)

内置 8 种优化机会检测器：

| 检测器 | 严重度 | 触发条件 |
|--------|--------|----------|
| EXTERNAL_RUNTIME_CALL | INFO | ExternalRuntime dispatch 调用 |
| VIRTUAL_DISPATCH | INFO | VirtualDispatch/InterfaceDispatch 调用 |
| DIRECT_CALL_INLINE_CANDIDATE | INFO | 同 assembly 内的 DirectCall |
| RUNTIME_SERVICE_CALLS | INFO | RuntimeService 调用（stsfld 等） |
| BOXING_OVERHEAD | WARN | box/unbox 操作 |
| NULL_GUARD_OVERHEAD | INFO | 空值检测模式 |
| IR_EXPANSION | WARN | IR 指令数超过 IL 的 2 倍 |
| BRANCH_HEAVY | INFO | 分支数 > 10 |

## 输出示例

```
================================================================================
ASM-COMPARE  |  ConvertCharSubjects.Subject_0
================================================================================

[HEADER]
  binary        = ConvertCharSubjects.dll
  method        = ConvertCharSubjects.Subject_0
  jit_status    = ok
  aot_status    = ok
  jit_source    = CoreCLR JIT x64
  aot_source    = Chaos IL2CPP AOT (NativeAotEmitter)
  captured_at   = 2026-05-14T15:30:00Z

  jit.address   = 0x7ff87b722598
  jit.size      = 256 bytes
  jit.instructions = 44

  aot.symbol    = ConvertCharSubjects_Subject_0
  aot.instructions = 13
  aot.locals    = 1
  aot.params    = 0
  aot.return_type = System.Char
  aot.is_static = True

[IL-DUMP]
  IL_0000: nop
  IL_0001: ldc.i4.1
  IL_0002: call        System.Convert.ToChar(System.Boolean)
  ...

[RAW-JIT]
  0x7ff87b722598:  ff 25 fa 3f 00 00    jmp       [rip+0x3ffa]
  0x7ff87b72259e:  4c 8b 15 fb 3f 00 00 mov       r10, [rip+0x3ffb]
  ...

[RAW-CPP]
  // Generated C++ (65 lines)
  inline char16_t ConvertCharSubjects_Subject_0(RuntimeObject* _this) {
      ...
  }

[SIDE-BY-SIDE]  (semantic-block aligned comparison)
  IL_0000 [entry]
    IL:      nop
    AOT IR:  nop
    AOT IR:  ldc.i4              1

  IL_0001
    IL:      ldc.i4.1
    AOT IR:  nop

  IL_0002 [call]
    IL:      call System.Convert.ToChar(System.Boolean)
    AOT IR:  call               System.Convert.ToChar(System.Boolean)

  IL_0007 [return]
    IL:      ret
    AOT IR:  ret
    JIT asm: xor       eax, eax                       ; 33 c0
    JIT asm: ret                                       ; c3

[IR-TRACE]  (IL → AotCoreIR instruction trace)
  Total AotCoreIR instructions: 13
  Unique IL offsets referenced: 6

  IL_0000:    nop
    nop

  IL_0001:    ldc.i4.1
    ldc.i4               val=1

  IL_0002:    call System.Convert.ToChar(System.Boolean)
    call                      :System.Char call=System.Convert.ToChar(System.Boolean) dispatch=ExternalRuntime nparams=1 ret=System.Char

  IL_0007:    ret
    ret

[METRICS]
  Metric                                JIT             AOT (IR)       Ratio
  ------                                ---             ---------      -----

  Instruction count (IR)                44              13             0.30x
  Code size (bytes)                     256             104            0.41x
  Call instructions                     22              2              0.09x
  Branch/jump/ret instructions          22              2              0.09x
  Memory access instructions            4               1              0.25x
  Estimated basic blocks                1               2              2.00x

  AOT IR covers 6/9 IL offsets (66.7%)

  AOT IR opcode distribution:
    nop                           7
    ldc.i4                        1
    stsfld                        1
    call                          1
    stloc                         1
    ldloc                         1
    ret                           1

[INLINE-MAP]  (AOT IR inline/call decisions)
  IL_0002 | call       |
         | callee      = System.Convert.ToChar(System.Boolean)
         | dispatch    = ExternalRuntime
         | nparams     = 1
         | return_type = System.Char

[ANALYSIS]  (optimization opportunity detection)

  #1   [i]   [EXTERNAL_RUNTIME_CALL]
       Call to 'System.Convert.ToChar(System.Boolean)' uses ExternalRuntime dispatch (IL_0002)
       -> Consider implementing as Direct dispatch for better AOT performance

[RAW-AOT]  (native AOT compilation & disassembly)

  MSVC version  = 14.40
  Object file   = C:\Users\...\obj\generated.obj
  Object size   = 12.5 KB

  -- dumpbin /DISASM --
   0000000000000000: 48 89 5C 24 08  mov       qword ptr [rsp+8],rbx
   0000000000000005: 48 89 74 24 10  mov       qword ptr [rsp+10h],rsi
   000000000000000A: 57              push      rdi
   000000000000000B: 48 83 EC 30     sub       rsp,30h
   ...

  -- In-house decoder (unified format) --
  .text sections: .text$mn
  .text size: 244 bytes
  Instructions: 16

  0x10000000       48 89 5c 24 08        mov       qword ptr [rsp+8], rbx
  0x10000005       48 89 74 24 10        mov       qword ptr [rsp+10h], rsi
  0x1000000a       57                     push      rdi
  0x1000000b       48 83 ec 30           sub       rsp, 0x30
  ...

[SIDE-BY-SIDE]  (native-vs-native aligned comparison)

  Block_0
    J     AOT
    -     ----
    push      rdi                         ; 57                     push      rdi                         ; 57
    sub       rsp, 0x30                   ; 48 83 ec 30            sub       rsp, 0x30                   ; 48 83 ec 30
    mov       qword ptr [rsp+8], rbx      ; 48 89 5c 24 08         mov       qword ptr [rsp+8], rbx      ; 48 89 5c 24 08

  Block_1
    mov       qword ptr [rsp+10h], rsi    ; 48 89 74 24 10         mov       qword ptr [rsp+10h], rsi    ; 48 89 74 24 10
    mov       ecx, 1                      ; b9 01 00 00 00         mov       ecx, 1                      ; b9 01 00 00 00
    call      0x7ff812345678              ; e8 xx xx xx xx         call      0x7ff812345678              ; e8 xx xx xx xx
```

## 验证

### 基础测试

```bash
# 帮助
chaos-il2cpp asm-compare --help

# 全 section 输出
chaos-il2cpp asm-compare <dll> --method <method>

# 输出到文件
chaos-il2cpp asm-compare <dll> --method <method> -o report.txt

# 指定 sections
chaos-il2cpp asm-compare <dll> --method <method> --sections header,metrics,analysis

# JSON 格式输出
chaos-il2cpp asm-compare <dll> --method <method> --format json

# 批量模式（多个方法，一次 pipeline 运行）
chaos-il2cpp asm-compare <dll> \
  --methods Subject_0,Subject_1,Subject_2 \
  --method-subject-ids "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean),System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)" \
  --format json \
  --sections metrics,analysis
```

### Foundation-DLL 验证

```bash
# 单方法
chaos-il2cpp asm-compare \
  testing/foundation-dll/System.Private.CoreLib/convert-char/managed/subjects/build-output/ConvertCharSubjects.dll \
  --method ConvertCharSubjects::Subject_0

# 批量模式（用于 foundation-dll 管线集成）
chaos-il2cpp asm-compare \
  testing/foundation-dll/System.Private.CoreLib/convert-char/managed/subjects/build-output/ConvertCharSubjects.dll \
  --methods Subject_0,Subject_1,Subject_2 \
  --method-subject-ids "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Boolean),System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32),System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int64)" \
  --format json \
  --sections metrics,analysis
  
# 通过 Python 验证管线自动调用（batch + cache）
# 新管线: testing/foundation-dll/verification/orchestration/engine.py
# 运行方式: python -m verification.orchestration.main --family convert-char --skip hotupdate ...
```

### 复杂案例验证

可使用 `ConvertCharSubjects::Run`（包含 18+ 个调用、循环控制流）：

```bash
chaos-il2cpp asm-compare \
  testing/foundation-dll/System.Private.CoreLib/convert-char/managed/subjects/build-output/ConvertCharSubjects.dll \
  --method ConvertCharSubjects::Run \
  --sections header,metrics,analysis
```

## 分阶段实现状态

| Phase | 功能 | 状态 |
|-------|------|------|
| 1 | CLI 入口、JIT 捕获、IL 读取、C++ 生成、基础报告 | ✅ 完成 |
| 2 | 语义块对齐 (SemanticAligner)、IR-TRACE、METRICS、INLINE-MAP、ANALYSIS、SIDE-BY-SIDE 升级 | ✅ 完成 |
| 3 | MSVC 编译管线（FindMsvc → cl.exe 编译 → COFF .obj 解析）、RAW-AOT 反汇编（dumpbin + 内建解码器）、SIDE-BY-SIDE native-vs-native 分块对齐、AsmSemanticAligner.AlignNativeVsNative() | ✅ 完成 |

## 已知限制

- **JIT 反汇编器**为内建实现（opcode-length table + group opcode dispatch），不依赖 Zydis，x86-64 指令覆盖率约 95%
- **JIT-IL offset 映射**为近似分配（按比例分配 JIT 指令到各 IL 指令），非精确映射
- **C++ 函数提取**通过 NativeSymbol 进行源码级匹配，在复杂模板展开中可能不准确
- **RAW-AOT** 依赖本地 MSVC 工具链（VS2022 Professional/Community/Enterprise），需要在安装了 C++ 工作负载的环境中运行
- **RAW-AOT 反汇编** 提供 dumpbin /DISASM 和 内建解码器两种输出；dumpbin 输出更权威，但内建解码器格式与 RAW-JIT 统一
- **JIT 边界检测**通过 ret 指令 + padding 检测，在非常规控制流中可能不准确
- **TIERED COMPILATION**: `RuntimeHelpers.PrepareMethod` 确保 Tier1，但不保证最佳优化
- **Native-vs-native 对齐** 为基本块位置对齐（第 N 个 JIT 块 ↔ 第 N 个 AOT 块），非 IL offset 锚定，在基本块数量不一致时可能错位
- **Subject_N 命名约束**：AOT 方法查找使用 Subject_N 名称（包装器名），JIT 捕获使用完整 methodSubjectId（framework 方法名）。两者名称不一致，必须通过 methodMap 双索引对齐
- **p1_lowering 检查**：代码生成 slot 变量可能使用 `_s0{};`（旧 codegen）或 `_s0;`（SEH 改写后），p1_lowering 检查同时接受两种模式