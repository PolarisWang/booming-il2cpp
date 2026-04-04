# 统一测试框架设计

**日期：** 2026-04-03 16:35 +08:00

## 背景

当前仓库已经有统一入口 `run.*`，但测试域仍然存在三类割裂：

- 公开命令割裂：测试相关能力分散在 `run build ...`、`run test ...`、`run verify ...` 以及底层脚本里。
- 目录语义割裂：现有 `tests/contracts`、`tests/platform`、`tests/tooling`、`tests/reference-desktop` 与目标测试家族模型不一致。
- 结果契约割裂：日志路径、JSON 输出、执行进度、性能数据、红绿灯覆盖和批量运行语义都还没有一套稳定公共约定。

用户希望把所有单元测试及相关测试族的 build 入口、执行入口、日志、统计和性能数据统一到一套跨平台框架里，并继续复用现有 `run` 入口、TUI 和 Python 实现。

## 目标

建立一套统一测试框架，使仓库内所有测试族都通过 `run test ...` 暴露，并共享如下能力：

- 统一的公开命令模型、批量执行模型和主机兼容性规则。
- 统一的 suite manifest、catalog、adapter 和 case 发现机制。
- 统一的日志目录、事件流、摘要文件、suite 报告和退出码。
- 统一的进度统计、时间统计、红绿灯覆盖和性能测试数据模型。
- 统一的 harness / CI / 本地开发接入方式，继续复用现有 `run.*` 入口。

## 非目标

本次设计不包含以下内容：

- 不保留测试域的旧双轨接口；旧的测试型 `run build ...` / `run verify ...` 不作为长期兼容目标。
- 不接入外部测试结果服务、数据库或远端性能基线服务。
- 不把 `--stage report` 设计成离线重算旧结果；它仍然是当前 suite 的执行阶段之一。
- 不在第一版支持复杂表达式过滤器；只支持最小 tag 过滤。
- 不在第一版引入 JUnit/XML 等额外结果格式，先稳定 JSON 和文件布局。

## 已确认约束

以下约束已经在对齐过程中确认：

- 统一入口继续使用 Python 实现，薄包装仍然是 `run.ps1` / `run.sh` / `run.cmd`。
- 测试域统一收口到 `run test ...`；非测试产物构建继续保留在 `run build ...`。
- 公开最小执行单元是 suite，case 只作为内部发现单元、事件单元和报告单元。
- 所有 suite 至少有一个 case；没有天然 case 的 suite 由 adapter 生成稳定 synthetic case，例如 `main`。
- `--host` 只在 suite 或 family 需要 host 约束时才要求显式传入。
- 结果文件、事件流和控制台输出都需要稳定 schema，并为自动化消费保留向前兼容空间。
- 普通模式和 `--strict` 模式都需要明确的升级语义，不能依赖隐式约定。

## 公开命令模型

### 主语法

测试域公开命令统一为：

```text
run test <family> <suite> [options]
run test <family> all [options]
run test all [options]
```

其中：

- `<family>` 是公开测试家族名。
- `<suite>` 是 suite 标识，公开最小执行单元。
- `all` 用于当前集合内的批量执行。

### Stage 控制

测试执行统一通过 `--stage` 控制阶段：

```text
--stage all|build|run|report
```

约定如下：

- `all` 是默认值，执行完整测试生命周期。
- `build` 只构建测试输入及其声明依赖。
- `run` 执行测试；如果构建缺失或过期，可隐式补足构建，并标记 `implicitBuild=true`。
- `report` 执行到可产出完整 suite report 的最小必要流程，不是离线重算历史报告。

### Discovery 与过滤

测试发现入口统一为：

```text
run test list [family] [--host <host>] [--json]
run test list --invalid [--host <host>] [--json]
```

第一版过滤能力只支持最小 tag 过滤：

```text
--include-tag <tag>
--exclude-tag <tag>
```

规则如下：

- 过滤发生在 `valid + host-compatible` 的 suite 集合上。
- 匹配的是最终 tag 集合，即 `adapter base tags + manifest derived tags`。
- 多个 `--include-tag` 取 AND。
- 多个 `--exclude-tag` 取 OR 排除。

### 旧接口迁移规则

测试域旧接口不再长期保留：

