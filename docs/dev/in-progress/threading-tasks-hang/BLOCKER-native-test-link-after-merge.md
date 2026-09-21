# 阻塞项 — native 契约测试 target 在合并 main 后无法链接

> 记录时间：2026-09-21
> 分支：`lock-ctor-wire`，HEAD = `d6a5d6cae`
> 状态：**已解决**（`d6a5d6cae`）—— 定性为 main 预存在，非本分支引入

## 〇、结论（已闭环）

修复提交 `d6a5d6cae`，改动仅 `tests/contracts/native/runtime-core/gc_test_stubs.cpp`（+40 行，纯新增）。

**验证**：`chaos_waithandle_validate_contract` **7 cases, 0 failed**（修复前根本链不上）；
`chaos_exception_sentinel_test` 6/6；`chaos_codegen_native_test` 通过；
`chaos_delegate_stress_test` 1/1。

**修法本身有一个值得记住的坑**：第一版按 `extern "C" { const T kX[1] = ...; }` 写，
**编译零诊断、零错误**，符号却完全不进 object，LNK2001 如故（排查耗时最久的一环）。
真因：**`extern "C"` 只设「语言链接」（名字修饰），不改「存储类链接」**；
namespace 作用域的 `const` 默认仍是 **internal linkage**。必须显式加 `extern` ——
与 `exception_helpers.cpp` 自己那句 "The `extern` keyword is load-bearing too" 同构。
5 行复现可判：同一段加不加 `extern`，object 符号数 **2 vs 0**。

---

## 一、现象（历史记录）

```
chaos_runtime_core.lib(exception_helpers.obj) : error LNK2001:
    unresolved external symbol kChaosExceptionTypes
chaos_runtime_core.lib(exception_helpers.obj) : error LNK2001:
    unresolved external symbol kChaosExceptionTypeCount
fatal error LNK1120: 2 unresolved externals
```

复现命令（本 worktree）：

```bash
cmake --build artifacts/presets/windows-x64-reference --config Debug \
      --target chaos_waithandle_validate_contract
```

两次运行完全一致，非偶发。

## 二、根因

`cb129e362`（main 的提交，"异常类型编译期符号表"）改了
`src/native/runtime-core/exception_helpers.cpp`，让 `ResolveTypeByName` 新增对
`kChaosExceptionTypes[]` / `kChaosExceptionTypeCount` 的 **extern 引用**
（`exception_helpers.cpp:41` 起）。

但这两个符号**只在 codegen 生成的 `ChaosGeneratedModule.cpp` 里发射**：

- `NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs:832-833`（空表分支）
- 同文件 `:855` / `:865`（非空表分支）

`ChaosGeneratedModule.cpp` 是**按项目**生成的文件。因此**任何链接
`chaos_runtime_core` 但不链接生成模块的独立 native 测试 target**，都必然缺这两个符号。
`tests/contracts/native/runtime-core/` 下的 target 全是这个形态。

## 三、定性证据（同 target 对照，非推断）

| 装置 | target | `exception_helpers.cpp` | 结果 |
|---|---|---|---|
| 合并后树 | `chaos_waithandle_validate_contract` | `cb129e362` 版 | **LNK2001 ×2** |
| **pristine main**（`b3d793b17`；移入分支的 test + threading_stubs + CMake 条目） | **同一 target** | **pristine 版** | **LNK2001 ×2（同）** |

⇒ **与分支的 shape / ATG / codegen 改动无关，是 main 自带。**

**注**：`chaos_exception_sentinel_test` 在 pristine main 上**能**链过 ——
说明 main 并非全局破损，失败只在这个 target 上暴露，即两个 target 的**依赖集不同**
（疑似 `chaos_codegen` 等库的链接差异，**未深挖**）。

### 一个被纠正的错误做法（勿重蹈）

最初我用「main 上另一个 target（sentinel）能过」当作「我的 target 失败是我引入的」——
这混淆了 **target 差异**与**文件差异**。正确做法是把分支专有文件搬进 pristine main，
让两边 target **同名同源**，才构成对照。

## 四、未受影响的验收项（已实测）

| 项 | 结果 |
|---|---|
| `dotnet build Chaos.IL2CPP.Generator` | ✅ 0 error（17 warning 均为 main 带入的 nullability/unused） |
| `atg-whitelist-guard`（managed 契约测试） | ✅ **459 checks, 0 failure** |
| 负控 A（伪造 `NativeSymbol`） | ✅ 真的转红、exit 1 |
| 负控 B（翻转断言） | ✅ 真的转红、exit 1 |
| `chaos_wait_handle_validate` 在 `.h`/`.cpp` 存活 | ✅ 各 2 处 |
| WIP 全部片段存活（`RegisterWaitHandleStatics` 接线 `Part1.cs:125`；S16 15 处；表 + `InternalsVisibleTo`） | ✅ |

**合并本身是干净的**：无冲突，且上面各项均通过。唯一损失是 native test exe 链不上。

## 五、修法（已实施 — 走的是候选 1 的既有先例）

**采用**：就地补进 `gc_test_stubs.cpp`。该文件**本就是为这一类符号存在的**
（`kChaosExternalRuntime*` / `chaos_il2cpp_aot_hotpatch_module` 同属「codegen 才提供、
测试树没有」），且由 `cmake/chaos_native_test.cmake:76` 经 `CHAOS_CODEGEN_STUB`
**自动链接进所有 native 测试 target** —— 所以只需补两条定义，无需逐个改 CMake，
也无需新增文件。

```cpp
extern const ChaosExceptionTypeEntryV0 kChaosExceptionTypes[1] = { { nullptr, nullptr } };
extern const int32_t kChaosExceptionTypeCount = 0;
```

**为何 count=0 是安全的**：`ResolveTypeByName` 的查表循环为
`for (i = 0; i < kChaosExceptionTypeCount; ++i)`，count=0 → 迭代零次 →
直接落到原有 reflection-image 路径，即 `cb129e362` **之前**的行为。

**未采用的候选**（保留理由备查）：
- 候选 2（宏开关关掉查表）：会引入「测试与生产走不同代码路径」的分叉，更差。
- 候选 3（链生成模块空实现）：需要生成模块能独立成库，未验证，成本更高。
- 新增独立 stub 文件：`gc_test_stubs.cpp` 已是 canonical 位置，另起炉灶属重复。

## 六、给接手者的下一步

1. ✅ native 链接已修复，7 cases 全绿。
2. **仍未做**：本分支**从未跑过完整 pipeline**，`19e212d30` 的 shape 是否真进派发表
   **仍未读产物验证**（memory `shape-registration-must-be-verified-by-generated-symbols`
   的判据：`grep chaos_wh_` 出现 **且** WaitHandle catch-all 消失）。
3. 之后才能谈 `realVerified ↑ 且 failed 不 ↑` 的 fact 层验收
   （见 `HANDOFF-waithandle-lock-ctor-wire.md` §四.4）。
4. **合并到 main 时须显式携带** `threading_stubs.{h,cpp}` 的
   `chaos_wait_handle_validate` / `_pair` —— main 不含该符号，而 shape 引用的正是它；
   漏带则生成物引用未定义符号（**C3861**）。
