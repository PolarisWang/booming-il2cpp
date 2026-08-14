# Profiler / D1 ns-op 锚 — Session 交接文档

> **交接日期**：2026-08-14 ｜ **交接分支**：main（工作区干净）
> **任务**：完善解释器 profiler 功能，产出可量化的 ns/op + opcode + per-method 性能锚
> **给新 session 的目的**：无缝承接调研结论 + 已完成工作 + 待执行方案，无需重读全部源码。

---

## 〇、核心调研结论（一句话版）

解释器缺乏**可量化的性能锚**：`VmProfileScope`(per-method RDTSC) 与 `DumpFastExecuteOpcodeHistogram` 代码都已存在，但 **原生树里没有任何触发 `DumpProfilerToFile` 的点**（只在 codegen 的 Scriban 模板里），导致"D1 代码已落地但输出不可生产"。这就是要完善的核心。

---

## 一、这个功能要解决什么问题（为什么做）

不是"缺一个 profiler"，而是**无法量化解释器各 tier 的成本 → 性能优化是盲优化**。三个痛根：

1. **热点不可归属**：`a1_external_fallback_gap_report` 实测 111 个外部 fallback 正确性缺口（无 C++ body + 无 stub → return-0）。修复/判定这些缺口必须先知道"哪些方法在真实路径上是热点"——没有方法级 profile 无法排序。
2. **优化无法归因**：C2（扁平存器文件）、C3（QuickJIT 阈值）、省写穿等优化，无"前后 ns/op + per-opcode 分布"基线就分不清真假。
3. **分层归因缺失**：同 IL 可跑 T1(FastExecute)/T2/T3(RegisterVM)/T4(JIT)。哪个 tier 主导？无 cycle 归因无法决定"优化哪层"。

> "转向 profile" = 从正确性修复 → 性能量化基建。

---

## 二、完整 picture（现状全貌）

### 2.1 三块组件，功能完备度

| 组件 | 位置 | 已做 | 缺口 |
|------|------|------|------|
| **FastExecute opcode histogram** | `fast_dispatch_execute.inc:136-164` | `g_fast_op_freq[128]` thread_local + `DumpFastExecuteOpcodeHistogram()` | ✅ `tiering_benchmark.cpp:1944` 已调 dump；❌ 只统计 T1，RegisterVM 无直方图；❌ thread_local 跨线程未聚合 |
| **RegisterVM per-method 锚** | `register_vm_profiler.h/.cpp` | `VmProfileScope` 接入 Step-B + OSR + method-replacement 三处 | ❌ **无 in-tree 调 `DumpProfilerToFile`**（只在 Scriban:1316）；❌ `FindOrCreateSlot` 线性探测 + RDTSC 注入污染测量；❌ `gc_alloc_bytes` 通道恒传 0 |
| **ns/op 锚** | `tiering_benchmark.cpp` | 循环 iter 调 `InterpreterEntryDirect` 计时 | ❌ 只整体 ns/op，无 per-opcode/per-tier/per-method 归因 |

### 2.2 硬约束（当前代码）
1. **RDTSC 仅 x86**：`register_vm_profiler.h:159` `#error` if 非 x86/ARM64 with VM_PROFILER_ENABLED。ARM64 只能用 `CHAOS_IL2CPP_PROFILE_SCOPE`（更粗）。
2. **`VmProfileScope` 体全编译出（默认 0）**：PROFILE tier 以下零开销（设计正确），但测就得整仓 PROFILE 构建。
3. **`FindOrCreateSlot` O(探测) 原子**：每次方法进入 RDTSC+原子 CAS → 高频短命方法下测量本身主导。
4. **`g_vm_profiler` 全局原子**：跨线程归因模糊。
5. **两套 profiler 不互通**：无法关联"哪个方法贡献了哪个 opcode"。

### 2.3 断点链（要补的缺口）
```
VmProfileScope 埋点 ✓ → g_vm_profiler 累积 ✓ → DumpProfilerToFile ✓
                                                        ↑
                                    缺 in-tree 触发点（只在 Scriban）
→ 原生树任何测试/benchmark 都触发不了 per-method dump → 输出不可生产
```

