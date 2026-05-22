# HotUpdate 工业化补齐 — Roadmap v1

> **task_id**: hotupdate-industrialization
> **创建日期**: 2026-05-23
> **基于设计摘要**: STATUS.md (clearance confirmed)

## 目标

将 Chaos.IL2CPP 热更新系统从 ~50% 成熟度提升到工业化水平：HybridCLR 商业版级别的执行性能 + Mono 级别的调试/诊断工具链，覆盖全部 11 项差距项（R1-R11）和解释器 12 项工业化差距（I-01~I-12）。

## 范围边界

- 覆盖：热更新包加载/卸载、方法替换、VTable 同步、解释器执行、调试诊断、测试验证、跨平台基础
- 覆盖：寄存器式 IR 改造、元数据统一、OSR 升级策略、COM interop
- 覆盖：Managed 侧 AutoBridge 扩展、静态变量隔离
- 不覆盖：iOS 合规签名体系（独立需求）
- 不覆盖：Unity 编辑器集成（非本项目范围）
- 不覆盖：完整 AppDomain 级别的代码隔离（Mono 模式，与 AOT 架构冲突）

## 非目标

- 不追求 Mono 级别的 JIT 编译性能（AOT 架构差异，非热更新系统问题）
- 不与 Unity IL2CPP 的 hot-reload 对标（不同架构体系）
- 不做通用调试器（只聚焦热更新代码的调试支持）

## 阶段列表

### Phase 1：架构地基（预估 2 月）

**Goal:** 统一元数据查询路径，消除 AOT/热更新元数据双分体系；完成跨平台基础改造，为后续所有阶段提供稳定的架构地基。

**Exit Criteria:**
- MetadataRegistry 实现并通过测试：AOT 注册 + 热更新注册 + 跨模块复合查询
- 所有已有的 hotupdate 测试用例在 MetadataRegistry 集成后仍全部通过
- std::atomic 替换完成，无 MSVC 内置函数残留
- ARM64（QEMU 模拟或 Windows ARM64 设备）上 hotupdate 测试套件全部通过

**Deliverables:**
- `src/native/runtime-core/metadata/metadata_registry.h/.cpp`
- `contracts/native/v0/metadata_registry.h`（统一元数据查询接口）
- AOT bootstrap 修改（注册到 MetadataRegistry）
- .patchdata 加载适配（注册到 MetadataRegistry）
- `hotpatch_table.cpp` 原子操作标准化
- ARM64 验证报告

**Dependencies:** 无

**Resolved Decisions:**
- 方案 A：统一注册表 + composite key + 统一哈希表
- module_id = 0 为 AOT root，>0 为热更新
- HotpatchNameRegistry 作为 MetadataRegistry 的子组件

**Watch Items:**
- AOT bootstrap 修改可能影响全系统回归 — 测试套件必须全部通过
- ARM64 环境不可用时降级为 QEMU 验证

### Phase 2：性能飞跃（预估 3-4 月）

**Goal:** 全新寄存器式 IR VM + codegen RegisterIREmitter，将解释器性能从 ~200-290x vs AOT 降低到接近 HybridCLR 商业版水平。

**Exit Criteria:**
- 寄存器式 VM 覆盖全部 54 个 opcode（单元测试逐指令覆盖）
- 新 VM + 旧 VM 可切换运行（降级路径清晰）
- codegen RegisterIREmitter 能生成正确的寄存器式 IR
- 性能测试：单次 hotpatch 调用 ≤ 100ns（从 ~500ns 降低 5x）
- OSR 升级逻辑可用：方法执行 1000 次后自动升级到 T4 AOT 代码
- OSR 峰值性能：升级后性能损失 ≤ 10%（vs 原生 T4 路径）

**Deliverables:**
- `src/native/interpreter/register_vm.h/.cpp`
- `src/managed/Chaos.IL2CPP.Generator/Emission/RegisterIREmitter.cs`
- `contracts/native/v0/register_ir.h`（寄存器式 IR 指令定义）
- `src/native/runtime-core/osr_trigger.h/.cpp`
- `.patchdata` reg_ir 段正式启用（v2 已预留格式）
- 性能基准报告（vs 旧栈式 VM、vs HybridCLR 参考数据）

**Dependencies:**
- C3 MetadataRegistry（Phase 1）— VM 需要统一元数据做类型/方法解析
- C6/C7 跨平台基础（Phase 1）— 无平台特定代码侵入

