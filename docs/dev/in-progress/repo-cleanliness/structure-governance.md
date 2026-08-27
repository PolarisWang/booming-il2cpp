# 仓库根结构治理规约（Structure Governance）

> 2026-08-27 · 域：仓库结构/技术债
> 目的：把"工作目录干净"从*靠运气*变成*靠约定 + 机器守卫*。本文是方案① 的
> 权威规约，被 `scripts/cleanliness/check_repo_clean.py` 的白名单引用。

---

## 一、输出根约定（单一事实源）

仓库**有且仅有两个输出根**，除此之外不得新建任何顶层输出目录：

| 输出根 | 用途 | 特性 |
|--------|------|------|
| `artifacts/` | pipeline / CMakePresets / benchmark 产物 | 全 gitignore；可整目录删除重生成 |
| `build/` | CMake 二进制（已 tracked 工具链源除外） | 生成部分 gitignore；`toolchains/` 等 tracked 源保留 |

**禁止**：为一次性任务新建 `output/ debug_output/ test_output/ solution_native/`
等顶层输出目录。若不确定写哪，一律写 `artifacts/` 下。

## 二、根级白名单（`check_repo_clean.py` ROOT_EXCEPTIONS 的权威来源）

根级允许存在的顶层条目（其余一律视为违规）：

```
src/ tests/ docs/ wiki/ contracts/ third_party/ scripts/ tools/ cmake/
schemas/ build/ artifacts/ optimization-campaign/ results/
.ai/ .claude/ .claude_local/          # 工具/技能 运行时状态
```

> 注意：历史根级 CMake 构建目录（abi/ bootstrap/ common/ runtime-core/ support/
> engine-bridge/ fuzz/ hot-update/ codegen/）**已被删除且不在白名单** —— 若被
> 根级 CMake 重建，守卫会立刻拦截。

## 三、根级文件规约

- 根级只允许源码入口文件（`CMakeLists.txt CMakePresets.json README.md CLAUDE.md
  LICENSE CHANGELOG.md SECURITY.md CONTRIBUTING.md STATUS.md dotnet-tools.json
  run*.sh run*.py .clang-format .editorconfig .gitattributes .gitignore`）。
- 其余（`.log .obj .vcxproj CMakeCache.txt *.stackdump …`）由守卫禁区拦截。
- 新文档一律进 `docs/`，新脚本进 `scripts/` 或 `tools/`，不得滞留根级。

## 四、守卫（A+C 双保险）

| 层 | 机制 | 作用 |
|----|------|------|
| A | `.githooks/pre-commit` → `check_repo_clean.py --hard` | commit 前硬拦根级违规 |
| C | `.claude/settings.json` PostToolUse → `check_repo_clean_hook.py` | 产生即时软提醒 + 去重 |

增强要点（2026-08-27）：
- 守卫现能捕获**新建的根级目录**（含嵌套文件），不只根级杂散文件。
- 守卫消费 `generated-registry.json`：可再生 manifest 改动报 informational、删除报错。
- **`--disk` 磁盘健康模式**：扫描 root-direct ignored 输出根 + 已知输出根
  (build/artifacts/.claude/results/optimization-campaign)，有界 walk 报大小；
  非预期超 200MB 根报 ALERT + exit 1。用于补"ignored 产物悄悄填磁盘"的盲区。

## 五、gitignore 治理

- `.gitignore` 已含大量历史输出根规则（被 88 个 commit 累积至 332 条）。
- 复核发现：多数规则**仍被实际脚本/CI 引用**（如 `SdkEmitter.cs→temp`、
  `project_workspace.py→solution/solution_native`、`gc_check.py→profile_data`、
  `ci.yml→reports`、`check_classification.py→output`），**非死规则，勿删**。
- 仅明确无引用的根（output_corelib/output_test_string/native-build/
  profiling_results/verification-history）可清理，但**收益极小（惰性行）**，
  建议保持现状或随重构一并处理，不做单独清理动作。

## 六、生成物放置规约（"生成式脚本不污染项目"）

- **生成式脚本的产物一律落 `*/generated/` 子目录**，不得平铺在手写源码旁
  （`src/native/interpreter/generated/` 是 good 模型；`fast_dispatch_*.inc` 平铺
  在 `interpreter/` 是应避免的反例，远期也应迁入 generated/）。
- 生成器必须支持 **`--out-dir` 输出重定向**，以便漂移守卫在沙箱捕获、不改真源树。
- 生成器不得**原地改提交物**做"漂移检查"——由 `check_generated_up_to_date.py`
  （hermetic，沙箱 + 语义 compare）负责，守卫自身永不写入 repo。
- 新增生成物必须在 `generated-registry.json` 登记（供漂移守卫与 churn 治理识别）。
- 提交的快照基线（`tests/unit/**/Baselines/**/generated/`）是**测试 golden**，
  committed 是必须的，不属污染。

---

## 六、变更记录

- 2026-08-27 建立本文；`check_repo_clean.py` 增强（根级目录捕获 + 白名单精简掉
  已删历史 CMake 构建目录）；根目录清理（首轮）见 `dev-plan.md`。
