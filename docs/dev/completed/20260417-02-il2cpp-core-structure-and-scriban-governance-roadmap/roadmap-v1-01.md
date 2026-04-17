# IL2CPP Core Structure And Scriban Governance Roadmap v1.01

Date: 2026-04-17 13:20:00 +08:00
Status: in-progress

## 1. 目标

把 `IL2CPP` 核心代码从“可工作但结构失衡”的状态，推进到“可持续扩展、模板治理明确、职责边界稳定”的状态。重点不在单点修 bug，而在建立长期可执行的结构治理基线：

- `Chaos.IL2CPP.CodeGen` 内部分层清晰。
- `NativeAOT` 的 planner / runtime support / emitter / template 明确分层。
- `Scriban` 成为核心 codegen 层的标准组成部分。
- 超大 stage 文件逐步拆分，文件尺寸恢复健康。
- 长期规范写入 wiki，后续 feature 接入可直接按规范验收。

## 2. 范围边界

本 roadmap 负责：

- `src/managed/Chaos.IL2CPP.CodeGen` 的结构治理。
- `NativeAot` 与 `NativeReference` 的 planner/emitter/template 职责收口。
- `Loader/Linker/Contracts/Driver/SemanticWorld/EngineBinding` 等热点项目的文件拆分策略。
- `Scriban` 在核心代码层的定位、扩展规则与治理要求。
- 最终的长期 wiki 规范沉淀。

本 roadmap 不负责：

- 在父 roadmap 中直接完成所有重构实现。
- 把 `Chaos.TestFramework` 和 `AOT Core IR` 混合设计。
- 新增一轮完整的 subject/test 主线重构。
- 第一阶段直接把 `CodeGen` 拆成多个新的 managed assembly。

## 3. 非目标

- 不以“继续堆更多 planner 逻辑到超大文件中”为代价扩 feature。
- 不把 `Scriban` 当成临时模板皮肤。
- 不因为执行方便就扩大新的 string 协议面。
- 不在 wiki 中记录临时执行流水。

## 4. 阶段列表

### Phase 0: Source Tree Cleanup And Boundary Freeze

- goal:
  - 冻结核心结构规则、目录边界、模板治理规则。
  - 清点并隔离会干扰结构判断的源码树噪音。
- exit_criteria:
  - 目标分层、文件尺寸阈值、模板治理规则已文档化。
  - 已识别并列出需要忽略或清理的 `bin/obj/artifacts/.tmp-*` 噪音路径。
  - 父任务与 wiki 中的长期规则一致。
- deliverables:
  - 设计文档冻结版。
  - 清理清单。
  - 执行阶段的 guardrails 清单。
- dependencies:
  - 无。
- open_questions:
  - 无，本阶段规则已冻结。
- checklist:
  - 列出超大文件与当前职责问题。
  - 冻结 `CodeGen` 目标目录结构。
  - 冻结 `Scriban` 作为核心层的规则。
  - 冻结“不混入 TestFramework”“不扩大 string 协议面”两条硬约束。

### Phase 1: CodeGen Internal Layering

- goal:
  - 在不新增 assembly 的前提下，把 `Chaos.IL2CPP.CodeGen` 重组为清晰的内部层次。
- exit_criteria:
  - `CoreIr/Planning/RuntimeSupport/Emission/Templating/Templates` 目录落地。
  - 新旧文件归属清晰，核心类型不再平铺在项目根目录。
  - 能说明每个目录的单一职责。
- deliverables:
  - 目录重组。
  - 命名调整。
  - 基础 namespace 收口。
- dependencies:
  - `Phase 0`
- open_questions:
  - 无，采用“先内部平衡分层、后评估 assembly 拆分”的冻结方案。
- checklist:
  - 为 `CodeGen` 建立目标目录。
  - 收口共用 model、plan、templating 基础类型。
  - 调整 namespace，避免“目录已拆、命名未收口”。
  - 保证构建入口和项目引用不被无意义扩大。

### Phase 2: NativeAot Planner Decomposition

- goal:
  - 把 `NativeAotLoweringPlanner.cs` 从超级文件拆成职责明确的 planner 家族。
- exit_criteria:
  - entry ABI、dispatch、EH、opcode legalization、translation unit planning、runtime support aggregation 不再混在单文件里。
  - planner 层不再直接承担最终文本拼接。
  - 超大 orchestrator 文件尺寸回到阈值内或接近阈值并有明确继续拆分路径。
