# Foundation DLL 全量优化战役 — Roadmap

> **面向执行 Agent：** 每个子任务独立运行 `dev-optimization-campaign` 全自动流水线。执行 Agent 不需要手动处理 merge/CI/cleanup，campaign 自动完成。
>
> **定位：** 本 roadmap 是父任务文档，不承担具体长期执行；具体实施通过派生子任务运行 campaign 完成。

## 目标

对全部 147 个 foundation DLL capability family 完成：
1. **正确性验证与修复** — 跑通 verification pipeline (strict mode)
2. **Benchmark 优化** — benchmark timing > 0，vs .NET 8 差距 ≤ 20%
3. **HotUpdate 正确性修复** — semantic_changed > 0，overhead ≤ 100%

## 范围边界

- **范围内**：147 个 family（16 个 assembly 的全部已注册 capability family）
- **范围外**：不在 ledger.json 中注册的 family；架构层重构；新增 IL 指令支持

## 非目标

- 不涉及 codegen 新增 IL 指令翻译
- 不涉及架构重构或模块拆分
- 不涉及调整测试框架本身

## 阶段列表

按 **assembly（DLL）** 分组。每个 assembly 是一个 phase，phase 内所有 family 子任务互相独立。

| Phase | Assembly | Families | 总 methods |
|-------|----------|----------|-----------|
| P01 | System.Private.CoreLib | 53 | 672 |
| P02 | System.Runtime.InteropServices | 14 | 396 |
| P03 | System.Text.Json | 13 | 694 |
| P04 | System.Linq | 12 | 228 |
| P05 | System.Collections.Immutable | 10 | 50 |
| P06 | System.Formats.Asn1 | 6 | 146 |
| P07 | System.ObjectModel | 6 | 41 |
| P08 | System.Runtime.Serialization.Formatters | 6 | 95 |
| P09 | System.Security.Principal.Windows | 6 | 96 |
| P10 | System.IO.Pipelines | 5 | 65 |
| P11 | System.IO.Compression.Brotli | 4 | 44 |
| P12 | System.Net.ServerSentEvents | 4 | 21 |
| P13 | System.Threading.Tasks.Parallel | 4 | 58 |
| P14 | System.IO.Compression.ZipFile | 2 | 46 |
| P15 | System.Runtime.Serialization | 1 | 0 |
| P16 | System.Security.Claims | 1 | 37 |

所有 phase 之间**无执行顺序依赖**（方案 B）。可同时派发任意 assembly 的任意 family。

## 子任务映射

