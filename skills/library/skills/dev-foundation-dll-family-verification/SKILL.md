---
name: dev-foundation-dll-family-verification
description: Execute per-family three-gate verification (Fact -> Benchmark -> HotUpdate) with Unity IL2CPP behavioral parity enforcement, no-skip policy, and mechanism audit.
---

# Foundation DLL Family Three-Gate Verification

## 概述

对选定的 capability family 执行三个验证门。**本技能是 `dev-foundation-dll-verification-pipeline` 的子步骤**，在管线中由 Step 2 调用。

### 核心原则

**项目全局优先级**（源自 CLAUDE.md，本技能所有验证门必须与之对齐）：

```
P1 — 性能最优（第一优先级）：生成的 C++ 代码运行时性能为第一考量
P2 — 方案完美性（第二优先级）：架构完整性、正确性、可维护性
P3 — 热更适配（第三优先级）：最大化支持热更新
```

**裁决规则**：低优先级与高优先级冲突时，低优先级让位于高优先级。但高优先级方案不应故意破坏低优先级。

**本技能的原则对齐要求**：每个验证门的结果必须附带原则对齐评估。测试通过但违反原则 == 不通过。

1. **Unity IL2CPP Behavioral Parity** — native 代码的行为必须与 Unity IL2CPP 生成的代码完全一致。不一致 == 不通过。
2. **No-Skip Policy** — 测试用例除以下情况外不允许 skip：
   - Unity IL2CPP 本身也不支持该行为（需提供证据）
   - 有已批准的修复计划 + ticket 跟踪
   - 在 skill 文档中登记 + 定期审查
3. **完备机制优先** — 每个问题必须用完备的 codegen/runtime 机制解决，不允许通过特例（hardcoded override、方法级豁免、`if(family==X)` 分支）让测试通过。
4. **Handwrite 注入** — AI review 过程中允许注入 handwritten entrypoint / test 用例，但必须与 auto-generated 用例使用同一验证管道，不可绕过 checksum 或 assertion。
5. **Mechanism Audit** — 每个 "passed" 结果必须经过机制审计：确认通过原因是真实 C++ lowering（`chaos_eval_stack`），而非桩代码或 stub 的偶然正确。

**Fact 验证有三个层次**：

| 层次 | 验证内容 | 通过标准 | 当前状态 |
|------|---------|---------|---------|
| **L1: Codegen Success** | entrypoint DLL 经 `chaos-il2cpp convert-to-cpp` 翻译为真实 C++ lowering | 生成代码包含 `chaos_eval_stack` lowering 模式，非 SimpleForward | ✅ 正式接入 |
| **L2: Semantic Correctness** | 生成的 C++ 编译为 native exe，执行结果与 managed 预期 checksum 一致 | `verify_<family>.exe` 退出码 = 0，所有 assert 计数 = 0 | ✅ 正式接入（convert-char 18/18 通过，待扩展到全部 family） |
| **L3: Runtime Execution** | 编译后的 native exe 加载运行并校验完整运行时 | 完整运行时 bootstrap + 跨程序集解析通过 | ⚠️ 依赖运行时基础设施完成 |

此外新增审计门：

| 门 | 验证内容 | 通过标准 | 当前状态 |
|----|---------|---------|---------|
| **M1+: Mechanism + Principle Audit** | C++ lowering 是真实翻译 + 符合三条原则 | 无 false-passing + overallAlignment == "ALIGNED" | ⬜ 待实现 |
| **M2: Unity IL2CPP Parity** | L2 checksum 与 Unity IL2CPP 参考编译输出一致 | Chaos checksum == Unity checksum | ⬜ 待实现 |

### 已知问题（L3 Follow-up）

