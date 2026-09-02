# Native C++ 大文件拆分方案 (module-split-design.md)

> 日期：2026-09-02
> 域：runtime(1) + translation(6) + GC(2)
> 对标基准：production il2cpp 工程门禁体系
> 全局优先级：P1=性能 > P2=架构完美 > P3=热更新（保持默认）
> 方法论：仿照 GC-工业化「验证先行 + 阶段推进」

---

## 1. 文档目标

本项目 native 源码约 123,000 行 (576 文件)，其中 **22 个超过 1000 行**的超大文件合计约 **36,376 行**，占总量的约 30%。其中 `gc_old_gen.cpp` 高达 **3918 行**，是单体风险最集中的文件。

本方案为这 22 个超大文件制定**纯文件级拆分**（只拆不改逻辑），目标与 roadmap-v1-01.md 的 P2 阶段（D3 架构级：无 >1000 行文件）对齐：**将 22 个超大文件拆至 ≤5 个，其余全部 ≤1000 行**。

**核心原则**：拆分为「只拆不改逻辑」的 git 友好策略。任何拆分提交不得改动函数实现、签名、行为，仅做文件移动 / 代码段落搬迁 / 命名空间重组。保证编译不破坏、git blame 可追溯、性能零回归。

---

## 2. 拆分总原则

### 2.1 「只拆不改逻辑」铁律

| 允许 | 禁止 |
|------|------|
| 把已存在的函数/类/静态变量整体移动到新文件 | 修改任何函数实现、参数、返回类型 |
| 新增 `#include` / 移动 `#include` 归属 | 改变任何符号的命名空间归属 |
| 把 `.h` 中的声明拆分到多个头文件并保持接口一致 | 增删功能、合并分支、重构内部算法 |
| 新增内部 forward declaration 以隔离依赖 | 改动 extern "C" ABI 导出符号名 |
| 重命名文件（保证 include 路径/guard 同步更新） | 改动全局静态变量语义 |
| 把头文件按 ABI/平台拆成 .h + .cpp | 改变任何数据布局 / 内存序 / 锁语义 |

- **提交粒度**：每个超大文件的分拆 = 独立 commit，commit message 标注 `[refactor] split <file> into N files (no logic change)`。
- **可验证性**：拆分提交前后必须**字节级或 token 级**等价。建议用 `git diff --word-diff` + clang-format 基线对齐 + 构建绿 + 单测绿 + JIT 回归绿作为验收。
- **git blame**：对被搬迁代码，新文件的每一行应保留原始 commit 归属。

### 2.2 拆分粒度标准

- **单文件 ≤1000 行为主**；允许局部小文件临界（900-1000）不强行再拆。
- **按职责边界拆，不按行数机械切**。
- **头文件 (.h) 元规则**：`x64_encoder.h` / `arm64_encoder.h` 若含 inline/模板实现则必须留头；非模板工具函数可下沉到 `.cpp`。

### 2.3 规模化聚类方法

每个超大文件先按内部职责段落（`// ── ... ──` 或 `// ==== ... ====` 节）聚类，再决定分组。通用分组维度：
1. **生命周期/子系统**：初始化/构造 vs 运行 vs 析构/清理
2. **算法类型**：分配器 vs 遍历/标记 vs 压缩/搬移 vs 诊断
3. **平台/后端**：x64 vs arm64 分开（encoder）；Windows(BCrypt) vs Linux(OpenSSL) 分开（crypto_stubs）
4. **ABI 面**：`extern "C"` ABI 导出 vs 内部 static 辅助
5. **重领域**：数据流（cache/metadata）vs 控制流（调度/线程）

### 2.4 命名空间与 include 纪律

- 拆分后保持**同一命名空间**归属不变。不允许为拆分而新增命名空间层。
- 被拆出的 .cpp 各写清自己的 include 集；公共声明放共享 .h。
- **禁止借拆分引入头文件循环**；若发现依赖环，降级为"保持合并"或抽出中间中立头。

---

## 3. 每个超大文件的分拆建议

