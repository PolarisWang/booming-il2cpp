# third_party Index

| 名称 | 源码仓库 | 版本 / commit | 许可证 | 用途 | 目录路径 |
|---|---|---|---|---|---|
| System.Reflection.Metadata (incl. PEReader) | [`dotnet/runtime`](https://github.com/dotnet/runtime) | runtime commit `ab2f538fca8e2d278e592da1dd6e6ad82954e91c`；本地快照来自 `src/libraries/System.Reflection.Metadata` | MIT | `Roadmap 1` 主前端读取层；PE/metadata 底层读取、`Metadata Graph` 与 `Typed IL IR` 底座 | `third_party/system.reflection.metadata` |
| Mono.Cecil | [`Unity-Technologies/cecil`](https://github.com/Unity-Technologies/cecil) | `unity-master` @ `55142e20f3972f9d83f2c18644a5d4093ad2f526`；当前 vendored 内容为本地 IL2CPP `netstandard` 二进制快照 | MIT | `Roadmap 1` 测试/对照基线；程序集结构对照、tricky case oracle、必要时测试辅助 | `third_party/mono.cecil` |
| BDWGC (Boehm-Demers-Weiser GC) | [`Unity-Technologies/bdwgc`](https://github.com/Unity-Technologies/bdwgc) | 本地 IL2CPP 源码快照；`README.md` 报告版本 `7.7.0 (next release development)`；精确 commit 不可恢复 | custom permissive；另含 `libatomic_ops` / GNU build files 的附加许可说明 | `Roadmap 3` GC backend 候选；用于 `Runtime Core v1` 早期堆对象分配与回收 | `third_party/bdwgc` |
| Scriban | [`scriban/scriban`](https://github.com/scriban/scriban) | 上游 tag/commit 未随当前 snapshot 带入；本仓库导入锚点为 `booming import commit 3ecd20e` | BSD-2-Clause | .NET 模板/脚本引擎（用于文本模板与脚本能力） | `third_party/scriban` |
