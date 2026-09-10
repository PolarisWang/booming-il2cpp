# Foundation DLL 全量 il2cpp 开发 Roadmap v1

Date: 2026-05-02
Updated: 2026-05-02
Status: in-progress

## 1. 目标

完成 **14 个 foundation DLL** 的全量 il2cpp 翻译能力开发，覆盖 managed → native → hotupdate (host + patch) 完整链路，以 capability family 为单位推进。核心产出是一个能覆盖所有 14 个 DLL 全部 IL 模式的通用 CodeGen emitter。

最终完成标准：

- 每个 DLL 的每个 family 都经过完整 il2cpp 核心开发（架构分析 → Planner → CodeGen Emitter → Runtime → HotUpdate 双模式）
- 每个 family 的验证闭环全部通过（managed-proof → native-proof → hotupdate-proof → benchmark → dashboard）
- 每条 family 的 method 合约保持泛化形式（体现 IL2CPP 框架的通用性）
- Dashboard 显示所有 DLL 的真实进度数据
- 所有生成代码可 review、可执行、可归档

## 2. 范围边界

本 roadmap 负责：

- **CodeGen emitter 能力扩展**：补全所有 14 个 foundation DLL 所需的 IL opcode 翻译路径
- **每个 family 的完整 il2cpp 开发**：架构设计 → Planner → Emitter → Runtime → HotUpdate
- **每个 family 的完整验证闭环**：managed/native/hotupdate proof + benchmark + dashboard
- **Runtime skeleton → 真实 CodeGen C++ 转换**：13 个 DLL 从 RuntimeSkeleton 存根升级到真实 C++ 翻译
- **CoreLib 验证收口**：现有 24 families 的真实 C++ 代码完成全部验证步骤
- **Dashboard 持续刷新**：每完成一个 DLL 后 dashboard 数据同步

本 roadmap 不负责：

- 扩展 foundation-dll scope 之外的 DLL
- 为真实 API 语义创建定制测试（保持泛化形式）
- 一次性完成所有 14 个 DLL — 按序逐个推进

## 3. 非目标

- 不允许跳过 family 级架构设计直接改 emitter
- **不允许 emitter 覆盖率不达到 100% 就标记 Phase A 完成** — 每个 DLL 中出现的每个 IL opcode 必须被 emitter 处理，或被代码级证据证明不需要处理（如 Loader 层吸收的 prefix）。不允许将缺失 opcode 分类为"假阳性""已覆盖""已归一化"而不提供代码级证实。
- **不允许用"文件存在"替代"命令实际执行"** — 验证报告/benchmark 文件存在不等于已验证通过。每个验证步骤必须实际执行对应命令，并收集 trace 日志作为证据。
- **不允许将"部分通过"记录为"已验证"** — managed-proof / native-proof / hotupdate-proof / benchmark 四个维度中任一缺失或未实际执行，都不能标记该维度完成。
- 不允许 emitter 扩展不经验证闭环就标记完成
- 不允许 runtime crash 不做根因修复就跳过 family
- 不允许在未完成当前 DLL 的所有 family 前切换到下一个 DLL
- 不允许用"后续优化再处理"跳过性能退化记录

## 4. 全局约束

### 4.1 三优先级约束

- **P1（最高）性能最优**：生成的 C++ 代码运行性能为第一考量。翻译路径选择、emission 策略、runtime helper 设计都以此为标准
- **P2 方案完美性**：与现有五层架构（Loader → SemanticWorld → Linker → CodeGen → Emitter）一致，不破坏职责边界
- **P3 HotUpdate 适配**：在 P1+P2 满足前提下最大化 hotupdate 友好性

### 4.2 每个 family il2cpp 开发的四项原则

每个 family 的核心开发阶段必须给出多方案设计，从以下四个维度评估：

1. **性能最优** — 生成的 C++ 运行效率
2. **il2cpp 完美架构** — 与五层架构一致，职责边界清晰
3. **HotUpdate 高适配** — host/patch 双模式皆可正常生成和执行
4. **CodeGen 代码优雅** — emitter 代码易读、易理解、易维护

### 4.3 Phase A + Phase B 模式

每个 DLL 独立走完整循环：