- deliverables:
  - 多个 planner 子文件。
  - 共享 plan model。
  - runtime support 需求聚合模型。
- dependencies:
  - `Phase 1`
- open_questions:
  - 无，已冻结为“planner 只出 plan，不负责 render”。
- checklist:
  - 拆出 `EntryAbi` 规划。
  - 拆出 `Dispatch` 规划。
  - 拆出 `ExceptionHandling` 规划。
  - 拆出 translation unit plan builders。
  - 拆出 runtime support aggregation。
  - 移除 planner 内的大块文本拼接。

### Phase 3: NativeAot Emitter And Scriban Deepening

- goal:
  - 让 `NativeAotEmitter` 真正依赖结构化模板层，而不是只包一层薄模板。
- exit_criteria:
  - 关键 `.cpp/.h` shape 均经 `Scriban` 模板输出。
  - `Templates/NativeAot/*` 不再只是 `{{ raw_code }}` 类型的薄包裹。
  - 新增 codegen 规则优先通过模板模型和模板函数扩展完成。
- deliverables:
  - 更完整的 `NativeAot` 模板集。
  - `Templating/` 渲染基础设施。
  - template model adapters / functions。
- dependencies:
  - `Phase 1`
  - `Phase 2`
- open_questions:
  - 无，已冻结“模板能力不足时先扩展 Scriban 支撑层”。
- checklist:
  - 审计并迁移 `StringBuilder.AppendLine(...)` 高频片段。
  - 为 method/object model/translation unit/include/helper block 建模板。
  - 收口模板加载、缓存、渲染错误诊断。
  - 给模板选择建立强类型映射。

### Phase 4: NativeReference And Secondary Emitters Cleanup

- goal:
  - 收口 `NativeReferenceLoweringPlanner` / `NativeReferenceProofEmitter` / `EngineBindingStubEmitter` 等二级 codegen 面的职责。
- exit_criteria:
  - `NativeReference` family 的 planning 与 emission 边界清晰。
  - family 常量、模板路径、模板加载逻辑不再分散重复。
  - 其它 emitter 不再各自手写一套模板基础设施。
- deliverables:
  - `NativeReference` 分层收口。
  - shared templating 复用。
  - secondary emitter 清理清单落地。
- dependencies:
  - `Phase 3`
- open_questions:
  - 无。
- checklist:
  - 抽 shared template resolver / loader。
  - planner 与 emitter 对齐统一模型。
  - 清理 `EngineBindingStubEmitter` 中可模板化文本生成。
  - 删除重复常量与路径定义。

### Phase 5: Loader/Linker/Contracts/Driver Stage Split

- goal:
  - 把其它 managed 热点项目中超大 stage 文件拆成按职责分层的文件集合。
- exit_criteria:
  - `LoaderStage.cs`、`LinkerStage.cs`、`ManagedClosureContracts.cs`、`DriverEntry.cs`、`SemanticWorldStage.cs` 的主要职责被拆开。
  - stage 文件不再兼做多种解析、序列化、诊断与 orchestration。
- deliverables:
  - 分层后的文件结构。
  - 保留的 orchestrator 壳文件。
  - 局部测试更新。
- dependencies:
  - `Phase 0`
  - 推荐在 `Phase 1-4` 收口后推进。
- open_questions:
  - 无。
- checklist:
  - `Loader`: 入口发现、句柄解析、方法体加载、诊断拆开。
  - `Linker`: reachability、preservation、closure 规则拆开。
  - `Contracts`: schema/serialization/closure contracts 拆开。
  - `Driver`: commands/pipelines/workspace/diagnostics 拆开。

### Phase 6: Gates, Verification And Wiki Closure

- goal:
  - 为结构治理建立持续验证入口，并把长期规则沉淀到 wiki。
- exit_criteria:
  - 至少有一组构建与关键测试验证结构重构未破坏主线。
  - wiki 页面与索引已更新。
  - 后续 feature 接入可以直接引用规范页进行自检。
- deliverables:
  - 验证命令清单。
  - wiki 规范页。
  - 必要的 reviewer checklist。
- dependencies:
  - `Phase 1-5`
- open_questions:
  - 无。
