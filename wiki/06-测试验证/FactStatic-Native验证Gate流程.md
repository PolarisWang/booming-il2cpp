# Foundation DLL Family 验证 Gate 流程

## 概述

Foundation DLL 验证管线将 C# entrypoint EXE 通过 il2cpp 翻译为原生可执行文件，直接运行来验证 Assert 全部通过。**不再涉及 C++ host 编译、`expected_checksums.h`、CMake 或静态链接。**

## 架构

```
managed entrypoint EXE  
    │
    ├─ Pass 1 (Probe): int-returning 方法, 在托管 .NET 下运行
    │     捕获每个 API 调用的真实返回值 / 异常类型
    │
    └─ Pass 2 (Emit): void-returning 方法, Assert.Equal(expected, actual)
           │
           ▼
    ChaosProofRunner.RunAll(MethodTable)
           │
           ▼
    每个方法: Reset → 调用 → 读 ExitCode → 累加 failures
           │
           ▼
    Passed: N/M  (exit code 0 = 全部通过)
```

## 两阶段探测流程

生成 entrypoint 时，自动执行两阶段流程：

1. **Phase 1 (Probe):** 生成 `static int MethodN()` 方法（返回 API 调用的 int 转换结果），构建为托管 EXE 并运行。输出格式：
   - `RESULT N:<value>` — 方法正常返回
   - `EXCEPTION N:<ExceptionType>` — 方法抛出异常
2. **Phase 2 (Emit):** 利用捕获的预期值生成 `static void MethodN()`，包含：
   - `Assert.Equal(<expected_value>, <call_expr>)` — 正常方法
   - `Assert.Throws<ExceptionType>(() => <call_expr>)` — 异常方法
   - 自定义条目 (Custom.cs) 使用 `Assert.Throws` 模式

## 入口命令

```bash
# 通过 run 系统（完整 7 阶段管线）
run foundation-dll verify-family --family convert-char
run foundation-dll verify-family --family convert-char --mode strict
run foundation-dll verify-family --family buffer-memory --skip benchmark hotupdate

# 直接运行 pipeline（仅 codegen + fact verify）
python build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py \
  --assembly System.Private.CoreLib --families convert-char

# 仅运行 entry EXE（需要先完成 codegen）
python build/toolchains/run/testing/foundation_dll/fact_verifier.py convert-char
```

## 7 阶段管线

`run foundation-dll verify-family` 自动执行以下阶段：

| 阶段 | 名称 | 说明 |
|------|------|------|
| 0 | Preflight | Contract 完整性检测，自定义条目发现 |
| 1 | Codegen | Entrypoint 生成 + il2cpp 翻译 (delegate to pipeline_native_aot_runner) |
| 2 | Fact | 运行原生 entry EXE，验证 Assert 全部通过 |
| 3 | Audit | Mechanism + Principle 审计 |
| 4 | Benchmark | 性能基线（可选，读取已有报告） |
| 5 | HotUpdate | 热更新验证（可选，读取已有报告） |
| 6 | Post-HU Benchmark | 热更新后性能（可选） |
| 7 | Aggregate | 评分、回归检测、Pass/Fail 判定 |

必过阶段：Preflight、Codegen、Fact、Audit。其余阶段在 standard 模式下为 optional。

## 验证标准

- exit code = 0
- stdout 包含 `Passed: N/M`
- 无 `FAIL` 行

## Assert 模式

所有 Assert 通过 `ChaosAssertState.ExitCode` 报告失败，而非抛出异常：

```csharp
public static void Equal(int expected, int actual)
{
    if (expected != actual) ChaosAssertState.RecordFailure();
}

public static void Throws<TException>(Action action)
{
    try { action(); ChaosAssertState.RecordFailure(); }
    catch (TException) { }
    catch (Exception) { ChaosAssertState.RecordFailure(); }
}
```

## 生成文件结构

```
il2cpp_dist/entrypoint/
  ConvertCharNativeEntry.cs        # 自动生成的主 entry（包含 MethodTable + per-method stubs）
  ConvertCharNativeEntry.Custom.cs # 手工编写的自定义条目（Assert.Throws 模式）
  Program.cs                       # Main() → ChaosProofRunner.RunAll
  ConvertCharNativeEntry.csproj    # Exe OutputType, 引用 TestFramework.Sdk + Runner
  build-output/
    ConvertCharNativeEntry.exe     # 托管 EXE（pipeline 用它构建）

il2cpp_dist/genuine/<AssemblyName>/generated/
  native-aot.generated.cpp         # il2cpp 翻译后的原生 C++
  entry.exe                        # 原生可执行文件（由 fact_verifier 运行）
```

## 与旧流程（已删除）的区别

| 维度 | 旧流程 | 新流程 |
|------|--------|--------|
| 验证方式 | C++ host 编译 + 静态链接 | 直接运行 entry.exe |
| 预期值 | expected_checksums.h + 二阶段 codegen | 托管探测自动捕获 |
| 断言机制 | Assert.Equal(__result, __result) 自比较 | Assert.Equal(expected, actual) 真实比较 |
| 失败信号 | ExitCode vs __chaos_assert_failures 错位 | 统一 ExitCode |
| 所需工具 | MSVC + CMake + 8 个 .lib | 无（entry.exe 已编译完成） |
| 维护成本 | 高（CRT 匹配、lib 依赖） | 低 |

## 自定义条目

对于会抛出预期异常的方法（如 `Convert.ToChar(true)` → `InvalidCastException`），使用 `Custom.cs` 手工编写：

```csharp
public static partial class ConvertCharNativeEntry
{
    static void CustomEntryMethod0()
    {
        Assert.Throws<InvalidCastException>(() => Convert.ToChar(true));
    }
}
```

通过 `capability-family-contract.json` 中的 `customEntryIndices` 字段声明哪些索引走自定义条目。