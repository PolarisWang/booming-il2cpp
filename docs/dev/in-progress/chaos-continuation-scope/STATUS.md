# chaos_continuation 作用域缺陷 — 修复交接

> **2026-09-13 更新**：C2065 修复**已落盘** `bb9a02fd0`（详情见下）。
> 后续的 `chaos_noop_void` C3861 也已落盘 `711afc07d`。
> **build 阶段已通过**（text chunk 1/1），但**新的阻断项**出现 —— 见文末
> 「🔴 新阻断：AOT entry.exe 启动即 SIGSEGV」。
> **⚠️ 该阻断的真正根因是陈旧 `crt_stubs.cpp`（已由 `d5d0630da` 修复），
> 不是当初以为的 ABI 值/指针错位 —— 见文末「✅ 已解决」。**

**状态**：修复已在工作区（未提交，见 fix.patch），编译通过；回归测试**已写但未跑绿**，原因已定位。

## 缺陷（原始 C2065）

`EmitPcDispatch` 写 `chaos_continuation` 的守卫（`exitTarget >= 0`，dispatch 自身属性）与
`EmitIRExceptionRegion` 声明它的守卫（`er.RegionExitTargetOffsets`，**region** 属性）不一致。

更深一层：**声明只存在于 `TryCatch` 分支**，`TryFinally` / `TryFilter` 在任何条件下都不声明。
而 `BuildFinallyOnlyExceptionIRBody` 等 5 个构造点**都不传 `exitTargetOffsets`**。

实测触发：`Chaos_TestFramework_...Assert_AreEqual_0_0_System_String`，
两层嵌套 `CHAOS_EH_TRY_FINALLY`，内层 pc-dispatch 写 `chaos_continuation = 315;` → `C2065`。

## 关键设计结论（不可退让）

**不能改成"pc-dispatch 自声明"**。读写关系已用代码证据锁定：

- 写者 = region 体内 pc-dispatch（`partitionExitOffset = recovered[0]`）
- 读者 = **兄弟节点** tail dispatch（`resumeOffsets: exitOffsets`）
- 全文件仅一处设 `ResumeOffsets`，且那条路径 `partitionExitOffset = -1` → **`hasExits` 与 `hasResume` 互斥**

即写者与读者**永远是不同节点、不同 C++ 作用域**；`CHAOS_EH_TRY_FINALLY`（`eh.h:88`）会开一个
brace scope，所以只有 region 层是公共作用域。自声明会让 tail 读到陈旧的 `-1`，**重现 `aba7851f4` 修掉的 bug**。

## 修复方案（已实现）

三个分支统一改用**子树预扫描**：

```csharp
bool needsContinuation = WritesHandoffSlot(er.TryBody) || WritesHandoffSlot(er.HandlerBody);
```

`WritesHandoffSlot` 递归遍历 StructuredIR，命中"有非负 exit target / fall-out 的 `IRPcDispatch`"即 true。
覆盖 `IRSequence` / `IRIfThenElse` / `IRWhileLoop` / `IRDoWhileLoop` / `IRSwitch` / `IRExceptionRegion` / `IRAwait`。
TryFinally 的声明落在 `CHAOS_EH_TRY_FINALLY` **宏之外**（与宏同缩进）。
同时删除 `EmitPcDispatch` 里那段**已不成立的空注释块**。

## 验证状态（诚实）

- ✅ `dotnet build Chaos.IL2CPP.Generator` — 0 error
- ✅ 曾成功产出正确 C++（page2.cpp: 2246 外层声明 / 2262 内层声明 / 2293+2326 写入）
- ❌ **回归测试 `FinallyPcDispatchHandoffSlotTests` 未跑绿** —— 但**不是 fix 的逻辑错误**

### 未跑绿的真因（2026-09-12 实测定位）

