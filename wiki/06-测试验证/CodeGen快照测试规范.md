# CodeGen 快照测试规范

> 面向 NativeAotEmitter 代码生成逻辑的增量回归测试体系。通过 IR 夹具驱动 emitter 生成 C++ 输出，与基线文件比对，确保生成逻辑变更的影响面可审查、可追溯。

## 1. 定义

快照测试（Snapshot Test）是一种增量回归测试方法：

- **输入**：预置的 AOT Core IR 夹具（JSON artifact 集合），模拟 `convert-to-cpp` 管线的产出
- **执行**：直接调用 `NativeAotEmitter.Generate()`，走完整 emission 路径
- **对比**：生成的 C++ 输出与 Git 追踪的基线文件逐文件比对
- **更新**：确认变更无误后，通过 `SNAPSHOT_UPDATE=1` 模式更新基线

### 核心原则

- **不 Mock** — 调用真实的 emitter，保证测试覆盖真实 emission 路径
- **全量快照** — 对比完整 C++ 输出，而非 hash 或抽样，确保 `git diff` 可直接审查
- **基线即文档** — 基线的 diff 就是 codegen 影响面的精确描述

### 与 Foundation DLL 验证的关系

两个体系互补，形成快/慢双反馈环：

| 维度 | 快照测试 | Foundation DLL 验证 |
|------|---------|-------------------|
| 反馈速度 | 秒级（纯 C#，无编译） | 分钟级（需原生编译+运行） |
| 覆盖范围 | IL 模式 → C++ 形状 | 语义正确性 + 运行时行为 |
| 运行时机 | 每次 codegen 修改后 | PR 合并前 / nightly |
| 定位 | 防非预期变更，快速迭代 | 最终语义正确性验证 |

快照测试**不替代** foundation-dll 验证。codegen 改动在快照测试通过后，仍须走完 foundation-dll 全管线才能声称完成。

## 2. 项目结构

```
tests/snapshots/
├── Chaos.IL2CPP.CodeGen.SnapshotTests/
│   ├── Chaos.IL2CPP.CodeGen.SnapshotTests.csproj
│   ├── SnapshotTestBase.cs              # 基类：加载夹具 → emitter → 对比基线
│   ├── SnapshotTheoryData.cs            # 自动发现所有 fixture 目录
│   ├── Infrastructure/
│   │   └── TempFixtureHost.cs           # 从 fixture 目录加载 → 写临时目录
│   ├── Baselines/                       # 基线输出（Git 追踪）
│   │   ├── 01-simple-add/
│   │   │   └── generated/
│   │   │       ├── native-aot.generated.cpp
│   │   │       └── runtime_helper_shapes.h
│   │   └── ...
│   └── Fixtures/                        # IR 夹具（Git 追踪）
│       ├── 01-simple-add/
│       │   ├── native-aot.lowering-plan.json
│       │   ├── aot-core-ir.json
│       │   ├── closure.manifest.json
│       │   ├── metadata-registration.json
│       │   └── supplemental-metadata-template.json
│       └── ...
```

## 3. 夹具规范

### 3.1 所需文件

每个夹具目录包含 emitter 所需的 5 个 JSON artifact 文件：

| 文件 | 内容 | 说明 |
|------|------|------|
| `native-aot.lowering-plan.json` | Lowering 计划 | PlanKind、EntrySubjectId、WorkloadAbi 等 |
| `aot-core-ir.json` | 目标方法的 IR 指令 | 包含 1-3 个方法的 instruction 序列 |
| `closure.manifest.json` | 闭包清单 | 最小可用版本 |
| `metadata-registration.json` | 元数据注册 | 最小可用版本 |
| `supplemental-metadata-template.json` | 补充元数据模板 | 可为 `{}` |

### 3.2 精简原则

- 一个夹具只覆盖 **1-3 种 IL 模式**的组合
- 一个夹具只包含 **1-3 个方法**，过多会降低可读性
- 被调用的外部方法尽量自包含在夹具 IR 中，减少外部依赖
- 优先从真实编译产物提取后裁剪，确保 JSON 语法和语义正确

### 3.3 生成方法

**方法 A — 从真实编译提取（推荐）：**

```powershell
# 1. 编写包含目标 IL 模式的最小 C# 程序
# 2. 运行 convert-to-cpp
chaos-il2cpp convert-to-cpp --assembly TestSubject.dll --output artifacts/test-subject/

# 3. 从 artifacts/test-subject/ 复制并精简到 Fixtures/
#    保留 5 个必需 JSON 文件，裁剪 aot-core-ir.json 只保留目标方法
```

