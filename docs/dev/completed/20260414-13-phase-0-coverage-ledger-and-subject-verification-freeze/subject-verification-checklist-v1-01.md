# Subject Verification Checklist v1.01

Date: 2026-04-14 12:52:36 +08:00
Status: frozen

## 1. 目的

这份文档冻结“subject 工程有完整验证能力”的统一验收标准。后续任何 retained subject 扩展、capability batch 或 archetype gate，都必须至少满足这份 checklist。

## 2. Subject 级 Checklist

- [ ] 顶层 owner 明确，只属于一个 retained subject。
- [ ] source 仍保持 solution 模式组织，不为单个 proof 新增顶层 subject。
- [ ] 统一入口可以发现该 subject 的 proof 与 benchmark。
- [ ] `subject.manifest.json`、compiled catalog、registry 与 dashboard 能指向同一 subject 身份。
- [ ] 不引入新的长期 string 调度协议。

## 3. Capability 级 Checklist

- [ ] 已映射到 frozen `capability_item`。
- [ ] 已确定 `owner_subject`。
- [ ] 已确定 `owner_archetype`。
- [ ] 已确定 `support_state_anchor`。
- [ ] 已确定是否需要 benchmark，以及 benchmark 属于 `required / selective / none` 哪种策略。

## 4. Proof 级 Checklist

- [ ] proof 使用 `ChaosUnitTest` + `Assert`。
- [ ] proof 能经由统一 discovery / compiled catalog 被发现。
- [ ] proof 失败时有稳定的条目标识，不依赖日志文本猜测。
- [ ] 若能力影响 solution ingest / bridge / patch compatibility，至少有一个工程级 proof 与之绑定。

## 5. Benchmark 级 Checklist

- [ ] benchmark 使用 `ChaosBenchmark` 声明。
- [ ] benchmark identity 稳定绑定到 `subject + archetype/capability + case`。
- [ ] dashboard 默认以 managed time 为基线。
- [ ] benchmark 结果缺失时能给出 reason，而不是静默空白。
- [ ] benchmark 阈值不写在 subject source 内部。

## 6. Archetype 级 Checklist

- [ ] archetype 表达的是完整 solution graph，而不是单个 proof 细节。
- [ ] archetype 的依赖图可复现，不依赖宿主机绝对路径。
- [ ] archetype 对应的 project/reference/package/corelib/hot-update 边界明确。
- [ ] archetype 至少能形成一条完整的 `ingest -> translate -> run/report` 链路。

## 7. Support-State 级 Checklist

- [ ] capability 当前是 `supported / partial / unsupported / not-applicable` 中的哪一种，外层系统可表达。
- [ ] 若是 `partial` 或 `unsupported`，有明确 reason code。
- [ ] support-state 来源位于 outer matrix / reporting / docs，而不是 attribute。
- [ ] subject / archetype / capability 三个层级的显示口径一致。

## 8. 诊断与产物 Checklist

最低要求：

- [ ] proof result
- [ ] benchmark result（若适用）
- [ ] support-state / reason code
- [ ] solution/archetype 绑定关系

按需增强：

- [ ] metadata supplement 摘要
- [ ] trace / symbol / rollback 证据

冻结判断：

- baseline 闭环只强制最低要求；
- trace/symbol 等诊断件默认不是 capability 完成前置，而是 Phase 1+ 的增强项。

## 9. 文档 Checklist

- [ ] architecture docs 能说明该能力由谁承接。
- [ ] usage / release docs 能说明如何运行和读取证据。
- [ ] dashboard / overview / reporting 的命名与 docs 一致。

## 10. 最终判定

只有当 subject、capability、proof、benchmark、archetype、support-state、产物链路、文档八个维度同时满足时，才允许声称：

> 该能力已经被 retained subject 完整验证。