| 问题 | 影响范围 | 根因 | 修复路径 |
|------|---------|------|---------|
| 缺失 `InterpreterEntryDirect` 符号 | L3 编译失败 | `NameIndexModuleV0.dispatch_table` 设置 `interrupt_ptr = &InterpreterEntryDirect`，该符号在 `interpreter_entry.h` 中定义但未包含 | Phase 3.3 运行时入口 — 确保解释器头文件被包含 |
| 跨程序集类型引用 | L3 链接失败 | 程序集 A 引用程序集 B 的 TypeInfo 时使用 `extern const TypeInfo`，但链接时找不到 B 的符号 | Phase 3.2 + 4 — 每程序集独立编译 + 链接阶段解析 |

## 原则评估矩阵（自动化执行）

每个验证门的结果必须附带原则对齐评估。以下 7 项检查由 `principle_auto_checks.py` **全自动执行**，无人工判断。

> 另有 4 项不可自动化的检查（p2_layer_correctness、p2_no_code_duplication、p2_abstraction_balance、p3_no_hardcoded_state）降级为 dashboard 趋势监控，不参与门禁阻塞。

### P1 — 性能最优评估（4 项自动化检查）

```yaml
principle: 性能最优
priority: 1
checks:
  - id: p1_lowering                                   # 自动: 检查 chaos_eval_stack
    pass_condition: "generated C++ 包含 chaos_eval_stack，无 SimpleForward"
    automation: "principle_auto_checks.py check_p1_lowering()"
  
  - id: p1_no_redundant_branch                         # 自动: 检查 if(family==X) / switch(methodIndex)
    pass_condition: "生成代码中无运行时 family/method 条件分支"
    automation: "principle_auto_checks.py check_p1_no_redundant_branch()"
  
  - id: p1_no_unnecessary_indirection                  # 自动: 检查 runtime_stubs 调用
    pass_condition: "生成代码不经过 runtime_stubs.cpp 的已知桩函数"
    automation: "principle_auto_checks.py check_p1_no_unnecessary_indirection()"
  
  - id: p1_benchmark                                   # 自动: benchmark_comparator.py 输出
    pass_condition: "avg speedup > -50%，无 managed_faster 退化"
    automation: "principle_auto_checks.py check_p1_benchmark()"
```

### P2 — 方案完美性评估（1 项自动化检查 + 3 项监控）

```yaml
principle: 方案完美性
priority: 2
checks:
  - id: p2_template_hygiene                           # 自动: 扫描 Scriban 模板
    pass_condition: "Scriban 模板不含语义 if/else 决策"
    automation: "principle_auto_checks.py check_p2_template_hygiene()"

  # 以下 3 项不可自动阻塞，仅 dashboard 监控趋势：
  - id: p2_layer_correctness                          # 监控: 文件路径分布
    monitoring: "记录变更文件所属目录与预期架构层的匹配度"
  - id: p2_no_code_duplication                        # 监控: 跨 family 相似度
    monitoring: "跨 family 生成代码 fingerprint 相似度分析"
  - id: p2_abstraction_balance                        # 监控: 调用链深度
    monitoring: "函数调用链深度与同类 family 中位数偏差"
```

### P3 — 热更适配评估（2 项自动化检查 + 1 项监控）

```yaml
principle: 热更适配
priority: 3
checks:
  - id: p3_patch_entry                                # 自动: dispatch_table interrupt_ptr
    pass_condition: "dispatch_table 包含该方法的 interrupt_ptr 条目"
    automation: "principle_auto_checks.py check_p3_patch_entry()"
  
  - id: p3_patchdata_generation                       # 自动: .patchdata 文件存在
    pass_condition: ".patchdata 文件存在且非空"
    automation: "principle_auto_checks.py check_p3_patchdata()"

  - id: p3_no_hardcoded_state                         # 监控: 静态变量声明扫描
    monitoring: "扫描 global/static 变量声明，黑名单模式匹配"
```

### 原则冲突裁决

当 P1/P2/P3 之间出现冲突时，按以下规则裁决：

