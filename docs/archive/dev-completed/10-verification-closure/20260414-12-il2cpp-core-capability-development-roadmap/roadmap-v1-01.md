# IL2CPP 核心能力补齐与 Subject 完整验证 Roadmap v1.01

Date: 2026-04-14 12:41:36 +08:00
Status: completed

## 1. 目标

在 `20260414-04` 已完成的 capability/archetype/retained-subject 基线上，继续推进到新的长期目标：

- 以两级 capability taxonomy 作为 IL2CPP、热更与 mixed execution 核心能力的统一真源。
- 让 retained subjects 成为能力 owner、工程级 archetype、proof、benchmark 与 support-state 的统一闭环载体。
- 把“subject 工程有完整验证能力”落成稳定的工程契约，而不是停留在口头标准。
- 为后续多个阶段子任务提供稳定的派生顺序、边界、验收标准和风险控制。

## 2. 范围边界

本 roadmap 负责：

- 冻结 capability coverage ledger 与 owner mapping。
- 补齐 retained subjects 的完整验证闭环。
- 按批次推进 IL2CPP 核心能力、低层运行时、metadata/AOT、full-solution、mixed/hot-update/productization 的后续扩展。
- 把 proof、benchmark、support-state、dashboard、reporting、docs 继续统一到同一证据链中。

本 roadmap 不负责：

- 新增第四个顶层 subject。
- 回退到大量 string 协议驱动的 subject 内部调度。
- 把每个历史语法点都机械变成独立 benchmark 工程。
- 在没有 Phase 0 coverage ledger 的情况下直接分散补功能。

## 3. 非目标

- 不重建旧的 `validation/`、`scenario/`、`workload/` 多目录定义模式。
- 不把 `Chaos.TestFramework` 扩张成新的 DSL 或产品配置层。
- 不把 support-state 与平台差异重新硬编码回 C# attribute。
- 不以“单条 benchmark 数字好看”替代能力闭环本身。

## 4. 阶段列表

### Phase 0: Coverage Ledger And Subject Verification Freeze

- 冻结 `capability_item -> owner_subject -> owner_archetype -> evidence` 台账。
- 冻结 repository-owned reference bundle 的边界、刷新规则和 archetype 消费方式。
- 冻结“完整验证能力”的统一 checklist 与支持状态出口。

### Phase 1: Subject Verification Closure

- 补齐 retained subjects 的验证闭环缺口。
- 清理 benchmark identity、dashboard 视图、support-state 输出与文档锚点的残余漂移。
- 让 retained subjects 达到“新增能力点可稳定接入”的状态。

### Phase 2: IL2CPP Core Semantics Batch A

- 优先补齐核心运行时语义能力：
  - `TaskAndValueTaskFlow`
  - `IteratorStateMachine`
  - `FinallyAndFilter`
  - `TaskScheduling`
  - `MonitorAndLocking`
  - `ThreadLocalState`
  - `RuntimeHelpers`
  - `GcSensitiveFlow`
  - `ResourceLifecycle`
  - `LoaderContract`

### Phase 3: Low-Level And Interop Batch B

- 补齐低层 runtime 与 interop 能力：
  - `SpanAndMemory`
  - `RefStructAndStackalloc`
  - `UnsafePointer`
  - `FunctionPointer`
  - `NativeCallInterop`
  - `StringAndUtf8Marshaling`
  - `StructMarshaling`
  - `DelegateCallbackInterop`

### Phase 4: Metadata And AOT Closure Batch C

- 补齐 metadata、linker、generic/AOT 闭包能力：
  - `CustomAttributeLookup`
  - `MemberMetadataLookup`
  - `MetadataClosure`
  - `LinkerPreserveContract`
  - `LinkerStripping`
  - `RequiredInstantiationClosure`
  - `GenericSharingBoundary`
  - `AotClosureValidation`

### Phase 5: Full-Solution And Productization Closure

- 把 canonical solution archetype 全部拉到真正可发布的工程级门禁：
  - `SimpleLibrarySolution`
  - `MultiProjectSolution`
  - `PackageReferenceSolution`
  - `ReferenceAssemblySolution`
  - `CoreLibReferenceSolution`
  - `MixedReferenceClosureSolution`
  - `FullProjectHotUpdateSolution`

### Phase 6: Mixed Execution, HotUpdate And Release Gates

- 完成 mixed execution 与 hot-update 的发布级闭环：
  - compatibility / rollback / supplement / diagnostics
  - mixed managed/native/interpreter 对照
  - release-gate threshold 与 support-state 证据

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 在真正继续补功能前，冻结能力台账、验证闭环 checklist 与 reference bundle 边界。
- `exit_criteria`
  - 每个二级 capability item 都能回答：
    - owner subject 是谁
    - owner archetype 是谁
    - 需要哪类 proof
    - 是否需要 benchmark
    - support-state 证据放在哪里
  - `CoreLibReferenceSolution`、`ReferenceAssemblySolution`、`ReferenceAssemblyPatchSolution` 的 bundle 策略固定。
