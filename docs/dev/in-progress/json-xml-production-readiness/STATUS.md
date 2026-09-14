# JSON / XML Translation Production-Readiness — STATUS

```yaml
task_id: json-xml-production-readiness
task_type: roadmap
phase: P0 (验收基础设施) — 详见 roadmap-v1-01.md
创建日期: 2026-09-13
entry_skill: dev-il2cpp → dev-il2cpp-fact-verification-expert
parent_task_id: null
roadmap_or_plan: roadmap-v1-01.md
关键文档:
  - docs/dev/review/json-xml-translation-production-readiness.md   # 上游评审基线
  - docs/dev/in-progress/json-xml-production-readiness/api-surface-report.md
  - docs/dev/in-progress/json-xml-production-readiness/api-coverage-matrix.json
  - docs/dev/in-progress/reflection-production-readiness/STATUS.md  # 结构范式来源
上游输入: docs/dev/review/json-xml-translation-production-readiness.md (2026-09-13)

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: P0-C-02
```

## 边界拍板

**覆盖的 assembly（3 个，共 1777 个 public method）**

| assembly | public method 总数 | chunk 数 | daily-build 状态 |
|---|---|---|---|
| `System.Text.Json` | 486 | 3 (global-ns / text-json / converters) | 1 PASS / 2 FAIL |
| `System.Private.Xml` | 849 | 4 (xml / schema / xsl / serialization) | PASS / PASS / FAIL / (未跑) |
| `System.Xml.ReaderWriter` | 442 | 2 (system-xml-schema / system-xml-xsl) | PASS / FAIL |

**明确不覆盖（非目标）**

- `System.Text.Encodings.Web`（JSON 依赖但不属于本次两模块边界）
- `System.Xml.Linq`（XDocument / XElement — 属 `System.Private.Xml.Linq`，长线 L 阶段再议）
- XML 序列化引擎（`XmlSerializer` 代码生成路径）— 属 `System.Private.Xml.Serialization`，本 roadmap 不展开

## authority 决策