**Resolved Decisions:**
- 方案 A1：一步到位全新寄存器式 VM，不做两阶段过渡
- 16 字节固定长度指令格式（与 .patchdata v2 reg_ir 段一致）
- OSR 条件：1000 calls + 1s interval + 5ms T4 编译预算

**Watch Items:**
- 寄存器式 VM 开发周期长（~3 月），Phase 2 中期无独立交付
- 寄存器分配器质量对最终性能有决定影响 — 可能需要多轮迭代
- OSR 触发逻辑可能引入微抖动 — 需要 benchmark 验证

### Phase 3：体验+质量（预估 2-3 月）

**Goal:** 基础调试器、EventPipe/profiling 工具链、测试覆盖全面补齐。三个子工作互不依赖，并行推进。

**Exit Criteria:**
- 调试器：支持断点设置/命中/恢复 + 单步执行 + 栈帧展开 + 变量查看
- EventPipe：热更新生命周期事件可被诊断工具消费
- Profiling：per-method 调用计数 + CPU 耗时 + GC 分配量可导出
- 增量更新：.patchdata.delta 格式可用
- GC 交互测试：解释器分配→GC触发→root 正确枚举 验证通过
- 多线程 stress 24h：无死锁/无 data race/无 segfault
- Opcode 自动化覆盖验证工具化通过
- Benchmark 回归基线接入项目回归体系

**Deliverables:**
- `src/managed/Chaos.IL2CPP.Debugger/DebugAdapter.cs`（DAP 实现）
- `src/native/diagnostics/debug_agent.h/.cpp`
- `src/native/diagnostics/hotupdate_events.h`（EventPipe 事件定义）
- `src/native/interpreter/register_vm_profiler.h`（VM 内计数器）
- `testing/src/native/hot-update/gc_interaction_test.cpp`
- `testing/src/native/hot-update/stress_test.cpp`
- `tools/opcode-coverage/`（opcode 覆盖验证工具）
- 增量更新格式设计文档

**Dependencies:**
- C3 MetadataRegistry（Phase 1）— 调试器依赖统一符号查询
- C1 寄存器式 VM（Phase 2）— 调试器依赖寄存器状态，profiler 依赖 VM 计数器

**Resolved Decisions:**
- 调试器使用 DAP 协议（不造轮子）
- 断点检查使用单条件分支（`g_debugger_attached` 标志 + 断点哈希表）
- Profiling 默认关闭，profile 构建启用
- 增量更新基于 .patchdata v2 reg_ir 段做 diff

**Watch Items:**
- DAP 协议实现的复杂度可能被低估 — Phase 3 只做"基础可用"
- 多线程 stress 可能暴露 Phase 1/2 未发现的竞争条件
- GC 交互测试需要 GC 侧配合暴露内部状态

### Phase 4：收尾对齐（预估 1 月）

**Goal:** Managed 侧补齐、静态变量隔离、COM Interop 支持。三个子工作互不依赖，并行推进。

**Exit Criteria:**
- AutoBridge 支持 20 类签名（含新增 15 类）
- 静态变量按 module_id 独立存储，卸载时完全回收
- COM Interop：Register-VM 中 ComImport 方法正确路由

**Deliverables:**
- `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs` 扩展
- `src/native/runtime-core/static_var_store.h/.cpp`
- Codegen 静态变量间接访问路径
- `src/native/interpreter/register_vm_com.cpp`（COM dispatch 集成）

**Dependencies:**
- C1 寄存器式 VM（Phase 2）— COM 路由在 VM 的 CallVirt 路径中
- 无其他 Phase 3 依赖（可与 Phase 3 并行或串行）

**Resolved Decisions:**
- AutoBridge 模板化：从手工签名类改为 `(return_type, param_types...)` 模板自动生成
- 静态变量隔离复用现有 MemoryDomain 架构

**Watch Items:**
- 无显著风险

## 子任务映射

