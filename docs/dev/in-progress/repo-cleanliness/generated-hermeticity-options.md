# 生成式脚本"不污染"方案 — 分析与选型

> 2026-08-27 · 域：仓库结构/技术债
> 需求：生成式脚本（codegen 脚本 / 代码生成器）不要把产物撒到项目里污染结构。
> 本文基于**实测 inventory**，给出方案 + 权衡 + 推荐。

---

## 一、现状盘点（生成式产物的分布）

"生成式产物"目前分三类，污染形态各异：

### 类别 1：`scripts/codegen/` Python 生成器 → 写入 tracked 源树
| 生成器 | 写入 | 污染形态 |
|--------|------|---------|
| `split_fast_dispatch.py` | `src/native/interpreter/fast_dispatch_*.inc`(9 文件) | `.inc` **平铺在手写 src 旁**（fast_dispatch.cpp 同目录），无 generated/ 隔离 |
| `generate_ir_opcodes.py` | `src/native/interpreter/generated/ir_opcodes.{h,names.h}` | 已在 `generated/` 子目录（**good 模型**） |

### 类别 2：C# codegen/TPG pipeline → **362 个 `.generated.cpp/.h` 已提交在树内**
分布：`tests/unit/managed/snapshot/Baselines/**/generated/`（**代码快照测试基线**，主体）、
`tests/e2e/translation/**/native_profile/subjects/`、`tests/e2e/trust/**/generated/`、
`src/native/runtime-core/runtime_stubs/unicode_tables.generated.h`。

**关键事实**：362 个文件**当前全部干净（不 churn）**——是**静态提交**，无自动重生成，
dev 手动改 schema 后跑生成器才变。**≠ subjects.metadata 的动态漂移**。

### 类别 3：临时 scratch
`build/scripts/patch_gc_debug*.py`、`*.generated.cpp.bak*` 等 gitignored 一次性物。

---

## 二、关键洞察（决定方案方向）

- **不是所有"生成式产物"都该搬走**。`tests/unit/.../Baselines/**/generated/` 是**代码生成快照测试基线**
  ——被单测 diff 的 golden 输出，**committed 是必须的**，搬走会破坏 codegen 测试。
- `src/native/interpreter/generated/` 已经是"generated 子目录隔离"的正确模型。
- 真正的"污染"是两类：**① `.inc` 平铺在手写 src 旁** + **② e2e/trust 的生成产物与源码混放**。

---

## 三、方案（供选型）

### 方案 A：生成式产物"隔离 + 验证漂移"（推荐，低侵入）
**做法**（不动提交策略，只做**结构隔离 + 一致性守卫**）：
1. `split_fast_dispatch.py` 的 9 个 `.inc` **移入** `src/native/interpreter/generated/`（对齐
   `ir_opcodes` 的 already-good 模型），`fast_dispatch.cpp` 里 `#include` 路径同步改。
2. **校验生成与提交一致**：新增 `check_generated_up_to_date.py`（挂在 pre-commit 第 7 道，
   或以 diff 触发），对每个生成产物跑对应生成器，diff **语义等价**才放行——防止
   "改了 schema 忘了重生成"或"手工 edit generated 文件"导致漂移。
3. `.generated.cpp` 提交位置规约写进 governance：**生成物一律进 `*/generated/` 子目录**。

**收益**：结构干净（生成物全隔离） + 防漂移（守卫）。提交策略不变（快照基线仍提交）。
**成本**：低中；移 9 个 .inc + 改 include + 新增 1 个校验脚本 + pre-commit 第 7 道。
**风险**：低；.inc 移动需同步 `#include` 与可能的 build 路径引用。

### 方案 B：build 时自动重生成 + gitignore（激进，消除提交污染）
**做法**：`ir_opcodes.h` / `.inc` / codegen 产物**从 git 删出**（`git rm --cached`），改
CMake 在**构建时调用生成器**，gitignore 掉生成物，构建产物落 `build//generated/` 或
`artifacts/`。

