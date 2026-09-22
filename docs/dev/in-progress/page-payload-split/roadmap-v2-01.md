# roadmap v2-01 — 段间依赖显式化（pps-3 架构修订）

> task_id: `page-payload-split`
> 创建：2026-09-22
> 上游设计：`design-v1-01.md`（brainstorm 已拍板，问题已清零）
> 上游根因：`c1002-heap-exhaustion-rootcause.md`
> 前序 roadmap：`roadmap-v1-01.md`（pps-1..3c 已交付，pps-3 未收口）
> **取代 `roadmap-v1-01.md` 成为当前权威**

## 一、目标

让 pps-3b 拆出的 8 个 payload TU **能够编译通过**，从而真正收口 system chunk 的 C1002 根治。

**当前状态**：C1002 已消除（page 0 从 71MB → 24MB，不再堆耗尽），但构建因段间依赖断裂而失败
（连续 4 轮修复，每修一批暴露下一批）。

## 二、为什么需要 roadmap（而非单一 plan）

1. **必须先修的守卫失效**：pps-1 的 L3 守卫是假绿（正则检出率恒 0），不先修则改完无法验证
2. **跨 4 类符号 + 5 个发射点**：`kSlots_*`(96) / `kChaosGcSlotMaps*`(2) / code_registration(3) / reflection helper(4)
3. **有强串行依赖**：守卫修复 → 清单完备 → 链接性改写 → 契约头 → 验证
4. **会跨多次会话**：单轮 system chunk 构建 ~9 分钟

→ `dispatch_model: sequential`

## 三、范围边界

**做**：
- 重写 L3 守卫，使其**能在真实产物上失败**
- 建立跨 TU 符号的**完整清单**（由登记器产出，非人工枚举）
- 96 个 `kSlots_*` 的 `static` → `extern` 改写
- 发射 payload 契约头，所有 payload TU include
- system chunk 构建 + fact 验证

**非目标**：
- 不改 runtime（`src/native/`）
- 不改符号命名（不改 ABI）
- **不变更段的划分依据**（D3，段边界保持稳定）
- 不处理 hotpatch 分段（`design-b` 的 ABI 约束问题另案）
- 不做无关重构

## 四、阶段列表

### Phase 1：守卫修复与清单完备（前置，可独立验证）

- **goal**：让 L3 守卫真正能抓 `static` 跨 TU 引用；产出完整符号清单
- **exit_criteria**：
  - L3 守卫正则锚定真实定义形态（覆盖 `static const ::ns::Type kFoo[] =`）
  - **负控成立**：在**真实产物**上回退 → 守卫必须红（记录红了几条）
  - 跨 TU 符号清单完备（含 `kRefl_desc_*` / `kGenericMethodArgTokens` 等已发现类别）
  - 登记器可产出该清单（非人工整理）
- **deliverables**：`PagePayloadSplitTests.cs`、符号登记器
- **dependencies**：无
- **resolved_decisions**：
  - 负控**必须用真实产物**，禁止合成 fixture（本项目已两次栽此）
- **watch_items**：
  - 守卫重写后可能暴露**更多**未识别类别（R6）

### Phase 2：链接性与契约头

- **goal**：所有跨段符号对每个 payload TU 可见
- **exit_criteria**：
  - 96 个 `kSlots_*` 全部改 `extern`，产物层面验证符号由 internal → external
  - 契约头 `native-aot.payload.contract.h` 发射，含完整类型定义（防 C2737）
  - 所有 payload TU include 契约头
- **deliverables**：`NativeAotEmitter.Shared.cs`、各发射点
- **dependencies**：Phase 1
- **resolved_decisions**：
  - D1：声明来源 = Planner 登记符号表
  - D2：`kSlots_*` 改 `extern` + 契约头声明
- **watch_items**：
  - 契约头需完整类型（前向声明会导致 C2737）

### Phase 3：端到端验证与收口

- **goal**：system chunk 构建通过 + 无回归
- **exit_criteria**：
  - **system chunk 构建通过**（当前阻断）
  - reflection / threading **不劣化**
  - 守卫双向负控成立
  - 单元测试全绿
  - snapshot 基线变更经人工核对
