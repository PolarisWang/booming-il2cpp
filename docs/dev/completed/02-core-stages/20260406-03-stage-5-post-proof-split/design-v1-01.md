# Stage 5 Post-Proof Split Design

Date: 2026-04-06 12:48:45 +08:00
Status: ready-for-roadmap

## 设计目标

把 Stage 4 已经跑通的 `HelloWorldObject` native reference proof，转化为下一轮稳定的 follow-on 路线，而不是重新回到“平台扩张”和“语义扩张”同时并行的模糊状态。

## 设计边界

### 纳入

- Stage 4 完成后续的 follow-on 顺序裁决。
- `GenericEcho`、`ReflectionLite`、`PInvokeLite` 三个样例的推进顺序。
- `macOS` parity 进入时点。
- 下一层子任务的 ready/planned 边界。

### 不纳入

- 直接修改生产代码、runtime 或 codegen 实现。
- 直接创建 `GenericEcho` 的实现 plan。
- 在本轮同步补跑 `macOS` 真机/真宿主验证。
- 重新改写 Stage 4 的完成定义或 proof gate。

## 关键设计结论

1. Stage 5 继续保持 `roadmap` 形态，而不是直接写单个 `plan`。

原因：

- post-proof follow-up 至少包含四个阶段。
- 每个阶段扩张的能力边界不同，不能用一个大 plan 直接吞掉。
- 只有先把顺序和阶段边界写清，后续子任务的 `plan-v1-01.md` 才会稳定。

2. 立即下一条主线是 `Windows sample-first`，而不是 `macOS` parity。

原因：

- Stage 4 刚在 `Windows x64` reference host 上建立了最完整的真实 proof 证据。
- 继续留在同一个 host 上推进 follow-on，能最大限度复用已知可工作的 build/run/gate 路径。
- 平台扩张应该建立在样例递进已经稳定的基础上，而不是作为下一轮的第一个变量。

3. follow-on 样例顺序固定为 `GenericEcho -> ReflectionLite -> PInvokeLite`。

原因：

- `GenericEcho` 仍然属于 managed 调用与实例形态的继续扩张，是从 Stage 4 `HelloWorldObject` 往上长出的最自然下一跳。
- `ReflectionLite` 会引入更明显的 metadata/reflection 查询面，应建立在前一轮样例语义路径已经稳定之后。
- `PInvokeLite` 直接跨入 interop/native boundary，应放在 managed/runtime/bootstrap 路径再次稳定之后。

4. `macOS parity` 延后到 Windows follow-on trilogy 之后。

原因：

- 这能避免把“新样例语义”和“新平台宿主差异”混成同一轮 debug。
- 也能让 `macOS` parity 消费一套已经更成熟的 Windows follow-on 语义基线，而不是只消费最小 `HelloWorldObject`。

5. Stage 4 的 `HelloWorldObject` gate 继续作为 Stage 5 全程基线。

要求：

- 新的 Stage 5A/5B/5C 子任务只能在新增样例 proof/gate 的同时，继续保持 Stage 4 gate 不回退。
- Stage 4 proof spine 不能因为 follow-on 子任务而被“升级替换”成更复杂但更脆弱的入口。

6. `src/**` 必须继续保持 sample-agnostic。

要求：

- 不允许把 `GenericEcho` / `ReflectionLite` / `PInvokeLite` 的专有判断写进 `src/native/**` 或 `src/managed/**`。
- 样例特化只能体现在：
  - `tests/proof/**`
  - `tests/smoke/**`
  - `artifacts/proof/**`
  - 对应文档与 gate 说明

## 推荐的阶段划分

### Stage 5A：GenericEcho Windows Reference

- 目标：在 Stage 4 现有 Windows reference spine 上，验证第一条 follow-on 样例的 generic/object/call 扩张。

### Stage 5B：ReflectionLite Windows Reference

- 目标：在保持 Windows host 不变的前提下，引入最小 reflection / metadata 查询语义。

### Stage 5C：PInvokeLite Windows Reference

- 目标：在 Windows host 上引入基础 interop proof，同时保持前两轮样例和 Stage 4 gate 都不回退。

### Stage 5D：macOS Reference Parity

- 目标：把已经在 Windows 上稳定的 Stage 4 + Stage 5 trilogy 语义，逐步迁入 `macOS` reference host。

## 验证策略

1. Stage 5 这轮只做文档、状态与路线冻结，不跑新的实现命令。
2. 后续每个 Stage 5X 子任务都必须：
   - 保持 Stage 4 `HelloWorldObject` Windows gate 不回退。
   - 明确自己的新增样例 proof/gate。
   - 不把测试样例逻辑泄漏进 `src/**`。

## 设计结果

Stage 5 应作为新的 active 子 `roadmap` 继续推进。下一步不是直接实现 `ReflectionLite` 或 `macOS` parity，而是优先派生 Stage 5A `GenericEcho Windows Reference` 子任务。
