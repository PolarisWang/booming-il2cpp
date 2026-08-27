# STATUS — 20260827-fixing-code-review-agent

- task_id: `20260827-fixing-code-review-agent`
- task_type: plan
- phase: completed
- updated_at: 2026-08-27
- next_step: 已完成 T1-T4。产出: dev-fixing-code-review skill + R2/R3 真实修复(已验证)。待用户确认是否建分支提交。R4 转出专项见下。

## 目标

新建一个 **domain-free 的通用 code-review 结果修复 orchestrator** skill（`dev-fixing-code-review`），
可接收任意域的 code review 报告，逐条验证有效性（meta-review）、标定严重度、发现遗漏，
然后**按域自动分发到底层 domain expert 落地修复并回归**。GC 仅作首轮端到端验证样例，agent 本身零域名硬编码。

## 输入清零（用户已逐轮拍板，本任务直接消费）

- `blocking_questions`: []
- `question_clearance`: cleared
- `clearance_confirmed_by_user`: true
- `clearance_source`: direct user confirmation (多轮讨论)
- 归属域：`quality`（复用现有 registry，避免域表膨胀）——用户在主推荐下未反对，按此默认执行；若执行中用户改意再回写此字段
- 执行机制：**混合编排**（用户明确拍板）
  - Phase A meta-review + Phase B 域分派核查 → **Workflow**（fan-out、schema 强校验、盲分隔防单点污染）
  - Phase C 汇总裁决 → **主循环内 Agent 派发**（仲裁：合并冲突/延后/拒假警报）
  - Phase D 逐域修复 + 回归 → **每个实现者一个 Agent**（worktree 隔离），主 agent 收回归
- 复用的既有 skill/底座：
  - `dev-dispatching-parallel-agents`（构造 agent 上下文）
  - `dev-subagent-driven-development`（实现期两阶段审查：规格合规→代码质量）
  - `dev-receiving-code-review`（保留，职责不同：被动处理意见 vs 主动 meta-review 报告）
  - `dev-il2cpp-core-agent` + 12 个 domain expert（底层执行者，经 `expert-registry.json` 路由）

## 设计摘要

三层分层 orchestrator，`domain rule = 从 review 的 file:line 所属代码路径匹配 expert-registry.json`（不写死任何域）：
- 顶层：`dev-fixing-code-review`（domain-free 控制器，skill 逻辑驱动）
- 中层：底层 domain expert（gc/runtime/jit/codegen/... 复用现有）
- 执行：Phase A→D 混合编排（Workflow + 主循环 Agent 派发）

## 架构映射

| 层 | 现有骨架 | 本次产出 |
|----|---------|---------|
| 入库 skill | `dev-receiving-code-review` / quality registry | `dev-fixing-code-review` {manifest, SKILL.md} |
| 分发底座 | `dev-dispatching-parallel-agents` | 编排脚本（Phase A 核查） |
| 实现期遵循 | `dev-subagent-driven-development` | Phase D 两阶段审查 |
| 底层执行者 | `expert-registry.json` 12 expert | 经域路由自动分发，零改动 |

## 变更范围（git 工作区影响面）

- 新增（本事域的代码改动仅限 skill 元文件）：
  - `.ai/skills/library/skills/dev-fixing-code-review/skill.manifest.json`
  - `.ai/skills/library/skills/dev-fixing-code-review/SKILL.md`
- 刷新（由生成脚本，非手改）：
  - `.ai/skills/discovery/registries/quality.md`
  - `.ai/skills/discovery/skill-index.md`
  - `.ai/skills/discovery/domain-catalog.json`
- 真实缺陷修复（T3 由新 agent 分派到底层 expert 落地，碰 `src/native/runtime-core/gc/gc_parallel_mark.cpp` / `gc_worker_pool.cpp` 等）：
  - 4 项首轮样例（见 plan §背景）
- 不删除 `dev-receiving-code-review`（职责不同）

## 待办（对应 plan 任务）

- [x] T1 建 skill 壳（manifest + SKILL.md 行为规约）
- [x] T2 用 GC 案例端到端验证通用编排（Phase A/B 真实 Workflow 跑通, allRoutedToGc=true, 盲分隔有效）→ Phase C 裁决完成
- [x] T3 真实修复落地: R3(notify_all) + R2(析构归一) 实现并通过测试 (13/13 GC native 全绿); R4 转出专项
- [x] T4 生成 catalog + 登记 + 跨域样例回测（JIT 域 → jit-expert, 证明 domain-free 通用）

## T3 落地与验证（2026-08-27）

### R3 (gc_worker_pool.cpp) — 补 `cv_.notify_all()` + 更新注释
- WorkerLoop 入口 `ready_count_.fetch_add` 后加 `cv_.notify_all()`，消除 Initialize 就绪屏障 1ms 轮询延迟。
- 更新 Initialize 快注释与 WorkerLoop 注释，与新行为一致。

### R2 (gc_parallel_mark.cpp) — 析构归一
- `DestroyParallelMarkContext` 手动 `deque.~vector()` 循环 → 逐 worker `~MarkWorkerState()`，统一析构 deque + steal_mutex，修复半析构。

