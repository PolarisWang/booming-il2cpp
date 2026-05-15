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
# 标准模式（8 阶段全跑）
run foundation-dll verify-family --family <family-slug>

# 跳过特定阶段（调试时使用）
run foundation-dll verify-family --family <family-slug> --skip benchmark hotupdate

# 跳过 asm-compare 阶段（无需 instruction-level 对比时）
run foundation-dll verify-family --family <family-slug> --skip asm_compare
```

## 8 阶段输出产物

每个阶段在 `verification/foundation-dll/<Assembly>/<family>/` 下生成以下产物：

### 阶段 0: Preflight

- **不生成文件**
- 验证 contract 完整性，读取 methodSubjectIds，识别 customEntryIndices
- 输出到 unified report 的 preflight 阶段

### 阶段 1: Codegen

生成目录结构（新结构，已验证 convert-char 可用）：

```
managed/
├── subjects/                            # Subjects DLL 项目
│   ├── <ClassName>Subjects.cs             # Auto-generated subject methods
│   ├── <ClassName>Subjects.csproj         # Library 项目（纯代码，无 TestFramework 依赖）
│   └── build-output/                      # dotnet build 产物（.dll）
codegen/
├── <AssemblyName>/generated/             # IL2CPP 转换输出
│   └── native-aot.generated.cpp           # 生成的 C++ 代码
native/
├── CMakeLists.txt                        # 每个 family 一份（模板固定，pipeline 不覆盖）
├── runtime-entry.cpp                     # 增强版 runtime 入口（pipeline 从 toolchain 复制）
├── runtime-patchdata.cpp                 # Patch 数据（pipeline 自动生成）
├── entry.exe                             # 编译好的验证可执行文件
└── build/                                # CMake 构建目录（.obj/.lib/.pdb）
managed/patch/                            # Patch 变体项目（由 _generate_patch_data 管理）
├── <ClassName>PatchEntry.cs
├── Program.cs
└── patchdata/
    └── <family>.patchdata                # 二进制 patch 数据
