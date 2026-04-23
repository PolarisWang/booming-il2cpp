# project-test-governance

> 面向本仓库 subject 测试主线、`Chaos.TestFramework.Sdk/Runtime` 分层、collection file、manifest、Python 模板化测试与 Scriban codegen 的项目级治理 skill。

## 基本信息

- 分层：质量保障
- 项目职责：作为测试治理入口，负责把任务路由回 authority 文档、planning intake 与 completion gate
- 实现路径：`.codex/skills/dev-project-test-governance/SKILL.md`
- 状态：`project`

## Authority 边界

- [`../../../docs/architecture/subject-test-framework-v1/INDEX.md`](../../../docs/architecture/subject-test-framework-v1/INDEX.md)
  - 拥有统一测试主线、collection / manifest / codegen 分层与 subject test framework 边界
- [`../../../docs/architecture/verification-v1/spec.md`](../../../docs/architecture/verification-v1/spec.md)
  - 拥有 formal verification、归并归档、projection 与权责图审核边界
- [`../../06-测试验证/INDEX.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/INDEX.md)
  - 拥有正式验证入口、对象导航与 completion 前对象优先级
- [`../../06-测试验证/AOT新Feature接入自测规范.md`](../../06-%E6%B5%8B%E8%AF%95%E9%AA%8C%E8%AF%81/AOT%E6%96%B0Feature%E6%8E%A5%E5%85%A5%E8%87%AA%E6%B5%8B%E8%A7%84%E8%8C%83.md)
  - 拥有 capability intake、owner subject、proof / benchmark / hotupdate obligation、formal verification 顺序

## 核心规则

- managed solution 是唯一测试源
- collection file 是 managed/native/hotupdate 的唯一跨执行形态契约
- `subject.features.json` 是 owner subject 与 proof / benchmark obligation 的真源
- canonical proof / benchmark / host correctness 不允许依赖 `Console.WriteLine` / `ChaosEvidenceKind.Stdout`
- 测试阶段的 `dotnet` 编译崩溃必须先查根因并修复
- IL2CPP / Python file-level codegen 默认 Scriban，能力不足时先扩展 Scriban
- cutover 完成后删除旧 alias、旧命名和旧双轨逻辑
- 如果任务要求新的验证通过记录或新的 verification 数据，formal object 跑通后还必须刷新 `verification-v1` 正式产物；默认命令为 `run verify verification-v1 --json`
- 新测试流程的数据以 `docs/testing-inventory/verification/{latest,master,reports}` 与 `subjects/*/verification/codegen-stubs/*` 为准；`artifacts/**` 运行日志只算过程证据

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
- `2026-04-23`：补充“新验证记录 / 新数据”规则，要求命中测试治理主线的任务在 formal object 通过后刷新 `verification-v1` 正式产物。