### Phase P01: System.Private.CoreLib (53 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F001 | array-copy | 0 | batch-01 | testing/foundation-dll/System.Private.CoreLib |
| F002 | array-indexing-copy | 13 | batch-01 | 同上 |
| F003 | attributes-custom-metadata | 9 | batch-01 | 同上 |
| F004 | boxing-unboxing-casts | 11 | batch-01 | 同上 |
| F005 | buffer-memory | 10 | batch-01 | 同上 |
| F006 | ccw-basic | 5 | batch-01 | 同上 |
| F007 | codegen-edge-cases | 4 | batch-01 | 同上 |
| F008 | collections-generic-core | 16 | batch-01 | 同上 |
| F009 | convert-char | 18 | batch-01 | 同上 |
| F010 | custom-generics | 6 | batch-01 | 同上 |
| F011 | delegate-core-invocation | 10 | batch-01 | 同上 |
| F012 | dispatch-basic | 5 | batch-01 | 同上 |
| F013 | eh-stress | 4 | batch-01 | 同上 |
| F014 | enum-parsing | 13 | batch-01 | 同上 |
| F015 | enumerator-iteration | 0 | batch-01 | 同上 |
| F016 | error-info-basic | 5 | batch-01 | 同上 |
| F017 | exception-throw-diagnostics | 17 | batch-01 | 同上 |
| F018 | garbage-collection | 7 | batch-01 | 同上 |
| F019 | generic-nullable-value | 10 | batch-01 | 同上 |
| F020 | generics-stress | 3 | batch-01 | 同上 |
| F021 | globalization-culture | 13 | batch-01 | 同上 |
| F022 | guid-random-hashcode | 14 | batch-01 | 同上 |
| F023 | interface-dispatch | 9 | batch-01 | 同上 |
| F024 | interface-dispatch,pinvoke-dllimport,primitive-numeric-conversions-core | 23 | batch-01 | 同上 |
| F025 | io-streams-basics | 18 | batch-01 | 同上 |
| F026 | math-numerics | 18 | batch-01 | 同上 |
| F027 | object-equality-identity | 7 | batch-01 | 同上 |
| F028 | pinvoke-dllimport | 4 | batch-01 | 同上 |
| F029 | pinvoke-stress | 4 | batch-01 | 同上 |
| F030 | primitive-numeric-conversions | 15 | batch-01 | 同上 |
| F031 | primitive-numeric-conversions-core | 10 | batch-01 | 同上 |
| F032 | rcw-basic | 7 | batch-01 | 同上 |
| F033 | reflection-activation | 8 | batch-01 | 同上 |
| F034 | reflection-assembly | 22 | batch-01 | 同上 |
| F035 | reflection-binding | 12 | batch-01 | 同上 |
| F036 | reflection-field-property | 34 | batch-01 | 同上 |
| F037 | reflection-generics | 12 | batch-01 | 同上 |
| F038 | reflection-member-basics | 14 | batch-01 | 同上 |
| F039 | reflection-member-complete | 22 | batch-01 | 同上 |
| F040 | reflection-module | 6 | batch-01 | 同上 |
| F041 | reflection-parameters | 10 | batch-01 | 同上 |
| F042 | reflection-type | 43 | batch-01 | 同上 |
| F043 | runtime-compiler-services | 10 | batch-01 | 同上 |
| F044 | snapshot-prover | 10 | batch-01 | 同上 |
| F045 | span-memory-buffers | 14 | batch-01 | 同上 |
| F046 | string-char-text-core | 20 | batch-01 | 同上 |
| F047 | string-construction | 8 | batch-01 | 同上 |
| F048 | threading-monitor-interlocked | 14 | batch-01 | 同上 |
| F049 | threading-sync | 17 | batch-01 | 同上 |
| F050 | threading-tasks-primitives | 15 | batch-01 | 同上 |
| F051 | threading-thread-basics | 11 | batch-01 | 同上 |
| F052 | time-date-time-timespan | 16 | batch-01 | 同上 |
| F053 | type-runtime-handles | 13 | batch-01 | 同上 |
| F054 | value-typebox | 0 | batch-01 | 同上 |

### Phase P02: System.Runtime.InteropServices (14 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F101 | com-types | 22 | batch-02 | testing/foundation-dll/System.Runtime.InteropServices |
| F102 | com-wrappers | 10 | batch-02 | 同上 |
| F103 | custom-marshaller-contracts | 0 | batch-02 | 同上 |
| F104 | exception-errors | 18 | batch-02 | 同上 |
| F105 | function-pointers | 1 | batch-02 | 同上 |
| F106 | generated-marshalling | 69 | batch-02 | 同上 |
| F107 | handles-safehandle-gchandle | 29 | batch-02 | 同上 |
| F108 | marshalling-attributes | 10 | batch-02 | 同上 |
| F109 | native-memory-pointers | 199 | batch-02 | 同上 |
| F110 | objective-c-interop | 4 | batch-02 | 同上 |
| F111 | pinvoke-dllimport | 12 | batch-02 | 同上 |
| F112 | runtime-interop-services | 17 | batch-02 | 同上 |
| F113 | secure-string-marshal | 4 | batch-02 | 同上 |
| F114 | unmanaged-calling-conventions | 1 | batch-02 | 同上 |

### Phase P03: System.Text.Json (13 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F201 | attributes | 23 | batch-03 | testing/foundation-dll/System.Text.Json |
| F202 | converter-factory | 2 | batch-03 | 同上 |
| F203 | converters | 9 | batch-03 | 同上 |
| F204 | document-element | 69 | batch-03 | 同上 |
| F205 | metadata-typeinfo | 39 | batch-03 | 同上 |
| F206 | nodes | 174 | batch-03 | 同上 |
| F207 | polymorphism-reference | 8 | batch-03 | 同上 |
| F208 | reader | 61 | batch-03 | 同上 |
| F209 | schema | 2 | batch-03 | 同上 |
| F210 | serializer | 104 | batch-03 | 同上 |
| F211 | serializer-options | 75 | batch-03 | 同上 |
| F212 | source-generation-context | 2 | batch-03 | 同上 |
| F213 | writer | 126 | batch-03 | 同上 |

