# Hot Update Skeleton v1

## 目标

Phase 5 的目标不是做完整热更新，而是建立一个可�?desktop 上验证的最小骨架，证明下面几件事已经可用：

- AOT �?Mixed 两种 runtime mode 可以切换�?- hot update package 可以被读取、校验并装载到最小运行时管理面�?- runtime 能在不改�?AOT 主路径默认行为的前提下，把指定方法路由到最�?interpreter stub�?- 卸载 hot update 后，调用结果可以回到 AOT 默认值�?
## 项目边界

### Managed

- `src/managed/Chaos.IL2CPP.HotUpdate/`
  - `HotUpdatePackage`
  - `PackageReader`
  - `PackageValidator`
  - `RuntimeManager`
  - `HotUpdateMethodRegistry`
  - `ConstantInt32InterpreterStub`

### Native

- `src/native/hot-update/`
  - `hot_update.h`
  - `hot_update.cpp`
- `src/native/runtime-core/`
  - mixed mode query / switch 支撑

### 证明�?subject

- `subjects/HotUpdateSkeletonProof/`

## 隔离规则

- `Chaos.IL2CPP.HotUpdate` 只允许依�?`Chaos.IL2CPP.Contracts`�?- `Chaos.IL2CPP.HotUpdate` 不允许反向依�?`Chaos.IL2CPP.Generator`�?- 当前 proof 使用 subject-id 级方法注册与分发，目的是先证明骨架，不是提前实现完整跨模式调用图�?
## Package 形状

当前最�?package manifest 关注下面这些字段�?
- `formatVersion`
- `packageId`
- `targetAotVersion`
- `assemblies[]`
- `supplementalMetadata`
- `signature`

Phase 5 的校验边界是�?
- 能正确读�?manifest
- 能拒绝损�?package
- 能在 AOT 版本兼容时完成最小装�?
它还不负责：

- 真实 supplemental metadata 合并
- 完整签名体系
- bridge generation

## Runtime 语义

- 默认模式�?`AOT`
- 加载 hot update package 后切�?`Mixed`
- 卸载后回�?`AOT`
- AOT-only 路径必须保持原始行为，不允许因为 skeleton 引入回归

## Interpreter 边界

当前 interpreter 只实现了 constant-return stub，用于证明：

- hot update 方法可以被注�?- dispatch hook 可以命中 hot update 路由
- 运行时可以返回稳定的替换�?
它不代表�?
- 完整 IL 解释�?- metadata supplement
- AOT/HotUpdate 双向 bridge

## Proof

`HotUpdateSkeletonProof` 的最小闭环如下：

1. AOT 默认调用 `Helper.GetValue()`，结果为 `1`
2. 加载 hot update package
3. 再次调用 `Helper.GetValue()`，通过 interpreter stub 返回 `42`
4. 卸载 package
5. 再次调用回到 `1`
6. 损坏 package 被正确拒�?
对应控制台输出要点：

- `before-load=1`
- `after-load=42`
- `after-unload=1`
- `corruption=rejected`

## 验证入口

### 单元测试

```powershell
python -m pytest tests/unit/run/test_phase5_hot_update_skeleton.py -v
python -m pytest tests/unit/run/test_repo_layout.py tests/unit/run/test_subject_manifest_schema.py -v
```

### Subject proof

```powershell
dotnet run --project subjects/HotUpdateSkeletonProof/source/HotUpdateSkeletonProof.csproj --
```

### 构建验证

```powershell
dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release
dotnet build verification/workspaces/core/windows/chaos-il2cpp-core.sln -c Release
cmake -S . -B artifacts/.tmp-hot-update-reference -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=windows-x64-reference
cmake --build artifacts/.tmp-hot-update-reference --config Release --target chaos_hot_update
```

## �?Phase 6 的交�?
Phase 5 只提供承载面。Phase 6 需要继续补齐：

- `supplemental-metadata-template.json` 输出
- supplemental metadata writer / loader
- AOT <-> HotUpdate bridge generation
- `BridgeRoundtripProof`
- `GenericSupplementProof`