- `deliverables`
  - coverage ledger freeze
  - subject verification checklist
  - reference bundle policy note
- `dependencies`
  - 无
- `open_questions`
  - benchmark 阈值是按 subject 维度还是按 capability family 维度治理，Phase 0 需要冻结。

### Phase 1

- `goal`
  - 让 retained subjects 自身具备完整、稳定、可扩展的验证闭环。
- `exit_criteria`
  - 新增 proof/benchmark 不需要再为外层系统补额外兼容壳。
  - benchmark/dashboard 使用 canonical identity，不再混淆历史 case 名称。
  - support-state / reason code 能稳定显示到 subject/archetype/capability 维度。
- `deliverables`
  - subject completeness audit
  - dashboard / reporting identity cleanup
  - verification closure docs
- `dependencies`
  - Phase 0
- `open_questions`
  - diagnostics artifact 的最小集合是否包括 trace/symbol/metadata supplement 摘要，Phase 1 需要冻结。

### Phase 2

- `goal`
  - 补齐 IL2CPP 核心运行时语义的第一批高价值能力点。
- `exit_criteria`
  - Batch A 的每个 capability item 都有 owner、proof、必要 benchmark 与 solution 级映射。
  - 与这些能力相关的 support-state 和已知边界已进入外层矩阵。
- `deliverables`
  - `SolutionCorePack` 新增或扩展的 feature slices
  - 对应 proof/benchmark entries
  - capability ledger 更新
- `dependencies`
  - Phase 1
- `open_questions`
  - async / threading 类 benchmark 的 host 噪声容忍度阈值需要单独定义。

### Phase 3

- `goal`
  - 补齐低层 runtime 与 interop 能力，使 IL2CPP 在真实项目常见 ABI/unsafe 路径上有稳定证据。
- `exit_criteria`
  - Batch B 的每个 capability item 都有正确性证据与工程级 archetype 映射。
  - interop 相关 benchmark 能稳定基于 managed baseline 展示差异。
- `deliverables`
  - interop / low-level proof slices
  - selective benchmark slices
  - ABI 边界文档更新
- `dependencies`
  - Phase 1
- `open_questions`
  - 是否需要为 native callback 与 marshaling 补独立的诊断工件摘要。

### Phase 4

- `goal`
  - 关闭 metadata、generic、AOT、linker 相关闭包能力的主要缺口。
- `exit_criteria`
  - Batch C 每个 capability item 都能在 native closure、solution archetype 与 docs 中找到明确证据。
  - `CoreLibReferenceSolution` 等关键 archetype 对应的 reference bundle 证据稳定。
- `deliverables`
  - metadata / AOT capability proofs
  - closure validation entries
  - docs 与 matrix 更新
- `dependencies`
  - Phase 1
- `open_questions`
  - foundation bundle 的刷新节奏与校验策略是否需要单独脚本任务。

### Phase 5

- `goal`
  - 让 canonical solution archetype 全部成为真正的工程级 gate。
- `exit_criteria`
  - 每个 archetype 都至少有一条“完整 ingest -> translate -> run/report”链路。
  - full-solution 验证可以稳定覆盖 project/reference/package/corelib/hot-update 关键场景。
- `deliverables`
  - archetype gate checklist
  - productization-oriented docs
  - full-solution evidence refresh
- `dependencies`
  - Phase 2, Phase 3, Phase 4
- `open_questions`
  - 哪些 archetype 需要成为默认 CI gate，哪些保持按需验证。

### Phase 6

- `goal`
  - 把 mixed execution、hot-update 与 release gates 拉到发布级闭环。
- `exit_criteria`
  - compatibility / rollback / bridge / supplement / mixed benchmark 都有统一外层证据。
  - 能明确输出 supported/partial/unsupported 的最终边界和 reason code。
- `deliverables`
  - release-gate checklist
  - mixed/hot-update evidence report
  - archived follow-up updates
- `dependencies`
  - Phase 5
