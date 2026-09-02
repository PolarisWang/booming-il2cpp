# Native 循环依赖 / 反向依赖消除方案设计

文件：`docs/dev/in-progress/native-industrialization/dep-cleanup-design.md`
日期：2026-09-02
状态：设计草案（未实施，不修改任何源码）
目标分层（红线）：

| 层 | 目录 | 允许依赖 |
|---|---|---|
| common | `src/native/common` | 无 |
| pal | `src/native/pal` | 无 |
| runtime-core | `src/native/runtime-core` | common, pal, contracts |
| jit / interpreter / bootstrap | `src/native/{jit,interpreter,bootstrap}` | runtime-core, common, pal, contracts |
| contracts | `contracts/native/v0` | 无（纯 C 头、无实现） |

四层红线：ATG（AOT 生成代码）/ Codegen / TPG（Tier PGO）/ Python 工具链不得因 native 模块改分层而破坏。任何解耦方案必须**不改变 ABI 结构体布局**、**不改变 extern "C" 桥符号名**、**不改变 generated 代码的 includes**。

---

## 1. 当前反向依赖清单（审计结果）

### 1.0 结论前置：真正的"反向依赖"只有 6 处 + 1 处 CMake 配置

`runtime-core` 反向 `#include` 到 jit / interpreter / bootstrap 的实际文件与符号如下（`../jit/`、`../interpreter/` 相对 include）：

| # | 文件（runtime-core 侧） | include | 用到的符号 | 调用点 | 依赖本质 |
|---|---|---|---|---|---|
| R1 | `src/native/runtime-core/CMakeLists.txt:301` | `target_link_libraries(... PUBLIC chaos_interpreter)` | （链接级）让 runtime-core 传递链接 interpreter | UNIX 分支 only | 功能型错误依赖 |
| R2 | `src/native/runtime-core/core/runtime_init.cpp:10` | `../jit/jit_seh.h` | `chaos::il2cpp::jit::RegisterJitSehHandler()` | `runtime_init.cpp:73` | 功能调用（一次性初始化） |
| R3 | `src/native/runtime-core/thread_state.cpp:22` | `../jit/jit_seh.h` | `chaos::il2cpp::jit::FindNativeCodeByAddress(void*)` 返回 `const JitMethod*` | `thread_state.cpp:992` | **数据访问**（GC 需要 JIT 元数据） |
| R4 | `src/native/runtime-core/thread_state.cpp:23` | `../jit/jit_method.h` | 结构体 `JitMethod` 字段：`slot_map_data` `gc_point_map_data` `code` `rbp_to_rsp_offset` | `thread_state.cpp:1006,1009,1020,1026` | **数据访问**（读元数据字段） |
| R5 | `src/native/runtime-core/patch_loader.cpp:3,4` | `"interpreter_entry.h"` + `<interpreter_vm.h>` | `InterpreterEntryDirect/Fast`, `PatchMethodLowerIR`, `ReapplyInlining`, `EagerCompilePatchMethods`, `interpreter::IRMethod`, `interpreter::InterpreterValue` | `patch_loader.cpp` 多处 | 功能+数据混合 |
| R6 | `src/native/runtime-core/gc_root_scanner.h` 已声明，但 `runtime-core` 侧**仅声明** `GcSetInterpFrameScanner/GcGetInterpFrameScanner` | `src/native/runtime-core/gc/gc_root_scanner.h` | 回调插槽，`interpreter/interp_frame_scanner.cpp:96` 反向注册 `GcSetInterpFrameScanner(GcScanInterpFrameChain)` | `thread_state.cpp:1039`（读 slot） | 已解耦（回调）——作为参考样板，非待办 |

> 注：`runtime-core` 内还有对 `bootstrap/bootstrap.h` 的 include（`GetCodegenBridgeV0`/`PeekBootstrapState` 等，见 `generic_context.cpp`、`instantiation_engine.cpp`、`metadata_interface.cpp`、`patch_loader.cpp`、`runtime_core.cpp`、`task_runner.cpp`、`threading_stubs.cpp`）。bootstrap **名义上是"上层"**，但实际它只是启动编排 + 代码生成桥的持有者。这是另一个方向的"语义倒置"，本任务聚焦 jit/interpreter 反向依赖，bootstrap 依赖单列在 5.4 作为后续观察项，不在本次范围内深挖。

