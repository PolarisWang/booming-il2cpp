# Subject Structure And Variant Traceability Brainstorm v1.01

Date: 2026-04-07
Status: design-approved-enter-roadmap

## 1. 目标

本轮 brainstorm 的目标不是直接改代码，而是把用户提出的三组复杂约束收敛成稳定设计：

- 仓库结构强约束
- il2cpp 流程的全链路可追踪性
- `CHECK / PROFILE / SHIP` 三种 native 变体语义

输出要求是：

- 先完成设计拍板
- 再进入 `roadmap`
- 最终实现再统一沉淀到 `wiki/`

## 2. 已确认的仓库结构硬约束

- `tests/` 只允许放与具体 subject 无关的通用测试脚本、框架适配器和 harness 公共代码。
- `subjects/` 升级为正式 subject 根，每个 subject 自带源码、单元测试、性能测试和 proof 相关材料。
- subject 运行过程中的全部中间产物与日志统一进入 `artifacts/subjects/<subject-id>/`。
- `src/` 只保留 il2cpp 核心代码，不再混入测试工程。
- `analysis`、`contract` 的旧分散布局需要收敛到对应 canonical 目录；其中误导性的 `examples` 命名要改成 `samples`。

## 3. 已确认的执行与追踪约束

- 目标主流程固定为 `analysis -> codegen -> native -> report -> perf`。
- 整条链路必须可追踪，日志可查询，输出目录层次清晰。
- `run test subject --id subject/<id>` 是统一公开入口。
- canonical 产物采用 run-scoped 布局，按 `runs/<run-id>` 保存，而不是覆盖式“最新目录”。

## 4. 已确认的 variant 约束

- native 项目必须区分 `CHECK`、`PROFILE`、`SHIP` 三个版本。
- `variant` 不是单纯目录标签，而是从 `codegen` 开始就参与宏定义生成。
- native 编译继续承接相同 variant 的宏定义与编译选项。
- 语义冻结为：
  - `CHECK`：正确性 / 开发追踪版，保留 debug 能力与可观测性。
  - `PROFILE`：性能分析版，偏高性能，关闭面向调试的额外输出。
  - `SHIP`：发布版，最高优化，去除 debug 信息。
- 宏定义清单最终要输出到 `wiki/`。

## 5. 关键拍板结果

本轮与用户逐项对齐后，已确认以下选择：

- `contracts` 收敛策略：先做 resolver first，再逐步物理迁移。
- `contracts/examples` 语义保留，但命名改为 `samples`，避免误会成运行期中间产物。
- `subjects/` 成为真正的 subject 根，采用 `source / validation / expected / baselines` 结构。
- subject 内的单测和 perf 代码都留在 `subjects/` 下，技术栈转为 C#。
- 单元测试框架使用 `xUnit`。
- 性能测试先由 C# perf harness 驱动。
- `subject.manifest.json` 采用显式 typed sections。
- 默认命令 `run test subject --id ...` 采用 default profile，而不是默认跑全部验证。
- `validationProfiles` 独立存在，不借 `goal` 承载 bundle。
- `goal` 降级为兼容层，不再作为公开主轴。
- `variant` 从 `codegen` 开始成为一等轴，不折进 `matrix`。
- run ledger 改为 `artifacts/subjects/<id>/runs/` subject-local 管理。
- canonical 产物以 `run-scoped` 目录为主，不再以覆盖式 latest 目录为 canonical。
- 目录分层采用 `run -> analysis -> variant -> matrix`。
- `pipeline-report/` 与 `validations/proof|unit|perf/` 分层保存。
- `validation` 配置采用 typed + minimal，只保留执行所需最小字段。

## 6. 推荐进入 roadmap 的原因

本任务已经明显超出单份实现 plan 的边界：

- 涉及多个顶层目录和执行入口
- 需要兼容旧路径和旧命令一段时间
- 需要分阶段推进 `resolver -> schema -> artifacts -> variant -> CLI -> wiki`
- 很可能跨多次会话完成

因此，本轮 brainstorm 的自然下一步不是 `writing-plans`，而是 `roadmap`。
