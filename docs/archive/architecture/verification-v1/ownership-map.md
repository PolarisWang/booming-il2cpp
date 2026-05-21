# Verification V1 权责图

## 目标

本文档把 `Verification V1` 的“权责图审核”落成可执行口径。它只回答：

- 哪些 lane 是固定主线
- 每条 lane 负责什么、不负责什么
- closure 在进入 job 生成前必须完成哪些审核
- 哪些问题应该直接判成 `blocked`

详细对象定义与状态语义以 [`spec.md`](./spec.md) 和 [`schema.md`](./schema.md) 为准。

## 1. 固定主线

`Verification V1` 固定审核六条主线：

| Lane | 核心问题 | 典型输入 | 典型输出 |
| --- | --- | --- | --- |
| `aot` | 哪些 capability 在 AOT 语义上必须闭合 | closure scope、capability base、route requirement | AOT obligation、平台限制、AOT blocker |
| `il2cpp` | managed -> native 转译后是否仍满足语义与 contract | codegen profile、native route、binding contract | native semantic/contract evidence |
| `codegen` | 生成边界是否清晰、关键产物是否可追溯 | emitter/template/model/stub candidate | codegen review evidence、stub refs |
| `contracts` | collection/schema/manifest/ABI/host contract 是否稳定 | schema、manifest、binding、loader contract | contract evidence、schema diff、ABI check |
| `loader` | load/bind/activate/rollback 生命周期是否闭合 | package、loader mode、host contract | lifecycle evidence、packaging/lifecycle blocker |
| `test_governance` | closure、gate、formal object、归并口径是否正确 | closure kind、required gates、master rules | authority report、governance blocker |

## 2. Lane 边界

### 2.1 `aot`

负责：

- capability 是否要求 `aot` 路由
- AOT 适用的平台、宿主、设备 profile 边界
- AOT 未实现、被策略禁止、或 scope 暂不覆盖时的 blocker 分类

不负责：

- 具体 native codegen 文件长什么样
- benchmark 采样细节

### 2.2 `il2cpp`

负责：

- managed -> native 路由是否可生成、可链接、可执行
- native 层语义是否与 managed claim 对齐
- generic dispatch、bridge、reflection carrier 等 IL2CPP 特有闭合问题

不负责：

- closure scope 是否应该包含该能力
- UI 投影怎么展示

### 2.3 `codegen`

负责：

- 模板/模型/发射器边界是否合理
- 是否存在重复生成逻辑或平行路径
- 哪些生成产物需要留下 formal 存根
- 存根能否与 owner subject 的 managed source 对照

不负责：

- 把所有中间生成物都长期归档
- 替代 `contracts` 去解释 schema

### 2.4 `contracts`

负责：

- collection schema
- native dispatch manifest
- hotupdate binding manifest
- ABI/binding/host contract
- formal object contract 与 projection contract

不负责：

- 物理设备调度
- perf 阈值本身

### 2.5 `loader`

负责：

- load/bind/activate/unload/rollback 的生命周期闭合
- package topology 与 loader mode 绑定
- lifecycle 与 packaging evidence 的最小要求

不负责：

- managed-only method 的语义断言
- UI 表头 tooltip

### 2.6 `test_governance`

负责：

- closure template 与 scope overlay
- obligation/evidence/master/report 的 formal 口径
- 状态语义与 reason/blocker 规则
- latest/master/reports 的正式路径和 merge key

不负责：

- 替执行器决定设备分配
- 替业务能力定义具体 benchmark 指标

## 3. Owner Subject

当前 canonical owner subjects 仍固定为：

| Owner Subject | 主要负责面 |
| --- | --- |
| `SolutionCorePack` | 基础语义、核心 AOT/IL2CPP、核心 proof/benchmark、通用 codegen 基线 |
| `MixedExecutionFeaturePack` | mixed execution、interpreter、桥接行为与跨路由差异验证 |
| `HotUpdateHostPack` | host/patch/contracts/loader/hotupdate lifecycle 与 packaging |

owner subject 的责任是：

- 持有 capability 的正式验证资产
- 持有与该 capability 对应的 codegen 存根
- 为 formal stage report 提供可引用的 source refs

owner subject 不等于所有执行都必须在该 subject 内发生；但 formal authority 最终回收于 owner subject。

## 4. 路由视角的权责分解

### 4.1 `managed`

mandatory lanes 通常为：

- `contracts`
- `test_governance`

按能力不同可追加：

- `codegen`
- `aot`

### 4.2 `aot`

mandatory lanes 通常为：

- `aot`
- `il2cpp`
- `contracts`
- `codegen`
- `test_governance`

### 4.3 `hotupdate`

mandatory lanes 通常为：

- `aot`
- `il2cpp`
- `contracts`
- `loader`
- `codegen`
- `test_governance`

是否全部 mandatory 取决于 closure scope，但凡命中该路线，缺少对应 lane 解释都应视为 authority 缺口。

## 5. 权责图审核清单

进入 planner job 生成前，必须完成以下审核：

1. `ownerSubjectId` 是否唯一
2. `closureKind`、`scopeCode`、`requiredRoutes` 是否冻结
3. 命中的 route 对应哪些 mandatory lanes
4. 每个 mandatory lane 的输入 contract 与输出 evidence 是什么
5. `requiredEvidenceKinds` 是否都有消费者与生产者
6. formal 输出路径是否已固定到 `verification/` 与 `codegen-stubs/`
7. 是否仍存在未关闭的 open question

## 6. 直接阻断条件

以下情况不应进入执行阶段，应直接形成 `blocked`：

- capability 没有 owner subject
- route 命中后没有 mandatory lane
- mandatory lane 没有 input contract
- mandatory lane 没有 formal output contract
- closure scope 未冻结
- 关键 evidence kind 没有 execution contract
- codegen 存根没有 owner 落点
- formal merge key/path key 无法稳定确定

## 7. 权责图产物

权责图审核最少要留下三类正式产物：

- `authority` 阶段报告
- obligation claim 上的 `mandatoryLanes`
- blocker/ref 列表

如果权责图审核通过但没有留下上述 formal 痕迹，视为审核未完成。

## 8. 与下游文档的关系

如果你已经明确知道：

- 这次 closure 要证明什么
- 谁负责
- formal 对象长什么样

下一步应转去看：

- [`schema.md`](./schema.md)
- [`projection-and-reporting.md`](./projection-and-reporting.md)
- [`cutover-and-rollout.md`](./cutover-and-rollout.md)