测试断言 `Assert.Contains("chaos_continuation =", source)` 失败，**连写点都不存在**。
dump 生成的 C++ 后确认：fixture 的 try 体确实产出了 pc-dispatch（不可约），
但其 `brfalse 0x07` 的目标 `0x07` **落在了本 CFG 内部**，于是
`BuildPcDispatchBody` 走 `targetBlockIdx >= 0` 分支 → `nextPcValue = 0`，`exitTargetOffset` 保持 `-1`。
fall-out 的 base 又是 `partitionExitOffset = -1`（finally builder 不传）。

→ **该 fixture 根本没有跨分区续接，无写点，`needsContinuation=false` 是正确行为。**
`WritesHandoffSlot` 判断正确；错的是 fixture 的形状假设（我原以为"不可约 ⇒ 必有 exit target"，不成立）。

**待办**：重写 fixture，让 try 体内出现**目标在分区外**的分支（`exitTargetOffset >= 0`），
才能真正驱动写点。

### 顺带发现的预存在缺陷（非本 fix 引入，需单独评估）

`StripExceptionPartitionExitTerminators`（`StructuredIR.cs:361`）对 `IRBlock` 只删终结符、
**保留块**：`IRBlock(block.BodyInstructions, null)`。而 `BuildFinallyOnlyExceptionIRBody` 调
`BuildExceptionPartitionTree(try, offsets)` **不传 `partitionExitOffset`**。

后果：try 体尾部的 `leave` 被剥掉后，其目标丢失 → 尾部块落到 dispatch 外 → `chaos_pc = -1` →
**直接穿过 `CHAOS_EH_FINALLY_END` 落到 tail**，既不执行 finally 也不记录交接偏移。
在 `leave` 语义下这是错的（`leave` 应当跳过 finally）。

本 fix **不改变**该行为（`needsContinuation` 只控制声明，不控制写点），但它使这条路径可见。
**需单独立项**：给 finally builder 传 `partitionExitOffset`，并把 try 体的 `leave` 还原为
写 `chaos_continuation` 的退出而非被剥离。

## 已知的下一层墙

C2065 消失后，native 链接阶段会撞上（**预存在，与本修复无关**）：

```
LNK2019: unresolved external symbol GcAllocateFast
```

`gc_alloc_stubs.h:35` 是 `CHAOS_IL2CPP_FORCEINLINE` 定义，`gc_helpers.h:23` 又声明了
**非 inline** 版本 → 链接器期待一个不存在的导出符号。

## 🔴 落盘注意事项

本修复**三次被并发 agent 抹除**（`git reset` / 在 `main` 上 commit）：

```
16:49:54  reset: moving to HEAD
17:03:30  commit f3ae90e24 → d6427ab6d
17:09:51  commit → 6e39a059e      (本次会话中 HEAD 又前进到 4fb7d3437)
```

落盘前**必须**：确认无并发 agent → `git apply fix.patch` → **立刻 commit**。
工作区对抗不住这种竞态。参考记忆：`parallel-agent-clean-checkout-clobber`。

---

# 🔴 新阻断：AOT entry.exe 启动即 SIGSEGV（2026-09-13）

> ## ⚠️ 本节结论已被证伪 — 勿据此重开 ABI 调查
>
> 下方「根因：按值返回的 64 位量被当作指针传入」是**拿 PDB 之前盲反汇编的产物，是错的**。
> 它把 `GateThreadLoop` 里 `sleep_for(milliseconds(15))` 的整数 `15` 误读成打包的时间量。
> 真实根因见文末「✅ 已解决」：陈旧 `crt_stubs.cpp` 的 `_Thrd_sleep_for` ABI 不匹配。
> 本节的 cdb 原始 dump 与「已证伪的假设」表仍有参考价值，**结论部分作废**。

## 现象（8/8 确定性，非竞态）

```
entry.exe --fact-json        → rc=139 (SIGSEGV), stdout 0 字节
entry.exe --benchmark 0 1    → rc=139, stdout 0 字节
entry.exe  (no args)         → rc=139, stdout 0 字节
entry-jit.exe --fact-json    → rc=0, 784 records  (对照, 3/3)
entry-jit.exe --benchmark 0 1→ rc=0, 123 字节    (对照)
```

