# chaos-il2cpp 解释器评审 · 深度深化（面向攻坚）

> **日期**：2026-08-13 ｜ **前置**：`interpreter-capability-peak-plateau-valley-2026-08-13.md`（峰平谷总评）｜ **性质**：对最关键的"谷"做攻坚级深挖
> **新增证据**：Explore agent 深挖对象模型互操作边界 + 人工核对 GC/线程/性能
> **目的**：把"存量谷"转成"可攻坚路径 + 代价/收益/优先级"

---

## 〇、本深化最重要的修正：谷#1 比初判严重得多

初评把谷#1 说成"对象模型两套并存、不互通"。深挖后实情是**更尖锐的：解释器对象以裸指针直接流入 AOT 原生代码，同时解释器非 GC 存储被反向注册成 GC root** —— 这不是"不互通"的性能/覆盖问题，而是**真实的内存安全风险（GC 见不到对象 / GC 扫描非堆内存当对象）**。定性与优先级都要上调。

---

## 一、谷#1 攻坚深挖：对象模型统一的可操作面 vs 硬墙

### 1.1 事实（全部 file:line）

| 面 | 事实 | 证据 |
|----|------|------|
| **参数向裸传** | 解释器对象经 `std::memcpy` 原样进 ABI `argv[]`（无包装/转换）| interpreter_dispatch.cpp:103-110 |
| **返回向裸传** | AOT 返回 GC 对象裸指针进解释器；解释器 Ret 对象裸写给原生调用者 | interpret_method_call.cpp:220-226 |
| **MIC 直调** | AOT thunk 按 `uint64_t` 原样收对象参 | precache_dispatch_raw.cpp:277-289 |
| **三布局互斥** | `InterpreterObject`(interpreter_vm.h:261) / `chaos_managed_array`(ChaosGeneratedRuntimePrelude.h:45) / `BoxedValueHeader`(runtime_mode.cpp) | 无共享头/标签 |
| **数组硬墙** | 解释器 ref-array=`std::vector<InterpreterValue>`、flat=`flat_data`；原生=`header+连续元素` 单块 | `chaos_array_get_elements(ArrayStorage*)` 读错偏移 |
| **⚠️ 反向注册 GC root** | 解释器把 `InterpreterObject*` 写进非 GC 的 `g_static_fields` 并调 `BgcSatbPreWriteBarrier`/`BgcRecordRootChange` | interpreter_vm.cpp:579-588、fast_dispatch_arithmetic.inc:304-317 |
| **字符串裸 char\*** | `ldstr` push 裸 `const char*`，非 managed string/intern/编码 | fast_dispatch_core.inc:334-342 |
| **唯一真实 GC 转换** | 仅异常路径（type_token→`abi->object_new`）| interpret_method_call.cpp:168-187 |

### 1.2 攻坚路径（可操作面集中，非遥不可及）

**统一 = 在三处跨边界出入口加"对象物化/反物化"**，而不是推翻解释器对象模型：
```
InterpreterDispatch          (interpreter_dispatch.cpp)
InterpreterDispatchRaw       (precache_dispatch_raw.cpp)
InterpretMethodCall          (interpret_method_call.cpp)
   └─ 跨边界 args/ret 时：
        解释器沙箱对象 ──物化──► 新建真实 GC 对象 + 拷贝字段/元素
        原生 GC 对象   ──反物化─► 解释器 可读 表示（或保持裸指针但标记性质）
```
- **为什么可操作**：出入口就这 3 处、参数/返回值都走裸 `void*`；现有写屏障调用（`interpreter_vm.cpp:586`、`fast_dispatch_object.inc:153`）在对象升级为 GC 后可直接复用。
- **为什么难（硬墙）**：数组 `ArrayStorage` 内存形态与 `chaos_managed_array` 完全不同，物化要重布连续内存；`LazyBox`/boxed 概念需重写为 GC box。**建议分阶段**：先只物化跨边界的"普通对象+数组"，box/string 保持解释器内闭环（不跨 AOT 的纯内部使用无需动见 1.3）。

### 1.3 关键界定：哪些必须统一，哪些不用
- **必须**：跨 AOT 边界的对象/数组（否则写屏障/GC 语义全错，见风险）。
- **不用**：解释器纯内部闭环功能（`ldstr` 不跨 AOT、`Box/Unbox` 只在解释器内传播）——这些没有 native 侧布局预期，动了反而更危险。
- **边界锚点**：用"该对象是否会经 3 出入口传给 AOT"判。热更后的方法若全在解释器内跑（no AOT crossing），可长期保持沙箱。

### 1.4 代价 / 收益 / 风险 / 优先级
- **代价**：中高。3 出入口各加物化层 + 数组/对象布局对齐；需 GC 对象构造 API（`abi->object_new`/`chaos_array` 分配）复用。
- **收益**：消除真实内存安全风险（GC 见不到对象→误回收/悬挂；GC 扫非堆→崩溃）；让解释器可正确解释更多泛型/带盒对象；减少 keep-native 依赖。
- **风险**：物化本身引入新 bug（复制字段错误）、性能（跨界拷贝）。需 stress/GC 专项验证。
- **优先级**：🔴 **P0**（内存安全）。即便不全面统一，至少**先消除"反向注册非 GC 存储为 GC root"**这一错误（interpreter_vm.cpp:586 / fast_dispatch_arithmetic.inc:311 的 `BgcRecordRootChange` 前先确保证槽是 GC 对象 or 不注册）——这是最小止血。

