# 回收风险复查 + 优化方案（对整套方案的批判性review）

> 2026-08-27 · 承接 options-2-3-analysis.md。**对已落地的整套洁净度方案做风险
> 复查**，诚实指出 bug/盲区/可优化点，并给出下一步方案。
> 每项均为实证（含一次已确认并修复的真实bug）。

---

## 一、已确认并修复的真实风险

### 🔴 RISK-1：registry glob 匹配不可靠 ★已修
- **实证**：Python `fnmatch` 把 `**` 等同于 `*`，且 `*` 不跨 `/`。我的
  `tests/e2e/translation/**/managed/subjects/*.json` 对 **1 目录深**能匹配、
  **N 目录深**不匹配（inconsistent）。最初测试"通过"是**巧合**（hardcode 前缀 + `*` 吞 `/`）。
- **已修**：`_matches_any` 改为自研 git 风格 glob→regex 转换器（`**/`→`(?:[^/]+/)*`，
  `*`→`[^/]*`），按 pattern 缓存。验证 real(2深)=T / shallow=T / wrong-file=F。
- **教训**：registry/守卫这类"匹配规则"依赖库语义，不查清 `**` 语义会静默错。

---

## 二、仍存风险（未修，需决策）

### 🟡 RISK-2：守卫只拦"可提交的垃圾"，**ignored 垃圾悄悄堆积不可见**
- **实证**：`.claude/` 现 **484MB**（几乎全 `worktrees/` 并行会话拷贝）；
  `build/` 394MB（外部 GC 工作重建）；`__pycache__` 52 处。
  这些都 gitignored → `git status --porcelain` 根本不显示 → **我的守卫完全看不见**。
- **本质**：整个方案治"git 结构干净"，但**磁盘仍可能静默被 ignored 产物填满**。
  这是最大概念盲区。
- **优化候选**：加一个"忽略垃圾磁盘健康检查"（`--disk` 模式，扫描 X 阈值以上的
  ignored 目录并报给 C hook/CI），或复用 `scripts/clean-build-artifacts.sh`。

### 🟡 RISK-3：C hook 每次工具调用 ~1.2s 开销
- **实证**：`check_repo_clean.py --soft` 全跑 1.2s（2 次 git status + 1 次 ls-files）。
  作为 C hook 在每次 Edit/Write/Bash 后执行，长会话累积可感知。
- **优化候选**：C hook 只检测"自上次调用是否有变化"（用 state 文件 + 轻量 mtime/单次
  `git status --short`），只有变化才全量扫。或把 C hook 的扫描降级为低频（每 N 次调用）。

### 🟡 RISK-4：②-B 移动的脚本可能是"死代码"
- **实证**：`populate_*`、`check_wiki_links` **无任何 workflow/hook/script 引用**
  （我移动前没验活，直接搬了）。可能是历史上一次性脚本，现在孤儿。
- **优化候选**：逐个确认——若真一次性，`scripts/archive/` 归档或删，而非留在 `scripts/codegen/`。

### 🟡 RISK-5：`.claude/worktrees/` 无回收机制，无限增长
- 并行会话（GC 线等）会建 worktree，gitignored、守卫看不见、无清理。484MB 且只增不减。
- **优化候选**：纳入磁盘健康检查 + 一个 `worktrees/` 回收脚本（保留未合并分支的 by age）。

---

## 三、优化方案（阶梯，各自给权衡）

### 优化 ①：磁盘健康守护（补上"ignored 垃圾不可见"的最大盲区）— 推荐
**做法**：`cleanliness` 加 `--disk` 模式：
- 扫描 `git status --ignored` 的顶层目录，按大小排序。
- 超阈值（如 200MB 单目录、总 500MB）→ 报"磁盘堆积警告"。
- 挂 CI 或 `scripts/clean-build-artifacts.sh` 联动（提示可回收项）。
- 把 `.claude/worktrees/`、`build/` 等列入"已知会再生的 ignore 根"白名单，避免误报。

**权衡**：成本低；补上最大盲区；但要避免对正常输出根（build/）误报，需白名单。

### 优化 ②：C hook 降频 + 变化检测（解性能风险）
**做法**：C hook 改为"上次调用的 worktree SHA 或 `.claude/` 目录 mtime 变化才全量扫"，
否则静默。或 hook 每 20 次工具调用扫一次。

