# 30 — Entry SEH 捕获缺口修复

> 来源：`MEMORY.md > entry-exe-seh-catch-gap.md`

## 问题

`CHAOS_EH_TRY/CATCH` 宏默认使用 C++ `catch(const chaos_managed_exception&)`，仅捕获 `CHAOS_EH_THROW` 抛出的受管异常。AOT 编译函数体中的 SEH 访问违例通过此处理程序时不会被捕获，导致进程崩溃，即使 `/EHa` 已启用（/EHa 仅将 SEH 转换为 `catch(...)`，不转换类型化 catch）。

## root_cause

`runtime-entry.cpp` 中的派发循环 (fact/fact-json/hotupdate) 使用 C++ 类型化 catch 而非 SEH __try/__except。

## fix_strategy

使用 `__try/__except(EXCEPTION_EXECUTE_HANDLER)` 替代 C++ catch。定义为 `CHAOS_VEH_TRY` / `CHAOS_VEH_CATCH_BEGIN` / `CHAOS_VEH_END` 宏，在 entry 模板中应用。

## 适用范围

AOT 编译的 CombinedSubjects 包装方法调用框架方法（Assert 辅助方法）时可能 AV（空 thunk 派发），未来生成的代码也可能因 codegen 错误、未初始化的 GC 句柄等产生 AV。验证派发中应始终使用 SEH 捕获。

## 相关文件

任何 foundation-dll chunk 的 `runtime-entry.cpp`。

## TRIGGER

AOT 方法调用时未捕获的 SEH 异常、entry.cpp 的异常处理逻辑修改、或 foundation-dll 验证崩溃时。
