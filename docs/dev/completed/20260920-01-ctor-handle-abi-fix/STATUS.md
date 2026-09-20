# STATUS — ctor 句柄 ABI 修复

```yaml
task_id: ctor-handle-abi-fix
phase: completed
lifecycle_status: completed
current_dir: docs/dev/completed/20260920-01-ctor-handle-abi-fix
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
关键文档:
  - design-v1-01.md
  - plan-v1-01.md
  - notes/verification-record.md
```

## 完成结论（2026-09-20）

**提交**：`5184a0e59` fix(codegen,native): ctor native 句柄被丢弃 — 句柄式 stub 家族修复

### 硬指标（XML chunk，746 subjects）

| 指标 | 基线 | 修复后 | 变化 |
|:-----|:----:|:------:|:----:|
| passed | 570 | **590** | **+20** |
| gatePassed | 385 | **405** | **+20** |
| failed | 100 | 96 | −4 |
| preAssertionRaise | 164 | 145 | −19 |
| assertionFailed | 12 | 11 | −1 |
| unassertable | 155 | 173 | +18 |
| stubGap | 185 | 185 | 0 |
| total | 746 | 746 | 0 |

**逐 subject 迁移：改善 20 / 回退 0**（`only-baseline=0`, `only-after=0`）

### 落地内容

- **C1** `InvocationTarget` + `ExternalRuntimeHelperDefinition` 新增 `CtorReturnsNativeHandle`
- **C2** `ShapeEntry`/`Register` 透传；`XmlTextWriter`/`XmlTextReader` 的 `.ctor` 置位
- **C3** `EmitLinearNewObject` 有参 reference 分支推工厂返回值（**REPLACE** 而非追加调用）
- **C4** `CreateDefinitionFromShapeEntry` 让 shim 返回 `CHAOS_IL2CPP_INTPTR`
- **附加**：修正 `xml_writer_stubs.cpp` 中三处被 .NET 8 实测证伪的异常语义
  （`WriteStartElement` 1参/3参、`WriteStartAttribute` 的 null 与 depth==0 检查）

### 验证证据

- **产物逐字节 diff（门槛）**：差异只在 `XmlTextReader`/`XmlTextWriter` 两个 `.ctor`
  的 wrapper 签名与调用点；未标记 ctor 差异为 **0**
- **跨 chunk**：threading 产物中 0 处标记生效点 —— 改动在其中不生效
- 详见 `notes/verification-record.md`

### 诚实标注

- 4 个目标 subject 转为 `unassertable`（非 `real`）—— 它们是 `void` 返回方法，
  ATG 结构性无法断言；修的是「错误地 caught」→「正确无断言」
- XML chunk 剩余 `failed 96` **未逐一对账**，不在本任务范围，不得外推
- threading 跨 chunk 回归被并行 agent 的挂死（`entry-jit.exe`）阻断，
  仅以产物级证据确认「改动在其中不生效」
- 修复过程中发现并行 agent WIP（`async.h`/`thread_pool.cpp`）混入工作树，
  已隔离后重建基线，未混入提交

## 原始问题陈述（保留）

修复「引用类型 `newobj` 的 native 工厂返回值被丢弃」这一**机制缺陷**，覆盖整个句柄式
stub 家族（XmlTextWriter / XmlTextReader / XmlWriter 等所有注册了 handle 语义的 ctor）。

**非目标**：不改 native stub 异常语义；不改 `CHAOS_EH_*` 宏体系；不引入裸指针全局状态表；
不对账 XML chunk 剩余 ~96 项 `realDefect`/`failed`。

## authority 决策

- **方案 B**：codegen 侧让工厂返回值接入 eval 栈（用户拍板，否决 A/C）
- **范围**：整个句柄家族（用户拍板）
- **验证口径**：全量 chunk 对比（用户拍板）

## 当前结论

### 根因（一手实证）

在 `ChaosXmlWriterWriteEndDocument` 入口插桩重建后，只被调用 1 次：

```
[WED-TRACE] entered handle=1468802672832 st=0000000000000000
```

