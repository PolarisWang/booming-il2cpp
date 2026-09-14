# JSON / XML Translation Production-Readiness — Roadmap v1.01

```yaml
task_id: json-xml-production-readiness
dispatch_model: sequential
上游设计: docs/dev/review/json-xml-translation-production-readiness.md
评审基线: 2026-09-13 daily build + 三 assembly dll-method-manifest
结构范式: docs/dev/in-progress/reflection-production-readiness/roadmap-v1-01.md
```

## 1. 为什么需要 roadmap

上游评审报告给出的是**问题清单**（J/M/L 三档 work items），不是**执行序**。三个 assembly 共 1777 个 public method，实测 real 覆盖 <3%，且验证层存在系统性假绿。若无统一口径与阶段切分，各修复会各自发明「通过」的定义，最终又回到「数字好看、真实不可用」。

本 roadmap 的唯一目的：把「生产就绪」翻译成**可复算的分母 + 可证伪的分子 + 显式的不支持清单**。

## 2. 范围边界

**覆盖**：`System.Text.Json`（486）、`System.Private.Xml`（849）、`System.Xml.ReaderWriter`（442）三 assembly 的 public method 面。

**不覆盖**：`System.Text.Encodings.Web`、`System.Private.Xml.Linq`、XML 序列化引擎（见 STATUS 边界拍板）。

## 3. 非目标

- ❌ 不追求 100% real 覆盖。`not-supported` / `not-applicable` 是合法终态。
- ❌ 不重构 `ExternalRuntimeFallback` 架构（那是 P0-A 的范围）。
- ❌ 不为凑覆盖率给 probe 加无断言 smoke。

## 4. 阶段列表

### Phase 0 — 验收基础设施（本任务 P0-C）

- **Goal**：目录 / STATUS / roadmap / 覆盖矩阵 / proof 就位，分母可复算。
- **Exit Criteria**：`api-coverage-matrix.json` 分母 == Σ 各 `dll-method-manifest.json`；`proof/` 有规范文档与样例；STATUS 三档判定制成文。
- **Deliverables**：本目录全产物。
- **Dependencies**：无。
- **Resolved Decisions**：
  - 分母权威 = `_dll/dll-method-manifest.json`（非 chunk 目录，chunk 可能是陈旧产物）。
  - **矩阵生成方式 = manifest 聚合**，不扩展 `ApiSurfaceScanner`。理由：
    1. 本任务边界是这 3 个 assembly 的 **public method** 面，manifest 已逐方法枚举且是本任务已验证的分母口径；`ApiSurfaceScanner` 面向 `System.Reflection` 命名空间的 **type member** 面（含 property/field/event），口径不同，强行扩展会引入第四套分母定义。
    2. `ApiSurfaceScanner` 从 ref-pack 反射元数据枚举，会纳入本 roadmap 明确不覆盖的类型（`System.Xml.Linq` 等）与 sealed 内部类型，需另一套过滤。
    3. manifest 聚合是纯数据、可离线复算，符合 Phase 0「分母可复算」的出口判据；工具化留待 P1-01（分类器接入）时再评估是否需要 `Chaos.IL2CPP.Tools` 侧收编。
  - 若后续 P1 需要「ref-pack 全量 vs manifest 交集」的差异视图，再单独开子任务评估，不阻塞 P0。
- **Watch Items**（已关闭）：~~`ApiSurfaceScanner` 硬编码 `System.Reflection`，需决定扩展 or 聚合~~ → 见上 Resolved Decisions（选聚合）。

### Phase 1 — 证据基建（依赖 P0-B #5、P0-A #4）

