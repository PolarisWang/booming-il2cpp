# 交接文档 — T7/S1/S2 反射+threading 工业化冲刺（2026-09-17 至 2026-09-22）

> **交接人**: Claude（多会话连续工作）
> **接手人**: 下一会话 Claude
> **权威输入**: 任务 #10 + 本文档 + `S1-SURVEY-2026-09-21.md` + `ROADMAP-FINAL-SPRINT-2026-09-21.md`

---

## ⚡ 2026-09-21 下午续报（会话 2）— 三层复合阻断解除，stubGap 118→8

> **先读本节再读下文**：下文的「stubGap 118 / realDefect 8」基线全部跑在 **09:49 陈旧 entry.exe** 上。

### 已修复（commit `d55628cf3`，已提交 main）

1. **重复 shape 注册全灭 codegen**：`IsDefined(Assembly,Type)` 被 Part1.S16（S2 冲刺）与 Part2.S6（并行会话）双注册 → `BuildDefault()` 每次抛 `Shape already registered` → **全部 codegen 零更新**，管线静默跑陈旧 exe。修复=删 Part1.S16 重复块，保留 Part2.S6 完整版（4 receiver），并修正其 Member/Module/Param 返回槽 NativeInt→Int32（原生返回 `CHAOS_IL2CPP_INT32`）。
2. **异常表发射语法错误**（cb129e362 从未编译成功过）：Dispatch.cs:852 每条 standalone `static const` 收尾写成 `},` 而非 `};` → C2226/C2143/C2447 级联（**kExcDesc0 不报、从 kExcDesc1 才开始**，极具迷惑性）。
3. **110 个假 stubGap**：`TestEmitter.HasKnownNativeImpl` 只查 `KnownNativeImpls`，不认 `WholeTypeRealViaDispatch`（CA 78 + TypeDelegator 32）→ 对实际有真实 AOT body 的方法发射 `return 42L`。修复=HasKnownNativeImpl 增加该集合查表。

### 最新实测（fresh exe，AOT=JIT 双侧一致）

```
stubGap 118→8（余量: ReflectionContext 5 / AssemblyNameProxy 2 / AssemblyName 1 — 全是 NotSupportedWholeTypes 候选）
UNVERIFIED smoke 119→9；failed 23（原样）；real 14；realDefect 8（原样）；unassertable 13
passed 241/272
```

### 🔴 realDefect 8 的真面目（T1 下一步）

不再是「静默返 0」— 直跑 `entry.exe --fact-json 2>&1` 实测 **SEH-FAULT 0xe0000001（CHAOS_IL2CPP_FAIL）**：
- `Assembly.GetForwardedTypes`/`GetManifestResourceNames`：native 已返回 `ChaosArrayNew1D` 空托管数组（module.cpp 同模式已翻绿，函数体本身不是崩溃点）；subject 现发射真实调用 + `Assert.AreEqual(new System.Type[]{}, result)` → **崩溃点疑似 Assert.AreEqual 的数组比较路径**（未实现 lowering），异常被 caught → value=0。
- 其余 6 项（GetCustomAttributesData/GetOptional|RequiredCustomModifiers/GetIndexParameters/GetSetMethod×2）同批triage。
- **已锁定判别式**（本轮末）：23 failed + 8 realDefect 的 subject 体全部以 `Assert.AreEqual(new T[]{ }, result)` 开头（T=引用类型），且 **AOT/JIT 双侧同红**；唯一通过的同构样本 `GetPublicKey_2__0` 用 `Array.Empty<byte>()`（T=byte 基元）。发射层已核对（page-0010.cpp:4729 起的 `GetForwardedTypes_7__0` body）：`new Type[0]` → `ChaosArrayNew1D(&chaos_type_info_managed_array.hot, chaos_mt_System_Private_CoreLib_System_Type.AsTypeInfoHot(), 1, 0)` → `chaos_stub_definition_...Assert__AreEqual_1...`。byte 与 Type 的发射形态完全一致 → **唯一变量是元素类型的 MethodTable**（`System.Type` 的 MT 可能是反射编码伪 MT / `.AsTypeInfoHot()` 返回坏指针 → 数组 header 残 → AreEqual 内 `isinst IEnumerable`/`GetEnumerator()` 接口分派 FAIL）。与记忆 [[typeof-fold-vs-object-model-mismatch]]（typeof 折叠 TypeInfoHot 不可被反射解码，架构级）同族。下一步：对比运行期 `chaos_mt_System_Private_CoreLib_System_Type` 与 `_Byte` 的 MT 值/AsTypeInfoHot 输出，定位坏指针来源。

