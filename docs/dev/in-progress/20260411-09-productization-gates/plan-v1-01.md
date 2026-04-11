# Productization Gates 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。

**目标：** 补齐产品化所需的兼容性测试、回滚机制、版本矩阵、iOS 发行策略和长期回归 gate。

**架构：** 以测试框架 + 策略文档 + 自动化 gate 为主。复用现有 subject 测试框架和 test registry 基础设施。

**技术栈：** Python (tests), Markdown (策略), JSON (矩阵配置)

**设计文档：** `phase-architecture-v1-01.md` Phase 8 节

---

## 步骤

### Batch 1: 兼容性测试矩阵

- [x] **1.1** 定义兼容性矩阵配置格式（JSON）：aotVersions × hotUpdateVersions × platforms
- [x] **1.2** 实现兼容性测试 runner：自动化 aot+hot 组合验证
- [x] **1.3** 创建 `subjects/CompatibilityMatrixProof/`：
  - 3 个 AOT 版本 × 3 个热更 patch × 3 个平台
  - 正向兼容：旧 AOT + 新 patch → OK
  - 反向兼容：新 AOT + 旧 patch → OK
  - 不兼容拒绝：incompatible patch → REJECT with error message
- [x] **1.4** 兼容性报告输出：JSON 格式，每个组合的 pass/fail + 错误详情

### Batch 2: 回滚机制

- [x] **2.1** 实现 `HotUpdateManager.Rollback()`：卸载当前 patch → 恢复 AOT-only
- [x] **2.2** 实现 `HotUpdateManager.GetActivePatches()`：返回已加载 patch 清单
- [x] **2.3** 实现 `HotUpdateManager.ValidateIntegrity()`：校验 metadata/bridge 一致性
- [x] **2.4** 验证：apply → verify → rollback → verify AOT-only → re-apply → verify
- [ ] **2.5** 验证：rollback 后所有现有 subject 测试通过

### Batch 3: iOS 发行策略 + 版本管理

- [x] **3.1** 编写 `docs/architecture/ios-distribution-policy.md`：
  - App Store 2.5.2 边界分析
  - 推荐策略：配置/数据热更（安全）vs 逻辑热更（需评估）
  - 技术可行性 vs 发行合规的分离
- [x] **3.2** 编写 `docs/architecture/version-compatibility-matrix.md`：
  - AOT 主包版本编号规则
  - 热更 patch 版本编号规则
  - 兼容性判定规则（major.minor 兼容，major 不兼容）
- [x] **3.3** 实现版本兼容性检查器（集成到 PackageValidator）

### Batch 4: 长期回归 + Soak Test

- [ ] **4.1** 建立 perf regression dashboard 配置：所有 subject × 所有 platform × 关键指标
- [ ] **4.2** 实现 soak test harness：连续运行 24h，监控内存/cpu/crash
- [ ] **4.3** soak test 验收：24h 运行，内存增长 < 1MB，0 crash
- [ ] **4.4** 建立 unsupported feature report：自动检测不支持的 CLR 特性并输出报告
- [x] **4.5** 编写 release checklist（markdown）：每个发布版本必须通过的 gate 清单

### Batch 5: 收尾

- [ ] **5.1** 运行全量兼容性矩阵
- [ ] **5.2** 运行 24h soak test
- [ ] **5.3** 所有 subject × 所有 platform 的 perf baseline 无回归
- [ ] **5.4** release checklist review
