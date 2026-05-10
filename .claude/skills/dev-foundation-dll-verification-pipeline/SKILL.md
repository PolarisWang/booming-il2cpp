---
name: dev-foundation-dll-verification-pipeline
description: Master orchestrator for foundation DLL verification — unified verify_family() 7-stage pipeline. Enforces no-skip policy, mechanism audit, principle alignment.
---

# Foundation DLL Verification Pipeline

## 概述

完整的 foundation DLL 验证管线编排。统一入口为 `verify_family()`（`family_verification_orchestrator.py`），覆盖 codegen → fact → audit → benchmark → hotupdate → aggregate。

**核心原则**（所有验证必须遵守）：

1. **L2/L3/Audit 必须通过**才能 claim "native-proof passed"
2. **Mechanism Audit (M1+)** 确保每个 passed 结果基于真实翻译而非 stub/skip
3. **No-Skip Policy** — 不可通过新增特例绕过测试失败
4. **Handwrite 用例与 auto-generated 用例共享同一验证管道**
5. **Dashboard 必须反映 Audit（含 Principle Alignment）结果**
6. **Principle Alignment** — overallAlignment == "ALIGNED" 才能 closureStatus=closed

**入口信号**：`dev-foundation-dll-verification-pipeline`

## 入口

```bash
# 单 family 验证（标准模式 — Preflight + Codegen + Fact + Audit）
run foundation-dll verify-family <family-slug>
run foundation-dll verify-family <family-slug> --skip benchmark

# 严格模式（额外包含 HotUpdate + PostHotBench）
run foundation-dll verify-family <family-slug> --mode strict

# 指定 assembly
run foundation-dll verify-family <family-slug> --assembly System.Private.CoreLib
```

代码入口：`build/toolchains/run/testing/foundation_dll/family_verification_orchestrator.py:verify_family()`

## 7 阶段管线

```
[0] Preflight   — 检查 capability-family-contract.json 存在性、提取 methodSubjectIds、检测 custom entry
                  (_stage_preflight)
                  ❌ 失败 = 停止，不继续

[1] Codegen     — 委托 pipeline_native_aot_runner.run_family()
                  1. 生成 entrypoint C# → dotnet build DLL
                  2. chaos-il2cpp convert-to-cpp → native-aot.generated.cpp
                  3. _patch_bypass_0xC0000409.py 后处理：
                     - 剥离 Program::Main() body → return 0
                     - 生成方法指针调度表 kAotMethods[] + kAotMethodCount
                     - 生成 BenchmarkMethod(entry_index, iterations) 函数，直接调用
                       kAotMethods[index]()，绕过 DispatchSlotGet/HotpatchLookupBySlot 开销
                  4. 生成 .patchdata → runtime-patchdata.cpp（kPatchData[]/kPatchDataSize）
                  5. CMake build entry.exe（使用增强版 runtime-entry.cpp 支持 5 种模式）
                  run_family() 只负责构建，不再运行 fact/benchmark/hotupdate 验证
                  ❌ 失败 = strict 模式下停止

[2] Fact        — 委托 fact_verifier.verify_fact()
                  运行 entry.exe，解析 stdout 中 Passed: N/M
                  N/M 现在使用真实方法计数（bitmask population count），不再是硬编码 1/1
                  ❌ 失败 = 记录，不阻塞继续

[3] Audit       — 委托 mechanism_audit.run_full_audit()
                  stub 检测 + _METHOD_OVERRIDES skip 审计 + 7 项原则检查
                  ❌ 失败 = 记录，不阻塞继续

[4] Benchmark   — 托管 vs 原生性能对比
                  1. stub_detect 解析 entrypoint 源码识别 stub 方法
                  2. 自动生成 managed benchmark harness（从 entrypoint Program.cs 提取
                     方法调用序列，包裹 in Stopwatch 循环）。managed 端也在 native AOT 内
                     执行，通过运行 entry.exe --managed-benchmark N 获取基线
                  3. 如 managed harness 已存在（managed_test/benchmark/*.cs），使用已有的；
                     不存在则自动生成临时 harness
                  4. 对每个非 stub 方法运行 entry.exe --benchmark N
                     - native 端调用 BenchmarkMethod(index, iterations) — 生成代码内直接
                       调用 kAotMethods[index]()，绕过 hotpatch dispatch 开销
                     - 测量纯 AOT 生成代码的真实性能
                  5. benchmark_comparator.compare() 生成对比报告
                  输出 benchmark-comparison-report.json
                  [标准模式 advisory；无 managed harness 时退化为 native-only 记录]

[5] HotUpdate   — 热补丁验证
                  1. stub_detect 解析 entrypoint 源码
                  2. 运行 entry.exe --hotupdate
                  3. 解析 JSON 输出（passedMethods/failedMethods/totalMethods）
                  输出 hotupdate-verification-report.json
                  [严格模式 required]

[6] PostHotBench — 热补丁后性能对比（interpreter 路径）
                   1. 读取 unified-verification-report.json 中的 native benchmark
                      数据获取 pre-patch ns/op
                   2. 对每个非 stub 方法运行 entry.exe --hotupdate-and-benchmark N
                   3. 计算 slowdown%（interpreter vs native）
                   输出 post-hotupdate-benchmark-report.json
                   **注意：interpreter 路径 slowdown 预期极高（1000x+），这是 interpreter
                   设计的已知特性，不作为失败判定。**
                   [严格模式 required；不阻塞 pipeline]

[7] Aggregate   — 汇总全部 stage 结果
                  计算 coverage（methodCoverage, testedRate）
                  - methodCoverage: fact.details.fact.passed / total（之前 Bug: 读取不存在的 l2）
                  - testedRate: 1 - (skipsFound / total_methods)，下限 max(0, ...)（之前 Bug: 可负）
                  - overall: methodCoverage 和 testedRate 的均值
                  回归检测（baseline_manager.compare_checksum/benchmark）
                  输出 unified-verification-report.json
```