### 1.1 linker 级：`chaos_runtime_core PUBLIC chaos_interpreter`（R1）

`runtime-core/CMakeLists.txt:300-304`：

```cmake
elseif(UNIX AND NOT APPLE AND NOT ANDROID)
    target_link_libraries(chaos_runtime_core PUBLIC chaos_interpreter)   # 反向
    target_link_libraries(chaos_runtime_core PUBLIC chaos_eventpipe)
    target_link_libraries(chaos_runtime_core PRIVATE numa)
endif()
```

**影响范围（PUBLIC 传播后果）**：`PUBLIC chaos_interpreter` 意味着：
- 任何 PUBLIC 链接 `chaos_runtime_core` 的目标（如 `chaos_bootstrap`、`chaos_interpreter`、`chaos_jit`、宿主 exe）都会被**隐含传递链接** `chaos_interpreter`，造成链接图出现 `runtime-core -> interpreter` 的环（因为 `interpreter` 本就 PUBLIC 链接 `runtime_core` 与 `chaos_jit`）。
- 该依赖**仅存在于 `UNIX AND NOT APPLE AND NOT ANDROID` 分支**（即 Linux）。Windows 分支（296-299 行）**没有**这条反向链接——Windows 构建反而是干净的分层。这强烈暗示 Linux 反向链接是"为了让某些符号在 Linux 静态链接下能被解析"的补丁式手段，而非设计意图。
- 由于是 STATIC 库，`PUBLIC` 反向链接的**实际危害主要是传播污染**：它把 `chaos_interpreter`（含 IRMethod 等）推给所有下游。一旦 R3~R5 源码级依赖被回调化，此链接即可降为 `PRIVATE` 或整条删除。

### 1.2 `runtime_init.cpp` -> `RegisterJitSehHandler()`（R2）

`runtime_init.cpp:73` `::chaos::il2cpp::jit::RegisterJitSehHandler();`。这是 T4 VEH/signal SEH 处理器的**一次性初始化**。`RegisterJitSehHandler` 实现在 jit 侧（`jit_seh.cpp:82`）。
注意：`bootstrap.cpp:197` 也调用了 `RegisterJitSehHandler()`，即**初始化责任已存在二次调用点**。`runtime_init` 与 bootstrap 谁先跑决定 SEH 处理器注册——这是"功能调用"，天然适合由"更高层/启动层"负责，而非 runtime-core 主动。

### 1.3 `thread_state.cpp` -> `FindNativeCodeByAddress`（R3）

`thread_state.cpp:992`（`GcScanAllThreadRoots` 第二阶段，T4 帧精确 GC 扫描）：

```cpp
const auto* nm = chaos::il2cpp::jit::FindNativeCodeByAddress(val);
if (nm == nullptr) continue;
if (nm->slot_map_data == nullptr) continue;
...
uint32_t rbpoff = nm->rbp_to_rsp_offset;
...
auto* sm = static_cast<const GcSlotMapV0*>(nm->slot_map_data);
const auto* point_map = static_cast<const GcPointMapV0*>(nm->gc_point_map_data);
```

- 用途：把栈上的返回地址映射到 JIT 方法的元数据（GcSlotMap / GcPointMap / code / rbp 偏移），做精确根扫描。
- **本质：只读数据访问**。runtime-core 只需要"给我 `JitMethod` 的 4 个只读字段"，不调用任何 JIT 行为、不触发编译。这是典型的"**低层（GC）需要高层（JIT）生成的元数据**"——信息流方向是 runtime-core 指向 jit 的"查询"，依赖方向却是 jit 在上。**完全可回调化**。

### 1.4 `thread_state.cpp` -> `JitMethod` 结构体（R4）

