# eh-industrial-completion — STATUS

> **task_id**: eh-industrial-completion
> **task_type**: roadmap
> **phase**: roadmap（brainstorm 完成，已分流）
> **创建日期**: 2026-09-24
> **entry_skill**: dev-brainstorm → dev-roadmap
> **parent_task_id**: —
> **roadmap_or_plan**: `roadmap-v1-01.md`
> **关键文档**: `roadmap-v1-01.md`、本文件
> **上游输入**: EH 完成度评审报告（本会话）+ `docs/dev/completed/eh-industrialization/STATUS.md`

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

---

## 边界拍板

**覆盖**：EH 四层（L1 投递 / L2 codegen 包装 / L3 异常翻译 / L4 类型表）中
**L2/L3/L4 的补齐** + 三模式等价性测试框架 + EH 铁律落地。

**不覆盖**：GC/线程调度本体、调试器 EH 集成、通用平台可移植性（非 EH 部分）。

## authority 决策

- **修复策略选 A**：全平台统一 `CHAOS_EH_*` 宏（用户拍板），
  否决「只解 Windows」与「统一 CPP_THROW」（后者违 P1）。
- **测试策略**：**独立 EH 小用例集**，不编译整 chunk ×3（用户拍板）。
- **平台范围**：全平台含 iOS/Android（用户拍板）；无 CI runner 时以
  **设计验证**替代并在验收口径中诚实标注。
- **测试框架不与 roadmap 实现耦合**：P3 可与 P2 并行。

## 当前结论

EH 子系统**运行时机制（L1）成熟，但 codegen 侧存在跨平台缺口**：
`MethodEmission.cs:474/499` 硬编码 C++ `try/catch` 绕过 `CHAOS_EH_*` 宏，
导致 Windows(WIN32_SEH) / 移动端(SETJMP) 下托管异常在断言执行前逃逸。
实测 9 个 chunk、**~566 处 `preAssertionRaise`** —— 与
`docs/dev/completed/eh-industrialization/STATUS.md` 声称的"100% 完成"不符。

**L3 新发现**：C# `throw` 的异常类型面与 `ResolveTypeByName` 可解析的类型面
（仅 15 个硬编码类型）不匹配 → 抛 null 对象 → catch 静默不匹配。

## 除四层补齐外的新增交付

- **新增 Expert**：`dev-il2cpp-eh-expert`（跨域协调者），已写入
  `.ai/skills/library/skills/dev-il2cpp-eh-expert/SKILL.md` 并注册到
  `expert-registry.json`（expert_keywords + cross_cutting + codegen-ctl）。
- **EH 铁律 11 条**：见 Expert SKILL，覆盖机制统一 / 正确性 / 验证三组，
  用于约束后续 agent 开发。

## 阶段切分

| Phase | 内容 | 依赖 |
|---|---|---|
| P1 | L2 包装层改宏 + C2712 桥接 | — |
| P2 | L3 翻译正确性 + L4 类型表 | P1 |
| P3 | 独立 EH 用例集 + CI 门 | **可与 P2 并行** |
| P4 | 全平台验证 + 铁律落地 | P1/P2/P3 |

## 并行与 worktree 策略

worktree：`wt-eh-p1-wrap` / `wt-eh-p2-translate` / `wt-eh-p3-tests` / `wt-eh-p4-platform`
执行方式：**优先 Workflow 委托**；每个 worktree 合回 main 后**立即清理**（用户要求）。

## 风险评估摘要

| 风险 | 概率 | 影响 | 缓解 |
|---|---|---|---|
| C2712 在桥接 TU 重现 | 中 | 高 | 桥接 TU 零 C++ 对象（参照 `tier_manager_seh.cpp`）|
| 桥接引入性能回归 | 中 | 高 | 只桥 raise 路径；benchmark 对照 |
| iOS/Android 无法实跑 | 高 | 中 | 验收口径诚实标注为"设计验证" |
| 修复被后续改动破坏 | 中 | 高 | **P3 的 CI 门是核心防线** |
| 并发会话冲突 | 中 | 中 | worktree 隔离 + cherry-pick 到最新 main |

## 三优先级权衡结论

- P1 性能：桥接只覆盖 raise 路径（非热路径）；**否决**统一 CPP_THROW（Windows 慢 2×）
- P2 架构：四层统一到宏抽象，消除三平台分叉
- P3 热更：无影响

## 下一步入口

`executing-plans` —— 从 **P1 + P3 并行** 开始（P3 不依赖 P1）。