---

## 三、完善方案（4 个，按风险/成本递进 —— 主推荐 A+B 组合）

### 方案 A：补 in-tree dump 触发点 —— 最小可产出（0.5d，低风险）
**目标**：`tiering_benchmark` 在 PROFILE build 下同时 dump opcode 直方图 + per-method 锚，解锁首个量化基线。
**改动**：`tiering_benchmark.cpp` `main`（现有 line 1944 直方图后）加：
```cpp
#if CHAOS_IL2CPP_VM_PROFILER_ENABLED
    extern void chaos::il2cpp::interpreter::DumpProfilerToFile(const char*) noexcept;
    extern void chaos::il2cpp::interpreter::ResetProfiler() noexcept;
    DumpProfilerToFile(nullptr);  // per-method cycles → stderr
    ResetProfiler();
#endif
```
镜像 Scriban(TestProject.RuntimeEntry.cpp.scriban:1314-1317)。配 `--preset profile`（`artifacts/presets/profile`，`CHAOS_IL2CPP_CONFIG_TIER=profile`，CMakePresets.json:40-48）构建跑出首个基线。
**局限**：仍 RDTSC-only、无 per-opcode×method 关联。
**验证**：`test_driver --layer unit` OVERALL OK 基线，PROFILE build 编译绿。

### 方案 B：降测量注入 — 保测量可信度（1-1.5d，中风险）
- **B1**：slot 查找改 thread_local 一级缓存（方法 token→slot），hit 免线性探测/原子 CAS。
- **B2**：RDTSC 采样（每 N 次记一次）降注入。
- **B3**：`gc_alloc_bytes` 接真 GC 分配差分（当前 `RecordCall(..., 0)`）→ per-method 锚含 GC 字节。
- **B4**：直方图跨线程聚合（thread_local → 汇总线程）。
**价值**：测量可信（不被自身主导）+ GC 分配维度。
**验证**：做 B 后用 A 的基线对照，热方法 mean cycles 不应因开 profiler 显著劣化；非 PROFILE 零开销不变。

### 方案 C：跨层归因 + per-opcode×method 关联（2-3d，高风险）
- **C1**：给 RegisterVM 加 opcode 频率直方图（对齐 `g_fast_op_freq`），T1/T2/T3 分布可比。
- **C2**：per-method 锚与直方图 share 方法 token，导出"方法→opcode 占比"二维。
- **C3**：`tiering_benchmark` scenarios 覆盖每 tier（现在混测）。
**风险**：触 register dispatch 表/数据模型，动 hot path。**需 A+B 基线压舱**，不建议现在做。

### 方案 D：跨平台/可移植 profiler（远期，2-3d+）
解 RDTSC-only：单调时钟抽象（`steady_clock`/`QPC`/`clock_gettime`）或采样 profiler，`#error` 改优雅降级。低优先（当前主战场 x64 Windows）。

---

## 四、建议执行路线（新 session 按此推进）

1. **方案 A**（本案最小可产出）：补 in-tree dump + `--preset profile` 构建跑出首个基线 → 解锁 C2/C3 量化。
2. **方案 B1+B3**（可信度）：thread_local slot 缓存 + 接真 GC 分配。保测量不被污染。
3. **方案 C1+B4**（归因）：RegisterVM opcode 直方图 + 跨线程聚合。回答"该优化哪层哪个 opcode"。
4. **方案 D**（跨平台）：移 ARM64 时再做，低优先。

**验收锚**：profile 构建后 `tiering_benchmark` 输出 ① opcode 直方图 ② per-method RDTSC cycles ③（做 B3 后）GC 分配字节 —— 全非空且可对比前后。

---

## 五、DTO/Session 已完成工作（新 session 无需重做，避免重复）