`thread_state.cpp` 依赖 `JitMethod` 的**内存布局**做解包：`slot_map_data` `gc_point_map_data` `code` `rbp_to_rsp_offset`。这些字段所指的 blob（`GcSlotMapV0`/`GcPointMapV0`）**已经定义在 contracts 层**（`contracts/native/v0/codegen_bridge.h`），因此 runtime-core 只需要拿到一个**不透明句柄** + 一个把该句柄解包成字段的**回调**即可，无需知道 `JitMethod` 完整布局。这属于 R3 的孪生问题，二者同解。

### 1.5 `patch_loader.cpp` -> interpreter（R5）

`patch_loader.cpp` 使用了一组 interpreter / hotpatch-dispatch 符号：
- `interpreter_entry.h`：`InterpreterEntryDirect`/`InterpreterEntryDirectFast`（dispatch 表中 hotpatch 后解释执行入口，通过 `hotpatch_dispatch.h` / `runtime_core.h` / `chaos_pch.h` 间接引用）、`PatchMethodLowerIR`、`ReapplyInlining`、`EagerCompilePatchMethods`。**这些函数实际定义在 interpreter 侧**（`interpreter_entry.cpp`、`interp_entry/entry_direct.cpp`、`eager_compile.cpp`），runtime-core 只是**声明并调用**。
- `interpreter_vm.h`：`interpreter::IRMethod` / `InterpreterValue` —— patch_loader 在 `DestroyPatchContext` 里 `delete static_cast<interpreter::IRMethod*>(m.cached_ir)`（patch_loader.cpp:396）、`assembly_manager.cpp`、`aot_core_ir_reader.h`、`instantiation_engine.h` 也引用 `interpreter::InterpreterValue`。
- **本质：功能调用（触发解释/IR lowering/内联/即时编译）+ 少量类型使用**。这是 R5 中是**最重**的一条，因为 `PatchMethod` 本身就在 runtime-core 定义，而它的 tier 状态机由 interpreter/jit 驱动。但注意：`tier_manager.h` 已经用 `SetTier3PromotionCallback`（interpreter 反向注册）成功解耦了 `PromoteToTier3/RebuildCallCacheForT3` —— **同一族的 function-call 依赖已有样板**。`DestroyPatchContext` 对 `IRMethod` 的 `delete` 则是**跨层 `delete` 不透明指针**，属于"释放高层分配的堆对象"——需要托管化的回调查看。

### 1.6 已解耦样板（不做、作为模式参照）

- `GcSetInterpFrameScanner`（gc_root_scanner）：interpreter 注册、GC 读取，`runtime-core` 只持有函数指针 slot，**零反向 include**。这是任务要求的方案C 的**现成范例**。
- `RegisterJitDemotionCallbacks`（jit_demotion.cpp）：jit_seh.cpp 反向注册 demote 回调，`runtime_core` 通过 slot 调用。**第二个方案C 范例**。
- `SetModuleRegisteredCallback` / `SetOriginalAotPtrCallback`（hotpatch_table）：同理。
- 结论：**本代码库已广泛、成功使用"runtime-core 持函数指针插槽、上层反向注册"的 方案C**。消除 R1~R5 不是引入新模式，而是把已打赢的仗扩展到其余点。

---

## 2. 依赖本质分类总结

| # | 本质 | 判定依据 | 可解耦性 |
|---|---|---|---|
| R1 | 链接级功能型错误依赖 | 仅 Linux；Windows 无此环 | 可删/降 PRIVATE |
| R2 | 功能调用（一次性初始化） | 调用 `RegisterJitSehHandler`，非数据 | 可回调/上移启动层 |
| R3 | 纯数据访问（只读查询） | GC 查询 T4 元数据，不触发行为 | **非常可解耦**（回调） |
| R4 | 纯数据访问（结构体布局） | 只读 4 个字段，blob 已在 contracts | **非常可解耦**（句柄+回调） |
| R5 | 功能依赖 + 少量类型使用 | 解释/IR lowering/内联/eager-JIT + `delete IRMethod` | 混合，可分步解耦 |
---
## 3. 解耦方案对照（A/B/C/D）