- **Goal**：假绿清零；每个 real 附可复现证据；三档判定落库为机器可读。
- **Exit Criteria**：probe `hasException→42L` 路径不再产出 `[UNVERIFIED]` 假 real；`not-supported` 全部由显式 `NotSupportedException` 承载。
- **Deliverables**：分类器覆盖报告；证据目录规范；判定落库 schema。
- **Dependencies**：P0-B (#5) 分类器 + ATG NotSupportedException；P0-A (#4) 兜底链诚实化。
- **Resolved Decisions**：静默 `return 0` 一律判 fake-green（红线）。
- **Watch Items**：修复后真实覆盖率会先「下降」——这是诚实化的正常代价，不得回退。

### Phase 2 — 止血（Phase S，上游 J1–J10）

- **Goal**：短清单 10 项全部收敛（real 或显式 not-supported）。
- **Exit Criteria**：J1–J10 每项有对应证据文件于 `proof/`；daily-build JSON 相关 chunk 由 FAIL 转 PASS。
- **Deliverables**：按 J 项拆分的修复 + 测试。
- **Dependencies**：Phase 1。
- **Resolved Decisions**：
  - J7 NameTable 单元测试 → 承 P0-A (#4) 已启动的 NameTable 单测。
  - J8 char[] 重载、J9 抽象 `XmlNameTable` 虚派发、J10 36 个 fail → 各自独立子任务。
- **Watch Items**：36 fail 可能含跨域根因（CodeGen + 运行时），触发 Workflow 路由。

### Phase 3 — 核心 API（Phase M，上游 M1–M8）

- **Goal**：8 个核心类型族可用。
- **Exit Criteria**：覆盖率 >60%；daily-build 全绿。
- **Deliverables**：M1 Utf8JsonReader / M2 JsonDocument / M3 Utf8JsonWriter / M4 XmlDocument / M5 XmlReader / M6 XmlWriter / M7 XDocument / M8 JsonElement。
- **Dependencies**：Phase 2。
- **Watch Items**：M1/M3 涉及 SIMD/span 路径，可能与既有 A2 vector operand 缺陷交互。

### Phase 4 — 收口

- **Goal**：长线 L1–L7 + 性能并入 + 归档。
- **Exit Criteria**：上游 review report 全部风险项状态关闭；chunk 归档；roadmap 标 completed。
- **Dependencies**：Phase 3。

## 5. 依赖关系

```
Phase 0 ──► Phase 1 ──► Phase 2 ──► Phase 3 ──► Phase 4
              ▲   ▲
   P0-B(#5) ──┘   │
   P0-A(#4) ──────┘
```

## 6. 子任务映射

### batch 总览

| batch_id | phase | 说明 |
|---|---|---|
| B0 | Phase 0 | 验收基础设施（本任务） |
| B1 | Phase 1 | 证据基建 |
| B2 | Phase 2 | 止血 J1–J10 |
| B3 | Phase 3 | 核心 API M1–M8 |
| B4 | Phase 4 | 收口 |

### 已登记但未执行的发现

| issue | 发现时间 | 描述 | 影响 |
|---|---|---|---|
| AOT/managed 异常行为对齐 | 2026-09-14 | JsonMetadataServices.Create*Info 是 NativeGenerated 路径，不走 catch-all。要让 AOT 抛与 managed 相同的异常类型，需要 ATG metadata → codegen reader → planning → emission 完整跨层链路。当前 catch-all 分支改动已清理。 | global-ns 55 个 stubGap 中 10 个 Set 0 方法已暴露为 realDefect（AOT 不抛异常），剩余 45 个仍为 stubGap。修复需独立立项。 |

### 子任务表

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| P0-C-01 | 0 | completed | workflow | 覆盖矩阵定稿 | — | B0 | 分母==Σmanifest | `api-coverage-matrix.json` | 分母可复算 | docs/ | 0.5d |
| P0-C-02 | 0 | pending | workflow | proof 目录规范 | P0-C-01 | B0 | 证据命名/结构约定 | `proof/README.md` + 样例 | 新证据可按规范落盘 | docs/ | 0.3d |
| P0-C-03 | 0 | pending | workflow | roadmap/STATUS 定稿 | P0-C-01 | B0 | 本文件 | STATUS.md / roadmap-v1-01.md | 评审通过 | docs/ | 0.2d |
| P1-01 | 1 | pending | workflow | 假绿分类器接入矩阵 | P0-B | B1 | 分类器输出可 join 矩阵 | 覆盖报告 v2 | 假绿计数=0 | src/ | 1d |
| P1-02 | 1 | pending | workflow | 三档判定落库 | P1-01 | B1 | schema | verdict 存储 | 每 method 有档位 | docs/+src/ | 1d |
| P2-J01..J10 | 2 | pending | workflow | 止血 10 项 | P1 | B2 | 见上游 §6 | 分项修复+测试 | 各项 real/not-supported | 多域 | 4wk |
| P3-M01..M08 | 3 | pending | workflow | 核心 API 8 族 | P2 | B3 | 见上游 §6 | 分项实现 | 覆盖 >60% | 多域 | 10wk |
| P4-L01..L07 | 4 | pending | workflow | 长线收口 | P3 | B4 | 见上游 §6 | 分项实现 | 风险项关闭 | 多域 | 持续 |
