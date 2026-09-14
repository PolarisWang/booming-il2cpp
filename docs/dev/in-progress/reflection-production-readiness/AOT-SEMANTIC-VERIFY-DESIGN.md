# 设计：让验证能观测 AOT 语义（REF-RISK-7 等修复的可证性）

> 日期：2026-09-13 ｜ 状态：待实施
> 背景：REF-RISK-7 接线已完成，但其语义变更**无法被现有验证观测到**

---

## 1. 问题

反射线的"验证"由两条路径组成，**都无法证明 AOT 语义正确**：

### 路径 A：reflection chunk 的 fact 断言（跑在 entry.exe）
断言由 ATG 从 **probe 的捕获结果**生成。对 `GetExecutingAssembly()`：

```csharp
var result_3_0 = global::System.Reflection.Assembly.GetExecutingAssembly();
return (object)(result_3_0) != null ? 1L : 0L;   // ← 只查非 null
```

**判据过程**：
- 该断言确实在 `entry.exe` 内执行（`fact_chunk.py:165` 的 `assertFailed` 由 native
  runner 盖章）——**AOT 侧能验证语义**
- 但**断言的强度受限于 probe 捕获到的信息**。此处 probe 未捕获（该类型下
  `probe-results.json` 为空），断言退化为"非 null"
- CoreLib 回退同样非 null → **无法判别 REF-RISK-7 是否生效**

### 路径 B：语义契约套件（corelib-reflection-contract）
是 `dotnet run` 的**独立 .NET 8 控制台**，跑在**参考实现**上，不消费 AOT 产物。
它验证"契约定义正确"，**不验证 Chaos 实现**。

### 已用反向测试坐实
关掉 codegen 接线后：
- 契约套件仍 **73/73 通过**（路径 B 确实不覆盖 AOT）
- fact 断言仍**通过**（路径 A 的断言太弱）

---

## 2. 结论：问题不在"没有 AOT 验证"，而在"断言强度"

`entry.exe` 内的断言机制是**可用的**。缺的是把"非 null"升级为"判别性断言"。

## 3. 方案

### 方案 A：增强 ATG 对反射访问器的断言（推荐）

对 `Assembly.GetCallingAssembly` / `GetExecutingAssembly` 这类**语义可判别**的 API，
ATG 不依赖 probe 的返回值，而是发射**结构性断言**：

```csharp
// 期望：报出的是本程序集，且不是 CoreLib 回退
var reported = Assembly.GetExecutingAssembly().GetName().Name;
Assert.IsTrue(reported != null && reported.Length > 0);
// 关键判别：与调用方自身的程序集名一致
Assert.IsTrue(reported == ExpectedExecutingAssemblyName);
```

其中 `ExpectedExecutingAssemblyName` 由 codegen 注入（本 chunk 的程序集名常量）。

- **优点**：直击问题；复用既有的 `entry.exe` 断言机制（无需新基础设施）
- **代价**：ATG 需为特定 API 维护"判别性断言模板"；需 codegen 提供期望值常量
- **风险**：中（改 ATG 断言生成，影响面限于被模板覆盖的方法）

### 方案 B：把契约套件跑在 AOT 上

让 `corelib-reflection-contract` 不再 `dotnet run`，而是被编译进 `entry.exe`。

- **优点**：契约的 73 条断言全部获得 AOT 覆盖，收益最大
- **代价**：需要把独立控制台改造成"chunk 内的 subject"，走 ATG/codegen 链路；
  控制台转 subject 涉及入口点、断言框架、结果回收三处改造
- **风险**：高（架构性改动，且 contract stage 的定位就是"独立参考实现校验"）

### 方案 C：新增 AOT 专用的判别性 subject

在 reflection chunk 的 subject 集里增加一小批**手写**测试类，专门断言语义
（如"GetExecutingAssembly 的程序集名 == 本 chunk 名"）。这些 subject 走正常
ATG→codegen→entry.exe 链路，因此天然在 AOT 中执行。

- **优点**：不改 ATG/契约架构；断言可任意强；落点在既有 subject 机制内
- **代价**：需维护手写 subject；覆盖面靠人工指定
- **风险**：低

---

## 4. 建议

**方案 C 优先，方案 A 作为其自然延伸。**

理由：
1. **C 的落点是既有机制**（subject 是项目里既有概念），不新造基础设施
2. **C 能立刻验证 REF-RISK-7**，把当前"改了无法证明"变成"改了有红灯"
3. 若 C 证明该模式有效，再把高频/关键 API 的判别性断言**模板化进 ATG**（即 A）

不建议先做 B：契约 stage 的定位（独立参考实现校验）本身是合理的，把它改成 AOT
会失去这一层，且改造面最大。

## 5. 最小验证步骤

1. 在 reflection chunk 的 subject 目录增加一个手写测试类，含 2-3 条判别性断言
   （GetExecutingAssembly 归属、GetCallingAssembly 归属）
2. 跑 `--stages build,fact`，确认这些 subject 出现在 fact 结果中且为 `real`（非 smoke）
3. **反向验证**：临时关掉 REF-RISK-7 接线，断言应转红 —— 这一步证明新断言**可失败**
4. 通过后，考虑把该模式推广（方案 A）

---

## 6. 实施调研结论（2026-09-13）：方案 C 与 A 都需要更深改造

### 已确证的事实

1. **`entry.exe` 内的断言机制是真实存在且可用的** —— `fact_chunk.py:165` 的
   `assertFailed` 由 native runner 盖章，说明测试方法体里的断言确实在 AOT 中执行。
2. **但 `GetExecutingAssembly` 这类方法的返回值是 `1L`/`0L` 哨兵，没有 `Assert.*`**：

   ```csharp
   var result_3_0 = global::System.Reflection.Assembly.GetExecutingAssembly();
   return (object)(result_3_0) != null ? 1L : 0L;   // ← 无 Assert
   ```

   该形态由 `Program.cs:913-915` 生成（"只查非 null 的哨兵"）。
   runner 拿它和 probe 的期望比对，而**该类型的 probe 捕获为空**
   （`System_Reflection_Assembly/probe-results.json` 无对应条目），因此无从判别。
3. **断言强度由 probe 决定**：probe 抓到什么，断言就能断言什么。probe 抓不到，
   就退化为哨兵。

### 两条方案的真实成本

| 方案 | 落点 | 需要的改造 |
|---|---|---|
| A（增强 ATG 断言） | `TestEmitter` / `Program.cs` | 需要一条**不依赖 probe 捕获**的断言通路：对特定 API 预置"判别性期望"。涉及 ATG 的模型/发射两处 |
| C（手写 AOT subject） | `chunk/managed/subjects/` | 需要把 `--custom-cs-files` 接到 reflection chunk 的 manifest 流程；需理解 metadata/coverage 合并 |

两者都**不是小改动**，且都触及 ATG 或测试治理主线（按 CLAUDE.md 应走
`project-test-governance`）。

### 建议

**暂停实现，先做方案选择与边界确认**。理由：
- 两个方案的落点都在测试治理主线上，按要求需先过 `project-test-governance`
- 本项的目标是"让验证能观测 AOT 语义"，属**基础设施**而非反射功能修复；
  它服务于整个仓库而不只是反射线
- 继续在反射 worktree 里猜落点，会重复之前 REF-RISK-7 注入点那类"三修不中"的弯路

**推荐路径**：把本项**立为独立任务**（域：测试治理），先确认：
1. 判别性断言是走"ATG 模板"还是"手写 subject"
2. 覆盖范围是仅反射访问器，还是泛化为"所有语义可判别但 probe 抓不到的 API"