**⚠️ 早先记录的"exit code 0 静默退出"是错的** —— 当时用 `| tail` 管道吞掉了真实退出码。
harness（`fact_chunk.py:47`）用 `capture_output=True` 会拿到 139，JSON 解析失败 →
`total==0` → `_tech_status` 判 `error` → 输出 "AOT is error (NOT promoting to partial)"。

日志停在 GC 初始化之后（GC-init 整块**跑两次**，slot 0 / slot 1），随后死。
**崩溃在 flag 分派之前**（`--benchmark` / 无参数 / `--fact-json` 全崩在同一处）。

## cdb 取证（决定性）

```
故障指令 chaos_entry+0x31079:  mov r8d, dword ptr [rcx+4]   ← AV
                  +0x3107d:  mov eax, 431BDE83h
                  +0x31088:  imul eax, r9d, 3E8h            ← /1000 整除
                  +0x3108f:  sar edx, 12h
函数入口 chaos_entry+0x31062:  sub rsp,20h; mov rbx,rdx;
                              test rcx,rcx; jne 0x31079     ← 有空指针检查，无有效性检查
```

**入口处 `rcx` 已是野指针**（非函数内自造）：
```
rcx = 00000a000000000f     (高32位 0x0a00, 低32位 0x0f=15)
rdx = 000000000000000f
```

调用者寄存器 trace：
```
0x5146e7 前: rcx = 0000006a6f4ffd30   ← 合法栈地址
             rdx = 00000a1b2ab1935e   ← 合法时间戳
0x5146e7   call 0x29c30               ← now()-形状
之后:        rax = 00000a000000000f   ← 返回值
             rdx = 000000000000000f
0x5146ef   call 0x31060               ← rcx 被设成 rax
入口:        rcx = 00000a000000000f   ← 崩
```

### ⇒ 根因：**按值返回的 64 位量被当作指针传入**

`0x29c30` 在 RAX 返回一个 64 位量，调用方直接把它放进 `rcx` 当指针，
被调方 `[rcx+4]` 解引用 → AV。这是 **ABI 值/指针错位**，同族于
`ir-lowering-root-cause-analysis`（ABI slots 缺 typeSubjectId）与
`async-r2b-box-abi-fixed`（ldloca 传 box 值非 &stack-slot）。

栈（稳定 5 帧复现）：
```
chaos_entry+0x31079  ← 崩
chaos_entry+0x5146f4
chaos_entry+0x514619
chaos_entry+0x59a29a
chaos_entry+0x53d160  ← 最外层
```

## 已证伪的假设（勿再提）

| 假设 | 证伪证据 |
|------|---------|
| 陈旧 lib 副本（同 `threading-lnk2019-stale-lib-copy`） | chunk 本地 lib(Sep13 12:01) 与权威 RelWithDebInfo(Sep13 12:16) **符号数完全相同 14460**，仅差 4KB 时间戳噪声 |
| `kChaosExternalRuntimeFnTable[101]` 越界写（模板 1311-1320 硬编码魔数） | 两表均声明为 `[219]`，`Count=219`，`[101]` 在界内 |
| 模板逻辑缺陷 | AOT/JIT 共用同一份 `runtime-entry.cpp`（HEAD 相同），仅 `CHAOS_IL2CPP_JIT_MODE` 宏不同；JIT 正常 ⇒ 缺陷在 AOT 链接的**运行时库**，不在模板 |
| 表长度与索引不匹配 | `kSubjectEntryCount=784` / `kAotMethodCount=1367` / `kChaosExternalRuntimeCount=219`，无一等于 15 或 2560 |

## 预存在 vs 本次引入 ⇒ **本次重建引入的回归**

