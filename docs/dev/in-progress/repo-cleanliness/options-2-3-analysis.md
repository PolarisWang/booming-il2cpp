# 方案 ②③ 选项 + 深度分析（供用户选型）

> 2026-08-27 · 承接 structure-deep-analysis.md。本文针对 ②（脚手架归一）与
> ③（生成文件 churn 治理）给出可选方案与权衡，供用户拍板。
> 证据均为实测（tracked 清单 + 引用反查），非臆测。

---

# 方案 ②：工具脚手架归一

## 现状证据（耦合度 = 迁移成本的关键）

工具脚本散在 **3 个根**，且耦合不均衡：

| 根 | tracked 内容 | 被谁引用（迁移成本） |
|----|--------------|--------------------|
| `scripts/` | `debug/(32) test/(12) ci/(5) codegen/(2) analysis/ cleanliness/` + 松散 `ci_run.sh ci_test.py asan-native-run.sh clean-build-artifacts.sh setup-hooks.sh arm64-deploy-test.sh setup-qemu-arm64.sh` | 少量 CI |
| `build/` | `toolchains/(117)` + 松散 `check_arch.py populate_*.py check_wiki_links.py build_*.cmd` + `managed_bench/` | **`check_arch.py` 被 pre-commit(1) + CMakeLists 4 处 + CI 多文件 硬引用**（CMake 有 `check_arch` custom target） |
| `tools/` | 6 个松散 .py（`gc-baseline.py gc-bench-compare.py profile_analyzer.py profile_data_collector.py migrate_contract_names.py`） | CI 引用（gc/profile） |

> 关键：**`build/check_arch.py` 不是自由移动的** —— 它是 CMake custom target
> 的 COMMAND，被 CMakeLists 4 处 + pre-commit + CI 硬编码。归一时必须同步改
> CMakeLists/CI/钩子。这是方案 ② 的主要成本来源。

## 选项 ②-A：全量归一到 `tools/`（彻底，成本最高）
**做法**：`scripts/ + build/ 松散脚本 + tools/` 全部并入单一 `tools/`，按职责分桶：
`tools/ci/ tools/test/ tools/codegen/ tools/debug/ tools/cleanliness/ tools/profile/
tools/gc/ tools/wiki/`。`build/` 只留 `toolchains/`（构建配方，可移 `tools/toolchains/`）。

- **改**：CMakeLists（4 处 `build/check_arch.py`）、`.githooks/pre-commit`（`build/check_arch.py` 引用）、5 个 CI workflow 路径、`scripts/` 间互相 import 的相对路径。
- **收益**：一树一责；新脚本放哪歧义永久消除；根从 3 tooling 根 → 1。
- **成本**：中高（~4 处 CMake + pre-commit + 5 CI + 37 文件移动）；回归面稍大。
- **风险**：CI 路径漏改会静默失效；pre-commit 引用错会导致 gate 无声跳过。

## 选项 ②-B：仅收敛松散散件（中成本，推荐）
**做法**：只规整"游离单文件"，**不动已成体系的子目录**：
- `build/` 根级 9 个松散 .py/.cmd（`check_arch.py populate_* check_wiki_links.py build_*.cmd`）→ 移入 `scripts/`（或新建 `scripts/codegen/` `scripts/gc/`），**并同步改 CMakeLists pre-commit CI 引用**。
- `tools/` 6 个松散 .py → 移入 `scripts/profile/` `scripts/gc/`。
- 子树（`scripts/debug|test|ci|codegen`、`build/toolchains`）**原地保留**，避免大搬移。

- **收益**：消除"散件该放哪"的最大歧义点；改动面集中在 ~15 个散件 + 其引用；子树稳定。
- **成本**：中；仍要改 CMake/CI/pre-commit 引用，但范围小于 A。
- **风险**：散件移动仍需全仓 grep 反查引用，但波及面受控。

## 选项 ②-C：零移动，仅规约（最低成本）
**做法**：不改路径，只把"落位规约"写进 `structure-governance.md`——新工具脚本一律进
`scripts/<domain>/`，build/ 不收手动脚本；历史散件标记为"已知遗留，不新增"。
- **收益**：零回归；即刻可用。
- **成本**：最低。
- **风险**：现有散件仍在 3 根，歧义未除（只是不再新增）。

## ② 权衡与推荐

| | A 全量归一 | B 收敛散件 | C 仅规约 |
|---|---|---|---|
| 消除现有歧义 | 高 | 中 | 低 |
| 回归风险 | 高 | 中 | 低 |
| 成本 | 中高 | 中 | 最低 |
| 一次性做到位 | ✅ | 部分 | ❌ |

> **推荐 ②-B**：收敛最脏的 ~15 个散件到 `scripts/<domain>/` 并同步引用；
> `build/toolchains` 与已成型子树原地保留。在"消除歧义"与"回归可控"间取平衡，
> 且后续可再升级到 A（不必一次全搬）。

