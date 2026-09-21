# worktree: json-writer-overload-shapes — 交接占位 / 环境警示

> 创建：2026-09-21 15:2x
> 分支：`worktree-json-writer-overload-shapes`
> 基线：`055769a95`（= origin/main HEAD，已确认推送）

## 目标

消除 `System.Text.Json/Utf8JsonWriter` 剩余的 **43 项 factoryGap**。

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

普查 agent 正在产出「43 项的三方对照表」：
- `A-仅缺注册`：native 符号已在，只差 shape 注册
- `B-缺 native 符号`：native 也没有该重载（多参需独立符号，否则 C2660）
- `C-其他`：特别注意 `JsonEncodedText` 是 **struct**，
  其 ABI 传递方式可能与引用类型不同 —— 这可能是一条**独立的坑**

普查结果回来后据此实施。
