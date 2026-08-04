---
parent_task_id: 20260530-optimization-campaign-reboot
source_task_id: T-A2
lifecycle_status: completed
phase: completed
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
---

# T-A2: Batch Pipeline 执行

## Scope
对 38 个无 benchmark 数据的 family（scanner 输出 40，剔除 `System.Private.CoreLib` 和 `reports`/`results` 目录）跑 `--stages benchmark --native-config profile`，填补数据空白。

## 执行摘要
- **总处理**: 38 families
- **Benchmark 成功**: 28 (73.7%) — 229 个方法成功运行 benchmark
- **编译失败**: 10 (26.3%)
- **退出标准达成**: >= 50% 成功 (28/38 = 73.7%), 失败的 family 已记录原因

### 注意
Pipeline 运行成功生成 `unified-verification-report.json`（含 benchmark 数据），但 `multi-run/multi-run-report.json` 由独立后处理步骤生成，非 pipeline 内建产出。Scanner 当前仅读取 `multi-run/multi-run-report.json`，若需 scanner 反映新数据需更新 scanner 逻辑。

## Benchmark 成功的 Family (28)

### reflection-* 家族 (10)
| # | Family | Methods | Duration |
|---|--------|---------|----------|
| 1 | reflection-assembly | 21/21 | 8.9s |
| 2 | reflection-customattr | 3/3 | 6.9s |
| 3 | reflection-edge | 8/8 | 11m |
| 4 | reflection-event | 3/3 | 1m26s |
| 5 | reflection-field-property | 34/34 | 38.7s |
| 6 | reflection-generics | 8/8 | 4.3s |
| 7 | reflection-hotupdate | 3/4 | 1m25s |
| 8 | reflection-member-complete | 7/7 | 2m5s |
| 9 | reflection-module | 5/5 | 31.5s |
| 10 | reflection-parameters | 1/1 | 34.2s |

### 其他家族 (18)
| # | Family | Methods | Duration |
|---|--------|---------|----------|
| 11 | array-copy | 2/2 | 34.5s |
| 12 | boxing-unboxing-casts | 8/8 | 4.2s |
| 13 | enumerator-iteration | 5/5 | 38.5s |
| 14 | exception-handling | 6/6 | 37.6s |
| 15 | generic-supplement | 4/4 | 21.2s |
| 16 | io-streams-basics | 18/18 | 11m46s |
| 17 | numerics-bitops | 6/8 | 46.2s |
| 18 | numerics-floating-point | 8/8 | 2m27s |
| 19 | pinvoke-stress | 2/3 | 9m45s |
| 20 | primitive-numeric-conversions-core | 10/10 | 44.3s |
| 21 | snapshot-prover | 10/10 | 46.4s |
| 22 | span-memory-buffers | 14/14 | 5m44s |
| 23 | string-construction | 7/7 | 2m38s |
| 24 | text-ebcdic | 5/5 | 2m44s |
| 25 | thread-sync | 4/4 | 4.8s |
| 26 | threading-tasks-primitives | 13/13 | 13.0s |
| 27 | type-runtime-handles | 10/10 | 45.8s |
| 28 | value-typebox | 4/5 | 4m45s |

## 编译失败的 Family (10)

| # | Family | Duration | 失败原因 |
|---|--------|----------|----------|
| 1 | reflection-activation | 30.6s | C2737/C2027/C2618: MethodInfo/ConstructorInfo 类型未完整定义 |
| 2 | reflection-binding | 30.3s | 同上 |
| 3 | reflection-member-basics | 31.7s | 同上 |
| 4 | multi-patch | 23.5s | 无原生可执行文件 |
| 5 | pinvoke-dllimport | 45.0s | P/Invoke 代码生成编译失败 |
| 6 | rcw-basic | 46.0s | COM Interop 代码生成编译失败 |
| 7 | runtime-compiler-services | 12.1s | C1083: 头文件未找到 |
| 8 | threading-sync | 12.4s | C1083: 头文件未找到 |
| 9 | solution-core-pack | 16.5s | 代码生成编译失败 |
| 10 | interface-dispatch,pinvoke-dllimport,... | 2m18s | C1083: 多 family 联合编译失败 |

## 根因分析

### 编译失败模式 (3 类)

**A 类 — 反射类型未定义 (3 families)**
`reflection-activation`, `reflection-binding`, `reflection-member-basics`:
codegen 输出的 `native-aot.generated.cpp` 中使用了 `offsetof(chaos_type_System_Private_CoreLib_System_Reflection_MethodInfo, ...)`，但该类型仅在前向声明中出现，未生成完整 struct 定义。导致 MSVC 报 C2027 (incomplete type) 和 C2618 (illegal offsetof) 错误。涉及的类型包括 `MethodInfo`, `ConstructorInfo`, `FieldInfo`, `PropertyInfo` 等反射核心类型。

**B 类 — P/Invoke/COM Interop 基础设施缺失 (3 families)**
`pinvoke-dllimport`, `rcw-basic`, `multi-patch`: P/Invoke 和 COM 互操作所需的 native 层基础设施未完整实现，导致 codegen 无法编译入口程序。

**C 类 — header/include 依赖缺失 (4 families)**
`runtime-compiler-services`, `threading-sync`, `solution-core-pack`, `interface-dispatch,primitive-numeric-conversions-core`: 编译器报 C1083 无法打开头文件，说明这些 family 的 codegen 输出存在 include 依赖未满足。

## 输出文件
- `testing/foundation-dll/batch-pipeline-run.log` — 完整执行日志
- 各 family 目录下 `unified-verification-report.json` — 含 benchmark 数据