```
for each DLL:
  Phase A: DLL 级批量 emitter 分析
    ① 反射 DLL 获取真实 IL body
    ② 全自动 IL 扫描 + 指令模式分析
    ③ 对每个缺失 opcode，逐一核实是否需要 emitter 处理
       - 给出**问题描述**：解释该 opcode 的作用语义、常见用法模式、emitter 需要完成什么翻译工作（生成什么结构的 C++ 代码），以及当前为什么没覆盖
       - 给出 **2~3 个解决方案**，每个方案从以下维度评估：
         - **性能最优** — 生成 C++ 的运行效率影响
         - **架构最好** — 与五层架构的一致性、职责边界清晰度
         - **HotUpdate 适配** — host/patch 双模式支持成本
         - **风险** — 实现复杂度、对现有 emitter 的冲击面、回归风险
         - **代码优雅** — emitter 代码易读性、可维护性、是否引入重复逻辑
       - 最终结论：
         - 需要补齐 → 选定方案，说明选择理由
         - 不需要 → 代码级证据（如 Loader 层吸收、IR 归一化路径追踪）
    ④ error-driven 试运行
    ⑤ 批量扩展 emitter
    产出: emitter 缺失清单 + 补齐
    ★ 完成标准: 该 DLL 涉及的所有 IL opcode 100% 被 emitter 覆盖或被代码证实排除

  Phase B: 逐个 family 验证
    Step 1 - il2cpp 核心开发:
      架构路径分析 → 多方案设计 → Planner 扩展
      → CodeGen Emitter 扩展 → Runtime 支持 → HotUpdate 双模式适配
    Step 2 - 验证闭环（每一步必须执行实际命令 + 保留 trace 日志）:
      [exec] AI 测试补全 → [exec] managed-proof → [exec] native-proof
      → [exec] hotupdate-proof → [exec] benchmark → [exec] dashboard 确认 → codegen-review
      验证证据要求:
      - managed-proof: dotnet test 实际运行 + 控制台输出日志 + 测试结果摘要
      - native-proof: native AOT 编译 + 测试实际运行 + 日志
      - hotupdate-proof: host 和 patch 双模式测试实际运行 + 日志
      - benchmark: benchmark 命令实际采集 + raw 数据 + 性能对比报告

    Family 完成标记
  DLL 完成 → dashboard 刷新
```

### 4.4 失败处理策略

| 类型 | 处理方式 |
|------|---------|
| A: emitter 缺失 | 暂停 Phase B，切回 Phase A 补齐，再回到当前 family |
| B: 测试代码问题 | 当前 family 立即修复 |
| C: 运行时崩溃 | 当前 family 立即修复 |
| D: 性能退化 > 阈值 | 记录问题，不阻塞，留待优化 |

### 4.5 Family 执行顺序

每个 DLL 内按 AI 分析 IL 复杂度推荐，从简单到复杂。

## 5. 阶段列表

### Phase 0: 基础设施对齐与 CoreLib 摸底

- goal:
  - 建立从 DLL → IL body → emitter 覆盖的自动化分析工具链
  - 对 CoreLib 的 24 families 做完整的 IL 扫描摸底
  - 确认当前 emitter 覆盖缺口

- exit_criteria:
  - IL 扫描工具可对任意 DLL 输出每个 method 的 IL opcode 序列
  - 扫描结果可对比 emitter 当前覆盖，自动生成缺失清单
  - CoreLib 24 families 的 IL 扫描完成，缺失清单输出
  - CoreLib 当前已生成的 C++ 代码验证状态摸底完成

- deliverables:
  - IL scanning + emitter coverage 分析工具
  - CoreLib emitter 覆盖缺口报告
  - CoreLib 现有验证状态摸底报告

- dependencies:
  - 无需前置依赖

- resolved_decisions:
  - IL 分析源使用项目已有 managed DLL 副本
  - 分析手段：全自动 IL 扫描 + 指令模式分析 + AI + wiki 查表 + error-driven 五种综合

- watch_items:
  - 扫描工具的性能（92 families × 平均 20 methods = 1840+ methods）

### Phase 1: System.Private.CoreLib 全量il2cpp开发 **（修正：实质性未完成）**

- goal:
  - ~~完成 CoreLib 24 families 的全部验证闭环~~ **[2026-05-03 修正]** 审计确认所有 42 families 为 scaffolding-only skeleton stubs，无真实 il2cpp 翻译代码。目标修正为：从真实 il2cpp 开发开始，完成 CoreLib 全量翻译。

