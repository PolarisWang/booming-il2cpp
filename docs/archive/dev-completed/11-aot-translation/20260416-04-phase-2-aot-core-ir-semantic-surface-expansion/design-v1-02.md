# Phase 2 AotCoreIr Semantic Surface Expansion Design v1.02

Date: 2026-04-16 10:36:50 +08:00
Status: completed

## 1. 目标

以 `AotCoreIr` 为唯一 owner，冻结并补齐当前已经被 loader 解锁、但在 IR / ABI contract 上仍缺失精确表达的真实语义面。

本轮最终收口的 surface：

- `ldtoken` 的强类型 `targetReference`
- `System.Int64` 独立 ABI carrier
- `System.UInt64` 独立 ABI carrier

## 2. 设计决策

### 2.1 `ldtoken` 必须在 `AotCoreIr` 落成强类型 `targetReference`

- type token -> `AotCoreIrReferenceKind.Type`
- field token -> `AotCoreIrReferenceKind.Field`
- method token -> `AotCoreIrReferenceKind.Method`

### 2.2 64-bit integer 不能继续退化成 `NativeInt`

- `System.Int64` -> `AotCoreIrAbiCarrierKind.Int64`
- `System.UInt64` -> `AotCoreIrAbiCarrierKind.UInt64`

### 2.3 NativeAOT ABI 往返必须走 bit-preserving helper

为了避免 64-bit integer 在 `eval stack <-> native ABI` 之间再次被实现细节污染，planner 新增：

- `chaos_store_int64 / chaos_load_int64`
- `chaos_store_uint64 / chaos_load_uint64`

### 2.4 owner evidence 继续固定在 `SolutionCorePack`

- `IntegerCarrierProof / IntegerCarrierBenchmark` 负责纯 integer carrier owner evidence
- `NativeCallInteropBenchmark` 负责真实 `ulong` interop owner surface

## 3. 非目标

- 本批不解决纯 `UInt64` literal / bitwise 的 planner opcode legalization
- 本批不解决更泛化的 framework intrinsic / host-platform gating
- 本批不引入新的 `Chaos.TestFramework` 协议或 string contract

## 4. 验收标准

- `aot-core-ir.json` 中：
  - `ldtoken` 具备正确 `targetReference`
  - `Int64 / UInt64` 方法与 import surface 具备独立 `carrierKindCode`
- `emit-native-aot` 生成的 C++ 中：
  - `Int64 / UInt64` 使用显式 `std::int64_t / std::uint64_t`
  - 出现对应 `chaos_store/load_*64` helper
- 相邻 compatibility 回归保持绿色
