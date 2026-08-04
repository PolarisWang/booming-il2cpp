# Phase 3 启动记录

时间：2026-04-14 11:04:15 +08:00

## 本阶段目标
- 收口 `HotUpdateHostPack` 的完整热更 workflow。
- 补齐 proof / benchmark 的 capability family / capability item / archetype / hot-update capability 元数据。
- 固化 `FullProjectHotUpdateSolution` 作为 canonical archetype，并让 declared metadata / registry / benchmark source 能正确消费。

## 首轮动作
- 先扩展 `test_declared_metadata_discovery.py` 做 RED。
- 再按 RED 结果补 proof/benchmark 元数据与缺失能力条目。