---

## 二、谷#3 攻坚深挖：safepoint / GC-mode 切换

### 2.1 现状与取舍
- 解释器执行循环只每 64 指令查 `pending_abort/interrupt`，**不调 `SafepointPoll()`**（fast_dispatch_execute.inc:484-494）。
- GC 挂起靠抢占式信号/APC 把 RIP 重定向到 `gc_suspend_trampoline_x64` 再跑 `SafepointPoll`（thread_state.cpp:327-400）。这是 **signal-based suspension**，il2cpp/Mono 也用它，本身是成熟的。
- 但**长运行原生调用 / blocking P/Invoke / 无回边热循环**期间：解释器不主动 `GC_TRANSITION_TO_PREEMPTIVE`，且不 poll，GC 只能靠信号在任意点抢占（解释器栈中点，非 safepoint）→ **栈上对象被 GC 扫描时不安全**（解释器帧扫描脚本要求栈数据一致，被信号打断在中间态有风险）。

### 2.2 攻坚路径（分两档）
- **档 A（快速消除长调用风险）**：在解释器调用"可能长时间运行的原生/P/Invoke"前主动 `GcTransitionToPreemptive`，返回后 `Cooperative`（对齐 `entry_direct.cpp:793-811` 的 T4 做法）。改动集中、可测。
- **档 B（彻底，慢）**：在解释器回边/调用点插 in-band `SafepointPoll()`，让解释器自身成为 safepoint（对齐 JIT `cfg.safepoint_fn`）。需解释器帧 scanner 支持任意 safepoint 点，工作量大。

### 2.3 优先级
- 🔴 **P0/P1**：档 A（blocking 调用 preemptive 切换）是真实死锁/GC 延迟风险，先做；档 B 是长期正确性，P2。

---

## 三、性能量化：评审暴露的缺口（wiki 声称，未独立验证）

初评只给了定性。wiki 声明（`01-翻译管线/19-解释器VM架构.md:11-15`）：
- T1/T2 FastExecute ~30ns、T3 RegisterExecute ~15ns、T4/T5 native ~3ns（**声明值，未跑基准**）。
- MIC 使 CallVirt 从 ~2200ns → ~30ns（**~72x**，`21-interpreter-optimizations.md` 自述）。

**评审结论**：这些是**自述的优化数据**，缺独立 benchmark 验证；`register_vm_profiler`（`register_vm_profiler.cpp`）有 per-method RDTSC cycles/gc_bytes 计数器 + `DumpProfilerToFile`，但没跑出基线。深化评审建议补一项验证（列而不做，标注待证）：用 `entry.exe --benchmark` / `RegisterExecute` 频次对比，确认 T1/T3 声明值与 native 的真实倍数，避免把自述当事实。

---

## 四、深化后攻坚优先级排序（汇总）

| 优先级 | 项 | 归属谷 | 一句话 | 风险 |
|--------|----|--------|--------|------|
| 🔴 P0 | 消除"非 GC 存储反向注册 GC root" | 谷#1 | `BgcRecordRootChange` 调用前确保对象是 GC 或撤注册 | 内存安全 |
| 🔴 P0 | 边界对象物化（3 出入口） | 谷#1 | 跨 AOT 的普通对象/数组物化/反物化 | 性能+新 bug |
| 🟠 P1 | blocking 调用 preemptive 切换 | 谷#3 | 档 A，对齐 T4 | GC 延迟/死锁 |
| 🟡 P2 | 解释器内联 safepoint poll | 谷#3 | 档 B，回边/调用点 | 正确性 |
| 🟡 P2 | null/越界抛异常 | 谷#2 | `Ldelem/Stelem` 补 NRE/IOOR | 语义 |
| 🟢 P3 | `ldtoken`/`sizeof` 真实语义 | 平 | RuntimeTypeHandle/Hsize | 语义完整性 |
| 🟢 P3 | delegate `LdVirtFtn` 快路径 | 平 | 对齐慢路径 | 覆盖 |
| 🟢 P3 | 异常对象跨边界保留 | 谷#4 | 修 `PalTryCallNoExcept` 丢 Exception 指针 | 诊断 |

---

## 五、一句话收敛

**谷#1 不是"覆盖不足"，是真实内存安全债**；其统一的可操作面集中在 3 处 dispatch 出入口 + 已复用的写屏障，可先做 P0 止血（撤错注册 GC root），再逐步物化边界对象。谷#3 先做 blocking 调用的 preemptive 切换。性能数字是自述、待基准验证。整个解释器要往"il2cpp 全功能 fallback"走，谷#1 是必经主线；若定位"热更专用解释器"，则 P0 止血 + 边界锚定（keep-native 隔离）即可守住，谷为可接受边界。

---

## 六、关联
- 前置总评：`interpreter-capability-peak-plateau-valley-2026-08-13.md`
- 对象模型深挖 agent 证据（本文件 §一 的 file:line）
- A2：`Vector<int>` keep-native(AOT 侧图) 是谷#1 架构根的实例（已修 `46a417b23`）
