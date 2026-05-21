# Phase 1 Progress - 2026-04-15 00:32:10 +08:00

## 已锁定的第一批切口

- `subjects/SolutionCorePack/source/Launcher/Program.cs`
  - 仍支持 `ChaosSourceEntryArguments`，并自行解析 `assembly/type/method` 字符串。
- `subjects/HotUpdateHostPack/source/Host/Program.cs`
  - 仍维护 `DeclaredProofEntriesBySourceEntry` 字符串字典。
- `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Program.cs`
  - 同样维护 `DeclaredProofEntriesBySourceEntry` 字符串字典。
- `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`
  - 仍使用 attribute 全名和命名参数名字符串读取 metadata schema。

## 下一步

- 找到现有 tests 中能承接 launcher / discovery 的位置，先写 RED 测试，再切第一批实现。
