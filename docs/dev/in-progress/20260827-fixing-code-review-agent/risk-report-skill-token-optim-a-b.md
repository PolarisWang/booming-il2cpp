# 风险评估报告 — Skill Token 优化 (A+B) 与整体改动

- task_id: `20260827-fixing-code-review-agent`
- 报告: `docs/dev/in-progress/20260827-fixing-code-review-agent/risk-report-skill-token-optim-a-b.md`
- 配套: `exec-plan-skill-token-optim-a-b.md`
- 评估范围: A+B 方案 + 前几波全部改动(dev-fixing-code-review skill / expert 孤儿挂接 / catalog 自检 / R2/R3 代码修复 / R4 转出)
- 日期: 2026-08-27
- 方法: 实读源码/hook/registry/catalgen 脚本交叉验证, 非纸面假设

---

## 一、当前已确认无风险（实测排除, 非假设）

| 项 | 结论 | 证据 |
|----|------|------|
| 5 孤儿 expert 修复 | 无风险 | 孤儿=0; sub_controller 全有 domain; 端到端可达(allReachable=true) |
| R2 析构归一 | 无风险 | worker_count=desired、构造循环到 desired、OOM 在构件前、placement-new 不抛 → 不触未构造析构 |
| 新 skill dev-fixing-code-review 登记 | 无风险 | manifest(quality/active) + quality.md + skill-index 计数 5 正确 |
| core-agent 不入 registry | 无风险(设计正确) | core-agent 是 Controller 非 domain expert, 由 dev-il2cpp 直连 |
| R4 optimizer 在 gc-ctl | 无风险(设计正确) | gc-ctl routing_strategy 显式 task_type 映射, 是有意跨域协作 |

---

## 二、A+B 方案自身的剩余风险（按严重度）

### 🔴 R1 — A1 拆分 registry → 孤儿自检静默失效（HIGH）
**触发**: 拆分 registry 后 `generate_skill_catalog.py` 的 `check_expert_orphans()` 仍只读 `expert-registry.json`(核), 而孤儿判定依赖已移到 routing 文件的 `expert_keywords`。
**后果**: 孤儿集合恒为空 → 我上一波做的防复发自检(曾验证模拟孤儿能触发 warn)**失能**, 未来加 expert 漏挂 domain 不再被捕获。
**防线**: 已在执行方案 Step 1 明确「孤儿自检改读核+routing 合并」; 验证 Step 4.2 模拟孤儿回归。
**残余**: 若执行时漏改生成脚本 → 恢复代价低(基准备份回归)。

### 🔴 R2 — A2 下沉 dev-il2cpp → hook 强制门断裂（HIGH）
**触发**: `check_classification.py` :212 强制「`loaded_expert` 首位必须是 `dev-il2cpp`」, 且 :206 查 SKILL.md 存在。若 A2 下沉时误删"写首位 dev-il2cpp"动作, hook 拦截全部 Edit/Write/Bash。
**后果**: 会话瘫痪(无法用任何写工具)。
**防线**: 方案明确保留该动作; 验证 Step 4.3 空操作 Edit 确认不拦截。
**残余**: 极低(有明确护栏+回滚)。

### 🟡 R3 — 拆分引用多处联动漏改（MEDIUM）
**触发**: `expert-registry.json` 被 7+ 处引用(core-agent SKILL / 生成脚本 / dev-fixing-code-review SKILL / orchestrator.workflow.js / 3 个 ctl md)。拆文件后任一处仍指旧单文件 → 该路径读取失败或读到不完整数据。
**后果**: 路由歧义/孤儿自检错/生成中断。
**防线**: Step 0.2 先记录全引用; Step 4.4 全仓 grep 复核。
**残余**: 执行纪律问题, 方案已强制列清单。

### 🟡 R4 — B 去重改 13 处同步漂移（MEDIUM, 但对冲后 LOW）
**触发**: 13 个 expert 警示块抽为单行引用后, 若引用路径写错或共享模板缺失 → 该 expert 加载时无加载说明。
**后果**: 个别 expert 失载说明(低危)。
**防线**: 共享模板先行 + 每处引用后立即验证文件存在。
**残余**: LOW。

