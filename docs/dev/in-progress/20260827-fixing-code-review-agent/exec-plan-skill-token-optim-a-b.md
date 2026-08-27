# 最终执行方案 — Skill Token 优化 (A+B)

- task_id: `20260827-fixing-code-review-agent`
- 方案文档: `docs/dev/in-progress/20260827-fixing-code-review-agent/exec-plan-skill-token-optim-a-b.md`
- 配套: `risk-report-skill-token-optim-a-b.md`（同目录风险评估报告）
- 目标: 默认加载 skill 精简且言简意赅, 降低每次任务 context 成本, 零功能损失
- 基线(实测): 46 skill SKILL.md 合计 370.9KB; 默认加载 3 资源 18KB; 每次 il2cpp 任务 ~48KB 起步
- 状态: 方案定稿

> 本方案承接用户决策 A+B（A=默认加载瘦身+入口极简; B=加载链去重）。风险处置建议已并入执行顺序(护栏前置)。

---

## 一、执行顺序（护栏前置，风险先行）

```
0. 护栏前置(防 A1/A2 断链):
     0.1 备份 expert-registry.json + register 生成脚本当前孤儿自检
     0.2 记录 core-agent 7 处 + dev-il2cpp 引用 registry 的所有路径点
     0.3 记录 hook check_classification.py 的强制门(首位 dev-il2cpp + SKILL 存在)
1. A1 拆分 expert-registry.json(11KB→2.6KB):
     - 核保留 version/last_updated/description/domains
     - 拆出 sub_controller+keywords+sub_controller_domain_index+cross_cutting+workflow_templates → expert-registry.routing.json
     - 【关键联动】generate_skill_catalog.py check_expert_orphans() 改读"核+routing 合并"
     - 【关键联动】core-agent SKILL 引 routing 文件位置(7 处引用复查)
2. A2 精简 dev-il2cpp(11.9KB→4KB):
     - 保: 会话缓存/指令优先级/路由协议(写首位)/自动加载资源
     - 下沉 core-agent: 加载确认/核心规则/架构说明
     - 【护栏】保留"写 classified 首位 dev-il2cpp"动作(hook :212 强制)
3. B1/B2 去重(13 处警示块 + 输出格式/关联段):
     - 建 _shared/expert-loading-notice.md, 13 处改单行引用
     - 各 expert 输出格式/关联段归一引用
4. 验证(收尾固定链路):
     4.1 python generate_skill_catalog.py --check  → catalog 一致
     4.2 孤儿自检回归: 模拟孤儿 → 仍触发 [warn] (验证 R1 护栏生效)
     4.3 hook 验证: 写 classified(首位 dev-il2cpp) → 触发一次 Edit/Write → 确认不拦截 (验证 R2)
     4.4 全仓 grep expert-registry → 复核拆分后无断引用
5. 提交: 建分支精确提交本批全部 skill/expert/registry 文件(不含 pre-existing)
```

---

## 二、每步改动明细

### Step 0 护栏
| 动作 | 目的 |
|------|------|
| 备份 registry + 记录孤儿自检现状 | A1 拆分后可对比回滚 |
| 列出 core-agent SKILL / 生成脚本 / dev-fixing-code-review / workflow.js / 3 个 ctl md 的 registry 引用 | A1 联动改引用不漏 |
| 记录 hook 首位 dev-il2cpp 校验可回验 | A2 不断 hook |

### Step 1 A1 拆分 registry
| 文件 | 动作 |
|------|------|
| `expert-registry.json` | 只留 version/last_updated/description/domains |
| `expert-registry.routing.json` | 新建, 承接 sub_controller/keywords/domain_index/cross_cutting/workflow_templates |
| `generate_skill_catalog.py` | `check_expert_orphans()` 读核+routing 合并(防孤儿自检静默失效 → R1) |
| `dev-il2cpp-core-agent/SKILL.md` | 路由引用改双读(:116 workflow_templates → routing 文件) |

验证: 默认 registry context 11KB→2.6KB; core-agent 路由决策仍可达(domains 在核)

### Step 2 A2 精简入口
| 文件 | 动作 |
|------|------|
| `dev-il2cpp/SKILL.md` | 砍到 ~4KB: 只留会话缓存/指令优先级/路由协议/自动加载 |
| `dev-il2cpp-core-agent/SKILL.md` | 承接加载确认 sed 序列+核心规则+红旗+健康检查 |

验证: 入口 11.9KB→~4KB; hook 强制门仍过(首位 dev-il2cpp 保留)

### Step 3 B 去重
| 文件 | 动作 |
|------|------|
| `_shared/expert-loading-notice.md` | 新建共享警示模板 |
| 13 个 expert SKILL.md | 警示块 → 单行引用 |
| 各 expert | 输出格式/关联段 → 归一引用 |

验证: 20 expert 线性减 ~2-4KB/个; 不碰领域内容无行为变化

### Step 4 验证 + Step 5 提交
见 §一 顺序 4.1-4.4 与 5。

---

## 三、可回滚性
- 所有改动为本文件/配置(非代码)。registry 拆分可合并回单文件; dev-il2cpp 下沉可为移位非删除 → 全程可回滚。
- 每步独立 commit(建分支), 分步回滚不强耦合。

---

## 四、验收口径
- 默认 context: registry 11→2.6KB, dev-il2cpp 11.9→4KB (合计 -16KB/任务)
- core-agent 路由: domains(核) + routing(按需) 双读正确, 7 处引用无断
- hook: 空操作 Edit/Write 不被拦截
- 孤儿自检: 模拟孤儿仍触发 [warn](R1 未失效)
- catalog: --check 一致