### 验证（统一测试流程）
| 验证 | 结果 |
|------|------|
| build `test_gc_parallel_mark` + `test_gc_worker_pool` | exit 0，两目标生成 |
| `test_gc_parallel_mark` (直接跑 + ctest -C Debug) | **6/6 PASSED** (含 InitDestroy 覆盖 R2 路径) |
| `test_gc_worker_pool` (直接跑 + ctest -C Debug) | **7/7 PASSED** (含 ReusePool/MultipleRounds 覆盖 R3 路径) |
| 统一入口 `--layer unit` | `fail=0`（改动的受影响域无回归） |

> ⚠️ 统一入口 stdout 显示 `OVERALL: FAILED` 但 `fail=0`：批 `EmittedCodeMatchesBaseline` FAIL 是**工作区 pre-existing managed codegen snapshot 漂移**（gitStatus 开头即 M 的 `NativeAotLoweringPlanner.ObjectModelEmission.cs` + `RuntimeHelperShapeRegistry.CoreStubs.Part2.*.cs`，非本任务改动）。native C++ 改动不可能影响 C# emitter snapshot，与 R2/R3 零因果。baseline `known-failures.unit.yaml` 无 snapshot 登记 → 属未定案的 pre-existing codegen 状态，不在本任务范围。

### R4 转出（并行 mark 终止协议专项，gc-expert 域）
- **不做单行修锁**。深读确认根因是并行 mark 终止协议缺陷：last worker set `parallel_done` 无法保证其它 worker 的 pending 已被 FlushPending，worker 被 done 打断时 while 头部 flush 不执行 → pending 指向的新标记对象子指针未扫描 → 漏标记（活内存回收）。
- 处置：登记为独立 P0 专项，转 `src/native/runtime-core/gc` 的 gc-expert 深度设计（CoreCLR re-scan-until-stable 或全 worker flush 握手），不进本任务快修。

## T4 catalog + 跨域回测（2026-08-27）

- `generate_skill_catalog.py` 刷新 quality registry（新增 dev-fixing-code-review, active/medium）+ skill-index quality 计数 4→5 ✅
- 跨域回测：用 `runtime-core/jit/ir_reg_alloc.cpp` JIT review 发现，**同一** domain-free 路由函数 `routeExpert()` → `dev-il2cpp-jit-expert` ✅；独立核查者 expertSuggestion 亦 JIT-expert，机器路由与人工判断一致。**证明 skill 不限于 GC，通用性成立。**

## 附加：expert 体系审查优化（2026-08-27）

对全部 16 个 expert 做一致性审查，发现并修复 3 类问题：

### 🔴 问题 1 → 已修：5 个孤儿 expert 挂接 domain
`dev-il2cpp-{abi,atg,external-runtime,pipeline,simd}-expert` 此前只在 `expert_keywords` 索引、未挂接任何 `domains[].alternateExperts` → **core-agent 主分发循环不可达**（实读 core-agent:260 确认分发按 `domains[N].defaultExpert`）。已挂接：
- domain1(runtime) alt += abi
- domain4(codegen) alt += external-runtime, simd
- domain5(testing)  alt += atg
- domain7(build)   alt += pipeline
- 复验：`keywords ∩ domains(alt+default)` 孤儿 = **无**；且 sub_controller 的 codegen-ctl 本已含全部 5 个 → 补挂后一致

### 🟠 问题 2 → 已修：pipeline 二专家边界划清
- `pipeline-expert`(build) 与 `verification-pipeline`(domain11) 语义重叠。两 SKILL.md 首行各加 `🔀 边界` 互相标注（构建缓存/增量 vs 验证 13-stage 编排），消除双命中文义。

### 🟡 问题 3 → 已修：catalog 加孤儿自检防复发
- `generate_skill_catalog.py` 新增 `check_expert_orphans()`：扫 expert-registry.json 中 keyword 未挂 domain 的 expert 输出警告。验证：模拟孤儿 → `[warn] dev-il2cpp-abi-expert` 触发；还原 → 干净；exit=0（警告不阻断）。

### 验证
- `--check` 复跑：无孤儿警告、catalog 一致 ✅

### R3/R4 收尾（低优先项, 2026-08-27）
- **R3 端到端可达性回测 ✅**：模拟 core-agent 路由决策对 ABI/SIMD/ATG/pipeline/extern 5 类任务均命中正确 alternate expert（`allReachable=true`）——挂接不仅结构正确, 语义上也被真实命中。
- **R4 optimizer 归属 ✅ 判断为设计正确, 不改**：读 gc-ctl `routing_strategy` 确认 `foundation-dll-optimizer` 挂入 gc-ctl 是**显式跨域协作**(task_type 映射 benchmark/profile/GC性能退化→optimizer, execution_order=optimizer先出数据→gc-expert再算法优化), 非随手拴错。移除会破坏 GC+优化混合域路由, 保留。