- exit_criteria:
  - ~~Emitter 覆盖率 100%~~ — 重新评估中
  - 所有 families 的 native skeleton stub 经过 codegen review，确认哪些为 stub 哪些为真实代码
  - 验证体系已修复：能够区分 skeleton stub vs 真实翻译代码，不再将"文件存在 + 编译通过"视为验证通过
  - 按修正后的新计划推进

- deliverables:
  - 真实完成度评估报告（42 families 逐 family codegen review）
  - 验证体系修复
  - 修正后的开发计划

### Phase 2: System.Collections.Immutable

- goal:
  - 完成 System.Collections.Immutable 的 10 families 全量 il2cpp 开发 + 验证闭环
  - 从 RuntimeSkeleton 升级到真实 CodeGen C++

- exit_criteria:
  - Phase A 扫描完成，**该 DLL 所有 IL opcode 100% 覆盖或代码级证据排除**
  - 全部 10 families 的实际执行验证闭环（每个维度实际跑命令+留日志，见 6.2 标准）
  - Dashboard 正确显示

- deliverables:
  - 10 families 的真实 C++ 生成代码
  - 验证报告 + benchmark 数据 + review bundles

- dependencies:
  - Phase 1

- resolved_decisions:
  - method 合约保持泛化形式，不做真实类型签名

- watch_items:
  - ImmutableArray/ImmutableList 等复杂集合类型可能涉及泛型实例化 emitter 扩展

### Phase 3: System.Formats.Asn1 + System.IO.Compression.Brotli + System.IO.Compression.ZipFile

- goal:
  - 完成 3 个小型 DLL（共 12 families）的全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - 每个 DLL Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 12 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 3 个 DLL 的真实 C++ 生成代码 + 验证报告

- dependencies:
  - Phase 2

- watch_items:
  - Brotli/ASN1 可能涉及复杂算法 IL 模式

### Phase 4: System.IO.Pipelines + System.Net.ServerSentEvents

- goal:
  - 完成 2 个 DLL（共 9 families）的全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - 每个 DLL Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 9 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 真实 C++ 生成代码 + 验证报告 + trace 日志包

- dependencies:
  - Phase 3

### Phase 5: System.Linq

- goal:
  - 完成 System.Linq 的 12 families 全量 il2cpp 开发 + 验证闭环
  - LINQ 操作（filtering, projection, partitioning, ordering, grouping, aggregation, set, concatenation）覆盖

- exit_criteria:
  - Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 12 families 的实际执行验证闭环（每个维度实际跑命令+留日志，见 6.2 标准）
  - Dashboard 正确显示

- deliverables:
  - 12 families 的真实 C++ 生成代码 + 验证报告

- dependencies:
  - Phase 4

- watch_items:
  - LINQ 的迭代器/状态机模式可能对 emitter 有较高要求
  - 泛型方法大量使用

### Phase 6: System.ObjectModel

- goal:
  - 完成 System.ObjectModel 的 6 families 全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 6 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 6 families 的真实 C++ 生成代码 + 验证报告 + trace 日志包

- dependencies:
  - Phase 5

### Phase 7: System.Runtime.InteropServices [high-risk]

- goal:
  - 完成 System.Runtime.InteropServices 的 10 families 全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 10 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 10 families 的真实 C++ 生成代码 + 验证报告 + trace 日志包

- dependencies:
  - Phase 6

- watch_items:
  - P/Invoke、Marshal 等 interop 相关的 emitter 路径可能缺失
  - high-risk 标签

### Phase 8: System.Runtime.Serialization.Formatters [high-risk]

- goal:
  - 完成 System.Runtime.Serialization.Formatters 的 6 families 全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 6 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 6 families 的真实 C++ 生成代码 + 验证报告 + trace 日志包

- dependencies:
  - Phase 7

- watch_items:
  - 序列化反射相关 emitter
  - high-risk 标签

### Phase 9: System.Security.Principal.Windows

- goal:
  - 完成 System.Security.Principal.Windows 的 6 families 全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 6 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 6 families 的真实 C++ 生成代码 + 验证报告 + trace 日志包

- dependencies:
  - Phase 8

### Phase 10: System.Text.Json [high-risk]

- goal:
  - 完成 System.Text.Json 的 13 families 全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 13 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 13 families 的真实 C++ 生成代码 + 验证报告 + trace 日志包

- dependencies:
  - Phase 9

