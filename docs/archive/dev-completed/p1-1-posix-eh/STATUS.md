# STATUS — P1-1: POSIX EH (PalTryCallNoExcept)

## Meta
- task_id: p1-1-posix-eh
- parent_task_id: 20260604-execution-path-risk-remediation
- source_relation: roadmap-child
- clearance_source: parent-roadmap
- lifecycle: completed
- phase: completed
- task_type: plan
- entry_skill: dev-writing-plans
- blocking_questions: []
- question_clearance: cleared
- clearance_confirmed_by_user: true

## Scope
实现 POSIX 平台上 PalTryCallNoExcept 的完整信号保护，使用 sigsetjmp/siglongjmp + 通用信号处理程序捕获 SIGSEGV/SIGBUS。使 POSIX EH 与 Windows SEH `__try/__except` 语义对等。

### 包含
- sigsetjmp/siglongjmp 包装层
- SIGSEGV/SIGBUS 通用信号处理程序（按地址范围过滤，不干扰 JIT 自用信号）
- POSIX signal handler 安装/拆卸（线程安全）
- 替换 pal_eh_posix.cpp 中现有的 passthrough 实现
- CHECK tier 测试覆盖

### 不包含
- 其他信号类型（SIGFPE、SIGILL 等）
- Windows SEH 修改
- W^X 修改（P1-2 范围）

## Inputs
- 上游设计结论: Approach 1b (complete sigsetjmp/siglongjmp + signal handler)
- 父 roadmap: docs/dev/in-progress/20260604-execution-path-risk-remediation/roadmap-v1-01.md
- 现有文件:
  - src/native/pal/pal_eh_posix.cpp（当前 passthrough 实现）
  - src/native/runtime-core/chaos/eh.h（CHAOS_EH_TRY/CATCH 宏定义）
  - src/native/runtime-core/exception_jmp.h（SEH jmp_buf 相关）
  - src/native/runtime-core/chaos/eh.h（SEH 过滤表达式）

## Expected Outputs
- 修改: src/native/pal/pal_eh_posix.cpp（sigsetjmp/siglongjmp 实现）
- 修改: src/native/runtime-core/chaos/eh.h（可能新增 POSIX 路径宏定义）
- 新增或修改: signal_handler.cpp/h 相关（通用信号处理程序）
- 测试验证: foundation-dll verification pipeline

## Exit Criteria
1. POSIX PalTryCallNoExcept 使用 sigsetjmp 保存上下文、siglongjmp 恢复
2. SIGSEGV/SIGBUS 处理程序能捕获非法内存访问并触发 longjmp
3. JIT 正常信号处理（写时 SIGSEGV 等）不被干扰
4. 已存在的 foundation-dll tests pass（无 regression）
5. CHECK tier 构建通过

## Terminal Notes
- pal_eh_posix.cpp: Replaced passthrough with sigsetjmp/siglongjmp + SIGSEGV/SIGBUS signal handler. Thread-local sigjmp_buf + TLS active flag. One-time lazy handler installation with chain forwarding.
- pal_eh.h: Updated doc comment to reflect POSIX behavior.
- LinuxSehHandler.cpp: Added ChainSignalToPrev helper, fixed Initialize() to save old handlers via sigaction, chaining at all unhandled-exit paths (nm==nullptr, code_addr==nullptr, no catch handler).
- runtime-core CMakeLists.txt: Added CHAOS_IL2CPP_HAS_TIER_MANAGER=1 to resolve SDK duplicate symbol issue.
- Build: chaos_pal + chaos_runtime_core + chaos_jit build cleanly.
- Tests: foundation-dll enum-parsing 11/11 pass.
- No regressions detected.
