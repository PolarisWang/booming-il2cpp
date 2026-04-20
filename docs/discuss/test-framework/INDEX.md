# test-framework

## 主题说明

- 这一组文档记录测试框架从“是否要重构”到“正式对象模型”的最早一轮讨论基线。
- 它们更偏总设计输入，适合作为后续 `subject-framework/` 细化文档的上游背景。

## 文档

- `20260406-01-test-framework-reboot-brainstorm-v1-01.md`
  - 第一轮讨论记录。
- `20260406-02-test-framework-reboot-design-draft-v1-01.md`
  - 第一版 design 草案。
- `20260406-03-test-framework-reboot-design-v1-02.md`
  - 更完整的正式设计讨论稿。
- `20260420-01-managed-host-no-per-subject-projects.md`
  - 决策：managed proof / benchmark 已切到共享 `Chaos.TestFramework.Runtime` 宿主，subject 目录下禁止新增 `DeclaredProofHost`、`DeclaredBenchmarkHost`；现存 `Program.cs` / `ProofEntry` 仅允许保留为 native 过渡契约，不再参与 managed 执行。
