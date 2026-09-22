# worktree: json-writer-overload-shapes — 交接占位 / 环境警示

> ## ⛔ 已完结 — 不要再按本文件执行（2026-09-22 加注）
>
> **本文件写于 worktree 会话中途，其中「下一步」已被执行完毕。**
> 接续者请直接读同目录的 **[`B-class-31-plan.md`](B-class-31-plan.md)**（2026-09-21 16:13 写入，
> 晚于本文件 16:10），它才是当前有效的实施规划。
>
> 已完成情况：
> - §「目标」的 43 项口径**已过期**。43 = B 类 31 + C 类 12
>   （见 [`utf8jsonwriter-43-factorygap-survey.md`](utf8jsonwriter-43-factorygap-survey.md) §0）。
> - C 类 12 项已修：`941436a26`（异常类型按族区分）。
> - B 类 31 项已修：`B0` 形参名归一化 `f7635f854` → `d2a467ee4`
>   （Utf8JsonWriter 缺失重载补齐，message 记为 factoryGap 62 → 0）。
> - 故本文件 §「下一步」所述「普查 agent 正在产出…普查结果回来后据此实施」
>   **已不再成立** —— 那次普查早已完成并落地。
>
> **重新执行那份普查是纯粹的重复劳动。** 若你正要这么做，先读 `B-class-31-plan.md`。
>
> 本文件保留下来的价值仅在 §环境警示（并发 agent 写共享工作树）与 §根因分析的推演过程，
> 其中的数字与口径**一律以 `B-class-31-plan.md` 为准**。

> 创建：2026-09-21 15:2x
> 分支：`worktree-json-writer-overload-shapes`
> 基线：`055769a95`（= origin/main HEAD，已确认推送）
>
> ⚠️ 基线写法已修订：`055769a95` 是**当时**的 origin/main HEAD。origin/main 会随时序前进，
> 事后核验「是否等于 HEAD」必然得到不同结论。以 commit 号本身为准，勿再核验「= HEAD」。
> （`B-class-31-plan.md` 用的是另一种写法：直接给 `ce6db123e`，更稳妥。）

## 目标

~~消除 `System.Text.Json/Utf8JsonWriter` 剩余的 **43 项 factoryGap**。~~

> 🔴 **已作废，见上方弃用头。** 实际剩余为 B 类 31 项，且已完成。

## 已定位的机制（本轮，一手证据）

以 `WriteStartArray_8_JsonEncodedText_0` 为例，生成体调用点：

```cpp
chaos_external_runtime_System_Text_Json_Utf8JsonWriter__WriteStartArray_System_Void_JsonEncodedText_();
```

**零参数**，且该符号的定义体（生成体 5798 行）是 catch-all：

```cpp
extern "C" CHAOS_IL2CPP_INTPTR chaos_external_runtime_..._WriteStartArray_System_Void_JsonEncodedText_() noexcept
{
    ... CHAOS_IL2CPP_LOG_WARN("ExternalRuntimeFallback", "catch-all helper invoked: ...") ...
    return ChaosExternalRuntimeFallback("System.Text.Json/Utf8JsonWriter::WriteStartArray:System.Void(JsonEncodedText)");
}
```

**根因**：`RuntimeHelperShapeRegistry.CoreStubs.Part1.S21.cs:43` 只注册了 0 参版本：

```csharp
RegisterUtf8WriterVoid(registry, "WriteStartArray", "ChaosUtf8JsonWriterWriteStartArray",
                       thisOnly, rawThis, Array.Empty<string>());
```

而失败的 subject 是 `WriteStartArray(JsonEncodedText)` / `WriteStartArray(string)` 等**多参重载**。
shape 键 = (type, method, paramTypes)，**多参重载必须独立注册**，否则落到 catch-all。

`ChaosExternalRuntimeFallback` 返回 0 且**不抛异常** → 测试体的
`throw new Exception("AOT stub did not throw")` 触发 → `caught=true` → 分类 `factoryGap`。

> ⚠️ **与上一轮的多 catch 缺陷无关** —— 多 catch 修复（`322af0276`）已正确应用到这些方法
> （生成体确认为单 try + 按序匹配链），但这些方法**根本没走到 native**，
> 所以多 catch 修复对它们无效果。

## 🔴 环境警示：并行 agent 正在写共享工作树

本 worktree 的 `git status` 出现 **55 个与本任务无关的脏文件**：

> ⚠️ 这个数字**不可复现，勿引用**。同一路径 `tmp_v7/`、`tmp_verify/` 在 2026-09-22
> 实测为 **787 个文件**（目录仍在，内容已被换过）。此类并发生成目录的计数随时漂移，
> 判据应改为「这些目录在 git status 里的**存在**」而非数量。

```
tmp_v7/**, tmp_verify/**   （合计 790609 insertions / 790609 deletions）
```

- 这些文件**不在**本会话启动时的 git status 中
- 时间戳 **15:28**（本 worktree 空闲期间），随后不再变化
- 结论：**并行 agent 的产物**，已提交过（`aed6f0287`），现又产生大量 churn

**应对**：
1. **绝不** `git add -A` / `git add .` —— 只显式列出本任务文件
2. 提交前必 `git diff --cached --name-only` 逐行核对
3. 跑 pipeline 前先确认没有并发写入（否则 fact 会读陈旧 exe → 假数字）

## 下一步

> ⛔ **本节已完成，勿再执行。** 下方「普查 agent 正在产出」的普查**早已完成**
> 并落在同目录 `utf8jsonwriter-43-factorygap-survey.md`（A=0 / B=31 / C=12），
> 且 B、C 两类均已实施完毕。当前有效规划见 `B-class-31-plan.md`。
> 本节保留仅为记录当时的推演。

普查 agent ~~正在产出~~「43 项的三方对照表」：
- `A-仅缺注册`：native 符号已在，只差 shape 注册
- `B-缺 native 符号`：native 也没有该重载（多参需独立符号，否则 C2660）
- `C-其他`：特别注意 `JsonEncodedText` 是 **struct**，
  其 ABI 传递方式可能与引用类型不同 —— 这可能是一条**独立的坑**

> 补充（2026-09-22）：上面这条 `JsonEncodedText` 推测**已被证伪** ——
> 普查 §3 结论「不是阻断点」，且 S21 对所有注册一律把 JsonEncodedText 映射到
> `objAbi`/`NativeInt` 载体。B 类真正的缺口是**标量宽度**（见 `B-class-31-plan.md` 组 1）。

~~普查结果回来后据此实施。~~ → 已完成。