### 方案 A：接口注入（runtime-core 持抽象接口指针）
- 定义抽象接口（如 `IJitCodeRegistry`：`virtual const CodeRunInfo* FindByCodeAddress(const void*)`），runtime-core 持全局接口指针，bootstrap/jit 注入实现。
- 优点：类型安全、可扩展、测试可 mock。
- 缺点：需要对象生命周期管理；对当前"多为自由函数/函数指针"风格偏重；`GcSlotMap` 字段读取要收敛进接口返回的视图。**对 R3/R4/R5 的"释放 IRMethod"仍有残留**。
- 影响红线：不坏 ABI（接口内部）、不坏 extern "C" 桥；但引入 vtable 间接，GC 热路径（T4 扫描）有单次间接调用开销（可接受，因 GC 是低频）。

### 方案 B：依赖倒置 / 符号下沉到 contracts
- 把跨层共同依赖的符号下沉：`GcSlotMapV0`/`GcPointMapV0` 已在 contracts（已是事实）；`JitMethod` 的**消费侧视图**（`CodeRunInfoV0`）可下沉到 `contracts/native/v0/codegen_bridge.h`，作为 jit 结构体的**稳定 ABI 预览**。
- 优点：从根本上解除"jit 在上"的元数据依赖；contracts 是纯头，无环。
- 缺点：`JitMethod` 是 jit 内部堆结构（含 vtable/析构/call-site 表等 jitter 私货），不能整体下沉；只能下沉"只读视图"。需要 jit 把 `JitMethod` 布局与 `CodeRunInfoV0` 保持一致（或用 `reinterpret_cast` 别名），存在布局漂移风险。
- 影响红线：需要新增 contracts 头/结构，**保证不改变现有 ABI 结构体**；生成的 `.gc_slot_maps` 段格式不能动（`GcSlotMapV0` 已在 contracts，只读不改）。

### 方案 C：回调注册（推荐，本库既有模式）
- runtime-core 暴露注册点，jit/interpreter/bootstrap 注册回调。
- **R3/R4（FindNativeCodeByAddress / JitMethod）**：新增 `runtime_core/` 的 `jit_code_registry.h`（或复用 `jit_demotion.h` 家族风格），声明：
```cpp
struct JitCodeRunInfo {
    const void* code;              // entry
    const void* slot_map_data;     // -> GcSlotMapV0 (contracts)
    const void* gc_point_map_data; // -> GcPointMapV0 (contracts)
    const void* code_end;          // 用于范围判定（可选，配合 code_size）
    uint32_t    rbp_to_rsp_offset;
};
using FindCodeRunInfoFn = const JitCodeRunInfo* (*)(const void* address);
void RegisterJitCodeRegistry(FindCodeRunInfoFn find_code_run) noexcept;
const JitCodeRunInfo* FindCodeRunInfo(const void* address) noexcept; // 默认 nullptr
```
- jit 侧在 `RegisterJitSehHandler` 初始化时 `RegisterJitCodeRegistry([](const void* a){ return WrapJitCodeRunInfo(FindNativeCodeByAddress(a)); })`；或 bootstrap 统一注册。
- `thread_state.cpp` 用 `FindCodeRunInfo` 替换 `FindNativeCodeByAddress`，并 `static_cast<const GcSlotMapV0*>(info->slot_map_data)`（GcSlotMapV0 已在 runtime-core 可见）。`JitMethod` 结构体依赖被消除（不再 include `jit_method.h`）。
- **R2（RegisterJitSehHandler）**：从 `runtime_init.cpp` 移除直接调用，改为"由 bootstrap 在启动序列负责调用"（`bootstrap.cpp:197` 已调用）。若需保证在 `RuntimeInit` 内完成初始化，改为 runtime-core 暴露 `SetSehHandlerInitializerFn` 注册点，由 jit/bootstrap 注入一次性回调；runtime-core 不持有注册者身份。**最简做法是直接依赖现有 bootstrap 调用，删除 runtime_init.cpp:73，论证见 5.2**。
- **R5（patch_loader -> interpreter）**：按函数族拆分注册点：
  1. dispatch 入口 `InterpreterEntryDirect*`：已是**函数指针注入**（`HotpatchEntryV0::interrupt_ptr` 由 interpreter/jit 填 `&InterpreterEntryDirect`，见 `contracts/codegen_bridge.h:269` 注释）。runtime-core 不应再 static 引用之；如需 runtime-core 在构造 dispatch 表时拿入口，用注册点拿到函数指针。
  2. `PatchMethodLowerIR` / `ReapplyInlining` / `EagerCompilePatchMethods`：新增一组"IR 服务工作集"注册点（`SetPatchIrWorkfnsFn`），interpreter 反向注册（同在 `interp_frame_scanner.cpp` 或 `entry_direct.cpp` 初始化处）。线程安全与 tier 回调同款（store/load + nullptr 默认）。
  3. `DestroyPatchContext` 的 `delete static_cast<interpreter::IRMethod*>(m.cached_ir)`：把"释放策略"交给上层。新增 `JitIrFreeFn` 注册点：`using JitIrFreeFn = void (*)(void* cached_ir);` interpreter 注册 `[](void* p){ delete static_cast<interpreter::IRMethod*>(p); }`。runtime-core 不再 include `interpreter_vm.h`，只把 `cached_ir` 当 `void*` 传给释放回调。
  4. `asm/type` 层面的 `interpreter::InterpreterValue` / `IRInstruction` 字段使用（`assembly_manager.cpp`、`instantiation_engine.h`、`aot_core_ir_reader.h`）：若仅为指针/引用传递可不前置；若需读字段，抽给回调或下沉这些**纯数据 IR 结构**到 contracts（方案B 局部化），二者择一，见推荐。
