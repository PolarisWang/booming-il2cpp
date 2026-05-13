# Native C++ 工程规范综合评估报告 v2

**评估日期：** 2026-05-13
**评估范围：** `src/native/` — 178 个源文件（.h + .cpp）
**评分版本：** v2（基于 v1 修复后状态）

---

## 摘要

工程规范经过全量修复后，综合评分从 **3.0/5 → 4.4/5**。命名空间分裂、Include 风格混用等核心问题已解决。主要遗留问题集中在 `architecture.yaml` 未同步、GC 区域代码内存分配宏使用不规范、以及少量头文件缺少 include guard。

| 维度 | 说明 | v1 评分 | v2 评分 | 变化 |
|------|------|---------|---------|------|
| A. 命名空间一致性 | namespace 声明规范 | 2.0 | 5.0 | +3.0 |
| B. Include 风格 | 统一 `<chaos/>` 禁止相对路径 | 2.0 | 4.5 | +2.5 |
| C. 构建与架构治理 | CMake + architecture.yaml | 3.0 | 3.5 | +0.5 |
| D. 文件结构 | header guard, clang-format | 3.0 | 4.0 | +1.0 |
| E. 内存分配合规 | CHAOS_IL2CPP_ 宏使用 | 3.0 | 3.5 | +0.5 |
| F. 文档与规范记录 | CLAUDE.md + wiki | 4.0 | 5.0 | +1.0 |
| G. extern "C" 规范 | ABI 注释文档化 | 3.0 | 4.5 | +1.5 |
| H. 快照/回归保护 | codegen 基线不变 | 5.0 | 5.0 | — |
| **综合** | | **3.0** | **4.4** | **+1.4** |

**核心指标：**
- 编译：6 个 native 库全部通过 ✓
- 测试：74/74 快照通过 ✓
- 验证：foundation-dll array-indexing-copy fact 14/14 ✓

---

## 详细评估

### A. 命名空间一致性 — 5.0/5

**状态：已修复（关键成果）。**

- `ChaosIl2cpp::Common` → `chaos::il2cpp::common` 全部 19 个头文件 + 5 个 .cpp + 跨模块引用已替换
- 零残留 `ChaosIl2cpp::Common` 引用（grep 验证通过）
- 向后兼容别名 `namespace ChaosIl2cpp { namespace Common = chaos::il2cpp::common; }` 已在 `common.h` 中添加
- 所有模块命名空间一致使用 `chaos::il2cpp::<module>` 小写形式

### B. Include 风格 — 4.5/5

**状态：基本修复。**

- 8 处 `"chaos/"` 引号风格 → `<chaos/>` 尖括号风格 ✓
- 7 处 `"../"` 相对路径全部消除 ✓

**⚠ 1 处豁免：**
- `bootstrap.cpp:15` — `#include "../runtime-core/runtime_instantiation.h"`（带注释：与 `contracts/native/v0/runtime_instantiation.h` 文件名冲突）。合理豁免。

### C. 构建与架构治理 — 3.5/5

**已修复：**
- `runtime-core/CMakeLists.txt` 已添加 `${REPO_ROOT}/src/native` PRIVATE include 路径 ✓
- 7 个 CMakeLists.txt 管理正常 ✓

**未修复（原评估等级，P0）：**
1. **`build/architecture.yaml:18` 引用旧命名空间** — `ChaosIl2cpp::Common` 需要更新为 `chaos::il2cpp::common`
2. **benchmark-host/ 和 proof-host/ 不在 architecture.yaml 中** — 5 个源文件未被架构规范覆盖
3. **architecture.yaml 缺少 chaos::il2cpp::common sub-namespace 声明**

### D. 文件结构 — 4.0/5

**通过项：**
- `.clang-format` 正确配置 IncludeBlocks Regroup + SortIncludes ✓
- 20/23 头文件使用 `#ifndef CHAOS_IL2CPP_*_H_` 风格 ✓

**3 个头文件缺少 include guard（P0）：**
| 文件 | 问题 |
|------|------|
| `common/chaos/json_reader.h` | 无 include guard |
| `runtime-core/aot_core_ir_reader.h` | 无 include guard |
| `runtime-core/reflection_metadata_impl.h` | 无 include guard |

### E. 内存分配合规 — 3.5/5

**合规：**
- 无全局 `operator new/delete` 重载 ✓
- 运行时代码正确使用 `CHAOS_IL2CPP_MALLOC/FREE` ✓
- `native_types.h` 定义了全套宏 ✓

**4 处偏离（P1）：**
| 位置 | 裸调用 | 应使用 |
|------|--------|--------|
| `gc_region.cpp:187` | `std::malloc(size)` | `CHAOS_IL2CPP_MALLOC` |
| `gc_region.h:173` | `std::malloc(size)` | `CHAOS_IL2CPP_MALLOC` |
| `gc_layout.cpp:33` | `std::malloc(...)` | `CHAOS_IL2CPP_MALLOC` |
| `interpreter_entry.cpp:537` | `new uint8_t[N]()` | `CHAOS_IL2CPP_NEW_ARRAY` |

### F. 文档与规范记录 — 5.0/5

**已全部完成：**
- `CLAUDE.md` 新增 5 个工程规范章节 ✓
- `wiki/03-架构约束/03-CPP原生代码约定.md` 已更新 ✓
- `wiki/04-历史决策/20-工程规范统一决策.md` 已记录 ✓

### G. extern "C" 规范 — 4.5/5

**状态：基本修复。**
- 所有 extern "C" 文件顶部有 ABI 注释块 ✓
- runtime_stubs/*.cpp 的 extern "C" 块已注释 ✓

### H. 快照/回归保护 — 5.0/5

- 74/74 快照测试通过 ✓
- foundation-dll array-indexing-copy 14/14 fact 通过 ✓

---

## 剩余改进路线图

| 优先级 | 问题 | 维度 | 修复难度 | 预期评分 |
|--------|------|------|----------|----------|
| P0 | architecture.yaml 引用旧命名空间 + 缺少 benchmark-host/proof-host | C | 5 分钟 | 4.4→4.5 |
| P0 | 3 个头文件缺少 include guard | D | 5 分钟 | 4.0→5.0 |
| P1 | GC 区域代码 std::malloc → CHAOS_IL2CPP_MALLOC | E | 30 分钟 | 3.5→5.0 |
| P2 | interpreter_entry.cpp 裸 new[] | E | 5 分钟 | — |

**以上全部修复后，评分可达 5.0/5。**