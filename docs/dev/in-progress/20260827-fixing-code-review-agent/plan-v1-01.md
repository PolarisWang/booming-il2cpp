# Plan v1-01 — 通用 code-review 结果修复 orchestrator skill

- task_id: `20260827-fixing-code-review-agent`
- 计划文档: `docs/dev/in-progress/20260827-fixing-code-review-agent/plan-v1-01.md`
- 设计文档: `n/a`（设计摘要已由多轮用户讨论拍板，记录在同目录 STATUS.md §设计摘要 + §输入清零，无独立 design 文件）
- 问题清零来源: `direct user confirmation`（多轮：①确认需通用 agent 而非 gc-expert ②确认混合编排 ③确认归属 quality 域）
- arch_review_mode: `normal`（不触及 AOT/IL2CPP/codegen/contracts/loader/test-governance 主线；skill 元文件变更 + 底层 GC 修复经 skill 内部路由，非本项目主线）
- 预期知识沉淀: `n/a`（skill 内容即知识；真实缺陷修复的根因记录走 T3 各实现者 commit 三段式）

> 我正在使用 writing-plans 技能来创建实现计划。

---

## 1. 背景与目标

用户希望一个**通用的** code-review 结果修复 agent，不同于 gc-expert：能接收任意域的 code review 报告，
逐条验证其有效性（meta-review，而非照单全收）、标定严重度、发现 review 遗漏，然后**按域自动分发到底层
domain expert 落地修复并回归**。

现状缺口（已核实）：
- `dev-receiving-code-review` 是**被动**处理哲学（收到意见怎么严谨回应/反驳），不主动审查一条 review 报告的完整性，不分域、不派发修复。
- 仓库已有 `dev-dispatching-parallel-agents`（分发底座）、`dev-subagent-driven-development`（实现期审查）、
  `dev-il2cpp-core-agent` + 12 个 domain expert（底层执行者），可全部复用。
- **缺**：一个"收任意 review → 逐条验证 → 自动路由域 → 协调多 expert 落地修复并回归"的 orchestrator。

**首轮端到端验证样例**（GC 三源码 + 两头部 + 调用方时序已实读，作为 skill 的第一次输入）——
一份共有 4 项真实发现的 review，其中 3 项是原 review 自带、1 项（遗漏 A）是 meta-review 应补上的：

| id | file:line | claim | 原 review | meta-review 判定 |
|----|-----------|-------|-----------|------------------|
| #1 | gc_parallel_mark.cpp:353-356 | drain_started 无超时 wait，注释称 bounded 实为 unbounded | 中 | **真/防错位**：实现当前不挂（store 先于 notify，gc_old_gen.cpp:1289<1343），但注释是谎言 → 根在注释/实现不匹配，改 `wait_for(1ms)` 两行收口；severity 应从"运行时风险"降为"文档诚信+未来脆弱性" |
| #2 | gc_parallel_mark.cpp:137-138 | 手动析构漏 steal_mutex | 中 | **真/防御性**：std::mutex 析构无实际资源逃逸，纯类型不匹配半析构 → low，建议 `worker[i].~MarkWorkerState()` 归一；不阻塞 |
| #3 | gc_worker_pool.cpp:78-81 | ready barrier 缺 notify_all → 确定性 1ms GC 停顿 | 中 | **真/运行期代价**：最实的一条，补 `cv_.notify_all()` 即消；correctness 已由超时兜底 |
| 遗漏 A | gc_parallel_mark.cpp:215-226 | AnyWorkRemaining 持 steal_mutex 读他 worker 的 pending，写者无锁写 → data race + 漏标记窗口 | **未见** | **真/高, meta-review 应补**：互斥域相反，直接关联历史 mark flakiness → 升级为主修复项 |

---

## 2. 交付方式：混合编排（用户拍板）

```
Phase A  Meta-review 逐条验证     → Workflow（fan-out 每项独立盲核查, schema 强校验 verdict）
Phase B  域分派核查(并行)          → Workflow（parallel 到各 domain expert, 独立验证隔离, worktree 可选）
         ─── barrier: 所有核查返回 ───
Phase C  汇总裁决(合并/延后/拒假)  → 主循环内 Agent 派发（你在循环里把仲裁）
Phase D  逐域修复 + 回归           → 每个实现者一个 Agent(worktree 隔离), 主 agent 收回归 + 两阶段审查
```

判定类（A/B：静态可并行、结果结构化）用 Workflow 省去逐帧调度且防单点污染；
仲裁+执行类（C/D：有真实性判断与遗留裁定）留主循环，确保人工把关。

---

## 3. 要创建/修改的文件清单

### 新增（skill 元文件，本项目主线产出）
| 文件 | 职责 |
|------|------|
| `.ai/skills/library/skills/dev-fixing-code-review/skill.manifest.json` | manifest，`domain:"il2cpp"`（与同域 skill 一致）/ 或视 catalog 归 quality；`parent: dev-receiving-code-review`；`status: active` |
| `.ai/skills/library/skills/dev-fixing-code-review/SKILL.md` | 行为规约：三层架构 + Phase A-D + meta-review 判定准则 + 输出契约 + 回归门 |

### 刷新（由生成脚本，非手改长表）
| 文件 | 触发 |
|------|------|
| `.ai/skills/discovery/domain-catalog.json` | 登记新 skill 后跑 `generate_skill_catalog.py` |
| `.ai/skills/discovery/registries/quality.md` | 同上自动刷新 |
| `.ai/skills/discovery/skill-index.md` | 同上自动刷新 |

