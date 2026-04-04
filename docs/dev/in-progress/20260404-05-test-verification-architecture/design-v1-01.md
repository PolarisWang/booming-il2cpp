# 测试与验证体系升级方案设计

**日期：** 2026-04-04 20:00 +08:00

## 背景

当前仓库已经拥有统一的 `run test ...` 测试执行框架，但测试与验证体系仍然存在以下结构性缺口：

- `.codex/skills` 是通用方法论，项目级测试说明散落在 wiki、architecture 和历史任务文档中，没有唯一入口。
- 现有测试框架以 `family / suite / case` 为中心，缺乏“模块验证”“系统场景”“测试管线”这类更贴近项目交付语义的正式对象。
- `run verify`、历史手工验证步骤、模块完成门槛与统一测试对象之间没有稳定桥梁。
- skill 无法稳定回答“改了哪个模块应跑哪些测试”“完成前哪些验证是必过项”“失败时该去看哪里”。
- 错误定位虽然已有日志和摘要，但缺乏完整的对象归属链、失败编码和面向人的一致提示格式。

## 目标

建立一套分层的测试与验证体系，使仓库同时具备：

- 对人和 skill 都稳定可查的项目级测试知识入口。
- 统一的执行层对象、验证层对象和编排层对象。
- 统一的 registry、静态索引、入口命令、TUI 视图和 harness 消费方式。
- 统一的完成前验证语义和重要验证沉淀规则。
- 统一的结果归属链、失败编码、降级定位和人类提示契约。

## 非目标

本次设计不包含以下内容：

- 不重写现有 `family / suite / case` 执行层的基本理念，只在其上增加更高层对象。
- 不在本设计中直接实现新的 registry、命令和 TUI，只给出一次性切换方案和实现边界。
- 不把 `.codex/skills` 改造成项目细节存储层，skill 仍保持通用。

## 总体架构

新体系采用一套框架、三层对象：

### 执行层

保留现有统一测试框架的执行对象：

- `family`
- `suite`
- `case`
- `adapter`

### 验证层

新增正式验证对象：

- 模块验证对象：`module/<module-id>/<profile-id>`
- 系统场景对象：`system/<scenario-id>`

### 编排层

新增正式编排对象：

- 测试管线对象：`pipeline/<pipeline-id>`

## 目录与知识入口

项目级测试知识唯一入口固定为：

```text
wiki/06-测试验证/
```

该目录第一版至少包含：

- `INDEX.md`
- `测试分层.md`
- `模块映射.md`
- `新增测试接入规范.md`
- `临时例外登记.md`
- `模块/<module-id>.md`
- `整体验证场景/INDEX.md`
- `整体验证场景/<scenario-id>.md`
- `测试管线/INDEX.md`
- `测试管线/<pipeline-id>.md`

wiki 页面既面向人，也面向机器回退消费，因此必须具备：

- 固定标题层级
- 固定元数据区块
- 固定链接区块

元数据和固定链接至少覆盖：

- 对象 ID
- 对象类型
- level
- primaryModuleId
- moduleIds
- subsystemIds
- 关联对象
- 正式命令入口
- registryRefs
- docRefs

## 验证层级模型

新体系定义横切维度 `level`：

- `code`
- `module`
- `system`

约束如下：

- `level` 是横切维度，不替代现有 `family`
- `code` 主体覆盖 `unit + integration + contract`
- `module` 与 `system` 按验证语义定义，不按现有 family 强绑
- suite 必须声明主层级
- case 可选覆盖 suite 的主层级
- pipeline 允许声明复合层级

## 模块与归属模型

统一使用以下字段表达归属：

- `primaryModuleId`
- `moduleIds`
- `subsystemIds`

约束如下：

- 每个对象至多有一个 `primaryModuleId`
- `moduleIds` 可包含主模块以外的关联模块
- `subsystemIds` 用于表达实现侧子系统、toolchain 区域、runtime 区域等实现归属

## 正式对象模型

### 模块验证对象

