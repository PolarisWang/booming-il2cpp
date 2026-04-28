# Denominator Authority Contract v1.01

Date: 2026-04-25 23:00:00 +08:00
Status: draft
Phase: phase-0 (20260425-04)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 foundation DLL capability closure 报告的**分母 authority**——即分母来自哪里、由谁维护、何时更新、如何裁决冲突。

本 contract 的核心结论：
- 报告分母 = `capability-family ledger` 中每个 DLL 的 `families[]` 总数
- 原始数据来源不是 ledger 本身，而是 4 个下层 artifact
- 报告 authority ≠ 原始 authority，两者通过冻结流程连接

## 2. 分母定义

### 2.1 分母=capability family 数，不是 method 数

```
DLL Capability Closure = closedFamilyCount / totalFamilyCount
```

其中：
- `totalFamilyCount` = 该 DLL 在 capability-family ledger 中的 semantic family 总数
- `closedFamilyCount` = 满足 §7 闭包条件的 semantic family 数

### 2.2 不使用以下分母

| 分母候选 | 不使用的原因 |
|----------|-------------|
| verification project 通过数 | 反映流程推进，不反映能力覆盖 |
| method count / coverage count | 容易被 overload / 平台方法 / 噪音方法数误导 |
| test case 通过数 | 不直接对应 native 翻译完整性 |
| workflow gate 完成数 | 同 verification project 通过数 |

## 3. 原始 authority

### 3.1 定义

原始 authority 是 foundation DLL capability family 的最初数据来源。它不是单一文件，而是 4 个 artifact 的组合。

| # | 原始 authority | 文件路径 | 提供什么 | 维护者 |
|---|---------------|----------|----------|--------|
| 1 | Audit Input Manifest | `audit/<assembly>/input-manifest.json` | DLL 的完整方法清单（含 metadata token、signature） | 工具自动生成 |
| 2 | Surface Ledger | `audit/<assembly>/surface-ledger.json` | 方法的 surface 分类（public/internal/private/runtime-backed）和语义标签 | 审核者+工具 |
| 3 | Semantic Ledger | `audit/<assembly>/semantic-ledger.json` | 方法的语义分类（属于哪个 capability family） | 审核者 |
| 4 | Nativeization Plan | `audit/<assembly>/nativeization-plan.json` | 方法到 native codegen 策略的映射 | codegen 工程师+工具 |

### 3.2 原始 authority 之间的依赖关系

```
input-manifest.json (方法清单底盘)
       ↓
surface-ledger.json (在方法清单基础上标注 surface 分类)
       ↓
semantic-ledger.json (在 surface 分类基础上标注语义族)
       ↓
nativeization-plan.json (在语义分类基础上标注 codegen 策略)
```

前置 authority 变更时，后续 authority 必须重新推导或人工确认。

### 3.3 原始 authority 的使用场景

原始 authority 用于：
1. **推导** capability-family ledger 中的 family scope（哪些方法属于哪个 family）
2. **验证** ledger 中 family scope 的准确性（是否遗漏方法、是否多包含方法）
3. **追溯** closure 证据（ledger 中标记 closed 的方法是否确实有 native codegen）

原始 authority **不直接**用于：
- 报告生成（报告必须使用 ledger 快照，而非实时读取原始 authority）

## 4. 报告 authority

### 4.1 定义

报告 authority 是 capability-family ledger 在某个时间点的**冻结快照**。报告生成器以 ledger 的数据为准，不实时查询原始 authority。

### 4.2 报告 authority 的构成

报告 authority = capability-family ledger 的全部字段（见 schema 文档），加上：

```
{
  "authoritySnapshotOf": "2026-04-25T22:00:00+08:00",
  "originalAuthorityDigest": {
    "auditInputManifestVersion": "sha256:<hash>",
    "surfaceLedgerVersion": "sha256:<hash>",
    "semanticLedgerVersion": "sha256:<hash>",
    "nativeizationPlanVersion": "sha256:<hash>"
  },
  "freezePolicy": {
    "freezeTrigger": "scheduled | manual | dependency-change",
    "freezeApprover": "codegen-review | audit-lead",
    "minInterval": "P7D"  // 两次冻结之间最小间隔 7 天
  }
}
```

### 4.3 报告 authority 的不可变区间

在一个冻结快照的生命周期内：
- `families[]` 的数量和组成不可变
- 每个 family 的 `scope.methodCount` 不可变
- 每个 family 的 `closureStatus` 可更新（但不改变分母总数）
- 报告 authority 的时间戳 `authoritySnapshotOf` 不变

## 5. Authority 更新流程

### 5.1 触发条件

以下任一条件可触发 authority 更新：

| 触发条件 | 紧急程度 | 说明 |
|----------|----------|------|
| DLL 版本变更 | 高 | DLL 新增/删除了 public API |
| semantic ledger 语义分类变更 | 中 | 审核者重新分类了某个方法到不同 family |
| nativeization plan 策略变更 | 中 | 新的 codegen emitter 改变了方法的实现策略 |
| 审核发现 family scope 错误 | 高 | 当前 ledger 中的 family 划分有遗漏或多包含 |
| 定期刷新 | 低 | 每 7 天自动检查原始 authority 是否有变化 |