| 冲突类型 | 裁决规则 | 示例 |
|---------|---------|------|
| P1 vs P2 | 性能优先，但必须记录架构妥协 | SimpleForward 比真实 lowering 快但架构上不正确 → 选真实 lowering 并优化性能 |
| P1 vs P3 | 性能优先，但不应故意破坏热更 | 内联函数调用提升性能但绕过 dispatch 表 → 用 dispatch 表 + 内联优化 |
| P2 vs P3 | P2 优先于 P3 | 为热更引入架构 hack → 不应接受，需找干净的方案 |

### 原则对齐结果

每个 gate 完成后，必须在验证报告中记录：

```json
{
  "principleAlignment": {
    "performance": {
      "status": "ALIGNED" | "CONCERN" | "VIOLATION",
      "evidence": {
        "p1_lowering": "chaos_eval_stack present ✓",
        "p1_no_redundant_branch": "no runtime dispatch branches ✓",
        "p1_no_unnecessary_indirection": "direct lowering ✓"
      },
      "concerns": []
    },
    "architecturalPerfection": {
      "status": "ALIGNED",
      "evidence": {
        "p2_layer_correctness": "fix in emitter layer, consistent with arch table ✓",
        "p2_no_code_duplication": "no duplicated patterns ✓"
      },
      "concerns": []
    },
    "hotupdateCompatibility": {
      "status": "NOT_APPLICABLE",
      "evidence": {},
      "concerns": ["family has no hotupdate path"]
    }
  },
  "overallAlignment": "ALIGNED" | "CONCERN" | "VIOLATION"
}
```

### 禁止的 skip 类型

| 类型 | 示例 | 替代方案 |
|------|------|---------|
| out/ref 参数导致跳过 | `DateTime.TryParse` | 修复 `test_code_generator.py` 的 ref 参数解析，自动生成临时变量 |
| 泛型 T 无法解析 | `RuntimeHelpers.GetSubArray` | 实现泛型参数推导，默认用 `object` 或 `int` |
| Parse 方法跳过 | `Int32.Parse`, `Guid.Parse` | 使用有效输入字符串，无需 skip |
| 集合方法跳过 | `List.Remove`, `Dictionary.ContainsKey` | 先用 Add 填充集合再调用 |
| 内部 intrinsic | `Buffer.Memmove` | 在 codegen 中标记为 intrinsic 并生成 direct call |
| 线程操作 | `Thread.Start`, `Monitor.Enter` | 使用轻量同步替代，不依赖真正多线程 |

### 允许的例外

所有例外必须在 `FAMILY_SKIP_REGISTRY` 登记，包含：
- 方法 ID
- skip 原因（必须引用 Unity IL2CPP 也不支持的证据）
- 关联 issue/修复计划链接
- 下次审查日期

```json
{
  "methodSubjectId": "System.Private.CoreLib/System.Buffer::Memmove(...)",
  "skipReason": "internal runtime intrinsic, not in .NET 8 public API",
  "unityReference": "Unity IL2CPP also does not expose Buffer.Memmove as public API",
  "eliminationPlan": "Phase 4 codegen intrinsic support — track in issue #xxx",
  "reviewDate": "2026-08-01"
}
```

## Handwrite 测试集成

### 触发条件

AI review 过程中发现以下情况时，应注入 handwritten 测试：
1. Auto-generated entrypoint 因方法签名复杂（ref/pointer/generic）生成无效 C#
2. 方法的行为无法通过简单的 checksum 验证（例如返回 void 但需要验证副作用）
3. 需要验证边界条件（空参数、null 输入、越界索引）
4. Auto-generated 用例无法覆盖的 codegen 路径

### 规范

1. Handwrite entrypoint 放在 `handwritten/<Type>.Handwritten.cs`
2. 必须继承同一入口点接口（`INativeEntryPoint`），与 auto-generated 使用相同发现机制
3. 必须参与 L2 checksum 验证，不可绕过 assert 机制
4. 必须在验证报告中标记为 `source: handwritten`

### 验证管道