### 首轮样例真实缺陷修复（T3 由新 agent 分派到底层 expert 落地，非本项目主线）
| 文件 | 修复项 |
|------|--------|
| `src/native/runtime-core/gc/gc_parallel_mark.cpp` | #1 wait_for; #2 归一析构; 遗漏A pending 互斥域 |
| `src/native/runtime-core/gc/gc_worker_pool.cpp` | #3 notify_all |

---

## 4. 任务清单（每项独立、可验证、边界清晰）

### T1 建 skill 壳（本主线产出）
- 创建 `.ai/skills/library/skills/dev-fixing-code-review/{skill.manifest.json, SKILL.md}`
- SKILL.md 必须包含：
  - **职责/不负责边界**（不负责 → 标记 remaining 回派发者，遵循 dev-il2cpp-core-agent 输出格式）
  - **三层架构**：domain-free 控制器；`domain rule = file:line → expert-registry.json`
  - **Phase A meta-review 判定准则**：真/真-防错位/真-运行期代价/遗漏(升主)/可并可延 —— 用 §1 表 4 项作为内置教例
  - **Phase A→B 输出契约**（喂 Phase B，含 verdict/severity/domainHint/fixScope，盲分隔不给答案）
  - **Phase C/D 编排**：仲裁在主循环，实现走 worktree 隔离 Agent + 两阶段审查（遵循 dev-subagent-driven-development）
- 验证：`load skill → cat SKILL.md` 结构完整、无域名硬编码（grep 不出现 `gc_`/`jit_` 等具体域文件）

### T2 用 GC 案例端到端验证通用编排
- 输入：§1 的 4 项 review 数据（构造 review.jsons 输入）
- 跑 Phase A（Workflow 核查 4 项，schema 校验 verdict）→ Phase B（路由到 dev-il2cpp-gc-expert 并行复核）→ Phase C（主循环裁决）→ Phase D（派实现者）
- 验证：4 项 verdict 输出正确（#1 降 sever、#2 low、#3 中、遗漏A high），domainHint 全部路由到 gc-expert，零跨域误路由
- **本步骤证明通用性**：skill 逻辑全程未写死任何域路径，仅靠 `file:line → registry` 推断

### T3 真实缺陷修复落地（首轮样例的工程价值）
- 由 T2 裁决后的清单，派 2 个实现者 Agent（各 worktree 隔离）：
  - 实现者1（GC/heap-mark 域）：#1 wait_for、#2 归一析构、遗漏A pending 互斥域统一
  - 实现者2（并发原语域）：#3 notify_all
- 每项 commit 遵循三段式根因（root_cause/fix_strategy/regression_check），遵循 dev-il2cpp-gc-expert 输出约束
- 回归门：`python tests/runner/test_driver.py --layer unit` 无新增 `[FAIL]`；GC 场景走 stress 选择器（M 并行 mark / F pinned root / D barrier）

### T4 生成 catalog + 登记 + 跨域样例回测
- 跑 `generate_skill_catalog.py` 刷新 registry/skill-index
- 在 INDEX.md 登记 task
- 抽 1 个跨域样例（如一份 JIT review 数据）回测：确认同一 skill 无改动即能路由到 dev-il2cpp-jit-expert，验证通用性不限于 GC
- 归档 completed + 合并提交

---

## 5. 关键测试与验证命令

```bash
# T1: skill 结构 + 无域名硬编码
cat .ai/skills/library/skills/dev-fixing-code-review/SKILL.md
grep -nE 'gc_|jit_|codegen_|/gc/' .ai/skills/library/skills/dev-fixing-code-review/ ; echo "expected: 无输出"

# T4: catalog 刷新 + 登记
python .ai/skills/tooling/catalog/generate_skill_catalog.py
grep -n 'dev-fixing-code-review' .ai/skills/discovery/registries/quality.md .ai/skills/discovery/skill-index.md

# T3 回归（统一测试入口，CLAUDE.md 强制）
python tests/runner/test_driver.py --layer unit   # OVERALL 不得新增 [FAIL]
```

---

## 6. STATUS.md / ACTIVE.md / notes 维护

- `STATUS.md`（主记录）：每个 T 完成后更新 phase + next_step + 待办勾选
- `docs/dev/ACTIVE.md`：任务进行中保持一条指针；T 完结或整体收尾时更新
- `notes/progress-*.md`：仅在阻塞/重要决策/验证失败时追加，不作例行更新点
- 收尾固定链路：`结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交`

---

## 7. 结构告警与架构审视关注点

- skill 是否真的 domain-free：SKILL.md 中不得内嵌任何具体域文件名/路径作为行为分支，须全部走 `file:line → expert-registry.json`
- 与 `dev-receiving-code-review` 职责不重叠：前者（本次）主动 meta-review 报告；后者（保留）被动处理意见——在 manifest 明确 `derived_from`/`parent` 关系防语义混淆
- 编排深度：Phase B 是否应复用 `parallel()` barrier（需所有核查返回再做 C 裁决）→ 是，barrier 正确
- `domain-catalog.json` 登记是否重复/冲突现有 skill

---

## 8. 固定收尾链路

1. 结构告警与架构审视（§7 全过）
2. 测试通过（§5 全绿）
3. 归档 `completed`（INDEX.md + STATUS.md phase 更新）
4. 合并 & 提交（含三-四段式 commit message）
