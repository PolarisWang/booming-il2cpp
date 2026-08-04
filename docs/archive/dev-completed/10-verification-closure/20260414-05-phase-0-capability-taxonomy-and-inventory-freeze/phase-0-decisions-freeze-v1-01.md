# Phase 0 Decisions Freeze v1.01

Date: 2026-04-14 10:12:00 +08:00

## 1. 目标

本文档汇总本轮已确认的 OQ2/OQ3/OQ4/OQ5，并给出工程落地口径。

## 2. OQ2 Support State 放置策略

最终选择：

- 混合模型

工程落地：

- support state 默认放在外部 `matrix/manifest/reporting`
- attribute 只放内在 metadata
- 极少数必须强绑定的特殊情况，允许局部覆盖，但不是默认路径

原因：

- support state 是平台/模式/产品边界判断，不是测试方法的本体属性
- 如果直接塞进 attribute，会导致同一测试在 native / hot-update / mixed 下重复声明

## 3. OQ3 Reference Bundle 范围

最终选择：

- medium `foundation bundle`

工程落地：

- 使用 repository-owned bundle
- 固定路径：`assets/reference-bundles/dotnet-foundation/`
- 目标是覆盖 canonical archetype 所需的代表性 corelib / BCL 闭包

原因：

- 最小包代表性不够
- 大包镜像会明显增加仓库体积与版本治理成本

## 4. OQ4 Benchmark 选择策略

最终选择：

- 全量 proof + selective benchmark

工程落地：

- 每个 capability item 至少有 proof
- benchmark 只针对高价值 runtime path
- benchmark 命名和展示必须映射到 capability family / archetype / mode

原因：

- 每个语法点都做 benchmark 成本过高且信噪比差
- 只做 proof 又会让性能视图失真

## 5. OQ5 Dashboard 主视图

最终选择：

- 双层视图

工程落地：

- 顶层汇总按 capability family
- 二级 drill-down 按 archetype / solution / case
- managed baseline 作为默认对比基线

原因：

- capability 视图适合看覆盖面
- archetype/case 视图适合排障与定位
- 双层结构能同时满足架构视角与执行视角

## 6. 结论

Phase 0 之后，这 4 个 OQ 不再重新打开。后续 Phase 1-6 只按本口径实现与验证。
