# Chaos IL2CPP — Native C++ 编码规范 v1

> **状态**：v1（2026-08-10，基于 3 路审计 + 现有 `native-code-conventions.md`/`03-CPP原生代码约定.md` 整合）
> **定位**：本项目 native 代码的**单一权威规范**。本文档描述**代码当前实际遵循**的约定，并列出**已知偏差**（后续阶段修复）。
> **关联**：CLAUDE.md 第〇条/编码规范表、`.clang-format`、`.editorconfig`、`wiki/.../native-code-conventions.md`（待本文档替换）

> ⚠️ **取舍原则**：当旧规范与已发布代码冲突时，**以代码为准**（改全库命名成本远高于改文档）；规范则描述实际 + 标注偏差由后续阶段逐步收敛。

---

## 1. 命名规范

| 类别 | 实际约定 | 反例（需修） |
|---|---|---|
| 类 / struct / enum | PascalCase | — |
| 枚举值 | PascalCase | `GcSlotKind{Gpr=0,Stack=1}` 已是 PascalCase；但注意成员风格与类内 snake_case 分裂（阶段2修） |
| 函数 | PascalCase | 旧规范写 snake_case → **以代码为准 PascalCase** |
| 成员变量 | `name_`（尾下划线） | `CompileConfig` 公共字段无尾下划线（已知偏差） |
| 全局 / extern | `g_` 前缀 | `g_t4_code_entries`、`g_reverse_slot_map` |
| 常量 | `k` + PascalCase（`kDeoptMagic`、`kRegHasDst`） | 旧规范写 `k_`+snake → **以代码为准 kPascal** |
| 宏 | `CHAOS_IL2CPP_*` | `CHAOS_IL2CPP_MALLOC`、`CHAOS_IL2CPP_VECTOR`；codegen 输出必须用 |
| 命名空间 | `chaos::il2cpp::<module>` 单行嵌套 | 旧式 3 行（asan_interface.h:48、native_types.h:92 待修） |
| header include guard | `#ifndef CHAOS_IL2CPP_{MODULE}_H_` | runtime-core 用 `#pragma once`（35 头，已知偏差待统一） |
| C-ABI 符号 | `Codegen*` / `Jit*` / `kChaosExternal*` / `Chaos*`（**保留不动**，详见 §4） | — |

**禁止**：`nm`/`rm`/`gen`/`cur` 等无实义单/双字母缩写（阶段2清理，见审计）；`using namespace std;`、`using namespace chaos;`、`NULL`，裸 `int`/`long`（用 cstdint）。

## 2. 注释规范

- **必须**：非平凡函数/类/算法（注册分配器、GC、deopt）缺注释者补；复合运算符（`Shl` 移位计数、`Ovf` 溢出）标注操作数语义。
- **禁止代号注释**：`V1`/`V2`/`T2.x`/`T4`（里程碑 tag）需首次出现时展开（如 `V1` = 频率缓存方案、`T4` = 第4层 JIT 生成代码），或统一术语表。
- **魔法值必须贴码注释**：`kDeoptMagic=0xDE0D7FA57A11`（"dead fish" sentinel）旁须说明含义与触发路径。
- **禁止过时注释**：注释须与实现同步；`V1 only uses callee-saved` 等已不符代码者修正（阶段3）。
- **无乱码/断句**：`gc_old_gen.cpp` 的 `�` mojibake、`gc_card_table.cpp:82` 断句注释待修。
- **术语统一**：GC `Gen1/gen1/Gen 1`、`nursery/young-gen`、weak-handle `Publish/Collect` 择一（阶段2 定词表）。
- 中文注释允许但须 UTF-8 合法，避免与 box-drawing `──` banner 混入乱码。

## 3. 格式

- 依据 `.clang-format`（Google / 4 空格 / K&R Attach）+ `.editorconfig`（UTF-8、LF、4 空格、最后换行、去尾空格）。
- **全量跑 clang-format 作为阶段1**（统一缩进/大括号/命名空间注释，消除 `jit_helpers.h` 手动对齐）。
- `FixNamespaceComments:true`：命名空间闭合注释统一 `// namespace chaos::il2cpp::jit`。

## 4. ABI 边界（关键约束）
- C-ABI 跨模块符号（`Codegen*`/`Jit*`/`kChaosExternal*`/`Chaos*`、`__register_frame`）**为 dll/module 边界，重命名会破坏 ABI → 一律保留不动**。如需改进命名，只加注释/文档，不改符号。
- 新增 C-ABI 符号用统一前缀（建议 `Chaos*` 或 `kChaosExternal*`，避免 Codegen 混用）。

## 5. 内存与宏
- **统一分配**：codegen/runtime 分配必须用 `CHAOS_IL2CPP_MALLOC/FREE/REALLOC/NEW_GC`，**禁止裸 `new`/`delete`/`std::malloc/free`** 跨域（见 CLAUDE.md 统一内存分配约束）。
- **已知违规待修（阶段1/2）**：
  - `native_types.h:373` `CHAOS_IL2CPP_NEW(T) new T` 直 new（绕过领域/GC 分配器）。
  - `native_types.h:126-128` 与 `:375-378` 重复 define `CHAOS_IL2CPP_MALLOC/FREE/REALLOC`。
  - runtime-core 裸 `std::malloc/free/realloc`：marshal_api.cpp:412,419 / memory_domain.cpp:157,190,217 / gc_card_table.cpp:38 / gc_region.cpp:49,1075 / gc_young_collector.cpp:226,322,407,631 / interop_stubs.cpp:256-310 / tls_carrier_pool.h:70。
- **日志**：`native_types.h` 声明禁 std::printf 调试，须用 `CHAOS_IL2CPP_LOG_*`；`jit_engine.cpp:4398,4401,4403,4819,4824,4836,4845` 裸 printf 待修。

## 6. 已决策 / 下一步（映射到计划）
| 阶段 | 内容 |
|---|---|
| 0 | **本文档确立**（v1）+ 修 CLAUDE.md 断裂引用 |
| 1 | 全量 clang-format；删僵尸文件 `com_ccw.cpp.phase2`、`Z0_tmp.h`；修 tab/旧 namespace |
| 2 | 去重宏、统一 guard（`#pragma once` vs `#ifndef`）、清 printf、简写命名、GC 术语定词 |
| 3 | 去代号（EmitGprArithmetic→语义名、kDeoptMagic 注释、V1/T4 术语表） |
| 4 | 文件结构（T2.4 monolith 拆分 + 去重 ReclaimDemotedCode + 归一 Win/Linux SEH + 删 register_vm.h shim） |

## 验证锚
每阶段后：`ctest -L jit` 15/15 + `test_jit_native` 69/69 + `jit_gc_slot_map` 16/16 + `jit_seh_handler_internal` 38/38 + `python tests/runner/test_driver.py --layer unit` OVERALL OK。
