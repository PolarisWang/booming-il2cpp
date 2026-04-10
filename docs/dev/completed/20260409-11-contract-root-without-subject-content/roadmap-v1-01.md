# Contract 去 Subject 内容化 + Repo 强规范化重组 Roadmap v1.01

Date: 2026-04-10
Status: roadmap-frozen-v1-01

## 1. 目标

本 roadmap 的目标不是只完成一次 contract 路径迁移，而是把当前仓库里已经开始失控的三个问题一起收口成可分阶段推进的重构路线:

1. `contracts/` 不再承载任何 concrete subject 内容
2. `build/`、`src/`、`tests/` 建立稳定、可持续扩展的规范化目录结构
3. 超长脚本与超长源码文件拆回合理职责边界，并建立防回退约束

用户已明确选择方案 C，因此本 roadmap 采用“强规范化重组”路线，而不是轻量拆文件或中度重组。

## 2. 范围边界

本 roadmap 负责:

- 重新定义 `contracts/`、`tests/contracts/`、`subjects/` 的 ownership boundary
- 重组 `build/toolchains/run/**`
- 重组 `src/managed/**` 中与当前主线高度耦合的项目，优先是:
  - `Chaos.IL2CPP.CodeGen`
  - `Chaos.IL2CPP.Loader`
  - 必要时扩展到 `Contracts`、`Linker`、`SemanticWorld`
- 重组 `tests/tooling/run/**` 与 `tests/unit/run/**`
- 拆分超长文件与分片 manifest
- 同步修正 active docs、path resolver、command surface 与 repo layout tests

本 roadmap 不负责:

- 对 `src/native/**` 做全面重组
- 重排 `docs/dev/completed/**` 等历史文档树
- 一次性重命名整个 managed namespace 体系
- 修改 subject 输入模型本身，例如 `subjects/<subject-id>/source/**` 的产品语义

## 3. 非目标

当前不做以下事情:

- 仅为了“看起来整齐”而进行无边界 rename
- 在 contract 边界未冻结前，就同时大规模移动所有测试
- 在运行入口未稳定前，就开始全面拆所有 managed 项目
- 把强规范化重组扩展成全仓一次性迁坟

## 4. 阶段列表

### Phase 0: Boundary And Naming Freeze

先冻结终态目录语义、命名规则、分片原则与大文件拆分边界。

### Phase 1: Contract / Fixture / Ownership Cutover

把 `contracts/` 从 concrete fixture 中剥离出来，并建立新的 canonical fixture 根。

### Phase 2: Run Tooling Domain Reorg

重组 `build/toolchains/run/**`，把 UI、command、subject execution、testing helper、manifest loader 分层。

### Phase 3: Manifest Fragmentation And Command Wiring

把超长 manifest 与入口 wiring 从单文件拼盘改成目录化分片与组合加载。

### Phase 4: Managed Source Domain Split

优先拆解 `CodeGen` 与 `Loader`，把 facade、decoder、matcher、template、model、resolver 分离。

### Phase 5: Tests Mirror Reorg

按 source / tooling 领域重新组织 `tests/tooling/run/**` 与 `tests/unit/run/**`，形成镜像式测试结构。

### Phase 6: Legacy Compatibility Cleanup And Enforcement

清理旧路径、兼容别名、遗留目录，并加上 repo layout / file size / path ownership 防回退约束。

## 5. 每阶段完成定义

### Phase 0

- goal:
  - 冻结 option C 的终态结构与命名规则，避免后续每个 child task 自己定义目录
- exit_criteria:
  - 已冻结 `contracts/`、`tests/contracts/`、`build/toolchains/run/`、`src/managed/`、`tests/` 的目标布局
  - 已冻结 “formal definition / shared fixture / subject-owned content” 三类 ownership
  - 已冻结超长文件拆分策略与软性行数预算
  - 已冻结 manifest 分片原则
- deliverables:
  - `layout-target-v1.md`
  - path mapping table
  - file split policy
- dependencies:
  - 当前 brainstorm/design 结论
- open_questions:
  - Python、C#、JSON 各自的软性行数预算定在多少更合适
  - `run_manifest.json` 是按 domain 分片还是按 command group 分片

### Phase 1

- goal:
  - 让 `contracts/` 回到 formal contract root，只保留定义，不再承载 concrete fixture
- exit_criteria:
  - `contracts/` 下不存在 sample / snapshot / native sample / examples
  - `tests/contracts/analysis/v0/**` 成为 analysis contract fixture 的唯一 canonical 根
  - `tests/contract/**` 不再作为 active canonical 路径
  - path resolver、contract validator、run manifest、active docs 已切到新根