- 影响红线：零新增函数调用间接在热路径可控；不改 extern "C" 桥；不改 generated 代码；不改 ABI 结构体。**最贴近现有已成功模式**。

### 方案 D：合理美化 / 豁免（能不解就不做）
- 对确实无法短期内解耦、且当前无实际危害的点，使用"文档化豁免 + include 白名单"。
- 适用：R1（Linux 链接环）在 Windows 本就干净，若评估纯 LOCALITY 影响为零可保留一天；R2 若确认 bootstrap 调用已覆盖则删 runtime-core 调用即可，无需注册点。
- 局限：R3/R4 是 GC 正确性依赖，**不能豁免**（指向清晰的上层数据，是分层红线的实打实破口）。R5 影响面大，长期豁免会固化环。

**对照矩阵：**

| | 方案A 接口注入 | 方案B 下沉 contracts | 方案C 回调注册（推荐） | 方案D 豁免 |
|---|---|---|---|---|
| 解耦 R3/R4 | 可（接口+视图） | 可（下沉视图） | **可（最优）** | 否 |
| 解耦 R5 | 可 | 部分（纯数据 IR） | **可（分族注册）** | 短期 |
| 解耦 R2 | 冗余 | 不适用 | **可** | 可（靠 bootstrap） |
| GC 热路径开销 | vtable 间接 | 无 | 无/函数指针 | - |
| 类型安全 | 高 | 高 | 中（函数指针+结构视图） | - |
| 实现量 | 中 | 中 | **低（复用既有模式）** | 最低 |
| 与本库风格契合 | 低 | 中 | **极高（已有3处样板）** | - |
---
## 4. 推荐方案（按依赖逐个）