**Standard 模式必过阶段**：preflight, codegen, fact, audit
**Strict 模式额外必过**：hotupdate, post_hotupdate_benchmark

## 输出

| 文件 | 位置 |
|------|------|
| `unified-verification-report.json` | `verification/foundation-dll/<Assembly>/<family>/` |
| `mechanism-audit-report.json` | 同上（审计独立文件） |
| `principle-alignment-report.json` | 同上（原则对齐独立文件） |

### unified-verification-report.json 格式

```json
{
  "family": "convert-char",
  "assembly": "System.Private.CoreLib",
  "mode": "standard",
  "overall_status": "passed" | "failed" | "partial",
  "stages": {
    "preflight":  { "status": "passed",  "summary": "18 methods, 2 custom entries" },
    "codegen":    { "status": "passed",  "summary": "Entrypoint built and IL2CPP compile OK",
                    "details": {"methodCount": 18, "dllPath": "..."} },
    "fact":       { "status": "passed",  "summary": "Fact verify=passed (18/18)",
                    "details": {"fact": {"status": "passed", "passed": 18, "total": 18}} },
    "audit":      { "status": "passed",  "summary": "false_passing=0, principle=CONCERN",
                    "details": {"falsePassing": 0, "skipsFound": 0, "principleStatus": "CONCERN"} },
    "benchmark":  { "status": "passed",  "summary": "avg_speedup=75.3%, native_faster=11/11" },
    "hotupdate":  { "status": "passed",  "summary": "18/18 passed, 0 failed" }
  },
  "coverage": {
    "methodCoverage": 1.0,
    "testedRate": 1.0,
    "overall": 1.0
  },
  "regression": {}
}
```

## Batch 模式

对所有 families 批量跑 codegen（不含 audit/benchmark/hotupdate）：

```bash
python build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py \
  --assembly-name System.Private.CoreLib \
  --families convert-char array-indexing-copy ...
```

批量跑全部 family：
```bash
python build/toolchains/run/testing/foundation_dll/pipeline_native_aot_runner.py \
  --assembly-name System.Private.CoreLib
```

## 失败处理

| 阶段 | 失败模式 | 处理方式 |
|------|---------|----------|
| Preflight | contract 不存在或 methodSubjectIds 为空 | 检查 capability-family-contract.json，完成 onboard 后再试 |
| Codegen | entrypoint build 失败 | 先 `run trace --exception` 查看失败原因，修复后重跑 |
| Codegen | convert-to-cpp 失败 | 同上 |
| Codegen | entry.exe cmake build 失败 | check CMakeLists.txt 路径 + native lib 存在性 |
| Codegen | **LNK2001: kAotMethodCount unresolved** | 检查 `_patch_bypass_0xC0000409.py` 中定义为 `extern "C"`（不是 `static constexpr`）|
| Codegen | **LNK2001: kPatchDataSize unresolved** | 检查 `_generate_patch_data()` 生成的 `runtime-patchdata.cpp` 使用 `extern const` |
| Fact | entry.exe 崩溃或返回非 0 | **阻塞** → 查看 trace → `dev-systematic-debugging` |
| Fact | **输出 1/1 而非真实方法数** | 检查 `kAotMethodCount` 在 `runtime-entry.cpp` 中可见（`extern "C"`） |
| Audit | false-passing > 0 | **阻塞** → 定位 stub/skip → 实现真实机制 → 重新验证 |
| Audit | Principle VIOLATION | **阻塞 closureStatus** → 审查原因 → 修复或登记 waiver |
| Audit | **skipsFound 计数异常（负 skipRate）** | 检查 `mechanism_audit.py` `_get_skip_entries()` 是否按 family 过滤 `_METHOD_OVERRIDES` |
| Benchmark | managed_faster > 0 | 排查翻译质量，标记 regression |
| HotUpdate | failed > 0 | 检查 codegen 输出 → 标记失败 |

### 闸门失败标准流程

任何闸门失败时：

```bash
# 1. 先查 trace
run trace --exception

# 2. 分析根因（使用 dev-systematic-debugging）
# 3. 实现真实机制修复（非新增 skip/特例）
# 4. 修复后重跑
# 5. 确认修复后再继续下一 family
```

### 禁止

