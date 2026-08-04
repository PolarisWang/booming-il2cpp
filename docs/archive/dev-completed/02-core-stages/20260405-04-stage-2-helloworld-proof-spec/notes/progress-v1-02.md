# 进度记录 v1-02

## 时间

- 2026-04-06 00:04:02 +08:00

## 已完成

- 新增长期规格文档 `wiki/03-功能模块/06-il2cpp核心架构/docs/archive/architecture/roadmap-0/hello-world-proof-spec-v0.md`，固定 object-based `HelloWorld` 的 canonical truth、artifact 真源、direct-call landing 约束与 gate 归属。
- 将 `tests/proof/input/HelloWorldObject/Program.cs` 对齐到 canonical truth：`Program.Main(System.String[]) -> int`、固定 `Greeter("World")`、打印结果并 `return 0`。
- 更新 `contracts/docs/v0/overview.md`，把 Stage 2 proof spec 纳入 canonical 阅读入口。
- 通过 TDD 新增 `tests/unit/run/test_stage2_helloworld_proof_spec.py`，锁定 `Program.cs`、proof spec 文档和 typed-il example 的关键事实。
- 发现并修复 `typed-il-ir` example / snapshot 与目标输出之间的隐藏分叉：`Greeter.BuildMessage()` 现在显式保留结尾 `!`，并在 canonical / compatibility snapshot 中同步。
- 评估 `tests/proof/hello-world-object/suite.manifest.json` 后确认当前 `supportedStages: ["build"]` 已足以表达 Stage 2 只冻结 build/spec gate，因此本轮不需要额外改动 manifest。

## 验证

- `python -m unittest tests.unit.run.test_stage2_helloworld_proof_spec`
- `dotnet build tests/proof/input/HelloWorldObject/HelloWorldObject.csproj -c Release`
- `python -m unittest tests.unit.run.test_stage1b_layout`
- `Get-ChildItem 'contracts/examples/v0/artifacts/typed-il-ir.min.json','analysis/contracts/examples/typed-il-ir.min.json','tests/contracts/schema/typed-il-ir.snapshot.json','tests/contract/schema/typed-il-ir.snapshot.json' | ForEach-Object { Get-Content $_ -Raw | ConvertFrom-Json | Out-Null }`

## 当前状态

- 任务状态：`completed`
- 当前阶段：`archived`

## 结论

- Stage 2 `HelloWorld` proof spec 已完成冻结。
- 后续如继续 reboot 主线，下一步应由父 roadmap 决定 Stage 3 `Managed Minimal Closure` 与 Stage 4 `Native Bootstrap And Reference Proof` 的派生节奏。