### 5.2 更新流程

```
1. 检测触发条件 →
2. 重新推导或人工调整 raw authority →
3. 更新原始 authority 的版本记录 →
4. 重新生成 capability-family ledger 的新快照 →
5. 执行 ledger 完整性校验（§9） →
6. 审核者审批快照 →
7. 冻结新快照，更新 authoritySnapshotOf →
8. 记录变更日志
```

### 5.3 紧急更新

当审核发现当前 authority 导致误报（如遗漏了关键 family 导致已 closed 的 DLL 实际不完整）时：
1. 立即标记对应 DLL 为 `blocked`，不允许进入 completion
2. 在 24 小时内完成 authority 更新流程
3. 更新后重新校验受影响的 closure 状态

## 6. Authority 冲突裁决

### 6.1 冲突类型

| 冲突类型 | 场景 | 裁决方法 |
|----------|------|----------|
| 方法归属冲突 | 一个方法同时属于两个 semantic family | 以 semantic ledger 的最新分类为准；仍争议时由审核者指定 |
| scope 边界冲突 | 原始 authority 显示某方法存在，ledger 未包含 | ledger 必须补回，除非有正式的 exclusion 记录 |
| closure 状态冲突 | coverage JSON 显示 covered，但 native proof 失败 | 以 native proof 结果为准（covered 但 proof failed = 仍未闭包） |
| 版本冲突 | 原始 authority 已更新但 ledger 未刷新 | 以 `authoritySnapshotOf` 为准；告警提示存在更新 |

### 6.2 裁决规则优先级

```
1. 审核者的人工判定（最高优先级）
2. 运行中的 native proof 结果
3. coverage JSON 的覆盖数据
4. semantic ledger 的分类信息
5. surface ledger 的 surface 信息
6. input manifest 的方法清单（最低优先级，仅作为底盘）
```

## 7. Authority 生命周期

```
原始 authority (持续更新)
  │
  ├── 触发条件满足 ──→ 重新推导 ──→ 生成新快照
  │                              │
  │                       ┌──────┴──────┐
  │                       │ 审核者审批   │
  │                       └──────┬──────┘
  │                              │
  │                    ┌─────────┴─────────┐
  │                    │                   │
  │              审批通过              审批驳回
  │                    │                   │
  │              冻结新快照            记录驳回原因
  │                    │             返回修改
  │              报告 authority
  │              (不可变区间开始)
  │                    │
  │              报告生成器消费
  │                    │
  │              触发条件再次满足 ──→ 进入下一轮
```

## 8. 报告 authority 的校验

每次冻结新快照后，必须执行以下校验：

| 校验项 | 方法 | 预期 |
|--------|------|------|
| DLL 完整性 | ledger 中的 DLL 列表与 program.json 一致 | 无遗漏 |
| Family 完整性 | 每个 DLL 至少有一个 semantic family | 通过 |
| Scope 互斥性 | 同一 DLL 内不同 family 的 method 集合无重叠 | 无重叠 |
| Method 覆盖 | 每个 family 的方法集合是原始 authority 的子集 | 通过 |
| Closure 证据 | closed 状态的 family 有 evidence 路径 | 所有 closed family 有证据 |
| Authority 可追溯 | 每个 family 的 authorityRef 可解析 | 所有 ref 可解析 |

## 9. 与原始 authority artifact 的映射实现

### 9.1 Ledger 与 Audit Input Manifest 的对应

```
input-manifest.json.members[]  →  ledger DLL 条目可引用的完整方法集合
                                  用于验证 family scope 没有遗漏
```

### 9.2 Ledger 与 Surface Ledger 的对应

```
surface-ledger.json 中标记为 public+ 的方法 → 必须属于某个 semantic family
surface-ledger.json 中标记为 internal/private → 可属于 runtime helper family 或 excluded
```

### 9.3 Ledger 与 Semantic Ledger 的对应

```
semantic-ledger.json 的 semanticFamily 标签 → ledger 的 familyId
同一个 semanticFamily 标签的所有方法 → 组成该 family 的 scope
```

### 9.4 Ledger 与 Nativeization Plan 的对应

```
nativeization-plan.json 的 codegenStrategy → implementation family 的 handler 映射
同一 codegenStrategy 的方法 → 组成一个 implementation family
```

## 10. 风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| 原始 authority 频繁变更 | ledger 快照频繁刷新，失去 Authority 意义 | `minInterval: P7D` 限制最短冻结间隔 |
| 审核者审批成瓶颈 | 快照无法及时冻结 | 紧急流程设置 24 小时 SLA |
| 原始 authority 版本记录丢失 | 无法追溯 authority 来源 | `originalAuthorityDigest` 记录 hash |
| family scope 手工维护过多 | 工具推导 coverage 低 | 优先自动推导，人工仅做审批和例外裁决 |

## 11. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-25 | 1.0.0 | 初始版本 | Phase 0 初始创建 |