```text
handwritten entrypoint 与 auto-generated entrypoint 在以下方面使用同一管道：
  ✓ chaos-il2cpp convert-to-cpp
  ✓ chaos-il2cpp emit-native-aot
  ✓ L2 expected_checksums.h 生成
  ✓ native_verify_main.cpp 编译
  ✓ checksum assertion 执行

禁止的绕过方式：
  ✗ 跳过 convert-to-cpp 直接提供手写 C++
  ✗ 自定义 assert 逻辑绕过 Mechanism Audit
  ✗ 只跑 managed 测试不跑 native 验证
```

## Mechanism + Principle Audit (M1+)

### 审计规则

对每个声称 `"passed"` 的 native-proof claim，同时执行机制审计和原则对齐审计：

```python
# ── Mechanism Audit Rules ──
AUDIT_RULES = {
    "not_a_stub": "生成的 C++ 代码不能是桩（无 NativeReferenceStub_ 模式）",
    "has_real_lowering": "包含 chaos_eval_stack 真实 IL 降级",
    "no_skip_override": "方法不在 _METHOD_OVERRIDES skip 列表中",
    "assert_triggered": "L2 验证中 assert intrinsic 至少执行了一次（证明有实际比较）",
    "stub_detector_clean": "stub_detector 报告 is_stub=False",
}

# ── Principle Alignment Rules (7 automated + 4 monitored) ──
PRINCIPLE_RULES = {
    # 自动化阻塞检查
    "p1_lowering": "自动: chaos_eval_stack 存在，无 SimpleForward",
    "p1_no_redundant_branch": "自动: 无 if(family==X)/switch(methodIndex) 运行时分支",
    "p1_no_unnecessary_indirection": "自动: 无 runtime_stubs 调用",
    "p1_benchmark": "自动: benchmark speedup > -50%",
    "p2_template_hygiene": "自动: Scriban 模板无语义 if/else",
    "p3_patch_entry": "自动: dispatch_table 有 interrupt_ptr",
    "p3_patchdata": "自动: .patchdata 文件存在",

    # 监控 only (不阻塞，仅 dashboard 趋势)
    "p2_layer_correctness": "监控: 文件路径分布",
    "p2_no_code_duplication": "监控: 跨 family 相似度",
    "p2_abstraction_balance": "监控: 调用链深度",
    "p3_no_hardcoded_state": "监控: static 变量扫描",
}
```

### 审计输出

#### 单方法审计记录

```json
{
  "family": "convert-char",
  "method": "System.Convert::ToChar(int)",
  "claimed": "passed",
  "auditResult": "PASS",
  "evidence": {
    "has_real_lowering": true,
    "stub_detector": {"is_stub": false, "pattern_matched": null},
    "no_skip_override": true,
    "assert_count": 1
  },
  "principleAlignment": {
    "performance": {
      "status": "ALIGNED",
      "evidence": {
        "p1_lowering": "chaos_eval_stack present, no SimpleForward",
        "p1_no_redundant_branch": "no runtime dispatch branches",
        "p1_no_unnecessary_indirection": "direct lowering in generated code"
      },
      "concerns": []
    },
    "architecturalPerfection": {
      "status": "ALIGNED",
      "evidence": {
        "p2_layer_correctness": "emitter-generated lowering consistent with arch table",
        "p2_no_code_duplication": "standard lowering pattern, no duplication"
      },
      "concerns": []
    },
    "hotupdateCompatibility": {
      "status": "NOT_APPLICABLE",
      "evidence": {},
      "concerns": ["method not on hotupdate path"]
    }
  }
}
```

#### Family 级汇总

```json
{
  "family": "convert-char",
  "total_methods": 18,
  "audited": 18,
  "false_passing": 0,
  "stub_detector_clean": true,
  "principle_summary": {
    "performance": {"aligned": 18, "concern": 0, "violation": 0},
    "architectural_perfection": {"aligned": 18, "concern": 0, "violation": 0},
    "hotupdate_compatibility": {"aligned": 0, "concern": 0, "violation": 0, "not_applicable": 18}
  },
  "overall_alignment": "ALIGNED",
  "passed": true
}
```