### 🟢 R5 — A 拆分影响 `workflow_templates` 读取（LOW）
**触发**: core-agent :116 读 `workflow_templates`(将移到 routing)。若未同步改 core-agent 文档路径 → 该模板读不到。
**后果**: 多域 Workflow 委托模板缺失。
**防线**: Step 1 明确联动改 :116 引用。
**残余**: LOW。

---

## 三、跨批次的综合剩余风险（A+B + 前几波, 非本方案独有）

### 🔴 R6 — 多批改动未提交 + main 上 pre-existing 污染（HIGH, 持续累积）
**事实**: 当前在 main; 工作区有前几波 skill/expert/R2R3 + 本批 A+B 的全部改动, 与 **17 个非我的 pre-existing 改动**(fact-266 codegen、gc_old_gen、parse_convert 等, 来自并行工作线)混杂。
**后果**: 任何 `git add -A` 或误带 → 污染提交/把他人半成品提交。memory `[[recurring-branch-switch-to-gc-config]]` 已记录同类事故 2 次。
**防线**: 方案 Step 5 明确「建分支 + 精确 add 我的边界文件」。但**未提交窗口持续存在, 是当前最高实际操作风险**。
**建议**: A+B 落地后立即建分支提交, 缩短暴露窗。

### 🟡 R7 — 并行工作线冲突（MEDIUM）
**事实**: gc-align-coreclr active + 另一条改 gc_old_gen 的线。A+B 动的是 `.ai/skills/`(技能域) + 我之前的 R2R3 碰 `gc_parallel_mark.cpp`/`gc_worker_pool.cpp`。
**后果**: 若并行线也提交 GC 文件, 与我 R2R3 改动冲突。
**防线**: R4(GC 终止协议)已刻意转出给他线不碰; R2R3 改动极小(8/4 行)冲突面小。
**残余**: LOW-MEDIUM, 建议提交前 git pull/rebase 核对。

### 🟢 R8 — A+B 是文本/配置改动, 不触发代码回归（LOW）
A+B 全部改动为 skill 文档 + registry JSON + 生成脚本读取路径。不碰 GC/JIT/编译路径 → 无 native/编译测试回归面。只需 catalog 再生成 + hook 验证 + 孤儿自检回归。
**残余**: 低, 验证面干净。

---

## 四、风险矩阵汇总

| # | 风险 | 严重度 | 可能性 | 缓解 | 残余 |
|---|------|--------|--------|------|------|
| R1 | 孤儿自检静默失效 | 中 | 中 | 生成脚本改合并读+回归测试 | 低(基准回滚) |
| R2 | hook 强制门断裂 | 高 | 低 | 保留首位+空操作验证 | 极低 |
| R3 | 拆分引用漏改 | 中 | 中 | 先列引用清单+grep复核 | 执行纪律 |
| R4 | 警示块同步漂移 | 低 | 低 | 共享模板先行+存在校验 | 低 |
| R5 | workflow_templates 读取 | 低 | 低 | 联动改:116引用 | 低 |
| R6 | 未提交+pre-existing污染 | **高** | **高** | A+B后立即建分支精确提交 | **持续, 当前最高实际风险** |
| R7 | 并行线冲突 | 低-中 | 中 | R2R3极小+提交前核对 | 低-中 |
| R8 | A+B触代码回归 | 低 | 低 | 无代码改动 | 低 |

---

## 五、结论
A+B 方案本身风险可控(有明确护栏 + 可回滚 + 验证面干净)。**真正的当前最高风险是 R6(未提交 + pre-existing 污染)**, 属操作风险非设计风险, 靠"短暴露窗 + 精确提交边界"缓解。建议: 方案按 Step 0-5 执行, 其中 Step 5(建分支提交)在 A+B 验证通过后立即做。