- watch_items:
  - Json 解析涉及大量字符串操作和反射
  - high-risk 标签
  - families 数量最多之一

### Phase 11: System.Threading.Tasks.Parallel

- goal:
  - 完成 System.Threading.Tasks.Parallel 的 4 families 全量 il2cpp 开发 + 验证闭环

- exit_criteria:
  - Phase A 完成 + 100% opcode 覆盖或代码级排除
  - 全部 4 families 的实际执行验证闭环（每个维度实际跑命令+留日志）
  - Dashboard 正确显示

- deliverables:
  - 4 families 的真实 C++ 生成代码 + 验证报告 + trace 日志包

- dependencies:
  - Phase 10

- watch_items:
  - 多线程/并行相关 runtime helper 可能缺失

## 6. 每阶段完成定义

### 6.1 DLL 阶段完成标准

一个 DLL 阶段只有在同时满足以下条件时才算完成：

- Phase A 扫描完成，**该 DLL 涉及的所有 IL opcode 已 100% 被 emitter 覆盖或被代码级证据证实排除**（不接受的解释：归类为"假阳性""已归一化"但没有代码级追踪证据）
- 所有 families 的 il2cpp 核心开发完成
- **每个 family 的四个验证维度全部实际执行并保留 trace 日志证据**：
  - managed-proof: `dotnet test` 实际运行 ✅ + 日志 ✅ + 结果摘要 ✅
  - native-proof: native AOT 编译 + 测试实际运行 ✅ + 日志 ✅
  - hotupdate-proof: host + patch 双模式测试实际运行 ✅ + 日志 ✅
  - benchmark: benchmark 命令实际采集 ✅ + raw 数据 ✅ + 性能对比报告 ✅
- Dashboard 已刷新并正确显示数据
- **aiCodegenReview 全部完成且符合 6.2.1 审查标准** — 每个 family 的 review 摘要文件已归档，审查结论为 "passed" 或附带修复计划
- **BenchmarkNativeEntry.cpp 真实性已验证** — 所有 family 的 benchmark entry 调用了真实 API（非 `return 42` 合成桩）
- **Fallback stub 比例已评估** — 每个 family 的 fallback stub 比例 ≤ 20% 或已记录修复计划
- CodeGen review 已归档
- 无未修复的 A 类/C 类失败

### 6.2 Family 完成标准

一个 family 只有在以下条件全部满足时才算完成：

- [ ] 架构路径分析完成并记录
- [ ] CodeGen 真实 C++ 生成（非 RuntimeSkeleton）
- [ ] **managed-proof fact 全部通过** — 实际执行 `dotnet test`，exit_code == 0，保留控制台日志 + 测试结果摘要
- [ ] **native-proof AOT fact 全部通过** — 实际执行 native AOT 编译 + 测试，exit_code == 0，保留编译日志 + 测试日志
- [ ] **hotupdate-proof (host + patch) fact 全部通过** — 实际执行 hotupdate host 和 patch 双模式测试，exit_code == 0，保留执行日志
- [ ] **benchmark 数据（managed + native）实际采集完成** — benchmark 命令实际运行，保留 raw 数据
- [ ] **aiCodegenReview 完成** — 按下方 6.2.1 审查标准执行语义级审查，review 摘要文件存在且结论为 "passed"
- [ ] **aiHotupdateReview 完成** — development-tracking.json 对应 family 的 aiHotupdateReview != "pending"
- [ ] 性能对比报告存在，包含 baseline vs native 对比数据
- [ ] Dashboard 正确显示 family 数据
- [ ] CodeGen review 已归档
- [ ] **trace 日志包完整** — 以上每个执行步骤的原始控制台输出/日志文件已存档到 `verification/foundation-dll/<dll>/<family>/trace-logs/`

#### 6.2.1 aiCodegenReview 审查标准

aiCodegenReview 必须对每个 family 的生成代码执行**语义级审查**（非仅结构检查），逐项验证以下内容：

**① 生成代码完整性检查：**
- [ ] C++ 编译通过，无语法错误（通过 native-proof 编译环节确认）
- [ ] 所有 managed 方法在 native 侧有对应实现入口，非缺失方法
- [ ] 方法签名、参数类型、返回值类型与 managed 合约匹配（泛化形式下正确翻译）

