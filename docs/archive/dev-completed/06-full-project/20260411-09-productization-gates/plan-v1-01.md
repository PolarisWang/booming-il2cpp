# Productization Gates 实现计划

> 面向执行 Agent: 必须使用 `dev:executing-plans` 执行本计划。
> 目标: 补齐产品化所需的兼容性测试、rollback、版本矩阵、iOS 发行策略与长期回归 gate。
> 技术栈: Python（tests / tooling）、Markdown（策略 / 评审）、JSON（矩阵配置 / 报告）
> 设计文档: `phase-architecture-v1-01.md` Phase 8 节

## 步骤

### Batch 1: 兼容性测试矩阵

- [x] **1.1** 定义兼容性矩阵配置格式（JSON）: `aotVersions × hotUpdateVersions × platforms`
- [x] **1.2** 实现兼容性测试 runner，自动化执行 AOT + hot update 组合验证
- [x] **1.3** 创建 `subjects/CompatibilityMatrixProof/`
- [x] **1.4** 输出 JSON 报告，包含每个组合的 pass/fail 与错误详情

### Batch 2: rollback 机制

- [x] **2.1** 实现 `HotUpdateManager.Rollback()`
- [x] **2.2** 实现 `HotUpdateManager.GetActivePatches()`
- [x] **2.3** 实现 `HotUpdateManager.ValidateIntegrity()`
- [x] **2.4** 验证 `apply -> verify -> rollback -> verify AOT-only -> re-apply -> verify`
- [x] **2.5** 验证 rollback 后现有 subject 回归通过

### Batch 3: iOS 发行策略 + 版本管理

- [x] **3.1** 编写 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/ios-distribution-policy.md`
- [x] **3.2** 编写 `wiki/03-功能模块/06-il2cpp核心架构/05-架构参考/version-compatibility-matrix.md`
- [x] **3.3** 将版本兼容性检查集成到 `PackageValidator`

### Batch 4: 长期回归 + Soak Test

- [x] **4.1** 建立 perf regression dashboard 配置
- [x] **4.2** 实现 soak test harness
- [x] **4.3** soak test 验收: `4h` 任务按用户指令中止，并以“当前无异常”接受为通过
- [x] **4.4** 建立 unsupported feature report
- [x] **4.5** 编写 release checklist

### Batch 5: 收尾

- [x] **5.1** 运行全量兼容性矩阵
- [x] **5.2** 运行 24h soak test（按用户指令跳过 strict wall-clock `24h`；以 `windows-android-soak` 300 秒通过与 `windows-android-soak-4h` 进入 `runtime-observe` 且未见 fail event 作为当前阶段 accepted evidence）
- [x] **5.3** 所有 `subject × platform` 的 perf baseline 无回归
- [x] **5.4** release checklist review（按用户指令接受 `24h soak` 与 `macOS/iOS runtime host evidence` waiver）
