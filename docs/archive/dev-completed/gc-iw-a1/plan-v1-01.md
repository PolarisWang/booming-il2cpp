# BGC-YoungGC 并发 segfault 修复计划

> **面向执行 Agent：** 使用 `dev-executing-plans` 执行本计划。步骤使用复选框跟踪。完成时必须先归档当前子任务目录，再同步父 roadmap。

**目标：** 修复 BGC 并发标记期间前台 Young GC 触发的 segfault（gc_bgc_smoke:TestBgcWithYoungGc），移除 SEH 隔离，使 BGC 与 Young GC 安全并发。

**架构：** BGC concurrent mark 期间，前台 GcYoungCollection() 在 STW safepoint 下执行 Cheney BFS 复制。崩溃根因疑为 BGC 的 mark stack 或 SATB 工作队列中的旧 nursery 地址在 Young GC 复制后被覆盖为 forwarding pointer，BGC 后续解引用时访问无效内存。修复方向为 BGC mark 期间禁止 Young GC，或 BGC mark 栈中追踪 nursery 引用安全转换。

**技术栈：** C++17, Windows x64, CMake

**架构审核模式：** normal（仅修改 gc_bgc.cpp/gc_young_collector.cpp，不触及 codegen/interpreter/loader 主线）

**结构告警重点：** 无 — 改动量小、边界清晰

**权责图审核主题：** n/a

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a
- capabilityItem: n/a
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: []

**设计文档：** n/a

**问题清零来源：** parent-roadmap（20260523-crag-gc-industrialization-sequel brainstorming cleared）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** wiki/03-功能模块/08-GC子系统/（若修复涉及 BGC-YoungGC 交互机制的记录，补充到 CRAG-GC-架构参考.md）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 1. 背景

`gc_bgc_smoke.cpp:TestBgcWithYoungGc`（Test 3/6）在 BGC concurrent mark 运行中循环触发 `GcYoungCollection()`。当前被 SEH `__try/__except` 隔离，crash 被捕获但根因未修复。

**已知信息：**
- Young GC 是 STW 操作（所有 mutator 在 safepoint 中停止）
- BGC 线程注册为 ManagedThread（Preemptive 模式），在 safepoint 时需确认
- 冲突路径：BGC 并发标记期间，Young GC 的 Cheney BFS 复制会将 nursery 对象搬移到 old-gen，并用 forwarding pointer（低位 tag bit=1）覆盖源对象 TypeInfo* 首字

**疑似的三个根因方向：**

| 方向 | 机制 | 可能性 |
|------|------|--------|
| R1: BGC mark stack 持有 nursery 旧地址 | BGC 并发期间读取的 nursery 指针在 Young GC 后变为 forwarding pointer，BGC 后续解引用崩溃 | **高** |
| R2: SATB 冻结协议与 Young GC 时序冲突 | Young GC 发生时 SATB freeze 正在进行，freeze 计数与实际线程状态不一致 | 中 |
| R3: BGC 与 Young GC 共享全局状态无保护 | `g_bgc_is_marking` 在 Young GC 路径中被检查但未与 BGC 互斥 | 中 |

## 2. 诊断步骤

### Step 1: 复现并收集崩溃信息

- [ ] 构建 Debug 配置：`cmake --preset debug`
- [ ] 单独运行 BgcWithYoungGc：直接运行测试验证 SEH 是否会崩溃
- [ ] 若崩溃可重现：移除 `__try/__except` 隔离，收集原始 crash dump
- [ ] 检查 crash 地址指向 nursery range / old-gen range / 空指针
- [ ] 通过 WinDBG/VS 分析 crash callstack，定位是 BGC 线程崩溃还是 mutator 线程崩溃

### Step 2: 定位根因

- [ ] 确定 crash 线程（BGC worker vs mutator）
- [ ] 若 BGC 线程崩溃：检查 mark stack deques 内容，确认是否指向已 forwarding 的 nursery 对象
- [ ] 若 mutator 线程崩溃：检查 SATB barrier 中 `g_bgc_is_marking` 检查与 Young GC 的竞态
- [ ] 验证 R1：在 BgcScavengeObject / ProcessChunk 中插入 assert 检查 nursery 地址是否已被 forwarding
- [ ] 验证 R2：检查 SATB freeze 计数 `satb_freeze_remaining_` 在 safepoint 期间的准确性
- [ ] 记录根因到 STATUS.md 的 notes

## 3. 修复方案

根据诊断结果选择对应方案：

### 方案 A（推荐）：BGC mark 期间禁止 Young GC

在 `GcYoungCollection()` 入口检查 `g_bgc_is_marking`：
- 若 BGC 正在标记：跳过 Young GC，直接进入 full GC fallback
  - 或：等待 BGC mark 阶段完成（进入 REMARK_NEEDED）后再执行 Young GC

改动点：
- `gc_young_collector.cpp` `GcYoungCollection()`: 入口检查 `g_bgc_is_marking`
- 当 marking 时：如果 nursery 真的满了，走 emergency allocation 到 old-gen 的 fallback

### 方案 B：BGC 标记期间序列化

在 `StartBgcCycle()` 中设置一个原子标志 `bgc_young_gc_blocked_`：
- `GcYoungCollection()` 检查该标志，若设置则：等待 BGC 完成当前 phase，或回退到 full GC
- BGC 离开 CONCURRENT_MARK 阶段时清除标志

### 方案 C：安全转换 nursery 指针

修改 BGC mark 路径 `ProcessChunk`，使其在解引用 nursery 地址前检查 forwarding pointer：
- 若对象已被 forwarding：用 promoted address 替换
- 若 nursery 已被 sweep：跳过该引用

**推荐顺序：** 先尝试方案 A（最简单，最小改动）。若诊断发现 R1 但方案 A 有语义问题，改用方案 B。方案 C 改动范围最大，仅作备选。

## 4. 验证步骤

### 单元测试验证

- [ ] `gc_bgc_smoke` 全部 6 tests PASS（移除 __try/__except 后无 crash）
- [ ] `gc_young_collector_test` 全部 tests PASS
- [ ] 全部已有 GC 单元测试 0 failures

### 压力测试验证

- [ ] `gc_bgc_stress` 长时间运行无 crash（至少 5 分钟）
- [ ] `gc_satb_stress` 长时间运行无 crash
- [ ] `gc_general_stress` 全场景 PASS

### 回归验证

- [ ] `gc_full_gc_test` PASS
- [ ] `gc_regression_benchmark` 无退化
- [ ] 全部 ~50 个 GC 测试 target 0 failures

## 5. 文件改动清单

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_young_collector.cpp` | `GcYoungCollection()` 入口添加 BGC marking 检查 + fallback |
| `src/native/runtime-core/gc/gc_bgc.h` | （可选）新增原子标志或辅助方法 |
| `testing/src/native/runtime-core/gc/gc_bgc_smoke_test.cpp` | 移除 SEH __try/__except 隔离 |

## 6. 风险与缓解

| 风险 | 影响 | 缓解 |
|------|------|------|
| 方案 A 导致 nursery 满时分配回退到 old-gen，内存效率下降 | 短期增 old-gen 分配量 | 仅在 BGC marking 期间生效，mark 结束后恢复正常 |
| 根因不是预期方向 | 诊断阶段延长 | 预留 3 天诊断时间；若根因不在 R1/R2/R3，上报父 roadmap 重新评估 |
| 移除 SEH 后修复不彻底，生产环境仍会崩溃 | 回归 | stress 长时间验证兜底；A2 验收门必须包含 ASAN 运行 |
