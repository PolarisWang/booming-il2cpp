# Chunk 4：Smoke 输入项目与子场景分配

## 目标

把 5 个 smoke 输入项目都落成真实 `.csproj`，并按已经确认的职责边界分配子场景。

### 任务 1：创建 smoke 项目骨架与公共约定

**文件：**
- 创建：`tests/smoke/README.md`
- 创建：`tests/smoke/Directory.Build.props`
- 创建：`tests/smoke/input/HelloWorld/HelloWorld.csproj`
- 创建：`tests/smoke/input/HelloWorld/Program.cs`
- 创建：`tests/smoke/input/GenericEcho/GenericEcho.csproj`
- 创建：`tests/smoke/input/GenericEcho/Program.cs`
- 创建：`tests/smoke/input/ReflectionLite/ReflectionLite.csproj`
- 创建：`tests/smoke/input/ReflectionLite/Program.cs`
- 创建：`tests/smoke/input/PInvokeLite/PInvokeLite.csproj`
- 创建：`tests/smoke/input/PInvokeLite/Program.cs`
- 创建：`tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj`
- 创建：`tests/smoke/input/HostEmbeddingLite/Program.cs`

- [ ] **步骤 1：创建 smoke 目录与项目目录**

运行：
```powershell
New-Item -ItemType Directory tests/smoke/input/HelloWorld,tests/smoke/input/GenericEcho,tests/smoke/input/ReflectionLite,tests/smoke/input/PInvokeLite,tests/smoke/input/HostEmbeddingLite -Force
```

- [ ] **步骤 2：写 `tests/smoke/README.md`**

说明：
- 为什么固定只有 5 个项目
- 每个项目负责什么
- 通过子场景扩展覆盖面，而不是继续加项目数

- [ ] **步骤 3：写 `Directory.Build.props`**

统一：
- target framework
- nullable / langversion 等通用设置
- 输出目录规范

- [ ] **步骤 4：为 5 个项目写最小 `.csproj` 和可运行 `Program.cs`**

要求：
- 每个项目都能单独 `dotnet build`
- 不允许只放占位空文件

### 任务 2：落实 `HelloWorld`、`GenericEcho`、`ReflectionLite`

**文件：**
- 修改：`tests/smoke/input/HelloWorld/Program.cs`
- 修改：`tests/smoke/input/GenericEcho/Program.cs`
- 修改：`tests/smoke/input/ReflectionLite/Program.cs`
- 创建：`tests/smoke/input/HelloWorld/README.md`
- 创建：`tests/smoke/input/GenericEcho/README.md`
- 创建：`tests/smoke/input/ReflectionLite/README.md`

- [ ] **步骤 1：让 `HelloWorld` 只承载最小 entry path**

要求：
- 保留最薄的一层 registration 痕迹
- 不承担完整 registration 流

- [ ] **步骤 2：让 `GenericEcho` 聚焦最直观的泛型调用**

要求：
- 只保留泛型实例与最直观调用形态
- 不再承载过多高阶 metadata 解释

- [ ] **步骤 3：让 `ReflectionLite` 成为高阶反射 contract 主体**

至少覆盖：
- `Type`
- `Field`
- `Method`
- `Property`
- `Event`
- `Parameter`
- 部分 `GenericContext`

- [ ] **步骤 4：为三个项目补 README**

每个 README 至少写清：
- 该项目负责的 contract 面
- 明确不负责的内容
- 子场景清单

### 任务 3：落实 `PInvokeLite` 与 `HostEmbeddingLite`

**文件：**
- 修改：`tests/smoke/input/PInvokeLite/Program.cs`
- 修改：`tests/smoke/input/HostEmbeddingLite/Program.cs`
- 创建：`tests/smoke/input/PInvokeLite/README.md`
- 创建：`tests/smoke/input/HostEmbeddingLite/README.md`

- [ ] **步骤 1：让 `PInvokeLite` 聚焦 interop 主线**

至少覆盖：
- `DllImport`
- native export
- 符号解析
- 基础 marshalling

- [ ] **步骤 2：把平台能力与 host 流收进 `HostEmbeddingLite`**

至少覆盖：
- host lifecycle
- thread attach / detach
- managed entry
- delegate / callback
- 文件系统
- 时间 / 时区
- 动态库装载
- registration 流
- metadata/bootstrap 状态机

- [ ] **步骤 3：在 `HostEmbeddingLite` README 中写清四段 bootstrap 状态机**

必须写清：
- registration complete
- metadata cache initialize
- GC-dependent late-init
- class world / reflection cache warmup

- [ ] **步骤 4：把最后一段 warmup 的 lazy 行为与 trace 依赖写清**

说明：
- `class world / reflection / generic / delegate` 为 lazy warmup
- 必须通过 trace contract 验证触发顺序

### 任务 4：构建全部 smoke 项目

**文件：**
- 测试：`tests/smoke/input/HelloWorld/HelloWorld.csproj`
- 测试：`tests/smoke/input/GenericEcho/GenericEcho.csproj`
- 测试：`tests/smoke/input/ReflectionLite/ReflectionLite.csproj`
- 测试：`tests/smoke/input/PInvokeLite/PInvokeLite.csproj`
- 测试：`tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj`

- [ ] **步骤 1：逐个构建 5 个项目**

运行：
```powershell
dotnet build tests/smoke/input/HelloWorld/HelloWorld.csproj -c Release
dotnet build tests/smoke/input/GenericEcho/GenericEcho.csproj -c Release
dotnet build tests/smoke/input/ReflectionLite/ReflectionLite.csproj -c Release
dotnet build tests/smoke/input/PInvokeLite/PInvokeLite.csproj -c Release
dotnet build tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj -c Release
```

预期：5 个项目全部 PASS。

- [ ] **步骤 2：把输出目录对齐到统一约定**

运行：
```powershell
Get-ChildItem tests/smoke/input -Recurse -Filter *.dll
```

预期：输出位置符合 `Directory.Build.props` 约定。

- [ ] **步骤 3：提交当前 chunk**

运行：
```bash
git add tests/smoke
git commit -m "feat: add roadmap0 smoke input projects"
```

预期：提交只包含 smoke 项目和对应 README / props。