| task_id | phase | status | purpose | depends_on | estimated_effort |
|---------|-------|--------|---------|------------|-----------------|
| C3 | Phase 1 | completed | MetadataRegistry 统一元数据注册表 | 无 | 3 周 |
| C6 | Phase 1 | planned | 跨平台原子操作抽象（std::atomic 替换） | 无 | 1 周 |
| C7 | Phase 1 | planned | ARM64 内存序验证与修复 | C6 | 1-2 周 |
| C1 | Phase 2 | planned | 寄存器式 IR 全新 VM + codegen emitter | C3 | 3 月 |
| C8 | Phase 2 | planned | OSR / Tier upgrade 激活策略 | C1 | 1 月 |
| C2 | Phase 3 | planned | 调试器 DAP 协议 + Runtime DebugAgent | C3, C1 | 2 月 |
| C4a | Phase 3 | planned | EventPipe 热更新诊断事件 | 无 | 1 周 |
| C4b | Phase 3 | planned | Profiling 计数器（per-method） | C1 | 2 周 |
| C4c | Phase 3 | planned | 增量更新 .patchdata.delta 格式 | C1 | 4 周 |
| C5a | Phase 3 | planned | 寄存器 VM 单元测试（逐指令覆盖） | C1 | 1 周 |
| C5b | Phase 3 | planned | GC 交互测试 | C1 | 2 周 |
| C5c | Phase 3 | planned | 多线程 stress 测试 | C1 | 2 周 |
| C5d | Phase 3 | planned | Soak 24h 测试 + opcode 覆盖工具 + benchmark 回归 | C1 | 3 周 |
| C9 | Phase 4 | planned | Managed AutoBridge 20 类签名扩展 | 无 | 2 周 |
| C10 | Phase 4 | planned | 静态变量 Assembly 级隔离 | 无 | 3 周 |
| C11 | Phase 4 | planned | COM Interop 基础支持 | C1 | 2 周 |

## 依赖

```
Phase 1              Phase 2              Phase 3              Phase 4
┌──────┐            ┌──────┐            ┌──────┐            ┌──────┐
│ C3   │────┐       │ C1   │────┐       │ C2   │←──C3,C1   │ C9   │
└──────┘    │       └──────┘    │       └──────┘            └──────┘
┌──────┐    ├──→    ┌──────┐    ├──→    ┌──────┐            ┌──────┐
│ C6   │────┘       │ C8   │←───┘       │ C4a  │            │ C10  │
└──────┘            └──────┘            └──────┘            └──────┘
┌──────┐                               ┌──────┐            ┌──────┐
│ C7   │←──C6                          │ C4b  │←──C1       │ C11  │←──C1
└──────┘                               └──────┘            └──────┘
                                       ┌──────┐
                                       │ C4c  │←──C1
                                       └──────┘
                                       ┌──────┐
                                       │ C5a-d│←──C1
                                       └──────┘
```

## 风险

| 风险 | 等级 | 触发条件 | 缓解措施 |
|------|------|---------|---------|
| C3 修改 AOT bootstrap 影响全系统回归 | 高 | Phase 1 集成后测试失败 | 测试套件必须 100% 通过后才进入 Phase 2 |
| C1 寄存器式 VM 开发周期超出预期 | 中 | 3 个月后 VM 仍未达到可用状态 | 设置月里程碑检查点；第 1 月必须完成指令集+解码器 |
| 寄存器分配器性能不达预期 | 中 | 寄存器式 VM 性能仍远高于 100ns | 预留 Phase 3/4 做寄存器分配优化迭代 |
| 调试器 DAP 实现复杂度被低估 | 中 | Phase 3 调试器超出"基础可用"边界 | 严格 scope：不做 IDE 集成，不做混合栈帧 |
| ARM64 环境不可用 | 低 | 无 ARM64 硬件或 QEMU 不稳定 | 降级为 Phase 3/4 处理，Phase 1 只做代码级正确 |

## 备选路径

- **Phase 1 C3 若回退**：采用方案 B（统一查询 Facade），不改 AOT bootstrap。但代价是调试器/OSR 仍有两套查询路径。
- **Phase 2 C1 若回退**：采用方案 B（翻译器路径），codegen 不变，PatchLoader 做栈式→寄存器式翻译。性能上限较低但开发周期缩短。
- **Phase 3 优先级调整**：如果调试器开发受阻，可将 C4c（增量更新）和 C5（测试）提前交付。

## 当前建议推进顺序

```
串行推进，按 Phase 1 → Phase 2 → Phase 3 → Phase 4 顺序。
Phase 1 内部: C3 + C6 并行 → C7 (依赖 C6)
Phase 3 内部: C2 / C4a+b+c / C5a-d 三线并行
Phase 4 内部: C9 / C10 / C11 三线并行
```

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `recommended_next_child`: C3