**方法 B — 手工编写（仅限极端/边缘场景）：**

直接构造 `AotCoreIrArtifact` 的 JSON，适用于测试特定 lowering 路径。注意保持与真实 IR 格式一致。

## 4. 测试执行

### 4.1 运行快照测试

```powershell
dotnet test tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/
```

### 4.2 更新基线

确认生成输出正确后：

```powershell
$env:SNAPSHOT_UPDATE = "1"
dotnet test tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/
```

然后通过 `git diff` 审查基线变化，确认无误后提交。

### 4.3 基线审查

基线变更审查要点：

- 是否产生了非预期的 C++ 结构变化？
- 是否引入了非预期的运行时 helper 调用？
- 是否影响到了不相关的 IL 模式（表明 lowering 逻辑有副作用）？
- 性能关键路径的生成代码是否有退化？

## 5. 开发工作流集成

### 5.1 新增 IL 模式

```
[1. 新增 IL 模式翻译逻辑]
    ↓
[2. 编写夹具 IR JSON]
    ↓
[3. 运行快照测试 → 失败（因为 emitter 尚不支持新模式）]
    ↓
[4. 实现 lowering 逻辑]
    ↓
[5. 运行快照测试 → 确认输出正确]
    ↓
[6. SNAPSHOT_UPDATE=1 生成基线]
    ↓
[7. git diff 审查 C++ 输出]
    ↓
[8. 确认后提交（夹具 + 基线 + 实现代码）]
```

### 5.2 修改现有生成逻辑

```
[1. 修改 lowering / emission 代码]
    ↓
[2. 运行快照测试 → 基线差异提示失败]
    ↓
[3. git diff 审查每个基线的变化]
    ├─ 变化符合预期 → SNAPSHOT_UPDATE=1 更新基线
    └─ 变化不符合预期 → 修复代码
```

### 5.3 触发条件

以下改动在实现前必须先运行快照测试：

- 修改 `NativeAotLoweringPlanner` 的任何 Emission / Planning 文件
- 修改 `NativeAotEmitter` 核心逻辑
- 修改 RuntimeSupport 文件且影响生成 C++ 形状
- 新增或修改 Scriban 模板

以下改动不需要运行快照测试：

- 纯 C++ runtime 实现改动（不涉及生成逻辑）
- 仅修改测试框架、构建脚本、文档
- 修改非 codegen 管线（如 linker、loader、metadata writer）

## 6. 夹具清单维护

### 6.1 初始覆盖目标

按 complexity 递增，逐步覆盖主流 IL 模式组合：

| 优先级 | 模式 | 说明 |
|--------|------|------|
| P0 | 算术运算、ldstr、静态调用、字段读写 | 最简基础模式 |
| P0 | 条件分支、switch | 控制流 |
| P1 | newobj、callvirt、callinterface | 对象/虚方法 |
| P1 | try-finally、try-catch | 异常处理 |
| P1 | box、unbox、isinst、castclass | 类型操作 |
| P2 | 泛型静态方法、constrained call | 泛型/约束 |
| P2 | ldtoken、refanytype | 运行时句柄 |
| P3 | tailcall、delegate、ldftn | 高级模式 |

### 6.2 新增夹具评审

新增夹具须满足：

- 有对应的最小 C# 源文件证明语义来源（放在 fixture 目录的 `source.cs` 中）
- 夹具 JSON 是通过真实编译提取 + 裁剪得到，或经过手工验证
- 夹具覆盖的 IL 模式在当前路径参考表中有对应条目
- 基线首次生成后经过人工审查

## 7. 常见问题

### Q: 基线文件变大了怎么办？

如果夹具方法数量没变但基线变大，说明 emitter 产生了额外的输出结构。审查这些额外结构是否合理。

### Q: 夹具 JSON 与真实 IR 脱节怎么办？

定期从真实 foundation-dll 编译产物中提取新夹具，确保夹具反映最新的 IR 格式。如果 artifact schema 升级（如 FormatVersion 变更），需要更新所有夹具。

### Q: 行尾差异导致测试失败？

测试基类应归一化行尾（CRLF → LF）后再对比。

### Q: emitter 重构导致大面积基线变更？

这是快照测试的核心价值——暴露影响面。逐个审查每个 fixture 的 diff，确认变更合理后统一更新基线。
