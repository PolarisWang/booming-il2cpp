# HybridCLR 对齐的 AOT 与热更开发计划 Brainstorm v1.01

Date: 2026-04-14 23:34:39 +08:00
Status: approved-for-roadmap

## 1. 背景

当前仓库已经完成一轮 subject、benchmark、solution 模式和测试入口收口，但 AOT 与热更的长期主线仍有两个根本问题没有完全统一：

- 当前 `NativeAotEmitter` 仍偏向最小子集验证，离“完整 typed-IL 翻译能力”还有明显距离。
- 如果未来热更能力要贴近 HybridCLR，那么单纯沿着“native-first opcode 补齐”继续走，会和后续混合执行、supplemental metadata、bridge/thunk 需求发生结构性冲突。

同时，`subjects/` 虽然已经大幅收敛，但历史语法点、proof/benchmark 组织方式、reference bundle 边界和 capability taxonomy 还需要一轮正式冻结。

## 2. 关键事实

- 未来热更路线已经明确要求贴近 HybridCLR，而不是纯解释器演示方案。
- `subject` 源码本身已经是 solution 模式，不应再回退成分散脚本或字符串配置驱动。
- 历史语法点需要机械拆到单文件级别，方便能力映射、定位和迁移，但不应该反向导致顶层 subject 再次碎片化。
- proof / benchmark 不应继续依赖大量 `validation/`、`scenario/`、`workload/` 目录协议，应该收敛到 `ChaosUnitTest` / `ChaosBenchmark` 这类 C# attribute 声明。
- 公共契约不应该继续扩大 string 协议面，优先使用带注释的枚举和稳定 numeric identity。

## 3. 方案对比

### 方案 A: 继续沿当前纯 AOT 路线扩 `NativeAotEmitter`

- 做法
  - 继续以当前 AOT emitter 为中心补更多 IL opcode、对象模型、调用模型、泛型和异常处理。
  - 热更部分作为后置补充能力，再额外挂 supplemental metadata 和 fallback。
- 优点
  - 短期看起来推进最快，能直接提升部分 native coverage。
  - 对现有 AOT 路径侵入最小。
- 缺点
  - 容易继续堆叠局部 patch，而不是建立共享执行契约。
  - 后续一旦接 HybridCLR 风格热更，dispatch、metadata、body availability、bridge/thunk 仍然要返工。
  - subject/test 侧仍然缺少稳定 owner mapping，能力覆盖表会继续漂移。

### 方案 B: AOT 核心增强 + 运行时 intrinsic/fallback

- 做法
  - 在 AOT 主线上引入更完整的 runtime intrinsic 和一部分 fallback。
  - 热更能力仍作为旁路系统接入，而不是架构中心。
- 优点
  - 比方案 A 更现实，能更快跨过部分对象/调用/泛型难点。
  - 允许逐步把 typed-il 转成更稳定的 AOT Core IR。
- 缺点
  - 仍然没有正面解决 HybridCLR 对齐下的统一 dispatch 和 supplemental metadata 问题。
  - 很容易形成“两套半系统”：AOT 一套、fallback 一套、热更再一套。

### 方案 C: HybridCLR 对齐的 AOT Host + Hybrid Execution + HotUpdate Runtime

- 做法
  - 把 `typed-il` 提升为共享契约。
  - 统一围绕 `AOT host runtime + hybrid execution layer + hot update assembly runtime` 设计。
  - AOT、解释执行、bridge/thunk、supplemental metadata 共用稳定 identity 和 method body availability 模型。
  - subject 按 solution owner pack 管理，proof / benchmark 通过 attribute 发现。
- 优点
  - 与未来热更方向一致，不会再把纯 AOT 路线做成死胡同。
  - 能把 typed-il、AOT、hot update、benchmark、subject taxonomy 放进同一套长期模型。
  - 方便定义清晰的 capability ledger、工程级 gate 和 mixed execution 证据链。
  - 方便把“已支持能力”和“明确不支持能力”同时纳入统一台账，而不是只记录成功路径。
- 缺点
  - Phase 0 设计冻结要求更高，不能直接跳进补功能。
  - 需要显式管理 body availability、stable id、reference bundle 和 bridge/thunk contract。

## 4. Subject 组织方案对比

### 方案 S1: 继续按历史 proof/benchmark 主题零散保留多个 subject

- 优点
  - 迁移阻力最低。
- 缺点
  - owner 不清楚。
  - 测试入口、dashboard、registry 很容易继续保留兼容层。

### 方案 S2: 彻底 generic 化，只保留一个超大 subject

- 优点
  - 顶层目录最少。
- 缺点
  - owner 边界模糊，工程级 archetype 和热更宿主差异会被揉平。
  - 后续平台差异、mixed execution、reference bundle 使用边界不易表达。

### 方案 S3: 保持少量 owner subject pack，内部按单文件 capability slice 机械拆分

- 做法
  - 顶层只保留少量 owner pack。
  - pack 内部使用 solution 模式，历史语法点按单文件切成 capability slice。
  - 多个 slice 按领域归并到合理的 project，而不是一条能力一个 project。
- 优点
  - 顶层结构干净。
  - 仍然保留工程级 archetype、热更宿主和 mixed execution 的自然边界。
  - 能直接对接 capability ledger 和 solution 级 gate。
- 缺点
  - 需要一轮机械搬迁和 capability owner 标注。

## 5. 推荐结论

推荐采用 `方案 C + 方案 S3`：

- 主架构选择 HybridCLR 对齐路线。
- `typed-il` 成为唯一共享执行契约。
- `subjects/` 保持少量 owner solution pack，内部按 capability slice 单文件化。
- proof / benchmark 全部转向 `ChaosUnitTest` / `ChaosBenchmark` 声明发现。
- 公共契约停止扩大 string 调度面，统一转向枚举、稳定 id 和注释完善的类型定义。
- Phase 0 必须补一份显式的 `AOT translation surface ledger` 与 `unsupported surface ledger`，否则不能宣称“已覆盖全部 AOT 翻译面”。

## 6. 进入 Roadmap 的理由

这个任务已经不适合直接写单次实现计划，原因是：

- 至少涉及 subject 结构、typed-il、dispatch、supplemental metadata、AOT codegen、benchmark/reporting 六个阶段。
- 多个 open question 需要在 Phase 0 先冻结，否则后续子任务会互相返工。
- 该任务会跨多次会话持续推进，必须用 roadmap 管理阶段边界和 child task 派生。