| 争议点 | 裁决 | authority |
|---|---|---|
| 验收口径以哪份 report 为准 | `docs/dev/review/json-xml-translation-production-readiness.md` 的量纲，但**分母必须由覆盖矩阵重算** | 评审 report §1/§6 |
| 三档判定制 | `real` / `not-supported`（显式 NotSupportedException）/ `not-applicable`；**禁止静默错值** | 反射线已验证范式 |
| probe smoke 能否充当验收 | **不能**。probe 假绿根因见 report §附录 (`TestEmitter.cs:234-260` `if (hasException && isExternalAssembly) return 42L`) | P0-B (#5) 正在修 |
| 覆盖矩阵生成方式 | 优先复用 `Chaos.IL2CPP.Tools.ApiSurfaceScanner`（需扩展命名空间过滤）；退化方案=从各 `_dll/dll-method-manifest.json` 聚合 | 见 api-surface-report.md |
| 阶段推进粒度 | sequential，每完成一个 child 追加「最近摘要」 | 反射线范式 |

## 当前结论

上游评审（2026-09-13）判定的三档覆盖现状：

- **System.Text.Json**：~17 / 486 real（≈3%）。1/3 chunk 通过，`global-ns` 与 `text-json` FAIL。
- **System.Private.Xml**：`xml` chunk 306/489 fact 通过；`xsl` 2 real / FAIL；`schema` 49 real / PASS。
- **System.Xml.ReaderWriter**：`xml` chunk 有 306 real，但 assembly 级 manifest 442 中仅 2 个（`NameTable.Add/Get`）真正被单元测试锁定。

**核心风险**：真实验证覆盖率 <10%，probe smoke 占绝对多数，且 probe 存在系统性假绿（hasException→42L）。P0 三子任务（A 兜底链诚实化 / B probe 假绿修复 / C 验收基础设施）缺一不可。

**本任务（P0-C）交付**：任务目录 + STATUS + roadmap + 覆盖矩阵 + proof 目录。**不产出实现修复**，只产出「可被后续 child 消费的验收脚手架」。

## 验收口径（三档判定制）

| 档位 | 定义 | 证据要求 | 是否计入 real |
|---|---|---|---|
| `real` | 真实调用路径执行且返回值与 .NET 8 参照一致 | 单元测试 / fact 断言附期望值 | ✅ 计入 |
| `not-supported` | 显式抛 `NotSupportedException`（或等价 AOT 明确拒绝） | 测试断言异常类型 | ❌ 不计入 real，但计入「诚实清单」 |
| `not-applicable` | 静态判定为环境不可达（如 `[Serializable]` 序列化回调、`AsyncCallback` Begin/EndInvoke 对） | 分类器输出 + 人工复核 | ❌ 不计入分母 |

**红线**：任何静默返回默认值（0 / null / false / 空串）且无断言的路径，一律判定为 `fake-green`，必须转 `not-supported` 或补真实实现。这条针对 `ExternalRuntimeFallback` Phase 3 `return 0`（见 P0-A #4）。

## 阶段切分

| 阶段 | 目标 | 出口判据 | 依赖 |
|---|---|---|---|
| **P0 验收基础设施** | 目录/STATUS/roadmap/覆盖矩阵/proof 就位 | 本文件 + roadmap + 矩阵 + proof/ 全部落盘，分母可复算 | 无（本任务） |
| **P1 证据基建** | probe 假绿修复 + fact 期望值体系 + 三档判定落库 | 假绿清零；每个 real 有可复现证据 | P0-B (#5)、P0-C |
| **P2 止血（Phase S）** | JSON stub 修复 J1–J10、NameTable 单测 J7、char[] 重载 J8、抽象派发 J9、36 fail J10 | 短清单全部 real 或显式 not-supported | P1 |
| **P3 核心 API（Phase M）** | M1–M8：Utf8JsonReader/JsonDocument/Utf8JsonWriter/XmlDocument/XmlReader/XmlWriter/XDocument/JsonElement | 覆盖率 >60%；daily-build 全绿 | P2 |
| **P4 收口** | 长线 L1–L7 + 性能并入 + 归档 | review report 风险项关闭；chunk 归档 | P3 |

## 风险评估摘要

| 风险 | 等级 | 缓解 |
|---|---|---|
| probe 假绿掩盖真实缺口 | 🔴 高 | P0-B 分类器 + ATG NotSupportedException；P1 gate 假绿清零 |
| `ExternalRuntimeFallback` return 0 静默错值 | 🔴 高 | P0-A 兜底链诚实化，转 not-supported |
| 反射/委托类 subject 可探测但 external 声明不全（C3861 家族） | 🟠 中 | 承袭 reflection 线经验（注册/签名契约修复） |
| 覆盖矩阵工具硬编码 `System.Reflection` | 🟠 中 | 扩展 ApiSurfaceScanner 命名空间过滤，或从 manifest 聚合 |
| chunk 陈旧产物混入分母 | 🟠 中 | 分母以 `_dll/dll-method-manifest.json` 为权威，非 chunk 目录 |

## 三优先级权衡结论

- **P1（性能）**：本任务不引入运行时开销；矩阵生成是离线工具。
- **P2（方案完美性）**：三档判定制优先于「覆盖率数字好看」——宁可 not-supported 显式，不造假 real。
- **P3（HotUpdate）**：暂不涉及。

## 最近摘要

- **2026-09-13**：P0-C 启动。已确认分母权威来源（3 个 `dll-method-manifest.json`：486 / 849 / 442），确认结构范式（reflection-production-readiness）。已落盘 STATUS + roadmap-v1-01 + 覆盖矩阵 + proof/。**未执行任何构建或 fact 运行**（侦察态）。
- **2026-09-13 (P0-C-01)**：覆盖矩阵定稿。分母验证：3 assembly manifest `totalMethods` 与 `methods[]` 长度一致、与矩阵 `publicMethodTotal` 一致、`e2e` 与 `foundation-dll` 树逐字节一致（generatedAt 同）、namespace 分桶和符合计。**决策**：矩阵采用 **manifest 聚合**而非扩展 `ApiSurfaceScanner`（口径差异论证已记入 roadmap）。`api-surface-report.md` 已追加分母复算验证节。

## latest_stop_point

`P0-C-01 覆盖矩阵生成` 完成侦察，待决定：扩展 `ApiSurfaceScanner` 命名空间（代码改动）vs 从 manifest 聚合（纯数据）。矩阵 JSON 已按 manifest 聚合口径产出初版。

## 下一步

1. `P0-C-01` 覆盖矩阵定稿（决定工具化 or 聚合）
2. `P0-C-02` proof 目录规范 + 首批证据样例
3. 与 P0-A (#4) / P0-B (#5) 合并返回，交由 #7 验证并入 main