- `open_questions`
  - 是否需要把部分 mixed/hot-update gates 继续下钻到 host-platform matrix。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze` | `Phase 0` | `completed` | `codex` | 冻结 capability 台账、完整验证 checklist 与 reference bundle 策略 | `-` |
| `20260414-14-phase-1-subject-verification-closure` | `Phase 1` | `completed` | `codex` | 收口 retained subjects 的 verification / dashboard / support-state 闭环 | `20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze` |
| `20260414-15-phase-2-il2cpp-core-semantics-batch-a` | `Phase 2` | `completed` | `codex` | 补齐 Batch A 核心运行时语义能力 | `20260414-14-phase-1-subject-verification-closure` |
| `20260414-16-phase-3-low-level-and-interop-batch-b` | `Phase 3` | `completed` | `codex` | 补齐 Batch B 低层 runtime 与 interop 能力 | `20260414-14-phase-1-subject-verification-closure` |
| `20260414-17-phase-4-metadata-and-aot-closure-batch-c` | `Phase 4` | `completed` | `codex` | 补齐 Batch C metadata / linker / generic / AOT 闭包能力 | `20260414-14-phase-1-subject-verification-closure` |
| `20260414-18-phase-5-full-solution-and-productization-closure` | `Phase 5` | `completed` | `codex` | 把 canonical solution archetype 拉到工程级与产品级 gate | `20260414-15-phase-2-il2cpp-core-semantics-batch-a, 20260414-16-phase-3-low-level-and-interop-batch-b, 20260414-17-phase-4-metadata-and-aot-closure-batch-c` |
| `20260414-19-phase-6-mixed-execution-hotupdate-and-release-gates` | `Phase 6` | `completed` | `codex` | 关闭 mixed execution、hot-update 与 release gate 的发布级闭环 | `20260414-18-phase-5-full-solution-and-productization-closure` |

## 7. 依赖

- Phase 0 是整个 follow-up 的冻结入口，没有这一步，后续子任务会再次出现 owner 漂移。
- Phase 1 是技术批次之前的必要闭环，因为后续所有能力点都要依赖稳定的验证入口和外层展示。
- Phase 2、3、4 在 Phase 1 后可以分批推进，但建议先做 Phase 2，以尽快补齐最核心运行时语义。
- Phase 5 依赖前三个技术批次的主要能力收口，否则 full-solution gate 只是表面闭环。
- Phase 6 最后执行，用来把 mixed/hot-update/release 维度全部统一到最终门禁。

## 8. 风险

### 风险 1：Roadmap 再次退化成“语法点打表”

- 缓解：
  - Phase 0 必须冻结 capability ledger。
  - 新 child task 必须按 capability item 和 owner archetype 组织，而不是按零散语法点命名。

### 风险 2：`SolutionCorePack` 再次膨胀为无边界容器

- 缓解：
  - 强制使用 `owner subject + owner archetype + feature slice` 三层映射。
  - 机械拆分只发生在单文件条目层，不扩张顶层 subject。

### 风险 3：Benchmark 与外层显示再次偏离真实能力状态

- 缓解：
  - Phase 1 先收口 identity、managed baseline、support-state 与 docs 锚点。
  - benchmark 继续只服务高价值 runtime path。

### 风险 4：reference bundle 漂移导致 full-solution 证据不稳定

- 缓解：
  - Phase 0 冻结 bundle 边界和刷新策略。
  - Phase 4 和 Phase 5 复核 archetype 对 bundle 的稳定消费。

## 9. 备选路径

### 备选路径 A：只做语法点补齐，不做 subject verification 闭环

- 优点：
  - 短期见效快。
- 缺点：
  - 迟早再次回到“代码有了，入口、报告、文档不可信”的状态。

### 备选路径 B：只做 full-solution archetype，不单独补 capability ledger

- 优点：
  - 更贴近真实项目视角。
- 缺点：
  - 难以定位运行时语义缺口，后续维护成本高。

### 当前判断

- 两条备选路径都不如当前主路线稳定。
- 推荐保持 capability-led 混合路线。

## 10. 当前建议推进顺序

1. 先执行 `20260414-13-phase-0-coverage-ledger-and-subject-verification-freeze`，把 capability 台账、完整验证 checklist、reference bundle 策略正式冻结。
2. 然后执行 `20260414-14-phase-1-subject-verification-closure`，把 retained subjects 的外层闭环完全收口。
3. 之后分批推进：
   - `Phase 2`
   - `Phase 3`
   - `Phase 4`
4. 技术批次有结果后，再进入 `Phase 5` 的 full-solution/productization gate。
5. 最后执行 `Phase 6`，形成 mixed execution、hot-update 与 release gates 的统一发布级闭环。

## 11. 里程碑

- `M1`
  - Phase 0 + Phase 1 完成
  - 结果：subject 验证闭环与能力台账稳定，可安全承接大规模能力补齐
- `M2`
  - Phase 2 + Phase 3 + Phase 4 完成
  - 结果：IL2CPP 核心语义、低层 runtime、metadata/AOT 的主缺口被系统性补齐
- `M3`
  - Phase 5 + Phase 6 完成
  - 结果：full-solution、mixed execution、hot-update 与 release-gate 形成统一闭环
