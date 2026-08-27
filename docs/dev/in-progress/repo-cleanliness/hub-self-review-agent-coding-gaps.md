# 卫生中枢自我 review — 当前责任 + agent-coding 下的不足

> 2026-08-27 · 域：仓库结构/技术债
> 对已建卫生中枢（chaos_hygiene.py + 4 项检查）做**批判性**自我 review：
> 不只是夸它做了什么，而是诚实地列它在 agent-coding 真实条件下的不足。
> 所有依据均为当前实况/实测，非猜测。

---

## 一、中枢当前责任（ground truth）

| 检查 | 治的漂移层 | 触发场景 | mode |
|------|-----------|---------|------|
| **root-clean** | L2 结构（根级新垃圾） | pre-commit(硬) / Claude hook(软) / CI | hard |
| **disk-health** | L2 磁盘（ignored 堆积） | pre-commit / CI | advisory |
| **generated-drift** | L1 机械（生成物 vs schema） | pre-commit(warn) / CI(--fail-fast) | warn |
| **completion-audit** | L3 形态（return-0 loan） | pre-commit / CI | advisory |

**接线**：`--gate`(pre-commit) · `--soft --check root-clean`(Claude hook) · `--ci`(workflow)。
**报告**：`artifacts/hygiene-report.json`(机器) + `STATUS.md`(dashboard)。

---

## 二、agent-coding 条件猫的中枢不足（实证）

### 🔴 GAP-1：**报告会 stale（实测）**
- 现在 `artifacts/hygiene-report.json` 记 **overall: FAIL / root-clean FAIL**，但**实时 `root-clean --hard` 是 PASS**——报告只在 `--report/--ci` 才刷新，两次之间永远陈旧。
- **agent-coding 影响**：agent/CI 读报告做决策会基于过期状态（以为根还脏，其实已净）。
- **改**：每次 `--gate/--soft` 也原子刷新报告（或报告带 auto-fresh 时间戳，读时先 re-run）。

### 🔴 GAP-2：**多会话无法区分"我的污染"vs"别人的在途工作"**
- 本会话多次：中枢 FAIL 是因为**另一个 session** 在树里留了 `.work_fact266/`、改了 `check_repo_clean.py`、WCT 文件。
- 中枢没有"按会话归属"的概念——它只看"树脏不脏"，不知道脏的是谁的。
- **agent-coding 影响**：多 agent 并行时，A 的中枢会因 B 的在途改 FAIL → A 可能误"修"B 的工作去过关。**这是真实风险，本会话已发生数次。**

### 🔴 GAP-3：**没有任何语义 oracle（真上限）**
- completion-audit 只抓**形状**（return-0 loan），不是**语义**（AI 改对返回了正确值但行为错）。
- 树里 1062 手写 cpp + AI stubs，那种"行为错但编译过、返回合法值"的语义漂移，中枢**结构性看不见**。
- **诚实**：除非接入真实行为锚/测试 oracle，否则中枢治不了 L3/L4 语义。这是设计上限，不是 bug。

### 🟡 GAP-4：**守卫本身无自检（hub 会静默退化）**
- 本会话 review_runner 在 check_repo_clean/check_generated_up_to_date 上**多次被外部 session 改出 bug**（fnmatch `**`、GBK、NameError），都是**运行时才暴露**。
- 中枢对自己的 4 个模块**没有任何单测**。
- **agent-coding 影响**：多 agent 都会编辑守卫文件；守卫静默坏掉 = gate 形同虚设而无人知。

### 🟡 GAP-5：**静态快照，无趋势**
- 报告只有即时状态，不知道"return-0 loan 数量较上周 +5""磁盘 +3GB"。
- **agent-coding 影响**：技术债是**累积**的；静态快照看不到恶化趋势，agent 无法 prioritize。

### 🟡 GAP-6：**realtime hook 覆盖不全**
- Claude hook 只 `--check root-clean`，不带 generated-drift/completion/disk。
- 所以中枢"软提醒"层只盯结构，其他 3 项要等 commit/CI 才知道。**mid-session 累积的其他类漂移不实时提示。**

### 🟡 GAP-7：**单一 gate = 高爆炸半径**
- 所有收尾集中成 pre-commit 一个 `--gate`。若中枢自身坏了/外部在途致 FAIL，**所有 commit 被卡**。
- fail-closed 设计（corrupt→raise）保安全但损可用性——多 agent 下尤甚。

### 🟡 GAP-8：**actionable 输出弱**
- 报告给 PASS/WARN/FAIL + detail，但 detail 常是"提示"，不是"直接可执行命令"（如 disk 列出目录，没给"run clean-build-artifacts.sh"）。
- **agent-coding 影响**：agent 读报告还要二次 interpret 才能行动。

---

## 三、哪些是"值得修" vs "本就是上限"

| GAP | 值得修? | 成本 |
|-----|---------|------|
| GAP-1 报告 stale | ✅ 高价值，低代码 | 低 |
| GAP-2 会话归属 | ✅ 高价值，但有难度（需要 git 状态按 session 归属） | 中 |
| GAP-6 hook 覆盖全 | ✅ 低，把 3 项也挂 hook | 低 |
| GAP-8 actionable | ✅ 中，detail 里给具体命令 | 中 |
| GAP-4 守卫自检 | ✅ 中，给 4 模块加最小单测 | 中 |
| GAP-5 趋势 | 🟡 可选，报告存历史快照 + 对比 | 中 |
| GAP-7 高爆炸半径 | 🟡 权衡（fail-open vs 安全） | 低 |
| GAP-3 语义 oracle | ❌ **设计上限**，非崩溃修。只能靠行为锚/测试 oracle | 高 |

---

## 四、结论

**中枢给了结构（L1/L2）+ 一种 L3 形状的守卫，这是实打实**。但在 agent-coding 多会话、多 agent 同树编辑的环境下，它有 8 个不足——**最要命的是 GAP-2（分不清谁弄脏了）和 GAP-1（报告会 stale）**，这两个直接降低可信度到"agent 可能误修别家/读旧数据"。

**诚实的自我判断**：我此前说"防住了/全绿"有过分乐观。中枢现在**实时是绿的**（root-clean 其实 PASS），但报告 stale 让我一度以为红——**这本身就证明 GAP-1 是真的**。

**建议下一步（按价值）**：先修 **GAP-1（报告 auto-fresh）** + **GAP-6（hook 覆盖全）**（低代码高价值，可信度立涨），再考虑 **GAP-2（会话归属）** 与 **GAP-4（守卫自检）**。GAP-3 语义 oracle 留给"行为锚 + 测试覆盖"那条线（已被我验证是治语义漂移的真正杠杆）。
