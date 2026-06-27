# 08-GC子系统 INDEX

> CRAG GC 分配器、收集器、诊断相关知识。

## 文档

| 文档 | 说明 |
|------|------|
| [`CRAG-GC-ETW诊断事件.md`](./CRAG-GC-ETW%E8%AF%8A%E6%96%AD%E4%BA%8B%E4%BB%B6.md) | GC ETW provider 身份、8 个事件定义、平台约束、使用方式。 |
| [`CRAG-GC-Phase3-并发能力.md`](./CRAG-GC-Phase3-%E5%B9%B6%E5%8F%91%E8%83%BD%E5%8A%9B.md) | Phase 3: Server GC 模式、Thread Hijacking、BGC-YoungGC 协调暂停、NUMA 感知空闲页回收、根 Slot 写屏障。 |
| **架构参考（外部）** | [`06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md`](../06-il2cpp%E6%A0%B8%E5%BF%83%E6%9E%B6%E6%9E%84/01-%E7%BF%BB%E8%AF%91%E7%AE%A1%E7%BA%BF/24-CRAG-GC%E6%9E%B6%E6%9E%84%E5%8F%82%E8%80%83.md) — 3 代设计、Region 框架、并行标记、BGC 生命周期、压缩、NUMA、性能特征。 |

## 测试覆盖

GC 子系统有 **50+ 测试目标**，包括 38+ 单元测试、14+ 压力测试和多个 benchmark：

| 类型 | 数量 | 说明 |
|------|------|------|
| 单元测试 | 38+ | `test_gc_*` 覆盖分配、布局、region、card table、GC scheduler、young/old/gen1 收集、finalizer、safepoint、并行标记、domain unload、demotion 等 |
| 压力测试 | 14+ | `test_gc_stress` (17 场景)、BGC stress、SATB stress、delegate stress、多线程 general stress、soak (72h) |
| Benchmark | 3 | gen1 暂停时间、吞吐量、回归套件 |
| 运行时 | — | `CHAOS_IL2CPP_GC_STRESS=1` 环境变量，每 1000 次分配触发完整 GC |