- 测试型 `run build ...` 收口为 `run test ... --stage build`。
- 测试型 `run verify ...` 收口为 `run test workflow ...`。
- `run build ...` 只保留给非测试产物构建，不再承载测试 build。

## 测试家族与目录布局

### 公开家族

统一测试框架的公开测试家族为：

- `unit`
- `integration`
- `contract`
- `smoke`
- `gate`
- `workflow`
- `perf`

### 目录布局

测试目录统一整理为：

```text
tests/
  unit/
  integration/
  contract/
  smoke/
  gate/
  workflow/
  perf/
```

现有目录迁移方向为：

- `tests/contracts -> tests/contract`
- `tests/platform -> tests/gate`
- `tests/reference-desktop -> tests/workflow`
- `tests/tooling/run/* -> tests/integration/run/*` 为默认归类起点

其中 `tests/tooling/run/*` 里纯解析、纯函数类测试后续可再细拆回 `unit`，但第一版默认按 `integration` 处理，以贴近真实 `run` 入口行为。

### 批量入口范围

批量执行约定如下：

- `run test <family> all` 执行指定 family 下的全部 host-compatible valid suites。
- `run test all` 默认包含 `unit / integration / contract / smoke / gate`。
- `workflow / perf` 不进入默认 `run test all`，必须显式指定 family 或 suite。

### 主机兼容性

主机兼容性规则如下：

- 显式点名不兼容 suite 时，命令直接失败。
- 批量 `all` 时，自动过滤掉当前 host 不兼容的 suite，并把过滤结果写入 session / summary。
- host 不兼容不会伪造成 `skip`，避免污染真正的跳过语义。

## Catalog 与 Suite Manifest

### Manifest 位置

每个 suite 采用本地 manifest：

```text
tests/<family>/<suite-id>/suite.manifest.json
```

`family` 和 `suiteId` 由路径推导，不在 manifest 中重复声明。

### Manifest 内容边界

manifest 保留运行时元数据，不承载 case 列表。核心信息包括：

- `displayName`
- `tags`
- `host requirement`
- `parallelSafe`
- `supportedStages`
- `coverage policy`
- `harness policy`
- `fingerprint inputs`
- `runnerType`
- `runnerConfig`

### Catalog 扫描

catalog 只扫描固定测试根目录，不扫描全仓库。运行时索引缓存到：

```text
artifacts/tests/catalog/<host>/index.json
```

索引刷新采用 fingerprint 驱动，而不是简单时间戳。

### Invalid Suite 处理

invalid manifest 或 invalid suite 不会拖垮整个 catalog：

- `run test list` 仍返回有效 suite 与无效 suite 两部分。
- 批量执行时只运行 valid suites。
- invalid suites 会进入 warning 和 `summary.invalidSuites`。
- 普通模式下不中断批量运行；`--strict` 下升级为失败。

## Runner Adapter 模型

### 固定 Adapter 类型

第一版 adapter 按执行模型拆分为少量固定类型：

- `python-pytest`
- `dotnet-app`
- `native-cmake`
- `shell-script`
- `workflow`

不按 family 各做一套 adapter，避免重复实现 discovery、日志、时间统计和失败摘要。

### RunnerConfig 结构

`runnerConfig` 采用分层结构：

- 框架公共少量字段，例如 `workingDirectory`、`timeoutMs`、`buildCommand`、`runCommand`
- adapter 命名空间配置，例如 `pytest.*`、`dotnet.*`、`cmake.*`

这样既保留公共校验能力，又避免把 workflow / shell-script 等场景硬塞进一套过度刚性的统一字段。

### Case 发现

case 由 adapter 在 discovery 阶段发现，manifest 不枚举 case。规则如下：

- `caseId` 由 adapter 生成稳定 ASCII `kebab-case` 标识。
- adapter 还保留 `sourceId` 与 `sourceName`，用于结果追溯。
- 如发生 `caseId` 冲突，追加稳定短后缀。
- 未提供天然 case 的 suite 由 adapter 自动生成 synthetic case。

## 生命周期与执行语义

### 固定阶段

所有结果对象统一写出固定 `stageResults` 键集合：

- `build`
- `setup`
- `run`
- `report`

每个 stage 都带固定状态：

- `ok`
- `fail`
- `skipped`
- `not-requested`
- `aborted`

必要时附带 `implicit=true`，例如 `--stage run` 触发了隐式构建。

