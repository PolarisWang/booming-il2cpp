# 进展记录 v1-01

## 时间

- 2026-04-11 15:21:32 +08:00

## 本轮完成

- 完成 Batch 1 `Chaos.IL2CPP.HotUpdate` 项目与 package format/loader 最小闭环
  - 新增 `HotUpdatePackage`
  - 新增 `PackageReader`
  - 新增 `PackageValidator`
- 完成 Batch 2 runtime mode switch
  - 新增 `RuntimeManager`
  - `runtime-core` 新增 mixed mode query / switch 支撑
- 完成 Batch 3 minimal interpreter stub
  - 新增 `ConstantInt32InterpreterStub`
  - 新增 `HotUpdateMethodRegistry`
  - 打通 subject-id 级 method registration / dispatch hook
- 完成 Batch 4 subject 与验证
  - 新增 `subjects/HotUpdateSkeletonProof/`
  - 新增 `tests/unit/run/test_phase5_hot_update_skeleton.py`
  - 更新 `tests/unit/run/test_repo_layout.py`
- 完成 native module 接线
  - 新增 `src/native/hot-update/CMakeLists.txt`
  - 新增 `src/native/hot-update/hot_update.h`
  - 新增 `src/native/hot-update/hot_update.cpp`
  - root `CMakeLists.txt` 与 windows core solution 已接入

## 验证结果

- `python -m pytest tests/unit/run/test_phase5_hot_update_skeleton.py -v` -> 6 passed
- `python -m pytest tests/unit/run/test_repo_layout.py tests/unit/run/test_subject_manifest_schema.py -v` -> 26 passed
- `python -m pytest tests/unit/run/test_subject_planner.py -v` -> 18 passed
- `dotnet run --project subjects/HotUpdateSkeletonProof/source/HotUpdateSkeletonProof.csproj --` -> `before-load=1`, `after-load=42`, `after-unload=1`, `corruption=rejected`
- `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release` -> succeeded
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Release` -> succeeded
- `cmake -S . -B artifacts/.tmp-hot-update-reference -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=windows-x64-reference` -> succeeded
- `cmake --build artifacts/.tmp-hot-update-reference --config Release --target chaos_hot_update` -> succeeded

## 备注

- `Chaos.IL2CPP.HotUpdate` 仅依赖 `Chaos.IL2CPP.Contracts`，故意不反向依赖 `Chaos.IL2CPP.CodeGen`。
- 当前 interpreter 仅覆盖 constant-return stub，用于证明 runtime mode / registration / dispatch 骨架，不承担真实 IL 解释职责。
- package corruption rejection 已验证，说明 package reader/validator 的最小完整性边界已建立。

## Wiki

- 已新增 `wiki/04-工具与集成/hot-update-skeleton-v1.md`
- 原因：热更新骨架的项目边界、package 形状、runtime mode 语义和 proof/verification 入口属于长期有效知识。

## 下一步

- 激活 `20260411-07-metadata-supplement-bridge`
- 先为 Batch 1 supplemental metadata template 写 RED：
  - `MetadataWriter` 输出 `supplemental-metadata-template.json`
  - convert 产物接线到 `hot-update/` 子目录
