# Native 代码库规范整理 — 审计 + 可落地修改计划

> **状态**：审计完成（2026-08-10）｜ **范围**：`src/native/`（558 文件 / 127,383 行，14 子目录）
> **依据**：3 路并行审计 agent（命名/结构、注释、代码规范）+ 抽查实证。
> **目标**：按用户 5 维（代码规范 / 命名规范 / 注释清晰 / 去代号 / 文件结构清晰）产出能落地的修改计划。

---

## 0. 规模与基线（已验证）
- **规模**：jit(50) / interpreter(31) / runtime-core(309) / pal(66) / diagnostics(49) / common(34) / hot-update(4) / bootstrap(5) 等；最大单体 `jit_engine.cpp` 302KB(6383 行)、`gc_old_gen.cpp` 147KB、`ir_reg_alloc.cpp` 123KB。
- **已有格式基线**：`.clang-format`(Google/4-space/K&R/Attach) + `.editorconfig`(UTF-8/LF/4-space) **存在但未接入 CI/git hooks**（grep 无引用）→ 格式无强制。
- **整体编码风格较一致**：`chaos::il2cpp::*` 单行嵌套 namespace、PascalCase 函数、`CHAOS_IL2CPP_*` 宏、K&R 大括号、UTF-8 合法（无 GBK）。
- ⚠️ **CLAUDE.md 引用的 4 个 wiki 规范文档（`wiki/C++编码规范.md`、`wiki/调试与性能分析规范.md`、`wiki/unordered_map选型规则.md`、`wiki/SDK-TPG边界约束.md`）磁盘上不存在** — 文档一致性缺口。

---

## 1. 代码规范问题（格式/命名空间/Include/宏/分配）
| 问题 | 证据 | 风险 |
|---|---|---|
| include guard 双方案 | runtime-core 35 头用 `#pragma once`；jit/interpreter/common 用 `#ifndef CHAOS_IL2CPP_*_H_`（0 个 pragma once）；guard 宏名漂移（`jit_engine.h` 用 `CHAOS_IL2CPP_CODEGEN_CODE_GENERATOR_H_` 陈旧名）| 一致性/维护 |
| 宏重复定义 | `native_types.h:126-128` 与 `:375-378` 重复 `#define CHAOS_IL2CPP_MALLOC/FREE/REALLOC`（无 #undef）| 正确性 smell/可维护 |
| 分配绕过 | runtime-core ~343 处裸 malloc/free/realloc 家族；具体绕过统一宏：marshal_api.cpp:412,419 / memory_domain.cpp:157,190,217 / gc_card_table.cpp:38 / gc_region.cpp:49,1075 / gc_young_collector.cpp:226,322,407,631 / interop_stubs.cpp:256-310 / tls_carrier_pool.h:70 | **违反 CLAUDE.md 统一内存分配约束** |
| `CHAOS_IL2CPP_NEW` 绕过 | `native_types.h:373` `#define CHAOS_IL2CPP_NEW(T) new T` 直 new | **违反统一分配**（跨域 new/GC 不匹配风险）|
| 命名空间旧式 | `common/chaos/asan_interface.h:48`、`native_types.h:92` 用 3 行 `namespace chaos { namespace il2cpp {`，其余单行；`common/chaos/common.h:43` 遗留 `namespace ChaosIl2cpp { namespace Common = ...; }` 全局别名 | 不一致 |
| 日志规则冲突 | `native_types.h:132-134` 声明 std::printf 禁用于调试，但 `jit_engine.cpp:4398,4401,4403,4819,4824,4836,4845` 仍用裸 printf/fprintf | 与规范相悖 |
| 格式未强制 | clang-format 未接入 CI/hooks；`jit_helpers.h` 返回类型手动对齐（clang-format 会折叠）| 无回归保障 |

