# Phase 0 结果：断言生成机制已建立，但判别式未通过反向验证

> 日期：2026-09-13 ｜ 状态：**机制可行，判别式待重做**

## 已完成的（机制本身）

按方案 A 在 ATG 建立「判别性期望表」：`TestEmitter.DiscriminatingExpectations`
（declaringType + method → 谓词表达式），命中时绕过 probe 的哨兵、发射强断言。

**机制工作正常**，生成物确认：

```csharp
public long GetExecutingAssembly_3__0()
{
    var result_3_0 = global::System.Reflection.Assembly.GetExecutingAssembly();
    var __corelib_assembly = typeof(int).Assembly;
    var __assemblies_match = result_3_0 != null && result_3_0.GetName().Name != null
        && result_3_0.GetName().Name!.Length > 0
        && result_3_0.GetName().Name != __corelib_assembly.GetName().Name;
    Assert.IsTrue(__assemblies_match, "...");
    return 1L;
}
```

- 表为空时**行为与改动前逐字节一致**（无匹配走原路径）
- 不触及 manifest 主线、不触发快照测试要求（符合治理规则 2/5/6 的评估）

## 途中修正的两个自身缺陷（已解）

1. 谓词初版写成 `{ ... }` 语句块 → 非合法表达式 → CS0106。改为纯表达式。
2. 谓词用 `Type.GetType("System.Object")` 取 CoreLib 名 → 该调用在本 chunk
   **无 native body，走 ChaosExternalRuntimeFallback 返回 0** → 解引用触发
   NRE，掩盖真实结果。改用 `typeof(int).Assembly`（本 chunk 已有成功用例）。

## ❌ 未通过：Phase 1 反向验证

**关掉 REF-RISK-7 接线后，断言仍然通过** —— 判别式无效。

接线关闭已确认有效（生成物中 `ChaosExecutingImageScope` 计数 0），
故问题不在接线开关，而在**判别式本身**。

### 已排除的解释

- 不是"接线没关"：实测注入点 0
- 不是"断言没执行"：生成物中 `Assert.IsTrue` 在位

### 待查明（按三修规则停下）

接线关闭时应回退到 `kImageCoreLib`，其 `image_name_utf8` 为 `"System.Private.CoreLib"`；
`typeof(int).Assembly.GetName().Name` 也应是同一字符串，理论上应**相等**→断言应失败。
实际却通过，说明**至少一侧在 AOT 下未返回该字符串**。两种可能：

1. `Assembly.GetName().Name` 在 AOT 下的降级路径返回值与预期不同（需实测该值）
2. `typeof(int).Assembly` 在 AOT 下解析到的不是 CoreLib 镜像

**下一步**：在生成的测试体里临时输出这两个字符串（或其一是否为 null），
一次运行即可定性。**不再靠推理改谓词** —— 本项已用掉三次尝试。

## 结论

- **基础设施可用**：判别性期望表机制成立，可承载任意强断言
- **判别式需要一次实测**：拿到 AOT 下 `Assembly.GetName().Name` 与
  `typeof(int).Assembly` 的真实值后，谓词可一次写对
- 本项**不应在判别式未验证的情况下合入** —— 一个"永远为真"的断言比没有断言更糟
