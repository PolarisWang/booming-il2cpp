# Subject Test Framework v1

## 文档

- `foundation-and-windows-cutover-v1.md`
  - 固定 Stage A+B+C 已经收口的长期边界：`subjects/HelloWorldObject` 真源、artifact bucket 命名、最小 planner/executor/worker contract，以及 Windows `verify-roadmap-0` compatibility gate 退化规则。
- `reporting-and-public-cutover-v1.md`
  - 固定 Stage D+E 已经收口的长期边界：`matrix -> subject -> session` reporting、canonical matrix events、`subject` registry/public command/TUI cutover，以及 `subjectResults` 不回灌 `suiteResults` 的约束。
- `perf-and-multi-subject-expansion-v1.md`
  - 固定 Stage F 已经收口的长期边界：`subjects/*/source` canonical smoke source、subject-aware `perf.dev` / `perf.release` baseline/report 路径、以及旧 `tests/smoke/input/*` 仅作为长期基线保留的规则。
- `../foundation-dll-capability-verification-closure-v1.md`
  - 固定 foundation-dll capability verification closure 的四层 authority、A2 Source-Annotation-First、单一正式 `.sln` 主入口与 family 局部加速入口 contract。
