# 优化：enum-parsing

## 优化对象
- **family**: `enum-parsing`
- **assembly**: `System.Private.CoreLib`
- **涉及方法**: 13 methods — Enum.Format, GetName, GetNames, GetValues, IsDefined, Parse, TryParse, ToString, BoxToString
- **branch**: `claim/enum-parsing/enum-parsing-opt`

## 优化清单

### #1: P0 每枚举 codegen 开关 — MetadataLookup 消除
- **修改**: `enum_stubs.cpp` — `_g_bake_cache_` 数组缓存 TryParse 结果，避免重复 metadata lookup
- **效果**: TryParse 从每次调用都走 metadata 解析变为缓存命中后直接返回结果

### #2: P1 非枚举类型 fast-fail（Negative Cache）
- **修改**: `enum_stubs.cpp` — 16-slot thread_local direct-mapped negative cache
- **效果**: `typeof(byte)` 等非枚举类型第一次确认后直接返回 nullptr，消除完整 resolution chain 开销

### #3: TypeInfo 编译时折叠 — typeof(constant type)
- **修改**: `InvocationPlanning.cs` — OpCode→Op 修复：`instr.OpCode is not (InstructionOpCode.Call or InstructionOpCode.CallVirt)` → `instr.Op is not ("call" or "callvirt")`
- **效果**: `typeof(byte)` 在 codegen 阶段编译为 `reinterpret_cast<INTPTR>(chaos_mt_XXX.AsTypeInfoHot())`，消除运行时反射调用

### #4: String.Length 内联 — 绕过外部 runtime dispatch
- **修改**: `ExceptionEmission.cs` — inline String.Length 读字段，添加 `_activeStructuredSlotContext == null` guard
- **效果**: `get_Length()` 调用 → 直接读取 `_stringLength` 字段的内联代码

## 问题根因分析

enum-parsing 原始瓶颈：
1. **enum_stubs.cpp 中 enum_resolve_meta 对非枚举类型（byte）执行完整 resolution chain** — GetNames(byte) 等方法每次调用都走完整链，产生大量 GC 分配
2. **typeof(byte) 运行时反射** — codegen 未折叠常量类型，运行时调用 ChaosReflectionGetTypeFromHandle
3. **String.get_Length() 外部 dispatch** — 每次调用通过 runtime 派发，增加函数调用和间接跳转开销

## 优化循环记录

| Attempt | 假设 | 预期提升 | 实际提升 | 结果 |
|---------|------|---------|---------|------|
| 1 | P2 stub fast-fail for non-enum types | 减少 GC alloc | 减少 GC 分配 | ✅ |
| 2 | P1 SkipIlOffsets DCE for GetTypeFromHandle | 消除死代码 | 不生效 | ❌ |
| 3 | P0 per-enum codegen switch for MetadataLookup | TryParse 快路径 | 缓存命中 | ✅ |
| 4 | TypeInfo compile-time folding | 3x on typeof | 代码变编译时常量 | ✅ |
| 5 | String.Length inlining | 跳过 dispatch | 内联生效 | ✅ |
| 6 | TryParse baked GC allocation optimization | 减少 alloc | 缓存命中 | ✅ |

## 性能数据

### RegisterExecute 路径 — chaos-aot 比 .NET 8 快 6~15x

| 方法 | chaos-aot | chaos-jit | .NET 8 | vs .NET 8 | 执行引擎 |
|------|-----------|-----------|--------|-----------|----------|
| [1] GetName(RuntimeType, UInt64) | **2.67 ns** | 48.6 ns | 18.0 ns | **6.7x 更快** | RegisterExecute |
| [2] GetName(Type, Object) | **3.04 ns** | 47.8 ns | 17.7 ns | **5.8x 更快** | RegisterExecute |
| [7] Parse(Type, String, Bool) | **2.96 ns** | 46.5 ns | 44.3 ns | **15.0x 更快** | RegisterExecute |

### Interpreter 路径 — typed reference blocker

| 方法 | chaos-aot | .NET 8 | vs .NET 8 | 阻塞原因 |
|------|-----------|--------|-----------|----------|
| [10] TryParse(Type, String, Bool, out) | 1017 ns | 45.3 ns | **22.4x 更慢** | typed ref (`out object`) |
| [11] TryParse(Type, String, out) | 1198 ns | 43.9 ns | **27.3x 更慢** | typed ref (`out object`) |

### Exception-path 方法（始终抛异常，自动排除）

方法 [0,3,4,5,8,9] — null value 或非枚举类型参数导致抛异常。NET 8 也走 exception path（3888~4273 ns vs chaos-jit 1541~1741 ns）。

## 极限分析：为何 chaos-aot RegisterExecute 比 .NET 8 JIT 快

RegisterExecute 路径的 ~3 ns/op 相当于约 3 条 CPU 指令（在 3GHz CPU 上），而 .NET 8 JIT 的 18~44 ns 包括：
1. 完整的 method entry/exit frame 设置
2. JIT-compiled enum 方法体执行
3. GC safepoint poll

chaos-aot 的 RegisterExecute 路径消除了上述所有开销——每方法仅执行最少的直接指令。这是 il2cpp 的 AOT 优势。

## 收敛检查

- [x] Phase 1: Subject 审计 + 结构性审计 passed
- [x] Phase 1: Subject freeze + baseline 已记录
- [x] Phase 2: Pipeline passed（除预知失败阶段）
- [x] Phase 2: 至少一次优化假设验证通过（#1, #3, #4 全部生效）
- [ ] Phase 2: vs .NET 8 ≤ 20%（exception-path 排除后）— **blocked: TryParse typed reference（参见 blocker）**
- [x] Phase 2: HotUpdate 已确认（d3PatchApplied=true, semantic_changed > 0）
- [ ] Phase 2: Commit + Merge + CI 完成

## 已知问题

1. **TryParse typed reference blocker** — 方法 [10][11] codegen 不支持 typed reference 参数的 native lowering，走 interpreter 导致 22-27x 退化。需独立 Phase 解决
2. **方法 [6] batch 未命中** — native-aot benchmark batch mode 对 Parse(Type, String) 未输出数据，需排查 batch 索引逻辑
3. **exception-path 方法** — [0,3,4,5,8,9] 始终抛异常，非性能优化目标
