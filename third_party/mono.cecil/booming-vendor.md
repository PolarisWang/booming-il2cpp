# Mono.Cecil vendor metadata

## 来源

- 上游项目：`Unity-Technologies/cecil`
- 当前目录内容来自本地 IL2CPP 快照：`E:\agent\dotnet\il2cpp\builds\external\cecil`
- 当前 vendored 路径：`third_party/mono.cecil`

## 许可证

- `MIT`
- 当前本地快照未随附独立许可证文件；本目录中的 `LICENSE.txt` 为按上游许可补录的 MIT 文本

## 用途

- 作为 `Roadmap 1` 的测试/对照基线
- 用于程序集结构对照、tricky case oracle、必要时的测试辅助
- 不作为前端主实现边界

## 落点说明

- 当前 vendored 内容为 `netstandard` 二进制快照，而不是完整源码树
- 上游修订信息来自本地快照附带的 `Readme.txt`：
  - branch: `unity-master`
  - commit: `55142e20f3972f9d83f2c18644a5d4093ad2f526`