| # | 推荐 | 方法 | 理由 |
|---|---|---|---|
| **R3+R4** | **方案C** | 新增 `RegisterJitCodeRegistry`/`FindCodeRunInfo` 回调 + `JitCodeRunInfo` 视图；jit 注册适配器把 `FindNativeCodeByAddress` 结果窄化为只读字段；删除 thread_state.cpp 对 `jit_seh.h`/`jit_method.h` 的 include | GC 只读查询、数据流单一方向、与 `jit_demotion` 完全同构；彻底断"GC->上层元数据"环 |
| **R2** | **方案D 收敛 + 轻度C** | 优先：删 `runtime_init.cpp:73`，确认 bootstrap.cpp:197 覆盖初始化（见 5.2 论证）。若必须在 `RuntimeInit` 内保证：改由 jit 反向注册 `SetSehHandlerInitializer` 一次性回调 | `RegisterJitSehHandler` 是初始化编排而非 runtime-core 职责；二次调用点已存在，无需 runtime-core 主动持有 |
| **R5-1 dispatch 入口** | **方案C（已是函数指针注入）** | 走 `HotpatchEntryV0::interrupt_ptr`（由 interpreter/jit 填 `&InterpreterEntryDirect`）；runtime-core 删除静态 `#include interpreter_entry.h` 引用，改用已注入指针 + 必要时注册点取地址 | dispatch 表本就用函数指针注入，`interrupt_ptr` 已是解耦事实 |
| **R5-2 IR 工作集** | **方案C** | 新增 `SetPatchIrWorkfns`（lower/inline/eager-jit 三回调），interpreter 反注册（`interp_frame_scanner.cpp` 初始化处） | 与 `SetTier3PromotionCallback`/`SetModuleRegisteredCallback` 同风格 |
| **R5-3 `delete IRMethod`** | **方案C** | 新增 `JitIrFreeFn` 注册点，interpreter 注册删除器；runtime-core 把 `cached_ir` 视为不透明 `void*`，删 `<interpreter_vm.h>` include（patch_loader.cpp:4） | 消除"低层 delete 高层对象"的环 |
| **R5-4 纯数据 IR 类型** | **方案B 局部** | `InterpreterValue`/`IRInstruction` 若仅引用传递则不前置；若 runtime-core 确需读字段，把**纯数据布局**抄入 `contracts/native/v0/interpreter_ir.h`（不改 `interpreter_vm.h`，只加只读 view）或抽回调读值 | 泛型 IR 是跨层数据，下沉契约可顺带服务 jit 与 codegen |
| **R1（CMake）** | **恢复分层次序** | 消除 `runtime_core PUBLIC chaos_interpreter`：把该 `PUBLIC` 降为 `PRIVATE` 或整条删除。前提是先完成 R3~R5 源码解耦，使 runtime-core 无需链接 interpreter 的符号（届时 interpreter 的 IR 符号不再被 runtime-core 直接引用） | linker 环是 R3~R5 的链接投影；源码层断环后链接层自然收敛（且 Windows 已无此环，证明可去） |

---
## 5. 执行顺序

分 3 阶段推进，每阶段可独立编译/测试/回退：

**阶段 1（先断 GC 环，正确性最关键）- R3 + R4**
1. 新增 `runtime-core/jit_code_registry.h/.cpp`（`RegisterJitCodeRegistry`/`FindCodeRunInfo` + `JitCodeRunInfo` 视图），默认 `FindCodeRunInfo=nullptr`。
2. jit 侧在 `RegisterJitSehHandler()`（或 jit 自己的 init）注册适配器 `FindNativeCodeByAddress -> JitCodeRunInfo`。（`JitCodeRunInfo` 只需引用 `JitMethod` 内的字段，不加 ABI 结构。）
3. 改 `thread_state.cpp`：`FindNativeCodeByAddress` -> `FindCodeRunInfo`；`slot_map_data/gc_point_map_data/code/rbp_to_rsp_offset` 读自视图；删两个 `#include "../jit/..."`。
4. 跑 GC 回归（含 T4/T2.2 精确扫描、hotpatch 后 demotion）。

**阶段 2（断 patch_loader 环）- R5**
5. 新增 patch-ir 工作集 + 释放回调注册点；interpreter 反注册。
6. `patch_loader.cpp` 改走注册点；`interpreter_entry.h`/`interpreter_vm.h` include 用不透明类型替换。
7. 跑 il2cpp 全量 + hotpatch 用例。

**阶段 3（断初始化 + 链接环）- R2 + R1**
8. R2：评估已存在 `bootstrap.cpp:197` 调用，删 `runtime_init.cpp:73`（或加注册点），跑启动序列。
9. R1：源码环断开后，将 `chaos_runtime_core PUBLIC chaos_interpreter` 降 PRIVATE / 删除，跑 Linux 完整构建确认链接无环。

> 顺序依据：GC 依赖是最底层安全红线，必须最先且独立验证；patch_loader 影响 hotpatch 功能其次；init/链接环最后且风险最低。
---
## 6. 风险与回退