### 失败处理

- 如果 audit 发现 false-passing（声称通过但实际是桩/特例），该 family 标记为 `audit:FAILED`
- 在 aggregate 报告中突出显示
- 阻塞该 family 的 "completed" 状态直到问题解决

## 管线上下文

如果在 `dev-foundation-dll-verification-pipeline` 中运行，数据完整性校验（Step 0）已在上游完成，本技能不重复执行。如果独立运行，先自行调用 `dev-foundation-dll-verify-data-integrity`。

## 前置条件

- 目标 family 已通过 `batch_native_aot_runner.py` 完整 pipeline（codegen + native compile）
- Solution 项目已生成在 `solution/System.Private.CoreLib/<family-slug>/`
- Native benchmark & hotupdate 可执行文件已构建在 `artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/`
- SDK 项目 `src/reference/Chaos.TestFramework.Sdk/` 已 `dotnet build --configuration Release`
- **（管线模式下已由上游确保）** claims 有 methodSubjectIds、coverage JSON 存在
- **Skip Registry 已加载**：`FAMILY_SKIP_REGISTRY` 中的例外已加载并验证未过期

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

### Step 1: Fact — Native AOT 语义正确性验证

> Fact 验证分三级。**L2（语义正确性）是基本通过标准**，M1（Mechanism Audit）是严格通过标准。

#### L1: Codegen Success（管线已自动完成）

batch pipeline 成功后即确认 L1 通过——生成的 C++ 代码有 native AOT lowering（`chaos_eval_stack` IL 降级），不是桩/SimpleForward。

验证方式：
```bash
python -c "
import re
cpp = open('verification/foundation-dll/System.Private.CoreLib/<family>/il2cpp_dist/genuine/generated/native-aot.generated.cpp').read()
has_lowering = 'chaos_eval_stack' in cpp
has_simple_forward = 'SimpleForward' in cpp
print(f'L1: AOT lowering={\"YES\" if has_lowering else \"NO\"} SimpleForward={\"YES\" if has_simple_forward else \"NO\"}')
"
```

#### L2: Semantic Correctness

> L2 验证器 `fact_l2_verifier.py` 使用 dual-verify 策略：
> 1. **Return value checksum** — native exe 的每个 entry 返回值与 managed L2Harness 预期值匹配
> 2. **Assert intrinsic** — CodeGen 将 `Assert.Equal(T, T)` 内联为 C++ 比较 + `__chaos_assert_failures` 计数器
>
> 已通过 convert-char 18/18 验证。**目标：所有 33 个 family 全部通过 L2。** L2 未覆盖的 family 不能声称 "native-proof passed"。

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
4. **所有方法的 assert_failures 必须 > 0**（证明 assert intrinsic 实际执行了比较，而非被优化掉）

如果 L2 失败（exit > 0），输出包含具体哪个 entry 的 expected vs actual 不匹配。

#### M1+: Mechanism + Principle Audit

L2 通过后，运行 M1+ Audit 确认通过基于真实翻译且符合三条原则：

```bash
# 一键执行 M1+ 审计（stub 检测 + 机制审计 + 7 项原则检查）
python build/toolchains/run/testing/foundation_dll/mechanism_audit.py \
  --family <family> \
  --assembly System.Private.CoreLib \
  --write

# 或者分步执行：
python build/toolchains/run/testing/foundation_dll/principle_auto_checks.py \
  --family <family> --output <family>/principle-alignment-report.json
python build/toolchains/run/testing/foundation_dll/skip_registry_auto.py --validate
```