- checklist:
  - 跑最小构建验证。
  - 跑关键 unit/contracts 回归。
  - 跑一条 `NativeAOT` 真链路。
  - 更新 wiki 与目录索引。

## 5. 每阶段完成定义

一个阶段只有在以下条件同时满足时才算完成：

- 结构改动已经落地，而不是只写在注释里。
- 对应职责边界能被文档与代码共同说明。
- 至少完成该阶段定义的最小验证。
- 未把临时过渡逻辑长期残留在主路径里。

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260417-02-phase-0-source-tree-cleanup-and-boundary-freeze` | `Phase 0` | `ready` | `codex` | 冻结结构规则、模板治理规则与源码树清洁边界 | `20260417-02` |
| `20260417-02-phase-1-codegen-internal-layering` | `Phase 1` | `planned` | `codex` | 建立 `Chaos.IL2CPP.CodeGen` 内部目录和命名分层 | `20260417-02-phase-0-source-tree-cleanup-and-boundary-freeze` |
| `20260417-02-phase-2-native-aot-planner-decomposition` | `Phase 2` | `planned` | `codex` | 拆分 `NativeAotLoweringPlanner`，收口 plan 职责 | `20260417-02-phase-1-codegen-internal-layering` |
| `20260417-02-phase-3-native-aot-emitter-and-scriban-deepening` | `Phase 3` | `planned` | `codex` | 深化 `NativeAotEmitter + Scriban` 模板主线 | `20260417-02-phase-2-native-aot-planner-decomposition` |
| `20260417-02-phase-4-native-reference-and-secondary-emitters-cleanup` | `Phase 4` | `planned` | `codex` | 清理 `NativeReference` 与二级 emitter 的重复职责 | `20260417-02-phase-3-native-aot-emitter-and-scriban-deepening` |
| `20260417-02-phase-5-loader-linker-contracts-driver-stage-split` | `Phase 5` | `planned` | `codex` | 拆分其它 managed 热点 stage 文件 | `20260417-02-phase-4-native-reference-and-secondary-emitters-cleanup` |
| `20260417-02-phase-6-gates-verification-and-wiki-closure` | `Phase 6` | `planned` | `codex` | 完成结构治理验证与 wiki 收口 | `20260417-02-phase-5-loader-linker-contracts-driver-stage-split` |

## 7. 依赖

- `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/brainstorm-v1-01.md`
- `docs/dev/abandoned/20260416-01-aot-core-ir-gap-audit-and-next-expansion-roadmap/design-v1-01.md`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceLoweringPlanner.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/`
- `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs`
- `src/managed/Chaos.IL2CPP.Linker/LinkerStage.cs`
- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureContracts.cs`
- `src/managed/Chaos.IL2CPP.EngineBinding/EngineBindingStubEmitter.cs`

## 8. 风险

- 如果 `Phase 2` 之前就继续扩大量新 feature，超大 planner 文件会继续膨胀。
- 如果 `Phase 3` 不同步推进，结构拆分后仍可能重新回退到字符串拼接。
- 如果 `Templating` 不统一，`NativeAot`、`NativeReference`、`EngineBinding` 会各自维护半套模板系统。
- 如果 `Phase 5` 拖太后，`Loader/Linker/Contracts` 仍会持续吸收新逻辑，结构债会重新累积。

## 9. 备选路径

### 备选路径 A：先拆 assembly 再拆文件

- 不推荐。
- 原因：会把当前坏边界物理复制到更多项目里，短期成本高，收益不稳定。

### 备选路径 B：先继续扩 feature，等稳定后再重构

- 不推荐。
- 原因：当前已经不是“轻微结构债”，而是直接影响主线扩张速度。

### 备选路径 C：只做模板迁移，不动结构

- 不推荐。
- 原因：只能缓解文本拼接，不能解决 planner/emitter/runtime support 混杂。

## 10. 当前建议推进顺序

1. 先以 `Phase 0` 冻结规则与清理边界。
2. 立刻进入 `Phase 1`，给 `CodeGen` 建立可承载后续拆分的目录骨架。
3. 紧接着推进 `Phase 2` 与 `Phase 3`，优先处理 `NativeAot` 主面。
4. 再做 `Phase 4` 与 `Phase 5`，清理次级 emitter 和其它 stage 热点。
5. 最后用 `Phase 6` 跑验证并将规范持续写入 wiki。
