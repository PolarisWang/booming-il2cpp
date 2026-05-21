# Design v1-04

## Batch 4 后半段最小实现设计

### 目标

为 Batch 4 的 interop 缺口建立一个可验证的 `MarshalingProof` slice，使主线第一次具备以下最小能力：

- source 明确表达 UTF-8 string marshal surface
- loader 能识别 `[UnmanagedCallersOnly]`
- semantic 能输出 interop 相关 capability
- linker 能冻结 UTF-8 marshal / export metadata 的 shape
- codegen 能为 `MarshalingProof` 选择独立 lowering family

### 设计范围

#### Contracts / Loader

- `ManagedMethodModel` 补充：
  - `IsUnmanagedCallersOnly`
- loader 从 method custom attributes 识别 `System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute`
- 本轮不解析 attribute 的 `EntryPoint` 字符串值，只冻结“该方法是 unmanaged export”

#### SemanticWorld

- 当方法体命中以下任一 surface 时，补 `requires-utf8-string-marshaling`
  - `Marshal.StringToCoTaskMemUTF8`
  - `Marshal.PtrToStringUTF8`
  - `Marshal.FreeCoTaskMem`
- 当方法自身 `IsUnmanagedCallersOnly == true` 时，补 `requires-unmanaged-callers-only-export`

#### Linker

- 新增 marshal/export shape helper
- 用 marker 固化：
  - `utf8-string-marshal`
  - `unmanaged-callers-only-export`
- 本轮不改 artifact schema

#### CodeGen

- 新增 family：
  - `interop.marshaling-utf8-export.minimal`
- 新增模板：
  - `NativeReferenceProof.MarshalingUtf8Export.cpp.scriban`
- lowering selection 条件：
  - assembly 名称为 `MarshalingProof`
  - world capability 命中 `requires-utf8-string-marshaling`
  - world capability 命中 `requires-unmanaged-callers-only-export`
- 计划字段继续复用 `ExpectedOutput`

### 验证策略

先写 RED 测试冻结：

- `MarshalingProof` subject 树
- source 中的 marshal / export 标记
- loader / semantic / linker / codegen 表面
- 模板存在

然后补实现并运行：

- `python -m pytest tests/unit/run/test_phase2_marshaling_proof.py -v`
- `python -m pytest tests/unit/run -v`
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug`
- `dotnet build subjects/MarshalingProof/source/MarshalingProof.csproj -c Debug`

### 风险

- `[UnmanagedCallersOnly]` 这里只冻结“export metadata 存在”，并不代表 native callback 执行已打通
- UTF-8 marshal 这里只覆盖最小 `Marshal.*UTF8` surface，不代表已有通用 marshaler
