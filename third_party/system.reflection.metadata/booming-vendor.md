# System.Reflection.Metadata vendor metadata

## 来源

- 上游项目：`.NET runtime`
- 当前目录内容来自本地源码快照：`E:\agent\dotnet\runtime\src\libraries\System.Reflection.Metadata`
- 当前 vendored 路径：`third_party/system.reflection.metadata`

## 许可证

- `MIT`
- 依据：上游仓库根许可证 `E:\agent\dotnet\runtime\LICENSE.TXT`

## 用途

- 作为 `Roadmap 1` 的主前端读取层
- 提供 `System.Reflection.Metadata` 与 `System.Reflection.PortableExecutable.PEReader`
- 用于 PE / metadata 底层读取、`Metadata Graph` 构建、`Typed IL IR` 前端底座

## 落点说明

- 该依赖按 Roadmap 1 的主实现决策 vendored 到 `third_party/`
- 当前保留 `ref/`、`src/`、`README.md` 与许可证文件，作为源码级第三方依赖快照
