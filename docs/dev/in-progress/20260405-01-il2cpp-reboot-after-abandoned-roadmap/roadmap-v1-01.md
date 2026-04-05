# IL2CPP-Like C# to C++ Reboot Roadmap v1-01

Date: 2026-04-05 22:40:00 +08:00
Status: active

## 1. 目标

把当前 reboot 主线从“单一超大 plan”升级为“一个轻量 roadmap + 多个子 plan”。

目标不是重新回到旧父级 roadmap 的大而全拆分，而是把当前已经冻结的 proof-first 路线拆成可执行、可审查、可逐步关闭的几个阶段入口：

1. 先冻结 proof-facing contract surface。
2. 再落结构迁移与 tests 语义迁移。
3. 然后进入 `HelloWorld` proof 规格冻结。
4. 再打通 managed minimal closure。
5. 最后落 native bootstrap 与 reference platform proof。

## 2. 范围边界

本 roadmap 只负责：

- 定义阶段边界
- 明确 parent-child 关系
- 标出依赖和启动顺序
- 决定哪些阶段现在创建子任务，哪些阶段先保持 planned

本 roadmap 不直接承载：

- 具体 schema 字段修改
- 具体 header / csproj / tooling 路径改动
- 具体验证命令执行与结果归档

这些工作都应进入派生子任务的 `plan-v1-01.md`。

## 3. 非目标

以下内容不在本 roadmap 中展开：

- 重新恢复旧 `20260329-01-il2cpp-like-cs-to-cpp` 的 7 路母 roadmap
- 在 roadmap 层面直接承载实现级 chunk 细节
- 现在就为 Stage 2 到 Stage 5 全部创建 active 子任务
- 把 post-proof 的 follow-on 路线提前细化成实现计划

## 4. 阶段列表

### Stage 1A：Contract Surface Reset

- 聚焦 `contracts/` 单根语义、artifact schema/example、canonical `subjectId`、`runtime ABI` / `codegen bridge` proof-required subset。

### Stage 1B：Structure And Test Migration

- 聚焦 `Chaos.IL2CPP.*` managed skeleton、`tests/contracts` / `tests/proof` / `tests/fixtures` 结构迁移、tooling 路径切换与 compatibility 收口。

### Stage 2：HelloWorld Proof Spec

- 固定 object-based `HelloWorld` 的输入、输出、IL 形状、metadata / helper 需求与 proof gate。

### Stage 3：Managed Minimal Closure

