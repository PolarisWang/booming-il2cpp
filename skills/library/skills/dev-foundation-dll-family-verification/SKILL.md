---
name: dev-foundation-dll-family-verification
description: Execute per-family three-gate verification (Fact -> Benchmark -> HotUpdate) as part of the foundation DLL verification pipeline. TRIGGER when asked to run managed dotnet test, native benchmark, or hotupdate verification for a specific capability family.
---

# Foundation DLL Family Three-Gate Verification

## 概述

对选定的 capability family 执行三个验证门。**本技能是 `dev:foundation-dll-verification-pipeline` 的子步骤**，在管线中由 Step 2 调用。

| 门 | 验证内容 | 工具 |
|----|---------|------|
| **Fact** | Managed xUnit `[Fact]` 测试全部通过 | `dotnet test` |
| **Benchmark** | Native AOT 性能不退化、计算结果正确 | native benchmark exe + managed baseline |
| **HotUpdate** | Native hotupdate patch/revert 机制正确 | native hotupdate exe |

## 管线上下文

如果在 `dev:foundation-dll-verification-pipeline` 中运行，数据完整性校验（Step 0）已在上游完成，本技能不重复执行。如果独立运行，先自行调用 `dev:foundation-dll-verify-data-integrity`。

## 前置条件

- 目标 family 已通过 `batch_native_aot_runner.py` 完整 pipeline（codegen + native compile）
- Solution 项目已生成在 `solution/System.Private.CoreLib/<family-slug>/`
- Native benchmark & hotupdate 可执行文件已构建在 `artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/`
- SDK 项目 `src/reference/Chaos.TestFramework.Sdk/` 已 `dotnet build --configuration Release`
- **（管线模式下已由上游确保）** claims 有 methodSubjectIds、coverage JSON 存在

## 选择 Family 的原则

根据不同考察目的选择测试覆盖范围：

| 目的 | 推荐 family | 理由 |
|------|-------------|------|
| 基准回归 | convert-char | 简单转发，结果稳定 |
| StringId 路径 | string-char-text-core | 字符串操作密集 |
| 类型系统/stable_id | boxing-unboxing-casts, object-equality-identity | isinst/castclass/equality |
| 数值运算 | primitive-numeric-conversions | 基础 IL 翻译 |
| 反射复杂路径 | reflection-type | 43 方法，涉及外部依赖 |
| 完整覆盖 | 全部 33 families | 综合验证 |

至少选择 **3-6 个**有代表性的 family 以保证覆盖不同维度。

## 执行步骤

### Step 1: Fact -- Managed xUnit 测试

> 数据完整性已在管线 Step 0 中验证（独立运行请先执行 `dev:foundation-dll-verify-data-integrity`）

```bash
# 单 family
dotnet test solution/System.Private.CoreLib/<family-slug>/tests/<ProjectBase>.Tests.csproj --configuration Release

# 多 family 并行（注意 SDK 锁竞争）
dotnet test solution/System.Private.CoreLib/convert-char/tests/ConvertChar.Tests.csproj --configuration Release &
dotnet test solution/System.Private.CoreLib/reflection-type/tests/ReflectionType.Tests.csproj --configuration Release &
wait
```

如果因 `Chaos.TestFramework.Sdk.dll` 文件锁导致 CS2012 错误，先单独 build SDK：

```bash
dotnet build src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj --configuration Release
```

然后重跑失败的项目。

验证标准：`Failed: 0`，所有 `[Fact]` 断言通过。

### Step 2: Benchmark -- Managed vs Native 性能对比

#### 2a. 运行 Managed 基线

```bash
dotnet run --project verification/foundation-dll/System.Private.CoreLib/<family-slug>/managed_test/benchmarks/ManagedBenchmarkHarness.csproj --configuration Release
```

输出 JSON 到 stdout，保存到 `benchmark/managed-baseline.json`。

#### 2b. 运行 Native Benchmark

```bash
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/chaos_benchmark_<slug>.exe \
  --entry-index <N> --iterations 100000 --subject-id "<methodSubjectId>"
```

对所有 methodSubjectId 逐个执行，输出保存为 `native/native-benchmark.json`。

#### 2c. 生成对比报告

```bash
python build/toolchains/run/testing/foundation_dll/benchmark_comparator.py \
  --managed <family-dir>/benchmark/managed-baseline.json \
  --native <family-dir>/native/native-benchmark.json \
  --output <family-dir>/benchmark-comparison-report.json
```

对比逻辑：
```python
speedup = ((managed_ms - native_ms) / managed_ms) * 100
native_faster = speedup > 1.0
managed_faster = speedup < -1.0
equal = -1.0 <= speedup <= 1.0
```

### Step 3: HotUpdate -- Native 热更验证

```bash
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/chaos_hotupdate_<slug>.exe 2>/dev/null
```

输出 JSON 到 stdout。验证字段：
- `passedMethods` = `totalMethods`
- 每个 method: `status == "passed"`, `revertVerified == true`
- `patchedReturnValue == expectedPatchedValue`

### Step 4: 记录结果

每个 family 完成验证后，记录到所在目录供 `dev:foundation-dll-verify-aggregate` 读取：

```
verification/foundation-dll/System.Private.CoreLib/<family>/
  benchmark-comparison-report.json   (from Step 2c)
  hotupdate-verification-report.json (from Step 3)
```

## 输出接口

下游 `dev:foundation-dll-verify-aggregate` 会读取以下文件：

| 文件 | 来源 | 用途 |
|------|------|------|
| `benchmark-comparison-report.json` | Step 2c | 性能分析 |
| `hotupdate-verification-report.json` | Step 3 | 热更新验证 |
| `managed_test/tests/` | Step 1 | Fact 结果目录 |

## 闸门失败处理

任何闸门（Fact / Benchmark / HotUpdate）失败时，按以下流程处理：

### 标准流程

```bash
# 1. 先查 trace 定位失败阶段
python -m build.toolchains.run.commands.trace_viewer --exception
python -m build.toolchains.run.commands.trace_viewer --stage <gate>  # gate = proof / benchmark / hotupdate

# 2. 分析 trace 输出
#    - phase="exception" 的行会显示 exception 类型和 message
#    - dur_ms 显示各阶段的耗时
#    - f 字段显示源文件:行号

# 3. 使用 dev:systematic-debugging 做根因分析
#    - 先在 trace 中找到失败的 exact 操作
#    - 然后查看对应的源文件行号
#    - 理解数据流后再修复

# 4. 修复后重跑当前 gate，确认通过

# 5. 确认通过后再继续下一 family 或下一 gate

# 6. 如果 3 次以上修复失败 → 调用 dev:systematic-debugging 质疑架构
```

### 各闸门特定检查

| 闸门 | trace 过滤 | 常见失败原因 |
|------|-----------|-------------|
| Fact | `--stage proof` | dotnet test 编译错误、断言失败 |
| Benchmark | `--stage benchmark` | 生成的 C++ 有性能退化 |
| HotUpdate | `--stage hotupdate` | stub 返回 `CHAOS_BRIDGE_STATUS_UNIMPLEMENTED` |

### 禁止

- 跳过 trace 直接猜测原因
- 不查根因就重跑
- 多次失败后不升级到 `dev:systematic-debugging`

## 关联

- 父流程: `dev:foundation-dll-verification-pipeline`
- 上游数据校验: `dev:foundation-dll-verify-data-integrity`
- 下游聚合: `dev:foundation-dll-verify-aggregate`
- 闸门失败调试: `dev:systematic-debugging` + `dev:trace-enforcement`