`results/benchmark.json`（Sep 10）：
```
exitCode = 0 ; technology = chaos-aot ; methodCount = 801 ; results = 801 条 ; stderr = 空
```
Sep 9 时 AOT exe 跑完 801 个方法并 exit 0；Sep 13 时启动即 rc=139。
（`records.jsonl` Sep10 的 802 条是 **benchmark** 记录 `mode=native`，**不是** fact 验证数据 ——
不要把它当成 "real fact" 基线。）

## 未解（下一手必须回答）

1. `0x29c30` / `0x31060` / `0x5145a5` / `0x59a29a` / `0x53d160` 各对应什么源函数。
   exe **无 PDB** ⇒ 需靠地址反查，或给 entry 加 `RelWithDebInfo` 重编一次拿符号。
2. `0x00000a00`(2560) 与 `0x0f`(15) 的语义（15 疑似迭代/样本数，2560 疑似时间量的高半）。
3. 修复落点：是 codegen 发射的 ABI（调用方）错，还是运行时函数签名（被调方）错。

**建议下一手**：重编 entry 带符号（`CMAKE_BUILD_TYPE=RelWithDebInfo`）后复跑 cdb
`ln` 反查这 5 个地址 —— 这能把上面 3 个问题一次答完。

---

# ✅ 已解决：AOT SIGSEGV 根因与修复（2026-09-13）

**根因不是 codegen，是 SDK 生成器的 write-once 守卫导致陈旧 stub 永不更新。**

## 因果链

`SdkEmitter.cs` 生成 `crt_stubs.cpp` 时用 `if (!File.Exists(crtStubsDst))`（原 468 行），
**已存在的文件永不覆盖**。而 chunk 的 hephaestus cache 跨构建保留 `runtime_stubs/`。

⇒ `_Thrd_sleep_for` 的 ABI 修复（2026-09-10：`int __cdecl(const void*, void*)`
→ `void __stdcall(unsigned long)`）**永远到不了已构建过的 chunk**。

- `text/native/codegen/runtime_stubs/crt_stubs.cpp` 停在 **Sep 10 00:08**（旧签名，1303 B）
- `text/build_jit_output/.../crt_stubs.cpp` 是 **Sep 13 12:07**（新签名，1292 B）
- 预编译 `chaos_runtime_core.lib` 已按**新 ABI** 调用

运行期 `thread_pool.cpp:290` 的 `sleep_for(milliseconds(15))` 把**整数 15** 传进 RCX，
旧 stub 却把 RCX 当**指针**解引用 `[rcx+4]` → AV。

## 符号化证据（PDB 在 hephaestus-cache）

```
_Thrd_sleep_for+0x19 ← sleep_until<steady_clock,ns> ← sleep_for<int64,milli>
  ← GateThreadLoop+0x7a ← std::thread::_Invoke          rcx=0x00000a000000000f (低32位=15)
```
**崩在 gate thread，非 main**；崩溃在 flag 分派前，故 `--benchmark`/无参数同样 rc=139。

## 修复

`d5d0630da` — `pal_time_stub.cpp` 与 `crt_stubs.cpp` 两处改为**无条件 `File.WriteAllText`**。
（`tmpObjPath` 那处 `File.Exists` 语义不同，保留。）

## 验证（前后对照）

| 指标 | 修复前 | 修复后 |
|---|---|---|
| `entry.exe --fact-json` | rc=139, 0 records | **rc=0, 784 records** (3/3) |
| chunk `--stages build,fact` | build error | **2/2 passed** |
| `fact.json` realVerified | **0** | **212** |
| `fact.json` realTotal | **0** | **212** |
| `build.status` | error | **passed** |

**「real fact 缺口」就此闭合** —— 本目录最初的目标达成。

## 教训

同形于 `sdkemicer-copynativelibs-stale-skip`（skip-if-exists 让 stale lib 永不更新）。
**凡是"内容由生成器决定"的产物，都不该用存在性守卫。**