## 2. 命名规范问题
| 问题 | 证据 |
|---|---|
| 同名异义缩写 | `jit_engine.h:151` `rm`=RegisterMethod vs `gc_*.cpp` `rm`=RegionManager；`eeclass.cpp:53-58` `ee`/`mt` 遮蔽参数 |
| 代号型命名 | `EmitGprArithmetic`/`EmitFprArithmetic`/`EmitBitwise`/`EmitShift`（Gpr/Fpr 代号）；`EmitREXB(buf,w,rm)`（REX.B + 单字母 w/rm）；`EmitAddRIDecomposed` 等 op-specific 代号 |
| 简写局部变量 | `nm`(NativeMethod)、`gen`、`cur`、`tmp` 等无实义缩写 |
| 魔法值 | `jit_helpers.h:21` `kDeoptMagic=0xDE0D7FA57A11`（"dead fish"）无内联注释 |
| enum/member 风格分裂 | `jit_method.h:51` enum `GcSlotKind{Gpr,Stack}` PascalCase vs `NativeCodeGenerator`/`GcScheduler` snake_case 成员 |
| 魔数常量无文档 | `kDeoptMagic`、`T4`/`V1`/`V2`/`kQuick` 等代号 |

## 3. 注释问题
| 问题 | 证据 |
|---|--|
| 过时注释 | `jit_reg_alloc.h:16,108` "V1 only uses callee-saved" 但代码已用 caller+callee；`jit_engine.cpp:323` 说 V2 replaces V1 |
| 代号未展开 | `jit_engine.cpp` 大量 `V1 cache`/`V2`、`T4`/`T2.2-B`/`T2.2-C1` 里程碑 tag 无内联定义 |
| 魔法值语义不贴码 | `kDeoptMagic` 含义分散在 jit_helpers.h:82,203 / jit_engine.cpp:1193，不在此常量旁 |
| 断句/乱码 | `gc_card_table.cpp:82` `// ops), no asm...` 缺前句；`gc_old_gen.cpp:2809,953,1287,1539` banner 含 `�` 替换字符 mojibake |
| 术语不一致 | GC 混用 `Gen1`/`gen1`/`Gen 1`、`nursery`/`young-gen`；weak-handle 流程 `Publish` vs `GcProcessCollectedWeakHandles` |
| 自动生成标记疑点 | `fast_dispatch_*.inc` "DO NOT EDIT — regenerated" 但生成源 `fast_dispatch.cpp` 已变 19 行 unity 聚合器（手改历史证据）|
| 深逻辑缺注释 | `jit_reg_alloc.h:507-562` simplify/select 死分支无解释；FPR simplify/select 只一行 header |

## 4. 文件结构问题
| 问题 | 证据 |
|---|---|
| 超大单体 | `jit_engine.cpp` 302KB(6+ 职责：regalloc/cache/GC/deopt/SEH/unwind/SIMD)；`gc_old_gen.cpp` 147KB(~13 阶段)；`ir_reg_alloc.cpp` 123KB（regalloc+解释器+frame）| ← T2.4 monolith 拆分 |
| 重复实现 | `ReclaimDemotedCode` 在 jit_seh.cpp:79 与 jit_seh_handler_internal.cpp:140 **互相矛盾**（一个 free、一个 no-op）；Win/Linux SEH handler ~1600 行近重复垂直实现 |
| 误导 shim 头 | `interpreter/register_vm.h`(474B) 纯粹 re-include `ir_reg_alloc.h` |
| 头文件名大小写重复 | `Arm64Encoder.h` vs `arm64_encoder.h`（同一概念两文件）；`x64_encoder.h` 无对应接口头 |
| 僵尸文件 | `runtime-core/com_ccw.cpp.phase2`、`runtime-core/gc/Z0_tmp.h`（WIP 残渣）|
| include 跨层 | `jit_engine.h:22` 跨 cluster include `../interpreter/ir_reg_alloc.h`；register_vm.h shim 掩盖潜在循环 |

