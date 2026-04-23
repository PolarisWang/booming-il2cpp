# Verification V1 切换与落地

## 目标

本文档定义 `Verification V1` 的实施顺序、切换门槛和遗留退役规则。目标是：

- 不再沿用旧的 run-oriented formal 归档
- 不在实现过程中长期双写两套 authority
- 先稳定 formal source，再切 UI 与下游工具

## 1. 总体原则

### 1.1 先 formal，后 projection

必须先稳定：

- closure/claim/evidence formal 对象
- latest/master/reports 归并
- codegen stub formal 落点

再去切：

- `testing-inventory`
- `benchmark`
- HTML/dashboard

### 1.2 先 correctness，后 perf

任何阶段都不允许为了先出 perf 结果而跳过 correctness formal 化。

### 1.3 不长期双写

迁移期允许短暂桥接，但不允许把“新 formal source + 旧 formal source”同时长期当 authority。

### 1.4 自动执行默认开启

切换过程中仍以自动推进为默认路径。只有 authority 冲突、scope 未冻结、schema 不兼容等结构问题，才暂停确认。

## 2. 五阶段切换

### 2.1 Stage 1 `Authority Freeze`

目标：

- 固定 closure kind
- 固定 evidence kind
- 固定六条主线 lane
- 固定 merge key 与 path key

进入条件：

- 架构方向已确认
- formal 路径边界已确认

完成条件：

- [`spec.md`](./spec.md) 与 [`schema.md`](./schema.md) 可作为唯一 formal 设计基线
- 所有主线 lane 已有明确 owner 和 blocker 口径

### 2.2 Stage 2 `Graph Introduction`

目标：

- 引入 `Authority Graph`
- 引入 `ObligationClaim`
- 引入 `EvidenceClaim`
- 让 planner/preflight 能基于这些对象工作

进入条件：

- Stage 1 完成

完成条件：

- job 生成前可以先完成 authority/self-review
- 结构缺口可以在 execution 前被识别为 `blocked`

### 2.3 Stage 3 `Fact Normalization`

目标：

- managed/aot/hotupdate 执行器统一回写 execution facts
- facts 统一规范化为 `EvidenceRecord`
- stage reports 可从 formal evidence 生成

进入条件：

- claims 已稳定
- stage/report contract 已冻结

完成条件：

- `latest` 和 `reports` 能从同一套 evidence 生成
- 成功项、失败项、阻断项、缺失项都能进入 formal 报告

### 2.4 Stage 4 `Projection Cutover`

目标：

- `docs/testing-inventory/*` 改为只读 formal source
- `docs/benchmark/*` 改为只读 formal source
- 主页视图改为只读 projection generator 的输出

进入条件：

- `latest/master/reports` 已稳定
- 列 tooltip 元数据已可从 schema 派生

完成条件：

- inventory/benchmark/dashboard 不再自己推导 authority
- 设备、route、状态都从 formal source 派生

### 2.5 Stage 5 `Legacy Retirement`

目标：

- 停止旧 formal 路径写入
- 删除 run-id 风格的正式归档
- 删除重复 report/summary 逻辑

进入条件：

- Stage 4 完成
- 下游 UI 已切到新 projection

完成条件：

- 旧 formal 路径不再接收新数据
- 新系统成为唯一 formal source

## 3. 推荐实施顺序

实现时建议按以下顺序推进：

1. DTO / schema types
2. master merge
3. stage report generator
4. codegen stub materializer
5. projection generator
6. inventory/benchmark/dashboard cutover
7. legacy cleanup

这是推荐顺序，不建议调整。

## 4. 每阶段的阻断条件

### 4.1 阻断而非继续补丁的情况

以下情况应直接阻断当前阶段，不要靠局部补丁继续推进：

- owner subject 无法唯一确定
- merge key 需要引入 `runId` 或 `actualDeviceId`
- formal 路径想写回 `artifacts/`
- `supportState` 与 `verificationState` 想合并成一列
- `failed / blocked / missing` 想折叠成同一状态
- codegen 存根想留在临时目录

### 4.2 需要用户确认的情况

只有以下情况需要暂停并确认：

- closure scope 变更
- authority 边界变更
- schema 破坏性改动
- 要删旧路径但仍有下游在消费

## 5. 遗留系统退役规则

旧系统退役时，必须保证：

- formal source 已完整覆盖成功与失败项
- inventory/benchmark 已完成读取切换
- codegen 关键存根已迁到 `subjects/*/verification/codegen-stubs`
- 临时目录中的中间产物不再被任何正式逻辑引用

以下做法不允许：

- 旧系统继续悄悄接收新写入
- 新旧两套 master 长期并存
- 页面一部分读新数据、一部分读旧数据且无说明

## 6. Cutover Gate

正式 cutover 前，至少要检查以下 gate：

1. `authority gate`
   - 六条主线 lane 已有明确审核路径。
2. `schema gate`
   - formal DTO、merge key、path key 已冻结。
3. `report gate`
   - stage reports 能同时表达 success/failure/blocked/missing。
4. `projection gate`
   - inventory/benchmark/dashboard 均可从 formal source 派生。
5. `archive gate`
   - latest/master/reports/codegen-stubs 落点正确，`artifacts/` 仅为临时目录。

## 7. 文档到实现的映射

开始实现时，推荐直接按下面的映射取入口：

| 实现问题 | 先看文档 |
| --- | --- |
| formal 对象和字段怎么定义 | [`schema.md`](./schema.md) |
| 哪个 lane 负责、为什么会 blocked | [`ownership-map.md`](./ownership-map.md) |
| inventory/benchmark 应该怎么派生 | [`projection-and-reporting.md`](./projection-and-reporting.md) |
| 什么时候能删旧路径 | [`cutover-and-rollout.md`](./cutover-and-rollout.md) |

## 8. 最终完成判据

只有以下条件全部满足，才视为 `Verification V1` 文档与切换口径完整：

- 架构规则有主规格
- formal schema 有独立文档
- 六条主线权责图有独立文档
- projection/reporting 有独立文档
- cutover/retirement 有独立文档

达到这一步后，文档层面的最小完整包就算补齐，后续应转入实现而不是继续加文档。
