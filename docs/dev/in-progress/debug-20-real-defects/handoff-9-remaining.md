# 9 个 realDefect 剩余修复 — 独立子任务交接

> **task_id**: fix-9-remaining-real-defects
> **创建日期**: 2026-09-15
> **前置条件**: `debug-20-real-defects`（SEH 诊断已就绪，能力清单 + 结构化匹配已生效）

## 背景

`System.Private.CoreLib/system` chunk 的能力清单判定 **9 个独立方法**为 `realDefect` —— 有 codegen shape（AOT 有实现），但运行时触发访问违规（`0xc0000005`）或托管异常（`0xe0000001`）。

**Int32/Int64/UInt32/UInt64 Parse 多参重载** 已修复并验证（commit `697b8550b`），但 pipeline 尚未运行最新产物验证。

## 9 个真实缺陷清单

### 分类

| 故障码 | 含义 | 数量 |
|--------|------|------|
| `0xc0000005` | STATUS_ACCESS_VIOLATION — 空指针/越界 | 4 |
| `0xe0000001` | C++ EH 异常 — 托管异常 | 2 |
| 待查 | 不在 SEH 日志中 | 3 |

### 按 SEH 故障码分类

#### Cluster A: 0xe0000001（托管异常，2 个）

| si | 方法 | 现象 |
|----|------|------|
| 151 | `Array::CreateInstance(Type, Int32)` | 托管异常 |
| 1371 | `Enum::Format(Type, Object, String)` | 托管异常 |

推测：这两个方法的 AOT body 内部抛了托管异常（非访问违规），`__except` 捕获后标记 `caught=true`。可能是参数验证失败或调用了另一个未实现的内部方法。

#### Cluster B: 0xc0000005（访问违规，2 个）

| si | 方法 | 现象 |
|----|------|------|
| 1871 | `Math::Cos(Double)` | `ChaosMathCos` 调用时 AV |
| 1877 | `Math::Pow(Double, Double)` | `ChaosMathPow` 调用时 AV |

推测：这两个有相同的模式 —— `BitConverter.DoubleToInt64Bits(result)` 生成的 C++ 代码尝试 reinterpret_cast 一个 `double` 返回值。检查 AOT 生成的页面代码。

#### Cluster C: 待查（3 个，未出现在 SEH 日志）

| si | 方法 | 说明 |
|----|------|------|
| 7 | `Activator::CreateInstance(Type)` | `ChaosReflectionCreateInstance` 有实现，但调用 typeof(int) 时会崩 |
| 1720 | `UInt64::Parse(String, NumberStyles)` | 同 Int32 Parse 多参问题，需验证最新 entry.exe |
| 2899 | `Type::GetField(String)` | `ChaosTypeGetFieldBindingFlags` 有实现，调用 "SampleField" 时崩 |

### 依赖关系

```
Cluster B (Math Cos/Pow)  ────────┐
                                   ├── 互不依赖，可并行
Cluster C (Activator/UInt64/Type)──┘
                                   ├── 不依赖 A/B
Cluster A (Array/Enum)  ──────────┘
```

所有 9 个相互独立，没有上下游依赖。

## 标准修复步骤

每个缺陷的修复遵循同一模式：

```bash
# 1. 定位
grep "si=<N>" /path/to/seh.log        # 故障码 + wrapper subjectId
# 2. 找到 BCL 方法名
sed -n '<line>p' CombinedSubjects.cs   # 生成的 C# 断言
# 3. 检查 shape 注册
grep -n "Type::Method" Part3.S23.cs    # 是否有 SimpleForward 注册
# 4. 检查 native 实现
grep -rn "ChaosXxx" src/native/runtime-core/ # native 符号存在吗
# 5. 检查生成的 AOT body
grep -A 30 "Tests_Method" native-aot.page-*.cpp # AOT 生成的 C++ 代码
# 6. 修复
# 7. 验证
```

## 调试工具

| 工具 | 用法 |
|------|------|
| SEH 诊断 | `entry.exe --fact-json 2>seh.log` 直接输出 `[SEH-FAULT] code= si= subjectId` |
| 能力清单 | `artifacts/.../aot-capability-manifest.json`（`subjectIdPrefix` 匹配） |
| `fact_chunk.py` | `classify_fact_record` 新增 `realDefect`/`notSupported`/`factoryGap` 档位 |
| 结构化匹配 | `_has_codegen_shape()` 用 `(Type::Method, params)` 元组精确匹配重载 |

## 前置完成的验证

```bash
# build+fact 全流程验证当前修复
cd /d/agent/chaos-il2cpp/tests/e2e
CHAOS_FOUNDATION_DLL=D:/agent/chaos-il2cpp/tests/e2e/translation \
  python -m verification.chunk_pipeline --chunk system --stages build,fact --smoke
```