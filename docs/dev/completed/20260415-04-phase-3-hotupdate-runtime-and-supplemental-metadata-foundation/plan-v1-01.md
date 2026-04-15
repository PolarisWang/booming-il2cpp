# Phase 3 HotUpdate Runtime And Supplemental Metadata Foundation 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:systematic-debugging` 推进本计划。先补 RED 测试，再做最小实现。
> 目标：建立可复用的 `HotUpdateAssemblyLoader`、`SupplementalMetadataRegistry` 与 `RuntimeManager` integration foundation，让 hot update runtime 真正具备“package -> assembly image -> metadata registry -> runtime state”闭环。

**设计文档**

- `docs/dev/in-progress/20260414-37-hybridclr-aligned-aot-hotupdate-development-plan/roadmap-v1-01.md`
- `docs/dev/completed/20260415-01-phase-0-contract-freeze-and-aot-translation-ledgers/stable-identity-and-body-availability-v1-01.md`
- `docs/dev/completed/20260415-03-phase-2-typed-il-shared-contract-and-hybrid-dispatch-foundation/STATUS.md`

## 预期改动面

- `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdatePackage.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/PackageReader.cs`
- 新增 `src/managed/Chaos.IL2CPP.HotUpdate/HotUpdateAssemblyLoader.cs`
- 新增 `src/managed/Chaos.IL2CPP.HotUpdate/SupplementalMetadataRegistry.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/RuntimeManager.cs`
- `tests/unit/compatibility/test_phase3_hotupdate_runtime_foundation.py`
- 视实现需要补充现有 metadata supplement / hot update subject tests

## 执行步骤

- [ ] Task 1: 先补 RED 测试，锁定以下 contract
  - `HotUpdateAssemblyLoader` 存在并成为 package/runtime 之间的程序集装载入口
  - `LoadedHotUpdatePackage` 或等价 runtime package model 能显式携带“已装载程序集 + supplemental metadata”
  - `SupplementalMetadataRegistry` 存在，且对外提供 registry 级 surface，而不是只有一次性 loader
  - `RuntimeManager` 通过 loader + registry 消费 package，而不是直接在 integrity 检查里临时读取 metadata bytes
- [ ] Task 2: 实现 `HotUpdateAssemblyLoader`
  - 从 `PackageReader` 分离程序集读取与 hash/size 校验
  - 形成可被 runtime 重用的 loaded assembly catalog
- [ ] Task 3: 实现 `SupplementalMetadataRegistry`
  - 支持注册活动 metadata
  - 支持与 package / runtime 状态对齐
  - public 面优先 typed-facing，必要的 string helper 仅限 internal/private
- [ ] Task 4: 接线 `RuntimeManager`
  - `LoadPackage` 走 loader + registry
  - `UnloadPackage` / `Rollback` 同步活动 metadata 状态
  - integrity validation 不再临时单次解析 metadata 文件
- [ ] Task 5: 跑通 Phase 3 首刀验证并更新父 roadmap / 子任务状态

## 验收

- hot update runtime 有明确的 loader / registry / active state 边界，而不是把 package 读取与 metadata 验证散落在 `RuntimeManager` 内部。
- supplemental metadata 不再只有 `LoadFromBytes -> 临时查询` 形态，runtime 可以显式持有当前活动 metadata registry。
- 新增 public runtime foundation surface 不扩张 string compatibility 面。
- 受影响的 proof / compatibility tests 通过，且不会回退 Phase 2 的 typed contract 边界。

## 验证命令

- `python -m pytest tests/unit/compatibility/test_phase3_hotupdate_runtime_foundation.py -q`
- `python -m pytest tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py tests/unit/compatibility/test_hot_update_skeleton_subject.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release -m:1`
- `dotnet build subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release -m:1 -p:BaseOutputPath=artifacts/.tmp-phase3a/hotupdatehost/`
- `dotnet run --project subjects/HotUpdateHostPack/source/HotUpdateHostPack.csproj -c Release --no-build -p:BaseOutputPath=artifacts/.tmp-phase3a/hotupdatehost/ --`
