# 项目收尾中央管理中枢 — 设计

> 2026-08-27 · 域：仓库结构/技术债 / 架构
> 需求：所有项目收尾（cleanliness / 漂移守卫 / 磁盘 / 语义锚）**集中一处管理**，
> 不要散在 3 个脚本 + 2 个 hook + 9 份文档里。
> 本文是**架构方案**，供确认后实施。

---

## 一、现状问题（为何要集中）

当前收尾机制**分散**：

| 维度 | 散点 |
|------|------|
| 入口 | `check_repo_clean.py` + `check_generated_up_to_date.py` + `check_repo_clean_hook.py` = **3 个** |
| 配置 | `generated-registry.json`（只覆盖生成物，根/磁盘的阈值硬编码在脚本） |
| 接线 | pre-commit 2 段 + settings.json 1 hook = **3 处** |
| 报告 | 各自 stdout，**无统一报告** |
| 文档 | 同一任务 9 份 `*.md`，真值靠人记 |

**后果**：加一个新守卫 = 新脚本 + 新 pre-commit 段 + 新 hook + 新文档，改一处接线全散。

---

## 二、目标架构：一个"卫生中枢"

### 单一入口 `chaos_hygiene.py`
```
python scripts/cleanliness/chaos_hygiene.py
   --gate        # 提交门：跑所有 hard 检查，任一 fail -> exit 1（pre-commit / CI）
   --soft        # 实时提醒：跑所有，只 warn（Claude hook）
   --report      # 生成 artifacts/hygiene-report.json（统一报告）
   --check <name># 只跑单个检查
   --ci          # = --gate + --report + --fail-fast
   --disk        # 快捷 = --check disk
```
内部**按 hygiene-registry.json 依次调度**每项检查，统一收集 PASS/WARN/FAIL。

### 单一清单 `hygiene-registry.json`（新增守卫 = 加一行）
```jsonc
{
  "version": 1,
  "checks": [
    { "name": "root-clean",       "module": "check_repo_clean",      "mode": "hard", "cmd": ["--hard"] },
    { "name": "disk-health",      "module": "check_repo_clean",      "mode": "soft", "cmd": ["--disk"] },
    { "name": "generated-drift",  "module": "check_generated",       "mode": "warn", "cmd": [] },
    { "name": "oracle-anchor",    "module": "check_behavior_anchor", "mode": "warn", "cmd": [] }   // 未来
  ],
  "output": "artifacts/hygiene-report.json"
}
```

### 统一报告 `artifacts/hygiene-report.json`
```json
{
  "timestamp": "...",
  "overall": "PASS",
  "checks": [
    { "name": "root-clean",   "status": "PASS", "detail": "..." },
    { "name": "disk-health",  "status": "WARN", "detail": ".claude 93MB(known)" },
    { "name": "generated-drift","status":"FAIL","detail":"ir_opcode_names.h 37/112 stale" }
  ],
  "issues": [ "... actionable items ..." ]
}
```
`--report` 落盘 + stdout 打印，CI/日志可消费。

### 单一接线（3 处 -> 1 处）
| 调用方 | 之前 | 之后 |
|--------|------|------|
| pre-commit | 2 段各调一个脚本 | `chaos_hygiene.py --gate`（1 段） |
| Claude hook | `check_repo_clean_hook.py` | `chaos_hygiene.py --soft --check root-clean`（轻量） |
| CI workflow | — | `chaos_hygiene.py --ci` |

### 单一状态页 `STATUS.md`（收尾任务的"公共 dashboard"）
- `chaos_hygiene.py --report` 后同步刷新 `docs/dev/in-progress/repo-cleanliness/STATUS.md`：
  - 当前各检查状态 + 上次运行时间。
  - 已知未决项（如 ir_opcode_names.h 37→112、disk 阈值）。
- 收尾任务真值**从人脑搬到 STATUS.md**，9 份单点文档归档，不再散。

---

## 三、演进：往中枢里加的东西（本次收尾任务闭尾）

| 模块 | check | mode | 治 |
|------|-------|------|----|
| root-clean | `check_repo_clean --hard` | hard | L2 结构 |
| disk-health | `check_repo_clean --disk` | soft | L2 磁盘 |
| generated-drift | `check_generated --fail-fast` | hard(CI)/warn(pre) | L1 |
| **oracle-anchor**（新） | 完成度断言（stub 必须真实现标记） | warn | L3 静默 stub 贷债 |
| **symbol-contract**（新，可选） | extern 引用 → 有定义 | warn | L3 符号假实现 |

**新增守卫 = registry 加一行 + 实现一个 `check_<name>.py`（或复用），不再动接线。**

---

## 四、实施步骤

1. **建 `chaos_hygiene.py`**：读 registry，调度 module 检查，聚合 PASS/WARN/FAIL，支持
   `--gate/--soft/--report/--check/--ci/--disk`。
2. **建 `hygiene-registry.json`**：登记现有 3 项。
3. **`--report` 落盘** `artifacts/hygiene-report.json` + 刷 `STATUS.md`。
4. **接线收敛**：pre-commit 2 段 → 1 段 `--gate`；hook → `--soft --check root-clean`；新增 CI workflow `--ci`。
5. **文档收编**：9 份单点 → 关键结论并入 STATUS.md，过程文档归 `docs/archive/`。
6. **验证**：`--gate` 在干净/脏状态各跑一次，确认聚合 + exit code 正确。

---

## 五、收益 / 边界

**收益**：
- 新增守卫零接线成本（registry 一行 + 一个 check 模块）。
- 报告统一，CI/日志/人三处可读。
- 收尾任务真值集中到 STATUS.md（dashboard），不再靠人脑。
- pre-commit/hook 变薄，触发一致性。

**诚实边界**：
- 中枢是**调度器**，不改变每项检查的能力边界（仍是 ③ + 锚 + 覆盖才治 L3）。
- 迁移初 ~0.5 天；现有 2 脚本逻辑不动，只包一层调度。
- STATUS.md 刷写需小心 CI 并发（写锁/幂等）。

## 六、交付物
- `scripts/cleanliness/chaos_hygiene.py`（调度 + 报告）
- `scripts/cleanliness/hygiene-registry.json`（清单）
- `scripts/cleanliness/STATUS.md`（dashboard，--report 刷新）
- `.githooks/pre-commit` + `.claude/settings.json` + `.github/workflows/`（接线收敛）
- 文档归档 `docs/archive/`
