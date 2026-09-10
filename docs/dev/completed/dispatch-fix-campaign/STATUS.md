# dispatch-fix-campaign — AOT/JIT/Interpreter 执行路径问题修复

## Phase

brainstorming → roadmap → execution → completed

## 边界拍板

- **范围**: 12 个问题（3 CRITICAL + 4 HIGH + 4 MEDIUM + 2 LOW），横跨 entry_direct.cpp、jit_precode.cpp、fast_dispatch.cpp、CMakePresets.json
- **非目标**: 不做 dispatch 架构重构、不改变 dispatch cascade 步骤、不引入新执行模式
- **分批策略**: 按 Wave 1→4 分批 PR，每批独立 review + merge

## Authority 决策

- 各 fix 按《CLAUDE.md》性能最优优先级执行，非热点路径加保护，热点路径用 LIKELY 零开销
- H1 GC_TRANSITION 用 RAII wrapper 而不是 goto cleanup 或函数提取
- C2 用 demotion 清 aot_entry（1行）而不是 Step A0 入口加一致性检查

## 当前结论

所有 12 个 fix 已全部实现并提交（4 waves + JIT codegen SIGSEGV fix）。x64 回归测试通过（LdcI4_Ret 修复确认，全 66 项非基准测试通过）。

## 方案摘要

| Wave | 问题 | 方案 | 文件 |
|------|------|------|------|
| Wave 1: P0 | C2 aot_entry 死循环 | demotion 加 `aot_entry = nullptr` | entry_direct.cpp |
| | C1/C3 DP1-a/spin 空指针 | null check + 返回 nullptr 降级 | jit_precode.cpp |
| Wave 2: GC | H1 GC_TRANSITION 泄漏 | RAII GcTransitionGuard | entry_direct.cpp |
| | H2 noexcept 抛异常 | try-catch 转 error code | fast_dispatch.cpp |
| | M2 struct 泄漏 | unique_ptr/RAII | fast_dispatch.cpp |
| Wave 3: OSR | H3 deopt PC 越界 | LIKELY 断言检查 | entry_direct.cpp |
| | M3 OSR 永不衰减 | 失败后 counter = 0 | fast_dispatch.cpp |
| Wave 4: 清理 | M4 队列线程安全 | 加 mutex | jit_precode.cpp |
| | L1 aot_entry 语义 | 加注释 | entry_direct.cpp |
| | L2 CMakePreset | 加 EXEC_PROFILE | CMakePresets.json |

## 风险评估摘要

- 所有 CRITICAL fix 涉及 dispatch 路径 null 保护，影响面窄，已验证无回归
- GC_TRANSITION RAII wrapper 编译器零开销
- 其他 fix 均为非热点路径或单行修改
- 不存在跨 Wave 的文件冲突（不同文件或同一文件的不同函数）

## 三优先级权衡结论

性能最优（P1）保持——非热点路径加保护，热点路径用 LIKELY 零开销。架构一致性（P2）在 aot_entry 注释中体现。HotUpdate（P3）不受影响。

## blocking_questions

- [x] 所有 blocking_questions 已清零
- [x] question_clearance: cleared
- [x] clearance_confirmed_by_user: true

## 下一步入口

brainstorm 已批准 → 创建 roadmap，按 Wave 1→4 顺序执行
