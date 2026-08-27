# 仓库工作目录洁净度 — 清理 + 长期监控 开发计划

> 2026-08-27 · 域：仓库结构/技术债
> 目标：一次性清理根目录残留，并建立可持续的"工作目录干净"监控措施，防止复发。

## ✅ 执行状态（2026-08-27 完成）

**清理（已完成，用户确认 scope = A级根级残留 + build/生成目录）：**
- A1 根级 9 个陈旧 CMake 构建目录已删（abi/bootstrap/common/engine-bridge/fuzz/hot-update/runtime-core/support/codegen，均 tracked=0+ignored）
- A2 根级 CMake/OBJ 残留已删（ALL_BUILD/ZERO_CHECK.vcxproj、cmake_install、CMakeCache、unsafe_stubs*.obj）
- A3 根级 ~30 个调试日志/scratch 已删（bgc_*/cdb_*/rc_*/w/prec/fact_err/verif-cov/cls_err/bash.stackdump/jit_allocation_stats/arith_block）
- A4 误建 `NuGet/` 已删；根级 `CMakeFiles/ testing/ nightly-build-report/ .pytest_cache/` 已删
- B1 `build/` 3062MB→2MB，**保留全部 133 个 tracked source**（toolchains/scripts/managed_bench + 8 个根级 .cmd/.py + architecture.yaml）
- D1 docs/assess(4) → `docs/archive/assessments/repo-cleanliness-2026-08-27/`；ACCEPTANCE.md → `docs/archive/contracts-gc-2026-08-27/`
- D3 memory_optimization_plan.md → `git mv docs/archive/optimize/`
- D2 `/codegen/` 误报 —— gitignore `.gitignore:306` 实际已覆盖，无改动

**监控（已完成，用户选择 = 方案 A+C 双保险）：**
- 方案 A：`scripts/cleanliness/check_repo_clean.py --hard` 挂入 `.githooks/pre-commit` 第 6 道 `[repo-clean]` 门。已测：净根 PASS、脏根 FAIL
- 方案 C：`.ai/skills/hooks/check_repo_clean_hook.py` 挂入 `.claude/settings.json` PostToolUse（Edit|Write|Bash|Skill），--soft 实时提醒 + 状态去重。已测：脏根提醒一次、重复静默
- 验证：unit 2277/2277 PASS；pre-commit `sh -n` OK；settings.json 合法 JSON
- 踩坑：`git` subprocess 需 `encoding="utf-8", errors="replace"`，否则 Windows GBK 解码炸（UnicodeDecodeError）

---

## 一、背景（现状盘点结论）

审查根目录 + git 跟踪 + gitignore 后，结论分两类：

- **git 层面已干净**：`.gitignore` 极全面，`git status` 无污染（仅有意 GC 改动 + 3 个未跟踪项）。`build/toolchains/`(117) 等是**有意跟踪的 source**，不能动。
- **物理磁盘层不干净**：根目录堆满被 ignore 的构建产物与调试日志，且历史根级 CMake 输出残留。

| 类别 | 内容 | 处置 |
|------|------|------|
| A1 | 根级陈旧 CMake 构建目录 `abi/ bootstrap/ common/ engine-bridge/ fuzz/ hot-update/ runtime-core/ support/`（tracked=0，仅 .vcxproj/CMakeFiles） | **删除**（现代版在 `build/native-runtime/`） |
| A2 | 根级 CMake/OBJ 残留 `ALL_BUILD.vcxproj ZERO_CHECK.vcxproj cmake_install.cmake CMakeCache.txt unsafe_stubs*.obj` | **删除** |
| A3 | 根级调试日志 ~20 个 `bgc_*.log cdb_*.log rc_*.log w.log prec.log fact_err.log verif-cov.log cls_err.txt bash.exe.stackdump jit_allocation_stats.json arith_block.txt` | **删除** |
| A4 | 误建杂散目录 `NuGet/`（仅空 `Migrations/1`） | **删除** |
| B1 | `build/` 下被 ignore 的生成目录（native/codegen/vs2022/managed*/testing* 等），本次约释放 ~3GB | **清空**（toolchains/scripts/managed_bench 等跟踪 source 保留） |
| D1 | `docs/assess/`（4 个 GC assessment）+ `tests/contracts/native/gc/ACCEPTANCE.md` | **已移入 `docs/archive/assessments/repo-cleanliness-2026-08-27/` + `docs/archive/contracts-gc-2026-08-27/`** |
| D2 | ~~根级 `codegen/` 无 ignore 规则~~ — **已核实为误报**：gitignore `.gitignore:306:/codegen/` 实际命中（此前 `git check-ignore /codegen/` 因 Git Bash 路径折叠 `/`→`C:/Program Files/Git/` 误判），无需补 | 无动作 |
| D3 | `memory_optimization_plan.md`（根级、已跟踪的架构文档） | **已 `git mv` → `docs/archive/optimize/`** |

---

## 二、清理执行步骤（依赖顺序）

> 全部为**删除磁盘文件**操作，前置确认后执行。不涉及 `src/` 工作区（GC 改动未提交，不动）。

### 阶段 0：快照 & 备份（安全网）
- 生成根目录当前文件清单快照到临时文件（`.claude/.repo-cleanup-snapshot-$(date).txt`），万一误删可对照恢复。
- 确认 git status 干净（除既有 GC 改动）。

