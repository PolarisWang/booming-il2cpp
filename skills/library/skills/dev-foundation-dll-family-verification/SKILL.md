---
name: dev-foundation-dll-family-verification
description: Execute three-gate verification (Fact → Benchmark → HotUpdate) across one or more foundation DLL capability families, then produce a consolidated correctness + performance report.
---

# Foundation DLL Family Three-Gate Verification

## 概述

对选定的 capability family 依次执行三个验证门：

| 门 | 验证内容 | 工具 |
|----|---------|------|
| **Fact** | Managed xUnit `[Fact]` 测试全部通过 | `dotnet test` |
| **Benchmark** | Native AOT 性能不退化、计算结果正确 | native benchmark exe + managed baseline |
| **HotUpdate** | Native hotupdate patch/revert 机制正确 | native hotupdate exe |

完成后输出一份包含通过率、加速比和异常分析的汇总报告。

## 前置条件

- 目标 family 已通过 `batch_native_aot_runner.py` 完整 pipeline（codegen + native compile）
- Solution 项目已生成在 `solution/System.Private.CoreLib/<family-slug>/`
- Native benchmark & hotupdate 可执行文件已构建在 `artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/`
- SDK 项目 `src/reference/Chaos.TestFramework.Sdk/` 已 `dotnet build --configuration Release`

## 选择 Family 的原则

根据不同考察目的选择测试覆盖范围：

| 目的 | 推荐 family | 理由 |
|------|-------------|------|
| 基准回归 | convert-char | 简单转发，结果稳定 |
| StringId 路径 | string-char-text-core | 字符串操作密集 |
| 类型系统/stable_id | boxing-unboxing-casts, object-equality-identity | isinst/castclass/equality |
| 数值运算 | primitive-numeric-conversions | 基础 IL 翻译 |
| 反射复杂路径 | reflection-type | 43 方法，涉及外部依赖 |
| 完整覆盖 | 全部 32+ families | 综合验证 |

至少选择 **3-6 个**有代表性的 family 以保证覆盖不同维度。

## 执行步骤

### Step 1: Fact — Managed xUnit 测试

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

### Step 2: Benchmark — Managed vs Native 性能对比

#### 2a. 运行 Managed 基线

```bash
dotnet run --project verification/foundation-dll/System.Private.CoreLib/<family-slug>/managed_test/benchmarks/ManagedBenchmarkHarness.csproj --configuration Release
```

输出 JSON 到 stdout，保存到 `benchmark/managed-baseline.json`。注意该输出**缺少开头 `{`**——需要修复：

```python
with open('managed-baseline.json') as f:
    content = f.read()
fixed = '{' + content
with open('managed-baseline.json', 'w') as f:
    f.write(fixed)
```

#### 2b. 运行 Native Benchmark

```bash
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/chaos_benchmark_<slug>.exe \
  --entry-index <N> --iterations 100000 --subject-id "<methodSubjectId>"
```

对所有 methodSubjectId 逐个执行，输出保存为 `native/native-benchmark.json`。

#### 2c. 生成对比报告

使用 `build/toolchains/run/testing/foundation_dll/benchmark_comparator.py`：

```bash
python build/toolchains/run/testing/foundation_dll/benchmark_comparator.py \
  --managed <family-dir>/benchmark/managed-baseline.json \
  --native <family-dir>/native/native-benchmark.json \
  --output <family-dir>/benchmark-comparison-report.json
```

**注意**：如果 managed 或 native JSON 中的字段名与 comparator 预期不一致（如 managed 用 `name` 而非 `methodSubjectId`，native 用 `subjectId` 而非 `methodSubjectId`），需要手写对比脚本。直接对比逻辑：

```
speedup = ((managed_ms - native_ms) / managed_ms) * 100
native_faster = speedup > 1.0
managed_faster = speedup < -1.0
equal = -1.0 <= speedup <= 1.0
```

Native 使用 100000 iterations，managed 使用 100000 iterations 以获得稳定结果。checksum 验证计算结果一致。

### Step 3: HotUpdate — Native 热更验证

```bash
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/chaos_hotupdate_<slug>.exe 2>/dev/null
```

输出 JSON 到 stdout（`verificationKind: "hotupdate-proof"`）。验证字段：

- `passedMethods` = `totalMethods`（或说明失败的合理性）
- 每个 method: `status == "passed"`, `revertVerified == true`
- `patchedReturnValue == expectedPatchedValue`

如果 Release 版 exe 不存在，可尝试 Debug 版（路径中 `Debug` 替换 `Release`）。

### Step 4: 汇总分析

对每个 family 收集以下数据：

```
| Family | Fact(tested/total) | Benchmark(matched/faster) | HotUpdate(passed/total) | RevertVerified | AvgSpeedup% |
```

需要关注的异常信号：
- Benchmark 出现 `managed_faster > 0`：代表 native 比 managed 慢，可能为回归
- HotUpdate 出现 `failedMethods > 0`：检查 `originalReturnValue` 是否与其他方法不一致
- HotUpdate 出现 `WARN: both host and semantic-patch returned 0 (stub?)`：方法为 skeleton stub，尚未真实翻译

#### reflection-type 已知问题

该 family 的 method index 21 存在 pre-existing stub 返回 `CHAOS_BRIDGE_STATUS_UNIMPLEMENTED`（值 0x01）而非 `CHAOS_BRIDGE_STATUS_OK`（值 0x00），导致 hotupdate 验证标记为 `failed`。Patch 逻辑和 revert 均正确，待真实 IL 翻译后自动修复。

## 正确性评估标准

| 等级 | 条件 | 行动 |
|------|------|------|
| ✅ PASS | 所有三扇门通过，零异常 | 报告通过 |
| ⚠️ WARN | 个别 family 有 pre-existing 失败（如 stub 返回值） | 说明原因，标记为 pre-existing 非回归 |
| ❌ FAIL | 新出现的 test 失败 / benchmark 退化 / hotupdate 不匹配 | 立即停止，分析根因 |

## 输出格式

报告应包含以下章节：

1. **验证范围** — 选中的 family 列表与选取理由
2. **Fact 结果** — 每 family 的通过/总数表格
3. **Benchmark 结果** — 每 family 的 native-faster/managed-faster/平均加速比表格
4. **HotUpdate 结果** — 每 family 的通过/失败/revert-verified 表格，异常方法分析
5. **总体评估** — 按正确性评估标准给出结论
