# 反射（System.Reflection）工业化生产级完成度评估报告

> 评估日期：2026-09-21
> 评估人：Claude（T7 会话内，应用户要求深度 review）
> 数据源：`artifacts/foundation-dll/System.Private.CoreLib/chunks/reflection/results/fact-results.json`（09-17 批跑）、
> `docs/dev/in-progress/reflection-production-readiness/`（STATUS/roadmap/L2 设计）、
> `api-coverage-matrix.json`、memory 反射线全部条目、T2 调查 workflow（wf_33995fb8-869）。

---

## 一、总体结论（执行摘要）

**反射线目前处于「API 覆盖设计完成 ~80%，运行时真实验证 ~11%」的阶段。**
距离工业化生产级（真实断言全覆盖 + 性能达标 + 热更新回归通过）还有 **大约 3-5 个会话的集中工作量**，
其中最大的单项风险不是反射实现本身，而是**「验证真实性」的基础设施缺口**（390/440 的 stubGap）。

三个层面的数字必须分开看，混淆会导致严重的进度误判：

| 层面 | 数字 | 含义 |
|---|---|---|
| ① API 覆盖矩阵（设计态分类） | **real 447 / not-supported 111 / 总 558（80.1%）** | 逐 API 判定"有真实实现"或"结构不可行"，Phase 2/3 产出 |
| ② 运行时 fact 验证（真实执行） | **real 47 / stubGap 390 / unassertable 3 / 总 440** | chunk 实际跑到、断言到值的 subject |
| ③ chunk 管线通过率 | 440/440 "全绿" | 表面数字 — 全绿是因为 stubGap 走 smoke 路径 |

**① 与 ② 之间 400 项的鸿沟就是剩余工作的主体。**

---

## 二、已完成并验证的里程碑（工业化基础，可信）

以下成果有三重证据链（managed 断言 + native 实现存在 + pipeline 通过），**可以信赖**：

1. **矩阵基建**：ApiSurfaceScanner 机器枚举 558 个反射 API，三档判定制落地，
   unclassified/real-planned 双双归零 — 「不允许静默返错值」的验收框架已建成。
2. **验证真实性修复能力**：五层断言缺陷链全部修复（VERIFY 剥空/查表键/runner 耦合/
   弱桩恒胜/循环 lowering），assertFailed 字段首次有效 — 失败从此可归因。
3. **反射成员元数据管线**（B3）：收集→发射→消费三步落地，GetProperty/GetField/
   GetEvent/GetMethod 从 FAIL 桩变为真实查表。
4. **性能三大项**：REF-RISK-1（O(n³)→O(1) 索引）闭合、REF-RISK-2（24 处静态缓冲区）闭合、
   REF-RISK-7 基建就位（TLS 执行镜像 RAII 支架）。
5. **语义断言 71/71 通过**（.NET 8 参考实现侧）。
6. **AOT/JIT 一致性**：283 条判定逐条一致，0 差异（非 AOT 专有缺陷的证明方法论）。

---

## 三、距生产级的差距清单（按优先级）

### 🔴 P0 — 验证真实性鸿沟（阻塞一切验收）

**390 个 stubGap subject 的构成**：ATG 对反射成员类型统一用 `SubjectInstanceFactory.Create<T>()`
（GetUninitializedObject）作主体 → 元数据视图类对象背后没有数据 → 访问器返回 null/0 →
smoke 通过但什么都没验证。这正是**本会话在 threading-tasks chunk 攻克的同一问题**
（Create<T> 泛型 stub 恒转发 Action 体 + GetUninitializedObject 零化对象 + typeof 折叠），
其修复链（共享禁用 / 调用点自身 stub / 双形态解析 / stable_id 特判）**可直接复用于反射线**。

**工作量**：receiver 供给改造（复用 T7 成果）+ 逐族真实断言接入 ≈ **1.5-2 个会话**。

### 🔴 P1 — typeof 值模型 ↔ 反射句柄体系打通（架构级，C 组 8 项）

`typeof` 折叠返回 `TypeInfoHot*`，而 `ChaosReflectionGetMethod` 等反射查询期望编码描述符句柄
→ 解码失败 NRE。**本会话在 T7 的 GetUninitializedObject 双形态解析（raw TypeInfoHot\* /
托管 Type 对象 +16）已经打通了第一半** — 同一模式的 coerce 方案推广到反射查询入口即可。
与并行线的 L2 泛型物化设计（按需加载跨程序集泛型定义）交汇，需一次架构对齐会。

