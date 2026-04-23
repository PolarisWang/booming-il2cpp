# Verification V1 投影与报告

## 目标

本文档定义 `Verification V1` 中 formal source 如何投影为：

- `latest/master/reports`
- `testing-inventory`
- `benchmark`
- 主页固定视图

它的重点不是字段定义，而是“哪些结果应该怎么被人看到”。

## 1. Formal Source 层级

正式层固定只有三类 source：

| Source | 用途 | 是否覆盖更新 |
| --- | --- | --- |
| `latest/` | 当前 closure 的最新正式快照 | 是 |
| `master/` | 按 stable key 归并后的总表 | 是 |
| `reports/` | 分 closure 和分阶段的可读报告 | 是 |

固定路径如下：

```text
docs/testing-inventory/verification/
  latest/
  master/
  reports/
```

辅助正式 source：

```text
subjects/<OwnerPack>/verification/codegen-stubs/
```

`artifacts/` 不是 formal source。

## 2. 首页固定视图

主页默认固定展示以下视图：

| 视图 | 主问题 | 主来源 |
| --- | --- | --- |
| `Closure Strip` | 现在在收什么口、闭合到哪里 | `closure-snapshot` / `closure-master` |
| `Responsibility View` | 谁负责、谁在挡路 | `result-master` + `authority` report |
| `Route Topology View` | 哪条 route 断了 | `result-master` + `stage-master` |
| `Platform-Device Matrix` | 哪个平台/设备 profile 还没闭合 | `stage-master` + perf/semantic evidence |
| `Detail` | 具体 evidence、blocker、source refs 是什么 | `evidence` + stage reports + stub refs |

展示顺序固定，除 `Detail` 外都不默认展示 scenario 和物理 device。

## 3. `latest` 的职责

`latest` 只回答一个问题：当前 closure 归并后的正式结论是什么。

它不承担：

- 历史保留
- 调试过程回放
- run 级比对

典型使用者：

- 当前 closure 的 dashboard
- 当前 closure 的阶段报告入口
- 当前 closure 的投影视图生成器

## 4. `master` 的职责

`master` 只回答一个问题：系统按稳定 key 归并后的“当前总表”是什么。

它的设计原则是：

- 不按 `run-id` 膨胀
- 不把 `actualDeviceId` 放进主键
- 不接受临时执行态
- 只认 formal merge key

典型使用者：

- capability 总表
- unit test / benchmark inventory
- platform/device profile matrix
- release 和 platform gate 汇总

## 5. `reports` 的职责

`reports` 面向阅读，不面向归并。它负责把 formal 结果解释成人类可读报告。

每个 closure 至少包含：

- `summary.md`
- `authority.md`
- `semantic.md`
- `contract.md`
- `lifecycle.md`
- `packaging.md`
- `perf.md`

若某阶段本次 closure 不要求，仍应在报告中写明 `not_required`，而不是缺文件。

## 6. Stage Report 规则

阶段报告必须同时记录：

- 成功项
- 失败项
- 阻断项
- 缺失项

阶段报告不得只记录异常项。否则阅读者无法判断“是真的没问题，还是只是没记成功项”。

阶段报告至少要能回答：

- 这一阶段覆盖了哪些 obligation claim
- 通过了哪些 claim
- 卡在哪些 blocker
- 关联了哪些 formal source refs

## 7. `testing-inventory` 投影

`testing-inventory` 是 formal source 的投影视图，不再是 authority。

### 7.1 Unit Test Inventory

固定行粒度：

```text
一行一个方法 x route/stage
```

推荐来源：

- `result-master`
- `stage-master`
- `EvidenceClaimRecord`
- `StageReportRecord`

适合展示：

- 方法属于哪个 capability
- 当前 route/stage 的 support/verification 状态
- 是否有 blocker
- 最近一次 evidence 在哪里

### 7.2 Capability Inventory

固定行粒度：

```text
一行一个 capability x closure
```

推荐来源：

- `capability-master`
- `closure-master`
- `result-master`

适合展示：

- owner subject
- required routes/platforms/device profiles
- 当前 closure 是否闭合
- 哪个 lane 在阻断

## 8. `benchmark` 与 Benchmark Inventory 投影

benchmark 相关投影拆成两层：

- `docs/testing-inventory/benchmark-inventory.*`
  - 面向 verification 视角，看 declared benchmark 与 device profile 的 formal 状态。
- `docs/benchmark/*`
  - 面向 perf 视角，看设备、平台、subject、指标与阈值。

### 8.1 Benchmark Inventory

固定行粒度：

```text
一行一个 declared-benchmark x device-profile
```

这里优先展示：

- capability / owner
- route
- device profile
- support / verification state
- 阈值判定与最近 evidence

### 8.2 Benchmark Dashboard

这里优先展示：

- 平台与设备名
- subject 与 benchmark 方法
- 关键性能指标
- 阈值结果
- 历史基线引用

benchmark dashboard 可以展示物理设备名，但 formal merge 仍以 `deviceProfileCode` 为准。

## 9. Tooltip 与中文说明

投影视图中的表头 tooltip 统一来自 formal schema 的列元数据，不允许在 HTML 内写第二套口径。

每个列 tooltip 至少包含：

- 这列是什么
- 为什么要看它
- 这个值从哪里来
- 为空时代表什么

中文说明应优先服务“第一次打开这张表的人”，避免只有内部人员才能读懂。

## 10. 物理设备边界

formal 层与 projection 层对物理设备的处理固定如下：

- `deviceProfileCode` 进入 formal contract
- `actualDeviceId` 只进入 evidence payload 和 detail
- 首页矩阵不以物理设备为主键
- 设备名可用于阅读，但不能成为 formal merge key

## 11. 成功与失败的归档原则

formal 报告与投影都必须归档成功项和失败项。

以下做法都不允许：

- 只归档失败项
- 只在日志里保留成功项
- 用 `unsupported` 掩盖 `failed`
- 用缺文件代替 `not_required`

## 12. 与实现的接口

如果要开始落代码，投影实现应遵守以下顺序：

1. 先消费 `latest/master/reports`
2. 再生成 `testing-inventory` 和 `benchmark`
3. 最后切换 HTML/dashboard 的读取入口

不要反过来从页面需求推回 formal schema。
