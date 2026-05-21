# IL2CPP-Like C# to C++ 重量版 Stage 1 Contract Reset 实现计划

> **面向执行 Agent：** 必须使用 `dev:subagent-driven-development`（如可用子 Agent）或 `dev:executing-plans` 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 在进入 object-based `HelloWorld` proof 实现前，完成 Stage 1 所需的 contract reset、目录结构收敛、`Chaos.IL2CPP.*` managed 骨架、以及 tests 角色重组。

**架构：** 本计划不直接实现 proof 闭环，而是先把 proof 所依赖的 contract surface、目录语义和验证入口压成稳定骨架。执行完成后，仓库应具备统一的 `contracts/` 根目录、细粒度 `Chaos.IL2CPP.*` managed 分层、以及明确区分 `contracts / smoke / proof / integration / platform / unit / fixtures` 的测试树。

**技术栈：** .NET 8、`System.Reflection.Metadata + PEReader`、JSON Schema、CMake/Ninja、PowerShell / shell wrapper、仓库内 `run test` 统一测试框架。

**设计文档：** `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/design-v1-01.md`

**预期知识沉淀：**

- `wiki/01-项目总览/仓库目录与产物边界.md`
- `wiki/04-工具与集成/统一测试框架.md`
- `wiki/06-测试验证/测试分层.md`

---

## 目标文件与职责

- 修改：`docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/STATUS.md`
  - 维护当前阶段、关键文档、最近摘要和下一步。
- 修改：`docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/notes/progress-v1-01.md`
  - 每完成一个 chunk 记录一次已完成与下一步。
- 修改：`docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/finding-2026-04-05-01-il2cpp-reboot-after-abandoned-roadmap.md`
  - 记录关键结构决策、迁移取舍和例外。
- 创建或修改：`contracts/artifacts/v0/`
  - 承接原 `analysis/contracts` 下的 schema、example artifact 与共享 `subjectId` 规则。
- 创建或修改：`contracts/native/v0/`
  - 保持 proof-facing `runtime ABI` 与 `codegen bridge` 头文件。
- 创建或修改：`contracts/docs/v0/`
  - 形成 artifacts + native contract 的统一文档入口。
- 创建或修改：`contracts/examples/v0/`
  - 放 bootstrap / registration / native 或 artifact 用例。
- 创建：`src/managed/Chaos.IL2CPP.Contracts/`
  - managed 侧 contract object model、shared DTO、canonical `subjectId`。
- 创建：`src/managed/Chaos.IL2CPP.Driver/`
  - orchestration skeleton。
- 创建：`src/managed/Chaos.IL2CPP.Loader/`
  - SRM + PEReader 读取层骨架。
- 创建：`src/managed/Chaos.IL2CPP.SemanticWorld/`
  - metadata graph / world skeleton。
- 创建：`src/managed/Chaos.IL2CPP.Pipeline/`
  - context / collector / result / phase skeleton。
- 创建：`src/managed/Chaos.IL2CPP.Linker/`
  - retained world / `AOT manifest` skeleton。
- 创建：`src/managed/Chaos.IL2CPP.MetadataWriter/`
  - `code registration` / `metadata registration` skeleton。
- 创建：`src/managed/Chaos.IL2CPP.CodeGen/`
  - proof-facing generated-code emission skeleton。
- 修改：`tests/contracts/`
  - 成为 contract 测试唯一 canonical 路径；合并 `tests/contract` 重复内容。
- 创建：`tests/proof/input/HelloWorldObject/`
  - object-based proof 输入项目。
- 创建：`tests/proof/hello-world-object/`
  - proof suite manifest 与 run gate。
- 修改：`tests/smoke/`
  - 保留 smoke 角色，去掉 proof 角色暗示。
- 修改：`tests/integration/`、`tests/tooling/`、`tests/unit/`
  - 更新所有硬编码路径、suite 发现和测试目录常量。
- 创建或修改：`tests/fixtures/`
  - 承接 catalog / registry / manifest fixture。

## 执行原则

