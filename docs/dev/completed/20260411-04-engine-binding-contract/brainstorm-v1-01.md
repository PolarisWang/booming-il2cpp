# Brainstorm v1-01

## 背景

现有仓库已经有一层 engine proof 能力，但它仍停留在 proof/helper 状态：

- `runtime_core.h/.cpp` 已提供：
  - `EngineLogWrite`
  - `CreateEngineObjectHandle` / `ResolveEngineObjectHandle`
  - `RegisterEngineLifecycleCallback` / `DispatchEngineLifecycleCallbacks`
  - `IsMainThreadLane`
- `EngineLogWriteLite` / `EngineObjectHandleLite` / `EngineLifecycleCallbackLite` 已通过 engine-proof pipeline 闭环
- `subject_reporting.py` 等 run/tooling 已具备 engine-proof summary 汇总面

缺口在于这些表面仍然没有被提升为正式 engine-facing contract：

- 没有 `contracts/engine/v0/`
- 没有 `Chaos.IL2CPP.EngineBinding`
- 没有独立 `src/native/engine-bridge/`
- `HostEmbeddingLite` 也还没有使用统一 ownership / callback 协议

## 候选方案

### 方案 A：直接实现完整 engine-bridge + EngineBinding

- 优点：
  - 一步到位
- 缺点：
  - scope 太大，会把 contract freeze、native bridge、binding generation 和 subject 扩展压在同一轮
  - 风险是沿用 proof helper 的隐式语义，事后再补 contract 文档

### 方案 B：先冻结 `contracts/engine/v0`，再逐层推进 bridge / binding

- 内容：
  - 先把 lifecycle / ownership / callback / thread model 写成正式 contract
  - 用 RED 测试锁定 header 名称、函数族和 thread 文档关键词
  - 明确哪些函数当前已有 runtime-core helper 支撑，哪些仍是 Phase 3 后续实现项
- 优点：
  - 与 roadmap 的“contract first”顺序一致
  - 可以立即把 proof helper 提升为正式边界
  - 便于后续 `engine-bridge` / `EngineBinding` 按 contract 落地
- 缺点：
  - 本轮只能完成 Batch 1，不能误报为整个 Phase 3 已闭环

### 方案 C：继续沿用 runtime-core helper，不单独建 contract

- 优点：
  - 当前改动最少
- 缺点：
  - 会把 proof helper 继续变成隐式协议
  - 后续 hot update / metadata supplement 仍没有可依赖的正式 engine-facing 边界

## 选择

选择方案 B。

## Batch 1 最小入口

- 新建 `contracts/engine/v0/`
- 新建四份正式文档/头文件：
  - `lifecycle.h`
  - `ownership.h`
  - `callback.h`
  - `thread-model.md`
- 新增专项 RED 测试，锁定：
  - 目录与文件存在
  - lifecycle / ownership / callback ABI 名称
  - thread model 关键词
- 明确事实边界：
  - 当前 runtime-core 已有 helper 只说明“现有 proof 有支撑点”
  - 不等于 `engine-bridge` / `EngineBinding` / `EngineHostProof` 已完成