**② Fallback Stub 检查（关键质量门）：**
- [ ] 逐一识别生成的 runtime stub 中所有 `return static_cast<CHAOS_IL2CPP_INTPTR>(0);` 或等价 fallback stub
- [ ] **所有 fallback stub 必须记录在 review 摘要中**，注明方法名、所属 IL 模式、fallback 原因
- [ ] fallback stub 数量不得超过该 family 方法总数的 20%。超过时必须记录 blocker 并制定修复计划，不得标记 aiCodegenReview 为 "passed"
- [ ] 不允许以"无需处理""此方法不需要 emitter"为由将实际方法归类为非 fallback — 必须有代码级证据（如 Loader 层吸收路径追踪或 IR 归一化证据）

**③ BenchmarkNativeEntry.cpp 真实性检查：**
- [ ] 确认 BenchmarkNativeEntry.cpp 中每个 benchmark entry 调用了真实的 API 方法或等价 IL 翻译路径，**禁止仅 `return 42;` 或等价合成返回值**
- [ ] managed benchmark 与 native benchmark 必须覆盖相同的逻辑路径
- [ ] 若某 benchmark entry 确实仅测试 dispatch 开销（如空方法、no-op 场景），必须在 benchmark 报告中显式标注，不可默认标记为性能数据

**④ 语义正确性检查：**
- [ ] 类型转换、装箱/拆箱、泛型实例化等关键操作在生成代码中正确
- [ ] 异常路径（throw/catch/finally）正确生成为 C++ 异常或错误码返回
- [ ] 虚方法/接口方法调用正确转译为 C++ 虚函数调用或查表分发
- [ ] 递归和循环调用不产生代码生成死循环或栈溢出模式

**⑤ Review 记录要求：**
- [ ] 每个 family 生成独立的 review 摘要文件，保存到 `verification/foundation-dll/<dll>/<family>/reviews/ai-codegen-review.md`
- [ ] 摘要必须包含：审查总方法数、fallback stub 列表及数量、BenchmarkNativeEntry 真实性评估、发现的问题清单、审查结论
- [ ] 审查结论为 "passed" 的条件：无非记录 fallback stub（超过阈值已修复或已记录 blocker）、BenchmarkNativeEntry 调用了真实 API、无未修复的语义错误
- [ ] 审查结论为 "failed" 时必须附带修复计划（哪些方法需要 emitter 扩展、哪些需要 runtime helper 补齐）
- [ ] development-tracking.json 中的 aiCodegenReview 字段必须与 review 摘要结论一致

#### 6.2.2 aiHotupdateReview 审查标准

aiHotupdateReview 必须对每个 family 的 hotupdate patch 生成代码执行审查：

**① Patch 代码正确性：**
- [ ] patch 变体生成代码中 method entry 签名与 host 变体一致
- [ ] patch entry 正确调用 host 桥接或完全替换实现（视 hotupdate 策略而定）
- [ ] host 与 patch 之间无符号冲突（symbol renaming 正确）

**② 运行时兼容性：**
- [ ] patch 加载/卸载流程不产生内存泄漏或悬挂指针
- [ ] 静态状态（static fields, static constructors）在 patch 切换时正确同步
- [ ] 泛型特化在 host 和 patch 间一致

**③ Review 记录要求：**
- [ ] review 摘要保存到 `verification/foundation-dll/<dll>/<family>/reviews/ai-hotupdate-review.md`
- [ ] 摘要包含 patch 代码审查结论和发现的问题

## 7. 子任务映射

### 7.1 三层子任务结构

每个 DLL 内部的子任务按三层粒度拆分：

**Layer 1 — Phase 入口任务**：创建任务目录、持有全局状态、跨 Phase A/B 的协调上下文。每个 phase 一个入口任务。

**Layer 2 — Phase A: 缺失 opcode 子任务（动态创建）**：IL 扫描完成后，对每个缺失的 opcode（或强相关的 opcode 组），动态创建一个 opcode 子任务。每个子任务的输出是：

1. 评估报告（问题描述 + 2~3 方案 × 5 维度评估）
2. 提交用户决策
3. 决策落地：补齐 emitter / 代码级证据排除

**Layer 3 — Phase B: family 子任务（静态创建）**：按该 DLL 的 capability family ledger，每个 family 创建一个子任务，包含完整的 il2cpp 核心开发 + 验证闭环。

