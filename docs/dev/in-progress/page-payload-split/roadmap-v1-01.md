# roadmap — page 0 大载荷段拆分为独立 TU（根治 C1002 堆耗尽）

> task_id: `page-payload-split`
> 创建：2026-09-21
> 上游设计：`design-a-payload-split.md`（brainstorm 已拍板）
> 上游根因：`c1002-heap-exhaustion-rootcause.md`
> 调研输入：`a-direction-input.md`

## 一、目标

根治 system chunk 的 `C1002: compiler is out of heap space in pass 2`：
把 page 0 上**不分页的大载荷段**拆成独立 TU，段内再按字节预算分页，
使每个 TU 都回到 `sizeThresholdChars` 的设计量级。

**现状**：page 0 = 71.2 MB / 1,236,192 行，超设计阈值（350 KB）约 **200 倍**。

## 二、为什么需要 roadmap（而非单一 plan）

1. **跨 2 个 CodeGen partial + 构建层**：`NativeAotLoweringPlanner.Methods.cs`
   （段产出）、`NativeAotEmitter.Shared.cs`（分页 + 共享头）、TPG（产物收纳）。
2. **存在必须先做的「链接性改造」阶段**：234 个顶层 `static` 表不改 `extern`
   就没法跨 TU，这是独立且可单独验证的一步。
3. **有天然串行依赖**：链接性 → 段拆分 → 构建验证，后一步依赖前一步产物。
4. **会跨多次会话**：单轮跑 system chunk 构建 ~10 分钟，验证循环较长。

→ `dispatch_model: sequential`（子任务强依赖、共享同一批文件，无法并行）。

## 三、范围边界

**做**：
- page 0 大载荷段的**段边界识别与拆分**
- 被拆段内定义的 `static` 表改 `extern` + 共享头补声明
- 段内按字节预算分页
- 单元测试 + 守卫测试 + 负控

**非目标**：
- 不改 runtime（`src/native/`）任何代码
- 不改符号命名（不改 ABI）
- 不改 `sizeThresholdChars` 语义本身
- 不动 reflection / threading chunk 已有行为
- 不做无关重构

## 四、阶段列表

### Phase 1：链接性改造（前置，可独立验证）

- **goal**：让被拆段内的表符号可跨 TU 引用
- **exit_criteria**：
  - 234 个 `static` 表中，被拆段内的全部改 `extern`
  - 共享头补齐对应 `extern` 声明
  - **产物层面验证**：`nm`/`dumpbin` 确认符号由 internal → external
  - codegen 单测全绿（无行为变更）
  - 此阶段**不改变产物结构**，仅改链接性（可单独回滚）
- **deliverables**：`NativeAotEmitter.Shared.cs`（`BuildSharedHeader` + 表发射点）
- **dependencies**：无
- **resolved_decisions**：
  - 用 `extern`（C++ 链接性），**非** `extern "C"` —— 实测三情形指令序列字节级相同，
    P1 打平，故按 P2 取与既有 `chaos_mt_*` 一致者
- **watch_items**：
  - `kSubjectSlotMap` 有 1 处**运行期索引**访问，须与该表同 TU
  - 漏改的 `static` 会在 Phase 2 编译期暴露

### Phase 2：段拆分与段内分页

- **goal**：page 0 的大载荷段各自成为独立 TU，段内按字节预算分页
- **exit_criteria**：
  - 段分类器按 `// ── <name> ──` 标记正确切段
  - 每段独立 `NativeAotGeneratedSource`，命名落在 `native-aot.*.page-*.cpp`
  - page 0 仅保留 includes + 注册入口
  - **无任何 TU 超过预算**（量化断言，见 Phase 3）
- **deliverables**：`NativeAotLoweringPlanner.Methods.cs`、`NativeAotEmitter.Shared.cs`
- **dependencies**：Phase 1
- **resolved_decisions**：
  - **每段一 TU**，段内再分页（用户已拍板）
  - 命名用 `native-aot.*.page-*.cpp` —— 命中 CMake fallback 模式（`:87-89`），
    同时被 flat glob（`:85`）自动收录
- **watch_items**：
  - 段间隐式顺序依赖（实测表均为数据，无运行期初始化顺序依赖，但需 fact 回归确认）
  - 生成物 TU 数上升 → 编译时间变化

### Phase 3：验证与收口

- **goal**：证明根治且无回归
- **exit_criteria**：
  - **system chunk 构建通过**（当前 C1002 阻断）
  - reflection / threading **不劣化**
  - 守卫测试**双向负控**成立
  - snapshot 基线变更经人工核对
  - 单元测试全绿
