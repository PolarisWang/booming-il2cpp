# design v1-02（重写） — 巨型 dispatch 函数拆分

> task_id: `page-payload-uniform-tu`
> 创建：2026-09-22
> 上游：`design-v1-01.md`（pps3-1 已交付）、`roadmap-v1-01.md`
> 阶段：brainstorm，待用户审查
> ⚠️ **本文件重写了 v1-02 初稿** —— 初稿基于错误的测量，见 §零

## 零、为什么重写（测量方法教训）

初稿称"三个 TU 的超标根因是 3 个巨型函数（79K/62K/47K 行）"，
**该结论数据错误**，原因是我用 python 读 `\r\n` 文件时行数被算成两倍，
且用"顶层行间距"推断函数边界，把**语句行**误当函数首行。

**可靠方法**（本次采用）：`grep -n '^extern "C"' | grep -v ';$'` 定位
真正的顶层定义行，再取相邻定义的行号差。

> **教训**：本项目已多次出现"测量方法不可靠 → 结论错误"。
> 凡涉及行数/边界，必须用 `wc -l` / `grep -n` 这类**工具原生**的方式，
> 不用自制的文本解析脚本。

## 一、实测根因（可靠数据）

> ⚠️ **2026-09-22 二次复核（本次实测，取代下方初版表格）**
> 下表初版的「行数」再次被 python `\r\n` 双计污染过。以下为 `sed -n 'A,Bp' | wc -lc`
> 的**字节+行数**双重实测值（当前 page 0 = 14.12 MB，pps3-1 交付后）：

| 区域 | 字节 | 行 | 占比 |
|---|---|---|---|
| 前导（type-id 2557 + 10,342 条 `extern "C"` 方法声明 + 杂项） | 5.66 MB | 44,848 | 40% |
| **`chaos_reflection_get_parameters_managed`** | **4.35 MB** | 62,546 | **31%** |
| 中间段 | 0.16 MB | 1,886 | 1% |
| **`chaos_reflection_resolve_method_handle`** | **1.97 MB** | 47,519 | **14%** |
| 尾部（`ChaosRegisterGcLayouts` 等） | 2.20 MB | 31,988 | 16% |

**两个巨型函数合计 6.32 MB（45%）** → 拆掉后 page 0 ≈ **8.4 MB**，
**仍略超 8 MB 目标**，需连带处理前导（见 §五 范围修正）。

分支数（`grep` 实测，非估算）：

| 函数 | 形态 | 分支数 |
|---|---|---|
| `get_parameters_managed` | 10,258 条扁平 `if (chaos_method_handle == LITERAL)` | **10,258** |
| `resolve_method_handle` | `switch(type_handle)` 89 个 case，case 内有序 `CHAOS_IL2CPP_STRCMP` 链 | **9,438** 条 STRCMP |

### 1.1 初版表格（保留供对照，行数不可信）

### 1.1 三个超标 TU 的真实构成

| TU | 大小 | 最大单元 | 数据 |
|---|---|---|---|
| **`payload.page-0001`** | 22.53 MB | `chaos_reflection_get_parameters_b3` | **10.47 MB / 135,212 行** |
| | | 第二个：`resolve_method_handle_b3` | 2.95 MB / 79,170 行 |
| | | Module registration 段（数据） | 7.22 MB / 41,279 行 |
| **`native-aot.generated.cpp`**（page 0） | 14.12 MB | `chaos_reflection_get_parameters_managed` | **111,951 行 / 约 4.2 MB** |
| | | `chaos_reflection_resolve_method_handle` | 47,384 行 |
| **`payload.page-0007`**（GC Slot Map） | 10.42 MB | `ChaosGcSlotMapsSectionV0` 结构体 | 8.54 MB |

### 1.2 共同结构

四个巨型反射函数都是 **dispatch 链**，且**分支自包含**：

```cpp
// 形态 1：扁平 if 链（get_parameters_*）
if (chaos_method_handle == static_cast<CHAOS_IL2CPP_INTPTR>(NNN))
{
    ... 构造 ...
    return chaos_array_value;
}

// 形态 2：switch(type) + 内层有序 STRCMP 链（resolve_method_handle*）
switch (chaos_type_handle) {
    case T: 
        if (STRCMP(name,"AreEqual")) return tok1;
        if (STRCMP(name,"AreEqual")) return tok2;   // ← 同条件不同值
}
```

**分支只读入参、以 `return` 结尾、无跨分支状态** → 分组自由。

## 二、方案

### 2.1 拆分模式

按**分支数均分**（用户拍板）拆成 N 个 `extern "C"` 分包函数 + 主函数退化为分派：

```cpp
extern "C" T chaos_reflection_get_parameters_b3_part0(...) { ...分支 0-999... }
extern "C" T chaos_reflection_get_parameters_b3_part1(...) { ...分支 1000-1999... }
...
extern "C" T chaos_reflection_get_parameters_b3(...) {
    // 按 handle 范围分派；无法归类的走原 default 语义
}
```

每个分包自成一个 section → 分页器可切 → 每 TU ≤ 8MB。

### 2.2 分包粒度

| 函数 | 分支数 | 建议分包数 | 每包 |
|---|---|---|---|
| `get_parameters_b3` | ~15,000 | 16 | ~1,000 分支 ≈ 0.65 MB |
| `resolve_method_handle_b3` | ~9,438 | 10 | ~940 分支 |
| `get_parameters_managed` | 10,258 | 11 | ~930 分支 |
| `resolve_method_handle` | ~9,438 | 10 | ~940 分支 |

