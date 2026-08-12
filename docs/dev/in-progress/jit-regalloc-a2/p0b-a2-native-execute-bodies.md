# P0-B A2-1/A2-2 — Native 便宜类 execute body 落地记录（native 侧）

> **日期**：2026-08-12
> **范围**：`src/native/` 仅（track B / managed 侧不在此改）
> **关联**：`docs/dev/in-progress/jit-regalloc-a2/a1-external-fallback-gap-report.md`（A1 定盘点法，111 缺口）
> **目标**：为 A2 便宜类方法补"真 native 执行体"，消除 `ChaosExternalRuntimeFallback` 的 return-0 隐身 pass

---

## 1. 架构关键结论（决定交付形式的边界）

侦查确认了两条硬事实：

1. **`ChaosExternalRuntimeFallback(const char* subject_id)` 只接收 subject 字符串，不接收运行时实参。**
   因此纯 native 侧**无法**用实参执行指针算术/封送语义 —— SIMD 路径靠 return-0/1 兜底可行，但 `Unsafe::As(ptr)`
   这类需要真实指针值的 intrinsic 无法在 fallback 内执行。

2. **真 native execute body 的唯一到达路径 = codegen/ShapeRegistry 注册**（`DirectNativeSymbol`→ `ChaosGCHandleAlloc` 等，
   或 `kChaosExternalRuntimeFnTable` 填表）。这套注册在 **managed 侧**（track B）：
   - `src/managed/.../RuntimeHelperShapeRegistry.CoreStubs.Part1.S9.cs`（GCHandle/SafeBuffer/MarshalHR/NativeMemory 的 `DirectNativeSymbol`）
   - `src/managed/.../ConvertToCppHandler.cs::FillExternalRuntimeStubs()`（`kChaosExternalRuntimeFnTable` 通用 return-0 兜底）

**据此，native 侧交付 = execute body + 签名 + 单测**（track B 注册的前提），注册接线按任务边界延后（见 §3 remaining）。

## 2. 既有且已正确的 native body（本批不重复实现）

| 域 | 符号 | 状态 |
|---|---|---|
| RuntimeHelpers.GetObjectValue | `ChaosRuntimeHelpersGetObjectValue`（`object_stubs.cpp`，identity） | ✅ 正确 |
| GCHandle.Alloc/Free/FromIntPtr | `ChaosGCHandleAlloc/Free/GetTarget`（`interop_stubs.cpp`） | ✅ 已有 |
| SafeBuffer.get_ByteLength/Read/Write | `ChaosSafeBufferGetByteLength/ReadByte/WriteByte`（`interop_stubs.cpp`） | ✅ 已有 |

## 3. 本批新增 native execute body — Unsafe 指针 intrinsics

> 纯机器指针 reinterpret + 字节步进算术，无托管类型元数据、无装箱、无分配，便宜且 ABI 平坦（`CHAOS_IL2CPP_INTPTR`）。

新文件：`src/native/runtime-core/runtime_stubs/unsafe_stubs.{h,cpp}`

| 签名 | 语义 |
|---|---|
| `ChaosUnsafeAsRef(IntPtr)` | `Unsafe.As<TFrom,TTo>(ref)`/`AsRef<T>(in T)` — 地址恒等 |
| `ChaosUnsafeAsFromPointer(IntPtr)` | `AsRef<T>(void*)` — 地址恒等 |
| `ChaosUnsafeAddByteOffset(IntPtr, IntPtr)` | `AddByteOffset` — 无符号地址 + byteOffset |
| `ChaosUnsafeSubtractByteOffset(IntPtr, IntPtr)` | `SubtractByteOffset` — 无符号地址 − byteOffset |
| `ChaosUnsafeAddElementOffset(IntPtr, IntPtr, Int32)` | `Add<T>(ref,int)` — 显式 elementSize × index |
| `ChaosUnsafeSubtractElementOffset(IntPtr, IntPtr, Int32)` | `Subtract<T>(ref,int)` — 显式 elementSize × index |
| `ChaosUnsafeGetObjectValue(IntPtr)` | `GetObjectValue(ref object)` — 引用恒等 |

**步进设计说明**：`Add<T>` 的泛型 stride 需托管类型大小，native 侧无此信息。故签名显式接收
`elementSize`（`IntPtr`），由 track B 的 `GenericShapeDescriptor` 发射编译期常量 stride —— 精确、无对齐假设。
字节偏移版本在无符号地址上运算，回绕良定义。

**内存域**：全路径零堆分配，天然满足统一内存约束（无 `CHAOS_IL2CPP_MALLOC` 触发）。

## 4. 验证

- **信号**：`tests/unit/runtime-native/runtime-core/runtime_stubs/unsafe_stubs_test.cpp`（gtest，14 用例）
  + `runtime_stubs/CMakeLists.txt` 注册 `unsafe_stubs_test`（直接编译 `unsafe_stubs.cpp`）。
- **未执行信号**：本环境无 C++ 编译器 + 无 CMake 配置，native CTest 未跑（见 §5 环境限制）。
- **managed unit 层**：`test_driver --layer unit` 在被隔离 worktree 无预编译 DLL，`--quick` 报 infra-No-trx 环境错；
  native 改动不影响 managed dotnet 测试目标。

## 5. 环境限制与未决

- 无 `cl`/`clang++`/`g++`、无 CMake cache → 本批 native 编译/单测**未在本地执行**，需 CI/有工具链环境验证。
- `unsafe_stubs.cpp` 依赖仅 `<chaos/native_types.h>`（include 根已在 `src/native/runtime-core/CMakeLists.txt:16,284` 确认）。
- `object_stubs_test.cpp`/`misc_stubs_test.cpp` 为孤儿（未注册进 CMake，pre-existing）。

## 6. remaining（延后 + 原因）

| 项 | 原因 |
|---|---|
| **dispatch 接线**（ShapeRegistry `DirectNativeSymbol` / `kChaosExternalRuntimeFnTable` 填表） | 属 managed 侧 track B，边界外 |
| **NameTable / XmlNameTable Add/Get** | 需托管 String 字面量驻留 + GC 根字典（跨 GC 保活），非"便宜类"；硬 return-0 违反本批原则 → A3 重型语义 |
| **Unsafe.Unbox/Box** | 需托管类型元数据/装箱布局，A3 |
| **GCHandle.FromIntPtr 语义修正**（S9 现映射到 GetTarget，非 wrap） | 属 managed 侧 S9.cs，track B |

---

## 架构映射

- **域**：IBC-native（runtime-core / runtime_stubs）+ external-runtime dispatch（边界外 track B）
- **红线**：未改 managed `.cs`；未跨层写 `.generated.*`；新 native 符号符合 native v1（`Chaos*` 前缀、`extern "C"`、PascalCase、无裸 new/malloc）
