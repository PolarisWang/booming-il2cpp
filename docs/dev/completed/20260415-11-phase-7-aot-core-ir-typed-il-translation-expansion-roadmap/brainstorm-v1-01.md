# Phase 7 AOT Core IR Typed-IL Translation Expansion Brainstorm v1.01

Date: 2026-04-15 20:11:23 +08:00
Status: approved-for-roadmap

## 1. 背景

上一轮父 roadmap `20260414-37` 已经把 HybridCLR 对齐的 AOT / hot update 主链路推进到可归档状态，并且项目级验证恢复到：

- `python -m pytest -q`
- 结果：`699 passed, 40 skipped`

但这并不等于 AOT Core IR 已经“全面完成”。真实代码里仍存在三类明显缺口：

- Loader 侧仍有 opcode / metadata handle / signature shape 的硬失败。
- `AotCoreIrLowering` 与 `NativeAotLoweringPlanner` 仍以最小子集为主，尤其是 scalar carrier、ABI carrier、structured control flow、exception shape 仍偏窄。
- `NativeAotEmitter` 已进入 `planner + emitter + Scriban templates` 主链，但后续 widening 还没有形成按 capability 闭环的执行路线。

## 2. 目标

本 follow-up roadmap 要解决的是：

- 把尚未闭环的 typed-IL translation surface 继续扩大。
- 继续保持 `AotCoreIr -> planner -> emitter + Scriban` 的单向收口。
- 让新支持面都能映射回 owner subject、proof、benchmark、engineering gate，而不是只做底层实现。

## 3. 约束

- 不把 `Chaos.TestFramework` 代码混入 AOT Core IR / planner / emitter。
- 不为这轮扩容再发明新的 string protocol；对 subject / reporting / dispatch 面继续复用 Phase 0 的 compact identity / enum 路径。
- 新增 native codegen 逻辑必须继续走 Scriban 模板，不重新退回大段内联 C++ 拼接。
- 这轮不重开一条“通用 benchmark/AOT”大路线；重点仍是 typed-IL translation expansion。

## 4. 讨论过的推进方案

### 方案 A：按 capability 批次纵向推进，每个批次都跨过 loader -> IR -> planner -> emitter -> subject evidence

优点：

- 最接近真实交付结果，避免单层假闭环。
- 可以直接和 Phase 0 translation ledger 对齐。
- 每一批次都有明确 owner subject / proof / benchmark 落点。

缺点：

- 需要前置做一次更细的 gap freeze。
- 每个批次会同时涉及多个模块，执行成本高于单层修补。

### 方案 B：先把 Loader / metadata decode 补得尽可能全，再逐层往下推进

优点：

- 工程切分简单。
- 对 metadata / opcode 盘点很直接。

缺点：

- 最容易出现 loader 变绿但 AOT Core IR / planner 仍不支持的假进展。
- 无法及时从 subject 证据反推实际优先级。

### 方案 C：先从 subject proof / benchmark 缺口反推底层实现，按测试驱动推进

优点：

- 用户可见收益强。
- 对 owner subject closure 很友好。

缺点：

- 如果没有先冻结底层 gap，subject 会再次演化成“案例驱动 patchwork”。
- 容易为了让 proof 通过而临时扩展 string 协议或专用路径。

## 5. 结论

采用方案 A。

执行方式：

- 先做一次新的 gap freeze，把当前 loader / IR / planner / emitter 的未支持点映射成 capability batch。
- 再按 5 个执行阶段推进，每一阶段都要求：
  - 至少有一组新的 capability batch 在底层闭环；
  - 有对应 owner subject proof；
  - 高价值路径补 benchmark 或明确标注 benchmark deferred；
  - `unsupported` 与 `missing evidence` 仍能在 reporting / gate 中区分。

## 6. 本轮 roadmap 的核心判断

- 这不是“重新设计测试系统”，而是“继续扩大 AOT translation 能力，并让测试系统只承担证据输出”。
- 这不是“把所有 C# feature 一次性做完”，而是“基于真实热点做下一轮 typed-IL widening”。
- 这不是“把 AOT pipeline 和 TestFramework 混起来”，而是“严格分层，subject 只声明与验证，pipeline 只翻译与生成”。

## 7. 进入 roadmap 的理由

该任务满足 `roadmap` 条件：

- 需要多个顺序阶段。
- 需要多个相互独立但有依赖的 child task。
- 会跨多次会话持续推进。
- 当前还需要先确定阶段边界、退出标准和 task mapping，不能直接写成单个稳定 plan。