### 工具链补充纪律（本轮新增踩坑）

11. **接手先 `stat entry.exe`** 对比最后相关 commit 时间 — commit 时间戳 ≠ 产物内容，fact 数字只会和 exe 构建时间一样新。
12. **Generator 源码改完先手跑 `dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator`**（连同 ATG）— `ensure_tool_built` 曾一次漏判（源 13:19 > DLL 12:03 仍跑旧码）。
13. **生成物语法错误排查路径**：MSBuild `-v:d` 抓 cl 命令 → `/P /C` 预处理 → 编 `.i` 复现 → 二分。首报错误行会晚于真实毒点（error recovery）。
14. **cwd 漂移**：Bash 前台 `cd` 会跨调用残留，相对路径 stat/ls 可能指向错误目录 — 排查时始终绝对路径。

---

## 一、已完成（全部推送 main）

### 反射 chunk（`reflection` chunk）

| 指标 | 基线 | 当前 |
|---|---|---|
| stubGap | 390 | **118** |
| real verified | 47 | **153** |
| not-supported 断言 | 0 | **187** |
| 诚实失败（可归因） | 0 | **31** |
| AOT=JIT | — | 一致 |

### threading-tasks chunk

| 指标 | 基线 | 当前 |
|---|---|---|
| passed | 340 | **343** |
| stubGap | — | **118** |

### 关键提交（全部已推送）

| 提交 | 内容 |
|---|---|
| `0c0f89cf3` | T7 Batch 1 — Compare 子串/TryParse 全族/Math.Round/MathF/page 孤儿根治 |
| `6466206db` | T7 Batch 2 — CompareTo 矩阵/IndexOf char 族 |
| `2d5d8987f` | T7 Batch 3 — 小整型 Parse 多参注册 |
| `d8b26f827` | T7 GetBytes ChaosArrayNew1D 修复 |
| `b38155d48` | S1-a TestEmitter 收窄 + IsDefined shape |
| `cabb3448b` | S2 Assembly/Binder not-supported Throws 断言 |
| `173ee085c` | S2 CA shapes 4 receiver 注册 |
| `0810fbcb6` | remaining_stubs.cpp 裸指针→ChaosArrayNew1D |
| `12fcb6a4d` | Module.GetFields/GetMethods 空数组守卫 |

---

## 二、当前反射 chunk 分布（最新管线实测）

```
326 subjects: stubGap 118 / smoke 150 / real 14 / realDefect 8 / failed 23 / unassertable 13
AOT=JIT 一致 295/295
```

**分类说明**：
- `real` = 有值断言且通过
- `smoke` = 真实调用但无值断言（部分是 not-supported Throws<NotSupported> 断言 — 诚实）
- `stubGap` = ATG 判"无 AOT body"→ 发 `return 42L`（需要修复）
- `realDefect` = 有 native 实现但返回值与 BCL 不符（需要修）
- `failed` = 断言失败（需要修）

---

## 三、剩余工作（按优先级）

### 🔴 第一优先：8 个 realDefect（预期一次全翻绿）

**根因已确认**：`remaining_stubs.cpp` 中 5 个函数返回裸 C 静态指针（`s_emptyArray`/`s_empty[1]={0}`），无 ManagedArrayAccessor header → 断言判型失败。

**修复已提交**（`0810fbcb6`）：全部改为 `ChaosArrayNew1D(managed_array, managed_array, value, 0)` 空托管数组。

