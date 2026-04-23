# 06-测试验证 INDEX

> 项目级测试与验证知识的正式入口。当前主线以 collection-driven 的 managed/native/hotupdate 流程为准。

## Authority 分工

- [`Verification-V1测试流程规范.md`](./Verification-V1测试流程规范.md)
  - 拥有新的长期 canonical 测试流程入口、formal source 口径、全覆盖模型与旧流程退役规则
- [`../../docs/architecture/subject-test-framework-v1/INDEX.md`](../../docs/architecture/subject-test-framework-v1/INDEX.md)
  - 拥有统一测试主线、collection / manifest / codegen 分层与宿主边界
- [`AOT新Feature接入自测规范.md`](./AOT新Feature接入自测规范.md)
  - 拥有 capability intake、owner subject、proof / benchmark / hotupdate obligation 与 formal verification 顺序
- 本 `INDEX.md`
  - 拥有正式验证入口、对象导航、completion 前对象优先级与跨页面总入口

## 正式主线

- 正式长期流程入口：[`Verification-V1测试流程规范.md`](./Verification-V1测试流程规范.md)
- 默认 formal refresh 入口：`run verify verification-v1 --json`
- 主线定义：`managed solution -> dotnet 8 collection analysis -> collection files -> managed test project -> native project -> native test project -> hotupdate patch project + hotupdate test host project`
- 测试声明：由 `Chaos.TestFramework.Sdk` 中的 attribute 在 managed solution 中声明。
- collection 产生：由 `.NET 8` collector 分析 managed project 中的 `Sdk` 声明并输出 collection file。
- 执行与断言：
  - `Sdk` 提供 subject-side `Assert`
  - `Runtime` 提供执行宿主、collection loader、benchmark harness、reporting
- 跨执行形态契约：UnitTest / Benchmark / HotUpdate collection file 是 managed/native/hotupdate test host 的统一输入。

## 优先阅读

| 文档 | 主题 | 说明 |
| --- | --- | --- |
| [`Verification-V1测试流程规范.md`](./Verification-V1测试流程规范.md) | Verification V1 测试流程 | 说明新的 canonical 测试流程、formal source、覆盖模型、投影视图与旧流程清理规则 |
| [`AOT新Feature接入自测规范.md`](./AOT新Feature接入自测规范.md) | AOT 新 feature 自测 | 说明 owner subject、hotupdate 触发条件、collector/registry/workspace 接线闸门与标准验收顺序 |
| [`新增测试接入规范.md`](./新增测试接入规范.md) | 新增测试接入 | 说明如何在 subject source 中声明测试，并接入 collection-driven 主线 |
| [`../04-工具与集成/统一测试框架.md`](../04-%E5%B7%A5%E5%85%B7%E4%B8%8E%E9%9B%86%E6%88%90/%E7%BB%9F%E4%B8%80%E6%B5%8B%E8%AF%95%E6%A1%86%E6%9E%B6.md) | 统一测试框架 | 说明 `Sdk / Runtime / collector / manifest` 分层 |
| [`../04-工具与集成/scriban-usage-and-codegen-rules.md`](../04-%E5%B7%A5%E5%85%B7%E4%B8%8E%E9%9B%86%E6%88%90/scriban-usage-and-codegen-rules.md) | Scriban 使用与 codegen 规范 | 说明 IL2CPP / Python codegen 默认优先 Scriban，以及能力不足时的扩展顺序 |
| [`subject-public-entry-and-reporting-cutover.md`](./subject-public-entry-and-reporting-cutover.md) | subject 统一入口 | 说明 subject 入口、public command 和结果落点 |
| [`subject-perf-and-smoke-baselines.md`](./subject-perf-and-smoke-baselines.md) | perf / smoke baseline | 说明 subject 的 perf baseline、smoke 验证与报告路径 |

## 子类别

| 类别 | 说明 | 索引 |
| --- | --- | --- |
| `模块` | 模块级验证对象与映射 | [`模块/INDEX.md`](./模块/INDEX.md) |
| `整体验证场景` | system 级验证对象与入口 | [`整体验证场景/INDEX.md`](./整体验证场景/INDEX.md) |

