# BDWGC vendor metadata

## 来源

- 项目名称：`BDWGC`（`Boehm-Demers-Weiser Garbage Collector`）
- 当前目录内容来自本地 IL2CPP 源码快照：`E:\agent\dotnet\il2cpp\builds\external\bdwgc`
- 当前 vendored 路径：`third_party/bdwgc`
- 本地 IL2CPP 许可证清单把该外部依赖登记为：`https://github.com/Unity-Technologies/bdwgc/`
- 当前快照自带的 `README.md` 把开发仓库指向：`https://github.com/ivmai/bdwgc`

## 许可证

- 当前快照未自带独立顶层 `LICENSE` 文件
- 许可说明主要来自：
  - `third_party/bdwgc/README.md` 的 `Copyright & Warranty` 段落
  - `E:\agent\dotnet\il2cpp\builds\LICENSE.md` 中 `external/bdwgc` 条目
- 本目录中的 `LICENSE.txt` 为依据上述说明整理出的本地随附许可摘要
- 额外注意：
  - 若干 GNU 风格构建文件带有不同但相近的许可说明
  - 内含的 `libatomic_ops` 子树包含多种许可文本；其中部分未被垃圾回收库实际使用的文件可能为 GPL

## 用途

- 作为 `Roadmap 3` 的 GC backend 候选/首选参考
- 用于 `Runtime Core v1` 早期阶段的堆对象分配、存活对象追踪与不可达对象回收
- 帮助尽快验证对象模型、runtime contract、线程与异常边界，而不在早期被自研 GC 阻塞

## 落点说明

- 当前 vendored 内容为源码级快照，不是预编译二进制
- 目录中保留了 `include/`、核心 `*.c` 源文件、`doc/`、`tests/` 以及 `libatomic_ops/` 子树
- 该依赖当前被记录为 `third_party` 库资产，不等同于已经完成 runtime 集成

## 版本 / commit

- `README.md` 报告当前快照版本为：`7.7.0 (next release development)`
- 当前本地快照无法恢复精确 upstream/fork commit
- 因此该依赖应视为：`Unity IL2CPP local source snapshot + reported version string`