---

## 5. 可落地修改计划（分层）

> **原则**：分层实现，每层绿基线护栏（jit 15/15 + jit_native 69/69 + gc_slot_map 16/16 + SEH 38/38 + unit 不回归）。

### 阶段 0 — 文档一致性（低风险，先做）
- **补 4 个缺失规范文档**（wiki 或 docs/dev）：`C++编码规范.md`、`调试与性能分析规范.md`、`unordered_map选型规则.md`、`SDK-TPG边界约束.md`，并把 CLAUDE.md 引用路径对齐到真实位置。
- 建立统一的 **native 编码规范 v1**（命名空间/Include/guard/命名/macro/分配/日志/注释约定），作为后续修复的依据。

### 阶段 1 — 低风险机械清理（clang-format 一刀切，不改语义）
- **全量跑 clang-format**（`src/native/**/*.{cpp,h}`），差异仅格式化 → 统一缩进/大括号/命名空间注释，消除 `jit_helpers.h` 手动对齐。
- **修复 3 处 tab**：`jit/LinuxSehHandler.cpp:498`、`jit_reg_alloc.h:105`、`native_types.h:388-396` block `#define`。
- **删除僵尸文件**：`com_ccw.cpp.phase2`、`Z0_tmp.h`（先 git ls-files 确认无引用）。
- **修复 2 处旧式 namespace**：`asan_interface.h:48`、`native_types.h:92` 单行化。

### 阶段 2 — 规范化修复（小范围语义无关）
- **去重宏**：`native_types.h` 合并重复 `CHAOS_IL2CPP_MALLOC/FREE/REALLOC`（用 #undef 或去一处）。
- **统一 include guard**：选定 `#pragma once`（或 `#ifndef`）→ 抽 clang-tidy 脚本批量迁移；修正 `jit_engine.h`/`Arm64Encoder.h` 陈旧 guard 名。
- **清理日志违规**：`jit_engine.cpp` 裸 printf → 统一 `CHAOS_IL2CPP_LOG_*`。（属调试规范）
- **命名规范化（低优先级、跨 ABI 敏感）**：`rm`/`nm`/`gen`/`cur` 等简写局部 → 语义化；enum 成员风格统一。**注意 extern "C" C-ABI 符号名（`Codegen*`/`Jit*`/`kChaosExternal*`）改起来 break ABI，需列入专门决策**（或只改注释/文档，不动符号）。

### 阶段 3 — 去代号（需人工语义确认，分文件）
- per 文件把 `EmitGprArithmetic`→`EmitGprArithmetic` 仍有代号，重命名为 `EmitGenericArithmetic`（含 GPR/FPR 统一）/ 或 `EmitIntArithmetic`；`EmitREXB(buf,w,rm)`→ 具名参数/注释展开 `REX` prefix 含义；`kDeoptMagic` 旁加含义注释或改 enum 命名。
- `V1`/`V2`/`T4`/`T2.x` 代号 → 首次出现处加"什么是 V1/T4"的内联展开或统一术语表（放 `docs/dev` 或规范文档）。
- **风险**：重命名符号涉及面广，建议先用 grep 建立符号引用清单，分批提交，每批跑全量测试。

### 阶段 4 — 文件结构
- **T2.4 monolith 拆分**（已在路线图，绿基线已就位）：`jit_engine.cpp` 302KB → 按 `[[jit-engine-modularization]]` 8 模块（regalloc/gc/unwind/emit/tier/osr/seh/encoding）**纯移动不改逻辑**。
- **收编重复**：`ReclaimDemotedCode` 双实现合一（以 方案3 no-op 语义为准，删 jit_seh.cpp 或 internal 之一）+ 去重；Win/Linux SEH handler 归一（抽公共层，保留 OS framing）。
- **删 shim**：`register_vm.h` → 调用方直接 include `ir_reg_alloc.h`；合并 `Arm64Encoder.h`/`arm64_encoder.h`。
- **解决跨层 include**：`jit_engine.h` → `ir_reg_alloc.h` 依赖按 T2.4 拆分时纳入模块边界。