对象 ID 形式：

```text
module/<module-id>/<profile-id>
```

模块验证对象允许同一模块存在多个 profile，不强制固定集合，但推荐命名：

- `basic`
- `full`
- `platform-<host>`
- `perf-smoke`
- `release`

### 系统场景对象

对象 ID 形式：

```text
system/<scenario-id>
```

系统场景命名推荐结构为：

- `<domain>-<purpose>-<host?>`

### 测试管线对象

对象 ID 形式：

```text
pipeline/<pipeline-id>
```

测试管线必须包含用途字段，推荐用途包括：

- `dev-regression`
- `completion`
- `release`
- `nightly`

测试管线允许混合：

- suite
- case
- module verification
- system scenario

并支持：

- 阶段分组
- 并行声明
- 前置准备步骤
- 后置清理步骤

## 重要验证沉淀规则

### 核心规则

重要验证不能长期停留在临时手工步骤中，必须沉淀为正式可执行对象。

“重要验证”至少包括：

- 证明真实 bug 已修复的回归验证
- 被当作完成前必须过的检查
- 会进入模块验证、系统场景或测试管线的检查
- 会被后续重复执行的高风险验证
- 失败后会影响模块/系统状态判断的关键验证

### 升格优先级

默认顺序为：

1. 优先加入现有 suite 的新 case
2. 只有在执行单元、准备流程、产物目录、目标平台、主验证目标出现本质变化时，才新建 suite

### 临时例外

允许临时例外，但必须：

- 在任务文档中记录上下文
- 在项目级统一索引中记录例外对象
- 仅允许事件驱动失效，不允许时间驱动失效

允许的失效事件包含：

- 当前任务完成前
- 当前任务合并前
- 接口冻结后
- 平台环境可用后
- 上游依赖合并后

## registry 与静态索引

### 运行注册目录

运行注册统一位于：

```text
tests/registry/
  modules/
  system/
  pipelines/
```

注册文件命名：

- 模块验证对象：`verification.manifest.json`
- 系统场景对象：`scenario.manifest.json`
- 测试管线对象：`pipeline.manifest.json`

### registry 入口

统一 registry 扫描以下对象：

- suite
- module verification
- system scenario
- pipeline

输出同时提供：

- 分段结构：`suites / moduleVerifications / systemScenarios / pipelines`
- 拍平视图：统一对象列表

### 静态索引快照

静态索引采用双层：

- 稳定指针：
  - `artifacts/tests/registry/current/index.json`
- 历史快照：
  - `artifacts/tests/registry/history/<stamp>/index.json`

registry 注册文件仅保留原始 selector / 引用定义；静态索引快照中补充展开结果，供 skill、harness、TUI 和结果归属链直接消费。

### 刷新与一致性检查

- 自动刷新只由运行注册文件变化触发
- wiki 不触发自动刷新
- 显式命令：
  - `run test registry refresh`
  - `run test registry list`
  - `run test registry check-consistency`

wiki 与 registry 的一致性不在每次变更时自动刷新，而在以下时机检查：

- 显式刷新
- CI 契约步骤

最低一致性检查项包括：

- 对象 ID
- 对象类型
- 主模块归属
- `docRefs`

严重级别分级：

- 对象不存在 / 类型不一致 / 主模块不一致：阻断
- 文档引用缺失或部分链接失效：警告

## 统一入口与命令语法

公开入口统一为：

```text
run test ...
```

`run verify` 一次性废除，不再作为正式入口保留。

命令实现采用“selector 内核 + 两种输入形式”：

- 人工友好拆字段形式
- harness 稳定对象 ID 形式

内部统一转为 selector 执行。

统一对象入口：

```text
run test suite ...
run test module ...
run test system ...
run test pipeline ...
run test registry ...
```

人工友好形式：

```text
run test suite --family smoke --suite HelloWorld
run test module --module metadata-registration --profile basic
run test system --scenario roadmap-0-windows
run test pipeline --pipeline release-readiness-windows
```