`handle` 是**托管对象堆指针**，`Resolve()` 期望 **1-based slot** → 返回 nullptr →
静默 return → ATG 生成的 `CHAOS_EH_THROW` 触发 → `caught=true` / `realDefect`。

**native 语义正确，勿改。上游交接的「EH 逃逸是共享根因」假设已被推翻。**

### 关键约束

| 约束 | 证据 |
|:-----|:-----|
| `CreateVoidAbiSlot()` 不能回退 —— 会让 subject body 被替换为 stub（更糟） | `Linear.cs:481/527` |
| `AotCoreIrAbiSlotArtifact` 仅 3 字段，无法表达「ctor 返回 handle」 | `TypedIlAndAotCoreIrContracts.cs:255` |
| GC **有** `StwCompact()` → 裸对象指针会移动 → 否决方案 A | `gc_bgc.h:198` |
| `newobj` 有四条 reference 分支，C3 只落**有参 ctor 分支** | `Linear.cs:462/479/509/543` |

### 设计要点

- C1：`InvocationTarget` + `ExternalRuntimeHelperDefinition` 新增 `CtorReturnsNativeHandle`
- C2：`ShapeEntry` + `Register()` 加可选字段；句柄式 ctor 注册点置位
- C3：`EmitLinearNewObject` 有参分支置位时推**工厂返回值**而非 `chaos_object`
- C4：`CreateDefinitionFromShapeEntry` 置位时 wrapper 返回 `CHAOS_IL2CPP_INTPTR`

## 风险评估摘要

| 风险 | 等级 | 缓解 |
|:-----|:----:|:-----|
| R1 回归面广（`newobj` 是公共路径） | 🟡 中 | 标记默认 false；**产物逐字节 diff** 证明未标记路径不变（§5.4，门槛） |
| R2 C3/C4 不同步 | 🟡 中 | 同提交内改；pipeline 编译门禁 |
| R3 标记漏注册 | 🟢 低 | 全量扫描发现，非静默；**不猜，用扫描定位** |
| R4 陈旧产物误判 | 🟡 中 | 重建 TPG + 清 3 层 cache + 校验 exe 时间戳晚于 codegen |
| R5 并发 WIP 污染 | 🟡 中 | 提交前 `git diff --cached --name-only` 逐文件核对 |

## 三优先级权衡结论

**方案 B 在三个维度上均优于或持平于 A/C，无优先级冲突，无需让位裁决。**

- P1 性能：✅ `newobj` 路径无额外运行时开销（对比 A：每实例方法都要查哈希表）
- P2 架构：✅ 修机制本身而非绕过；保留 native slot 边界检查
- P3 热更：✅ 中性偏好，Hotpatch 路径不受影响（`hotupdateImpact: Smoke`）

## 外部输入核对（2026-09-20）

`HANDOFF-c1-evidence.md`（`closure-lambda-emitting` 线，非本轮任务）经核对
**不改变设计方向**，但促成两处修正：

1. **影响面精确化**：`EmitLinearNewObject` 有四条 reference-type 分支，
   C3 只落**有参 ctor 分支**（`Linear.cs:543-580`），其余三条物理上不进入（§5.3.1）
2. **验证门槛升级**：原「未标记路径逐字节不变」措辞过强 → 改为
   **产物的逐字节 diff 验证**，并从 watch_item 提升为验证门槛 §5.4-3

核对已确认 §3 的 `CreateVoidAbiSlot()` 约束引自实际代码（`Linear.cs:481/527`）而非注释，**复核后仍成立**。

## 未验证 / 诚实标注

- 根因**只对 4 个 WriteEnd\* subject 取得直接证据**；~28 处 ctor 注册为 **mechanism 推断**，需执行期验证
- XML chunk **基线已漂移**（并行工作落地）：stubGap 185 / realTotal 235 / realVerified 135 / total 746 / failed 100 / assertionFailed 12
- 上游交接文档的 84 / 338 / 229 等数字**已过期，勿引用**
- `stream_stubs` 的裸指针键隐患（W3）本轮**不修**，仅记录

## 下一步入口

`dev-executing-plans` / `dev-subagent-driven-development` —— 按 `plan-v1-01.md`
步骤 0 → 1 → 2 → 3 → 4 → 5 → 6 执行。
