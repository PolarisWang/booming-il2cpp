# pps2-1-guard — 重写 L3 守卫 + 真实产物负控

> parent_task_id: `page-payload-split`
> source_task_id: `pps2-1-guard`
> source_relation: roadmap-child
> clearance_source: parent-roadmap
> entry_skill: dev-executing-plans
> 创建：2026-09-22

```
task_type: child
phase: executing
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## Scope

重写 `PagePayloadSplitTests.PayloadTables_AreNotStaticWhenReferencedAcrossSections`，
使其能在**真实产物**上正确检出跨 TU 的 `static` 引用；并建立可用的负控。

**背景**：该守卫当前**恒绿**（正则 `[^\n;=]*?` 懒惰量词匹配率实测为 0），
而产物中存在 96 个 `static` 跨 TU 引用（`kSlots_*` 定义在 page0、被 page-0007 引用）。

范围**不含**：修 `static`→`extern`（属 pps2-3）、发契约头（属 pps2-4）。

## Inputs

- `design-v1-01.md` §三（守卫假绿的机制与实测证据）
- `roadmap-v2-01.md` §四 Phase 1
- 真实产物：`artifacts/foundation-dll/System.Private.CoreLib/chunks/system/native/subjects/`
  （由 `python -m verification.chunk_pipeline --chunk system --stages build` 生成）

## Expected Outputs

- `tests/unit/managed/codegen/Emission/PagePayloadSplitTests.cs`（守卫重写）
- 负控记录（回退 → 红几条 → 恢复 → 绿），写入 commit message

## Exit Criteria

1. 守卫正则锚定**真实**定义形态，能匹配：
   ```
   static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_Foo[] =
   ```
2. **负控成立**：在**真实产物**下（未修 static/未发契约头）→ 守卫**必须红**
   - 必须**实测记录**红了几条
   - **禁止**用合成 fixture 做负控（本项目已两次栽此）
3. 判据自查：**「我见过这个测试红吗」** —— 必须是"见过"

## Terminal Notes

### 执行结果（2026-09-22）

**守卫已重写并通过负控。**

#### 关键修正：我先前的诊断是错的

我最初判断「守卫假绿是因为正则 `[^\n;=]*?` 匹配率为 0」。**这个判断是错的** ——
那是 `grep -E` 的行为（`[^\n=]` 中 `\n` 被当字面字母 `n`，而 `vtable_registry` 含 `n`）。
在 **.NET Regex**（测试实际使用的引擎）上实测：**旧正则和新正则都能正确匹配**（各 1 次）。

**真正的假绿原因只有一个：扫描范围。**

守卫的 payload 仅拼 `ObjectModelCodeBuilder + ModuleRegistrationCode`，
而 `kSlots_*` 的**引用方在 CodeRegistration 段** → 引用方不在被扫描文本中
→ `refs` 恒为 1（只有定义自身）→ `1 <= 1` → **恒绿**。

#### 第二处问题：合成 fixture 结构性不可用

`BuildModel()` 只造 `TestModule.TestClass::M{i}` —— 无类型、无 object model、
无 vtable slot 表。**被测符号类在该 fixture 中无法构造**，断言不可达。

→ 按用户拍板：**守卫直接扫真实产物**（`artifacts/.../system/native/subjects/`）。

#### 新守卫的两处自身缺陷（我引入并修掉的）

1. 引用检测只找 `sym[` / `&sym`，漏掉**裸符号引用**
   （真实形态是 `reinterpret_cast<const void*>(kSlots_Foo)`）→ 首跑假绿
2. 修正后 105 项检出

### 负控证据（真实产物，非合成）

| 项 | 值 |
|---|---|
| 守卫状态 | **FAIL** ✅（第一次真正见到它红） |
| 违反项 | **105** |
| 去重符号 | **98** |
| 确定性 | 连续两次跑均为 105 ✅ |

### 检出清单（比预估更全）

| 符号类 | 数量 | 定义 | 被引用 |
|---|---|---|---|
| `kSlots_*` | 96 | `native-aot.generated.cpp` | `payload.page-0007` |
| `chaos_ftn_thunk` | 8 | `generated.page13` | `page16-19` + `page-0068~0071` |
| `kChaosGcSlotMapsSize` | 1 | `payload.page-0006` | `payload.page-0007` |

> ⚠️ **新发现**：`chaos_ftn_thunk` 表明跨 TU static 引用**不限于 payload 段** ——
> **方法页之间**也存在。这一类别在此前的分析中完全缺失。

### Exit Criteria 核对

1. ✅ 守卫锚定真实定义形态（可匹配 `static const ::ns::T kFoo[] =`）
2. ✅ 负控成立：真实产物下 **FAIL，105 条**
3. ✅ 判据自查：「我见过这个测试红吗」 → **见过**

### 遗留（转交 pps2-2）

- 守卫耗时 **~72s**（9215 defs × 84 files 的 O(n·m) 扫描），后续可优化
- 完整符号清单已产出，供 pps2-2 登记器使用
