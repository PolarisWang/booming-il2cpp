# system / threading-tasks chunk 分诊（2026-09-17 批跑）

> 基线（reflection 283/283 全绿之后的横向测量，commit `d2bec8405` 起）
> 本文档是分诊结论 + 移交清单，尚未实施修复。

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