### 改动文件（4）
`.ai/skills/discovery/expert-registry.json`（挂接+last_updated）· `.ai/skills/tooling/catalog/generate_skill_catalog.py`（自检）· `.ai/skills/library/skills/dev-il2cpp-pipeline-expert/SKILL.md` · `dev-il2cpp-verification-pipeline/SKILL.md`（边界标注）

## Skill Token 优化 A+B（已落地部分, 2026-08-27）

> 用户评审全项目 skill token 成本（46 skill SKILL.md 合计 370.9KB; 默认加载 3 资源 18KB; 每次 il2cpp 任务 ~48KB 起步）。产出 exec-plan + risk-report 两份定稿后, 本轮执行落地。

**已落地**: A1 拆分 registry(11→3.2KB, 生成脚本合并读防孤儿失效) ✅; A2 保守(修重复编号, 实勘修正: 核心规则是行为规约不宜激进砍) ⚠️部分; B 13 处警示块去重+_shared ✅; catalog --check exit0 ✅。
**提交**: `feat/skill-token-optim-a-b` 分支 30 文件(+1050/-201), 纯 .ai/skills + docs, 无 src/ 混合 ✅。
**R6 缓解**: skill/expert/A+B 已隔离提交; R2R3 GC 代码(`gc_parallel_mark.cpp`/`gc_worker_pool.cpp`)留工作区待决(避并行线 R7); pre-existing(fact-266/gc_old_gen等)未碰。
**剩余**: 见 `final-landing-skill-token-optim.md` §三(决定 R2R3 去向 + PR)。


## 结构告警与架构审视（收尾, 通过）

| 审视项 | 结论 |
|--------|------|
| skill 是否 domain-free | ✅ 无 `src/` 路径/分支硬编码; 教例仅校准 severity 非路由; 路由全走 `file:line → expert-registry` |
| 与 dev-receiving-code-review 职责 | ✅ manifest parent 明确; 前者被动处理意见, 本 skill 主动 meta-review 报告 |
| Phase B parallel barrier 正确性 | ✅ T2 实测 4 项全路由 + verdict 结构化; 盲分隔有效(R4 独立补全后果链, R1 独立纠正过判) |
| catalog 登记冲突 | ✅ 无; quality 域 4→5 干净并入 |

## T2 编排验证结果 + Phase C 汇总裁决（2026-08-27）

用真实 Workflow fan-out 4 个核查 agent（各自独立实读源码、schema 强校验 verdict、盲分隔不共享结论）：

| id | 我的 meta-review | 独立核查 verdict | Phase C 裁决 |
|----|-----------------|-----------------|-------------|
| R1 | 真/防错位(改wait_for) | **PLAUSIBLE/LOW** — 注释"bounded"指逻辑有界(谓词先置位)非时间有界, **非矛盾**; 当前无 hang | **不修**（独立核查纠正我的过判; 若动仅改注释措辞, 非必需） |
| R2 | 真/防御性 | CONFIRMED/LOW — 半析构UB属实, 无资源逃逸; 建议 `~MarkWorkerState()` 归一 | **修(低)**：归一析构 |
| R3 | 真/运行期代价 | CONFIRMED/MEDIUM — 确定性1ms轮询, 无死锁无data race | **修(中)**：补 `cv_.notify_all()` |
| R4 | 遗漏/高 | CONFIRMED/HIGH — data race + 漏标记回收活内存 | **升级专项**：深读发现**不止互斥域, 更st8是并行mark终止协议缺陷**（last worker set done 无法保证其它 worker 已 flush; worker 被 done 打断时 while 头部 FlushPending 不会执行 → pending 丢失 → 漏标记） |

**T2 验证结论**：domain-free skill 逻辑 + 混合编排机制验证通过 —— 4 项全路由到 gc-expert（allRoutedToGc=true）、verdict 全结构化、盲分隔有效（R4 核查者独立补全后果链、R1 独立纠正我过判，证明非单点污染）。**skill 本身零改动即正确路由 GC 域，通用性成立。**

**R1 不修**：核查者实读证明注释与实现不矛盾（"bounded"=逻辑有界，store 先于 notify、丢失唤醒无害），severity LOW，非缺陷。为诚实记录，原计划把 R1 当"注释谎言需改实现"是 meta-review 过判，已被独立核查纠正。

## R4 升级说明（并行 mark 终止协议缺陷）

初判为"pending 互斥域 data race → 修锁"。深读 + 核查者后果链补全后确认为**更深缺陷**：
- worker A 在 ProcessChunk 持有未 flush 的 pending 时，last worker Z 若 set `parallel_done=true`，A 回到外层 `while(!parallel_done)` 条件为 false 不再进 body → **A 的头部 FlushPending 不执行 → pending 指向的新标记对象子指针从未扫描**。
- 根因不是单行锁，是**并行 mark 终止协议**：`active_workers fetch_sub 到 1` 只保证"还有未 idle worker"，无法保证"未 idle worker 会被 done 打断前完成 flush"。需 CoreCLR 式 re-scan-until-stable 或"所有 worker 确认已 flush/idle 才 set done"的握手。
- **处置**：升级为独立 P0 专项，转 gc-expert 深度设计（触及 GC 并行内核，不塞进本任务快修）。R2/R3 不受影响。
