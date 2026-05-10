# Foundation DLL Family Verification — 标准化流程

## 概述

本文档定义 family 验证的标准化流程，基于 `convert-char` 的验证实践固化而来。所有 CoreLib family 验证必须按此流程执行。

## 前置条件

每个 family 目录下必须存在以下**源文件**（不会被 pipeline 删除或覆盖）：

| 文件 | 必需 | 说明 |
|------|------|------|
| `capability-family-contract.json` | ✅ | 定义 methodSubjectIds、customEntryIndices、义务标志 |
| `handwritten/*.Custom.cs` | 按需 | Handwrite 注入的 partial class entrypoint |
| `handwritten/*.csproj` | 按需 | Legacy 全项目模式（仅 interface-dispatch） |
| `review/*.md` | 按需 | Codegen review 文档 |
| `managed_test/tests/*.cs` | 按需 | Managed 单元测试（不会被 pipeline 运行） |
| `README.md` | 可选 | Family 说明 |

> `managed_test/benchmarks/` 也会被保留（pipeline 不会删除），但 pipeline 仅在它存在时才运行 managed benchmark。

## 验证执行步骤

```bash
# 标准模式（7 阶段全跑）
run foundation-dll verify-family --family <family-slug>

# 跳过特定阶段（调试时使用）
run foundation-dll verify-family --family <family-slug> --skip benchmark hotupdate
```

## 7 阶段输出产物

每个阶段在 `verification/foundation-dll/<Assembly>/<family>/` 下生成以下产物：

### 阶段 0: Preflight

- **不生成文件**
- 验证 contract 完整性，读取 methodSubjectIds，识别 customEntryIndices
- 输出到 unified report 的 preflight 阶段

### 阶段 1: Codegen

生成目录结构：

```
il2cpp_dist/
├── entrypoint/                          # C# 入口点项目
│   ├── <ClassName>NativeEntry.cs        # Auto-generated entry class
│   ├── <ClassName>NativeEntry.csproj    # 项目文件（引用 Shared Runner）
│   ├── <ClassName>PatchEntry.cs         # Patch entry（hotupdate 用）
│   ├── <ClassName>PatchEntry.csproj
│   ├── Program.cs                       # Main() 入口
│   ├── build-output/                    # dotnet build 产物
│   │   ├── <ClassName>NativeEntry.dll
│   │   ├── <ClassName>NativeEntry.exe
│   │   ├── Chaos.TestFramework.Runner.dll   # Shared Runner
│   │   └── Chaos.TestFramework.Sdk.dll
│   └── obj/                             # MSBuild 中间产物
├── genuine/                             # IL2CPP 转换输出
│   ├── <ClassName>/generated/
│   │   ├── native-aot.generated.cpp      # 生成的 C++ 代码（含 chaos_eval_stack）
│   │   ├── runtime_helper_shapes.h
│   │   └── entry.exe                     # 编译好的验证可执行文件（~845KB）
│   ├── build/                           # CMake 构建目录（含 .obj/.lib/.pdb）
│   ├── runtime-entry.cpp                # 增强版 runtime 入口（pipeline 提供）
│   ├── runtime-patchdata.cpp            # Patch 数据
│   ├── *.json                           # IL2CPP 元数据（aot-core-ir, manifest, closure 等）
│   └── hot-update/supplemental-metadata-template.json
└── patch/
    └── patchdata/
        └── <family>.patchdata           # 二进制 patch 数据
```

**代码生成后处理**（`_patch_bypass_0xC0000409.py`）：
1. 剥离 `Program::Main()` body → `return 0`
2. 生成方法指针调度表 `kAotMethods[]` + `extern "C" const int kAotMethodCount`
3. 生成 `.patchdata` → `runtime-patchdata.cpp`（`kPatchData[]/kPatchDataSize`）

### 阶段 2: Fact

| 文件 | 格式 |
|------|------|
| 写入 unified report 的 `stages.fact.details.fact` | `{"status": "passed", "passed": N, "total": N}` |

- 运行 `entry.exe`（无参数），解析 `Passed: N/M`（bitmask population count）
- 计数使用真实方法数，非硬编码 1/1

### 阶段 3: Audit

| 文件 | 生成 |
|------|------|
| `mechanism-audit-report.json` | ✅ 每次 pipeline 运行重新生成 |
| `principle-alignment-report.json` | ✅ 每次 pipeline 运行重新生成 |

**Mechanism Audit** 检查项：
- false_passing=0（所有 passed 方法基于真实 lowering）
- stubs_found=0（无 NativeReferenceStub_ 模式）
- skips_found 按 family 过滤（`relevant_type_names`），不再全局计数
- methods_without_lowering=0（所有方法有 chaos_eval_stack）