```
{dll-phase-entry}                         # Layer 1: Phase 入口
  ├── {dll-phase-entry}-opcode-{n}        # Layer 2: 每个缺失 opcode（动态创建）
  │   └── 评估报告 → 用户决策 → 实现/排除
  ├── {dll-phase-entry}-family-{family-1}  # Layer 3: family 核心开发 + 验证闭环
  ├── {dll-phase-entry}-family-{family-2}
  └── ...
```

### 7.2 执行顺序规则

1. Layer 2（opcode 子任务）全部完成后，才进入 Layer 3
2. Layer 3（family 子任务）按 AI 推荐的 IL 复杂度从简单到复杂串行执行
3. 验证中缺 emitter 时（A 类失败），暂停当前 family，动态插入新的 opcode 子任务，补齐后再恢复当前 family
4. 每个 opcode 子任务完成后，立即更新 emitter 覆盖状态

### 7.3 Phase 级子任务总览

| task_id | phase | status | purpose | Layer 2 | Layer 3 | depends_on |
| --- | --- | --- | --- | --- | --- | --- |
| `foundation-dll-phase-0-infra` | `phase-0` | `planned` | IL 扫描工具 + CoreLib 摸底 | — | — | — |
| `foundation-dll-phase-1-corelib` | `phase-1` | `planned` | System.Private.CoreLib 全量验证收口 | 由扫描结果定 | 24 families | `foundation-dll-phase-0-infra` |
| `foundation-dll-phase-2-collections` | `phase-2` | `planned` | System.Collections.Immutable | 由扫描结果定 | 10 families | `foundation-dll-phase-1-corelib` |
| `foundation-dll-phase-3-small-dlls` | `phase-3` | `planned` | Asn1 + Brotli + ZipFile (3 DLLs) | 由扫描结果定 | 12 families | `foundation-dll-phase-2-collections` |
| `foundation-dll-phase-4-pipelines` | `phase-4` | `planned` | IO.Pipelines + Net.ServerSentEvents | 由扫描结果定 | 9 families | `foundation-dll-phase-3-small-dlls` |
| `foundation-dll-phase-5-linq` | `phase-5` | `planned` | System.Linq | 由扫描结果定 | 12 families | `foundation-dll-phase-4-pipelines` |
| `foundation-dll-phase-6-objectmodel` | `phase-6` | `planned` | System.ObjectModel | 由扫描结果定 | 6 families | `foundation-dll-phase-5-linq` |
| `foundation-dll-phase-7-interop` | `phase-7` | `planned` | System.Runtime.InteropServices [high-risk] | 由扫描结果定 | 10 families | `foundation-dll-phase-6-objectmodel` |
| `foundation-dll-phase-8-serialization` | `phase-8` | `planned` | System.Runtime.Serialization.Formatters [high-risk] | 由扫描结果定 | 6 families | `foundation-dll-phase-7-interop` |
| `foundation-dll-phase-9-security` | `phase-9` | `planned` | System.Security.Principal.Windows | 由扫描结果定 | 6 families | `foundation-dll-phase-8-serialization` |
| `foundation-dll-phase-10-text-json` | `phase-10` | `planned` | System.Text.Json [high-risk] | 由扫描结果定 | 13 families | `foundation-dll-phase-9-security` |
| `foundation-dll-phase-11-parallel` | `phase-11` | `planned` | System.Threading.Tasks.Parallel | 由扫描结果定 | 4 families | `foundation-dll-phase-10-text-json` |

### 7.4 示例：Phase 2 (System.Collections.Immutable) 完整子任务展开

当 Phase 1 完成后，以下为 Phase 2 的完整子任务展开模板：

```
Phase 2 入口: foundation-dll-phase-2-collections

Phase A — 按实际 IL 扫描结果动态创建：
  ├── phase-2-collections-opcode-ldobj       # opcode 评估 + 实现
  ├── phase-2-collections-opcode-stobj       # opcode 评估 + 实现
  └── ... (按扫描结果定)

Phase B — family 子任务（按 ledger 静态创建，10 families）:
  ├── phase-2-collections-family-immutable-array
  ├── phase-2-collections-family-immutable-array-1
  ├── phase-2-collections-family-immutable-array-2
  ├── phase-2-collections-family-immutable-list
  ├── phase-2-collections-family-immutable-queue
  ├── phase-2-collections-family-immutable-stack
  ├── phase-2-collections-family-immutable-dictionary
  ├── phase-2-collections-family-immutable-sorted-dictionary
  ├── phase-2-collections-family-immutable-hash-set
  └── phase-2-collections-family-immutable-sorted-set
```