**⚠️ 但管线仍报 realDefect 8** — 需排查：
1. 确认 preset SDK lib 包含新代码：
   ```bash
   python testing/foundation-dll/artifacts/build_presets.py --preset windows-x64-reference --force
   grep -c "ChaosArrayNew1D.*managed_array" src/native/runtime-core/reflection/remaining_stubs.cpp  # 应 = 12
   ```
2. 直跑 entry.exe 抓 stderr（stderr 不进管线日志）：
   ```bash
   timeout 60 <chunk>/native/entry.exe --fact-json 2>&1 | grep "GUO\|WAIT\|PARK"
   ```
3. 如果返回非零 → 修复生效但 assert 仍失败 → 是 assert 比较 MethodTable 的类型匹配问题
4. 如果返回 0 → SDK 传播链断裂 → 检查 preset 源目录副本是否同步

**受影响函数与行号**（修复后行号会偏移，按函数名搜）：
- `ChaosReflectionAssemblyGetForwardedTypes` — remaining_stubs.cpp
- `ChaosReflectionAssemblyGetManifestResourceNames` — remaining_stubs.cpp
- `ChaosReflectionGetOptionalCustomModifiers` — remaining_stubs.cpp
- `ChaosReflectionGetRequiredCustomModifiers` — remaining_stubs.cpp
- `ChaosReflectionPropertyGetIndexParameters` — remaining_stubs.cpp
- `ChaosReflectionPropertyGetSetMethod` — remaining_stubs.cpp（语义已正确，需另查）
- `ChaosReflectionModuleGetMethods` / `GetFields` — module.cpp（已修 ✓ 实测翻绿）

### 🔴 第二优先：118 stubGap 逐批消除

**覆盖审计结论**（`tmp_coverage_audit.py` 可重跑）：
全部 118 项**无 Classifier KnownNativeImpls 键**。分域：

| 域 | 数量 | 修法 |
|---|---|---|
| CustomAttributeExtensions | ~40 | **native 已有**（remaining_stubs.cpp 11 处）→ 补 Classifier 键 + shape 注册。⚠️ IsDefined 已由并行会话注册（Part2.S6）— 勿重复 |
| Assembly 动态加载 | 0 | ✅ 已转 not-supported Throws 断言 |
| TypeDelegator | ~32 | 部分 real（27/29 先例）→ 补真实实例 factory |
| Binder | 0 | ✅ 已转 not-supported |
| ReflectionContext 等 | ~46 | 逐个评估 |

**shape 注册范式**（照抄 Part1.S16.cs 已有的 IsDefined/CA shapes 条目）：
```csharp
registry.Register("System.Reflection.<Type>", "<Method>",
    [<paramTypes>],
    ShapeKind.SimpleForward, "<nativeSymbol>",
    <abiSlots>, <returnSlot>, new HashSet<int> { <indices> });
```

### 🟡 第三优先：31 诚实失败 triage

8 realDefect（见上）+ 23 failed（TypeDelegator 8/Module 6/PropertyInfo 5/ParameterInfo 4/Assembly 3/FieldInfo 2/MethodBase 2/MethodInfo 1）。逐项按三档判制定。

### 🟡 第四优先：smoke 150 → real

smoke = 真实调用但无值断言。转 real 需 probe 记录确定性返回值 → ATG 自动发 Assert.AreEqual。探针已在 .NET 8 侧运行，部分方法返回非确定性值（如 GetHashCode）→ 这些保持 smoke 是正确的。

### ⏳ 后续大任务

- **S3 typeof coerce**：反射查询入口双形态解析推广（T7 GetUninitializedObject 模式）— C 组 8 项
- **S4 RISK-7 接线**：dev-architecture-first-development 先行（三次盲试已禁）
- **T7 挂点**：threading-tasks 的 [WAIT] completed=0 → [PARK] 死等 — 需 cdb 抓栈或 per-subject fprintf
- **T8 Activator**：15 项真抛 + 7 项 shape 接线

