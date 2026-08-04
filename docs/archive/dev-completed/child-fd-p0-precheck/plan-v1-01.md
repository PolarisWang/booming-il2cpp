# Pipeline 预检 — 实现计划

> **面向执行 Agent：** 使用 dev:executing-plans 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。当前子任务完成时必须先归档子任务目录，再同步父任务。

**目标：** 修复已知的 pipeline 阻塞问题，确保 13-stage 管线可稳定运行。

**架构：** 6 项修复（microbench crash、audit 计数、subject IDs ×2、pipeline-config），各自独立，按依赖顺序执行。最后用 1 family 做端到端 13-stage 验证。

**技术栈：** Python（pipeline 脚本）、JSON（contract）、YAML（pipeline-config）、C++（microbench）

**架构审核模式：** normal

**结构告警重点：** 本次是 bug 修复，不涉及结构改动

**权责图审核主题：** n/a

**AOT/IL2CPP/Test Governance Intake：** n/a（不涉及翻译路径变更）

**设计文档：** docs/dev/in-progress/foundation-dll-industrialization/VERIFICATION_CHECKLIST.template.md

**问题清零来源：** parent-roadmap（foundation-dll-industrialization STATUS.md + brainstorm 设计确认）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** wiki/06-测试验证/（修复完成后更新故障诊断文档）

**收尾约束：** 执行完成后必须进入"测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 步骤

### 步骤 1：排查并修复 microbench crash

**目标**：定位 entry.exe --microbench 的 0xC0000005 ACCESS_VIOLATION 根因。

**已知信息**：
- 发生在 stage 7（Microbench）
- exit code 0xC0000005 = ACCESS_VIOLATION
- 影响多个 family

**排查路径**：
1. 找一份 unified-verification-report.json 确认哪个 family 触发该 crash
2. 读对应 family 的 microbench 相关的输出/日志
3. 定位 access violation 的具体位置
4. 修复（如果是 runtime bug → 修 src/native/，如果是 pipeline → 修 Python）
5. 用该 family 重跑 stage 7 确认修复

**验证**：`run pipeline --family {family} --stages microbench` 返回 passed

- [ ] 定位 crash 根因
- [ ] 修复
- [ ] 验证通过

### 步骤 2：修复 audit 计数口径不一致

**目标**：统一 pipeline 与 audit report 的 method 计数。

**已知信息**：
- buffer-memory family 的 audit report 自报 "passed: true"
- 但 pipeline 标 failed
- 差异在于 report 统计 63 methods vs pipeline 统计 87 methods

**排查路径**：
1. 读 `mechanism_audit.py` 的计数逻辑
2. 读 `family_verification_orchestrator.py` 的 audit stage 评估逻辑
3. 对比两者计数方式差异
4. 统一为一种口径（建议以 pipeline 为准）
5. 用 buffer-memory 验证

**验证**：`run pipeline --family buffer-memory --stages audit` 返回 passed，且 pipeline 和 report 计数一致

- [ ] 定位计数差异根因
- [ ] 修复
- [ ] 验证通过

### 步骤 3：补齐 Asn1/encoding-rules subject IDs

**目标**：补全 `testing/foundation-dll/System.Formats.Asn1/encoding-rules/contract.json` 中的 methodSubjectIds。

**已知信息**：该 family 缺 subject IDs，导致无法通过 stage 0 preflight。

**排查路径**：
1. 读该 family 的 contract.json
2. 读同 assembly 其他 family（如 Asn1/reader）的 contract.json 了解格式
3. 确认需要补充哪些 methodSubjectIds
4. 补全

**验证**：`run pipeline --family encoding-rules --stages preflight` 返回 passed

- [ ] 分析需要的 subject IDs
- [ ] 补全 contract.json
- [ ] 验证通过

### 步骤 4：补齐 Text.Json/attributes subject IDs

**目标**：补全 `testing/foundation-dll/System.Text.Json/attributes/contract.json` 中的 methodSubjectIds。

同步骤 3 的流程，针对 Text.Json/attributes。

- [ ] 分析需要的 subject IDs
- [ ] 补全 contract.json
- [ ] 验证通过

### 步骤 5：修复 pipeline-config.yaml 与实际运行脱节

**目标**：确保 pipeline-config.yaml 被 orchestrator 实际消费，或清理不可用配置。

**排查路径**：
1. 读 `pipeline-config.yaml` 的内容
2. 读 `family_verification_orchestrator.py` 看是否引用该配置
3. 如果配置字段（如 migrationSkips=true）未被消费 → 让 orchestrator 消费它，或删除该字段
4. 如果无法消费 → 删除无用配置项

**验证**：运行 orchestrator 不报 config 相关 warning/error

- [ ] 分析配置与实际运行的差距
- [ ] 修复（消费或清理）
- [ ] 验证通过

### 步骤 6：端到端验证

**目标**：选 1 个 family 完整跑通 13-stage，确认所有修复生效。

**推荐 family**：`System.IO.Compression.Brotli/decoder`（Phase 0 已通过过的 family，可作为基线验证）

**流程**：
1. 清理该 family 的旧构建产物
2. 运行全量 13-stage
3. 确认 ALL PASSED
4. 记录 unified-verification-report.json 的关键指标

**验证**：13-stage 全部 passed，overall_status = "passed"

- [ ] 清理旧构建产物
- [ ] 运行全量 13-stage
- [ ] 确认 ALL PASSED
- [ ] 记录 baseline 指标

### 步骤 7：记录输出产物

- [ ] 创建 `pipeline-fix-log.md`，记录每项修复的根因和操作
- [ ] 创建 `KNOWN_ISSUES.md`，记录已知固有限制（如 p1_benchmark 固有 VIOLATION）
- [ ] 创建初始 `verification-tracker.json`

### 步骤 8：子任务收尾

- [ ] 移动子任务目录到 `docs/dev/completed/`
- [ ] 回写父 roadmap 的 child mapping
