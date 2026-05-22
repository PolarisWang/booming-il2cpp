# project-test-governance

> 面向本仓�?subject 测试主线、`Chaos.TestFramework.Sdk/Runtime` 分层、collection file、manifest、Python 模板化测试与 Scriban codegen 的项目级治理 skill�?

## 基本信息

- 分层：质量保�?
- 项目职责：作为测试治理入口，负责把任务路由回 authority 文档、planning intake �?completion gate
- 实现路径：`.codex/skills/dev-project-test-governance/SKILL.md`
- 状态：`project`

## Authority 边界

- [`../../../docs/architecture/subject-test-framework-v1/INDEX.md`](../../../docs/architecture/subject-test-framework-v1/INDEX.md)
  - 拥有统一测试主线、collection / manifest / codegen 分层�?subject test framework 边界
- [`../../../docs/archive/architecture/verification-v1/spec.md`](../../../docs/archive/architecture/verification-v1/spec.md)
  - 拥有 formal verification、归并归档、projection 与权责图审核边界
- [`../../06-测试验证/INDEX.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/INDEX.md)
  - 拥有正式验证入口、对象导航与 completion 前对象优先级
- [`../../06-测试验证/AOT新Feature接入自测规范.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/AOT%E6%96%B0Feature%E6%8E%A5%E5%85%A5%E8%87%AA%E6%B5%8B%E8%A7%84%E8%8C%83.md)
  - 拥有 capability intake、owner subject、proof / benchmark / hotupdate obligation、formal verification 顺序
- [`../../06-测试验证/CodeGen快照测试规范.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/CodeGen%E5%BF%AB%E7%85%A7%E6%B5%8B%E8%AF%95%E8%A7%84%E8%8C%83.md)
  - 拥有 CodeGen 增量回归测试的夹具规范、基线管理、开发流程集成规则

## 核心规则

- managed solution 是唯一测试�?- collection file �?managed/native/hotupdate 的唯一跨执行形态契�?- `subject.features.json` �?owner subject �?proof / benchmark obligation 的真�?- canonical proof / benchmark / host correctness 不允许依�?`Console.WriteLine` / `ChaosEvidenceKind.Stdout`
- 测试阶段�?`dotnet` 编译崩溃必须先查根因并修�?- IL2CPP / Python file-level codegen 默认 Scriban，能力不足时先扩�?Scriban
- cutover 完成后删除旧 alias、旧命名和旧双轨逻辑
- 如果任务要求新的验证通过记录或新的 verification 数据，formal object 跑通后还必须刷新 unified test framework 正式产物；默认命令为 `run test inventory --json`
- 如果本轮改动触及 formal report / projection contract，例如 `Program / DLL / Verification Project / Artifact` 报告对象、`latest/master/reports` 字段、`testing-inventory` / `benchmark` 页面字段或证据链接规则，也必须在实现后刷新 unified test framework 正式产物；不允许只改 schema、模板或页面读取逻辑而不刷新正式数据
- 对于 DLL-first reporting 一类 evidence-driven projection，`artifacts/**` 下的真实产物才允许进入 primary evidence / artifact index；`docs/**`、`subjects/**`、`testing/**` 等引用只能作为 support refs，不能把项目状态置为通过，也不能混入 primary artifact table
- foundation-dll-audit 的真实进度主轴是 `Capability Closure`，其分母 authority 是 `artifact/verification-catalog/projections/foundation-dll-audit/capability-family-ledger.json`；不允许再用 verification project 数量、workspace 数量或样例数量代替
- `Workflow Progress` 只是辅助流程条，用来表示 family 级 required gates 推进到哪一步，不代表 capability 覆盖本身
- `completion-certification` 是 DLL 级 gate，不进入 capability family 的 workflow denominator
- `workspace` 只允许作为 proof container / reproducibility container，不能成为 DLL capability progress 的正式分母
- waiver / exclusion / platform-blocked 三类豁免必须落在 capability-family ledger 中并带正式 authorityRef，不允许只写在 task `STATUS.md`
- 新测试流程的数据以 `artifact/verification-catalog/archive/{latest,master,reports}` 与 `artifact/verification-catalog/evidence/owners/*/codegen-stubs/*` 为准；`artifacts/**` 运行日志只算过程证据
- 命中 projection / report contract 时，还必须确认对应 `artifact/verification-catalog/projections/**` 派生产物已更新
- `run test inventory` 只是内部命令，不作为对外 public entry。
- `benchmark --record` 只写 raw benchmark records，不等于 formal refresh。需要新的 benchmark archive / projection / merged data 时，必须后续执行 `run test inventory --json`。

## 命中 AOT / IL2CPP / test-flow 任务时必须先冻结

- `capabilityFamily`
- `capabilityItem`
- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

## 最近变更
- `2026-04-18`：把 skill 本身收缩为治理入口，不再重复承担全部 authority 叙述，并新增 obligation intake 字段冻结要求。
- `2026-04-23`：补充”新验证记录 / 新数据”规则，要求命中测试治理主线的任务在 formal object 通过后刷新 unified test framework 正式产物。
- `2026-04-24`：补充 public command surface 口径，明确 `run test inventory` 已内部化，以及 `benchmark --record` 只写 raw records，不等于 formal refresh。
- `2026-04-25`：补充 DLL-first reporting / projection contract 刷新要求；命中报告对象、页面字段或证据链接规则改动时，不能只改 schema 或页面，必须同步刷新 unified test framework 正式数据与对应 projection。
- `2026-04-25`：补充 DLL-first reporting 证据分层口径；`artifacts/**` 只计 primary evidence，`docs/**` / `subjects/**` / `testing/**` 只作为 support refs。
- `2026-04-26`：补充 foundation-dll-audit 双轴口径；`Capability Closure` 成为 DLL 真实进度主轴，`Workflow Progress` 退为辅助流程条，`workspace` 明确降级为 proof container，不再允许作为正式分母。