---

## 四、工具链纪律（⚠️ 每一条都是本轮踩过的坑）

1. **SDK --force 重建**：native（src/native/**）改动后必须 `python testing/foundation-dll/artifacts/build_presets.py --preset windows-x64-reference --force` — 否则管线用旧 lib
2. **CHAOS_FOUNDATION_DLL 必带**：`D:\agent\chaos-il2cpp\testing\foundation-dll` — 否则解析到 tests/e2e
3. **TPG 捆绑 DLL 陈旧锁死**：并发管线锁住 Generator.dll 时 MSBuild 静默跳过刷新 → 诊断用 UTF-16LE 字节 grep（字符串字面量）或 ASCII（标识符名）；修复 = 轮询等锁释放后强拷
4. **TPG 模板三副本**：Templates/（源）、bin/Debug/.../Templates/、bin/Release/.../Templates/ — 改源后必须同步 bin 副本
5. **heredoc 反斜杠折叠**：bash heredoc 中 `\\` 变 `\` → 用 Write 工具创建 tmp_*.py 脚本文件代替
6. **UTF-16LE 字节验证**：.NET DLL 中字符串字面量是 UTF-16 — ASCII grep 恒 0 假阴性
7. **fact stderr 不进管线日志**：直跑 entry.exe --fact-json 2>&1 抓
8. **并行会话冲突**：开工前查 .claude/.classified + git status 脏文件 + 文件 mtime
9. **code 生成产物多文件**：任何单文件核对都可能漏 — 按 page 文件全集核对
10. **late type-id 定义在 namespace 顶部**：NativeAotEmitter.cs:403-410（勿移回尾部 — 消费点在模块注册 switch）

---

## 五、关键文件索引

| 文件 | 用途 |
|---|---|
| `docs/dev/in-progress/reflection-production-readiness/COMPLETENESS-ASSESSMENT-2026-09-21.md` | 工业化评估报告 |
| `docs/dev/in-progress/reflection-production-readiness/ROADMAP-FINAL-SPRINT-2026-09-21.md` | R-Final 冲刺计划 |
| `docs/dev/in-progress/reflection-production-readiness/S1-SURVEY-2026-09-21.md` | S1 普查报告 |
| `docs/dev/in-progress/reflection-production-readiness/FAILURE-TRIAGE-2026-09-14.md` | 反射失败分诊 |
| `docs/dev/in-progress/system-threading-chunk-triage/investigation-2026-09-17.md` | T7 8 族根因调查 |
| `docs/dev/in-progress/system-threading-chunk-triage/STATUS.md` | T7/S1/S2 进度与工程纪律 |
| `memory/system-chunk-verification-loop-discipline.md` | 验证循环纪律（8 条） |
| `memory/generated-page-files-stale-orphans.md` | page 孤儿问题 |
| `.claude/tasks/10` | 任务 #10（当前活动任务） |

---

## 六、数据根与 SDK 路径速查

```
SDK:              testing/foundation-dll/sdk/windows-x64-reference/lib/
Foundation root:  D:\agent\chaos-il2cpp\testing\foundation-dll  (CHAOS_FOUNDATION_DLL)
Pipeline:         cd testing/foundation-dll && CHAOS_FOUNDATION_DLL="D:\\agent\\chaos-il2cpp\\testing\\foundation-dll" PYTHONPATH=/d/agent/chaos-il2cpp/tests/e2e python ../../tests/e2e/verification/chunk_pipeline.py --assembly System.Private.CoreLib --chunk <slug> --stages build,fact
AOT 产物:         artifacts/foundation-dll/<Assembly>/chunks/<slug>/native/
JIT 产物:         artifacts/foundation-dll/<Assembly>/chunks/<slug>/build_jit_output/
反射 chunk fact:  artifacts/foundation-dll/System.Private.CoreLib/chunks/reflection/results/fact-results.json
系统 chunk fact:  artifacts/foundation-dll/System.Private.CoreLib/chunks/system/results/fact-results.json
```
