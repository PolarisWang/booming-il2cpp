# IL2CPP-Like C# to C++ Reboot Roadmap v1-01

Date: 2026-04-05 22:40:00 +08:00
Status: completed

## 1. 目标

把 reboot 主线从“单一超大 plan”升级为“一个轻量 roadmap + 多个可归档子任务”，并沿着 proof-first 路线完成一条真实可执行的重建链路。

目标不是复制旧父 roadmap 的大而全体量，而是把 proof-facing contract、`HelloWorldObject` spec、managed closure、native proof 与 post-proof follow-on 顺序全部冻结成可追踪资产。

## 2. 范围边界

本 roadmap 负责：

- 定义 Stage 1A / 1B / 2 / 3 / 4 / 5 的阶段边界。
- 维护 parent-child 关系、依赖顺序与收口时点。
- 让重建后的 proof-first 主线能够被逐步执行并最终归档。

本 roadmap 不直接承载：

- 每个子任务内部的实现级 chunk 细节。
- 具体 schema / source / toolchain 改动的逐项执行。
- 后续 `macOS` parity 的真实宿主实现。

## 3. 非目标

- 不恢复旧 `20260329-01-il2cpp-like-cs-to-cpp` 的 7 路母 roadmap。
- 不在父 roadmap 层承载实现级 checklists。
- 不要求从第一天同时展开所有样例与所有平台。

## 4. 阶段列表

### Stage 1A：Contract Surface Reset

- 冻结 `contracts/` 单根语义、artifact schema/example、canonical `subjectId`、`runtime ABI` / `codegen bridge` proof-required subset。

### Stage 1B：Structure And Test Migration

- 落 `Chaos.IL2CPP.*` managed skeleton、`tests/contracts` / `tests/proof` 结构迁移与 tooling canonical path。

### Stage 2：HelloWorld Proof Spec

- 固定 object-based `HelloWorldObject` 的输入、输出、IL 形状、metadata/helper 需求与 proof gate。

### Stage 3：Managed Minimal Closure

