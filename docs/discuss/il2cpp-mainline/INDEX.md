# il2cpp-mainline

## 主题说明

- 这一组文档专门承接 IL2CPP 主线的长期讨论。
- 先有总骨架，再围绕阶段边界、优先级、平台策略和输入兼容逐层细化。

## 文档

- `20260407-01-il2cpp-mainline-roadmap-skeleton-v1-01.md`
  - IL2CPP 主线的第一版 roadmap 骨架。
- `20260407-02-phase-a-phase-b-boundary-v1-01.md`
  - 细化 `Phase A` 核心层稳定化与 `Phase B` `.NET 8` 核心语义扩张之间的边界。
- `20260407-03-phase-a-semantic-world-responsibility-v1-01.md`
  - 细化 `Phase A` 中 `SemanticWorld` 应承担的职责、边界与推荐方案。
- `20260407-04-phase-a-linker-codegen-emitter-capability-boundary-v1-01.md`
  - 细化 `Phase A` 中 `Linker / CodeGen / NativeReferenceProofEmitter` 的 capability 与 lowering 边界，并给出 3 方案判断。
- `20260407-05-phase-a-lowering-family-vocabulary-v1-01.md`
  - 细化 `Phase A` 中 capability hints 与 lowering family 的 vocabulary，给出 3 方案判断与首轮命名骨架。
- `20260407-06-phase-a-lowering-plan-role-contract-v1-01.md`
  - 细化 `Phase A` 中 lowering plan 应冻结成怎样的 role contract，给出 3 方案判断与推荐骨架。
- `20260407-07-phase-a-dependency-policy-contract-v1-01.md`
  - 按五段式结构细化 `Phase A` 的 dependency policy contract，明确 managed external dependency 与 runtime/host binding 的边界。
- `20260407-08-phase-a-emitter-template-boundary-v1-01.md`
  - 按五段式结构细化 `Phase A` 的 emitter/template 边界，明确 emitter 的正式输入、允许的 validation 白名单与 `TemplateKind` 的升级闸门。
- `20260407-09-phase-a-emitter-boundary-step-by-step-implementation-v1-01.md`
  - 按五段式结构把 emitter boundary 的推荐方案展开成具体可执行的 7 步实现路径。
- `20260407-10-phase-a-engine-api-and-host-binding-boundary-v1-01.md`
  - 按五段式结构细化自研引擎嵌入、引擎向 C# 暴露 API 与 `engine binding / runtime / host` 三层边界的推荐方案。
- `20260407-11-phase-a-engine-binding-lowering-plan-contract-v1-01.md`
  - 按五段式结构细化 `engine binding` 在 `LoweringPlan` 中的正式落点，推荐增量引入 `EngineBindings` 与 `HostBindings` 两个结构化层。
- `20260407-12-phase-a-engine-proof-subject-matrix-and-artifact-chain-v1-01.md`
  - 按五段式结构细化首轮 engine proof 的 canonical subjects、首批 Windows matrix 与继续复用现有 artifact-chain 的推荐路径。
- `20260407-13-phase-a-engine-subject-manifest-and-execution-pipeline-draft-v1-01.md`
  - 按五段式结构细化三个 engine subjects 的 `subject.manifest.json` 草案与共享 `executionPipelines` 的推荐结构。
- `20260407-14-phase-a-engine-artifact-evidence-and-report-contract-v1-01.md`
  - 按五段式结构细化 engine proof 在 `analysis/generated/runtime.manifest.json` 与 `report.json` 中的证据落点、局部 report 复用策略与推荐 contract。
- `20260407-15-phase-a-engine-summary-consumer-boundary-v1-01.md`
  - 按五段式结构细化 `engineProofProfile / engine*Summary / engineProofSummary` 的主消费者、禁止消费边界与推荐 public surface 入口。
- `20260408-01-phase-a-layered-core-pipeline-concrete-responsibilities-v1-01.md`
  - 用两个最小 C# 例子把 `Loader / SemanticWorld / Linker / CodeGen / Emitter` 的推荐分层职责具体走一遍，并明确各层的非职责边界。
- `20260408-02-phase-a-linker-output-contract-v1-01.md`
  - 细化 `Linker` 在 B 方案下的最小输出 contract，收敛为 retained closure + external managed dependencies + declarative reasons，而不侵入 lowering/runtime/host 层。
- `20260408-03-phase-a-reason-taxonomy-granularity-v1-01.md`
  - 比较纯语义层 reason 与双层 reason 的架构差异，明确方案 3 的增量价值主要在 semantic-to-implementation traceability，而不是新的核心编译能力。
- `20260408-04-phase-a-semantic-reason-vocabulary-v1-01.md`
  - 在已拍板的方案 2 下冻结 `RetainedReasons / DependencyReasons` 的最小 semantic vocabulary，收敛为共享命名空间 + `structural/semantic` 两类 family，并明确禁止 lowering 泄漏命名。
- `20260408-05-phase-a-emission-order-contract-v1-01.md`
  - 细化 `LinkedWorld` 中 `EmissionOrder` 的正式 contract，收敛为 entry/type/member/dependency 四个 semantic surface 的稳定遍历顺序，并明确排除 codegen/template/render phase 语义。
- `20260408-06-phase-a-linker-diagnostics-contract-v1-01.md`
  - 细化 `LinkedWorld` 中 `Diagnostics` 的正式 contract，收敛为 entry/closure/dependency/order-invariant 四类 linker-scope 结构化诊断，并明确区分下游可消费性边界。