> 标注：`P0`=必须拆（>1500 行 或 单体高耦合热径），`P1`=建议拆，`P2`=可选/保持
> 当前行数为 wc -l 实测。

### 3.1 优先级总表

| # | 文件 | 当前行数 | 建议新文件数 | 优先级 | 主要拆分轴 |
|---|------|:---:|:---:|:---:|:---|
| 1 | gc_old_gen.cpp | 3918 | 6-7 | **P0** | OldGen 生命周期/分配/标记/压缩/收集/finalizer |
| 2 | ir_reg_alloc.cpp | 2969 | 3 | **P0** | 分配器主流程/指令emit/GC屏障 |
| 3 | enum_stubs.cpp | 2520 | 4 | **P0** | metadata/辅助/ToString/Parse 簇 |
| 4 | x64_encoder.h | 2216 | 4 (.h+.cpp) | **P0** | 基础/整数ALU/浮点/YMM |
| 5 | jit_codegen_generate.cpp | 2062 | 3 | **P0** | 优化/主Generate/入口 |
| 6 | jit_codegen_emit.cpp | 2050 | 2-3 | **P0** | 大 switch 拆簇 |
| 7 | vector_fixed_templates.h | 2002 | 保持 | **P2** | 纯模板，必需头文件 |
| 8 | gc_bgc.cpp | 2001 | 4 | **P0** | 周期/标记worker/finalizer/Gen1位图 |
| 9 | interpreter_vm.cpp | 1970 | 3-4 | **P0** | InterpreterValue/栈/Execute主循环 |
| 10 | gc_region.cpp | 1772 | 4 | **P0** | Region分配/Nursery/POH/区间查询 |
| 11 | arm64_encoder.h | 1485 | 3 (.h+.cpp) | **P0** | 基础/整数/FP-SIMD |
| 12 | entry_direct.cpp | 1251 | 3 | **P1** | 参数/IR clone/PIC/QuickJit |
| 13 | marshal_api.cpp | 1168 | 4-5 | **P1** | 字符串/COM/Variant/custom |
| 14 | patch_loader.cpp | 1093 | 3 | **P1** | MetadataCache/PatchContext/队列 |
| 15 | crypto_stubs.cpp | 1091 | 2-3 | **P1** | BCrypt/OpenSSL/公共 |
| 16 | thread_state.cpp | 1076 | 2 | **P1** | TLS/safepoint |
| 17 | jit_optimizer.cpp | 1047 | 3 | **P1** | LICM/IV/常折叠 |
| 18 | gc_young_collector.cpp | 1024 | 2-3 | **P1** | TLAB/收集 |
| 19 | gc_gen1.cpp | 1006 | 2 | **P1** | Gen1收集/跨代 |
| 20 | jit_helpers.cpp | 948 | 2 | **P1** | codegen辅助/TLS-TLAB |
| 21 | vtable_registry.cpp | 952 | 2 | **P1** | 缓存/注册 |
| 22 | gc_api.cpp | 755 | 保持 | **P2** | 实际<1000行，无需拆 |

---

## 4. 关键文件内部分析摘要

### 4.1 gc_old_gen.cpp —— 3918 L（P0，最高优先）
单体热径 + 多子系统混合。命名空间 `chaos::il2cpp::runtime_core`，单类 `MarkSweepOldGen`。职责段：
- 虚拟内存/Init/页面分配 (61-475) → `gc_old_gen_init.cpp`
- 分配 (freelist) (708-1044) → `gc_old_gen_alloc.cpp`
- 标记 (1045-1450) → `gc_old_gen_mark.cpp`
- Sweep/合并 (1461-1712) → `gc_old_gen_sweep.cpp`
- 压缩/搬移 (1713-2676) → `gc_old_gen_compact.cpp`
- 收集主流程 (2677-3243) → `gc_old_gen_collect.cpp`
- finalizer (3364-3511) → `gc_old_gen_finalize.cpp`
- BGC + 急诊区 (3520-3918) → collect (BGC) + alloc (急诊)

收益/风险比极高（1026 行单体 → 6 个 400-700 行单元），风险中等（成员间耦合靠函数物理分离，不改对象结构）。