**Principle Alignment** 7 项自动检查：

| 检查 | 预期状态 | 说明 |
|------|---------|------|
| p1_lowering | ALIGNED | chaos_eval_stack 存在 |
| p1_no_redundant_branch | ALIGNED | 无运行时 family/method 条件分支 |
| p1_no_unnecessary_indirection | ALIGNED/CONCERN | 少量 runtime_stub 调用可接受（custom entry 场景） |
| p1_benchmark | ALIGNED | avg_speedup > -50% |
| p2_template_hygiene | ALIGNED | Scriban 模板无语义 if/else |
| p3_patch_entry | ALIGNED | dispatch_table 包含 interrupt_ptr |
| p3_patchdata | ALIGNED | .patchdata 文件存在且非空 |

> `methods_without_assert=21` 是正常值——runtime-entry.cpp 的 fact 模式使用返回码比较而非 assert intrinsic，所有 entry 函数会被标记为 assert 未触发。这不表示验证无效。

### 阶段 4: Benchmark

| 文件 | 生成 |
|------|------|
| `benchmark-comparison-report.json` | ✅ 每次 pipeline 运行重新生成 |
| `native/native-benchmark.json` | ✅ 原生 benchmark 原始数据 |
| `benchmark/managed-baseline.json` | ✅ 仅当 managed harness 存在时 |

- managed harness 位置：`managed_test/benchmarks/ManagedBenchmarkHarness.csproj`
- 若不存在 → 所有方法标记为 `managed_harness_unavailable`，avg_speedup=0%
- throwing 方法会被过滤出 benchmark 对比

### 阶段 5: HotUpdate

| 文件 | 生成 |
|------|------|
| `hotupdate-verification-report.json` | ✅ 每次 pipeline 运行重新生成 |

- 运行 `entry.exe --hotupdate`
- 解析 passedMethods/failedMethods/totalMethods
- 期望：18/18 passed，0 failed

### 阶段 6: Post-HotUpdate Benchmark

| 文件 | 生成 |
|------|------|
| `post-hotupdate-benchmark-report.json` | ✅ 每次 pipeline 运行重新生成 |

- 对每个非 stub 方法运行 `entry.exe --hotupdate-and-benchmark N`
- interpreter 路径下有 300-55000x slowdown 是正常的（interpreter 开销）
- **此项在 standard mode 下不阻塞 overall status**

### 阶段 7: Aggregate

| 文件 | 生成 |
|------|------|
| `unified-verification-report.json` | ✅ 每次 pipeline 运行重新生成 |

Coverage 计算公式：
```
methodCoverage = fact.details.fact.passed / total    # 非 l2（历史 Bug 修复）
skipRate       = max(0, 1 - skipsFound/total_methods)  # max(0, ...) 防负值（历史 Bug 修复）
overall        = mean(methodCoverage, skipRate)
```

## 运行日志证据

每次运行在 `artifacts/logs/` 下生成：

| 路径 | 内容 |
|------|------|
| `run/<session>/events.jsonl` | 4 事件：session-start, stage-start, progress, final-summary |
| `run/<session>/summary.json` | finalStatus=ok, exitCode=0 |
| `run/<session>/console.log` | 完整控制台输出 |
| `run/<session>/telemetry.json` | 性能数据 |
| `trace/<session>/trace.jsonl` | 跨语言 span（Python + C# driver） |

**Trace 初始化要求**：`verify_family()` 入口处必须调用 `trace_init(_REPO_ROOT, stage=f"verify-family/{family_slug}")`，否则 Python 侧的 trace span 不会写入 trace.jsonl。已在 `family_verification_orchestrator.py:verify_family()` 中默认添加。

**已知限制**:
- orchestrator 内部 `trace()` 调用在 run.py 集成路径下可能不持久化到 trace.jsonl（`trace_init` 需先被调用）。仅有 `session_start` 和 C# driver span 被写入。
- trace_init 是幂等的——多次调用不会重复初始化。

## 验证通过标准（Standard 模式）

## 验证通过标准（Standard 模式）

| 阶段 | 必须通过 |
|------|---------|
| Preflight | ✅ 通过（contract 存在，methodSubjectIds 非空） |
| Codegen | ✅ 通过（entry.exe 编译成功） |
| Fact | ✅ 通过（N/N = 100%） |
| Audit | ✅ 通过（false_passing=0，principle != VIOLATION） |
| Benchmark | 不阻塞 |
| HotUpdate | 不阻塞 |
| PostHotBench | 不阻塞 |