**✅ ②-B 已执行（2026-08-27）**：
- `tools/`(6 .py + gc-baselines) → `scripts/gc/ scripts/profile/ scripts/`（tools/ 已空）
- `build/` 根 6 个散件（populate_* build_*.cmd check_wiki_links.py）→ `scripts/codegen/`（build/ 根只剩 `architecture.yaml + check_arch.py`）
- **例外决策**：`build/check_arch.py + architecture.yaml` **留在 build/** —— 它们是 CMake custom target（CMakeLists 4 处 COMMAND）+ 硬编码 `YAML_PATH=build/architecture.yaml` 的构建工具链，同 `toolchains/` 是真正的 CMake 输入；移动需改 6 处引用 + 硬编码路径，属低价值高险 churn，与 tools/ 散件性质不同。留给远期 ②-A 全量归一时一并处理。
- 校验：guard PASS、迁移 .py 语法全过、unit 2277/2277、无残留旧路径引用（已 sed 修正自引用 docstring）。

---

# 方案 ③：生成文件 churn 治理

## 现状证据
- `tests/e2e/translation/**` 下 tracked `.json` **532 个**，核心是 `subjects.metadata.json`
  + `supplemental-coverage.json` + `CombinedSubjects.runtimeconfig.json`。
- 这些是 pipeline 每次 run 重新生成的**权威快照**，导致工作区常驻 ` M` 噪音 + commit 掺无关 diff。

## 为何是权衡不是 bug
- **check-in** = 用"权威快照随代码走"换"高频 churn + 噪音"。
- **check-out** = 干净工作区，但失去可追踪的黄金快照（若 codegen 有非确定性，回归难复核）。
- 根治须选一头 + 机制。

## 选项 ③-A：生成文件出源到 `artifacts/`（消除 churn，最彻底）
**做法**：`subjects.metadata.json` 等 `??-generated` 从 `tests/e2e/translation/**/managed/`
删出 git 跟踪 → gitignore（`**/subjects.metadata.json`），pipeline 生成到 `artifacts/`。
- **收益**：工作区彻底干净；guard 自然放行。
- **成本**：需改 build.py/管线写路径 + 删 ~532 个 tracked .json（产生大量 git 删除 diff，一次性）。
- **风险**：失去随代码走的权威快照；CI 若强依赖"源码树内有 metadata"需改。
- **适用**：metadata 能由 codegen 确定性再生的场景。

## 选项 ③-B：幂等写 — 消 churn 不消失文件（保留快照，中成本）
**做法**：不改路径；改**写回逻辑**——代码生成前先对目标文件做"语义等价判定"
（如 JSON 规范化后逐 key 对比），等价则不重写（保持 mtime/content 不变），
不等价才写。立即消除"每次 run 都改"的 churn。
- **收益**：干净工作区 + 保留权威快照 + 低风险（不动路径不动 CI）。
- **成本**：中；需在生成器/管线写回点加幂等判定（~1 处核心写回函数）。
- **风险**：若含时间戳/绝对路径等非确定性字段，需先规范化或排除，否则判定不稳。
- **适用**：metadata 内容确定性、仅重建时重写的情况——**当前场景最贴合**。

## 选项 ③-C：registry + 守卫特判（治理透明度，辅助）
**做法**：建 `generated-registry.json` 声明哪些文件是"可再生生成文件"；guard 读取它，
对 registry 内的文件**豁免 churn 提醒**（工作区有改动不警告），同时确保它们不进。
- **收益**：守卫更懂"这本来就是生成文件"；与 A/B 组合。
- **成本**：低；单文件维护。
- **风险**：单靠它不消 churn，需配 A 或 B。

## ③ 权衡与推荐

| | A 出源 | B 幂等写 | C registry |
|---|---|---|---|
| 消除 churn | 高 | 高 | 无 |
| 保留权威快照 | ❌ | ✅ | ✅ |
| 回归风险 | 中（改写路径+删537） | 低 | 最低 |
| 成本 | 中高 | 中 | 低 |

> **推荐 ③-B + ③-C 组合**：B 做根——写回点幂等判定，直接消 churn 且保留快照、
> 不动路径；C 做透明——registry 声明 + guard 豁免，让守卫既不误报也不漏监管。
> 若未来 metadata 需完全脱离源码，再升级到 A（A 是 B 的远期替代）。

---

# 组合执行建议

- **本轮**：③-B（幂等写）+ ③-C（registry + guard 特判）——低风险直接生效，消工作区噪音。
- **下一轮**：②-B（收敛散件 + 同步 CMake/CI/pre-commit 引用）。
- **远期**：②-A（全量归一）+ ③-A（metadata 出源）按需。