**分包尺寸 0.65 MB ≪ 8 MB** ✅

---

## 三、🔴 风险与解决方案（本轮重点）

### R2 — STRCMP 链顺序语义（**静默错值，最高危**）

**风险**：同一 `case` 内实测有 **16 个重复的 `"AreEqual"`**，返回不同 token。
这是"取第一个命中"语义。**乱序 → 静默返回错误 token**，无编译报错、无异常。

**解决方案（三重）**：

| 层 | 措施 |
|---|---|
| **切分策略** | **以 `case` 块为最小搬迁单元** —— 同 `case` 内的 STRCMP 链**整体搬入同一分包**，绝不拆散。跨 `case` 的分包不受此约束（case 间条件不同） |
| **实现约束** | 分包时按**原文顺序** append，不排序、不去重 |
| **验证（负控）** | 构造负控：故意颠倒同 `case` 内两条 STRCMP 的顺序 → `reflection` chunk fact **必须变红**。若不变红，说明验证装置无效 |

### R3 — 分派层丢 case

**风险**：主函数分派逻辑写错 → 某些 handle 无对应分包 → 返回 0（"解析失败"）。

**解决方案**：

| 层 | 措施 |
|---|---|
| **实现** | 分派**按分支序号区间**（`partN` 覆盖 `[N*k, (N+1)*k)`），而非靠人工枚举 case —— 区间覆盖天然无缝隙 |
| **守卫** | 断言：所有 `partN` 的分支数之和 == 原函数分支总数（内容守恒，与 pps3-1 同思路） |
| **负控** | 故意让 `partN` 少覆盖一个区间 → 必须变红 |

### R4 — 分包函数命名/链接性

**风险**：命名冲突、或分包函数是 internal linkage 导致跨 TU 不可见。

**解决方案**：
- 命名：`<原函数名>_part<N>`（与 pps3-1 的 section 名 `vtable<N>` 无冲突）
- 链接性：**`extern "C"`**（与主函数一致）+ **共享头发射声明**（复用 pps2-4 登记器机制）
- 守卫：断言每个 `partN` 在共享头有对应声明

### R5 — 8MB 仍不达标

**解决方案**：分包粒度按**实测分支数**计算（见 §2.2），而非拍脑袋。
实现后**实测每个 TU 大小**，若仍超标则调小粒度。

### R6 — 三个巨型函数的 default 语义

**风险**：原函数的 `default: return 0;`（或等价兜底）在拆分后必须**恰好执行一次**。

**解决方案**：分包函数末尾**不加 default**（未命中即落入下一个分包）；
只有主函数保留 default。改动前后语义等价。

### R7（新增，审视时发现）— `get_parameters_managed` 与 `get_parameters_b3` 的关系

**审视发现**：page 0 有 `get_parameters_managed`，page-0001 有 `get_parameters_b3` ——
**两个不同函数**，但名字相近。若实现时混淆，会改错对象。

**解决方案**：拆分脚本/代码**按符号全名精确匹配**，并在 commit message 中列出实际拆分的符号清单。

### R8（新增，审视时发现）— Module registration 段（7.22 MB）连带处理

**审视发现**：page-0001 除两个巨型函数外，还有 **7.22 MB 的数据段**。
即使拆掉两个函数（13.4 MB），**该 TU 仍有 7.22 MB + 其它 ≈ 9 MB，仍超标**。

**解决方案**：
- 数据段的切分**不在本设计范围**（属 B 类的第二部分）
- 本设计**明确**：拆完函数后 page-0001 预计从 22.53 → **约 9 MB**，
  **仍超标**，需**追加数据段切分**才能达标
- 该追加项列为 **pps3-B2**（见 §五）

### R9（新增）— GC Slot Map（10.42 MB）不在本设计范围

**审视发现**：`payload.page-0007` 的 8.54 MB 是**一个 packed struct**，
不是函数链，**本设计的方法不适用**。

**解决方案**：明确归入 **pps3-C（= roadmap 的 pps3-2）**，本设计不含。

---

## 四、验收

| 验收项 | 判定 |
|---|---|
| 拆分后主函数符号仍在 | 符号存在 |
| 分包函数链接成功 | entry.exe 构建通过 |
| **每个 TU ≤ 8 MB** | 实测（**本设计单独无法达成 page-0001**，见 R8）|
| **fact 语义不变** | reflection chunk fact 无新增 failed |
| **负控成立** | R2/R3 的负控均变红 |

## 五、范围修正（审视结论）

| 子任务 | 内容 | 能否独立达标 |
|---|---|---|
| **pps3-A** | 拆 page 0 的 2 个函数 | ✅ page 0 → 预计 ≤ 8 MB |
| **pps3-B1** | 拆 page-0001 的 2 个函数 | ❌ 拆完仍 ~9 MB |
| **pps3-B2** | 切 page-0001 的 7.22 MB 数据段 | 与 B1 合并后才 ≤ 8 MB |
| **pps3-C** | GC Slot Map（8.54 MB struct） | 独立（方法不同：C2a 文本压缩） |

**结论**：原 design 的"拆 3 个函数 → 三 TU 达标"**不成立**。
page-0001 需要**函数拆分 + 数据段切分**两件；page-0007 需**另一套方法**。

## 六、推进顺序（修正）

```
pps3-1 收口（vtable+kSlots，已就绪）
  → pps3-A（page 0 两函数）           ← 可独立达标
  → pps3-B1 + pps3-B2（page-0001）   ← 必须成对
  → pps3-C（GC Slot Map）            ← 独立方法
```

## 七、问题清零

```
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: pending
```