### 7.5 子任务创建时机

| 时机 | 创建内容 | 创建者 |
|------|---------|--------|
| Phase 入口启动时 | Layer 1: Phase 入口任务 | roadmap 首次执行 |
| IL 扫描完成后 | Layer 2: 缺失 opcode 子任务（每 opcode 一个） | Phase A scan 子任务 |
| Phase A 全部 opcode 完成后 | Layer 3: family 子任务（每 family 一个） | Phase 入口任务 |
| Phase B 执行中遇到 A 类失败 | 新 opcode 子任务（插入到当前 family 前） | 当前 family 子任务 |

## 8. 依赖

- `wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/` — 翻译路径参考表
- `wiki/06-测试验证/` — 测试验证规范
- `verification/catalog/programs/foundation-dll-translation-audit.program.json` — 14 DLL authority
- `verification/projections/foundation-dll-audit/capability-family-ledger.json` — Family ledger
- `src/managed/Chaos.IL2CPP.CodeGen/` — CodeGen emitter 源码
- `build/toolchains/run/testing/foundation_dll/` — 验证工具链

## 9. 风险

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| 复杂 IL 模式（迭代器、状态机）emitter 缺失严重 | 中 | 高 | Phase A 提前识别，留足扩展时间 |
| CoreLib 现有 C++ 代码含隐藏运行时 bug | 中 | 高 | 验证闭环先压 CoreLib，暴露问题 |
| benchmark 性能退化难以根因定位 | 中 | 中 | 退化记录不阻塞，留优化窗口 |
| 新 emitter 扩展破坏现有 CoreLib 生成 | 低 | 高 | 回归验证：改 emitter 后重跑 CoreLib 验证 |
| HotUpdate patch 模式下的运行时崩溃 | 中 | 中 | C 类失败立即修复，不跳过 |
| 泛化 method 合约无法覆盖真实 IL 模式 | 低 | 中 | IL 扫描基于真实 DLL body，合约只做分类 |
| **子任务/执行 agent 跳过实际测试，用"文件存在"替代"命令执行"** | **高** | **高** | ✅ 本版修正：所有 exit criteria 明确要求"实际执行+留日志"，trace 日志包为硬性交付物 |
| **执行 agent 对 emitter 缺失做无代码证据的归类和排除** | **高** | **高** | ✅ 本版修正：非目标明确禁止无代码级证据的"假阳性"归类

## 10. 备选路径

### 路径 A：全量并行推进

- 不适合
- 原因：用户已确认串行，一次一个 family

### 路径 B：跳过 CoreLib 验证收口，优先新 DLL

- 不适合
- 原因：CoreLib 是 foundation 核心，其 emitter 覆盖是新 DLL 的基础。验证收口必须最先完成

### 路径 C：混合 — 新 DLL 先走 RuntimeSkeleton 快速验证，再逐步升级

- 不适合
- 原因：用户要求每个 family 走完整 il2cpp 核心开发 + 验证闭环，不允许半成品

### 路径 D：按当前 roadmap 推进

- 推荐
- 原因：逐 DLL 推进、逐 family 验证，保证质量

## 11. 当前建议推进顺序

1. **Phase 0**：基础设施对齐 — 建立 IL 扫描 + emitter coverage 分析工具链
2. **Phase 1**：CoreLib 验证收口 — 24 families 完整验证闭环
3. **Phase 2-11**：按 orderIndex 逐 DLL 推进

## 12. 子任务执行策略

### 12.1 执行模式

- `child_execution_mode: auto`
- `auto_continue: true`
- `auto_stop_policy: blocking-only`
- `dispatch_model: sequential`

### 12.2 三层执行流

```
Phase 入口启动
  ↓
  ├── 执行 Layer 2: 缺失 opcode 子任务（全部完成后）
  │   └── 每个 opcode: 评估报告 → 用户审批 → 实现/排除
  ↓
  ├── 执行 Layer 3: family 子任务（逐个串行）
  │   └── 每个 family: 核心开发 → 验证闭环 → 完成标记
  │   └── A 类失败时暂停 → 新建 opcode 子任务 → 补齐后恢复
  ↓
Phase 完成 → dashboard 刷新
```

### 12.3 初始入口

- `recommended_next_child`: `foundation-dll-phase-0-infra`
