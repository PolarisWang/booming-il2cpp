# HelloWorld app-mode 运行时专项：e0000001 托管异常

> 2026-08-31 · 域：runtime-core / codegen 外部运行时 helper
> 交接给 runtime 领域，独立跟踪

---

## 1. 背景

`chaos-il2cpp publish <HelloWorld.dll> --mode app` 已能产出 `entry.exe`，运行时初始化、`ChaosRuntimeHost::Initialize()`、`ChaosGeneratedModuleActivate()` 全部通过（GC 低内存挂起也已通过 `g_low_mem_enabled=false` 修复）。

**剩余阻塞**：`HelloWorld_Program::Main(0)` 调用后，抛出 `0xE0000001`（`kChaosManagedExceptionCode`，托管异常码），导致进程终止（exit 127）。

---

## 2. 证据

### 2.1 运行日志
```
[INFO][GcLowMem] low-memory monitor disabled (g_low_mem_enabled=false)   ← GC 修复生效
[app_main][DIAG] step3: module activated                                ← Activate 通过
(1890.62ec): Unknown exception - code e0000001 (first chance)           ← Main(0) 抛托管异常
(1890.62ec): Unknown exception - code e0000001 (!!! second chance !!!)  ← 未被捕获，进程终止
```

### 2.2 异常码定义
```
src/native/runtime-core/exception_jmp.h:92:  kChaosManagedExceptionCode = 0xE0000001
src/native/jit/WinSehHandler.cpp:427:       kManagedSehExceptionCode = 0xE0000001
```

### 2.3 codegen 中可能的 raise 点
HelloWorld 的 Main 里：
- `$"args={args.Length}"` → `chaos_default_interpolated_string_handler_*` 系列外部 runtime helper
- `Console.WriteLine(...)` → `chaos_external_runtime_...WriteLine`（已实现打印）
- `RegistrationTrace.Marks` 静态字段 → `chaos_static_...Marks`（可能 null → `raise_null_reference_exception`）

`0xE0000001` 最可能来自 `chaos_runtime_get_abi_v0()->raise_null_reference_exception()`（codegen 里多处 `if (chaos_arg_0 == 0) raise_null_reference`）。

---

## 3. 待排查方向（独立 runtime 专项）

1. **裸调 vs dispatch 差异**：`app_main.cpp` 用 `HelloWorld_Program::Main(0)` 裸调 codegen 函数，而 foundation-dll harness 用 `ChaosDispatchMethod`（有完整异常/ABI 封装）。裸调可能缺「异常分派上下文」（VEH handler 栈、EH jmp_buf）。
2. **外部 runtime helper stub**：`chaos_default_interpolated_string_handler_*`、`ChaosStringConcat2` 等是否完整实现，还是返回 0/抛异常。
3. **静态字段初始化**：`chaos_static_HelloWorld_RegistrationTrace__Marks` 是否为 null（若未初始化则 raise null reference）。
4. **args 传入**：`Main(0)` 传 null string[]，若 codegen 在某处解引用（虽有 null 保护），或 `args[index]` 访问越界。

---

## 4. 建议修复方向

- **优先定位 raise 点**：用 cdb 在 `e0000001` 处断 `!analyze -v` 拿调用栈，确认是哪个 Helper/字段抛的。
- **对比 runtime-entry.cpp**：看 foundation-dll 怎么建立异常上下文，app_main 需要补什么。
- 最可能快速修的是「传入空托管 string[]」而非 null（消除 args 相关异常），需要 `CreateEmptyStringArray` helper（runtime 提供）。

---

## 5. 已交付（此前 commit）

| commit | 内容 |
|--------|------|
| `e4bed035a` | publish Phase 1-4 架构（ConvertService + PublishController + app_main + CMake） |
| `4660d7ac3` | GC gate：app 模式禁用 BGC + low-memory，消除 safepoint 挂起 |
| （已含） | Console.WriteLine stub 实现 + codegen S6.cs 打印逻辑 |

## 6. 验收标准（专项完成条件）

`chaos-il2cpp publish HelloWorld.dll --mode app --clean` 产出的 `entry.exe`：
1. 正常运行，**不抛 e0000001**
2. 打印 `HelloWorld smoke entry reached.`
3. 正常退出（exit 0）
