---
task_id: ws1-jit-industrialization
task_name: JIT 工业化收官
task_type: plan
phase: completed
lifecycle_status: completed
parent_task_id: full-industrialization-roadmap
source_task_id: full-industrialization-roadmap
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
created: 2026-05-24
completed: 2026-05-24
---

# JIT 工业化收官

## Scope
将 JIT 从 2.5/5 提升至 5.0/5。覆盖 4 个实际子任务（串行执行）：
1. wf1-arch: IEncoder/ISehHandler 接口抽象 + jit_engine.cpp 重构 — ✅ **已提前完成**
2. wf1-gc: GC 精确槽映射（全量发射 + GPR 条目） — ✅ **已提前完成**
3. wf1-liveness: 开启 Liveness + 验证 — ✅ **改默认值 true**
4. wf1-osr: OSR segfault 修复（3 个向量） — ✅ **修复完成**
5. wf1-unwind: Unwind 编译守卫 — ✅ **static_assert 已添加**
6. wf1-tests: 模块级测试套件补齐 — ✅ **新增 5 个测试，总计 199 全部通过**

范围内：src/native/jit/、src/native/interpreter/（有限）、testing/src/native/jit/
范围外：Interpreter T1-T3 改造、runtime-core GC 算法改动、managed/codegen 侧 C# 编译器改动

## Inputs
- 上游设计：full-industrialization-roadmap/roadmap-v1-01.md（WS1 节完整定义）
- 当前 JIT 代码：src/native/jit/（现有实现）
- 当前测试：testing/src/native/jit/（176/181 通过）

## Expected Outputs
- IEncoder.h / ISehHandler.h 接口 ✅ （已提前完成）
- X64Encoder / WinSehHandler 实现 ✅ （已提前完成）
- jit_engine.cpp 重构 ✅ （已提前完成）
- GcSlotMapV0 含 GPR 条目 ✅ （已提前完成）
- Liveness 默认开启 ✅ （enable_liveness=true in jit_engine.h）
- OSR 无 segfault ✅ （3 个向量修复：null instr_offsets, null code, jmp rax guard）
- Unwind static_assert 守卫 ✅ （4 个 layout assertions）
- 测试通过率 >= 99% ✅ （199/199 = 100%）
- 所有 CI 回归通过 ✅

## Exit Criteria
- GC 精确槽映射就绪（nm->gc_slot_map 非空） ✅
- Liveness 默认开启，GC 精确扫描通过 ✅
- Test_OsrEntry 不 segfault ✅
- Unwind 编译守卫就位 ✅
- 全回归通过（>= 99%） ✅ （199/199）

## 子任务列表（串行）
| # | task_id | status | description |
|---|---------|--------|-------------|
| 1 | wf1-arch | completed | IEncoder/ISehHandler 接口抽象（已提前完成） |
| 2 | wf1-gc | completed | GC 精确槽映射（已提前完成） |
| 3 | wf1-liveness | completed | Liveness 改默认值 true |
| 4 | wf1-osr | completed | OSR segfault 修复 |
| 5 | wf1-unwind | completed | Unwind 编译守卫 |
| 6 | wf1-tests | completed | 测试套件补齐 |

## 修改清单

| 文件 | 修改 |
|------|------|
| src/native/jit/jit_engine.h:53 | enable_liveness 默认值 false→true |
| src/native/jit/jit_helpers.cpp | OsrResolveLoopHeader: 3 个空指针守卫 + PROFILE_SCOPE |
| src/native/jit/jit_engine.cpp | OSR entry: RAX null guard 后 jmp |
| src/native/jit/jit_unwind.cpp | 4 个 static_assert 布局守卫 |
| testing/src/native/jit/jit_native_test.cpp | 新增 GC+Liveness 联合测试（2 个用例） |
| testing/src/native/jit/jit_osr_test.cpp | 新增 OSR 边界测试（3 个用例） |
| testing/src/native/jit/jit_multi_thread_test.cpp | 新建多线程 JIT 编译压力测试（3 个用例） |
| testing/src/native/jit/CMakeLists.txt | 新增 test_jit_multi_thread 目标 |

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: cmake --build . --target test_jit_* --config Debug && run all test_jit_*.exe
  - formalObject: testing/src/native/jit/ 测试套件
  - summaryPath: n/a（单元测试级，无单独验证报告）
- test_result: passed （199/199, 100%）
- wiki: n/a
- next: archive completed → merge → WS2

## 下一步
归档 ws1-jit-industrialization 到 completed，回写父 roadmap（full-industrialization-roadmap）WS1 状态为 completed，检查 WS2（Hybrid 工业化）启动条件。