### 阶段 1：gitignore 补洞（先补规则再删，防复发）
- 在 `.gitignore` 根级 CMake 输出区确认/补上独立 `/codegen/` 规则（当前未命中空目录）。
- 确认 `NuGet/` 无忽略需求 → 直接删目录无需规则。

### 阶段 2：B 类 — 归档未跟踪文档
- `git mv docs/assess/*.md docs/archive/`（4 个 GC assessment）
- `git mv tests/contracts/native/gc/ACCEPTANCE.md` → 归档位置（先确认归档路径规约，参考 docs/dev/hanging 或 docs/archive 结构）
- 检查引用方（已有 investigations.md 引用 docs/assess）→ 同步更新链接。

### 阶段 3：D 类 — 迁移根级架构文档
- `git mv memory_optimization_plan.md docs/archive/discuss/`（如确认归属）。

### 阶段 4：A 类 — 删除根级残留
- A1：`rm -rf` 9 个根级陈旧构建目录。
- A2 + A3：`rm` 根级 12 个 CMake/OBJ + ~20 个调试日志。
- A4：`rm -rf NuGet/`。

### 阶段 5：B 类 — 清理 build/ 生成目录（~3GB）
- 按 gitignore 清单清空 `build/native/ build/codegen/ build/vs2022/ build/managed-build/ build/testing*/ …`（保留 `toolchains/ scripts/ managed_bench/` 及少量被跟踪 source）。

### 阶段 6：落地监控措施（核心交付，见下节）
- 按所选方案接线到 pre-commit / CI / Claude hook。

### 阶段 7：验证
- `git status` 确认根目录只剩真实源文件。
- 跑 `python tests/runner/test_driver.py --layer unit` 确认无回归（清理不改逻辑）。
- 执行监控脚本 dry-run，确认可检出"复发脏根"。

---

## 三、监控措施 — 方案（给用户选择）

所有方案都**复用现有 `.githooks/pre-commit` + CI + Claude hooks 的接缝**，不做一套全新的孤立系统。核心目标一致：**自动检测根目录脏文件/漏 ignore 的构建产物/误新增的 scratch**，在 commit 前或 CI 上阻断。

### 方案 A（推荐）：pre-commit 静态守卫 — `check_repo_clean.py`
- **做法**：新增 `scripts/cleanliness/check_repo_clean.py`，挂进 `.githooks/pre-commit`。
  - 扫描根目录：命中文档维护的"根级黑名单 glob 表"（*\.log, *.obj, *.vcxproj, CMakeCache.txt, bash.*.stackdump …）→ 若有未跟踪新增项即告警。
  - 校验缺失 ignore：对 `git ls-files --others --exclude-standard` 扫描，若根级出现未忽略的新文件且不在白名单 → 阻断（或提示加入 `.gitignore` 或提交）。
  - 校验被跟踪的构建产物不应出现（.dll/.obj/.log 不得新进入 index）。
- **优点**：零成本、本地即时、天然融入现有 5 项 pre-commit 检查链，不需 CI 往返。
- **缺点**：只拦 commit 操作，不拦中途的磁盘堆积。

### 方案 B：CI 门禁 — `dir-hygiene.yml` workflow
- **做法**：新增 GitHub Actions workflow，每次 PR/push 跑 `git ls-files --others --exclude-standard + 黑名单检查`，失败则 job 红。
- **优点**：强制、团队可见、历史回归可追溯。
- **缺点**：只有在 push 后才发现；本地体验有延迟。

### 方案 C：Claude PostToolUse/Stop hook — 会话内实时提醒
- **做法**：新增 `.ai/skills/hooks/check_repo_clean.py`，挂进 `.claude/settings.json` 的 `PostToolUse`（复用现有 hook_track_all 链路），每次工具调用后轻量扫描根目录，检出新增脏文件即在会话内提示。
- **优点**：**实时**，在产生垃圾的那一步就拦住/提示，最贴合"工作目录干净"的诉求。
- **缺点**：需在每次工具调用的 hook 里追加一次 `git status` 调用，有轻微性能开销；需避免误伤（白名单要稳）。

### 方案 D：定期清理脚本 — `scripts/cleanliness/clean_root.sh`（辅助）
- 不单独使用，作为 A/B 的补充命令存根，把 A1-A4 + B1 的删除清单固化成可幂等重跑的脚本，供维护者一键执行。可选纳入 CI 定期/手动 job。

### 推荐组合
**A（pre-commit 静态守卫）+ C（Claude 实时 hook）双保险**，B（CI）作为远景可选。理由：
- 既有 pre-commit 已承担 5 项架构/边界检查，加第 6 项"根洁净度"最顺，零新增基础设施。
- 真正让"工作目录干净"的是 **C**：在产生垃圾的第一时间提醒，而非等 commit。
- A+C 都复用现有接缝；A 拦 commit 逃逸，C 拦产生源头，互补。

---

## 四、交付物清单

| 交付物 | 类型 | 说明 |
|--------|------|------|
| `.githooks/pre-commit` + `scripts/cleanliness/check_repo_clean.py` | 代码 | 方案 A 守卫 |
| `.ai/skills/hooks/check_repo_clean.py` + `settings.json` 接线 | 代码 | 方案 C 实时提醒 |
| `scripts/cleanliness/clean_root.sh` | 脚本 | 方案 D 幂等清理存根 |
| `.gitignore` 补 `/codegen/` | 配置 | D2 |
| `docs/archive/` 归档 | 文档 | D1（docs/assess + ACCEPTANCE） |
