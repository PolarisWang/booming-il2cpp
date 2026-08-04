# JIT 工业化收官 — 执行计划

> **面向执行 Agent：** 使用 dev-executing-plans 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 将 JIT 从 2.5/5 提升至 5.0/5。覆盖剩余 4 个子任务（wf1-liveness、wf1-osr、wf1-unwind、wf1-tests）。

**实际状态更新：**
- ✅ **wf1-arch (IEncoder/ISehHandler 抽象)** — 已完成。IEncoder.h、ISehHandler.h、X64Encoder.h、WinSehHandler.h、LinuxSehHandler.h 均存在并已集成。
- ✅ **wf1-gc (GC 精确槽映射)** — 已完成。GcSlotMapV0 在 jit_engine.cpp Generate() 序列化（3689-3702），在 entry_direct.cpp 注册（683-685/848-850），gc_root_scanner 支持精确/混合扫描，12 个单元测试存在。

**技术栈：** C++ (native), GoogleTest, x64 assembly

**架构审核模式：** normal

**结构告警重点：** 无新增文件，只修改现有代码。

**权责图审核主题：** n/a

**设计文档：** n/a（父 roadmap 已定义完整边界）

**问题清零来源：** parent-roadmap（full-industrialization-roadmap）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 任务 1: wf1-liveness — Liveness 默认开启

**目标：** 将 `config_.enable_liveness` 默认值从 `false` 改为 `true`，使所有 Tier 1 JIT 编译启用 liveness 过滤。

**现状：**
- Liveness 分析已实现（ComputeLiveIn in jit_engine.cpp, ~3293-3388）
- RecordGcPoint 已支持 liveness 过滤（use_liveness_ flag 控制）
- 但 `CompileConfig::enable_liveness` 默认为 `false`（jit_engine.h:53）
- ISehHandler 的 T4 code 已正确注册，GC slot map 已生成

**修改：**

- [ ] 1.1 在 `jit_engine.h` 中将 `enable_liveness` 默认值改为 `true`：
      ```cpp
      bool enable_liveness = true;  // was false
      ```

- [ ] 1.2 验证 entry_direct.cpp 的 CompileConfig 创建处不受影响（当前没有显式设置 enable_liveness）

- [ ] 1.3 构建验证 debug preset 和 ship preset

- [ ] 1.4 运行全回归确认无退化

- [ ] 1.5 在 JIT benchmark 中对比开启前后 GC slot map size 变化

**风险：** 非常低。Liveness 只是过滤掉已经确定 dead 的 ObjectRef vreg，不会遗漏活引用。false positive（保守）→ true positive（精确）是单向安全变化。

---

## 任务 2: wf1-osr — OSR segfault 修复

**目标：** 修复已知 OSR segfault 的 3 个向量。

**现状：**
- OSR 测试 `jit_osr_test.cpp` 存在
- OSR 实现在 `jit_engine.cpp`（EmitDeoptSequence, OsrResolveLoopHeader）
- 已知有 segfault 在 Test_OsrEntry 中

- [ ] 2.1 **向量 1 — OsrResolveLoopHeader 空指针**：检查 OsrResolveLoopHeader 中是否存在对 null/invalid RegisterMethod 指针的解引用

- [ ] 2.2 **向量 2 — DeoptTlsState 栈帧对齐**：检查 DeoptTlsState 中的帧对齐逻辑，确保从 T4 编译代码切换到 OSR entry 时 RSP/RBP 对齐正确

- [ ] 2.3 **向量 3 — OSR entry 基本块跳转偏移**：检查 OSR entry stub 中的 jmp 指令偏移计算是否正确

- [ ] 2.4 在 OSR 相关路径添加 nullptr 检查、边界检查等防御性断言

- [ ] 2.5 在 OSR 路径添加 PROFILE_SCOPE 预埋点（便于未来调试）

- [ ] 2.6 编译并运行 Test_OsrEntry 确认无 segfault

- [ ] 2.7 全回归通过

**定位方法：** 先在 debug 构建下运行 OSR 测试获取完整堆栈，根据 crash offset 反向定位。使用 trace 系统 `run trace` + `trace-analyze`。

---

## 任务 3: wf1-unwind — Unwind 编译守卫

**目标：** 为 Unwind Info 增加编译期 static_assert 守卫。

**现状：**
- unwind 实现在 `jit_unwind.cpp`
- Win64 `.pdata/.xdata` 结构体定义
- Linux DWARF `.eh_frame` 结构体定义

- [ ] 3.1 在 jit_unwind.h 或 jit_unwind.cpp 中添加 static_assert 守卫：
  
  ```cpp
  // Win64 UNWIND_INFO 相关
  static_assert(sizeof(UNWIND_INFO) <= ...,
                "UNWIND_INFO size must fit in xdata header");
  ```

- [ ] 3.2 具体守卫项：
  - Win64 RUNTIME_FUNCTION 大小和偏移约束
  - UNWIND_INFO 结构体约束
  - 函数表偏移量范围检查
  - 对齐约束

- [ ] 3.3 构建确认 static_assert 不触发

- [ ] 3.4 运行 unwind 测试 `jit_unwind_test.cpp`

---

## 任务 4: wf1-tests — 测试套件补齐

**目标：** 将 JIT 测试通过率从当前 baseline 提升至 >= 99%。

**现状：** 测试文件列表（14 个）：
- jit_native_test.cpp, jit_il_smoke_test.cpp, jit_mode_test.cpp
- jit_hybrid_mode_test.cpp, jit_seh_handler_test.cpp, jit_seh_test.cpp
- jit_pgo_test.cpp, jit_abi_test.cpp, jit_gc_slot_map_test.cpp
- jit_osr_test.cpp, jit_unwind_test.cpp, jit_binary_ir_test.cpp
- bench_jit_vs_aot.cpp, jit_hybrid_mode_test.cpp

- [ ] 4.1 运行全量 JIT 测试，记录当前 pass/fail 清单

- [ ] 4.2 逐项 triage 已知失败的 5 个用例（176/181 baseline）：
  - 属于 wf1-osr 修复范围的 → 在任务 2 修复
  - 属于其他 bug 的 → 修复或记录 scope limitation

- [ ] 4.3 补齐缺失测试场景：
  - GC 精确槽映射 + Liveness 联合测试（开启 liveness 后 slot map 正确性）
  - OSR 边界条件（循环嵌套、深栈帧、泛型方法 OSR）
  - Unwind 边界测试（异常展开路径、跨帧展开）
  - SEH 过滤表达式边界测试
  - 多线程 JIT 编译压力（同时编译多个方法）

- [ ] 4.4 新增测试：JIT 编译压力测试
  - 大量方法（100+）快速 JIT 编译
  - 编译并发（多线程同时调用 Compile）
  - 编译阶段异常恢复（OOM、非法 IR）

- [ ] 4.5 全回归验证，确保新测试不破坏既有用例

- [ ] 4.6 更新测试 README 或注释记录已知 scope limitation

---

## 收尾链路

- [ ] 5.1 结构告警与架构审视：检查是否引入了重复代码、文件职责是否清晰

- [ ] 5.2 全量测试通过

- [ ] 5.3 归档 completed：将 `docs/dev/in-progress/ws1-jit-industrialization/` 移至 `docs/dev/completed/ws1-jit-industrialization/`

- [ ] 5.4 回写父任务：更新 `full-industrialization-roadmap/STATUS.md` 中 WS1 状态为 completed，检查 phase-2 (WS2) 启动条件是否满足

- [ ] 5.5 git commit（用户确认后）
