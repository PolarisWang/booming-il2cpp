# Brainstorm v1-03

## 背景

Batch 2 已完成 `src/native/engine-bridge/` 的最小原生落点与真实编译验证，但 Phase 3 仍缺少 managed 侧的正式入口：

- 还没有 `src/managed/Chaos.IL2CPP.EngineBinding/`
- 还没有 `[EngineExport]` / `[EngineCallback]` 这组 engine-facing 标注
- 还没有一个可重复执行的 binding generation baseline
- 还没有把“生成出来的 C++ wrapper 形状”沉淀成 compile-only smoke

如果继续把 engine-facing 规则散落在 `runtime_core` helper、subject proof 或临时脚本里，Phase 3 的 contract 仍然是不完整的。

## 候选方案

### 方案 A：直接把完整 managed/native 回调桥一起做完

- 优点
  - 一步到位，离 `EngineHostProof` 更近
- 缺点
  - scope 过大，会把 binding project、生成器、native callback 语义和 subject 验证压在同一批
  - 很容易把当前批次再次膨胀成半个 runtime 重构

### 方案 B：先做最小 `EngineBinding` 项目 + stub emitter + smoke baseline

- 内容
  - 新建 `Chaos.IL2CPP.EngineBinding`，只依赖 `Chaos.IL2CPP.Contracts`
  - 定义 `[EngineExport]` / `[EngineCallback]`
  - 提供 `EngineBindingStubEmitter.GenerateForInterface(Type)`，先支持 v0 最小 interface 约束
  - 新建 managed smoke 合同，输出稳定 C++ baseline
  - 新建 native compile-only smoke，验证 baseline 可编译
- 优点
  - 满足 Phase 3 的 binding generation 落点
  - 不提前把 `EngineHostProof` 和 HostEmbedding 改写拉进来
  - 生成结果可以被 repo 内单测和 native 编译同时锚定
- 缺点
  - 只交付 stub baseline，不宣称 managed invocation/full callback dispatch 已完全打通

### 方案 C：只加属性，不做 emitter 和 smoke baseline

- 优点
  - 改动最小
- 缺点
  - 不能满足 “从 `[EngineExport]` interface 自动生成 C++ wrapper” 的 Phase 3 验证目标
  - 仍然缺少可编译的 binding baseline

## 选择

选择方案 B。

## Batch 3 最小入口

- 新建 `src/managed/Chaos.IL2CPP.EngineBinding/`
- 定义：
  - `EngineExportAttribute`
  - `EngineCallbackAttribute`
  - `EngineBindingStubEmitter`
- emitter 只承担 v0 baseline：
  - 输入：带 `[EngineExport]` / `[EngineCallback]` 的 interface
  - 输出：包含 `#include "engine_bridge.h"` 的 compile-only C++ wrapper stub
  - 明确不在本批次承诺完整类型映射、managed body 调用、序列化桥接
- 新建 managed smoke 合同与 native compile-only smoke

## 事实边界

- 本批次可以声称：
  - `Chaos.IL2CPP.EngineBinding` 项目已建立
  - `[EngineExport]` / `[EngineCallback]` 已冻结为 managed 入口
  - 可以从 smoke interface 生成稳定 C++ wrapper baseline
  - baseline 可在 native 侧 compile-only 验证
- 本批次不能声称：
  - `EngineHostProof` 已完成
  - callback bridge 的 managed/native 往返调用已完全闭环
  - HostEmbedding 已迁移到正式 ownership protocol