harness / 稳定引用形式：

```text
run test suite --id smoke/HelloWorld
run test module --id module/metadata-registration/basic
run test system --id system/roadmap-0-windows
run test pipeline --id pipeline/release-readiness-windows
```

selector 内核第一版至少支持：

- `id`
- `family`
- `suite`
- `module`
- `profile`
- `scenario`
- `pipeline`
- `level`
- `primaryModuleId`
- `tag`
- `targetHost`

其中：

- 对象注册保留 `supportedHosts`
- 本次执行意图使用 `targetHost`

## TUI 视图

TUI 保留两套视图：

- 新语义视图
- family 专家视图

首次默认新语义视图，后续记住用户偏好。

新语义视图默认展示顺序：

1. 代码测试
2. 模块验证
3. 整体验证
4. 测试管线

细分展示规则：

- 模块验证：默认先展示模块，再展开 profile
- 系统场景：先按场景域分组，再按平台/host 细分
- 测试管线：先按用途分组，再显示主模块/主场景/主平台信息

## skill 集成

skill 的读取顺序固定为：

1. 静态索引快照
2. `docRefs` 指向的 `wiki/06-测试验证/` 页面
3. 相关 architecture / 任务文档（仅作为回退）

skill 推荐测试动作分三级：

- `recommended`
- `required-before-completion`
- `required-for-pipeline/release`

默认策略：

- 若主模块已有模块验证对象，则 skill 默认至少给出一个 `required-before-completion` 的模块验证对象
- 若主模块尚无模块验证对象，则退回到 case/suite，并给出补齐建议
- 系统场景默认不是完成前必过项
- 测试管线默认属于更高阶段门槛

skill 测试必须至少覆盖：

- 静态索引读取
- 对象解析
- wiki 回退
- 命令生成
- registry 对象存在性校验

其中“skill 引用链正确性”必须独立测试，且具备明确失败阶段和细粒度 `failureCode`。

## 结果、错误与定位契约

每个失败结果必须可回溯到：

- suite
- case
- 所属 module verification
- 所属 system scenario
- 所属 pipeline

标准定位粒度为：

- `caseId`
- `sourceFile`
- `line`
- `column`

若底层拿不到 `line/column`，允许降级，但必须补齐：

- `failureCode`
- `parserHint`
- `logPath`

`failureCode` 为正式机器字段，采用分层命名，粒度可细；不要求单独提供面向人的 wiki 页面。

每个失败结果都必须给出 `humanHint`，格式固定为：

- 失败对象
- 原因
- 下一步建议

生成责任为双层：

- 运行器优先提供具体提示
- reporting 层在缺失时兜底

下一步建议必须引用：

- 正式对象 ID
- 正式 `run test ...` 命令
- 或明确日志路径

## 一次性切换方案

本设计采用一次性切换策略：

1. 同时引入新的对象层、registry、静态索引和 wiki 入口
2. 同时改造 `run test` 语义
3. 同时让 TUI、harness、skill 消费新对象模型
4. 同时下线 `run verify` 作为正式入口

旧结构最多只保留必要隐藏兼容，不再出现在正式文档、TUI 和 skill 中。

## 成功标准

当以下条件同时满足时，本设计视为落地成功：

- `wiki/06-测试验证/` 成为项目级唯一正式入口
- `.codex` 通用 skill 可以稳定通过静态索引 + wiki 回退消费项目测试知识
- `run test` 成为唯一公开入口，并覆盖 suite、module、system、pipeline、registry 子域
- `module/system/pipeline` 成为正式注册对象，可被 TUI、CLI、harness、skill 和结果系统稳定引用
- 重要验证有正式升格规则与例外机制
- 结果具备完整归属链、失败编码、降级定位和 `humanHint`

## 下一步

基于本设计文档进入实现计划编写，明确：

- 目录与 schema 迁移步骤
- `run test` 命令与 TUI 改造顺序
- registry、静态索引和一致性检查落地步骤
- skill 引用链和错误定位契约验证步骤