### Prepare / Build 协作

`run test ...` 与环境准备的关系为：

- `run test ...` 可自动补足缺失的声明式 prepare 步骤，并标记 `implicitPrepare=true`。
- `run prepare ...` 仍保留，作为显式预热、环境复用和 harness 协议入口。
- build 是否过期由 fingerprint 判断，而不是 mtime。

### 并行与失败策略

执行策略如下：

- 并行只允许在 suite 层发生。
- 默认 `--jobs 1`。
- 只有 `parallelSafe=true` 的 suite 才允许并发运行。
- suite 默认在 case 失败后继续跑完；如需 fail-fast，由 catalog 元数据显式声明。
- harness 生命周期默认按 suite 复用。
- harness 崩溃默认 fail-fast；只有被声明为 recoverable 的 suite 才允许自动重启一次并重试被中断 case。

## 指纹与构建缓存

### 构建产物位置

测试 build 缓存统一写入：

```text
artifacts/tests/build/<host>/<family>/<suite>/fingerprints/<fingerprint>/
```

并保留 `current` 指针指向当前成功构建版本。

### 指纹输入

指纹输入采用受控模型：

- 文件输入通过受控 input group 声明，例如 `sourceRoots`、`configFiles`、`toolchainFiles`
- 文件指纹基于归一化内容哈希
- 环境变量指纹由框架与 `runnerType` 共同定义 allow-list
- suite 只能从 allow-list 中选择所需环境变量，并声明 `required` 或 `optional`

全局硬忽略固定包含：

- `.git/`
- `artifacts/`
- `log/`
- 临时文件
- 编辑器缓存文件

suite 不允许把这些路径重新纳入指纹。

### 缓存回收

缓存回收采用 “保留最近 N 版 + TTL” 组合策略：

- 新 fingerprint 成功写入后再触发轻量清理
- 只在阈值超限时清理
- 清理失败只记 warning；在 `--strict` 下升级失败

## 日志、产物与运行目录

### 运行日志根目录

测试运行日志根目录固定为：

```text
artifacts/logs/tests/
```

每次运行落到：

```text
artifacts/logs/tests/<run-id>/
```

其中 `runId` 格式为：

```text
<local-timestamp>-<host>-<short-suffix>
```

JSON 内部时间统一使用 UTC ISO 8601。

### Latest 指针

`artifacts/logs/tests/latest/` 优先采用轻量指针：

- 支持的平台使用符号链接或目录联接
- 不支持时退化为轻量元数据文件指向最新 `runId`

不做真实目录复制，避免重复占用空间。

### 固定文件布局

每次运行固定写出以下文件：

```text
session.json
summary.json
console.log
events.jsonl
telemetry.json
suites/<suite-id>/report.json
suites/<suite-id>/stdout.log
suites/<suite-id>/stderr.log
```

规则如下：

- 控制台保留 ANSI 颜色。
- 文件日志剥离 ANSI。
- `console.log` 记录控制台主视图。
- suite 原始大输出保留在 `stdout.log` / `stderr.log`，不内联进 JSON 报告。

## JSON 结果契约

### Session 与 Summary

`session.json` 记录运行事实，不承载聚合结果。`summary.json` 是机器优先摘要，采用：

- session 级汇总
- `suiteResults[]` 索引

每个 suite 索引只带：

- `suiteId`
- `status`
- `stageResults`
- `caseCounts`
- `trafficLightCounts`
- `readyAtUtc`
- `startedAtUtc`
- `finishedAtUtc`
- `startupMs`
- `executionMs`
- `totalMs`
- `reportPath`

case 级详细结果不进入 `summary.json`。

### Suite Report

`suites/<suite-id>/report.json` 保留完整 suite 级追溯信息，包括：

- suite identity
- manifest / runtime snapshot
- `stageResults`
- `warnings`
- `artifacts`
- telemetry summary
- `caseResults[]`

`caseResults[]` 覆盖 `ok / fail / skip / aborted` 全量已进入执行的 case，并保留必要的失败或跳过摘要；更重的原始输出继续留在 `.log` 文件里。

### 通用状态模型

统一状态集合为：

- `finalStatus`: `ok | fail | aborted`
- `suite status`: `ok | fail | skip | aborted`
- `case status`: `ok | fail | skip | aborted`
- `stage status`: `ok | fail | skipped | not-requested | aborted`