**权衡**：低；消体验卡顿；但实时性略降。

### 优化 ③：孤儿脚本排查（解死代码）
**做法**：逐个验证 `populate_*`/`check_wiki_links` 是否被调用；孤儿 → archive/删。
不新增机制，只清理。

**权衡**：最低；减少维护面。

### 优化 ④：worktrees 回收（解磁盘堆积）
**做法**：`scripts/cleanliness/prune-worktrees.sh`——列出 `.claude/worktrees/*`
已合并到 main 的 branch，by age 回收未合并的；dry-run 默认。

**权衡**：中；安全（dry-run + 未合并警告）；但需确认并行 session 不会误杀在途 worktree。

### 优化 ⑤（远期）：guard 正则 ReDoS 硬化 + 全守卫单测
- 当前 registry 是 committed 可信输入，ReDoS 非真实威胁；但若未来 registry 允许
  外部传入，需硬化 `_glob_to_regex`（禁嵌套 `**` 量词）。
- 补 `unit` 测试直接测 `check_repo_clean`（现无单测，靠手工 + pre-push review）。

---

## 四、推荐组合

| 优先级 | 优化 | 理由 |
|--------|------|------|
| **本轮** | ① 磁盘健康 + ③ 孤儿清理 | ① 补最大盲区（.claude 484MB 证明存在）；③ 零成本减维护面 |
| **下轮** | ② hook 降频 + ④ worktrees 回收 | 解性能 + 磁盘堆积 |
| **远期** | ⑤ ReDoS 硬化 + 守卫单测 | 巩固质量 |

> **核心诚实结论**：清洗方案已把"**git 结构**"收拾干净且自保护；但"**磁盘**"仍会
> 静默被 ignored 产物填满（.claude 484MB 就是活证据）。若要真"保持目录干净"，
> 下一步的杠杆在**磁盘健康守护（优化①）**，而不在 git 结构——后者已到位。

## ✅ 已执行（2026-08-27）

### 优化① 磁盘健康守护 — `check_repo_clean.py --disk`
- 实测盲区被证实：`.claude/` (worktrees) + `build/` 是最大 ignored 堆积，git 结构守卫完全看不见。
- 落地：`run_disk_check()`——扫 root-direct ignored 输出根 + 已知输出根
  (DISK_KNOWN_ROOTS: build/artifacts/.claude/results/optimization-campaign)，
  有界 os.walk (4000 file budget + lower-bound 报告) 避免 `du` 挂起/慢；
  已知根报 info、非预期超 200MB 根报 ALERT 并 exit 1。
- 实证：`leak_test/` 260MB → ALERT exit 1；正常状态 (.claude/build/artifacts) → info exit 0。
- 注意：.claude 有界 walk 返回 lower-bound (93MB 而非 484MB)，作为健康信号够用；精确值留给手工 du。

### 已知 reviewer 误报（L3 mojibake × 2 次，均证伪）
- pre-push review 在 options doc 与 structure-governance/check_repo_clean 上都报
  "mojibake/lone surrogate (\udc95)"。**均为 false positive**：两文件 strict UTF-8 decode
  通过 + round-trip encode==原始字节，证实**不存在** lone surrogate（UTF-8 本就不含代理）。
  reviewer 把合法的 CJK/box-drawing 字符（─ ‖ → ·【】中文）误判为编码损坏。此模式在
  含中文的改动上会反复出现，可预先知晓并忽略。

### 优化③ 孤儿脚本清理 — 归档 6 个一次性脚本
- 实证 `git grep`：`populate_all_families/synthetic_subject_ids/check_wiki_links/
  build_d3_family/build_trh_d3/populate_noncorelib` **无任何代码/CI/CMake/文档引用**
  （仅 populate_noncorelib 被另一个孤儿引用）。→ `git mv` 归档到
  `docs/archive/dev-completed/scripts-orphaned-2026-08-27/` + README。
- `scripts/codegen/` 现只剩真实生成器：`generate_ir_opcodes.py` `split_fast_dispatch.py`
  （split_fast_dispatch 被 fast_dispatch*.inc 引用 = 真用）。
- build_d3/build_trh 确认非 CMake target。残余风险：generate_ir_opcodes 无引用但属
  合理生成器（生成 IR opcodes），保守保留，待稳定 emission 后复查。