**收益**：源树彻底无生成物；修改 schema 后 build 自动重生成，无漂移。
**成本**：高；删 362 tracked 文件（其中含**测试快照基线**，删了会破坏 codegen 单测——
须重做基线为"运行时 diff"）+ 大改 CMake + CI。
**风险**：高；快照基线移除 = codegen 测试语义变化；`ir_opcodes` 需要在编译前生成，
CMake 时序敏感。

### 方案 C：生成物白名单 + 提交位置规约（最低，纯治理）
**做法**：只把"生成物提交位置"写进规约（一律 `*/generated/` 子目录） + pre-commit
校验"新增的 `.generated.*` 必须落在 `generated/` 子目录"，否则拦。**不搬现有文件**。

**收益**：防未来新增污染；零回归。
**成本**：最低；只加 1-2 条 pre-commit 规则。
**风险**：现有 `.inc` 平铺问题不除（只防新）。

---

## 四、权衡与推荐

| | A 隔离+漂移守卫 | B build重生成 | C 白名单+规约 |
|---|---|---|---|
| 现有污染清理 | ✅(.inc 移 generated/) | ✅(全清) | ❌(不动) |
| 结构性根治 | 中 | 高 | 低 |
| 回归风险 | 低 | 高(快照基线/CMake时序) | 最低 |
| 成本 | 低中 | 高 | 最低 |

> **推荐 方案 A**：把 `.inc` 移入 `generated/` 对齐 good 模型 + 加"生成物一致性守卫"
> （pre-commit 第 7 道：生成器重跑 diff 语义等价才放行）。这是**低风险高价值**——
> 解决"生成物平铺污染" + "改 schema 忘重生成"两大实际问题，且**不触碰**必须提交的
> 快照基线。方案 B 冲击快照测试 + CMake 时序，风险过高，仅作远期。
> 方案 C 可作为 A 的兜底规则（新建生成物必须落 `generated/`）。

---

## 五、推荐落点（方案 A 拆解步骤）
1. `git mv` 9 个 `.inc` → `src/native/interpreter/generated/`；同步 `fast_dispatch.cpp`
   + `fast_dispatch_core.inc` 的 `#include "x.inc"` 路径。
2. `scripts/cleanliness/check_generated_up_to_date.py`：对每个注册生成器（.inc/ir_opcodes）
   跑生成 → 与 committed diff 语义等价才 PASS（幂等读回对比，不写）。
3. pre-commit 加第 7 道 `[generated-drift]`（hard）。
4. governance 加规约：生成物一律 `*/generated/`。
5. 快照基线（tests/unit/Baselines）**不动**——它们是测试 fixtures。
## ✅ 已执行（方案 A 核心，2026-08-27）
- `check_generated_up_to_date.py`：**全 hermetic 漂移守卫**——对注册生成器用 `--out-dir` 沙箱重跑，
  与 committed 语义 diff（whitespace-normalized，防假阳；真 content drift 照抓）。默认 warn（pre-commit
  不 brick 无关 commit）、`--fail-fast`（CI 硬门）。**守卫本身永不写入 repo**（验证过 git status 不变）。
- `generate_ir_opcodes.py` 增强：加 `--out-dir`（沙箱捕获）修复 Windows GBK YAML 读（`encoding="utf-8"`）
  + `_display()`（temp 沙箱不在 repo 下时 `relative_to` 不崩）。
- pre-commit 第 7 道 `[generated-drift]`（warn 模式）。
- governance 规约：**生成物一律落 `*/generated/`**（ir_opcodes 已是 good 模型）。

### 🔴 守卫当场抓到一个真实漂移
`src/native/interpreter/generated/ir_opcode_names.h` committed 只含 **37 个 opcode 名**，
schema 有 **112** —— **缺失 ~75 条**，疑似 schema 增长后未同步重生成（或截断旧版）。
enum `ir_opcodes.h` 是当前的（111），names 数组陈旧。**这是真实 latent bug，非我引入**。
修复 = 重生成该文件（+build 验证解释器仍编译）→ 属代码改动，单独立项，勿在本次清理顺手做。

> **验证**：守卫 test PASS(warn 0) / FAIL(fail-fast 1) / hermetic(repo 未动) / 假漂移(110→999) 照抓。