说明如下：

- 已开始但未正常完成的 case 记 `aborted`
- 未开始的后续 case 不补伪结果，也不计入 `caseCounts.total`
- `skip` 保留给真正的跳过，不复用来表示 host 不兼容或 invalid manifest

### 计数模型

统一计数模型如下：

- `suiteCounts = total / ok / fail / skip / aborted`
- `caseCounts = total / ok / fail / skip / aborted`
- `trafficLightCounts.green = total / ok / fail / skip / aborted`
- `trafficLightCounts.red = total / ok / fail / skip / aborted`
- `trafficLightCounts.yellow = total / ok / fail / skip / aborted`

`caseCounts.total` 固定等于 `ok + fail + skip + aborted`。

### 时间统计

session / suite 固定输出：

- `readyAtUtc`
- `startedAtUtc`
- `finishedAtUtc`
- `startupMs`
- `executionMs`
- `totalMs`

其中：

- `readyAtUtc` 表示 build、环境准备、harness、健康检查完成，第一条 case 即将执行
- `startupMs` 统计从 session / suite 启动到 `readyAtUtc` 的耗时
- `executionMs` 统计真正执行测试体的耗时
- `totalMs` 统计完整运行耗时

`perf` family 再下钻到 case 级时间数据。

## 事件流契约

### 事件封套

`run test ... --json` 输出行分隔 JSON 事件流，事件统一封套为：

- `schemaVersion`
- `eventType`
- `timestampUtc`
- `runId`
- `suiteId`
- `stage`
- `status`
- `payload`

最终以 `final-summary` 事件结束。

### 事件类型

固定事件集为：

- `session-start`
- `stage-start`
- `stage-finish`
- `progress`
- `case-result`
- `warning`
- `artifact`
- `final-summary`

### Progress 事件

进度采用 work-unit 模型，而不是时间估算。suite 的 unit 固定来自：

- `build`
- `setup`
- `case:<id>`
- `teardown`
- `report`

session 总进度 = `completedUnits / totalUnits`。心跳重复发送相同百分比，并带：

- `heartbeat=true`
- `activeUnit`
- `elapsedMs`

控制台百分比显示为绿色。

### Artifact 事件

`artifact` 事件只在稳定产物第一次可用时发出，不跟踪每次写盘。payload 固定为：

- `artifactType`
- `path`
- `scope`
- `suiteId?`
- `stage`
- `availableAtUtc`

结果文件中的 `artifacts` 字段直接复用同一结构。

### Warning 事件

`warning` 事件 payload 固定为：

- `warningCode`
- `message`
- `scope`
- `suiteId?`
- `stage`
- `strictImpact`
- `details?`

其中 `strictImpact` 只允许：

- `none`
- `fail-in-strict`

### Final Summary 事件

`final-summary` 只输出中等粒度汇总，不内联全部 suite 明细。核心内容包括：

- `finalStatus`
- `exitCode`
- `strictMode`
- `startedAtUtc`
- `finishedAtUtc`
- `durationMs`
- `stageResults`
- `suiteCounts`
- `caseCounts`
- `trafficLightCounts`
- `artifacts`

## Strict 模式与退出码

### Strict 模式

`--strict` 的规则为：

- 只有 `strictImpact=fail-in-strict` 的 warning 会在 strict 下升级为失败
- 普通 warning 仍只作为提示
- invalid suite、catalog 局部失效、缓存清理失败、可恢复 harness 重试等都可通过该机制统一升级

不采用 “所有 warning 一律失败” 的粗暴策略。

### 退出码

测试框架退出码集合固定为：

- `0`: 成功
- `1`: 已进入执行，但 suite / case / stage 失败
- `2`: 请求、契约、manifest、catalog 或 strict 校验失败
- `3`: 框架内部或基础设施异常

退出码保持小而稳定，便于 harness / CI 可靠消费。

## 红绿灯覆盖模型

### 默认覆盖策略

默认强制红绿灯覆盖的 family 为：

- `unit`
- `integration`
- `contract`
- `smoke`
- `gate`

默认非强制的 family 为：

- `workflow`
- `perf`

单个 `workflow` suite 可显式开启强制覆盖，并附带 justification。

### 选择器规则

红绿灯赋值通过 manifest selector 完成，只允许受限结构化匹配：