```

**关键变更**（对比旧 il2cpp_dist/ 结构）：
1. **Subjects DLL** 取代旧 entrypoint EXE，作为 convert-to-cpp 输入
2. **codegen/** 取代 il2cpp_dist/genuine/，输出到 `codegen/<Assembly>/generated/`（按 assembly 分组）
3. **native/** 集中管理 CMake 项目、runtime 入口、build 目录、entry.exe
4. **无后处理**：`_patch_bypass_0xC0000409.py` 不再需要（新结构不产生 Program::Main）

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

### 阶段 4: AsmCompare

| 文件 | 生成 |
|------|------|
| `asm-compare-report.json` | ✅ 每次 pipeline 运行重新生成 |

- 对 contract 中每个 methodSubjectId 运行 `chaos-il2cpp asm-compare --format json --sections metrics`
- 聚合确定性指标：
  - JIT 指令数 / AOT IR 指令数 / IR 膨胀比
  - JIT 代码大小 / AOT 近似代码大小
  - 外部 runtime 调用数 / virtual dispatch 数 / boxing 操作数
- 输出汇总统计（min/max/avg/total per metric）
- **不阻塞 overall status**（advisory stage）
- 当 subjects DLL 不存在时自动跳过（依赖 codegen 阶段先执行）
- 替换了 AI 自分析 managed vs native 代码——现在由确定性工具产生数据

### 阶段 5: Benchmark

| 文件 | 生成 |
|------|------|
| `benchmark-comparison-report.json` | ✅ 每次 pipeline 运行重新生成 |
| `native/native-benchmark.json` | ✅ 原生 benchmark 原始数据 |
| `benchmark/managed-baseline.json` | ✅ 仅当 managed harness 存在时 |

- managed harness 位置：`managed_test/benchmarks/ManagedBenchmarkHarness.csproj`
- 若不存在 → 所有方法标记为 `managed_harness_unavailable`，avg_speedup=0%
- throwing 方法会被过滤出 benchmark 对比

### 阶段 6: HotUpdate

| 文件 | 生成 |
|------|------|
| `hotupdate-verification-report.json` | ✅ 每次 pipeline 运行重新生成 |

- 运行 `entry.exe --hotupdate`
- 解析 passedMethods/failedMethods/totalMethods
- 期望：18/18 passed，0 failed

### 阶段 7: Post-HotUpdate Benchmark

| 文件 | 生成 |
|------|------|
| `post-hotupdate-benchmark-report.json` | ✅ 每次 pipeline 运行重新生成 |

- 对每个非 stub 方法运行 `entry.exe --hotupdate-and-benchmark N`
- interpreter 路径下有 300-55000x slowdown 是正常的（interpreter 开销）
- **此项在 standard mode 下不阻塞 overall status**

### 阶段 8: Aggregate

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
| AsmCompare | 不阻塞（advisory — instruction-level deterministic metrics） |
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
| asm-compare 所有方法 fail 或 skip | subjects DLL 未构建或 contract 为空 | 先跑 codegen 阶段，再跑 asm-compare |
| asm-compare JSON 解析失败 | driver 输出非 JSON 前缀（如错误信息） | 查看 stdout/stderr，确认 asm-compare --format json 生效 |
| asm-compare 超时 | 方法过于复杂或 pipeline 过载 | 增大超时或跳过 asm_compare 阶段 |

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

## Post-Pipeline AI Analysis

Pipeline 产出 raw 验证数据（unified report、benchmark 对比、审计报告、**asm-compare-report.json** 确定性 JIT vs AOT metrics）。**分析验收交由 AI 完成**，使用 `dev-foundation-dll-verify-analysis` 技能。

**变更说明**：Managed vs native 代码分析不再由 AI 直接阅读源码判断，而是消费 asm-compare stage（stage 4）产出的 `asm-compare-report.json`。该文件包含 deterministic instruction-level metrics（指令数、IR 膨胀比、dispatch 分布、boxing 操作数等），消除了 AI 自分析 managed/native 代码的不确定性和幻觉风险。AI 分析阶段的角色从"阅读并理解 managed/native 代码"转变为"解读确定性指标并给出优化建议"。

### 调用方式

```bash
# 直接对 AI 说：
# "请用 dev-foundation-dll-verify-analysis 技能分析 <family-slug>"
```

该技能会自动：
1. 重新执行 verify-family pipeline（确保数据最新）
2. 执行 Handwrite/AutoGen Pre-check
3. 读取全部 14 种产物（reports、logs、contract、dashboard projection 等）
4. 按 11 维度标准化模板逐项分析
5. 输出分析报告到 `verification/reports/<family>-verification-analysis-<YYYYMMDD>.md`
6. 更新 `verification/reports/analysis-index.md`

### 分析模板覆盖的维度

| 维度 | 缩写 | 核心问题 |
|------|------|---------|
| A. Pipeline 数据正确性 | correctness | fact/benchmark/hotupdate/coverage 数据是否可信 |
| B. 性能分析 | performance | native vs managed 速度对比、异常值、JIT 影响 |
| C. 架构质量 | architecture | principle alignment、mechanism audit、lowering |
| D. 热更新适配 | hotupdate | patch 完整性、interpreter 路径、dispatch |
| E. 内存/体积 | footprint | entry.exe、生成 C++、IL2CPP 产物大小 |
| F. 正确性 | correctness-depth | fact 失败根因、contract 一致性、custom entry |
| G. 完整性（外围产物） | completeness | review/handwrite/managed-test/benchmark 是否存在 |
| H. 效率 | efficiency | pipeline 耗时、瓶颈、稳定性 |
| I. 维护性 | maintainability | handwrite 质量、contract 完整度、文档 |
| J. Dashboard 输出验证 | dashboard | projection 一致性、详情页数据、证据链接 |
| K. 综合评定 | verdict | 整体评分、风险、改进建议、验收建议 |

### 验收建议规则

| 条件 | 建议 |
|------|------|
| 所有维度得分 >= 4，且无 blocking 问题 | **approved** |
| 有维度得分 < 4，但无结构性失败 | **review-needed** |
| 存在结构性失败（fact rate < 0.9 / false_passing > 0 / contract 无效） | **blocked** |

### AI 分析报告位置

```
verification/reports/
├── <family>-verification-analysis-<YYYYMMDD>.md   # 完整分析报告
└── analysis-index.md                               # 摘要索引（每次新增一行）
```

### 人工审核流程

1. 读取 `verification/reports/` 下最新分析报告
2. 核对关键数据是否与 pipeline 原始产物一致
3. 根据推荐（approved/review-needed/blocked）决定 closureStatus
4. 确认后刷新 dashboard：`run verify verification-v1 --json`

> **注意**：AI 分析报告是辅助人工决策的参考，**不是自动门禁**。最终验收决定由人工做出。

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