# Phase 3 启动记录

时间：2026-04-15 03:37:41 +08:00

## 启动前置条件

- Phase 2 已完成，typed shared contract 与 public string compatibility cleanup 已收口。
- `tests/unit/compatibility` 全套通过，说明进入 Phase 3 前的 contract 基线稳定。

## 首刀范围

- 先补 `HotUpdateAssemblyLoader`、`SupplementalMetadataRegistry`、`RuntimeManager` integration 的 RED 测试。
- 然后把 package 读取 / assembly image 装载 / metadata registry 激活状态从 `RuntimeManager` 临时逻辑中拆出来，建立最小 runtime foundation。