**工作量**：**0.5-1 个会话**（含与 L2 设计的边界对齐）。

### 🟡 P2 — REF-RISK-7 codegen 接线（🔴 遗留第 1 项）

native RAII 支架（ChaosExecutingImageScope）就位，但 codegen 未发射 push/pop，
`tls_executing_image` 恒 null。**三次修复未果**（按三修复规则已禁止盲试），
记入 REFRISK7-WIRING-STATE.md，需按纪律先走 dev-architecture-first-development。

**工作量**：**1 个会话**（架构查询 + 定位发射点 + 实现）。

### 🟡 P3 — FieldInfo/PropertyInfo.GetValue 字段偏移（功能性缺口）

`FieldGetValue` 忽略字段偏移、静态字段直接失败。架构限制：ReflectionQueryFieldDescriptor
无 offset 字段。可行路径 = 扩展 GcTypeLayout 的 offsetof 表语义到普通字段。

**工作量**：**0.5-1 个会话**（设计 + 实现 + 验证）。

### 🟡 P4 — 111 项 not-supported 的显式抛异常验证

residual stub 已统一改 RaiseManagedException(NotSupportedException)，但**该路径未在
chunk 内触发**，需 ReferenceProof 管线验证 + 抽样确认。

**工作量**：**0.5 个会话**（验证为主）。

### 🟢 P5 — 已知小缺口

- GetPublicKey/GetPublicKeyToken 2 项真实断言失败（首次被观测）
- PropertyInfo.GetSetMethod 对只读属性语义（应 null 而回退 getter）
- 标记指针经 INT64 返回槽漏出（4 项，0x80007ff68290f0f0）

**工作量**：**0.5 个会话** 合并处理。

### ⚪ 不覆盖（已拍板，不计入工作量）

Reflection.Emit/TypeBuilder/AssemblyBuilder（not-applicable）、Binder（abstract 不可构造，
正确归 not-supported）、Mono.Cecil、调试器扩展。

---

## 四、工作量汇总与路线建议

| 阶段 | 内容 | 会话数 | 依赖 |
|---|---|---|---|
| P0 验证真实性 | receiver 供给改造（复用 T7）+ 真实断言接入 | 1.5-2 | T7 keystone 成果（已就位） |
| P1 typeof coerce | 反射查询入口的双形态解析推广 | 0.5-1 | 与 L2 设计对齐 |
| P2 RISK-7 接线 | dev-arch-first + 发射点实现 | 1 | — |
| P3 字段偏移 | GcTypeLayout 扩展 | 0.5-1 | — |
| P4 not-supported 验证 | ReferenceProof 抽样 | 0.5 | — |
| P5 小缺口 | 3 项合并 | 0.5 | — |
| **合计** | | **4.5-6 个会话** | P0 先行 |

**建议顺序**：P0 → P1 → P2 → P3 → P4/P5（P4/P5 可穿插）。
P0 完成后"440 全绿"才第一次成为**有意义的**全绿；P1 完成后 reflection 与 threading 的
receiver/typeof 两条线正式合流；P2 完成后三性能大项全部闭合。

---

## 五、风险与陷阱备忘（评估过程中确认仍然有效）

1. **表面全绿陷阱**：440/440 全绿中 88.6% 是 stubGap — 任何以 chunk 通过率汇报的
   进度都不可信，必须看 fact resultKind 分布。
2. **进度记录失真前科**：旧 reflection-industrialization 任务 17 个子任务标 completed
   但 proof 目录不存在 — 本报告全部结论以三重证据链为准。
3. **fact realTotal 与 resultKind 不可混淆**：`value==0` 与"异常被吞"不可区分（memory），
   必须 combine caught/assertFailed 字段。
4. **多 agent 并发**：main index 常被占用；worktree 验证时注意 bin/Debug 模板三副本
   （本次 [SUBJ] 打印三副本陷阱的教训）。
5. **hephaestus 缓存**：曾误判修复无效 — 改 codegen 后必须清缓存或 --force。

---

## 六、一句话结论

> **反射系统的"骨架"（矩阵、判定制、元数据管线、性能三闭合）已工业化，
> 但"血肉"（运行时真实执行的验证）只有 ~11% — 剩余 4.5-6 个会话的工作量
> 中，一半以上是验证基础设施建设而非反射功能本身，且 T7 刚验证过的
> receiver 供给修复链可以直接复用，这是最大的成本红利。**
