# Step 1.2 边界对象物化 — 调研结论与范围判定

> **日期**：2026-08-13 ｜ **来源**：Explore agent 定位物化注入点 + 人工核对返回方向
> **状态**：调研完成，范围判定为"8 项里最大最难"，建议独立立项分阶段。

## 一、物化注入点（精确清单，file:line）

**参数入口（解释器对象 → 真实 GC object）** — 4 处：
1. `precache_dispatch_raw.cpp:199-209`（argv 构建）+ `:277-288`（MIC DirectFn 直连）
2. `fast_dispatch_call.inc:208-212`
3. `ir_reg_alloc.cpp:1949-1951`(Reg_Call) / `:2017-2019`(Reg_Calli)
4. `interpreter_dispatch.cpp:103-107`

**返回入口（真实 GC object → 解释器可读）** — 4 处：
5. `precache_dispatch_raw.cpp:384-389`
6. `interpreter_dispatch.cpp:211-216`
7. `fast_dispatch_call.inc:616-618` / `:691-700`
8. `ir_reg_alloc.cpp:1984-1986` / `:2042-2044`

## 二、决定性事实

1. **解释器对象字段/元素当前只存解释器自建对象**（`InterpreterObject`/`ArrayStorage`, MALLOC）。唯一引入真实 GC 对象的是异常路径，且不落回解释器字段。
   → 回答 memory 决策问题：**当前字段不持真实 GC 对象**。Step 1.1 未覆盖的那些对象字段/数组元素 barrier（`fast_dispatch_arithmetic.inc:446`、`fast_dispatch_object.inc:107/152/224/341`、`interpreter_vm.cpp:835/858/1572/1850`）在"解释器对象闭环"下是无意义/错误的（对非GC对象打 SATB/card）。一旦做物化就可能持真实GC对象 → barrier 变必要。**这是物化设计的连带决策，不是独立撤 barrier。**

2. **物化可行面**：解释器对象→真实 GC object 有 API（`object_new`(`runtime_abi.h:164-167`)、`field_set_value`(:198-211)、`array_new`(:168-172 注意取 element_type)）；`type_token→TypeInfoHandle` 经 codegen bridge `token_resolver.cpp:142-150` `resolve_type_by_token`（可用）。

3. **反物化断头**：`RequireObject/RequireArray`(`interpreter_vm.cpp:108-121`) 无条件 `static_cast`。**无 `IsInterpreterObject`/reverse bridge**。native 返回真实 GC object 进解释器后，解释器按 `InterpreterObject`/`ArrayStorage` 布局读必然读错。判别现状仅 `type_token != 0`(interpret_method_call.cpp:173) 不可靠，但 `type_token` 在 offset 0 且用于 MIC（`ArrayStorage` 注释）——**不能简单加 magic 破坏布局**。

## 三、范围判定

- **比 Step 1.1 大一个量级**：8 处注入点 + 参数方向物化(可行) + **反物化 + 对象判别(断头，需先设计判据，且 offset 0 布局约束)** + 数组硬墙(解释器 vector vs 原生连续内存) + box。
- **反物化是硬前置**：无对象判别，反物化无法做；判别受 offset0/MIC 约束不能乱加。
- → **不建议在主线"顺手做"**。应独立立项、分阶段：
  - **1.2-a**：对象判别机制（在不破坏 offset0/MIC 前提下，如反方向用 type_token 值域/辅助表判别"解释器 vs GC 对象"）
  - **1.2-b**：参数方向物化（解释器对象→真实GC object，相对可行）
  - **1.2-c**：返回方向反物化（需 1.2-a 判别成立）
  - **1.2-d**：数组/box 特例
- **连带决策**：对象字段/数组元素 barrier 是否撤，取决于物化后字段是否持真实 GC 对象——并入 1.2 设计，不单独动。

## 四、建议

Step 1.2 从主线转独立专项。主线先推进 **Step 2.1**（blocking 调用 preemptive 切换，相对独立、低风险、能用"GC 响应/死锁窗口"给前后差异），再回 Step 1.2。