### 已提交（main，全部回归绿）—— review 修的正确性 P1/P2
| commit | 内容 | 验证 |
|--------|------|------|
| `817ba761a` | MixedOpcode 崩溃修复（帧复用 sp 泄漏） | MixedOpcode 5万次 0 错 EXIT=0 |
| `f6e3a3335` | RegisterVM div/rem 除零+INT32_MIN 守卫 + reg 越界防护 | ir_reg 50/50 |
| `b13116729` | NoChk/barrier opcode 103-106 allocator 改写 | ir_reg 51/51 |
| `b9653cb5d` | Reg_StInd/StObj GC 写屏障 | ir_reg 51/51 |
| `1ffa775a3` | Reg_LdArgA 真地址 + LdLocA 路由 FastExecute | ir_reg 53/53 |
| `d80230fd3` | C4 SmallFieldArray 几何增长 | ir_reg 54/54 |

### 已提交（D1/C4 profile 相关）
- `b4d1ec85b`：VmProfileScope 接入 RegisterExecute（Step-B + OSR + VmProfileScope 挂 PROFILE tier）
- `806a4f107`：C4 call>8 合并单块
- `3cbed6f96`：**profile 功能整体设计深度分析**（`docs/dev/assessments/interpreter-profiler-overall-design-2026-08-14.md`）← 本调研核心文档

### 回归基线
`test_driver --layer unit` **OVERALL OK**（2249→2248 pass, known 全既有非本次）。

---

## 六、环境坑（共享 worktree，必须注意）

1. **并行线（jit/gc）实时编辑**：session 内经见 `gc_young_collector.cpp`、`gc_region_barrier_stress_test.cpp`、`com_ccw.cpp` 被并行改。**commit 前 `git status` 只 stage 自己文件**，勿 `git add -A`。
2. **PROFILE 构建**：`--preset profile` 独立 binaryDir `artifacts/presets/profile`（git-ignored），不影响共享 `build/native`。配 `CMAKE_BUILD_TYPE` 用 VS 17 2022。
3. **禁 `git stash`**；验证用临时 worktree 或显式提交。
4. **PDB 冲突修复**：`ir_reg_alloc.cpp` 编译遇 C1041 PDB 竞争（并行构建）→ 重跑即可。
5. **`chaos_is_gc_pointer`** 在 `chaos::il2cpp::runtime_core` namespace，且 `gc_helpers.h` 不 include `gc_api.h` —— 跨 TU 用需显式 `<gc/gc_api.h>` + 全限定。

## 八、本轮 session 落地修正（2026-08-14 验证后补）

> 新 session 直接采信以下实测结论，勿再踩以下三个坑。均已在本 session 修复并验证。

1. **build 配方更正**：`--preset profile` 单独跑会 `FATAL_ERROR: Unsupported ROADMAP0_PRESET_TARGET=''`（`CMakeLists.txt:775`）。顶楼 CMakeLists 完全按 `ROADMAP0_PRESET_TARGET` 分流，`profile`/`debug`/`ship`/`asan` 预设都**不设**该变量。正确配方（Windows x64 桌面）：
   ```
   cmake --preset profile -DROADMAP0_PRESET_TARGET=windows-x64-reference
   cmake --build artifacts/presets/profile --config Debug --target chaos_tiering_benchmark -j 4
   ```
   （VS 多配置生成器必须 `--config`；MSB8029 temp-dir 警告无害；`artifacts/` git-ignored。）