### Phase P04: System.Linq (12 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F301 | aggregation | 11 | batch-04 | testing/foundation-dll/System.Linq |
| F302 | filtering | 9 | batch-04 | 同上 |
| F303 | generation-concat | 10 | batch-04 | 同上 |
| F304 | grouping-lookup | 8 | batch-04 | 同上 |
| F305 | joins | 8 | batch-04 | 同上 |
| F306 | materialization | 17 | batch-04 | 同上 |
| F307 | numeric-aggregation | 90 | batch-04 | 同上 |
| F308 | ordering | 14 | batch-04 | 同上 |
| F309 | partitioning | 10 | batch-04 | 同上 |
| F310 | projection | 9 | batch-04 | 同上 |
| F311 | set-operations | 18 | batch-04 | 同上 |
| F312 | shuffle-index | 24 | batch-04 | 同上 |

### Phase P05: System.Collections.Immutable (10 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F401 | extensions-marshal | 4 | batch-05 | testing/foundation-dll/System.Collections.Immutable |
| F402 | frozen-collections | 8 | batch-05 | 同上 |
| F403 | immutable-array | 6 | batch-05 | 同上 |
| F404 | immutable-dictionary | 3 | batch-05 | 同上 |
| F405 | immutable-hash-set | 5 | batch-05 | 同上 |
| F406 | immutable-interlocked | 1 | batch-05 | 同上 |
| F407 | immutable-list | 6 | batch-05 | 同上 |
| F408 | immutable-queue-stack | 10 | batch-05 | 同上 |
| F409 | immutable-sorted-dictionary | 2 | batch-05 | 同上 |
| F410 | immutable-sorted-set | 5 | batch-05 | 同上 |

### Phase P06: System.Formats.Asn1 (6 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F501 | decoder | 33 | batch-06 | testing/foundation-dll/System.Formats.Asn1 |
| F502 | encoding-rules | 5 | batch-06 | 同上 |
| F503 | options-errors | 7 | batch-06 | 同上 |
| F504 | reader | 40 | batch-06 | 同上 |
| F505 | tag-model | 19 | batch-06 | 同上 |
| F506 | writer | 42 | batch-06 | 同上 |

### Phase P07: System.ObjectModel (6 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F601 | collection-change-notifications | 2 | batch-07 | testing/foundation-dll/System.ObjectModel |
| F602 | command-type-metadata | 15 | batch-07 | 同上 |
| F603 | data-errors | 6 | batch-07 | 同上 |
| F604 | object-model-collections | 6 | batch-07 | 同上 |
| F605 | observable-collections | 8 | batch-07 | 同上 |
| F606 | property-change-notifications | 4 | batch-07 | 同上 |

### Phase P08: System.Runtime.Serialization.Formatters (6 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F701 | binary-formatter | 16 | batch-08 | testing/foundation-dll/System.Runtime.Serialization.Formatters |
| F702 | binder-type-style | 2 | batch-08 | 同上 |
| F703 | formatter-base | 16 | batch-08 | 同上 |
| F704 | formatter-services-converter | 34 | batch-08 | 同上 |
| F705 | object-manager-id-generator | 16 | batch-08 | 同上 |
| F706 | surrogate-selection | 11 | batch-08 | 同上 |

### Phase P09: System.Security.Principal.Windows (6 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F801 | identity-reference | 23 | batch-09 | testing/foundation-dll/System.Security.Principal.Windows |
| F802 | nt-account | 10 | batch-09 | 同上 |
| F803 | security-identifier | 20 | batch-09 | 同上 |
| F804 | token-access | 4 | batch-09 | 同上 |
| F805 | windows-identity | 31 | batch-09 | 同上 |
| F806 | windows-principal-roles | 8 | batch-09 | 同上 |

### Phase P10: System.IO.Pipelines (5 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| F901 | options-scheduler | 23 | batch-10 | testing/foundation-dll/System.IO.Pipelines |
| F902 | pipe-core | 7 | batch-10 | 同上 |
| F903 | read-flush-results | 7 | batch-10 | 同上 |
| F904 | reader-writer | 27 | batch-10 | 同上 |
| F905 | stream-adapters | 1 | batch-10 | 同上 |

### Phase P11: System.IO.Compression.Brotli (4 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| FA01 | decoder | 3 | batch-11 | testing/foundation-dll/System.IO.Compression.Brotli |
| FA02 | encoder | 7 | batch-11 | 同上 |
| FA03 | options | 3 | batch-11 | 同上 |
| FA04 | stream | 31 | batch-11 | 同上 |

### Phase P12: System.Net.ServerSentEvents (4 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| FB01 | formatter | 2 | batch-12 | testing/foundation-dll/System.Net.ServerSentEvents |
| FB02 | item-parser | 4 | batch-12 | 同上 |
| FB03 | parser | 8 | batch-12 | 同上 |
| FB04 | sse-item | 7 | batch-12 | 同上 |

