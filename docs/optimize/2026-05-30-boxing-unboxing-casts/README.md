# 优化：boxing-unboxing-casts — InterpreterEntryDirect 路径选择重构

## 优化对象
- family: `boxing-unboxing-casts`
- 涉及模块: `src/native/interpreter/interp_entry/entry_direct.cpp`
- 涉及方法: Subject_0 ~ Subject_10（11 个 boxing/unboxing 相关方法）

## 问题根因分析

`InterpreterEntryDirect` 的 Step B（RegisterExecute，~2.5ns/op）和 Step C（FastExecute）启用了指令数 guard：

```
Step B: effective_instr_count >= 2  → 允许 RegisterExecute
Step C: effective_c_instr_count >= 2 → 允许 FastExecute
```

AOT Core IR 中所有 boxing 方法只有 **2 条指令**（LdC+Ret），这是完整的业务逻辑，不是 stub。但 guard 认为 `2 > 2` 为假，将所有 2-instr 方法从 RegisterExecute/FastExecute 排除，坠入 Step D InterpreterVM（~2500ns/op）。

**教训**: 用指令数量作为执行路径选择依据是错误的抽象。2 条 AOT Core IR 指令在 boxing 场景下代表完整的 LdC+Ret 序列，不是空壳。

## 修复方案

**基于资源可用性选择执行路径，而非指令数量启发式。**

| Step | 引擎 | 前提条件 | 旧 guard | 新 guard |
|------|------|----------|----------|----------|
| Step B | RegisterExecute | 寄存器 IR 可用 | `instr_count >= 2` | `cached_reg_method != nullptr && !instructions.empty()` |
| Step C | FastExecute | 真实堆栈 IR 可用 | `instr_count >= 2` | `reg_ir_data == nullptr && !ir->instructions.empty()` |

修改要点:
- 删除 `effective_instr_count` / `effective_c_instr_count` hack 变量
- 删除调试 `fprintf` 输出
- 删除 `CanRegisterExecute` 调用（恒 true，无实际作用）
- Step C 增加 `reg_ir_data == nullptr` 保护，防止 v2 binary path（当前未激活）使用 1-instr 占位符执行 FastExecute

## 性能数据

| 方法 | chaos-aot (ops/sec) | chaos-jit (ops/sec) | vs 修复前 | 加速比 |
|------|--------------------|--------------------|----------|--------|
| m0 | 405.0M | 399.5M | ~400K→405M | ~1000x |
| m1 | 280.3M | 304.7M | ~400K→280M | ~700x |
| m2 | 312.2M | 285.6M | ~400K→312M | ~780x |
| m3 | 373.6M | 406.8M | ~400K→374M | ~935x |
| m4 | 411.7M | 401.8M | ~400K→412M | ~1030x |
| m5 | 322.0M | 326.8M | ~400K→322M | ~805x |
| m6 | 408.2M | 399.8M | ~400K→408M | ~1020x |
| m7 | 335.2M | 327.0M | ~400K→335M | ~838x |
| m8 | 413.6M | 330.0M | ~400K→414M | ~1035x |
| m9 | 357.8M | 329.9M | ~400K→358M | ~895x |
| m10 | 293.9M | 330.8M | ~400K→294M | ~735x |
| **平均** | **355.8M** | **349.3M** | **~400K→356M** | **~890x** |

## 收敛检查

- [x] Step 4: Pipeline 全部 passed（native-bench 11/11, hotupdate_jit_fact 11/11, allSemantic=true）
- [x] Step 5: benchmark timing > 0（355M ops/sec AOT average）
- [ ] Step 6: vs .NET 8 ≤ 20%（无 managed 对比基线，benchmark-comparison-report 显示 managed_harness_unavailable）
- [x] Step 7: hotupdate semantic_changed=10 > 0
- [ ] Step 7: hotupdate overhead ≤ 100%（hotupdate AOT 仍有 ~90% overhead，需独立修复 bridge thunk 路径）