- [ ] 所有变更都必须围绕 Stage 1 proof-facing contract reset，不要顺手扩张到 `HelloWorld` proof 实现本身。
- [ ] 不要再新增第三套 contract 根目录或第三套 tests family 语义。
- [ ] `tests/contracts` 是 canonical 路径；`tests/contract` 只能作为短期兼容过渡，不能继续新增内容。
- [ ] `tests/smoke/input/HelloWorld` 明确保留为 smoke；新的 object-based proof 必须进入 `tests/proof/input/HelloWorldObject`。
- [ ] `src/managed` 一律使用 `Chaos.IL2CPP.*` 命名；不混用旧托管命名空间。
- [ ] 每完成一个 chunk，都要同步更新 `STATUS.md`、`docs/dev/ACTIVE.md`、`notes/progress-*.md`，并在必要时追加 finding。

## Chunk 1：统一 contract 根目录

- [ ] 创建 `contracts/artifacts/v0/` 目录布局：
  - `schemas/`
  - `examples/`
  - `docs/` 或统一挂到 `contracts/docs/v0/`
- [ ] 将原 `analysis/contracts/schemas/*.json` 迁移或镜像到 `contracts/artifacts/v0/schemas/`。
- [ ] 将原 `analysis/contracts/examples/*.json` 迁移或镜像到 `contracts/artifacts/v0/examples/`。
- [ ] 明确并落文 canonical `subjectId` 规则，作为 `typed-il-ir / aot-manifest / code-registration / metadata-registration` 的共享约束。
- [ ] 在 `contracts/docs/v0/` 建立统一入口文档，说明：
  - artifact contract
  - native contract
  - versioning
  - shared `subjectId`
- [ ] 评估 `analysis/contracts` 的保留方式：
  - 若直接迁移，更新引用路径
  - 若短期保留，明确标成 compatibility copy，并禁止继续写新内容

## Chunk 2：冻结 Pack B + Pack D 的 artifact 契约

- [ ] 对 `typed-il-ir` schema 做 Stage 1 proof-required subset 收敛。
- [ ] 对 `AOT manifest` schema 做 proof 最小 retained world 收敛。
- [ ] 对 `code registration` schema 做最小 generated method pointer 收敛。
- [ ] 对 `metadata registration` schema 做最小 type/method/field/string 元数据收敛。
- [ ] 为 `Greeter..ctor`、`Greeter.BuildMessage`、`Program.Main` 产出新的 canonical example artifact 或 snapshot。
- [ ] 明确 artifact 排序与 diff 稳定性规则：
  - 方法顺序
  - block 顺序
  - registration 顺序
  - slot / symbol 生成规则

## Chunk 3：冻结 Pack C 的 runtime / bridge 边界

- [ ] 审视 `contracts/native/v0/runtime_abi.h`，标出 Stage 1 proof-required subset 与 deferred subset。
- [ ] 审视 `contracts/native/v0/codegen_bridge.h`，标出 Stage 1 proof-required subset 与 deferred subset。
- [ ] 在 `contracts/docs/v0/` 中补一页“ABI vs bridge 职责边界”说明：
  - ABI 负责 lifecycle / allocation / field access / minimal query / managed failure
  - bridge 负责 registration / bootstrap / token resolve / minimal generated-code helper
- [ ] 明确 `invoke_virtual`、`resolve_virtual_method` 在 Stage 1 的窄化语义。
- [ ] 明确 `box_value / unbox_value / delegate / icall` 延后，不进入 Stage 1 的完成门槛。

## Chunk 4：建立 `Chaos.IL2CPP.*` managed 骨架

- [ ] 创建 `src/managed/Chaos.IL2CPP.Contracts/Chaos.IL2CPP.Contracts.csproj`。
- [ ] 创建 `src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj`。
- [ ] 创建 `src/managed/Chaos.IL2CPP.Loader/Chaos.IL2CPP.Loader.csproj`。
- [ ] 创建 `src/managed/Chaos.IL2CPP.SemanticWorld/Chaos.IL2CPP.SemanticWorld.csproj`。
- [ ] 创建 `src/managed/Chaos.IL2CPP.Pipeline/Chaos.IL2CPP.Pipeline.csproj`。
- [ ] 创建 `src/managed/Chaos.IL2CPP.Linker/Chaos.IL2CPP.Linker.csproj`。
- [ ] 创建 `src/managed/Chaos.IL2CPP.MetadataWriter/Chaos.IL2CPP.MetadataWriter.csproj`。
- [ ] 创建 `src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj`。
- [ ] 建立最小 project reference 规则，禁止跨层乱依赖。
- [ ] 在 `Chaos.IL2CPP.Contracts` 中放第一批 shared contract model：
  - canonical `subjectId`
  - artifact DTO
  - proof sample identity

## Chunk 5：重组 tests 目录语义

