# Capability Family Denominator And Authority Freeze Plan v1.01

Date: 2026-04-25 23:00:00 +08:00
Status: in-progress
Parent: `20260425-03-foundation-dll-capability-closure-reporting-roadmap` (Phase 0)

## 1. 目标

把 foundation DLL 报告的 denominator 从隐式的 "verification project 通过数" 冻结为显式的 "capability family closure"，并文档化其 authority 分层。

## 2. 范围

### 负责

- 定义 `capability-family ledger` 的 schema 和权威来源
- 定义 capability family 的主粒度（语义 family）和从属粒度（implementation family / source group）
- 定义 `Denominator Authority` 的层级：原始 authority → 报告 authority
- 提供 3-5 个 family 粒度示例，覆盖简单和复杂案例
- 文档化后经用户 review 确认

### 不负责

- 在本阶段实现报告生成器或 dashboard 代码
- 在本阶段冻结 workflow required gates（那是 Phase 1）
- 在本阶段冻结 DLL completed 规则（那是 Phase 1）
- 在本阶段做 HTML 页面重构（那是 Phase 3）

## 3. 执行步骤

### Step 1: Capability-Family Ledger Schema

- 产出: `capability-family-ledger-schema-v1-01.md`
- 内容:
  - ledger 的核心字段定义
  - family 层级结构（family → sub-family → implementation → source）
  - authority 分层模型（原始 authority → 报告 authority）
  - family ID 命名规范与约束

### Step 2: Denominator Authority Contract

- 产出: `denominator-authority-contract-v1-01.md`
- 内容:
  - 原始 authority 的定义与来源（audit input manifest, surface/semantic ledger, nativeization plan）
  - 报告 authority 的定义（capability-family ledger 的快照）
  - authority 更新的触发条件和流程
  - authority 冲突时的裁决规则

### Step 3: Family Granularity Examples

- 产出: `family-granularity-examples-v1-01.md`
- 内容:
  - 3-5 个实际 family 示例，说明粒度选择理由
  - 每个示例包括: family ID、包含的方法范围、exclusion 规则、对应的 verification gates
  - 负面示例: 过粗和过细粒度的风险说明

## 4. 已拍板决策

- **主进度条 = capability closure**，不是 workflow progress
- **原始 authority ≠ 报告 authority**：原始 authority 来自 audit input / semantic / nativeization artifacts；报告 authority 是 ledger 的快照
- **family 粒度不可过细**：退化为 method count 则失去审核意义
- **family 粒度不可过粗**：掩盖高风险能力空洞
- **implementation family** 和 **source grouping** 是从属的、不独立的映射层

## 5. 设计原则

1. **审核者优先**: 粒度选择以审核者能判断"这个能力族是否完整"为准
2. **authority 可追溯**: 每个 family 的组成和 closure 状态都必须能追溯到原始 artifact
3. **迭代渐进**: 第一版 ledger 不追求完美，追求可用和可审核
4. **自动推导优先**: family 内容尽量由工具从现有 artifact 推导，减少手工维护

## 6. 风险

- 如果 family 映射逻辑过于复杂，工具推导成本高
- 如果 family 数量过多（>50），总报告会不可读
- 如果 authority 分层拍得太细则执行不下去
