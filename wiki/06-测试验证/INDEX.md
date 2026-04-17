# 06-测试验证 INDEX

> 项目级测试与验证知识的正式入口。当前主线以 collection-driven 的 managed/native/hotupdate 流程为准。

## 正式主线

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
| [`AOT新Feature接入自测规范.md`](./AOT新Feature接入自测规范.md) | AOT 新 feature 自测 | 说明 owner subject、hotupdate 触发条件、collector/registry/workspace 接线闸门与标准验收顺序 |
| [`新增测试接入规范.md`](./新增测试接入规范.md) | 新增测试接入 | 说明如何在 subject source 中声明测试，并接入 collection-driven 主线 |
| [`../04-工具与集成/统一测试框架.md`](../04-%E5%B7%A5%E5%85%B7%E4%B8%8E%E9%9B%86%E6%88%90/%E7%BB%9F%E4%B8%80%E6%B5%8B%E8%AF%95%E6%A1%86%E6%9E%B6.md) | 统一测试框架 | 说明 `Sdk / Runtime / collector / manifest` 分层 |
| [`subject-public-entry-and-reporting-cutover.md`](./subject-public-entry-and-reporting-cutover.md) | subject 统一入口 | 说明 subject 入口、public command 和结果落点 |
| [`subject-perf-and-smoke-baselines.md`](./subject-perf-and-smoke-baselines.md) | perf / smoke baseline | 说明 subject 的 perf baseline、smoke 验证与报告路径 |

## 子类别

| 类别 | 说明 | 索引 |
| --- | --- | --- |
| `模块` | 模块级验证对象与映射 | [`模块/INDEX.md`](./模块/INDEX.md) |
| `整体验证场景` | system 级验证对象与入口 | [`整体验证场景/INDEX.md`](./整体验证场景/INDEX.md) |
| `测试管线` | pipeline 级验证对象与入口 | [`测试管线/INDEX.md`](./测试管线/INDEX.md) |

## 本层规则

- 本目录是测试与验证规则的正式知识入口，不再以临时脚本或零散说明替代。
- `subjects/` 只保留 managed solution 级 source。
- subject / managed project 只直接引用 `Chaos.TestFramework.Sdk`。
- collection file 由 `.NET 8` collector 统一产出；三端执行项目只消费它，不重复扫描 `Sdk`。
- native 与 hotupdate 的执行细节通过 manifest 分层，不污染 collection contract。
- dashboard 和报告只消费正式产物，不能替代 collection contract 本身。
- Python 自动化测试优先模板化；手工 benchmark、dashboard 观察和控制台输出都不能替代自动化验证。
- 测试阶段只要 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃，就必须先检查崩溃原因并修复；不能靠重试、跳过或降级为环境噪音继续推进。
- `subject.features.json` 是 owner subject 与 proof / benchmark obligation 的正式 authority。
- canonical proof / benchmark / host correctness 不允许使用 `Console.WriteLine` / `ChaosEvidenceKind.Stdout`。
- product pipeline 不允许回退到旧入口协议或旧 subject 命名。
- 不保留长期 `Annotation` alias 或旧双轨入口。

## 最近更新

- `2026-04-17`：新增 [`AOT新Feature接入自测规范.md`](./AOT新Feature接入自测规范.md)，固定 AOT 主线 feature 的 owner subject、自测顺序与 collector/registry/workspace 三层闸门。
- `2026-04-17`：把主线升级为 `Sdk + Runtime + collector + manifest` 分层，并明确 `Assert` 下沉到 `Sdk`、native/hotupdate 分别采用各宿主和 patch/host 分离。
- `2026-04-17`：新增测试阶段 `dotnet` 编译崩溃闸门；编译崩溃必须先查根因并修复，不能靠重试或跳过继续测试。
- `2026-04-18`：补充 `subject.features.json` authority、canonical verification 禁止 stdout 判定，以及旧入口协议/旧目录语义 purge contract。
