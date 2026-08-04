# Capability Family Test-Code-First Pipeline — Design v1

## 1. 总体架构

```
[Developer] 写 managed 功能代码
     │
     ▼
[Codegen Pipeline] 自动运行
     │  → coverage.json（emittedMethodCount / uncoveredMethodSubjectIds）
     │  → .cpp dispatch tables
     ▼
┌──────────────────────────────────────────────────┐
│  Gap Analyzer (foundation-dll analyze-gaps)       │
│  → 对比 ledger family 的 emitted vs test code     │
│  → 输出每个 family 的 testCodeStatus               │
│  → --auto-generate 触发骨架生成                    │
└──────────────────┬───────────────────────────────┘
                   │ 如果 testCodeStatus = "needs-tests"
                   ▼
┌──────────────────────────────────────────────────┐
│  Test Code Generator (test_code_generator.py)     │
│  → 从 coverage.json 提取方法签名                    │
│  → 生成 partial class 测试骨架                      │
│  → 生成 .csproj / benchmark stubs / hotupdate      │
│  → 输出到 tests/families/{assembly}/{family}/      │
└──────────────────┬───────────────────────────────┘
                   │ Developer 填充 Assert 后
                   ▼
┌──────────────────────────────────────────────────┐
│  Test Discovery (dotnet build + 反射)              │
│  → 扫描 [CapabilityTest] attribute                │
│  → 扫描 [Benchmark] / [Fact] 方法                 │
│  → 输出 test matrix JSON                           │
└──────────────────┬───────────────────────────────┘
                   │
                   ▼
┌──────────────────────────────────────────────────┐
│  Pipeline: 7 Stages (Plugin-based)                │
│  Stage 1: managed test                            │
│  Stage 2: native verify                           │
│  Stage 3: benchmark                               │
│  Stage 4: hotupdate analysis                      │
│  Stage 5: hotupdate test                          │
│  Stage 6: hotupdate benchmark                     │
│  Stage 7: evidence collection + aggregation       │
└──────────────────┬───────────────────────────────┘
                   │
                   ▼
┌──────────────────────────────────────────────────┐
│  Ledger Write-back + Dashboard Refresh            │
│  → 更新 family 的 verificationGates               │
│  → 生成 {Dll}.tooltips.json（on-demand 数据）     │
│  → Family table: progress bar + hover tooltip     │
│  → Test Gap 列                                    │
└──────────────────────────────────────────────────┘
```

## 2. SDK Attribute 层（分线 1）

### 2.1 CapabilityTestAttribute（新增）

class-level，一个类一个：

```csharp
[CapabilityTest("System.Private.CoreLib/convert/char")]
public class ConvertCharTests { ... }
```

属性：

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| FamilyId | string | (required) | Ledger 中的 family ID |
| IncludeNativeProof | bool | true | 是否验证 native proof |
| IncludeBenchmark | bool | false | 是否验证 benchmark |
| IncludeHotUpdate | bool | false | 是否验证 hotupdate |
| ExecutionModes | ChaosExecutionMode | All | 执行模式 |

### 2.2 BenchmarkAttribute（从 ChaosBenchmarkAttribute 改名）

新增强类型阈值：

| 属性 | 类型 | 说明 |
|------|------|------|
| P99Ms | int | P99 延迟上限 (ms) |
| MinOpsPerSec | int | 最低吞吐量 (ops/sec) |
| MaxAllocBytes | int | 最大分配 (bytes) |

`ChaosBenchmarkAttribute` 保留为 `[Obsolete]` 子类，保证向后兼容。

### 2.3 开发者用法

```csharp
[CapabilityTest("System.Private.CoreLib/convert/char")]
public class ConvertCharTests
{
    // managed proof + native proof
    [Fact]
    public void ToBoolean_True() { Assert.True(Convert.ToBoolean(true)); }

    // benchmark（强类型阈值）
    [Benchmark(ChaosBenchmarkCategory.RuntimeDispatch, ChaosMetric.WallClockUs,
               P99Ms = 100)]
    [Fact]
    public void ToBoolean_Benchmark() { /* loop call + assert */ }

    // hotupdate
    [ChaosUnitTest(ChaosUnitCategory.HotUpdateContract)]
    [Fact]
    public void ToBoolean_HotUpdate() { Assert.True(Convert.ToBoolean(true)); }
}
```