| 风险 | 影响 | 缓解 |
|---|---|---|
| **GC 扫描正确性回归（R3/R4）**：`FindCodeRunInfo` 缺省/未注册时返回 nullptr -> 精确扫描退化为保守扫描，可能**漏报 / 延迟退休对象** | 内存泄漏 / UAF | (a) 缺省时沿用原保守路径并打 warning；(b) `RegisterJitCodeRegistry` 必须在首个 GC 前完成（init 顺序约束）；(c) 阶段1含 T4 精确扫描 + hotpatch demotion 回归 |
| **回调未注册竞态（R5）**：eager-JIT / lower / free 回调晚于调用点注册 | 空指针破坏 / 泄漏 | 复用 tier/hotpatch 已有的 "store + 读时判空 + 缺省安全路径" 模式；注册点文档化为"init 时完成" |
| **`JitCodeRunInfo` 与 `JitMethod` 布局漂移（R3/R4）**：jit 新增/改动 `JitMethod` 字段 | 视图值与实际元数据不一致 -> GC 错扫 | 视图为**只读快照 + 构造点单一**（在 jit 注册适配器内一次性同步）；加 `static_assert`/注释契约；任何 `JitMethod` 字段改动需同步视图字段 |
| **`delete IRMethod` 改回调（R5）**：释放职责转交 interpreter，若 interpreter 已卸载则 delete 落空 | 泄漏 | interpreter 注册的生命周期绑定进程（同 tier 回调），不卸载；缺省 free = 记录 warning + 泄漏告警（不崩） |
| **R2 依赖 bootstrap 调用时机**：若删除 `runtime_init.cpp:73` 后 bootstrap 未比首次异常更早调用 | cold-path SEH 未注册 | 阶段3专项启动回归；若时序不稳，保守回退为"注册一次性回调 + 首次调用兜底初始化" |
| **R1 删链接后 Linux 链接报未解析符号** | 链接失败 | 说明 R3~R5 断环不彻底（仍有 runtime-core 直接引用 interpreter 符号），回退为 PRIVATE 并把残存引用登记回补 |

**通用回退策略**：三阶段均以"可单独 merge、可单独 revert"的 diff 落地；每阶段以既有测试套件 + hotpatch 用例做门禁；任一阶段回归即 git revert 该阶段，不影响其它阶段。

**红线核对**：
- ATG / Codegen 生成的 `.gc_slot_maps` 段：`GcSlotMapV0`/`GcPointMapV0` 在 contracts 未改，**不破坏**。
- extern "C" 桥（`chaos_*` 序列）：方案C 全部为新增注册点/新增调用，不改任何既有桥的符号名，**不破坏**。
- TPG / Python 工具链：不触碰 `.patchdata` 布局、不触碰 `interpreter_entry` 的 ABI，**不破坏**。

---
## 7. 依赖清单到文件的索引（供实施定位）

- R1：`src/native/runtime-core/CMakeLists.txt:301`
- R2：`src/native/runtime-core/core/runtime_init.cpp:10,73`；实现在 `src/native/jit/jit_seh.cpp:82`；另见 `src/native/bootstrap/bootstrap.cpp:197`
- R3：`src/native/runtime-core/thread_state.cpp:22,992`
- R4：`src/native/runtime-core/thread_state.cpp:23,1006,1009,1020,1026`；`JitMethod` 在 `src/native/jit/jit_method.h:94-183`
- R5：`src/native/runtime-core/patch_loader.cpp:3-4,396,740-845,810-852,1048-1070`；`interpreter_entry.h`（`src/native/interpreter/interpreter_entry.h`）、`interpreter_vm.h`、`runtime_core.h:59`、`chaos_pch.h:57`、`hotpatch_dispatch.h:39`、`delegate_thunks.cpp:6`
- 样板（方案C 现成参照）：`route: gc_root_scanner.h:144-154 + gc_root_scanner.cpp:268`、`jit_demotion.h/.cpp`、`hotpatch_table.h:135-136,171-173`、`tier_manager.h:76-78`
- 契约层（方案B 目标）：`contracts/native/v0/codegen_bridge.h`（含 `GcSlotMapV0`/`GcPointMapV0`/`HotpatchEntryV0`/`CodegenBridgeV0`）
