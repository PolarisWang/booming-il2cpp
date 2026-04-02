# Reference Desktop Gates (Roadmap 0)

## 定位

`Windows x64 + macOS` 共同构成 `Roadmap 0` 的 reference desktop。它们不是对等复制，而是“Windows 主基线 + macOS 同语义子集”。

## Windows x64

- 作为高阶 contract 主基线
- 负责 ABI / bridge compile-only smoke
- 负责 5 个 smoke 输入项目的主线 build
- 负责 Windows lazy warmup trace compare
- 平台能力相关 `icall` 仅保留 query / basic-path 级别要求

## macOS

- 作为 reference desktop 的同语义子集
- 负责与 Windows 接近范围的 trace / contract 验证
- 负责 macOS lazy warmup trace compare
- iOS compile/link/packaging gate 由 macOS host 调度
- 平台能力相关 `icall` 同样只保留 query / basic-path 留白

## 允许留白

- 不要求 `Roadmap 0` 完成平台专属 `icall` 的完整实现
- 不要求 Android / iOS 提前进入完整移动平台 runtime 语义
- 允许 reference desktop gate 以 contract、trace、startup shell、packaging shell 为主，而不是完整产品级运行时

## 与 Host Matrix 的关系

- Windows host: `Windows + Android + Linux`
- macOS host: `macOS + iOS + Linux`
- Linux cross-compile / packaging 永远属于 common gate，由两个 host profile 都调度

## 结论

reference desktop gate 的目标不是“把所有平台都跑通”，而是给后续 runtime / codegen 演进提供一个稳定、可追责、可 diff 的桌面 contract 基线。