- `caseIds`
- `sourceIds`
- `tags`
- `include`
- `exclude`

不支持正则或脚本。

规则如下：

- 每个 discovered case 必须恰好映射到 `green / red / yellow` 三者之一
- selector 重叠属于配置错误
- 未匹配 case 属于配置错误
- 红绿灯只表示覆盖类型，不改变 `ok / fail / skip / aborted` 的执行语义

例如红灯 case 如果负向断言命中，状态仍然是 `ok`。

### Tag 管线

tag 管线固定为：

1. adapter base tags
2. manifest derived tags
3. traffic-light selectors 消费最终 tag 集合

manifest 只能追加 derived tags，不能覆盖或删除 adapter 原始 tags。最终 tag 集合要做归一化、去重和排序。

## 性能测试模型

### 基本语义

`perf` family 默认负责采集性能数据，不自动成为硬门禁。只有 suite manifest 配置了阈值或基线规则时，性能回退才判失败。

### 基线来源

第一版同时支持两种基线来源：

- manifest 内静态阈值
- 仓库内基线文件

不依赖外部历史数据库。

### 基线存放与更新

基线文件存放在各 perf suite 自己的目录下：

```text
tests/perf/<suite>/baselines/<host>.json
```

规则如下：

- 基线按 host 拆分
- 基线纳入仓库版本管理
- 只能通过显式命令更新，例如 `run test perf <suite> --update-baseline`
- 普通执行绝不自动改写基线

## TUI / Harness / CI 集成

### TUI 集成

统一测试框架必须接入现有 `run` TUI，而不是另起一套测试入口。TUI 需要直接消费：

- `run test list`
- suite 元数据
- 当前 host 可见性
- `progress` / `warning` / `artifact` / `final-summary` 事件

### Harness 集成

harness 流程统一通过入口指令拉起测试环境，采用：

1. `run bootstrap --yes --json`
2. `run prepare ... --json`
3. `run test ... --json`
4. `run clean --scope ... --json`

不再要求 harness 直接拼接底层脚本。

### Batch 运行与 Invalid Suite

批量执行时：

- 只跑 valid suites
- invalid suites 进入 warning 和 summary
- 普通模式不中断可运行测试
- `--strict` 下升级为失败

## 风险与取舍

### 主要收益

- 测试域对外协议统一，避免 `build/test/verify` 长期双轨。
- 结果契约统一，便于 TUI、harness、CI 和后续报表消费。
- 红绿灯覆盖、性能数据和进度语义都不再依赖各家脚本自定义。
- 目录与 family 对齐后，后续扩展新 suite 的心智成本更低。

### 主要风险

- 目录迁移和旧命令迁移会影响现有脚本，需要清晰的迁移窗口。
- manifest 与 adapter 引入了新的 schema 维护成本。
- 事件、日志、summary、report 四层契约同时落地时，需要严格防止字段漂移。
- `tests/tooling/run/*` 的 unit / integration 边界需要在落地时再做一次细化判断。

### 核心取舍

- 当前优先统一测试入口与结果契约，而不是一开始就做复杂的过滤语言或外部报表系统。
- 当前优先 suite 级并行，避免过早引入 case 级并行带来的共享资源复杂度。
- 当前优先稳定 JSON 契约和文件布局，不在第一版扩展更多结果格式。

## 成功标准

当以下条件全部满足时，本设计视为落地成功：

- 仓库内测试域只通过 `run test ...` 暴露公开接口。
- 目录结构、family 命名、catalog 和 manifest 达成统一约定。
- 本地开发、TUI、harness、CI 都能消费同一套 suite / case / artifact / warning / summary 语义。
- 日志统一进入 `artifacts/logs/tests/`，build 缓存统一进入 `artifacts/tests/build/`。
- 红绿灯覆盖、性能数据和进度百分比都有稳定、可追溯、可自动化消费的结果模型。

## 后续入口

本设计的下一步不是直接实现，而是基于本文件编写 implementation plan，拆分至少以下工作流：

1. `run test` 命令与旧测试接口迁移
2. 测试 family 目录迁移与 catalog / manifest 骨架
3. adapter 框架与 case 发现契约
4. 日志、事件流、summary / report 契约落地
5. 红绿灯覆盖与 perf baseline 机制落地