## 3. Gap Analyzer（分线 4）

### 3.1 命令

```bash
foundation-dll analyze-gaps                              # 全量
foundation-dll analyze-gaps --dll System.Private.CoreLib  # 单 DLL
foundation-dll analyze-gaps --family convert-char         # 单 family
foundation-dll analyze-gaps --auto-generate               # 自动生成骨架
foundation-dll analyze-gaps --dry-run                     # 只看不改
foundation-dll analyze-gaps --update-ledger               # 写回 ledger
```

### 3.2 输出

每个 family 的状态机：

| emittedCount | testCode 存在 | 上次状态 | testCodeStatus | action |
|---|---|---|---|---|
| 0 | 任意 | 任意 | no-coverage | none |
| >0 | 否 | 任意 | needs-tests | write-tests |
| >0 | 是 | needs-tests | coverage-widened | update-tests |
| >0 | 是 | present/coverage-widened | present | none |

### 3.3 写入 ledger

```json
{
  "familyId": "family/System.Private.CoreLib/convert/char",
  "verificationGates": { ... },
  "testCode": {
    "testCodeStatus": "present",
    "emittedMethodCount": 18,
    "requestedMethodCount": 18,
    "allMethodsCovered": true,
    "action": "none"
  }
}
```

## 4. Test Code Generator（分线 3）

### 4.1 输出结构

```
tests/families/
  System.Private.CoreLib/
    convert-char/
      ConvertCharTests.cs                ← 手写（不覆盖）
      ConvertCharTests.AutoGenerated.cs  ← 自动生成（可覆盖）
      ConvertCharTests.csproj            ← collect file
      ConvertCharBenchmarks.cs           ← benchmark stubs
      ConvertCharHotUpdateTests.cs       ← hotupdate stubs
      README.md                          ← generation note
```

### 4.2 关键设计

- 使用 `partial class` 隔离手写和自动生成代码
- `AutoGenerated.cs` 每次重新生成，`{Class}.cs` 永不覆盖
- 新方法从 coverage.json 的 `uncoveredMethodSubjectIds` 提取签名
- 生成的方法体为 `throw new NotImplementedException()`，gate evaluator 检测到则 blocked

## 5. Test Discovery（分线 2）

### 5.1 流程

```
dotnet build tests/families/{assembly}/{family}/{Class}.csproj
  → 加载 assembly
  → 反射扫描 [CapabilityTest]
  → 提取 familyId, gate flags
  → 扫描 [Fact] 方法 → managed-proof
  → 扫描 [Benchmark] + thresholds → benchmark
  → 扫描 [ChaosUnitTest(HotUpdateContract)] → hotupdate-proof
  → 输出 test matrix JSON
```

### 5.2 Test Matrix 格式

```json
{
  "familyId": "System.Private.CoreLib/convert/char",
  "testProject": "tests/families/System.Private.CoreLib/convert-char/",
  "testClass": "ConvertCharTests",
  "gates": {
    "managed-proof":   {"enabled": true,  "testCount": 4},
    "native-proof":    {"enabled": true,  "testCount": 4},
    "benchmark":       {"enabled": true,  "testCount": 1, "thresholds": {"P99Ms": 100}},
    "hotupdate-proof": {"enabled": true,  "testCount": 1}
  }
}
```

## 6. Pipeline Plugins（分线 5）

### 6.1 接口

```python
class FamilyGateEvaluator(ABC):
    gate_code: str

    def evaluate(
        self,
        family: dict,
        test_metadata: FamilyTestMetadata | None,
        evidence_bundle: EvidenceBundle,
    ) -> GateEvaluation:
        """返回 gate 状态和证据链接"""
```

### 6.2 按阶段推进

| 阶段 | Evaluator | 数据来源 | 可用性 |
|------|-----------|---------|--------|
| Phase 1 | NativeProofEvaluator | coverage.json | 立刻可用 |
| Phase 2 | ManagedProofEvaluator | test report | 需要 test 结果 |
| Phase 3 | BenchmarkEvaluator | benchmark report | 需要 benchmark 数据 |
| Phase 4 | HotUpdateEvaluator | hotupdate report | 需要 hotupdate 分析 |
| Phase 5 | AuditInputEvaluator | 文件检查 | 立刻可用 |
| Phase 6 | CodegenReviewEvaluator | review 标记 | 需要人工标记 |

