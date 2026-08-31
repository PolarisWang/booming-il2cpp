# 卫生中枢最终审核 — 用户指出脚本污染，审计 + 修复建议

> 2026-08-27 · 域：仓库结构/技术债
> 用户要求：最终审核卫生中枢是否合理，尤其 `scripts/` 里 agent 生成的临时脚本怎么办。
> 本文是**审计结论**：确认了用户的担心有效，量化了问题，给出修复方向。

---

## 一、审计结论：用户的担心**有效且被证实**

`root-clean` 守卫把 `scripts/` 整棵树视为白名单根 → **`scripts/` 内的 agent 一次性临时脚本完全不可见**。实测：

### 已提交的 agent 一次性脚本（应被拦住/不该长期留在源码树）

| 文件 | 性质 | 来源 |
|------|------|------|
| `scripts/_build_wct.cmd` | 一次性编译 helper | 外部 GC 会话提交 |
| `scripts/wct_deadlock_spy.cpp` | **自述"DIAGNOSTIC tool, not part of runtime, no production impact"** | 外部 GC 会话提交 |
| `scripts/wct_deadlock_spy.exe` | spy 的 .exe 构建产物（ignored） | 会话 |
| `scripts/debug/tmp_build_*.cmd`（约 8 个） | **一次性临时 build 脚本** | agent 提交 |
| `scripts/debug/tmp_check_*.py` | 一次性检查脚本 | agent 提交 |

**分析**：这些是 agent 为**单次任务**（调试 WCT 死锁、临时 build）生成的工具，提交后成为永久源码。它们：
- 不该在 `scripts/` 根/`debug/` 长期驻留（应该 gitignore 或放一次性/scratch 区）
- 守卫（root-clean/completion-audit）**都对它们失明**

### 根因（为什么 hub 没抓到）
1. `root-clean` 白名单 `scripts/` 整体 → scripts 内部结构无守卫
2. completion-audit 只扫 `runtime_stubs`，不扫 `scripts/`
3. 没有"这是不是一个 agent 一次性调试脚本 vs 维护工具"的判别层

---

## 二、修复方向（给用户选）

### 方案 A：scripts-scratch 判别守卫（推荐，直接治根）
**做法**：health hub 加一个检查 `check_scripts_hygiene.py`，扫描 `git ls-files scripts/`，**flag 匹配 agent 一次性脚本命名模式**的：
- 前缀 `tmp_`、`_build*`、`_check*`、`_spy*`
- 名字含 `spy`/`deadlock`/`wct`/`dump`/`debug`/`probe`
- `.exe`/`.obj` 构建产物（应 gitignore）
- 文件头自述"diagnostic/not part of runtime/one-off"
→ 报 WARN：这些是一性次脚本，建议移入 `scratch/`（gitignored）或确认后保留为工具。

**收益**：把"agent 临时脚本长住 scripts/"摆到眼前，防未来继续累积。
**成本**：低；纯静态模式匹配。

### 方案 B：一次性脚本落位规约 + 收编
**做法**：
- 规约：agent 一次性/调试脚本一律放 `scripts/debug/`（已有该目录）或 gitignore 的一次性区，**不进 `scripts/` 根**。
- 存量：把 `_build_wct.cmd`/`wct_deadlock_spy.cpp` 移入 `scripts/debug/` 归档，`.exe` gitignore。
**收益**：根治存量；命名即规约。
**成本**：中；移动 + 改引用。

### 方案 C：仅 gitignore（最轻）
**做法**：给 `scripts/debug/tmp_*`、`*.exe`、`*_spy.*` 加 gitignore（但已提交的得 `git rm --cached`）。
**收益**：最快收益；但这些文件已在 git 历史，`git rm --cached` 后需确认无人引用。
**成本**：低但治标。

---

## 三、衡量与推荐

| | A 判别守卫 | B 落位+收编 | C gitignore |
|---|---|---|---|
| 治根因 | 中（防新） | 高（存量+规约） | 低（只藏） |
| 成本 | 低 | 中 | 低 |
| 直接消除污染 | 部分 | ✅ | 部分 |
| 对 agent 工作流友好 | ✅（提示不阻断） | 中 | ✅ |

> **推荐 A + B 组合**：
> - A：加 `check_scripts_hygiene.py`，WARN 拦未来 agent 一次性脚本进 `scripts/` 根（提示不阻断，agent 友好）。
> - B：存量收编——`_build_wct.cmd`/`wct_deadlock_spy.cpp`/`tmp_*` 移 `scripts/debug/` 归档，`.exe` gitignore。
> - C 作为兜底（已提交的 `.exe` 走 git rm --cached）。

---

## 四、诚实边界
- **判别是一次性脚本 vs 维护工具 = 语义判断**，纯命名模式会有少数误报（如 `debug/` 下合法的构建工具）——故用 **WARN 提示**而非硬门，agent 可自行确认。
- 这同样受 GAP-2 影响：多会话并行时，A session 的一次性脚本会被 hub 标 WARN，但那是 A 的合法在途工作。**判别守卫在单会话/收尾时最有效。**
