---
name: dev-foundation-dll-family-verification
description: Execute per-family three-gate verification (Fact -> Benchmark -> HotUpdate) as part of the foundation DLL verification pipeline. TRIGGER when asked to run managed dotnet test, native benchmark, or hotupdate verification for a specific capability family.
---

# Foundation DLL Family Three-Gate Verification

## 概述

对选定的 capability family 执行三个验证门。**本技能是 `dev-foundation-dll-verification-pipeline` 的子步骤**，在管线中由 Step 2 调用。

**Fact 验证有两个层次**：

| 层次 | 验证内容 | 当前状态 |
|------|---------|---------|
| **L1: Codegen Success** | entrypoint DLL 经 `chaos-il2cpp convert-to-cpp` 翻译为真实 C++ lowering | ✅ **正式接入** — 使用真实 IL2CPP 翻译通道（PipelinePlan → NativeAotEmitter），通过率 98.3%（119/121 families） |
| **L2: Semantic Correctness** | 生成的 C++ 编译为 native exe，执行结果与 managed 预期 checksum 一致 | ✅ **正式接入** — 通过 `fact_l2_verifier.py` 的 dual-verify（return value checksum + assert intrinsic 失败计数），已验证 convert-char 18/18 |
| **L3: Runtime Execution** | 编译后的 native exe 加载运行并校验 checksum | ⚠️ **已知问题** — 需完成 runtime bootstrap + 跨程序集方法表初始化 + 类型系统初始化 |

> L1 验证的是 IL→C++ 翻译管线的正确性（不崩溃、生成了真实 lowering）。L2/L3 验证的是生成的 C++ 能否编译运行。

### 已知问题（L3 Follow-up）

| 问题 | 影响范围 | 根因 | 修复路径 |
|------|---------|------|---------|
| 缺失 `InterpreterEntryDirect` 符号 | L3 编译失败 | `NameIndexModuleV0.dispatch_table` 设置 `interrupt_ptr = &InterpreterEntryDirect`，该符号在 `interpreter_entry.h` 中定义但未包含 | Phase 3.3 运行时入口 — 确保解释器头文件被包含 |
| 跨程序集类型引用 | L3 链接失败 | 程序集 A 引用程序集 B 的 TypeInfo 时使用 `extern const TypeInfo`，但链接时找不到 B 的符号 | Phase 3.2 + 4 — 每程序集独立编译 + 链接阶段解析 |

| 门 | 验证内容 | 工具 | 状态 |
|----|---------|------|------|
| **Fact L1** | entrypoint DLL → 真实 C++ lowering（`convert-to-cpp`） | `batch_native_aot_runner.py` | ✅ 正式接入 |
| **Fact L2** | native exe 自校验：return value checksum + assert intrinsic | `fact_l2_verifier.py`（batch 管线 Step 3 自动调用） | ✅ 正式接入（convert-char 18/18 通过） |
| **L3: Runtime** | native exe 加载运行并校验完整运行时 | native_verify_main.cpp | ⚠️ 依赖 L2 + 运行时初始化 |
| **Benchmark** | Native AOT 性能不退化 | native benchmark exe | ⬜ 待激活 |
| **HotUpdate** | Native hotupdate patch/revert | native hotupdate exe | ⬜ 待激活 |

## 管线上下文

如果在 `dev-foundation-dll-verification-pipeline` 中运行，数据完整性校验（Step 0）已在上游完成，本技能不重复执行。如果独立运行，先自行调用 `dev-foundation-dll-verify-data-integrity`。

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

### Step 1: Fact -- Native AOT 语义正确性验证

> Fact 验证分两级。**L2（语义正确性）是最终通过标准**。

#### L1: Codegen Success（管线已自动完成）

batch pipeline 成功后即确认 L1 通过——生成的 C++ 代码有 native AOT lowering（`chaos_eval_stack` IL 降级），不是桩/SimpleForward。

验证方式：
```bash
python -c "
import re
cpp = open('verification/foundation-dll/System.Private.CoreLib/<family>/il2cpp_dist/genuine/generated/native-aot.generated.cpp').read()
# 检查是否有真实 AOT lowering（而非 SimpleForward 转发）
has_lowering = 'chaos_eval_stack' in cpp
has_simple_forward = 'SimpleForward' in cpp
print(f'L1: AOT lowering={\"YES\" if has_lowering else \"NO\"} SimpleForward={\"YES\" if has_simple_forward else \"NO\"}')
"
```

#### L2: Semantic Correctness — ✅ 正式接入

> L2 验证器 `fact_l2_verifier.py` 使用 dual-verify 策略：
> 1. **Return value checksum** — native exe 的每个 entry 返回值与 managed L2Harness 预期值匹配
> 2. **Assert intrinsic** — CodeGen 将 `Assert.Equal(T, T)` 内联为 C++ 比较 + `__chaos_assert_failures` 计数器
>
> 已通过 convert-char 18/18 验证。在当前 batch 管线中作为 Step 3 自动运行。

**流程**：

```text
batch_native_aot_runner.py Step 3 自动执行：
  1. L2Harness (C# reflection) → expected_checksums.h
  2. Patch entry .cs: Assert.Equal(<checksum>, __result) 代替自比较
  3. Rebuild DLL + re-run convert-to-cpp（Assert.Equal 值内联到 C++）
  4. Compile native_verify_main.cpp + native-aot.generated.cpp → verify_<family>.exe
  5. Run exe → 比较 return value + assert 失败计数
  6. L2: passed/total
```

验证方式：

```bash
# 单 family（手动）
python build/toolchains/run/testing/foundation_dll/fact_l2_verifier.py <family>
python build/toolchains/run/testing/foundation_dll/fact_l2_verifier.py <family> --verbose

# 通过 batch 管线自动运行（Step 3）
python build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py --families <family>
```

**验证标准**：
1. L1: 生成代码包含 `chaos_eval_stack` lowering 模式（非 SimpleForward）
2. L2: `verify_<family>.exe` 退出码 = 0（所有 checksum 匹配，assert 计数 = 0）
3. L2 输出格式：`L2: N/M passed (assert_failures=0, return_failures=0)`

如果 L2 失败（exit > 0），输出包含具体哪个 entry 的 expected vs actual 不匹配。

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

每个 family 完成验证后，记录到所在目录供 `dev-foundation-dll-verify-aggregate` 读取：

```
verification/foundation-dll/System.Private.CoreLib/<family>/
  benchmark-comparison-report.json   (from Step 2c)
  hotupdate-verification-report.json (from Step 3)
```

## 输出接口

下游 `dev-foundation-dll-verify-aggregate` 会读取以下文件：

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
- 多次失败后不升级到 `dev-systematic-debugging`

## 关联

- 父流程: `dev-foundation-dll-verification-pipeline`
- 上游数据校验: `dev-foundation-dll-verify-data-integrity`
- 下游聚合: `dev-foundation-dll-verify-aggregate`
- 闸门失败调试: `dev-systematic-debugging` + `dev-trace-enforcement`