审计内容：
```
1. stub_detector 扫描：所有声称 passed 的方法 is_stub=False
2. _METHOD_OVERRIDES 交叉检查：没有方法通过 skip 绕过验证
3. chaos_eval_stack 确认：每个被验证方法的 native-aot.generated.cpp 包含真实 lowering
4. assert 触发确认：每个方法的 assert 计数器 > 0
5. P1 原则对齐：lowering 选择高效方案（非 SimpleForward），无冗余分支
6. P2 原则对齐：修复/实现在正确的架构层，无重复代码
7. P3 原则对齐：涉及热更路径的方法有 dispatch table 条目
```

审计报告示例：
```
M1+ Audit: convert-char — PASS | PRINCIPLE: ALIGNED
  total_methods=18, audited=18, false_passing=0
  mechanism: 18/18 real lowering, 0 stubs, 0 skip_overrides, 18/18 assert_fired
  principle:
    P1 (performance):      18 ALIGNED, 0 concern, 0 violation
    P2 (architecture):     18 ALIGNED, 0 concern, 0 violation
    P3 (hotupdate):         0 N/A, 0 concern, 0 violation
  overall_alignment: ALIGNED
```

#### M2: Unity IL2CPP Parity（待实现）

Unity IL2CPP 参考管道就绪后，增加额外验证：

```bash
python build/toolchains/run/testing/foundation_dll/unity_parity_verifier.py \
  --family <family> \
  --chaos-checksum verification/.../expected_checksums.h \
  --unity-checksum verification/unity-reference/.../expected_checksums.h
```

**验证标准**：Chaos IL2CPP 的 expected_checksums.h 与 Unity IL2CPP 的 checksum 文件逐方法一致。

### Step 2: Benchmark — Managed vs Native 性能对比

#### 前置要求
- 每个 methodSubjectId 必须有对应的 benchmark case
- Handwrite benchmark 用例与 auto-generated 使用同一测量框架
- Benchmark 结果必须有 Unity IL2CPP 参考基线（当参考管道可用时）

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

**通过闸门**：
- 不允许任何 method 的 native 比 managed 慢超过 50%（`speedup < -50`）
- 有 Unity IL2CPP 基线时：Chaos native performance 不得低于 Unity IL2CPP native performance 的 80%

### Step 3: HotUpdate — Native 热更验证

#### 前置要求
- 每个 family 必须有 hotupdate test case（auto-generated 或 handwritten）
- Patch entry 生成的 `.patchdata` 文件必须存在
- 验证包含 patch apply + revert 两个方向

```bash
./artifacts/presets/windows-x64-reference/verification/foundation-dll/System.Private.CoreLib/<family-slug>/native/Release/chaos_hotupdate_<slug>.exe 2>/dev/null
```

输出 JSON 到 stdout。验证字段：
- `passedMethods` = `totalMethods`
- 每个 method: `status == "passed"`, `revertVerified == true`
- `patchedReturnValue == expectedPatchedValue`
- 不存在 `CHAOS_BRIDGE_STATUS_UNIMPLEMENTED` 返回值

### Step 4: 记录结果

每个 family 完成验证后，记录到所在目录供 `dev-foundation-dll-verify-aggregate` 读取：

```
verification/foundation-dll/System.Private.CoreLib/<family>/
  benchmark-comparison-report.json            (from Step 2c)
  hotupdate-verification-report.json          (from Step 3)
  mechanism-audit-report.json                 (from M1+ Audit — 含 principleAlignment)
  principle-alignment-report.json             (M1+ 的原则对齐分报告)
  skip-registry-audit.json                    (skip 例外审查结果)
```

**必须包含 M1+ Audit 结果（含原则对齐）才能进入聚合步骤。** 没有 M1+ 结果的 family 在 aggregate 中标记为 `audit:pending`。

## 输出接口

下游 `dev-foundation-dll-verify-aggregate` 会读取以下文件：

