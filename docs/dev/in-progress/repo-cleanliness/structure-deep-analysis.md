# 仓库目录洁净度 — 深度优化方案分析

> 2026-08-27 · 域：仓库结构/技术债
> 前置：首轮清理 + A+C 监控已落地（见 dev-plan.md）。本文是**结构性根治**分析——
> 不只看"当前哪脏"，而是查"为什么会脏" + 如何从架构上消除产脏源头。

---

## 一、结构化根因诊断（二次深挖证据）

现状 git 层/磁盘层已干净（16 源文件 + 11 目录），但深挖出 **3 个结构性产脏源头**：

### 根因 1：输出路径扩散（output-path sprawl）— 历史遗留
- `.gitignore` 已被 **88 个 commit** 改过，现 **332 条规则 / 582 行**，其中引用 ~30 个互不相同的输出根：
  `output/ output_corelib/ output_test/ output_test_string/ solution/ solution_native/ temp/ tmp/ out/ deploy/ native-build/ profile_data/ profiling_results/ reports/ verification/` 等。
- 这些根**当前磁盘上全部不存在**（现代 CMakePresets 已收敛到 `artifacts/` 单根），即：**大量"死规则"** 只为历史上每次 session 随手建的输出目录留了案底。
- 根级 9 个陈旧 CMake 目录（abi/bootstrap/runtime-core/...）= 这之前 CMAKE_BINARY_DIR 落在 repo 根导致的病灶，现已删。

### 根因 2：工具脚手架分散（tooling scaffold sprawl）— 当前仍存在
同一类运维脚本散在 **3 个根**：
| 根 | 内容 | 问题 |
|----|------|------|
| `scripts/` | `ci/ test/ codegen/ debug/ analysis/ cleanliness/` + 松散 `ci_run.sh ci_test.py asan-native-run.sh clean-build-artifacts.sh setup-hooks.sh` | 主载体 |
| `build/` | `scripts/`(6) + `toolchains/`(117) + `populate_*.py check_arch.py check_wiki_links.py build_d3_family.cmd` | **与 scripts/ 语义重叠** |
| `tools/` | 5 个松散 `.py`（profile_*/gc-baseline/migrate_contract） | 语焉不详 |

且 `build/` 是**混合目录**：117 个 tracked 工具链源 + 生成构建输出共存。`git status build/` 必然混合 source 与 artifacts——这不是当前 bug，但结构上是"生成物与源同居一室"的陷阱。

### 根因 3：生成文件被 check-in（generated-file churn）
- `subjects.metadata.json` 等**已提交的生成文件**每次 pipeline 重跑就改，工作区常驻 ` M`，造成无关 commit 噪音 + 反复脏树。
- 这是"check-in 权威快照 vs check-out 消噪音"的设计权衡，需要显式决策，不是 bug。

### 附：清理工具已有两套（互补不冲突）
- `scripts/clean-build-artifacts.sh`（已跟踪）：**主动清理** translation 树 build 缓存（保守/aggressive）。
- `scripts/cleanliness/check_repo_clean.py`（本次新增）：**被动守卫**，拦根级新生垃圾。
- 二者职责不同，但**命名/落位**有气味（一个在 scripts/ 根、一个在 cleanliness/ 子目录）。

---

## 二、优化方案（从对接到彻底，按侵入度增序）

### 方案 ①：收敛输出根 —— 单根 + 死规则清理（低成本，推荐第一步）
**做法**
- 确立 `artifacts/ + build/` 为仅有两个输出根（build/ 只放 CMake 二进制，artifacts/ 放 pipeline/preset 产物），团队约定。
- 清理 `.gitignore` 中**已死的输出根规则**（output_*/solution*/temp/out/deploy/native-build/profile_* 等 ~30 条）：删规则前先 `git check-ignore -v` 确认当前确实无对应文件在盘。
- 在 pre-commit 守卫中加一条"**根级只允许 N 个白名单根**"——任何不在 `artifacts|build|src|tests|docs|wiki|scripts|tools|contracts|third_party|cmake|schemas|optimization-campaign|results` 的新根，一律阻断。

**✅ 已执行（2026-08-27）**：
- 守卫 `check_repo_clean.py` 已增强——**能捕获新根级目录**（含嵌套文件，此前裸文件检查漏掉整个新目录树）；白名单精简掉已删的历史根级 CMake 构建目录；新增结构治理规约 `structure-governance.md` 作为单输出根约定的权威来源。
- **诚实结论 — 死规则清理大部分放弃**：深挖发现多数"死规则"仍被实际脚本/CI 引用（`SdkEmitter.cs→temp`、`project_workspace.py→solution/solution_native`、`gc_check.py→profile_data`、`ci.yml→reports` 等），非真死；删除有回归风险且收益近零，故不作为独立清理动作。仅 5 个明确无引用根可删但**收益极小（惰性行）**，建议随重构一并处理。

