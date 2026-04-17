# verification-before-completion

> 项目中的完成前验证 skill，负责在宣称完成前确认“该跑哪些正式对象、为什么这些验证足够、失败时去哪里定位”。

## 基本信息

- 分层：质量保障
- 项目职责：在标记 `completed` 前选择 formal verification objects，并确认验证和 wiki 更新已完成
- 实现路径：`.codex/skills/dev-verification-before-completion/SKILL.md`
- 状态：`project`

## 默认对象选择顺序

1. 先查 `wiki/06-测试验证/INDEX.md`
2. 再查 registry 快照或 `run test registry list --json`
3. 有 `canonicalCommand` 就直接执行
4. 有 `docRefs` 再回退到 `wiki/06-测试验证/` 对应页面

## Obligation-Driven 规则

命中以下字段时，验证必须升级为 obligation-driven：

- `ownerSubjectId`
- `proofRequired`
- `benchmarkRequired`
- `hotupdateImpact`
- `formalVerificationObjects`
- `requiredGates`

默认要求：

- 先执行 plan 声明的 `formalVerificationObjects`
- 至少确认 `requiredGates`、managed proof、native proof、按需 hotupdate proof / benchmark、以及受影响 regression
- 如果 authority 要求正式对象或 case，但仓库里不存在，先补对象或 case，再允许完成

## 最近变更

- `2026-04-18`：把该 skill 明确为 formal object selector，并接入 AOT obligation-driven completion gate。