| 文件 | 来源 | 用途 |
|------|------|------|
| `benchmark-comparison-report.json` | Step 2c | 性能分析 |
| `hotupdate-verification-report.json` | Step 3 | 热更新验证 |
| `mechanism-audit-report.json` | M1+ Audit | 机制真实性 + 原则对齐验证 |
| `principle-alignment-report.json` | M1+ Audit | 原则对齐分报告（供 aggregate 直接消费） |
| `skip-registry-audit.json` | Skip Registry | 例外管理 |
| `managed_test/tests/` | Step 1 | Fact 结果目录 |

## 闸门失败处理

### 标准流程

```bash
# 1. 先查 trace 定位失败阶段
python -m build.toolchains.run.commands.trace_viewer --exception
python -m build.toolchains.run.commands.trace_viewer --stage <gate>  # gate = proof / benchmark / hotupdate

# 2. 分析 trace 输出
#    - phase="exception" 的行会显示 exception 类型和 message
#    - dur_ms 显示各阶段的耗时
#    - f 字段显示源文件:行号

# 3. 使用 dev-systematic-debugging 做根因分析
#    - 先在 trace 中找到失败的 exact 操作
#    - 然后查看对应的源文件行号
#    - 理解数据流后再修复

# 4. 修复后重跑当前 gate，确认通过

# 5. 确认通过后再继续下一 family 或下一 gate

# 6. 如果 3 次以上修复失败 → 调用 dev-systematic-debugging 质疑架构
```

### 各闸门特定检查

| 闸门 | trace 过滤 | 常见失败原因 |
|------|-----------|-------------|
| Fact | `--stage proof` | dotnet test 编译错误、断言失败 |
| M1+ Audit | `--stage audit` | stub 检测、skip 遗漏、assert 未触发、**原则违规** |
| Benchmark | `--stage benchmark` | 生成的 C++ 有性能退化 |
| HotUpdate | `--stage hotupdate` | stub 返回 `CHAOS_BRIDGE_STATUS_UNIMPLEMENTED` |

### 禁止

- 跳过 trace 直接猜测原因
- 不查根因就重跑
- 多次失败后不升级到 `dev-systematic-debugging`
- **通过新增 skip 来绕过失败的测试**（违反 No-Skip Policy — `skip_registry_auto.py --validate` 会自动检测）
- **修改 test_code_generator.py 的 `_METHOD_OVERRIDES` 来隐藏问题**（`skip_registry_auto.py --generate` 自动重新生成 registry，暴露变更）
- **违反自动化原则检查的修复**：7 项自动化原则检查（`principle_auto_checks.py`）全部为阻塞门禁

### Mechanism Audit 失败的特殊处理

如果 M1+ Audit 发现 false-passing 或原则违规：

```
1. 立即停止该 family 的验证
2. 标记该 family 为 audit:FAILED 或 principle:VIOLATION，在 dashboard 中高亮显示
3. 定位失败根因：
   a. false-passing（机制审计失败）：
      - stub 检测遗漏 → 更新 stub_detector 模式
      - skip 列表中不应 skip 的条目 → 移除 skip 并实现真实机制
      - assert 未触发 → 验证 L2 管道中 assert intrinsic 正确内联
   b. principle violation（原则违规）：
      - P1 性能违规 → 改用真实 lowering，移除 SimpleForward 或不必要的间接层
      - P2 架构违规 → 修复在正确的架构层，引用架构表确认
      - P3 热更违规 → 补全 dispatch table 条目或 patch data
4. 修复后重新从 Steps 1-3 完整验证，不可仅重跑 audit
```

## 关联

- 父流程: `dev-foundation-dll-verification-pipeline`
- 上游数据校验: `dev-foundation-dll-verify-data-integrity`
- 下游聚合: `dev-foundation-dll-verify-aggregate`
- 闸门失败调试: `dev-systematic-debugging` + `dev-trace-enforcement`
- Skip 治理: `wiki/06-测试验证/foundation-dll-skip-registry.md`
- Handwrite 规范: `wiki/06-测试验证/handwrite-test-guidelines.md`