**收益** 消除"随手建输出根→又加 ignore 规则"的恶性循环；守卫从源头拦截新根。
**成本** 低；守卫+规约已落地。
**残余** 真正需要的是团队遵守单根约定，由规约 + 守卫共同保证。

### 方案 ②：脚手架归一 —— 单一 tools 树（中成本，根治分散）
**做法**
- 把 `scripts/` + `build/scripts/` + `build/*.cmd/*.py` + `tools/` 统一到一个根，按职责分桶：
  `tools/ci/`（ci*.sh/py）· `tools/test/` · `tools/codegen/` · `tools/cleanliness/` · `tools/debug/` · `tools/profile/`
- `scripts/` 只保留 1-2 个用户入口（如 `setup-hooks.sh`），其余下沉。
- 保留 `build/toolchains/`（CMake 引用的工具链，需与 CMakePresets 联动）——它是构建配方，与运维脚本性质不同，可不动或移 `tools/toolchains/`。
- 改 CMakeLists 里引用（`add_subdirectory`、`CMAKE_SOURCE_DIR/scripts/...`）与 CI workflow 路径。

**收益** 一树一责；新脚本不用纠结放哪；根从 3 个 tooling 根→1 个。
**成本** 中；触及 CMakeLists + CI 引用 + 文档链接，需全仓 grep 反查。
**风险** 移动 tracked 文件可能触发 pre-commit layer-boundary / check_arch；需验证 CI 不引用旧路径。

### 方案 ③：生成文件出源 —— generated/ 与 churn 治理（中成本，根治脏树噪音）
**做法**
- 对 `subjects.metadata.json` 等**已提交生成文件**做显式决策：
  - 选项 A：移出源树到 `artifacts/` 并 gitignore（消提交噪音，但失去"权威快照随代码走"）。
  - 选项 B：保留在源树 + **codegen 后判定语义等价才不重写**（幂等写，消 churn 不消失文件）。
- 为"生成文件"建 `generated-registry.json`（文件→生成器→是否 canonical），供守卫用它替代人工枚举。

**收益** 工作区不再常驻 ` M`；commit 干净；守卫可自动化判"这文件本来就该被 pipeline 改"。
**成本** 中；需逐文件判语义，动 CI/管线写路径。
**风险** check-in 快照的追溯价值可能减弱（选 A 时）。

### 方案 ④：build/ 脱混合 —— 源/产物彻底分离（中高成本，结构性收尾）
**做法**
- `build/` 仅保留 tracked 源（toolchains/scripts/*.cmd），**生成二进制改走 `artifacts/`**（CMakePresets 已部分如此），`build/` 不再落任何生成物。
- 或反其道：把 tracked 工具链源移出 `build/` → `tools/toolchains/`，让 `build/` 成为一个纯生成目录（整体 gitignore）。

**收益** `build/` 语义单一；根目录"哪是源/哪是产物"一眼分明。
**成本** 中高；CMakeLists/CI 大量路径引用可能需要跟随。
**风险** 高；回归面大，建议作为远期，不与 ①②③ 同批做。

---

## 三、推荐路径（阶梯式，避免一次大爆炸）

| 阶段 | 方案 | 理由 |
|------|------|------|
| **本轮/下轮** | ① 收敛输出根 + 死规则清理 + 守卫白名单根 | 低成本高收益，直接消除产脏主因，可独立验证 |
| **短期** | ② 脚手架归一（scripts/tools/build 合一） | 根治"新脚本放哪"歧义；与 ① 无冲突 |
| **短期** | ③ 生成文件出源/churn 治理 | 消噪音，最贴合"工作目录干净" |
| **远期** | ④ build/ 脱混合 | 重构面大，确认 ①②③ 后再评估 |

> 核心思想：**先治"产脏速度"（①守卫+单根），再治"产脏源头"（②③④）**。已有 A+C 监控兜底可拦住复发，所以可以放心逐步做根治，不必一次到位。

---

## 四、交付物建议

- `scripts/cleanliness/check_repo_clean.py` 增强：加"根级白名单根内校验" + "生成文件 registry 判定"。
- gitignore 死规则清理清单（~30 条，preflight 验证无引用后删）。
- `generated-registry.json` + churn 幂等写。
- 文档：`docs/dev/in-progress/repo-cleanliness/structure-governance.md`（根名称规约 / 输出根 / 脚手架落位）。
