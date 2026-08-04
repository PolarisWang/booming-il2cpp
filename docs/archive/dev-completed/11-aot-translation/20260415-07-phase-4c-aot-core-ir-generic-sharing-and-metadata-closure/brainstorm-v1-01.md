# Phase 4C Brainstorm v1.01

## 问题定义

- Phase 4B 已把 object/runtime surface 接入 `AotCoreIr`，但 generic-related contract 仍停留在 gap 记录层。
- HotUpdate 与 AOT 共享的 identity / body availability / metadata closure 还没有通过同一套 typed carrier 对齐。
- Native AOT 生成链已经拆成 `planner + emitter + Scriban`，Phase 4C 需要在这个结构上继续推进，而不是另开旁路。

## 本轮判断

- 先从 retained owner subject 中已有的 generic proof 反推 contract，是风险最低的路线。
- 先冻结最小 generic context / instantiation / sharing kind / metadata closure role，再做 lowering，比直接扩实现更稳。
- metadata closure 必须保留为 codegen/runtime contract，不应借测试框架胶水兜底。

## 本轮不做

- 不在本阶段一次性补齐 exception / ABI widening / 全量 generic optimization。
- 不把 `Chaos.TestFramework` 逻辑拉进 `AotCoreIr` / planner / emitter。
- 不回退为 emitter 里的 ad-hoc C++ 拼接。
