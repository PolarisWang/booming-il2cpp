# A1-2 — 修 stloc 写入错误源

```yaml
task_id: A1-2
task_type: plan
phase: implementation
parent_task_id: codegen-loop-abi-defects
source_task_id: codegen-loop-abi-defects
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-executing-plans
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
lifecycle_status: completed
```

## Scope

修 A1-1 定位的缺陷：`stloc` 写入源为接收者句柄而非 `?:` 结果。

## Exit Criteria

- 6 项 `value` 不再是标记指针 —— ✅ **6 → 0**
- codegen 2252/2252 绿 —— ✅

---

## ✅ 已完成（commit `42915b6b4`，方案 A）

### 根因（A1-1 定位，本轮实测确认）

`cond ? 1L : 0L` 恢复成 `IRIfThenElse` + `PostMergeBody`。两臂各 push 一个值，
但合并前 `RestoreDepth(postCondDepth)` 把深度拉回分支前 → 臂内 push 的槽成孤儿
→ 合并点 `stloc` 从错位深度弹到更早的残留值（`_s1` = `GetModule()` 接收者句柄）。

### 实现（方案 A，用户拍板）

`IRIfThenElse` 增显式字段 `MergeCarriesValue`：

```csharp
bool MergeCarriesValue = false
```

**不从 `PostMergeBody` 形状推断**——已实测确认两种形态那里都是普通指令序列
（本例头部是 `ldloc`），不可靠。

判定依据：**合并块自身的 IL 栈效应**。首条指令为
`stloc/pop/starg/stfld/stelem/stind/throw/ret/br*` 等消费值的操作 → true；
`conv.*`/`neg` 等 1-to-1 转换不算（`MergeBlockConsumesValue`）。

5 个构造点全部透传；emitter 据此：

```csharp
int mergeDepth = ite.MergeCarriesValue ? postCondDepth + 1 : postCondDepth;
_state.Value!.ActiveStructuredSlotContext?.RestoreDepth(mergeDepth);
```

### 验证

| 指标 | 修前 | 修后 |
|---|---|---|
| negative-value passing 记录 | **6** | **0** |
| `passed` | 248 | 248（无回归） |
| codegen 测试 | 2252/2252 | 2252/2252 |

生成物核对（`si=159`）：

```cpp
// 修前
chaos_locals[1] = ChaosStoreInt64(_s1);   // 接收者句柄 → 标记指针
// 修后
chaos_locals[1] = ChaosStoreInt64(_i2);   // ?: 的结果
```

### 归因层裁决

**codegen**（IR→C++ 的 local↔slot 深度记账），**不是 ATG**。
依据：IR 正确（`ret` 取 `ldloc.1`），`stloc` handler 本身正确，
缺陷在合并点的深度恢复。

## 附带发现（**未修**，登记）

这 6 个 API 走 catch-all 兜底且为**零参数**（`this` 未传）：

```
chaos_external_runtime_..._Module__IsResource_System_Boolean__()
```

与已修的 `Type::get_Module` 同类（漏注册）。
**不阻塞本项**（即便兜底返回 0，正确 emit 也应返回 `0L`）。
建议并入 A2 的清扫范围。

## Terminal Notes

- 提交：`42915b6b4`
- 诊断代码（`CHAOS_DIAG_ALL/STLOC/STACK/MERGE`）已全部清除
- 提交前工作区有两个并发 agent 的文件（`MethodEmission.cs`、`S19.cs`），**未纳入**
