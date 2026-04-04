# 测试与验证体系升级方案实现计划

> **面向执行 Agent：** 执行时优先使用 `dev:subagent-driven-development` 或 `dev:executing-plans`。实现期必须优先采用 `dev:test-driven-development`，并在完成声明前执行 `dev:verification-before-completion`。

**目标：** 将现有统一测试框架升级为面向项目的测试与验证体系：新增 `module/system/pipeline` 正式对象层、统一 registry 与静态索引、统一 `run test` 子域、统一 TUI 新语义视图、统一 skill 引用链和结构化错误定位，并一次性废除 `run verify` 正式入口。

**架构：** 保留现有 `family/suite/case/adapter` 执行层；新增验证层对象和编排层对象；引入 `tests/registry/**` 与 `wiki/06-测试验证/**` 双层结构；CLI/TUI/harness/skill 全部转向 `run test` + selector 内核；静态索引快照作为机器主入口，wiki 作为说明与机器回退入口。

**技术栈：** Python `build/toolchains/run/`、`run_manifest.json`、现有 `testing/` 子系统、`tests/**` 测试家族、`wiki/06-测试验证/`、`.codex/skills/**`、结构化 JSON 结果与事件流。

**设计文档：** `docs/dev/in-progress/20260404-05-test-verification-architecture/design-v1-01.md`

**预期知识沉淀：** `wiki/06-测试验证/**`

---

## Chunk 1：项目级测试知识入口与正式规则

- [ ] 建立 `wiki/06-测试验证/` 主入口、模块说明目录、系统场景目录、测试管线目录。
- [ ] 迁移现有长期有效的测试说明到新入口，旧文档只保留必要引用。
- [ ] 建立“重要验证必须升格”为正式规则集，并加入临时例外登记结构。
- [ ] 为 wiki 页面定义固定元数据区块、固定标题层级和固定链接区块。

**关键验证：**
- 页面结构检查通过。
- 规则集与例外登记支持机器消费。

## Chunk 2：正式对象层与统一 registry

- [ ] 新增 `tests/registry/modules/**/verification.manifest.json`
- [ ] 新增 `tests/registry/system/**/scenario.manifest.json`
- [ ] 新增 `tests/registry/pipelines/**/pipeline.manifest.json`
- [ ] 新增统一 `registry.py`，同时扫描 suite、module verification、system scenario、pipeline。
- [ ] 让注册文件保留 selector / 引用定义，静态索引快照补充展开结果。
- [ ] 生成：
  - `artifacts/tests/registry/current/index.json`
  - `artifacts/tests/registry/history/<stamp>/index.json`

**关键验证：**
- registry 输出同时具备分段视图和拍平视图。
- 快照同时包含定义层和展开层。

## Chunk 3：`run test` 新子域与 selector 内核

- [ ] 在 `run test` 下新增：
  - `suite`
  - `module`
  - `system`
  - `pipeline`
  - `registry`
- [ ] 增加 selector 内核，支持对象 ID 与拆字段两种输入形式。
- [ ] 一次性废除 `run verify` 正式入口。
- [ ] 引入 `run test registry refresh/list/check-consistency`。

**关键验证：**
- 正式入口只剩 `run test`。
- `run test suite/module/system/pipeline` 均可通过 selector 内核解析。
- `run test registry` 子域可工作。

## Chunk 4：TUI 与静态索引消费

- [ ] TUI 接入双视图：
  - 新语义视图
  - family 专家视图
- [ ] 首次默认新语义视图，后续记住用户偏好。
- [ ] 新语义视图按 `code/module/system/pipeline` 展示。
- [ ] 模块验证、系统场景、测试管线分别按已确认的分组方式展示。

**关键验证：**
- TUI 首屏与路由均能消费统一 registry/静态索引。
- 新语义视图与专家视图可切换并保持偏好。

## Chunk 5：skill 引用链与完成门槛

- [ ] 新增 skill 使用的对象解析与文档回退逻辑。
- [ ] skill 先读静态索引快照，再按 `docRefs` 回退到 `wiki/06-测试验证/`。
- [ ] 实现三级输出：
  - `recommended`
  - `required-before-completion`
  - `required-for-pipeline/release`
- [ ] 若主模块已有模块验证对象，则默认至少输出一个完成前必过项。
- [ ] 若对象缺失，则退回 suite/case 并给出补齐建议。
- [ ] 独立建立“skill 引用链正确性”测试。

**关键验证：**
- skill 引用对象必须真实存在于 registry。
- 三级输出与模块完成门槛策略正确。

## Chunk 6：归属链、失败编码与人类提示

- [ ] 在 summary/report/events 中补齐完整归属链：
  - suite/case
  - module verification
  - system scenario
  - pipeline
- [ ] 统一 `failureCode`、`parserHint`、`logPath` 降级定位契约。
- [ ] 统一 `humanHint` 格式为“失败对象 + 原因 + 下一步建议”。
- [ ] 要求下一步建议只能引用正式对象、正式命令或日志路径。

**关键验证：**
- 结果能追溯到完整对象链。
- 低精度定位也具备结构化可恢复信息。

## Chunk 7：一致性检查与一次性切换

- [ ] 将 wiki 与 registry 一致性检查纳入 `run test registry check-consistency`。
- [ ] 在 CI 中新增显式检查步骤。
- [ ] 对不一致项做阻断/警告分级。
- [ ] 清理正式帮助、TUI 菜单、skill 引用和文档中的 `run verify`。
- [ ] 完成一次性切换并保留最小隐藏兼容层。

**关键验证：**
- CI 有独立可见的一致性检查步骤。
- 正式用户入口、TUI 和文档中不再出现 `run verify`。

## 最终验证

```text
run test registry refresh
run test registry check-consistency
run test registry list --json
run test suite --id smoke/HelloWorld
run test module --id module/<sample-module>/basic
run test system --id system/<sample-scenario>
run test pipeline --id pipeline/<sample-pipeline>
```

最终整体验证需覆盖：

- registry
- CLI
- TUI
- skill 引用链
- 结果结构
- 错误定位

## 风险与回滚点

- 一次性切换会同时影响入口语义、文档和测试消费方，必须先用 registry、skill 引用链和帮助输出测试锁死边界。
- wiki 不参与自动刷新，但要通过显式刷新和 CI 契约检查保障一致性。
- 如果一次性切换中发现对象模型不足，可在不恢复 `run verify` 的前提下，先限制新对象公开范围，再补实现。
