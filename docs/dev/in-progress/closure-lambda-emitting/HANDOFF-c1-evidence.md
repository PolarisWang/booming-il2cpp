# C1 证据交接 — 给 `verify-guard-removal` 分支的执行者

> 日期：2026-09-20
> 来源：独立 spike（`.claude/worktrees/spike-c1`，已清理）
> 目的：你们正在做的 `false && ...` spike，**结果我已跑出**，可直接消费

## 一、我做了什么（与你们当前未提交改动一致）

在隔离 worktree（base `bdf0630ba`，**不含**并发 GUO 挂死 WIP）把
`MethodEmission.cs:175` 改为：

```csharp
var _isDisplayClassCtor = false && method.SubjectId is not null
    && method.SubjectId.Contains("<>c::", StringComparison.Ordinal)
    && !IsLambdaBodySubjectId(method.SubjectId);
```

跑 `threading` chunk 的 **build**（nativeConfig=check）。

> ⚠️ 注意：`--no-incremental` 必须显式加，否则 dotnet 会认为"已最新"而
> 跳过重编译（我踩过：第一次跑出的产物仍是旧的 18 个 stub）。
> 且必须按 Generator → TPG 顺序重建，否则 TPG 携带旧的 Generator.dll。

## 二、结果（结论：**C1 成立，guard 可整条删除**）

| 判据 | 旧行为（A 落地的状态） | **Spike（guard=false）** |
|---|---|---|
| display-class stub 数 | 18 | **0** ✅ |
| 编译错误 | — | **0** ✅ |
| 畸形代码 `auto chaos_value = return` | — | **未出现** ✅ |
| link | — | **成功**（`1/1 passed`，724 subjects）✅ |
| `<>c::.cctor` body | fallback 假体 | **真实 body** ✅ |
| `<>c::.ctor` body | fallback 假体 | **真实 body** ✅ |
| lambda `b__14_0` body | 真实 body（A 已解） | 真实 body（不变）✅ |

### 2.1 `.cctor` 的真实 body（关键证据）

```cpp
extern "C" void ..._LazyInitializerTests___c_cctor(void)
{
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_args{};
    CHAOS_IL2CPP_ARRAY(CHAOS_IL2CPP_INTPTR, 1) chaos_locals{};
    ...
    try {
    {
        auto* chaos_object = CHAOS_IL2CPP_NEW_GC(chaos_type_...___c, {});
        chaos_object->header.type_info = chaos_mt_...___c.AsTypeInfoHot();
        ...___c__ctor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(chaos_object));
        ...
```

**完全正确的结构化发射**：`NEW_GC` 分配 → 设 `type_info` → 调 `.ctor`。
**没有任何畸形代码。**

### 2.2 产物体积反而更小

| | entry.exe |
|---|---|
| 旧（含 18 个 fallback 桩） | 14,404,096 |
| **Spike（0 个桩）** | **14,131,200** |

**−273 KB**。原因：`fallback 桩 + try/catch + 异常路径` 比平凡 body 更大。
→ 印证「修复是净收益」，而非付出体积代价。

## 三、对你们 fix 说明的一处修正

`da9fe2c57` 的 fix_strategy 写：

> `<>c::.ctor/.cctor` → 保持原 fallback（**其 newobj 结构化发射器确实无法降低**）

**这句话不成立** —— Spike 实测 `<>c::.cctor` 的 `newobj` **正常降低了**。
依据：

- `EmitLinearNewObject`（`ExceptionEmission.Linear.cs:529-535`）已有
  **无参 reference-type newobj** 分支（注释自述用例 `e.g. ArrayList::.ctor()`）
- `stsfld` 亦有完整 case（`EmitInstruction.cs:1209`）
- 那条 guard 的注释（`auto chaos_value = return`）**无对应代码产物** ——
  推测写于 `EmitLinearNewObject` 支持无参分支之前，**理由已过时**

## 四、建议

**把 A 扩大为 C1**：直接删除整条 `<>c::` guard（连带 `IsLambdaBodySubjectId`
helper 若不再需要）。

| | A（当前 main） | **C1（建议）** |
|---|---|---|
| 解封 | 200 lambda | **224**（+24 ctor） |
| 遗留 | 1 条名字启发式 | **无** |
| 未来 `g__`/`d__` 新形态 | ⚠️ 会再次误伤 | ✅ 自动正确 |
| 改动 | — | 再删 ~20 行 |

**若不做 C1**：至少请修正 fix 说明中的那句话（"无法降低"不成立），
否则会误导后续读者把已修好的能力当成限制。

## 五、我未做的部分（需你们补）

Spike 只跑了 **build**（worktree 缺 chunk 运行环境）：

- ❌ **fact 阶段未跑** —— 需确认 24 个 ctor 解封后的 fact 结果
- ❌ **未测 threading-tasks**（122 lambda + 8 ctor）—— 建议回归

**不预设全绿** —— 解封只是拿回 body，下游独立缺口仍在
（如 `QueueUserWorkItem` 3 参无 native）。

## 六、附：可复现步骤

```bash
git worktree add .claude/worktrees/spike-X <base> --detach
# 改 MethodEmission.cs:175 为 `false && ...`
dotnet build src/managed/Chaos.IL2CPP.Generator/... --no-incremental   # 必须
dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/...
# 补 _dll 元数据（worktree 缺 gitignore 的源输入）
cp -r testing/foundation-dll/System.Private.CoreLib/_dll <worktree>/testing/.../
CHAOS_FOUNDATION_DLL=<worktree>/testing/foundation-dll \
PYTHONPATH=<worktree>/tests/e2e \
python tests/e2e/verification/chunk_pipeline.py \
  --assembly System.Private.CoreLib --chunk threading --stages build
```