### Phase P13: System.Threading.Tasks.Parallel (4 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| FC01 | loop-result | 2 | batch-13 | testing/foundation-dll/System.Threading.Tasks.Parallel |
| FC02 | loop-state | 6 | batch-13 | 同上 |
| FC03 | parallel-apis | 43 | batch-13 | 同上 |
| FC04 | parallel-options | 7 | batch-13 | 同上 |

### Phase P14: System.IO.Compression.ZipFile (2 families)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| FD01 | zip-file-apis | 34 | batch-14 | testing/foundation-dll/System.IO.Compression.ZipFile |
| FD02 | zip-file-extensions | 12 | batch-14 | 同上 |

### Phase P15: System.Runtime.Serialization (1 family)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| FE01 | surrogates-selection | 0 | batch-15 | testing/foundation-dll/System.Runtime.Serialization |

### Phase P16: System.Security.Claims (1 family)

| task_id | slug | methods | batch | conflict_scope |
|---------|------|---------|-------|---------------|
| FF01 | windows-identity | 37 | batch-16 | testing/foundation-dll/System.Security.Claims |

## 子任务状态总表

当前全部子任务状态为 `planned`。分配给终端后标记为 `in-progress`，完成/阻塞后更新。

## 依赖

- **子任务间依赖**：无（方案 B — 完全独立并行）
- **外部依赖**：`dev-optimization-campaign` 技能正常工作；pipeline 脚本可用
- **共享资源冲突**：同一 assembly 的 family 共享 `testing/foundation-dll/<assembly>/` 文件，但 campaign 的 merge 步骤处理了冲突策略（合约文件取 main 版本）

## 风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|---------|
| 多个 campaign 修改同一 codegen 文件后 merge 冲突 | 中 | 低 | campaign Step 11 有分层冲突解决策略 |
| CI 验证在 merge 后失败 | 低 | 中 | campaign Step 12 自动检测 CI 状态 |
| 某 family 三轮修复仍不过 | 低 | 低 | 写 blocker.md，跳过，不阻塞其他 family |
| SPCoreLib 的 codegen 修复改变其他 family 的验证基线 | 高 | 低 | 方案B 下各自独立验证，codegen 修复 merge 后自然同步到 main |

## 备选路径

- 如某 assembly 的所有 family 都频繁产生相同 codegen 修复 → 可先在该 assembly 中跑一个代表性 family，修复 codegen 后，再并行跑其他 family
- 如果 campaign 流水线长时间阻塞 → 可手动进入 worktree 排查

## 当前建议推进顺序

由于方案 B 完全独立，**无强制顺序**。建议：

1. 先挑一些已验证过的 family（`lastVerified` 不为空）作为热身，熟悉 campaign 流程
2. 逐步推进到未验证的 family
3. System.Private.CoreLib（53 families）体量最大，建议持续穿插执行

## 子任务执行策略

- **`child_execution_mode`: auto** — 子任务创建后可自动启动
- **`auto_continue`: true** — 完成后自动推进到下一可用子任务
- **`auto_stop_policy`: blocking-only** — 仅在遇到 blocker 时停止
- **入口**: 每个子任务直接调用 `dev-optimization-campaign` 技能，以 slug 为参数
- **不需要写 plan** — campaign 技能自带完整流水线

## 调度策略

本 roadmap 使用 `hybrid` 调度模式：

1. **派发方式**：用户手动选择 `<slug>` 分派到终端
2. **终端执行**：`dev-optimization-campaign <slug>` 全自动
3. **批量说明**：无固定 batch 约束，可按任意顺序或组合派发
4. **合并**：每个 campaign 独立 merge 到 main

## 完成定义（子任务级别）

每个 family 子任务的完成定义：

- [x] Step 2-4: Pipeline 全部 stage passed（或 blocker.md 记录）
- [x] Step 5: benchmark timing > 0
- [x] Step 6: vs .NET 8 ≤ 20%（AOT + JIT，或 blocker.md 记录）
- [x] Step 7: hotupdate semantic_changed > 0 + overhead ≤ 100%（或 blocker.md 记录）
- [x] Step 8: `docs/optimize/YYYY-MM-DD-<slug>/README.md` 完整
- [x] Step 9-10: Commit + Push 到 claim 分支
- [x] Step 11: Merge 到 main + 清理 worktree
- [x] Step 12: CI 验证通过
- [x] Step 13: main 分支干净已同步

## 父任务完成定义

所有 147 个子任务都进入终态（`completed` / `blocked` / `abandoned`）后，父任务完成。