## 本层规则

- 本目录是测试与验证规则的正式知识入口，不再以临时脚本或零散说明替代。
- `subjects/` 只保留 managed solution 级 source。
- subject / managed project 只直接引用 `Chaos.TestFramework.Sdk`。
- collection file 由 `.NET 8` collector 统一产出；三端执行项目只消费它，不重复扫描 `Sdk`。
- native 与 hotupdate 的执行细节通过 manifest 分层，不污染 collection contract。
- dashboard 和报告只消费正式产物，不能替代 collection contract 本身。
- Python 自动化测试优先模板化；手工 benchmark、dashboard 观察和控制台输出都不能替代自动化验证。
- IL2CPP / Python 结构化文本 codegen 默认优先 `Scriban`；功能不足时先扩展 `Scriban`，不新增长期手写拼串主线。
- 测试阶段只要 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃，就必须先检查崩溃原因并修复；不能靠重试、跳过或降级为环境噪音继续推进。
- `subject.features.json` 是 owner subject 与 proof / benchmark obligation 的正式 authority。
- canonical proof / benchmark / host correctness 不允许使用 `Console.WriteLine` / `ChaosEvidenceKind.Stdout`。
- obligation-driven AOT / IL2CPP 任务在计划中必须显式声明 `formalVerificationObjects` 与 `requiredGates`。
- `completed` 前必须先通过计划声明或 authority 要求的正式验证对象，不能只用“项目测试套件大致通过”替代。
- product mainline 不允许回退到旧入口协议或旧 subject 命名。
- 不保留长期 `Annotation` alias 或旧双轨入口。

## 最近更新

- `2026-04-23`：新增 [`Verification-V1测试流程规范.md`](./Verification-V1测试流程规范.md)，将 `Verification V1` 固定为新的长期 canonical 测试流程入口，并明确 formal source、覆盖模型、投影视图与旧流程退役规则。
- `2026-04-17`：新增 [`AOT新Feature接入自测规范.md`](./AOT新Feature接入自测规范.md)，固定 AOT 主线 feature 的 owner subject、自测顺序与 collector/registry/workspace 三层闸门。
- `2026-04-17`：把主线升级为 `Sdk + Runtime + collector + manifest` 分层，并明确 `Assert` 下沉到 `Sdk`、native/hotupdate 分别采用各宿主和 patch/host 分离。
- `2026-04-17`：新增测试阶段 `dotnet` 编译崩溃闸门；编译崩溃必须先查根因并修复，不能靠重试或跳过继续测试。
- `2026-04-18`：补充 `subject.features.json` authority、canonical verification 禁止 stdout 判定，以及旧入口协议/旧目录语义 purge contract。
- `2026-04-18`：明确 architecture / AOT onboarding spec / 本索引三层 authority 分工，并要求 completion 前先跑 formal verification objects。
- `2026-04-19`：补充 Scriban-first codegen 规则，明确 IL2CPP / Python 生成型结构化文本默认优先 Scriban，能力不足时先扩展 Scriban。
- `2026-04-21`：补充 full-assembly-closure native codegen 产物命名约束；runtime skeleton summary/page 必须显式落在 `generated/runtime/`，audit summary 必须落在 `generated/audit/*.audit.generated.cpp`，audit method inventory page 必须改为 JSON sidecar，不能继续作为 C++ translation unit 编译。
- `2026-04-22`：补充 async generic family generated review 规则；runtime-skeleton async wrapper 必须按真实方法签名推导 parameter carrier，`Task<string>(int)` 这类 mixed-carrier case 必须进入正式 generated artifact 回归，不能再靠结果类型白名单推导参数 C++ 类型。
- `2026-04-22`：补充 canonical generic runtime kernel 的 hotupdate/dispatch/reflection 规则；最终 authority 固定为 `InstantiationStubId + generation`，包版本校验固定为 `PackageFormatVersion + KernelArtifactVersion`，advanced carrier 必须带正式 generated review 与 benchmark 证据。
