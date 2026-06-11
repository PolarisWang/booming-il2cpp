# 31 — Shutdown Abort Longjmp 修复

> 来源：`MEMORY.md > shutdown-av-abort-longjmp-fix.md`

## 根因

`CHAOS_IL2CPP_FAIL()` / `std::abort()` 在 AOT/interpreter 派发路径中被调用。在 Win10+ 上，`abort()` 调用 `__fastfail(FAST_FAIL_FATAL_APP_EXIT)`，触发不可恢复的 `STATUS_STACK_BUFFER_OVERRUN (0xC0000409)` — 进程即使在 `__try/__except` 内也会死亡。

## 修复策略 (3 部分)

### Part 1 — `_set_abort_behavior(0, ...)`

阻止 abort 调用 `__fastfail`。替代行为：abort 触发 SIGABRT 信号，然后调用 `_exit(3)`。

### Part 2 — SIGABRT 处理器 + `longjmp`

处理器调用 `longjmp()` 跳出 `abort()`，回到 fact-json 派发循环中的 `_setjmp()` 点。循环记录方法失败并继续下一个 subject。

### Part 3 — 每次循环迭代重新注册信号

MSVC 的 `signal()` 在信号传递后将处理器重置为 `SIG_DFL`。不重新注册的话，第二次 abort 将调用 `_exit(3)` 终止进程。

## 关键模式

```cpp
for (int si = 0; si < kCount; si++) {
    signal(SIGABRT, [](int) { longjmp(t_abort_jmp, 1); });
    if (_setjmp(t_abort_jmp) == 0) {
        __try { dispatch(); }
        __except(...) { caught = true; }
    } else {
        caught = true; // SIGABRT 的 longjmp
    }
}
```

## 修改文件

- `TestProject.RuntimeEntry.cpp.scriban` — 永久模板修复
- 各生成 chunk 的 `runtime-entry.cpp`

## TRIGGER

0xC0000409 ShutdownAV 崩溃、fact 阶段进程被 abort 杀死、或修改 entry 模板的异常恢复逻辑时。