- **新增 skip 绕过失败**（违反 No-Skip Policy）
- **修改 _METHOD_OVERRIDES 隐藏问题**
- **不查 trace 直接猜测原因**
- **多次失败后不升级到 dev-systematic-debugging**
- **违反自动化原则检查** — 7 项原则检查全是阻塞门禁

## Handwrite C# 集成

### Partial Class 模式（推荐）

handwrite C# 作为 **partial class** 注入 entrypoint，不干预 auto-generate 流程：

```
verification/foundation-dll/<Assembly>/<family>/
├── handwritten/                          ← 手写源（只读，管线不修改）
│   └── <ClassName>NativeEntry.Custom.cs  ← partial class，包含 CustomEntryMethodN()
├── il2cpp_dist/entrypoint/              ← 管线输出（由 auto-generate + copy 生成）
│   ├── <ClassName>NativeEntry.cs        ← auto-generate 的 partial class
│   ├── <ClassName>NativeEntry.Custom.cs ← 从 handwritten/ 复制的 partial class
│   ├── Program.cs                       ← auto-generate
│   └── <ClassName>NativeEntry.csproj    ← auto-generate，自动包含 Custom.cs
```

**集成流程**（`pipeline_native_aot_runner.py:_build_entrypoint()`）：

1. 检测 `handwritten/` 目录是否存在
2. 如果存在 `.csproj` → 按 **legacy 全项目模式** 处理（复制全部文件到 entrypoint/，直接 build）
3. **如果只有 `.cs` 文件**（如 `Custom.cs`）→ 复制到 `il2cpp_dist/entrypoint/`，然后**回退到 `generate_and_build()`**
4. `generate_and_build()` 自动检测 `Custom.cs` 文件，在 `.csproj` 中加入 `<Compile Include="<Class>.Custom.cs" />`
5. 根据 contract 中的 `customEntryIndices` 为 custom method 生成空桩，由 `Custom.cs` 提供实现

**关键约束**：
- Custom 方法的签名必须与 auto-generated 桩一致：`public static void CustomEntryMethodN()`
- `_exitCode = 1` on failure，`try/catch` for expected exceptions
- `handwritten/` 目录是**只读源**——管线只做 copy，从不写入

### 覆盖保护

管线不会覆盖 `handwritten/` 目录。每次运行 `_build_entrypoint()` 时：

```
handwritten/  → READ ONLY — 管线从目录读取 .cs 文件，从不写入
il2cpp_dist/  → 可覆盖 — 所有 entrypoint/ 下的文件可由 generate_and_build() 重新生成
```

## 参数

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `--assembly` | 目标 assembly | `System.Private.CoreLib` |
| `--mode` | `standard` / `strict` | `standard` |
| `--skip` | 跳过阶段列表 | 空 |
| `--family` | 单 family slug（`verify-family` 子命令） | 必填 |

## 职责边界

| 技能 | 负责 | 不负责 |
|------|------|--------|
| `verification-pipeline` | 编排、参数传递、流程控制、约束定义 | 单阶段执行逻辑 |
| `family-verification` | 原则定义、skip 规则、handwrite 规范 | 执行步骤 |
| `verify-aggregate` | 跨 family 聚合策略、回归检测逻辑 | 单 family 验证执行 |
| `verify-data-integrity` | dashboard 数据一致性检查 | 验证执行 |
| `onboard-family` | 新 family 接入流程 | 已有 family 验证 |
| `verify-analysis` | pipeline 产物的 AI 综合分析 | pipeline 执行、数据生成 |

## AI 综合分析

Pipeline 只产出 raw 验证数据。**分析验收交由 `dev-foundation-dll-verify-analysis` 技能完成。**

调用方式：

```bash
# 直接调用 AI 分析技能 — 对 AI 说：
# "请用 dev-foundation-dll-verify-analysis 技能分析 <family-slug>"
#
# 该技能会自动执行：
#   0. 先自动跑 verify-family pipeline（确保数据最新）
#   1. Handwrite/AutoGen Pre-check
#   2. 收集所有 pipeline 产物、外围文件、dashboard projection
#   3. 按 11 维度标准化模板逐项分析
#   4. 输出分析报告到 verification/reports/<family>-verification-analysis-<YYYYMMDD>.md
#   5. 给出验收建议（approved / review-needed / blocked）

# 人工审核 AI 报告：
#   - 查看 verification/reports/ 下最新分析报告
#   - 根据推荐和建议决定 closureStatus
#   - 确认后刷新 dashboard: run verify verification-v1 --json
```

## 关联

- 入口: `family_verification_orchestrator.py` / `verify_family()`
- Codegen: `pipeline_native_aot_runner.py` / `run_family()`
- Fact: `fact_verifier.py` / `verify_fact()`
- Audit: `mechanism_audit.py` / `run_full_audit()`
- Principle: `principle_auto_checks.py` / `run_all_checks()`
- CLI 路由: `commands/foundation_dll.py` / `_handle_verify_family()`
- 下游: `dev-foundation-dll-verify-analysis`（AI 综合分析）
- 人工审核: 基于分析报告决定 closureStatus