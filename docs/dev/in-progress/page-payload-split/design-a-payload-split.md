# A 方向设计 — page 0 大载荷段拆分为独立 TU

> 日期：2026-09-21
> 任务：system chunk C1002（MSVC 堆耗尽）根治
> 阶段：brainstorm → 待用户批准
> 上游：`c1002-heap-exhaustion-rootcause.md`、`a-direction-input.md`

## 一、问题（一句话）

分页器**只切 `templateModel.Methods`**，而 page 0 额外挂载的 object model /
module registration / GC slot map **整体不分页**，导致 page 0 = **71.2 MB**，
远超 `sizeThresholdChars = 350_000` 的设计目标（超 ~200 倍），触发
`C1002: compiler is out of heap space in pass 2`。

## 二、边界拍板（用户已确认）

| # | 裁决 |
|---|---|
| 方向 | **A** —— 拆大载荷段 + 段内再分页（彻底） |
| 段边界 | **按段自然边界切**，每个大段独立 TU |
| 拆分粒度 | **每段一 TU**，段内再按字节预算分页 |
| 链接性 | **`extern`（C++ 链接性）**，非 `extern "C"` |

## 三、关键证据（实测）

### 3.1 链接性不影响性能 —— 三情形字节级相同

```asm
; static void (*kMethodTable[3])() = {...};
; extern void (*kMethodTable[3])();
; 跨 TU（只见声明不见定义）
    mov rax, qword ptr [?kMethodTable@@3PAP6AXXZA]   ; 7 bytes, RIP-relative
    ret
```

**三者指令序列完全相同。** 链接性是编译期/链接期属性，不是 codegen 输入。
→ P1（性能）上 `extern` 与 `extern "C"` **打平**，故按 P2 取与既有模式一致者。

### 3.2 不改 ABI —— 生成侧 `kMethodTable[]` 无外部按名依赖

runtime 的是**另一个符号** `g_method_table`（`method_table.cpp`）。
全仓搜 `extern kMethodTable` / `"kMethodTable"` **无命中**（仅注释提及）。

### 3.3 跨 TU 机制已存在且在跑

共享头已声明 `chaos_mt_*`(2439) / `chaos_vtable_*`(96) / `chaos_type_id_*`(2438)，
page 2..N 今天就在跨 TU 引用。**复用既有机制，非新发明。**

### 3.4 构建系统自动接纳新 TU

`TestProject.CMakeLists.txt.scriban:85` — `file(GLOB ... subjects/*.cpp)` 无条件收录。
新 TU 无需改 CMake/TPG。**但命名需落在 fallback 模式内**
（`:87-89` 的 `native-aot.generated.*` / `native-aot.page-*`）。

### 3.5 索引形态

`kMethodTable` 9,626/9,628 引用是**编译期字面量索引**，**零处出现在循环中**。
`kSubjectSlotMap` 有 **1 处运行期索引** —— 拆 TU 时须与该表同 TU。

## 四、方案

### 4.1 结构

```
Page 0 (native-aot.generated.cpp)      ← 只留：Includes + 注册入口 + 跨 TU 声明消费
  ↓ 拆出
native-aot.om.page-XXX.cpp    × N      ← Virtual method table arrays (22.2MB/31%)
native-aot.reg.page-XXX.cpp   × N      ← Module registration        (22.0MB/31%)
native-aot.gcsm.page-XXX.cpp  × N      ← GC Slot Map Section        ( 8.7MB/12%)
native-aot.misc.page-XXX.cpp  × N      ← hotpatch / abi / 其余小段
```

### 4.2 三个组件

| 组件 | 职责 | 位置 |
|---|---|---|
| **段分类器** | 把 `moduleRegSb` 的内容按 `// ── <name> ──` 标记切段 | `NativeAotLoweringPlanner.Methods.cs` |
| **段分页器** | 每段按字节预算（如 ≤2MB）切分，输出到独立 `NativeAotGeneratedSource` | `NativeAotEmitter.Shared.cs` |
| **声明同步器** | 把拆出段内的**定义符号**补进共享头 `extern` | `NativeAotEmitter.Shared.cs`（`BuildSharedHeader`） |

### 4.3 改动点

1. **`NativeAotLoweringPlanner.Methods.cs`** — 目前所有段 `Append` 进单个
   `moduleRegSb`（:1271-1360）。改为**按段产出命名区块**，保留段名。
2. **`NativeAotEmitter.Shared.cs`** — `BuildGeneratedSources` 增加「按段拆 TU」分支，
   与既有「按方法分页」并存；page 0 只持有注册入口。
3. **链接性改写** — 234 个顶层 `static` 表 → `extern`（**仅限被拆出段的**）。
4. **共享头** — 补对应 `extern` 声明。

## 五、风险评估

| # | 风险 | 触发条件 | 后果 | 缓解 |
|---|---|---|---|---|
| R1 | **`static` 漏改** | 拆出段中仍有 `static` 表被别段引用 | C2065 / LNK2019 | 编译期即暴露；TDD 加「无跨 TU static 引用」守卫 |
| R2 | **段间隐式顺序依赖** | 某段依赖另一段的**定义顺序**（非声明） | 初始化顺序变化 → 运行期行为变化 | 实测：表均为**数据**，无运行期初始化顺序依赖；fact 全量回归验证 |
| R3 | **命名不匹配 glob** | 新 TU 名不在 fallback 模式内 | 非 flat 路径漏编译 | 命名统一用 `native-aot.*.page-*.cpp` |
| R4 | **快照基线大范围变更** | 发射结构变化 | snapshot 红 | 预期内；`SNAPSHOT_UPDATE=1` 后**再跑一次 clean run** 才算验证 |
| R5 | **TPG 内嵌陈旧 Generator.dll** | 改 Generator 后未重建 TPG | pipeline 静默跑旧 codegen | 改后必 `dotnet build` TPG（同既有教训） |
| R6 | **性能回退** | 跨 TU 丢失优化 | 运行期变慢 | **已实测排除**：三情形指令序列相同（3.1） |

## 六、三优先级权衡

| 优先级 | 评估 | 结论 |
|---|---|---|
| **P1 性能最优** | 链接性改动**零性能影响**（3.1 实测）；TU 变小反而利于 MSVC 单 TU 优化 | ✅ **强正向** |
| **P2 架构完美** | 消除「page 0 无界」的结构缺陷；复用既有跨 TU 声明机制 | ✅ **强正向** |
| **P3 热更适配** | `extern` 不改符号名、不改 ABI，hotpatch 表位置与语义不变 | ⚪ **中性** |

**无冲突。**

## 七、验收

1. system chunk **构建通过**（当前 C1002 阻断）
2. reflection / threading **不劣化**（当前均通过）
3. `no TUs over threshold`：page 0 ≤ 设计阈值量级
4. codegen 单测全绿（含既有 2261 条）
5. snapshot 基线变更经人工核对
6. **负控**：守卫测试须证明在旧结构下会红

## 八、问题清零

```
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: pending
```