### 6.3 关键约束

`NativeProofEvaluator` 中：如果 `testCodeStatus = "absent"` → blocked（不推进）

```python
def evaluate(self, family, ...):
    if family.get("testCode", {}).get("testCodeStatus") == "needs-tests":
        return GateResult(status="blocked", reason="test code required")
    # 正常评估逻辑
```

## 7. Dashboard 改造（分线 6）

### 7.1 Family Table 变化

当前：
```
| Native Proof |
|  ● in-progress |
```

改造后：
```
| Native Proof [████████░░] 67% ● in-progress |
```

### 7.2 On-demand Tooltip

数据文件：`{DllName}.tooltips.json`（每个 DLL 一个）

```json
{
  "families": {
    "Convert.Char Conversions": {
      "native-proof": {
        "progress": 67, "numerator": 12, "denominator": 18,
        "runs": [
          {"runId": "e72d", "status": "passed", "n": 12, "d": 18,
           "evidence": [
             {"label": "coverage.json", "path": "artifacts/..."},
             {"label": "page-0013.cpp", "path": "artifacts/..."}
           ]}
        ]
      }
    }
  }
}
```

渲染：JS 在 hover 时按需 fetch `tooltips.json`，渲染 rich tooltip。

### 7.3 新增 Test Gap 列

```
# | Family | ... | methodCount | Test Gap
1 | Convert.Char | ... | 18 | ✅ present
2 | Primitive Numeric | ... | 0 | ⚠️ needs-tests
```

## 8. 开发流程约束（分线 7）

| 阶段 | 触发 | 动作 |
|------|------|------|
| 开发中 | `foundation-dll check-family --family xxx` | 手动检查单个 family 的 gap |
| 提交前 | pre-commit hook | `foundation-dll analyze-gaps --fail-if-absent` |
| Codegen 后 | pipeline | 自动 analyze-gaps + auto-generate |
| PR 时 | CI | 检查 test gap，PR comment 报告 |
| 日常 | CI daily | 全量 gap 扫描 → 更新 dashboard |

## 9. 关键文件清单

### 新增文件

| 文件 | 行数 | 分线 |
|------|------|------|
| `src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.cs` | ~1500(改) | 1 |
| `build/toolchains/run/testing/foundation_dll/gap_analyzer.py` | ~280 | 4 |
| `build/toolchains/run/testing/foundation_dll/test_code_generator.py` | ~350 | 3 |
| `build/toolchains/run/testing/foundation_dll/test_discovery.py` | ~180 | 2 |
| `build/toolchains/run/testing/foundation_dll/plugins/evaluator/native_proof.py` | ~100 | 5 |
| `build/toolchains/run/testing/foundation_dll/plugins/evaluator/managed_proof.py` | ~80 | 5 |
| `build/toolchains/run/testing/foundation_dll/plugins/evaluator/benchmark.py` | ~80 | 5 |
| `build/toolchains/run/testing/foundation_dll/plugins/evaluator/hotupdate.py` | ~80 | 5 |
| `build/toolchains/run/testing/foundation_dll/plugins/evaluator/audit_input.py` | ~60 | 5 |
| `build/toolchains/run/testing/foundation_dll/plugins/evaluator/codegen_review.py` | ~50 | 5 |

### 修改文件

| 文件 | 改动 | 分线 |
|------|------|------|
| `build/toolchains/run/commands/foundation_dll.py` | 新增 analyze-gaps handler + routing | 4, 7 |
| `build/toolchains/run/manifests/run/commands.test-and-verify.json` | 注册 analyze-gaps, check-family 命令 | 4, 7 |
| `build/toolchains/run/testing/foundation_dll_audit_generator.py` | progress bar + tooltip + gap 列渲染 | 6 |
| `verification/projections/foundation-dll-audit/dashboard.css` | tooltip + progress bar 样式 | 6 |
| `build/toolchains/run/testing/foundation_dll/derive.py` | 初始化 testCode 字段 | 4 |
| `build/toolchains/run/testing/foundation_dll/promote.py` | 传播 testCode 字段 | 4 |
