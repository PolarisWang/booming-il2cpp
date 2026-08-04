# Stage 5 Post-Proof Split Brainstorm

Date: 2026-04-06 12:48:45 +08:00
Status: concluded

## 任务意图

Stage 4 已经在 `Windows x64` reference host 上真实跑通 `HelloWorldObject` native proof，说明 reboot 主线第一次具备了“generated C++ -> native bootstrap -> 真实输出”的闭环证据。

proof 跑通之后，当前不再缺少“有没有第一条 spine”这个答案，而是缺少“下一轮先扩什么、怎么扩、按什么顺序扩”的答案。

Stage 5 的任务不是立即实现某一个 follow-on feature，而是把 post-proof follow-up 从抽象想法压缩成一条新的、可持续推进的路线。

## Stage 5 必须守住的边界

- Stage 4 的 `HelloWorldObject` Windows reference proof 继续作为固定基线，不因为 follow-on 样例推进而降级。
- 生产代码仍然必须保持 sample-agnostic；任何 `GenericEcho` / `ReflectionLite` / `PInvokeLite` 的样例特化都只能留在 `tests/**`、`artifacts/**` 与文档中。
- 当前必须先决定“样例优先”还是“平台优先”，不能把两个方向混成同一轮推进。
- 若无特别强的反证，follow-on 应继续先留在 `Windows x64` reference 上推进，等语义路径稳定后再扩到 `macOS`。

## 备选方案

### 方案 A：样例优先，继续留在 `Windows x64` reference

- 做法：先在现有 Stage 4 已跑通的 `Windows x64` native proof spine 上，按 `GenericEcho -> ReflectionLite -> PInvokeLite` 的顺序推进 follow-on 样例；等这条样例递进路径稳定后，再考虑 `macOS` parity。
- 优点：
  - 最大限度复用 Stage 4 已经打通的构建、proof host、gate 与排障经验。
  - 每次只扩一个主维度，问题定位仍集中在 managed semantics / metadata / bootstrap / codegen 之间。
  - 和早期路线里对 follow-on 样例的优先级判断保持一致。
- 缺点：
  - `macOS` reference desktop 的真实 parity 会继续后移。
  - 会让 post-proof 继续在 Windows 上停留更长时间。

### 方案 B：平台优先，先补 `macOS` parity

- 做法：保持 `HelloWorldObject` 语义不变，先把 Stage 4 的 proof spine 扩到 `macOS` reference host，然后再回头做 `GenericEcho` / `ReflectionLite` / `PInvokeLite`。
- 优点：
  - 更快恢复 `Windows + macOS` 双 reference desktop 的叙事完整性。
  - 平台工具链与 host 差异会更早暴露。
- 缺点：
  - 会把当前 blocker 从“下一条语义递进样例”换成“另一条平台构建/链接/宿主差异线”。
  - 还没来得及验证 follow-on 样例语义，就先增加一整层 host/platform 变量。

### 方案 C：样例扩张与平台扩张并行

- 做法：一边在 Windows 上推进 `GenericEcho` / `ReflectionLite` / `PInvokeLite`，一边同步铺 `macOS` parity。
- 优点：
  - 表面上推进最快。
  - 里程碑看起来更完整。
- 缺点：
  - 这是最容易重新回到旧大 roadmap 体量的路径。
  - 一旦出问题，很难判断是样例语义、runtime/bootstrap、还是平台/工具链导致。

## 当前推荐

- 采用 **方案 A**。

## 额外判断

Stage 5 不适合直接进入单个实现 `plan`，而是应该成为新的子 `roadmap`：

- follow-on 至少天然分成四段：`GenericEcho`、`ReflectionLite`、`PInvokeLite`、`macOS parity`。
- 这些阶段共享 Stage 4 proof spine，但扩张的能力层次和风险类型并不相同。
- 若现在直接写一个大实现 plan，很容易再次回到“多个语义面和多个平台面一起推进”的旧问题。

因此，Stage 5 的下一步应是：

1. 写出正式的 `design-v1-01.md`。
2. 落地 Stage 5 自己的 `roadmap-v1-01.md`。
3. 在 Stage 5 roadmap 下优先派生 Stage 5A `GenericEcho Windows Reference` 子任务。
