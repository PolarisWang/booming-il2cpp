# Brainstorm v1-04

## 背景

Batch 4 的前半段 `NestedExceptionProof` 已完成，剩余缺口集中在 interop：

- string marshaling
- export metadata / reverse P/Invoke 入口

当前基线已经具备：

- loader 能识别 `PinvokeImpl`
- codegen 有 simple `DllImport` family
- `PInvokeLite` source 已包含 `Marshal.StringToCoTaskMemUTF8` / `PtrToStringUTF8` / `[UnmanagedCallersOnly]`

但“真正的 native -> managed callback 执行”仍然需要更大的 ABI 与 runtime 设计，因此不能直接作为下一个 proof 入口。

## 候选方案

### 方案 A：直接做 reverse P/Invoke callback 闭环

- 优点：最贴近 Batch 4 原始目标
- 缺点：范围过大，需要 callback trampoline、delegate ABI 与 native 回跳执行

### 方案 B：先冻结 UTF-8 marshal + export metadata

- 内容：
  - 新建 `MarshalingProof`
  - source 明确包含 `Marshal.StringToCoTaskMemUTF8` / `PtrToStringUTF8`
  - source 明确包含 `[UnmanagedCallersOnly(EntryPoint = ...)]`
  - loader / semantic / linker / codegen 新增最小 interop surface
- 优点：
  - 能把 interop 能力面先接入主线
  - 不需要立即实现真正的 reverse callback 执行
- 缺点：
  - 仍然不是完整 reverse P/Invoke

### 方案 C：只补 `DllImport` 扩展

- 优点：实现最小
- 缺点：与现有 `pinvokeDllImportMinimal` 重叠太多，新增价值低

## 选择

选择方案 B。

## 最小 proof slice

- subject：`MarshalingProof`
- source 形态：
  - `[DllImport("kernel32.dll", ExactSpelling = true)]`
  - `Marshal.StringToCoTaskMemUTF8`
  - `Marshal.PtrToStringUTF8`
  - `[UnmanagedCallersOnly(EntryPoint = ...)]`
  - `delegate* unmanaged<int, int, int>`
- semantic 目标：
  - `requires-utf8-string-marshaling`
  - `requires-unmanaged-callers-only-export`
- linker 目标：
  - 冻结 UTF-8 marshal 与 unmanaged export shape
- codegen 目标：
  - family `interop.marshaling-utf8-export.minimal`
  - 对应独立 template

## 非目标

- 本轮不做真正的 native -> managed callback 执行
- 本轮不做 blittable/non-blittable 的完整通用 marshaler
- 本轮不做 delegate reverse P/Invoke trampoline
