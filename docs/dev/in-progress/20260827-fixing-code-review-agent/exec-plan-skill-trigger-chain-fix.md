# 最终实施方案 — Skill 触发链风险修复（R1-R5 + 计划自身 R-A~R-F 加固）

- task_id: `20260827-fixing-code-review-agent`
- 定稿: `docs/dev/in-progress/20260827-fixing-code-review-agent/exec-plan-skill-trigger-chain-fix.md`（v2, 风险加固版）
- 关联: `risk-report-skill-trigger-chain.md`(功能 R1-R5) + 实施计划元评估(计划自身 R-A~R-F)
- 输入清零: 用户确认"修复风险 → 定稿最终实施方案"
- arch_review_mode: normal（.ai/skills 文档 + Python hook + CLAUDE.md 第〇条格式, 不触 AOT/GC/编译）
- 日期: 2026-08-28

> v2 修订：本版在老版 exec-plan 基础上，**并入计划自身风险 R-A~R-F 的加固措施**。核心结构执行顺序不变，但前置加固 + hook 自锁防护 + 格式一次性定死。

---

## 〇、vs v1 修订点（计划自身风险加固）

| 计划自身风险 | 本版加固 |
|-------------|---------|
| R-A hook 自锁(改 hook 崩→会话瘫痪) | item 0 备份+逃生; T1 拆"先定格式/备份 → 再改 hook 即改即验(py_compile)" |
| R-B 复杂度格式未定→改动面蔓延 | item 0 一次性定死格式 + 全入口改动清单(CLAUDE.md+hook+dev-il2cpp+加载确认) |
| R-C --require 硬模式忘写→工具全拦 | 明确默认软(warning) + 逃生线(复杂度缺失仅提示不拦) |
| R-D hook 全局影响+并行线冲突 | 隔离分支; 软默认; hook 改动可独立 revert |
| R-E T5 演练测不出结论 | T5 降为"机制演示"(非严格验证), 标注预期 |
| R-F 可逆 | 分 T commit, 每 T 可独立回滚 |

---

## 一、item 0：前置加固（先做完, 才允许 T1）

1. **备份 hook**：`cp .ai/skills/hooks/check_classification.py /tmp/check_classification.bak`
2. **备份 CLAUDE.md 第〇条**：记录当前格式(echo 命令), 便于回滚
3. **一次性定死复杂度格式**（R-B 关键）:
   - 复杂度字段落点：**`.classified` 末行** 追加 `complexity=<direct|brainstorm|plan|roadmap>`
   - 全部入口统一用此格式：CLAUDE.md:11 分类 echo、dev-il2cpp 加载确认、core-agent 闸门指示
   - **改动清单一并列出**（见 §2 T1 清单）→ 避免改到一半发现四处都要动
4. **逃生线默认值明确**：复杂度缺失/非法 → **仅 stderr warning, 不 sys.exit(1)**（软模式为默认, 永不block会话）

---

## 二、任务清单

### item 0 已含：(备份 + 格式定死 + 逃生默认)

### T1 — hook 复杂度声明（R1 核心, 分两小步防自锁）
- **T1a（先定）**：定义 `complexity=` 字段解析逻辑(读 .classified 末行), 合法值枚举
- **T1b（改 hook + 即改即验）**：
  1. `check_classification.py` 在 loaded_expert 校验后加复杂度读取; 缺失/非法 → stderr warning(默认软)
  2. `--require-complexity` flag: 显式开启时才缺失方 sys.exit(1)
  3. **改后立即** `python -m py_compile check_classification.py` 验证语法
  4. **自测**：手工跑 hook(a) 带复杂度(b) 不带(c) 非法值 → 确认 warning 而非 block
- 同步改入口（R-B 清单一处不漏）:
  - `CLAUDE.md:11` 分类 echo 加 `complexity=<档>` 示例
  - `dev-il2cpp/SKILL.md` 加载确认 sed 序列带 complexity
  - `core-agent/SKILL.md` 阶段1.5 指示输出 complexity 声明
- 验证: catalog --check + hook 三用例

### T2 — 闸门最小探读 + 必升档（R2）
- core-agent 阶段1.5 补写: 判定前可最小探读(ls 目录/看符号表)获复杂度线索
- 明确"无法判断复杂度 → 必须升档 brainstorm(非 direct)"
- 验证: 打样 单点/跨域重构/新功能 3 任务判定举例

### T3 — 闸门唯一入口（R3）
- dev-il2cpp 核心规则1 + core-agent: 明确"复杂度判定唯一走闸门"; 旧 registry 主观兜底仅作闸门失败 fallback
- 删/标注 old "直接实现"宽默认描述
- 验证: grep 无残留宽默认

### T4 — brainstorm 轻量模式显式三要素（R4）
- dev-brainstorm:24 加: 仅当父 design 已提供 范围/接口契约/authority 三要素才可轻量; 否则全流程
- 验证: 静态评审

### T5 — 端到端机制演示（RS, 降级预期）
- 构造 4 代表任务(单点/跨域重构/新功能/回归) 手动演示触发链
- **明确预期**: 演示 agent 是否执行闸门; 若 agent 不主动跑 → 证明 R1 仍需 T1 hook 兜底, 非计划失效
- 产出: 演示记录 + 残留缺口标注

---

## 三、执行顺序 & 验证命令

```
item 0(备份/格式定死/逃生) → T1(hook,即改即验) → T2 → T3 → T4 → T5
每 T 后: catalog --check
T1 后: python -m py_compile + hook 三用例手工
全文: catalog exit0 + hook 三用例 + T5 演示 + hygiene gate
收尾: 结构审视 → 测试 → 归档 completed → 分支 feat/skill-trigger-chain-fix 精确提交(5 文件 + CLAUDE.md)
```

---

## 四、验收口径
- item0: 备份就位 + 复杂度格式唯一(全入口一致) + 逃生默认软
- hook: 缺失 warning 不 block; --require 才硬; py_compile 过
- 闸门: 无法判断必升档 brainstorm
- 唯一入口: 无直接实现宽默认残留
- brainstorm: 轻量仅三要素齐备
- T5: 机制演示记录 + 残留标注
- catalog --check exit0

---

## 五、收尾链路
1. 结构审视: hook 改动可独立 revert; 软默认不拖累并行会话
2. 测试: catalog + hook 三用例 + py_compile + T5 + hygiene
3. 归档 completed + 更新 STATUS/INDEX/ACTIVE
4. 提交: 分支精确 add 6 文件(5 skill/hook + CLAUDE.md), 三段式 commit

---

## 六、R-A 逃生预案（万一改 hook 崩了）
- 若改 hook 后任何工具被拦: `! python C:\...\check_classification.bak` 或 `cp /tmp/check_classification.bak ...` 由用户 shell 恢复
- `!` 前缀命令在本会话执行, 绕过 hook 恢复
