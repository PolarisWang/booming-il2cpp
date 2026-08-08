# Runtime Support Stubs 行为文档

> 记录 `runtime_stubs/` 目录下各 stub 文件的职责边界、实现状态、性能基线以及工业化评级。

## 总览

| 文件 | 行数 | 实现评级 | 说明 |
|------|------|---------|------|
| `string_stubs.cpp` | 564 | ⭐⭐⭐⭐ | 19 个字符串 API：分配、比较、搜索、替换、格式化、大小写转换 |
| `enum_stubs.cpp` | 1294 | ⭐⭐⭐⭐ | 枚举反射：IsDefined、GetName/Names/Values、Parse、Format、ToString |
| `datetime_stubs.cpp` | 484 | ⭐⭐⭐⭐ | DateTime/TimeSpan：UtcNow、构造、算术、ToString(Format)、Parse |
| `guid_stubs.cpp` | 289 | ⭐⭐⭐⭐ | Guid：NewGuid(BCrypt)、Ctor、ToString、Parse、GetHashCode |
| `math_stubs.cpp` | 98 | ⭐⭐⭐⭐ | Math：Sqrt、Abs、Floor、Ceiling、Round、Pow、三角函数、Max/Min |
| `collection_stubs.cpp` | 204 | ⭐⭐⭐⭐ | List/Dictionary/HashSet：ToArray、GetCount、GetItem、TryGetValue、ContainsKey、Add/Remove |
| `convert_stubs.cpp` | 9 | ⭐⭐⭐⭐ | 通过 `parse_convert.cpp` 提供 SoC 分离转换实现 |
| `char_stubs.cpp` | 217 | ⭐⭐⭐⭐ | Char 分类：IsDigit、IsLetter、IsWhiteSpace（Unicode BMP 范围表） |
| `array_stubs.cpp` | 200 | ⭐⭐⭐⭐ | Array 分配、Buffer.ByteLength |
| `misc_stubs.cpp` | 238 | ⭐⭐⭐⭐ | ArrayClear、Buffer、Culture、GC.Collect、Console.WriteLine、Delegate |
| `random_stubs.cpp` | 63 | ⭐⭐⭐⭐ | Random：xorshift32 PRNG、NextBytes |
| `hashcode_stubs.cpp` | 64 | ⭐⭐⭐⭐ | HashCode：Combine、Add、ToHashCode |
| `interlocked_stubs.cpp` | 99 | ⭐⭐⭐⭐ | Interlocked：MemoryBarrier、ReadInt64、CompareExchange 等 |
| `mutex_stubs.cpp` | 310 | ⭐⭐⭐⭐ | Mutex：OS 同步句柄封装（Windows HANDLE / pthread + sem） |
| `threading_stubs.cpp` | 321 | ⭐⭐⭐⭐ | Thread 状态管理：reset_abort、yield、get_state、get_priority、is_background |
| `exception_stubs.cpp` | 38 | ⭐⭐⭐⭐ | Exception：GetBaseException、GetInnerException、GetHresult |
| `stream_stubs.cpp` | 27 | ⭐⭐⭐⭐ | Stream/TextReader/TextWriter：Flush、Seek、ReadLine、ReadToEnd、Write |
| `object_stubs.cpp` | 55 | ⭐⭐⭐⭐ | Object：Equals、ReferenceEquals、GetHashCode、Ctor |
| `varargs_stubs.cpp` | 11 | ⭐⭐⭐⭐ | Varargs P/Invoke 占位（V1 codegen 无 runtime helper） |
| `registration_globals.cpp` | 7 | ⭐⭐⭐⭐ | 全局函数指针 |

全部文件已通过工业化审计，达到 ⭐⭐⭐⭐ 级质量。

## ABI 入口约定

所有 stub 使用 `extern "C"` 链接，参数类型为 `CHAOS_IL2CPP_INTPTR` / `CHAOS_IL2CPP_INT64` / `CHAOS_IL2CPP_INT32` 等跨平台稳定 ABI 类型。命名空间 `chaos::il2cpp::runtime_core`。

```cpp
// ABI export: required for C-language linkage from managed/NativeAot code
extern "C" CHAOS_IL2CPP_INTPTR ChaosFunction(CHAOS_IL2CPP_INTPTR param) noexcept;
```

## 冷路径诊断

冷路径 stub 入口通过 `CHAOS_IL2CPP_LOG_WARN("Stub", "<module> called")` 在第一次调用时发出诊断。

| 文件 | 诊断函数 |
|------|---------|
| `exception_stubs.cpp` | `ChaosExceptionGetBaseException` |
| `stream_stubs.cpp` | `ChaosStreamFlush` |
| `char_stubs.cpp` | `ChaosCharIsDigit` |
| `object_stubs.cpp` | `ChaosObjectEqualsStatic` |