- [ ] 合并 `tests/contract` 与 `tests/contracts`：
  - `tests/contracts` 作为唯一 canonical 根
  - 清点所有仍引用 `tests/contract` 的脚本、测试和文档
- [ ] 新建 `tests/proof/input/HelloWorldObject/` 与 `tests/proof/hello-world-object/`。
- [ ] 保留 `tests/smoke/input/HelloWorld/`，并在 README 或 suite 文档里明确它只代表 smoke。
- [ ] 清理 `tests/gate` 的长期语义：
  - 能迁入 `tests/platform` 的迁入 `tests/platform`
  - 只在 CI 阶段有意义的 gate，保留为流程概念而不是长期目录主轴
- [ ] 清理 `tests/registry` 的长期语义：
  - fixture 数据迁入 `tests/fixtures`
  - runner / 扫描测试保留在 `tests/integration`
- [ ] 更新 suite manifest、fixture 路径和 run tooling 中的 family/path 常量。

## Chunk 6：补齐 Stage 1 文档和知识沉淀

- [ ] 更新 `design-v1-01.md` 中已确认的结构决策与执行落点。
- [ ] 在 `wiki/01-项目总览/仓库目录与产物边界.md` 写明：
  - `contracts/` 是 contract 资产统一根
  - `artifacts/` 是运行产物
- [ ] 在 `wiki/04-工具与集成/统一测试框架.md` 记录新的 tests family 语义：
  - `contracts`
  - `smoke`
  - `proof`
  - `integration`
  - `platform`
  - `unit`
  - `fixtures`
- [ ] 在 `wiki/06-测试验证/测试分层.md` 补充 smoke 与 proof 的角色差异。

## Chunk 7：计划内状态维护

- [ ] 开始执行前，在 `STATUS.md` 里把 `roadmap_or_plan` 指向本计划文件。
- [ ] 每完成一个 chunk，更新：
  - `STATUS.md` 的最近摘要
  - `docs/dev/ACTIVE.md` 的最近摘要 / 下一步
  - `notes/progress-*.md`
- [ ] 遇到结构决策变化或迁移例外时，追加 `finding-*.md`。
- [ ] 若迁移过程中形成长期有效规则，补充对应 wiki 页面。

## 关键验证命令

- [ ] JSON / schema 相关文件至少通过基础可解析性检查：
```powershell
Get-ChildItem contracts/artifacts/v0/schemas/*.json,contracts/artifacts/v0/examples/*.json | ForEach-Object { Get-Content $_ | ConvertFrom-Json | Out-Null }
```

- [ ] Python runner / suite 发现相关测试：
```powershell
python -m pytest tests/unit/run tests/integration/run tests/tooling/run
```

- [ ] Native contract compile-only smoke：
```powershell
cmake -S tests/contracts/native/abi -B artifacts/native-abi-smoke -G Ninja
cmake --build artifacts/native-abi-smoke
cmake -S tests/contracts/native/bridge -B artifacts/native-bridge-smoke -G Ninja
cmake --build artifacts/native-bridge-smoke
```

- [ ] Managed smoke 与新的 proof 输入项目至少能 build：
```powershell
dotnet build tests/smoke/input/HelloWorld/HelloWorld.csproj -c Release
dotnet build tests/proof/input/HelloWorldObject/HelloWorldObject.csproj -c Release
```

- [ ] 统一命令入口至少通过已有 CLI 覆盖：
```powershell
powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File tests/integration/run/test_run_cli.ps1
```

## 完成标准

- [ ] `contracts/` 已成为 proof-facing contract 的统一根目录语义。
- [ ] `src/managed` 已建立 `Chaos.IL2CPP.*` 细粒度骨架。
- [ ] `tests/contracts` 成为唯一 canonical contract family。
- [ ] `tests/proof` 已引入 object-based `HelloWorld` 的独立路径。
- [ ] 所有受影响的 run/tooling/integration 测试路径已同步。
- [ ] 关键 wiki 页面已补充新的目录和测试语义。
- [ ] `STATUS.md`、`ACTIVE.md`、`notes/progress-*.md`、`finding-*.md` 已同步。

## 执行交接

计划完成后，执行者必须先运行上面的关键验证命令，再决定是否进入下一轮实现。

计划完成并保存到 `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/plan-v1-01.md` 后，下一步应使用 `dev:subagent-driven-development` 或 `dev:executing-plans` 进入执行。
