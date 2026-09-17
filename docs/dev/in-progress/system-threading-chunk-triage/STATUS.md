# system / threading-tasks chunk 分诊（2026-09-17 批跑）

> 基线（reflection 283/283 全绿之后的横向测量，commit `d2bec8405` 起）
> 本文档是分诊结论 + 移交清单，尚未实施修复。

## 执行进度（2026-09-17 下午更新，收尾快照）

> **当前状态**：Batch 1 已交付并验证（system 3008 → 3060 绿，+52）。
> 验证循环暂停原因：**并发会话**正在同一工作树实施 Convert.ToChar/Convert 家族修复
> （`Part1.S17.cs` 含 `[TMPDEBUG]` 调试输出，当前 WIP 使 TPG generate-dll 崩溃 rc=0xE0434352，
> 见 run8.log）。等待其完成后再跑第 9 轮验证 + NumberStyles.None 守卫的 +6 确认。

> **给后续执行者的管线纪律**（本轮血泪沉淀）：
> 1. native（src/native/**）改动后必须先 `python testing/foundation-dll/artifacts/build_presets.py --preset windows-x64-reference`
>    再跑 chunk 管线，否则 exe 链接旧 chaos_runtime_core.lib（run 5 教训）。
> 2. 跑管线必须带 `CHAOS_FOUNDATION_DLL=D:\agent\chaos-il2cpp\testing\foundation-dll`，
>    否则数据根解析到 tests/e2e（僵尸树遮蔽）。
> 3. AOT codegen 分页是内容寻址的：**旧 page 文件残留 = 重复定义 = 链接器静默选旧实现**。
>    build.py 7c 步骤已在 TPG 前清空 page 文件（本轮根治）。怀疑"修了没生效"时先查
>    `native/codegen/generated/` 有无重复定义（grep 函数体定义出现于 >1 个 page）。
> 4. 同一托管方法可能有多处发射点（inline 通道 S23 vs GenericShape 通道 S14），
>    只修其一会出现 AOT=FAIL/JIT=PASS 的 cross-tech diff。接线后核对两份生成物
>    （`native/codegen/generated/` AOT 与 `build_jit_output/codegen/generated/` JIT）。
> 5. 单次管线运行内 AOT exe 可能先于 codegen 再生成构建 → fact 跑旧代码；
>    拿不准就重跑一轮再判。
> 6. fact 阶段 `value_warnings`（passed 且 value<0≠-1）会判 stage 失败：
>    Compare 族 probe 的 `result ^ 0xFF` 编码（-1→-256）合法触发此校验器，
>    需 ATG/fact 线裁决（扩白名单或改 probe 编码），非 codegen 缺陷。

> **第 3 轮验证根因（重大）**：AOT 侧 fact 不翻绿的真凶不是注册缺失，而是
> **陈旧 page 孤儿重复定义**（memory: generated-page-files-stale-orphans 再次应验）—
> 09-16 的 13 个 `native-aot.page-*.cpp`（0009..0021）残留于 `native/codegen/generated/`，
> 与今日新分页（page-0022..0075）按内容迁移后产生重复 extern "C" 定义
> （如 Compare_7_3 同时定义于旧 page-0011 + 新 page-0052），链接器 COMDAT 先到先得
> 选中旧实现 → 注册/生成物/发射全部正确但运行时跑旧代码。
> 已删除 13 个孤儿 + build.py 新增 7c 步骤根治（TPG 前清空 page 文件，转换器每轮全量重发射）。

1. ✅ **失败清单重建**（post-f8a286e79 基线，今日 12:35 批跑）：system 270 红 / threading-tasks 97 红
   → `failure-worklist-2026-09-17.json`
2. ✅ **8 族根因调查**（workflow wf_33995fb8-869，8 agent 并行，全部有 file:line 证据）
   → `investigation-2026-09-17.md`。结论要点：
   - ⚠️ 方法论沉淀：**同一托管方法可能有多处发射点**（inline 通道 vs GenericShape 通道），
     调查报告只命中其一会导致"修了没生效"。判别信号：Cross-tech diff 中
     AOT=FAIL/JIT=PASS 且 JIT codegen 已含新发射 → AOT 走了另一处注册点。
     接线后必须核对 `native/codegen/generated/`（AOT）与 `build_jit_output/codegen/generated/`（JIT）两份产物。
   - **orphan-catchall（未注册 shape）** 占大头：C2 TryParse/Parse(65)、M1 CompareTo 矩阵(20+2)、
     String 方法族(21)、C1 BitConverter(10)+Guid(5)、C3 TryParse/Parse(33)、MathF(2)
   - **wired-wrong-semantic**：A 族 String.Compare 子串重载(S23:345 把 int 偏移当第二字符串)、
     String IndexOf char 族硬编码 return 0(S5)、Math.Round decimal 注册键错位(System.Decimal vs System.Math)、
     Convert.ChangeType(TypeHot* 句柄 vs TypeCode int32)
   - **atg-fixture / unassertable / unstable**（ATG 线，不接线）：B 族 HashCode(14, 随机盐)、
     Convert.ToXxx(DateTime)(10, 恒抛内联)、GC(4, 绝对内存常量)、Random(4, PRNG 硬编码期望)、
     Memory.TryCopyTo(4, span 载体缺失)
   - **preexisting-abi**：Half Float32 载体错配(3, f8a286e79 已知残留)
   - **needs-design**：E 族 Activator 15/22、Enum.Parse/HasFlag(2, typeof fold 同源)、
     Array.CreateInstance(1)
4. ✅ **Batch 1 修复完成（第 4 轮验证：system 3008 → 3060 绿，+52，AOT/JIT 首次一致）**：
   - A 族 Compare 10/10 全绿（双发射点修复 + 孤儿清理后生效）
   - C2 整型 TryParse 40/40 全绿（含多参变体：参数序修正 + NumberStyles.None 数字守卫）
   - Math.Round(decimal) 4/4、MathF 2/2、Half ABI 3 项自行回绿
   - NumberStyles.None 守卫已就位，+6（Double/Single/SByte _4/_5 变体）待验证
     （并发 WIP 阻塞验证循环，恢复后跑一轮 pipeline build,fact 即可确认）
   - 残留：Int128/UInt128 TryParse 10 项（无 128 位 carrier，遗留 Batch 2+）
   - ⚠️ 已知新问题：fact 阶段报 "9 method(s) returned negative values" —
     `fact_chunk.py:1218` 校验器把「passed 且 value<0(≠-1)」判为数据异常，
     而 Compare 族 probe 的 `result ^ 0xFF` 编码对 result=-1 产出 -256（合法）。
     修好 Compare 后首次触发，属 probe 编码约定 vs 校验器口径冲突（ATG/fact 系统层），
     待与 ATG 线一并裁决（改编码或扩校验器白名单）。
   - A 族 10 项：String.Compare 修复实际有**两处发射点**（调查报告漏了一处）：
     ① S23 `RegisterStringCmpInline`（inline 通道，JIT codegen 命中）已按参数矩阵分流；
     ② **Part2.S14 `RegisterStringCompare` GenericShape（AOT codegen 实际命中）** —
     其非 2-string 重载走 `(void)chaos_arg_i; return 0;` 兜底，已加子串分支
     `ChaosStringCompare(chaos_arg_0, chaos_arg_2)`。第二轮验证发现 AOT/JIT 各命中一处，
     只改 S23 时 AOT 仍红（跨 tech diff AOT=FAIL/JIT=PASS 为判别信号）。
   - C2 TryParse 65 项（由调查实施者并行落地，已审查并入）：S26 `RegisterNumericTryParse`
     + parse_convert.cpp `ChaosTryParse*` 家族(44 符号) + convert_stubs.h 声明。
     第二轮验证发现**多参变体参数序错配**：shape 按托管序转发 (str,styles,provider,out)，
     native 误实现为 (str,out,styles,provider) → out 槽收到 styles(0)，out==0 守卫 return 0
     （2 参基础变体因序恰好一致而通过）。已修正 forwarder 签名（out 置尾）。
     ⚠️ Int128/UInt128 TryParse(10 项) 未覆盖（无 128 位 carrier），遗留
4. ⏳ 后续批次（按 investigation-2026-09-17.md 分组）：
   - Batch 2：C3 TryParse/Parse(33) / CompareTo 矩阵 / String IndexOf+IndexOfAny / BitConverter GetBytes / Guid
   - Batch 3（ATG 线）：HashCode 白名单 / Convert.ToXxx(DateTime) fixture / GC+Random 期望改性质断言
   - Batch 4（设计级）：Activator / ChangeType TypeHot 句柄 / Half Float32 carrier / typeof fold 族

## 原始分诊（批跑当日）

## 批跑结果

| chunk | passed | 失败 | 构成 |
|---|---|---|---|
| reflection | **283/283** ✅ | 0 | —（B7 全部闭环） |
| system | 2994/3278（jit 2996） | 284 | af=True 234 / raise 50 |
| threading-tasks | 359/456（jit 356） | 97 | factoryGap 65 / failed 29 / nullArg 2 |

## system chunk（284 红）失败族

### 族 A：文化感知语义（~16，代表：String 33 中的 Compare_{9,10,11}_3/_4）
- 模式实测：String.Compare 每个重载只有 **CultureInfo/CompareOptions 变体
  set 失败，ordinal set 全绿**。
- 根因方向：culture-aware 比较未实现（catch-all/ordinal 退化），probe 期望
  来自 net10 文化行为。
- 修法方向：wire CultureInfo 感知 compare（InvariantCulture ≈ ordinal 起点），
  需先确认 ATG fixture 用的 culture。

### 族 B：HashCode.Combine（15）——**期望值本质不稳定**
- .NET 的 HashCode 随机化种子（每进程不同）→ probe 期望不可复现。
- 修法方向：**ATG 层判 unassertable**（非 runtime 缺陷）。挂 ATG 白名单线。

### 族 C：Convert/BitConverter GetBytes（~46，含 Int128/UInt128）
- BitConverter.GetBytes/ToXxx 基本类型族。方向：shape 注册 + 端序实现
  （小端）——与 reflection 的孤儿函数模式同构（grep 0 参 catch-all 铁证）。

### 族 D：数值类型 ToString/Parse（~100：Int16/Byte/UInt16/SByte/Single/
  Double/Decimal/Int32/Int64/UInt*）
- 方向：逐类抽样定位（ToString 格式 / Parse 语义 / Convert 链）。
- 预计与 C 族同构（shape 缺失或语义缺口），批处理性价比高。

### 族 E：Activator.CreateInstance（22）
- BindingFlags/args 变体——构造器解析 + 激活，依赖 descriptor 构造器表。

## threading-tasks chunk（97 红）

- **factoryGap 65**（Task/Task<T>/TCS/ValueTask）——ATG fixture 域
  （Task 家族 probe 构造失败 → factoryGap），非 runtime 缺陷优先。
- **failed 29**——与 async 线既有记忆衔接（`async-p3-tcs-partial-landed`、
  `async-segment-bc-native-builder-wiring`）：TCS .ctor/get_Task、
  Task.Delay handoff 等。
- nullArg 2——ATG wrap 已覆盖大部分；这 2 项可能 probe-null 组合不同，单查。

## 推荐执行顺序

1. **C+D 族批处理**（~146 项）：grep 生成物找 0 参 catch-all → 孤儿函数
   批量接线（复用 reflection 的 shape 注册模式 + `IsKnownStaticMethod`
   已就位）。性价比最高。
2. **B 族 ATG unassertable 判定**（15 项，一次性）。
3. **A 族文化感知**（~16 项）：先确认 fixture culture，再 wire 实现。
4. **threading-tasks failed 29**：按 async 线既有记忆逐项（跨域，单独排期）。
5. **E 族 Activator**（依赖构造器表，设计级）。

## 复用工具（本轮沉淀，见 memory b7-reflection-chunk-final-triage）
- grep 生成物 helper 签名：`0 参 catch-all` = 未接线铁证
- 直接 TPG generate-dll 看生成物（pipeline 可能不重生成 codegen）
- `build_presets.py --preset windows-x64-reference` 重建 SDK