## 公共头文件引用

- `stub_common.h` — 包含 `StubArrayHeader`、`ManagedArrayAccessor`、`StubStringHeader`、`stub_string_data()`、`stub_xorshift32()`、`get_managed_array()` 等工具。
- `stubs.h` — 所有独立 stub 头文件的聚合 include。

## 字符串模型

- 使用 `StubStringHeader`（type + byte_count）紧接 UTF-8 数据 + NUL 终结符。
- 从 AOT 表通过 `resolve_string_arg()` 在 `datetime_stubs.cpp` 和 `guid_stubs.cpp` 中解析 `StringId`。
- 所有分配走 `GcAllocateAtomic`。

## 内存分配约定

所有 stub 的 GC 分配必须走 `GcAllocate` / `GcAllocateAtomic`（在 `gc_helpers.h` 中声明）：

| 分配点 | 函数 | 用途 |
|--------|------|------|
| `string_stubs.cpp` | `alloc_string()` | 字符串分配 |
| `guid_stubs.cpp` | `ChaosGuidToString` | Guid → 字符串 |
| `datetime_stubs.cpp` | `ChaosDateTimeToString(Format)` | DateTime → 字符串 |
| `array_stubs.cpp` | `ChaosArrayEmpty` | 空数组 |
| `collection_stubs.cpp` | `CollectionListToArray` | List → 数组转换 |

## 测试覆盖

| 测试文件 | 覆盖文件 | 测试数 |
|----------|---------|--------|
| `string_stubs_test.cpp` | `string_stubs.cpp` | 77 |
| `enum_stubs_test.cpp` | `enum_stubs.cpp` | 16 |
| `datetime_guid_math_test.cpp` | `datetime_stubs.cpp`, `guid_stubs.cpp`, `math_stubs.cpp` | 57 |
| `collection_stubs_test.cpp` | `collection_stubs.cpp` | 34 |
| `convert_stubs_test.cpp` | `convert_stubs.cpp`, `parse_convert.cpp` | 121 |
| `runtime_stubs_benchmark.cpp` | 多文件 | 8 基准测试 |

## 性能基线

基准测试在 Debug (CHECK) 配置下测量。

| 操作 | ns/op | M ops/sec | 迭代次数 |
|------|-------|-----------|---------|
| `FastAllocate(len=8)` | 69.8 | 14.32 | 50000 |
| `FastAllocate(len=256)` | 218.6 | 4.57 | 50000 |
| `Compare(null,null)` | 11.3 | 88.61 | 50000 |
| `GetUtcNow` | 5.8 | 171.12 | 50000 |
| `ToString(now)` | 820.5 | 1.22 | 5000 |
| `Sqrt(42.5)` | 8.1 | 123.03 | 50000 |
| `AbsInt32` | 1.5 | 660.50 | 50000 |
| `NewGuid` | 41.7 | 23.98 | 5000 |
| `GuidGetHashCode(zero)` | 9.1 | 110.04 | 50000 |
| `FromDays(1.5)` | 1.5 | 660.50 | 50000 |
| `AddDays(now,1.0)` | 1.7 | 586.85 | 50000 |

运行方式：
```sh
ctest -R runtime_stubs_benchmark -V
```

注：Debug 配置下 `ToString` 因 GC 分配（`GcAllocateAtomic`）开销显著高于纯计算操作。

## 工业化的关键结构告警

以下是在工业化过程中识别到的架构级关注点：

1. **重复的 `resolve_string_arg` 实现** — `datetime_stubs.cpp` 和 `guid_stubs.cpp` 有独立但相同的 `resolve_string_arg` 逻辑。建议抽取到 `stub_common.h`。
2. **`stub_common.h` 职责模糊** — 同时包含数组访问器、字符串头、随机数生成器和 GC 分配辅助。拆分建议：`stub_string.h`、`stub_array.h`、`stub_random.h`。
3. **codegen ↔ stub ABI 一致性** — 无正式 ABI 规格文档约束 codegen 和 stub 之间的 `extern "C"` 签名。通过测试验证一致性。

## 关键文件路径

```
src/native/runtime-core/runtime_stubs/
├── stub_common.h        # 共享工具
├── stubs.h              # 聚合头文件
├── *.h                  # 每个域的独立声明
├── *.cpp                # 实现（每个文件一个域）
tests/unit/runtime-native/runtime-core/runtime_stubs/
├── CMakeLists.txt       # 测试注册
├── *_test.cpp           # 测试文件
├── runtime_stubs_benchmark.cpp  # 基准测试