- **deliverables**：测试文件、STATUS 更新
- **dependencies**：Phase 2
- **resolved_decisions**：验收口径见 §六
- **watch_items**：TPG 内嵌陈旧 `Generator.dll` → 改后必重建 TPG

## 五、子任务映射

> **2026-09-21 实测修正**：`pps-2-extern` 的范围由「234 个 static 表」修正为
> **6 个符号**（唯一 static 表 7,716 个中被跨段引用的只有 6 个），但**增加了
> `sizeof` 阻断的改造**（见 §五-补）。详见 `c1002-heap-exhaustion-rootcause.md`。

### §五-补：`pps-2` 的真实工作量（实测）

**跨段引用的 static 表只有 6 个**（两个独立方法互证：产物段扫描 + 守卫测试）：

```
kGenericTypeArgTokens      kGenericTypeEntries
kGenericMethodArgTokens    kGenericMethodEntries
s_method_aot_entries       s_method_aot_entry_args
```
定义在 `Registration calls` 段，被 `VTable descriptors` 段引用。

**但存在一个必须先解决的阻断**：这 6 个的引用形态是
`sizeof(x)/sizeof(x[0])`，而 `sizeof` 要求**完整类型**：

| 写法 | 结果 |
|---|---|
| `extern const T k[];` + `sizeof(k)` | ❌ **C2070: illegal sizeof operand** |
| `extern const T k[2];` + `sizeof(k)` | ✅ 通过（但动态长度无法预知维度） |
| `extern const T k[]; extern const T kCount;` + 用 `kCount` | ✅ **编译干净** |

→ 必须**额外发射计数标量**，消费方从 `sizeof` 改引用标量。
涉及 `NativeAot.CodeRegistration.cpp.scriban` **8 处** `sizeof(x)/sizeof(x[0])`
+ `NativeAot.GenericRegistration.cpp.scriban` 补发计数标量。

**该改造必须先于拆分落地，且在不拆分时也必须可用**（加「计数标量 == sizeof 结果」守卫）。

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| `pps-1-tests` | 1 | **completed** | main | 先写失败测试（TDD）：段分类/预算/链接性守卫 | — | b1 | managed 单测 + 负控 | `tests/unit/managed/codegen/Emission/PagePayloadSplitTests.cs` | ✅ 负控 2/3 红（`0868a37f3`） | `tests/unit/managed/codegen/**` | S |
| `pps-2-extern` | 1 | ready | main | 6 个跨段 static 表改 `extern const` + **计数标量改造** + 头补声明 | pps-1 | b1 | 产物符号链接性 + 计数标量等价性 | `NativeAotEmitter.Shared.cs`、`Templates/NativeAot.CodeRegistration.cpp.scriban`、`Templates/NativeAot.GenericRegistration.cpp.scriban` | dumpbin 确认符号 external；L3 测试转绿；计数标量守卫绿 | 同上 + `Templates/**` | M |
| `pps-3-split` | 2 | planned | main | 段分类器 + 段内分页 | pps-2 | b2 | 段边界正确 + 每 TU 不超预算 | `NativeAotLoweringPlanner.Methods.cs`、`NativeAotEmitter.Shared.cs` | 无 TU 超预算 | 同上 + `.../Emission/NativeAotLoweringPlanner.Methods.cs` | L |
| `pps-4-verify` | 3 | planned | main | system/reflection/threading 三 chunk 验证 | pps-3 | b3 | 三 chunk 构建 + fact | STATUS.md | system 通过且无劣化 | `docs/dev/in-progress/page-payload-split/**` | M |

## 六、单元测试规划（用户重点关注）

### 6.1 可测性事实（已实测）

| 事实 | 影响 |
|---|---|
| `BuildGeneratedSources` 是 **`private static`** | 不能直接调；**不改可见性**，改用下面的公开 seam |
| `NativeAotEmitter.GenerateFromArtifacts` 是 **`public`** | ✅ **主 seam**：全流程驱动，返回 `NativeAotResult.GeneratedSources` |
| `NativeAotResult.GeneratedSources` 公开 | ✅ 可直接断言 TU 列表 / 名称 / 内容 |
| 既有 `NativeAotEmitterTests.cs` 用反射测 private static | 既有先例，但**本任务优先用公开 seam**，反射仅作补充 |

### 6.2 三个测试层次

**L1 — 段分类器单测（纯函数，最快）**