- deliverables:
  - migrated fixture tree
  - updated path resolver / consumer wiring
  - docs sync
- dependencies:
  - Phase 0
- open_questions:
  - 是否需要短期 compatibility shim，还是直接硬切旧路径

### Phase 2

- goal:
  - 把 `build/toolchains/run/**` 从“根目录 + commands + testing”的松散结构升级为 domain-based layout
- exit_criteria:
  - `ui / commands / core / subject / testing / manifests` 分层完成
  - `tui.py`、`commands/test.py`、`subject_workers.py` 不再承担混合职责
  - CLI 行为保持兼容，至少对公共命令 surface 不回退
- deliverables:
  - new run tooling tree
  - imports and dispatch rewiring
  - compatibility notes
- dependencies:
  - Phase 0
  - Phase 1
- open_questions:
  - `project_workspace.py` 应该归到 `subject/` 还是 `workspace/`
  - `testing/` 内的 registry/session/reporting 是否独立子域

### Phase 3

- goal:
  - 把 manifest / command registry 从超长单文件改成目录化分片与组合加载
- exit_criteria:
  - `run_manifest.json` 不再是唯一大文件入口
  - manifest 分片后仍能生成完整 registry 与菜单
  - 命令发现、帮助输出、TUI 菜单与测试用例全部读取新组合入口
- deliverables:
  - manifest shard loader
  - per-domain manifest fragments
  - manifest consistency tests
- dependencies:
  - Phase 0
  - Phase 2
- open_questions:
  - shard 文件是继续使用 JSON 还是切到更适合 merge 的结构

### Phase 4

- goal:
  - 拆开 managed 主线中最明显的“大厨房水槽文件”，先让 `CodeGen` 与 `Loader` 可维护
- exit_criteria:
  - `NativeReferenceLoweringPlanner.cs` 只保留 facade / orchestration
  - `LoaderStage.cs` 只保留 facade / orchestration
  - matcher、factory、template、decoder、resolver、model 分文件或分目录落位
  - `.csproj` 与 tests 全部适配新布局
- deliverables:
  - new `CodeGen` tree
  - new `Loader` tree
  - compatibility-safe source split
- dependencies:
  - Phase 0
  - Phase 2
- open_questions:
  - 是否同轮拆 `NativeReferenceProofEmitter.cs`
  - 是否把 `Linker` / `Contracts` / `SemanticWorld` 一并纳入这一阶段尾声

### Phase 5

- goal:
  - 按系统域重排测试树，让 `tests` 能稳定镜像 source / tooling 结构
- exit_criteria:
  - `tests/tooling/run/**` 按 command、tui、subject、contracts、bootstrap 分组
  - `tests/unit/run/**` 按 contracts、subject、registry、workspace、phases 分组
  - 不再继续向扁平目录追加新测试
  - test discovery、CI、命令行运行方式保持稳定
- deliverables:
  - new test tree
  - updated imports and discovery assumptions
  - test taxonomy notes
- dependencies:
  - Phase 1
  - Phase 2
  - Phase 4
- open_questions:
  - legacy shim tests 是保留在原位还是一起迁到 `phases/legacy/`

### Phase 6

- goal:
  - 清空遗留别名、旧路径和结构回退风险，让 option C 的终态能够自我维持
- exit_criteria:
  - active code/doc/test 中不再引用旧 canonical 路径
  - `contracts/examples/**`、`contracts/native/examples/**`、`tests/contract/**` 等遗留并行树已退出 active 使用
  - 新增 repo layout / file budget / ownership rule tests
  - 没有仍然明显违反拆分边界的超长热点文件
- deliverables:
  - cleanup patch set
  - enforcement tests
  - repo layout docs
- dependencies:
  - Phase 1
  - Phase 2
  - Phase 3
  - Phase 4
  - Phase 5
