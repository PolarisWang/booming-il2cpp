# Flaky 容忍与重试策略

> 项目级 authority：本文定义「测试结果不稳定（flaky）」的判定、分类、处置与审计规则。
> 与 [`INDEX.md`](./INDEX.md) 既有约束一致 —— **编译崩溃 / 真实失败不得靠重试掩盖**。

---

## 1. 第一原则：重试不是修复手段

本仓库已有硬约束（见 `06-测试验证/INDEX.md`）：

> 测试阶段只要 `dotnet build` / `dotnet test` / `msbuild` 发生编译崩溃，就必须先检查崩溃原因并修复；
> **不能靠重试、跳过或降级为环境噪音继续推进**。

本文不推翻该约束，只在其下补充「**确定性通过但偶发失败**」这一类场景的处置细则。
判据很明确：

| 现象 | 处置 |
|------|------|
| 编译崩溃、链接错误、确定性 FAIL | **禁止重试**。查根因、修复（INDEX.md 既有约束） |
| 同一 commit 同环境重复跑出现 PASS/FAIL 两种结果 | 本文适用——按第 2 节分类 |

**结论：重试只作用于「已确认的非确定性」，且必须先分类、后重试。**

---

## 2. 判定：什么算 flaky

一个测试被认定为 flaky，需同时满足：

1. **同 commit、同环境**重复运行（≥5 次）出现 **PASS 与 FAIL 两种结果**；或
2. **隔离单跑必过、并行（`-j N`）跑必挂**；或
3. 历史统计：该测试失败次数 / 总运行次数 **≥ 1/1000**。

不满足上述任一条的失败，一律按**真实失败**处理。

---

## 3. 分类（决定处置方式，不可跳过）

| 类型 | 特征 | 处置 |
|------|------|------|
| **A 良性时序/资源竞争** | 隔离单跑必过；仅并行/全门禁挂；无崩溃 | 入 `known-failures` 基线（照跑照报）+ 排期修复 |
| **B 内存破坏/崩溃** | SEH / ASan / 随机地址 / 堆破坏 | **禁止入 known-failures**。一律当 blocker，不得靠重试放行 |
| **C 环境依赖** | 仅特定 runner / 基础设施抖动 | 修环境；或加 `runner.os` 条件门禁 |
| **D 测试自身缺陷** | 断言依赖时序/共享全局状态 | 修测试本身（这是测试 bug，不是容忍对象） |

> **B 类是本策略的红线**：内存破坏即使偶发也意味着潜在真 bug。已有先例见
> `tests/runner/baselines/known-failures.integration.yaml` 中 `test_gc_young_collector` 的
> `[HEAP-CORRUPTION WARNING]` 标注 —— 该条目保留只为「一旦确定性命中立即翻红」，绝不作为放行理由。

---

## 4. 处置流程

```
失败发生
   │
   ├─ 编译崩溃 / 确定性 FAIL ──→ 查根因修复（禁止重试）
   │
   └─ 疑似非确定性
        │
        ├─ 用隔离单跑验证（必过？）
        │     ├─ 否 → 真实失败，查根因
        │     └─ 是 → 分类（第 3 节）
        │             ├─ A 类 → known-failures 基线 + 排期
        │             ├─ B 类 → blocker，禁止放行
        │             ├─ C 类 → 修环境 / 条件门禁
        │             └─ D 类 → 修测试
        │
        └─ 更新 flaky 台账（见第 6 节）
```

---

## 5. 重试的严格约束（本项目当前**默认不启用自动重试**）

基于 `INDEX.md` 的既有立场，**本项目默认不引入 CI 自动重试**。若将来确需引入，必须同时满足：

1. **仅对已归档的 A 类 flaky** 生效（白名单驱动，不允许全局重试）；
2. **≤ 3 次**，且重试结果**全部保留在报告中**（不覆盖首次结果）；
3. **B 类永不重试** —— 内存破坏重试只会掩盖问题；
4. 重试后仍 FAIL → 按真实失败处理，不看「某一次过了」；
5. 重试全 PASS → 必须标记 `[FLAKY]` 并**写入 flaky 台账**，不允许静默变绿；
6. 引入前需在 `06-测试验证/INDEX.md` 补充 authority 记录。

> 当前建议：**先不落地自动重试**（与本任务卡的「retry 方案要小心」一致）。
> 先建设台账 + 分类 + 审计能力；重试作为后续可选增强单独评审。

---

## 6. Flaky 台账与 known-failures 基线的分工

| | `known-failures.{layer}.yaml` | `flaky-registry.yaml`（本策略新增） |
|---|---|---|
| 含义 | 已归档的**确定性或高置信**已知失败 | **非确定性**、需持续观察的 flaky |
| 生效方式 | 标 `[KNOWN-FAIL]`，该组仅含这些失败时门禁放行 | 仅记录统计，**不参与放行判定** |
| 存续 | 修复后移除条目 | 按出现率/时效定期回顾 |
| B 类 | **禁止写入** | **禁止写入**（B 类只能当 blocker） |

台账建议格式（machine-readable）：

```yaml
version: 1
entries:
  - test: test_gc_young_collector
    layer: integration
    first_seen: 2026-08-28
    last_seen: 2026-09-10
    occurrences: 3
    runs_total: 2400
    rate: 0.00125
    classification: B          # A / B / C / D
    isolation_passes: true     # 隔离单跑是否必过
    action: blocker            # blocker / scheduled / fixed
    root_cause_ref: docs/dev/in-progress/gc-align-coreclr/notes/...
```

---

## 7. 升级路径（量化阈值）

| 条件 | 动作 |
|------|------|
| 单次出现 | 记录，不处理 |
| 出现率 ≥ 1/1000 | 自动入 flaky 台账 |
| 连续 3 次 CI 出现同一 flaky | 降级为 **blocker**，不允许 merge |
| 出现率 > 1% | **强制排期修复**，不允许长期挂 known-failures |
| 台账条目超 30 天未修复 | 月度回顾必报 |

---

## 8. 审计

- 每次 CI 门禁红 → 比对 flaky 台账，判定「已知 flaky」还是「新问题」。
- `test_driver.py` 产出报告时附上 flaky 命中信息（当前 `tests/runner/test-report.json` 已有 `known` 字段，可扩展）。
- 月度回顾：台账 + known-failures + known-issues.md 三份文件交叉核对。

---

## 9. 当前落地状态

| # | 动作 | 状态 |
|---|------|------|
| 1 | 策略文档（本文） | ✅ 已落地 |
| 2 | flaky 台账 `flaky-registry.yaml` | ⬜ 待建 |
| 3 | CI 自动重试 | ⬜ **暂不启用**（需先建台账 + 单独评审） |
| 4 | 门禁集成（driver 读台账） | ⬜ 待做 |
| 5 | 连续 3 次告警（飞书） | ⬜ 待做 |

---

## 关联文档

- [`INDEX.md`](./INDEX.md) — 测试验证总入口（含「不能靠重试掩盖编译崩溃」既有约束）
- [`测试分层.md`](./测试分层.md) — 分层定义
- [`测试框架能力缺口与改进方向.md`](./测试框架能力缺口与改进方向.md) — 已知能力缺口
- `tests/runner/baselines/known-failures.{layer}.yaml` — 分层 known-failures 基线
- `tests/runner/baselines/known-issues.md` — 基础设施缺口登记
