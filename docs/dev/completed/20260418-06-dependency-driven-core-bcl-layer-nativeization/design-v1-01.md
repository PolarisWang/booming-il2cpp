# Dependency-Driven Core BCL Layer Nativeization 设计 v1.01

## 1. 背景

`20260418-05` 已经证明 `System.Private.CoreLib` 的首批 semantic-family execution entry 可以接到真实 proof / conformance / engineering workload / benchmark authority，并稳定完成实跑。下一阶段不能继续停留在 corelib 内部，而是要把已认证 foundation 能力按依赖关系扩展到核心 `System.*` DLL。

## 2. 目标

- 基于 `.NET 8` / `.NET 10` foundation pack 资产与依赖图，构建核心 `System.*` DLL 的分层 nativeization 路线。
- 为每一层定义明确的 native proof、benchmark 与 monitor signals。
- 先形成 machine-readable layer plan，再选择首层 DLL 做真实 execution entry 与闭环验证。

## 3. 非目标

- 本任务不直接收口复杂 BCL、外部 DLL、hotupdate consumer 或 `.NET 10` delta release gates。
- 本任务不回退到“按业务 DLL 倒推 foundation”的路线。
- 本任务不接受“层内大多数 DLL 可用”的模糊口径；每层都必须明确哪些 DLL 已 certified、哪些仍 blocked。

## 4. 设计原则

- 依赖优先：层次由 foundation pack 依赖关系决定，而不是按手工直觉分组。
- 认证优先：每层都要先定义 proof / benchmark authority，再开始实跑。
- 版本同构：`.NET 8` 与 `.NET 10` 使用同构 layer plan 结构，允许内容差异，但不允许流程分叉。
- 语义化命名：新任务、设计、计划、产物继续使用语义化命名，不再新增 `Phase{x}` 风格名称。

## 5. 产物

- `layer-plan-v1-01.json`
- `assembly-nativeization-plan/`
- `proof-benchmark-lane-v1-01.json`
- `STATUS.md`
- `plan-v1-01.md`
- `notes/progress-v1-01.md`

## 6. 执行思路

1. 读取 foundation pack catalog、contract lane、corelib substrate 与 semantic-family 闭环结果。
2. 为核心 `System.*` DLL 生成依赖分层结果，并区分：
   - 已被 corelib 认证覆盖的基础层
   - 可以立即推进的首层 DLL
   - 仍缺 authority / proof / benchmark 的阻塞层
3. 为每层建立可验证的 native proof / benchmark 入口与 monitor 指标。
4. 选择首层 DLL 进入真实 execution entry 计划，作为后续 child task 或当前任务的实跑起点。

## 7. 完成判断

- `layer-plan-v1-01.json` 能解释核心 `System.*` DLL 的层次与依赖。
- 每层都有明确 proof / benchmark authority，而不是事后补测。
- 首层 DLL 已经具备可执行的下一步计划与 formal verification 对象。