**coverage 指标**：
- methodCoverage: > 0（应为 1.0）
- skipRate: [0, 1]（应为 1.0）
- overall: > 0（应为 1.0）

## 故障排查

| 症状 | 根因 | 修复 |
|------|------|------|
| LNK2001: kAotMethodCount | `_patch_bypass_0xC0000409.py` 中定义为 `static constexpr` | 改为 `extern "C" const int` |
| LNK2001: kPatchDataSize | `_generate_patch_data()` 生成的 `runtime-patchdata.cpp` | 使用 `extern const` |
| Fact 输出 1/1 | kAotMethodCount 不可见（未用 extern "C"） | 同上 |
| skipsFound 异常（负 skipRate） | mechanism_audit 未按 family 过滤 | 检查 `_get_skip_entries()` 的 `relevant_type_names` 参数 |
| managed_harness_unavailable | managed_test/benchmarks/ 不存在 | 这是正常的——没有 managed harness 时跳过对比 |
| post_hotupdate_benchmark failed | interpreter 路径的固有高延迟 | 标准模式下不阻塞 |

## Dashboard 更新

每次完成 family 验证后，必须刷新 dashboard 以反映最新结果：

```bash
# 刷新所有 projection（含 foundation-dll audit dashboard）
run verify verification-v1 --json
```

该命令重新生成以下输出：
- `verification/projections/foundation-dll-audit/dashboard.html`
- `docs/verification/foundation-dll-audit/dashboard.html`
- `verification/projections/foundation-dll-audit/family-verification.json`
- 每个程序集的详细 HTML 页面（含 fact/benchmark/hotupdate 子页面）

> **注意**：dashboard 数据来源是 `unified-verification-report.json` + contract + case indexes。dashboard 的 `nativeProof` 字段需要 `native-reference.runtime-skeleton.coverage.json` 才有详情数据；纯 pipeline 跑完的 fact 18/18 会显示为 `0/18`（因为缺少 runtime-skeleton coverage evidence），但 `nativeCorrect` 状态会保持 `pending`（等待批量管线接入）。

## AI 验证 Dashboard 正确性

每次 dashboard 刷新后，必须通过以下 checklist 验证输出正确性：

### 验证点

| 检查项 | 预期值 (convert-char) | 验证方法 |
|--------|----------------------|---------|
| fact 阶段 | 18/18 passed | 对照 `unified-verification-report.json` stage 2 |
| benchmark 阶段 | avg_speedup=0.0%，所有方法 `managed_harness_unavailable` | 对照 `benchmark-comparison-report.json` |
| hotupdate 阶段 | 18/18 passed，0 failed | 对照 `hotupdate-verification-report.json` |
| coverage 指标 | methodCoverage=1.0, skipRate=1.0, overall=1.0 | 对照 `unified-verification-report.json` |
| audit 结果 | false_passing=0, principle=CONCERN | 对照 `mechanism-audit-report.json` + `principle-alignment-report.json` |
| overall_status | passed | 对照 `unified-verification-report.json` |

### 步骤

1. 刷新 dashboard：`run verify verification-v1 --json`
2. 读取 `family-verification.json` 中对应 family 的 `nativeProof`、`benchmarkProof`、`hotupdateProof` 字段
3. 读取详细 HTML 页面核对：`dlls/<Assembly>/families/<slug>-fact.html`、`-benchmark.html`、`-hotupdate.html`
4. 确认 `unified-verification-report.json` 中的数值与 dashboard 显示一致
5. 如果发现不一致：先检查 pipeline 输出产物是否正确，再检查 dashboard generator 是否读取到最新数据

### 常见 dashboard 数据问题

| 症状 | 原因 |
|------|------|
| fact 页显示 `0/18` 而非 `18/18` | dashboard nativeProof 来自 runtime-skeleton coverage，而非 pipeline fact 阶段——纯 pipeline 跑完不产生 coverage.json |
| hotupdate 页显示 `passed 0/0` | hotupdateProof 数据格式为 schemaVersion 1，dashboard 需要 schemaVersion 2 才能渲染详情 |
| `nativeCorrect` 显示 `pending` | 需要批量管线结果文件 `batch-native-aot-pipeline-results.json`；单 family 验证不会生成此文件 |

## 与其他流程的集成

```bash
# 1. 清理旧数据后跑单个 family（验证 pipeline 工作正常）
run foundation-dll verify-family --family <family-slug>

# 2. 批量跑所有 family（全量验证）
# （需要批量脚本或编排器）

# 3. 验证 audit + 原则
# 已包含在 verify-family 中，无需单独运行
```