- open_questions:
  - enforcement 做成 repo layout test 还是单独 lint/guard 更合适

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `layout-boundary-freeze` | `Phase 0` | `ready` | `codex` | 冻结终态目录、命名、ownership 和分片规则 | `-` |
| `contract-fixture-ownership-cutover` | `Phase 1` | `ready` | `codex` | 把 contract 定义与 concrete fixture 分离 | `layout-boundary-freeze` |
| `run-tooling-domain-reorg` | `Phase 2` | `planned` | `codex` | 重组 `build/toolchains/run/**` 的目录与模块边界 | `layout-boundary-freeze, contract-fixture-ownership-cutover` |
| `manifest-fragmentation-and-registry-loader` | `Phase 3` | `planned` | `codex` | 把 command manifest 改为目录化分片与组合加载 | `run-tooling-domain-reorg` |
| `managed-codegen-domain-split` | `Phase 4` | `planned` | `codex` | 拆分 `Chaos.IL2CPP.CodeGen` 热点文件与子目录 | `layout-boundary-freeze, run-tooling-domain-reorg` |
| `managed-loader-domain-split` | `Phase 4` | `planned` | `codex` | 拆分 `Chaos.IL2CPP.Loader` 热点文件与子目录 | `layout-boundary-freeze, run-tooling-domain-reorg` |
| `tests-mirror-reorg` | `Phase 5` | `planned` | `codex` | 按 source/tooling 域重排测试树 | `contract-fixture-ownership-cutover, run-tooling-domain-reorg, managed-codegen-domain-split, managed-loader-domain-split` |
| `legacy-cleanup-and-enforcement` | `Phase 6` | `planned` | `codex` | 移除旧路径、补规则、防止结构回退 | `manifest-fragmentation-and-registry-loader, tests-mirror-reorg` |

## 7. 依赖

推荐依赖顺序如下:

1. 先冻结终态边界与命名规则
2. 再完成 contract / fixture / ownership cutover
3. 再重组 run tooling 主骨架
4. 然后拆 manifest 与 command registry
5. 并行推进 managed `CodeGen` 与 `Loader` 拆分
6. 之后再重排 tests，避免测试先动后源代码再动两次
7. 最后统一清理 legacy path 与 enforcement

其中:

- `managed-codegen-domain-split` 和 `managed-loader-domain-split` 可以在 Phase 2 后并行准备
- `tests-mirror-reorg` 不应早于 Phase 4 完成，否则测试会重复迁移

## 8. 风险

### 风险 1: 范围膨胀

方案 C 最大的风险不是技术难度，而是它会天然诱惑人继续扩到 `src/native/**`、历史文档树、全仓 namespace 风格统一，最后偏离主线。

### 风险 2: 先搬 tests，后搬 source

如果测试树先镜像化，而 `build/` 与 `src/managed/` 还没稳定，测试会经历两轮路径迁移，成本翻倍。

### 风险 3: 同轮同时重组 build、source、tests

如果三边同时大规模 rename，很容易出现“代码没坏，但 import/discovery/path wiring 全部坏掉”的非功能性回归。

### 风险 4: 热点文件拆分时引入行为变化

`NativeReferenceLoweringPlanner.cs` 和 `LoaderStage.cs` 都承担很多隐式约束，拆分时若顺手改逻辑，回归面会很大。

### 风险 5: compatibility shim 留太久

如果旧路径兼容层保留太久，最终仓库会再次形成双 canonical 根。

## 9. 备选路径

### 备选路径 A: 退回方案 A

只拆超长文件，不重组目录。

不推荐。可以短期见效，但会把 option C 的长期收益直接砍掉。

### 备选路径 B: 退回方案 B

保留中度重组，停止强规范化。

这是可接受降级路径。如果后续发现方案 C 的推进成本过高，可以退回这里。

### 备选路径 C: 保持当前选定路线

先做边界冻结与 contract cutover，再重组 run tooling，再拆 managed，再镜像化 tests，最后清理 enforcement。

这是当前推荐路径。

## 10. 当前建议推进顺序

既然你已经选了方案 C，我建议立刻启动的 child task 只有两个:

1. `layout-boundary-freeze`
   - 先把终态结构和命名规则写死
   - 不然每个后续 task 都会自己定义一版目录

2. `contract-fixture-ownership-cutover`
   - 这是整个强规范化重组里最先该收口的 shared boundary
   - 不先切 clean ownership，后面的 `build/`、`tests/` 和 docs 都会继续带着旧路径

这两个任务完成后，后面的推荐顺序是:

1. `run-tooling-domain-reorg`
2. `manifest-fragmentation-and-registry-loader`
3. `managed-codegen-domain-split`
4. `managed-loader-domain-split`
5. `tests-mirror-reorg`
6. `legacy-cleanup-and-enforcement`

从工程控制角度看，这个顺序最稳，因为它遵守了:

- 先边界
- 再入口骨架
- 再核心源码
- 最后镜像 tests 和做清理

而不是一上来就“全仓一起搬”。
