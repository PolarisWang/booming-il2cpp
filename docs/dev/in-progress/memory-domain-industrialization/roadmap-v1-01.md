# Memory Domain 工业化补齐 — Roadmap v1

> **面向执行 Agent:** 必须使用 dev-subagent-driven-development 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。所有子任务进入终态后，必须归档当前任务目录并同步父任务。

**目标：** 将 Memory Domain 子系统从 Phase 1 中期提升到完美级工业化标准：风险清零 + EventPipe 诊断 + 压力/soak 测试 + 基准基线 + Proof 测试。

**架构：** 基于现有三域分配模型和 IDomainHeap 抽象扩展，EventPipe 桥接沿用 ep_gc_bridge 模式，测试沿用 reflection_benchmark/stress 模式。

**技术栈：** native C++20, GoogleTest (benchmark/stress/proof), EventPipe

**架构审核模式：** normal（不涉及 AOT/codegen/contracts/loader 主线）

**结构告警重点：** memory_domain.cpp 增长后的文件职责；ep_memorydomain_bridge 是否与 ep_gc_bridge 保持模式一致

**AOT/IL2CPP/Test Governance Intake:**
- capabilityFamily: n/a（native-only 基础设施，无 managed API）
- capabilityItem: n/a
- ownerSubjectId: memory-domain
- proofRequired: true
- benchmarkRequired: true
- hotupdateImpact: Smoke
- formalVerificationObjects: [MemoryDomain, IDomainHeap, DomainUnloadResult, AllocationHeader]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（设计摘要已写入 STATUS.md）

**问题清零来源：** brainstorm-approved STATUS.md

**计划来源：** direct-brainstorm

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 1. 为什么需要 roadmap

Memory Domain 工业化涉及 6 个子任务，横跨 diagnostics（EventPipe 桥接）、runtime-core（domain 修复）、interpreter（泄漏修复）、testing（3 个新测试套件）四个子系统。子任务可按 batch 并行，必须跨多次会话持续推进。单一 plan 无法覆盖。

## 2. 范围边界

**覆盖：**
- EventPipe 域诊断桥接：DomainRegistered/DomainUnloaded/UsageLimitExceeded
- 压力/Soak 测试：高频注册/分配/卸载循环
- 基准测试：Push/Pop/AllocateTagged/FreeTagged/Reallocate 基线
- Proof 测试：DomainScope、跨域安全、卸载安全等关键合约
- 修复 R1（interpreter operator new 泄漏）+ R3（struct_ret_buf 泄漏）
- 修复 R2（CachedCallInfo 跨域）+ R5（TLS 栈溢出断言）+ R6（usage_limit 原子化）+ R7（g_domains tombstones）

**不覆盖：**
- 不重新设计 MemoryDomain 架构
- 不改 ABI 合约
- 不改 codegen 输出
- 不实现 ArenaHeap/PassthroughHeap

## 3. 子任务映射

| task_id | phase | purpose | depends_on | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|---------|------------|--------------|---------------|----------------|-----------------|
| M1 | Phase 1 | EventPipe 域诊断桥接 | — | ep_memorydomain_bridge.h/.cpp, ep_instance.cpp 注册, CMakeLists.txt | 3 个事件可用，build 通过 | src/native/diagnostics/ | 1 天 |
| M2 | Phase 2 | 压力/Soak 测试 | Phase 1 | memory_domain_stress_test.cpp, CMakeLists.txt | 24h no crash | testing/ | 1 天 |
| M3 | Phase 2 | 基准测试 | Phase 1 | memory_domain_benchmark.cpp, CMakeLists.txt | 基线可重复 | testing/ | 1 天 |
| M4 | Phase 2 | Proof 测试 | Phase 1 | memory_domain_proof_test.cpp, contract.json | 核心合约覆盖，build 通过 | testing/ | 1 天 |
| M5 | Phase 1 | Interpreter 域分配泄漏修复 | — | fast_dispatch.cpp, runtime_instantiation.cpp | R1+R3 清零，test pass | src/native/interpreter/ + runtime-core/ | 2 天 |
| M6 | Phase 1 | Domain 修复 | — | memory_domain.h/.cpp, interpreter_entry.cpp | R2+R5+R6+R7 清零，build 通过 | src/native/runtime-core/ + interpreter/ | 1 天 |

## 4. 依赖关系

```
batch-1 (并行):
  M1 (独立)
  M5 (独立)
  M6 (独立)

batch-2 (依赖 batch-1 完成):
  M2 (独立)
  M3 (独立)
  M4 (独立)
```

## 5. 风险

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| M5 改 fast_dispatch.cpp 热点路径可能引入性能退化 | 中 | 加性能断言，与 M3 benchmark 对比 |
| M6 改 g_domains 数据结构可能影响域查找正确性 | 中 | 验证全部现有测试通过 |
| parallel-batch 需要多终端执行 | 低 | 需要 3（batch-1）+ 2（batch-2）终端 |

## 6. 完成判定

所有子任务完成后进入收尾闭环：结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交。