### 4.2 ir_reg_alloc.cpp —— 2969 L（P0）
命名空间 `chaos::il2cpp::interpreter`。拆：
- `ir_reg_alloc.cpp`（主流程：栈快照/寄存器约定/SEH 映射，~800 行）
- `ir_reg_emit.cpp`（逐指令 emit 翻译分支）
- `ir_reg_gc_helpers.cpp`（GC 写屏障 Reg_StInd/Reg_StObj + 静态字段）

### 4.3 vector_fixed_templates.h —— 2002 L（P2，保持）
纯函数模板（~120 个 `template<typename...>`），simd 向量运算必须暴露头文件。**不推荐拆**。如想缓解阅读压力，P3/可选拆成聚合头，但**不强制**。

### 4.4 gc_bgc.cpp —— 2001 L（P0）
单类 `BgcController`。拆 4：`gc_bgc_controller.cpp` / `gc_bgc_mark_worker.cpp` / `gc_bgc_finalizer.cpp` / `gc_bgc_gen1_bitmap.cpp`。

### 4.5 enum_stubs.cpp —— 2520 L（P0）
`extern "C"` ABI 导出 + 内部 static 辅助，切 4：
- `enum_stubs_common.cpp`（字符串分配/符号名缓存）
- `enum_stubs_metadata.cpp`（dispatch 表/注册/负缓存）
- `enum_stubs_tostring.cpp`（ToString/Format 簇）
- `enum_stubs_parse.cpp`（Parse/TryParse 簇）

### 4.6-4.22 其余
见 3.1 总表；各文件均按内部职责段拆分，原则相同。

---

## 5. 风险与缓解

| 风险 | 概率 | 影响 | 缓解 |
|------|:---:|------|------|
| git blame 溯源丢失 | 低-中 | 历史追溯困难 | 拆分 commit 附带 git blame 迁移提示；用 git mv 保 path 溯源 |
| 编译破坏（include/循环依赖） | 中 | 构建 red | 每个拆分独立 commit + 独立构建验证；禁止制造头循环；借 T-A clang-tidy/format 护网 |
| extern "C" ABI 符号改变 | 极低 | ABI 错 | 拆分绝不重命名导出符号；extern "C" 块整体搬运 |
| 巨型 switch 拆分逻辑回归 | 中 | 机器码错误 | EmitInstruction/Execute 拆簇只搬迁不改写；JIT 回归 + 性能基线 Δ%<5% 硬验 |
| 宏定义顺序依赖 (x64 VEX/YMM) | 中 | 预处理器错误 | 宏簇按依赖顺序整段搬；非宏函数才下沉 .cpp |
| 拆分过多碎片化 | 低 | 维护成本上升 | ≤1000 行为主；900-1000 临界不强行再拆 |
| 平台差异 (BCrypt/OpenSSL) | 低 | 条件编译错 | crypto_stubs 按平台分段拆；T-A msvc+linux clang 双验证 |

---

## 6. 建议执行顺序

- **阶段 1（P0 先驱）**：enum_stubs.cpp (风险最低) + crypto_stubs.cpp (平台隔离) → 建立 ABI-preserving split 范例
- **阶段 2（P0 单体巨头）**：gc_old_gen.cpp → jit_codegen_generate + jit_codegen_emit → interpreter_vm.cpp
- **阶段 3（P0 其余 + 头文件）**：ir_reg_alloc → gc_bgc/gc_region → x64_encoder.h/arm64_encoder.h（头文件最后，宏顺序风险最高）
- **阶段 4（P1 收尾）**：entry_direct/marshal_api/patch_loader/thread_state/jit_optimizer/gc_young_collector/gc_gen1/vtable_registry/jit_helpers
- **阶段 5（P2 复核）**：vector_fixed_templates.h 保持；gc_api.cpp 实际<1000 行保持

---

## 7. 拆分成效预估

- 拆分前超大文件：22 个（36,376 行）
- 拆分后超大文件：预期 1-3 个 → 满足 roadmap "≤5 个"
- 单文件峰值：3918 → ~700 行
- 每次拆分：纯文件移动，零逻辑改动