- 打通 `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 的最小 managed 闭环。

### Stage 4：Native Bootstrap And Reference Proof

- 打通 `runtime-core -> bootstrap -> support`，在 `Windows x64` reference host 上跑通 `Hello, World!`。

### Stage 5：Post-Proof Split

- 在真实 proof 成功后，完成 post-proof follow-on 的排序、Windows trilogy 闭环与 `macOS` parity handoff 冻结。

## 5. 每阶段完成定义

### Stage 1A

- `goal`: 冻结 first proof 直接依赖的 contract surface。
- `exit_criteria`:
  - `contracts/` 成为 proof-facing contract 的统一入口语义。
  - `typed-il-ir`、`AOT manifest`、`code registration`、`metadata registration` 与 canonical `subjectId` 对齐。
  - `runtime ABI` / `codegen bridge` proof-required subset 明确。
- `deliverables`:
  - 子任务 `20260405-02-stage-1a-contract-surface-reset`
  - child brainstorm / design / plan 文档
  - contract/doc/example/snapshot 的冻结方案

### Stage 1B

- `goal`: 把 Stage 1A 冻结的 contract 方向落到 managed skeleton 与 tests 结构上。
- `exit_criteria`:
  - `src/managed/Chaos.IL2CPP.*` skeleton 建立。
  - `tests/contracts` 成为唯一 canonical contract family。
  - `tests/proof` 与 `tests/smoke` 的角色分离落地。
  - tooling / manifest 中的旧路径切换明确。
- `deliverables`:
  - 子任务 `20260405-03-stage-1b-structure-and-test-migration`
  - child brainstorm / design / plan 文档
  - managed/tests/tooling 迁移计划

### Stage 2

- `goal`: 固定 object-based `HelloWorldObject` proof spec。
- `exit_criteria`:
  - proof 输入输出、最小语义、metadata/helper 需求全部固定。
- `deliverables`:
  - 子任务 `20260405-04-stage-2-helloworld-proof-spec`
  - canonical proof spec 与 source / artifact / native landing 对齐清单

### Stage 3

- `goal`: 打通 managed minimal closure。
- `exit_criteria`:
  - 最小 managed pipeline 能从真实程序集走到 generated-code input bundle。
- `deliverables`:
  - 子任务 `20260406-01-stage-3-managed-minimal-closure`
  - managed closure bundle spec
  - `HelloWorldObject.dll` 到 generated-code input 的最小输出链

### Stage 4

- `goal`: 在 reference platform 上拿到 first native proof。
- `exit_criteria`:
  - `Hello, World!`
  - `exit code = 0`
  - 无 per-method rescue path
- `deliverables`:
  - 子任务 `20260406-02-stage-4-native-bootstrap-and-reference-proof`
  - Stage 4 brainstorm / design / plan 文档
  - first generated C++ emission 与 Windows reference proof 骨架

### Stage 5

- `goal`: 完成 post-proof follow-on 路线的拆分与交接。
- `exit_criteria`:
  - Windows trilogy `GenericEcho -> ReflectionLite -> PInvokeLite` 均已完成归档。
  - `macOS` parity 的首轮 handoff 计划已冻结为独立子任务，并明确排除当前 Windows-only 的 Stage 5C proof。
  - follow-on 是按样例优先还是平台优先推进的边界已经固定。
- `deliverables`:
  - 子任务 `20260406-03-stage-5-post-proof-split`
  - child brainstorm / design / roadmap 文档
  - `20260406-07-stage-5d-macos-reference-parity` handoff 计划

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `20260405-02-stage-1a-contract-surface-reset` | `Stage 1A` | `completed` | `codex` | 冻结 proof-facing contract surface，拆出 contract/doc/example/snapshot 的稳定入口 | `-` |
| `20260405-03-stage-1b-structure-and-test-migration` | `Stage 1B` | `completed` | `codex` | 迁移 managed skeleton、tests family 与 tooling canonical path | `20260405-02-stage-1a-contract-surface-reset` |
| `20260405-04-stage-2-helloworld-proof-spec` | `Stage 2` | `completed` | `codex` | 固定 object-based `HelloWorldObject` proof spec，并收口 canonical sample truth | `20260405-02-stage-1a-contract-surface-reset, 20260405-03-stage-1b-structure-and-test-migration` |
| `20260406-01-stage-3-managed-minimal-closure` | `Stage 3` | `completed` | `codex` | 打通 managed 最小闭环，并把输出结束在 generated-code input bundle | `20260405-04-stage-2-helloworld-proof-spec` |
| `20260406-02-stage-4-native-bootstrap-and-reference-proof` | `Stage 4` | `completed` | `codex` | 消费 Stage 3 closure bundle，已落地 native bootstrap、first generated C++ emission 与 Windows x64 reference proof | `20260405-04-stage-2-helloworld-proof-spec, 20260406-01-stage-3-managed-minimal-closure` |
| `20260406-03-stage-5-post-proof-split` | `Stage 5` | `completed` | `codex` | 基于真实 proof 结果完成新的 post-proof 子 roadmap，并把 follow-on 收敛为 Windows sample-first 路线与独立的 `macOS` parity handoff | `20260406-02-stage-4-native-bootstrap-and-reference-proof` |

说明：

- Stage 1A / 1B / 2 / 3 / 4 / 5 六个阶段均已完成归档。
- reboot 父 roadmap 已不再保留 active 职责；后续 `macOS` parity 若要继续执行，应从 Stage 5D handoff 计划重新开启新任务。

## 7. 依赖

全局依赖关系如下：

1. Stage 1A 先冻结 contract surface。
2. Stage 1B 再落结构迁移和 tests canonical path。
3. Stage 2 基于 1A/1B 的落点固定 proof spec。
4. Stage 3 和 Stage 4 才进入真实闭环。
5. Stage 5 基于 Stage 4 的真实 proof 结果完成 follow-on 拆分与交接。

## 8. 风险

- 如果父 roadmap 写得过重，会重新变成旧路线的翻版。
- 如果不把样例扩张与平台扩张分开，Stage 5 之后很容易再次膨胀成新的大 roadmap。
- 如果后续 `macOS` parity 绕过 Stage 5D handoff 中对 Stage 5C 的排除说明，会把 Windows-only interop 假设误写为跨宿主 contract。

## 9. 备选路径

### 方案 A：继续单一 plan

- 优点：文档最少。
- 缺点：体量过大，不利于依赖管理和归档。

### 方案 B：建立轻量 roadmap，并按阶段派生子任务

- 优点：既拆解体量，又不重新回到超大 roadmap。
- 缺点：需要维护 parent-child 状态与索引。

### 方案 C：一开始就把 Stage 1 到 Stage 5 全部建成 active 子任务

- 优点：形式最完整。
- 缺点：会制造一批尚未 ready 的 active-like 目录，管理成本过高。

当前采用：

- **方案 B**

## 10. 当前建议推进顺序

1. Stage 1A `Contract Surface Reset` 已完成归档。
2. Stage 1B `Structure And Test Migration` 已完成归档。
3. Stage 2 `HelloWorld Proof Spec` 已完成归档。
4. Stage 3 `Managed Minimal Closure` 已完成归档。
5. Stage 4 `Native Bootstrap And Reference Proof` 已完成归档，真实 Windows native proof 已闭环。
6. Stage 5 `Post-Proof Split` 已完成归档，Windows trilogy 已闭环，`macOS` parity handoff 已冻结到独立计划。
