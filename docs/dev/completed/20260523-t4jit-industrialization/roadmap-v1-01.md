# T4 JIT Native Codegen 工业化完善 — Roadmap

> task_id: 20260523-t4jit-industrialization

**目标：** 将 T4 JIT Native Codegen 模块（src/native/codegen/ 15 源文件）提升到工业化质量：补齐 GC 精确槽映射、修复 OSR segfault、加固 Linux 平台 SEH/unwind、建立模块级测试套件。

**上游设计：** brainstorm 阶段 5+1 阶段方案已获用户确认，blocking_questions = []。

**调度模式：** sequential（所有阶段共享 codegen/ 目录，有文件重叠，必须串行）

**架构审核模式：** critical

---

## 范围边界

- **范围内：** src/native/codegen/ 全部 15 源文件；gc_root_scanner.cpp 的 GcRegisterSlotMap/GcScanFrameHybrid；testing/src/native/codegen/ 测试
- **范围外：** Interpreter (T1-T3) 改动；runtime-core GC 算法改动；新指令集支持（ARM64）；Linux 运行时验证
- **全局优先级：** P1（性能最优）> P2（方案完美性）> P3（HotUpdate 支持）

## 非目标

- 不改变 interpreter (T1-T3) 的代码生成路径
- 不添加新的 x64 指令编码支持
- 不做 T4 JIT 全模块重构，只做目标问题修复
- 不要求在 Linux 上运行测试（只保证编译 + static_assert）

---

## 阶段列表

### Phase A — GC 精确槽映射
- **goal:** RecordGcPoint 发射 GPR 条目 + 注册 nm->gc_slot_map + 启用 liveness 过滤 + 移除 TODO(Phase 3d)
- **exit_criteria:** GcSlotMapV0 包含 Stack+GPR 条目；nm->gc_slot_map 非空；GcScanFrameHybrid 走精确路径；TODO 已移除
- **deliverables:** code_generator.cpp 改动 + native_method.h 清理
- **dependencies:** 无
- **resolved_decisions:** 分两步：先全量发射再开 liveness
- **watch_items:** liveness 首次启用可能暴露出框架数据不足

### Phase B — OSR segfault 修复
- **goal:** 消除 Test_OsrEntry 已知崩溃的 3 个 null 解引用向量
- **exit_criteria:** instr_offsets null 检查、0 count 边界处理、null return 防御均已添加；Test_OsrEntry 不 segfault
- **deliverables:** codegen_helpers.cpp + code_generator.cpp + codegen_native_test.cpp
- **dependencies:** Phase A（共享 code_generator.cpp，避免合并冲突）
- **resolved_decisions:** OSR 失败回退解释执行，不改变已编译代码

### Phase C — SEH Linux 加固
- **goal:** t4_seh_handler.cpp Linux 端从 `__builtin_trap` 升级为 `CHAOS_IL2CPP_FAIL`
- **exit_criteria:** Linux stub 区域替换完成；static_assert 布局校验已添加
- **deliverables:** t4_seh_handler.cpp 改动
- **dependencies:** Phase B

### Phase D — unwind_info 编译守卫
- **goal:** unwind_info.cpp 添加跨平台 static_assert 验证结构体布局一致性
- **exit_criteria:** static_assert 校验 sizeof/offsetof 关键结构体
- **deliverables:** unwind_info.cpp 改动
- **dependencies:** Phase C

### Phase E — 模块级测试套件
- **goal:** 补齐 GC slot map、OSR、unwind、SEH 的独立单元测试
- **exit_criteria:** 4 个新测试文件全部通过；CMakeLists.txt 已更新
- **deliverables:** codegen_gc_slot_map_test.cpp, codegen_osr_test.cpp, codegen_unwind_test.cpp, codegen_seh_test.cpp + CMakeLists.txt 更新
- **dependencies:** Phase A-D（测试验证已被修复的功能）

### Phase F — 集成验证
- **goal:** 全回归通过，所有测试通过，无性能退化
- **exit_criteria:** test_codegen_native 878 测试通过；新测试全部通过；test_t4_seh_handler 通过
- **deliverables:** 测试报告
- **dependencies:** Phase E
- **watch_items:** 性能基线是否退化

---

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|--------------|---------------|----------------|-----------------|
| t4jit-A | Phase A | planned | main | GC 精确槽映射完整实现 | — | seq-1 | managed proof + native proof | code_generator.cpp, native_method.h | GcSlotMapV0 含 GPR 条目; nm->gc_slot_map 非空; 精确扫描验证通过 | src/native/codegen/ | 大 |
| t4jit-B | Phase B | planned | main | OSR segfault 修复 | t4jit-A | seq-2 | native proof | codegen_helpers.cpp, code_generator.cpp, codegen_native_test.cpp | Test_OsrEntry 不 segfault; 防御检查就位 | src/native/codegen/ | 小 |
| t4jit-C | Phase C | planned | main | SEH Linux 加固 | t4jit-B | seq-3 | native proof | t4_seh_handler.cpp | Linux stub 替换为 CHAOS_IL2CPP_FAIL; static_assert 就位 | src/native/codegen/ | 小 |
| t4jit-D | Phase D | planned | main | unwind_info 编译守卫 | t4jit-C | seq-4 | native proof | unwind_info.cpp | static_assert 就位 | src/native/codegen/ | 极小 |
| t4jit-E | Phase E | planned | main | 模块级测试套件 | t4jit-D | seq-5 | managed proof + native proof | 4 个新测试文件 + CMakeLists.txt | 所有测试通过 | testing/src/native/codegen/ | 大 |
| t4jit-F | Phase F | planned | main | 集成验证 | t4jit-E | seq-6 | managed proof + native proof | 测试报告 | 全回归通过，无退化 | — | 中 |

---

## 依赖图

```
t4jit-A → t4jit-B → t4jit-C → t4jit-D → t4jit-E → t4jit-F
```

---

## 风险

| 风险 | 级别 | 阶段 | 缓解 |
|------|------|------|------|
| enable_liveness 首次启用可能缺字段 | 中 | A | 分两步：先全量发射验证精确扫描正确，再开 liveness 裁剪 |
| GC slot map 注册时机与 section 注册冲突 | 低 | A | GcRegisterSlotMap() 单点注册 vs GcRegisterSlotMapsFromSection 批量扫描，共存互不干扰 |
| Test_OsrEntry 修复后测试行为变化 | 低 | B | 从 segfault → 可预期的跳过/失败返回值 |
| 新测试发现深度 bug | 中低 | E | 记录并修复，不阻塞 Phase E 交付 |

---

## 子任务执行策略

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `recommended_next_child: t4jit-A`

---

## 验证方式

```bash
# 构建 codegen 模块
cmake --build build/testing --target chaos_codegen

# 运行已有测试（验证无退化）
ctest --test-dir build/testing -R test_codegen_native --output-on-failure
ctest --test-dir build/testing -R test_t4_seh_handler --output-on-failure

# 运行新测试（Phase E 后）
ctest --test-dir build/testing -L codegen --output-on-failure
```