---

## 6. 优先级/工作量粗估
| 序 | 阶段 | 风险 | 估时 |
|---|---|---|---|
| 1 | 0 文档一致性 + 规范 v1 | 无 | 0.5-1d |
| 2 | 1 clang-format 全量 + 僵尸清理 | 低（纯格式） | 0.5d |
| 3 | 2 宏/guard/日志/命名小修 | 低-中 | 1-1.5d |
| 4 | 3 去代号（分批，测试护栏） | 中（符号面广） | 1.5-2.5d |
| 5 | 4 文件结构（T2.4 + 去重/shim） | 中-高（结构） | 2-4d（T2.4 单独计）|

**总原则**：阶段 0/1 低风险立即值；阶段 2/3 需分批 + 全量测试；阶段 4 T2.4 复用既有 A-2 绿护栏。**C-ABI 符号重命名（破坏 dll 边界）须单独立项评估**，默认只改文档/注释不动符号名。

**验证锚**：每阶段后 `ctest -L jit` 15/15 + `test_jit_native` 69/69 + `jit_gc_slot_map` 16/16 + `jit_seh_handler_internal` 38/38 + `python tests/runner/test_driver.py --layer unit` OVERALL OK（snapshot 2-fail 为 pre-existing Generator 漂移）。

## 阶段 0-3 落地记录（2026-08-10/11，全部绿）
- 阶段0 `113c404df` v1规范+CLAUDE.md 引用修复
- 阶段1 `9e84ed513` clang-format(jit+interp 81文件)+僵尸清理+OSR对齐方案3
- 阶段2 `fede790ad` 宏去重 / `85ccb6d41` guard统一+printf清理 / `552e45204` GC术语
- 阶段3a `093cd6b04` 注释展开(kDeoptMagic/V1/V2/T4/EmitREXB) / 3b `0ab28fabe` EmitGprArithmetic→EmitIntegerArithmetic + 修T2.2遗留ABI 31bit断言
- 阶段4(安全项) `c47cfa5a3` 删 register_vm.h shim

## T2.4 jit_engine monolith 拆分（当前准确边界, 6426行, 2026-08-11）
关键前提：`NativeCodeGenerator` 类**在 jit_engine.cpp:171-448 定义**（jit_engine.h 不声明它）。第一步必须把类声明+全部成员移到 jit_engine.h，让分拆的各 `.cpp` TU 都能访问成员。然后按方法切：
| 模块 | 方法 | 当前行 |
|---|---|---|
| arith | EmitIntegerArithmetic/EmitFloatingArithmetic/EmitBitwise/EmitShift | 848-1110 |
| branch | ResolveBranches | 1111-1171 |
| deopt | EmitDeoptSequence | 1172-1235 |
| memory/cache | SelectCacheableRegs/SpillCachedRegs/SpillGcRefCachedRegs/EmitCallWithSpill/EmitRuntimeHelperCallImpl | 1236-1450 |
| gc | RecordGcPoint/EmitSafepointPoll/EmitInlineDirtyCard/PropagateTypes | 572-847, 1451-1560 |
| emit(dispatch) | EmitInstruction(2000行 switch 含 SEH/SIMD/PIC/CallVirt inline) | 1561-3649 |
| simd | EmitSimd | 5585-6426 |
| prologue/generate | Generate() + Compile/CanCompile + OptimizeInstructions(static) | 3650-5584 |
风险：EmitInstruction/Generate 读 `buf_`/`gcr_`/`slot_patches_`+30+成员，需类声明进头；核心难点=把 2000 行 switch 拆出去需要 member 全可见。纯移动不改逻辑，估 5-7d，需专用 session 逐块 15/15+69/69 验证。

