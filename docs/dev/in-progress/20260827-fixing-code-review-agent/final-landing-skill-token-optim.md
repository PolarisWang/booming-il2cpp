# 最终落地方案 — Skill Token 优化 A+B（已完成 + 剩余）

- task_id: `20260827-fixing-code-review-agent`
- 文档: `docs/dev/in-progress/20260827-fixing-code-review-agent/final-landing-skill-token-optim.md`
- 分支: `feat/skill-token-optim-a-b`（已提交 30 文件, +1050/-201）
- 日期: 2026-08-27
- 状态: A1✅ B✅ 实证; A2 保守修正; R6 提交已隔离(部分)

---

## 一、已落地（实测验证）

### A1 — 拆分 expert-registry.json ✅
| 项 | 结果 |
|----|------|
| 核 registry(.json) | 只留 version/last_updated/description/**domains** → **3.2KB**(原 11KB), 默认加载 -8KB |
| routing(.routing.json) | 承接 sub_controller/keywords/domain_index/cross_cutting/workflow_templates → 7.8KB, 按需加载 |
| generate_skill_catalog.py | `check_expert_orphans()` 改合并读核+routing(3 处), **孤儿自检不失效** |
| R1 回归 | 模拟全新孤儿 expert → 触发 [warn]; 还原干净; catalog --check exit0 |

### A2 — dev-il2cpp 保守修正 ✅(实勘修正)
- **修正**: 重复编号 `### 6 → ### 7`。
- **实勘修正**: 通读发现 `## 核心规则`(124-222) 是**入口 skill 的行为规约**(选技能/推进/红旗), 非冗余。
  激进砍到 4KB 会误伤行为完整性, 收益边际(实测可安全删空间~1-2KB 远低于方案估的 8KB)。
  → **不激进精简, 只修质量缺陷**。诚实偏离原方案"A2=>4KB"承诺。

### B — 13 处警示块去重 + _shared ✅
- 新建 `_shared/expert-loading-notice.md` 共享模板。
- 13 个 expert SKILL 警示块 → 单行引用(9 多行块 + 4 两行块, 逐文件验证)。
- `generate_skill_catalog.py` 遍历**跳过 `_shared`**(下划线前缀=共享资源非 skill)。
- catalog --check exit0。

### R2/hook 安全 ✅
- 本批**不改** dev-il2cpp 前端(路由协议/加载确认写首位) + 不改 settings.json/hooks。
- dev-il2cpp 仅 1 行(编号), hook 强制门(首位 dev-il2cpp + SKILL 存在)天然不受影响。
- 实测 git diff dev-il2cpp = +1/-1。

## 二、风险处置结果

| 风险 | 处置 |
|------|------|
| R1 孤儿自检失效 | ✅ 生成脚本改合并读 + 模拟回归验证 |
| R2 hook 断裂 | ✅ 本批不动 hook 段; dev-il2cpp 仅编号修正 |
| R3 拆分引用漏改 | ✅ 生成脚本 3 处合并读; core-agent 文档引用为描述性(核仍名 expert-registry.json, 不断); routing 仅孤儿自检消费 |
| R4/R5 | ✅ 低, 已验证无影响 |
| R6 提交污染 | ✅ **部分缓解**: 30 文件隔离提交到分支(纯 .ai/skills + docs, 无 src/混合) |
| R7 并行线冲突 | ⚠️ 剩余: R2R3 GC 代码留工作区未提交(刻意, 避并行线) |

## 三、剩余未决（需用户决策）

### ⚠️ R2R3 GC 代码未提交
`gc_parallel_mark.cpp`(+2/-2) + `gc_worker_pool.cpp`(+3) 属本任务(自测 13/13 绿), 但**刻意排除在本次 skill commit 外**(避免与并行 gc 线冲突, R7)。
**选项**:
- (a) 并入本分支二次提交(它们是隔离小 diff, 测试已绿)
- (b) 留给并行 gc 线合并
- (c) 后续用户单独决定

### 剩余 pre-existing(绝不碰)
fact-266 codegen / gc_old_gen / parse_convert / math_stubs / crypto metadata 是**并行工作线**, 保持 main 工作区。

## 四、验收口径（已达成）

- 默认 context: registry 11→3.2KB（-8KB）; dev-il2cpp 保持行为完整
- core-agent 路由: domains(核) 完整可达
- hook: 未动, 空操作 Edit 不拦截(本批无 hook 改动)
- 孤儿自检: 拆分后仍有效(模拟验证)
- catalog: --check exit0
- 提交: 30 文件隔离分支, 无 src/ 污染

## 五、建议下一步
1. 决定 R2R3 GC 代码去向(并入/留分岔)。
2. 生成 PR 提交分支(仅影响 .ai/skills + docs, 与并行线解耦)。
3. 如后续仍要 A2 激进精简 → 独立专项(需先确认核心规则可下沉的边界, 不破坏行为规约)。