| 测点 | 断言 |
|---|---|
| 段标记识别 | 给定含 `// ── A ──` / `// ── B ──` 的文本 → 切出 2 段且归属正确 |
| 无名段 | 无标记内容归入默认段，不丢内容 |
| **内容守恒** | 各段拼接 == 原文本（**关键**：防拆分丢内容） |

**L2 — 分页与预算（核心行为）**

| 测点 | 断言 | 负控 |
|---|---|---|
| 每 TU 不超预算 | 所有 `GeneratedSources` 大小 ≤ 预算 × 容差 | 旧结构下 page 0 **必然超** → 红 |
| 段各自成 TU | 大段的 TU 数 ≥ 1 且不与其他段混合 | 旧结构下只有 1 个巨型 TU → 红 |
| 命名符合 glob | 每个 TU 名匹配 `native-aot.*.page-*.cpp` 或 `native-aot.generated.cpp` | 防 CMake 漏编译 |
| page 0 瘦身 | page 0 大小 ≤ 阈值量级 | 旧结构 71.2 MB → 红 |

**L3 — 链接性守卫（防 Phase 1 回归）**

| 测点 | 断言 |
|---|---|
| 无跨 TU `static` 引用 | 被拆段内不得存在被别段引用的 `static` 表 |
| 共享头声明完备 | 凡在 A TU 定义、B TU 引用的符号，头里必须有 `extern` |

> ⚠️ **L3 是本任务最容易写成假绿的测试**（历史教训：全局计数/存在性断言会被
> 邻近合法文本满足）。**必须锚定到具体符号对**，且**必须做负控**。

### 6.3 负控要求（硬性）

每个 L2/L3 断言在提交前**必须证明在旧结构下会红**：

```
1. 保存新代码 → 回退到旧发射逻辑
2. 跑测试 → 必须 FAIL（记录红了几条）
3. 恢复新代码 → 必须 PASS
4. 两者都记录进 commit message
```

### 6.4 端到端验证（非单测）

| 层次 | 手段 |
|---|---|
| 产物 | 生成后量每个 TU 的行数/字节，确认无超预算者 |
| 构建 | system chunk 真实构建通过 |
| 语义 | fact 回归：system / reflection / threading 三 chunk |

## 七、依赖

```
pps-1-tests ──> pps-2-extern ──> pps-3-split ──> pps-4-verify
   （TDD 先行）      （链接性）        （拆分）        （验证）
```

## 八、风险

| # | 风险 | 触发 | 后果 | 缓解 |
|---|---|---|---|---|
| R1 | `static` 漏改 | 拆出段仍有 static 被跨段引用 | C2065/LNK2019 | Phase 1 独立验证 + L3 守卫 |
| R2 | 段间顺序依赖 | 表有运行期初始化顺序耦合 | 行为变化 | 实测为纯数据；fact 全量回归 |
| R3 | 拆分丢内容 | 分类器边界错 | 静默缺失符号 | **L1 内容守恒断言** |
| R4 | snapshot 大范围变 | 结构变化 | snapshot 红 | 预期内；UPDATE 后**再跑 clean run** |
| R5 | TPG 陈旧 DLL | 改 Generator 未重建 TPG | 静默跑旧 codegen | 改后必重建 TPG |
| R6 | 性能回退 | 跨 TU 丢优化 | 运行期变慢 | **已实测排除**（指令序列相同） |
| R7 | 编译时间上升 | TU 数增加 | CI 变慢 | 观测项；若显著则调预算 |

## 九、备选路径

| 方案 | 何时启用 |
|---|---|
| E：`page0Overhead` 不参与 `accumulated` | 若 Phase 2 发现拆分不足而 page 0 仍超 → 追加 |
| B：`/Zm` + 限 `/MP` | 若拆分后**仍有个别 TU** 超堆 → 环境侧兜底 |
| 回退 | Phase 1/2 各自可独立回滚（Phase 1 不改产物结构） |

## 十、当前建议推进顺序

`pps-1-tests` → `pps-2-extern` → `pps-3-split` → `pps-4-verify`

## 十一、子任务执行策略

```yaml
task_type: roadmap
phase: roadmap
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: pps-1-tests
```

## 十二、三优先级权衡

| 优先级 | 评估 | 结论 |
|---|---|---|
| **P1 性能最优** | 链接性改动零性能影响（实测指令序列字节级相同）；TU 变小利于 MSVC 单 TU 优化 | ✅ 强正向 |
| **P2 架构完美** | 消除「page 0 无界」结构缺陷；复用既有跨 TU 声明机制 | ✅ 强正向 |
| **P3 热更适配** | 不改符号名、不改 ABI，hotpatch 语义与位置不变 | ⚪ 中性 |

**无冲突。**