- **deliverables**：STATUS 更新、验证记录
- **dependencies**：Phase 2
- **resolved_decisions**：验收口径详见 `design-v1-01.md` §七
- **watch_items**：TPG 内嵌陈旧 `Generator.dll` → 改后必重建 TPG

## 五、子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| `pps2-1-guard` | 1 | **completed** | main | 重写 L3 守卫 + 真实产物负控 | — | b1 | 守卫能在真实产物上红 | `PagePayloadSplitTests.cs` | 负控红（记录条数）+ 恢复后绿 | `tests/unit/managed/codegen/**` | M |
| `pps2-2-symbols` | 1 | **completed** | main | 登记器产出完整跨 TU 符号清单 | pps2-1 | b1 | 清单完备（含全部类别） | 登记器代码 + 清单产物 | 清单覆盖产物中所有跨 TU 引用 | `src/managed/Chaos.IL2CPP.Generator/**` | M |
| `pps2-3-extern` | 2 | **completed** | main | 96 个 `kSlots_*` 改 extern + 契约头 | pps2-2 | b2 | 产物符号 external | `NativeAotEmitter.Shared.cs` 等 | 产物验证符号外部可见 | 同上 | L |
| `pps2-4-contract` | 2 | **completed** | main | 契约头发射 + 全 TU include | pps2-3 | b2 | 契约头含完整类型 | `NativeAotEmitter.Shared.cs` | 各 payload TU 可编译 | 同上 | M |
| `pps2-5-verify` | 3 | **completed** | main | system/reflection/threading 验证 | pps2-4 | b3 | 三 chunk 构建 + fact | STATUS.md | system 通过且无劣化 | `docs/dev/in-progress/page-payload-split/**` | M |

## 六、每阶段完成定义

见 §四各阶段 `exit_criteria`。

## 七、依赖

```
pps2-1-guard ──> pps2-2-symbols ──> pps2-3-extern ──> pps2-4-contract ──> pps2-5-verify
   （守卫修复）      （清单完备）        （链接性）         （契约头）          （验证）
```

## 八、风险

| # | 风险 | 触发 | 后果 | 缓解 |
|---|---|---|---|---|
| R1 | 登记遗漏 | 某发射点未登记 | 该符号 C2065 | 守卫从产物反向校验 |
| R2 | `static`→`extern` 改漏 | 96 个中部分漏 | LNK2019 | 守卫 + 产物扫描双向核对 |
| R3 | **守卫再次假绿** | 正则/断言自我满足 | 缺陷逃逸 | **负控必须用真实产物** |
| R4 | 类型不完整 | 契约头只有前向声明 | C2737 | 契约头须含完整类型定义 |
| R5 | TPG 陈旧 DLL | 改 Generator 未重建 | 静默跑旧 codegen | 改后必重建 TPG |
| R6 | 隐藏的第 N 类依赖 | 仍有未发现类别 | 再次失败 | Phase 1 先出完整清单（已实测坐实必要性） |

## 九、备选路径

| 方案 | 何时启用 |
|---|---|
| 回退 pps-3b 段拆分 | 若 Phase 2 证明契约头方案无法覆盖某类依赖 |
| 扩展契约头为 per-TU 声明 | 若全局契约头导致 ODR/编译膨胀 |

## 十、当前建议推进顺序

`pps2-1-guard` → `pps2-2-symbols` → `pps2-3-extern` → `pps2-4-contract` → `pps2-5-verify`

## 十一、子任务执行策略

```yaml
task_type: roadmap
phase: roadmap
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: (none — all children completed)
```

## 十二、三优先级权衡

| 优先级 | 评估 | 结论 |
|---|---|---|
| **P1 性能最优** | 声明与链接性改动零运行期影响（roadmap-v1 已实测指令序列字节级相同） | ✅ 中性偏正 |
| **P2 架构完美** | 把隐式依赖显式化，落地 roadmap-v1 搁置的「声明同步器」 | ✅ **强正向** |
| **P3 热更适配** | 不改符号名、不改 ABI、不改 hotpatch 语义 | ⚪ 中性 |

**无冲突。**