- 打通 `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 的最小 managed 闭环。

### Stage 4：Native Bootstrap And Reference Proof

- 打通 `runtime-core -> bootstrap -> support`，在 reference platform 跑通 `Hello, World!`

### Stage 5：Post-Proof Split

- proof 成功后，再决定 follow-on sample 和下一轮 roadmap 拆分。

## 5. 每阶段完成定义

### Stage 1A

- `goal`: 冻结 first proof 直接依赖的 contract surface。
- `exit_criteria`:
  - `contracts/` 成为 proof-facing contract 的统一入口语义
  - `typed-il-ir`、`AOT manifest`、`code registration`、`metadata registration` 与 canonical `subjectId` 对齐
  - `runtime ABI` / `codegen bridge` proof-required subset 明确
- `deliverables`:
  - 子任务 `20260405-02-stage-1a-contract-surface-reset`
  - child brainstorm/design/plan 文档
  - contract/doc/example/snapshot 的冻结方案
- `dependencies`: 无
- `open_questions`:
  - `analysis/contracts` 是直接迁移还是短期 compatibility copy
  - direct-call landing 在 header/doc 中的最终落点如何命名

### Stage 1B

- `goal`: 把 Stage 1A 冻结的 contract 方向落到 managed skeleton 与 tests 结构上。
- `exit_criteria`:
  - `src/managed/Chaos.IL2CPP.*` skeleton 建立
  - `tests/contracts` 成为唯一 canonical contract family
  - `tests/proof` 与 `tests/smoke` 的角色分离落地
  - tooling/manifest 中的旧路径切换计划明确
- `deliverables`:
  - 子任务 `20260405-03-stage-1b-structure-and-test-migration`
  - child brainstorm/design/plan 文档
  - managed/tests/tooling 迁移计划
- `dependencies`:
  - `20260405-02-stage-1a-contract-surface-reset`
- `open_questions`:
  - 哪些迁移可以与 1A 并行预铺，哪些必须等待 1A 输出冻结

### Stage 2

- `goal`: 固定 object-based `HelloWorld` proof spec。
- `exit_criteria`:
  - proof 输入输出、最小语义、metadata/helper 需求全部固定
- `deliverables`:
  - 待创建子任务
- `dependencies`:
  - `20260405-02-stage-1a-contract-surface-reset`
  - `20260405-03-stage-1b-structure-and-test-migration`
- `open_questions`:
  - proof sample 是否需要在此阶段同时产出 first artifact snapshots

### Stage 3

- `goal`: 打通 managed minimal closure。
- `exit_criteria`:
  - 最小 managed pipeline 能从程序集走到 generated-code input
- `deliverables`:
  - 待创建子任务
- `dependencies`:
  - Stage 2
- `open_questions`:
  - `Linker` 与 `MetadataWriter` 的最小边界先怎么切

### Stage 4

- `goal`: reference platform proof。
- `exit_criteria`:
  - `HelloWorld` 输出 `Hello, World!`
  - exit code 为 `0`
  - 无 per-method rescue path
- `deliverables`:
  - 待创建子任务
- `dependencies`:
  - Stage 2
  - Stage 3
- `open_questions`:
  - reference platform 最终先锁 Windows 还是 Linux

### Stage 5

- `goal`: post-proof 拆分 follow-on 路线。
- `exit_criteria`:
  - 基于真实 proof 结果给出下一轮 roadmap 或 plan
- `deliverables`:
  - 待创建 follow-up
- `dependencies`:
  - Stage 4
- `open_questions`:
  - follow-on sample 的顺序是否仍保持 `GenericEcho -> ReflectionLite -> PInvokeLite`

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260405-02-stage-1a-contract-surface-reset` | `Stage 1A` | `completed` | `codex` | 冻结 proof-facing contract surface，拆出 contract/doc/example/snapshot 的稳定入口 | `-` |
| `20260405-03-stage-1b-structure-and-test-migration` | `Stage 1B` | `completed` | `codex` | 迁移 managed skeleton、tests family 与 tooling canonical path | `20260405-02-stage-1a-contract-surface-reset` |
| `reboot-stage-2-helloworld-proof-spec` | `Stage 2` | `planned` | `unassigned` | 固定 object-based `HelloWorld` proof spec | `20260405-02-stage-1a-contract-surface-reset, 20260405-03-stage-1b-structure-and-test-migration` |
| `reboot-stage-3-managed-minimal-closure` | `Stage 3` | `planned` | `unassigned` | 打通 managed 最小闭环 | `reboot-stage-2-helloworld-proof-spec` |
| `reboot-stage-4-native-bootstrap-and-reference-proof` | `Stage 4` | `planned` | `unassigned` | 打通 native bootstrap 与 reference platform proof | `reboot-stage-2-helloworld-proof-spec, reboot-stage-3-managed-minimal-closure` |
| `reboot-stage-5-post-proof-split` | `Stage 5` | `planned` | `unassigned` | 基于 proof 结果决定 follow-on roadmap | `reboot-stage-4-native-bootstrap-and-reference-proof` |

说明：

- 当前只正式创建 Stage 1A 和 Stage 1B 子任务目录。
- Stage 2 到 Stage 5 先保留为 roadmap 中的 planned 子任务，不提前激活。

## 7. 依赖

全局依赖关系如下：

1. Stage 1A 先冻结 contract surface。
2. Stage 1B 再落结构迁移和 tests canonical path。
3. Stage 2 基于 1A/1B 的落点固定 proof spec。
4. Stage 3 和 Stage 4 才进入真实闭环。
5. Stage 5 必须等待 Stage 4 的真实 proof 结果。

## 8. 风险

- 如果 roadmap 写得过重，会重新变成旧父级 roadmap 的翻版。
- 如果不拆 roadmap，当前 Stage 1 plan 会继续膨胀，难以控制 review checkpoint。
- 如果 1A 和 1B 完全并行推进，容易在 canonical path 与 compatibility path 上互相踩线。

## 9. 备选路径

### 方案 A：继续单一 plan

- 优点：文档最少。
- 缺点：当前 plan 已经过大，不利于依赖管理。

### 方案 B：现在建立轻量 roadmap，并只创建 2 个子 plan

- 优点：既拆解体量，又不重新走回超大 roadmap。
- 缺点：需要维护 parent-child 状态和索引。

### 方案 C：现在就把 Stage 1 到 Stage 5 全部创建成子任务

- 优点：形式最完整。
- 缺点：会制造一批尚未 ready 的 active-like 目录，管理成本过高。

当前采用：

- **方案 B**

## 10. 当前建议推进顺序

1. 先执行 `20260405-02-stage-1a-contract-surface-reset`
2. 再执行 `20260405-03-stage-1b-structure-and-test-migration`
3. 现在进入 Stage 2 子任务创建与推进
4. 等 Stage 2 关闭后，再决定 Stage 3 / Stage 4 的并行度