2. **`register_vm_profiler.h` 缺 `__rdtsc` include（P1 修复）**：PROFILE build 下 `CHAOS_IL2CPP_VM_PROFILER_ENABLED=1` 激活 `VmProfileScope`，但头文件只 include `<cstdint>/<cstdio>/<atomic>/<cstring>`，**没有 `<intrin.h>`/`<x86intrin.h>`** → `C3861 '__rdtsc'` 编译失败。已按 `common/chaos/profile.h:50-58` 的守卫式 include 修复。
3. **方案 A 已落地并实测产出**：`tiering_benchmark.cpp` 加入 `#include "register_vm_profiler.h"`（line 25）+ 在 `CHAOS_IL2CPP_PROFILE_DUMP()` 后（main 尾部）加 `#if CHAOS_IL2CPP_VM_PROFILER_ENABLED` 包住的 `DumpProfilerToFile(nullptr); ResetProfiler();`。**probe 实测**：`VmProfileScope` → `DumpProfilerToFile` 正确 dump 出按 call_count 排序的 per-method cycles/calls 表；GC Bytes 恒 0（印证 B3 缺口）。benchmark TU 在 profile 预设下干净编译链接成 exe。
   > ⚠️ **Debug+PROFILE 下跑完整 benchmark 极慢**（`/Od` + per-call profiler，12+ scenarios 超出会话时长）。要快速看到 dump，直接跑 unit probe 或改用 `--config Release`。真实基线仍须用 Release/PROFILE。
4. **`vm_profiler_test.cpp` 也在 in-tree 调 `DumpProfilerToFile`**（不止 Scriban），修正文档 §〇/§一 "只在 Scriban" 的措辞。

### 九、⚠️ Release/PROFILE 全量 benchmark 的 `-1` 正确性 bug（2026-08-14 实测，D1 基线阻断点）

> **这是当前 D1 全量基线无法产出的真正阻断**，需独立 systematic-debugging session，与 profiler 本身无关（profiler dump 在 main 末尾，根本走不到）。

**现象**：`Release/PROFILE` 构建跑 `chaos_tiering_benchmark.exe`，多数 scenario 方法返回 **`-1`**（= `InterpreterEntryDirect` 未写 `ret_val`，即方法 **fault/无返回值**），非 timing：
- `bench_arithmetic`：`FAIL: warmup expected=60 got=-1`、`MISMATCH expected=162 got=-1`
- `bench_register_10` / `bench_native`(expected=30) / `bench_multi_alu_t4`(60) / `bench_loc_storm_t4`(40) / `bench_branches_t4`(42) 全 FAIL
- **`bench_callvirt_pic` PASS**（同 profile 构建、同 `VmProfileScope` 激活下）

**为何排除"我改的 profiler"**：① `callvirt_pic` 在 profiler 激活下通过（profiler 非 blanket 腐化）；② profiler dump 在 main 末尾，fail 早于它；③ `VmProfileScope` 只是局部 `uint64` RAII + RDTSC + 全局原子 CAS，不 touch 调用者 `RegisterFrame`，无法 fault 方法；④ 失败是方法级 `-1`=fault，与特定 opcode 路径相关。

**为何像 pre-existing Release-only bug**：`bench_arithmetic` 用 `ldarg(6)/add(25)/ret(53)`，我近轮改的 div/rem、NoChk、StInd/StObj barriier、LdArgA/LdLocA、reg 越界都**不触这些 opcode**。Debug/PROFILE（§8 probe 跑通）vs Release 差异 → 像 **-O2 下解释器/JIT 的 UB**，非新增回归。

**下一步（新 session/systematic-debugging）**：定位哪个 tier/opcode 在 -O2 下 fault。线索：`bench_callvirt_pic` 过、纯算术/存器/直调 T4 fail → 优先查 `RegisterExecute` 与 T4 原生在 Release/PROFILE 的 `-O2` 行为（可能 `g_jit_deopt_state`/`RegisterFrame` 未初始化/跨 -O2 排序）。修好后即可产出 D1 全量基线。



---

## 七、关联文档链
- **本调研核心**：`docs/dev/assessments/interpreter-profiler-overall-design-2026-08-14.md`
- 主计划（review 正确性）：`session-handoff-2026-08-13.md` §二.D1 + §三.1
- code：`register_vm_profiler.h/.cpp`、`fast_dispatch_execute.inc:136-164`、`tiering_benchmark.cpp:1944`
- CMake preset：`CMakePresets.json:40-48`（profile）
- memory：`interpreter-session-2026-08-13-round2`、`interpreter-c4-remainder-2026-08-14`、`perf-line-benchmark-credibility-and-dispatch`
