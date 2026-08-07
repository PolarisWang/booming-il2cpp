# IL2CPP Closure 精度架构

## 动机

`--full-closure` 模式下，ConvertToCppHandler 会扫描 `--assembly-dir` 中的所有 DLL 并加入附加加载列表。当 `--assembly-dir` 指向 subjects DLL 的构建输出目录时，NuGet 传递依赖（FSharp.Core、xunit、Newtonsoft.Json 等）被引入 closure，导致 AOT IR 从预期的 ~3,000 methods 膨胀到 50,000+，生成 C++ 代码 1.5GB，构建时间 6min+，且 BRIDGE-AOT 尝试编译 23,000+ 个不可达的 callee。

## 架构

### D5：默认方案（无 --assembly-dir）

```
Subjects DLL (via --assembly)
  └── 主管线 (Loader → CodeGen)
       ├── 生成 subjects 的 C++ 代码
       └── BRIDGE-AOT 处理跨程序集 callee
            └── ResolveAssemblyPath 通过 TPA 发现 DLL
```

- **不传 `--assembly-dir`**：Loader 只加载 subjects DLL 本身
- **`--full-closure` 仍启用**：但只作用于已加载的程序集
- **BRIDGE-AOT 独立发现**：通过 TPA（Trusted Platform Assemblies）定位 runtime DLL
- **效果**：AOT IR 从 49,862 → ~350 methods

### D1：config 化的 assemblyDirs

当 subjects 需要引用非 runtime 目录中的程序集（如游戏引擎 DLL）时，通过 `pipeline-config.yaml` 配置：

```yaml
pipeline:
  chunks:
    my-chunk:
      assemblyDirs: "../game-engine-dlls"
```

配置的目录会作为 `--assembly-dir` 传递给 codegen。

### D2：Subject-Rooted Closure（设计，未实现）

更精益的方案：从 subject wrapper 方法的 IL 指令中提取 callee SubjectId，只加载那些 assembly。已在 `CodegenOrchestrator.cs` 中实现了 `SubjectAssemblyResolver` 作为原型，但因 closure 校验失败搁置。完整实现需要在 Loader 层级进行 2-4 周的改造。

## 配置参考

`pipeline-config.yaml` 位于 `tests/e2e/translation/config/`：

```yaml
pipeline:
  defaultMode: standard
  chunks:
    numerics:
      assemblyDirs: ""  # 空 = 不加额外 assembly-dir（D5 默认）
    my-chunk:
      assemblyDirs: "path/to/dlls"  # 分号分隔多个目录
  timeouts:
    codegen_aot: 600
    fact_aot: 600
    benchmark: 600
```

## 关键文件

| 文件 | 作用 |
|------|------|
| `CodegenOrchestrator.cs` | 控制是否传递 `--assembly-dir` |
| `ConvertToCppHandler.cs` | 扫描 `--assembly-dir` 收集依赖 DLL |
| `build.py` | 从 pipeline-config.yaml 读取 assemblyDirs |
| `chunk_pipeline.py` | 解析配置并注入 ChunkContext |
| `pipeline-config.yaml` | 中心配置 |

## 相关文档
- [32-byte ABI 修复](20-32byte-abi-fix.md) — Vector<T> pass-by-reference 及 opcode 归一化

## 性能数据

| 指标 | 修复前 | 修复后 (D5) |
|------|--------|-------------|
| AOT IR methods | 49,862 | 352 |
| 生成 C++ 文件 | 726 | 9 |
| 构建时间 | 6min+ | ~37s |
| BRIDGE-AOT | 313 OK, 23,335 FAIL | 72 OK, 11 FAIL |
| Fact 通过率 | 110/121 | 112/121 |
