# Roadmap: IL2CPP Closure 精度重构

## 目标

解决 `--full-closure` 模式下 Loader 扫描 NuGet 测试框架 DLL 导致 AOT IR 膨胀（49862 methods → 期望 ~3000）、生成代码 1.5GB、构建 6min+ 的问题。建立从"扫所有 DLL"到"精确 root 闭包"的渐进式 closure 管理体系。

## 范围边界

- TPG 侧的 CodegenOrchestrator 与 ConvertToCppHandler 的 assembly-dir 管理
- Loader 的 closure 计算策略
- pipeline-config.yaml 的 assemblyDirs 配置
- 22 个 foundation-dll chunk 的 pipeline 适配
- 200+DLL 游戏引擎场景的 closure 扩展性

## 非目标

- 不改动 AOT IR 或 NativeAot 的代码生成逻辑
- 不改动 runtime-core 的执行引擎
- 不改动 BRIDGE-AOT 的 callee 级编译（cloure 变小后自然变快）

---

## 阶段列表

### Phase D5: Quick Win — 限制 assembly-dir 范围

- **调度模式**: sequential
- **goal**: 通过限制 CodegenOrchestrator 的 assembly-dir 为 runtime 目录，立即消除 numerics chunk 的 closure 膨胀
- **exit_criteria**: numerics chunk pipeline 从 6min 降到 <1min，AOT IR 从 49862 降到 ~3000
- **deliverables**:
  - CodegenOrchestrator.cs 的 probeDirs 改为仅 runtime 目录
  - BridgeAotCompiler.BuildGenericDemandLookup 统一为循环去重（一致性修复）
  - numerics chunk build+fact 验证通过
- **dependencies**: 无
- **resolved_decisions**:
  - assembly-dir 仅指向 System.Private.CoreLib 的 runtime 目录
  - 游戏引擎 DLL 等非 runtime 程序集暂不处理（D1 阶段解决）
- **watch_items**:
  - 其他 21 个 chunk 是否因 assembly-dir 缩小而产生额外问题
  - chunk 依赖的 assembly 不在 runtime 目录时的 fallback 行为

### Phase D1: Config 化 Assembly 目录管理

- **调度模式**: sequential
- **goal**: 在 pipeline-config.yaml 中引入 assemblyDirs 配置，支持显式指定多个 assembly 搜索目录，替代 codegen 层的硬编码 probeDirs
- **exit_criteria**: 所有 22 个 chunk 使用新的 assemblyDirs 配置运行，且 closure 不膨胀
- **deliverables**:
  - pipeline-config.yaml 增加 assemblyDirs 字段
  - ChunkContext 增加 assembly_dirs 属性
  - CodegenOrchestrator 读取 assembly dirs 配置而非硬编码
  - 兼容 fallback：当 assemblyDirs 未配置时默认使用 runtime 目录
- **dependencies**: D5
- **resolved_decisions**:
  - assemblyDirs 是配置驱动的白名单，不是黑名单
  - 默认值：runtime 目录（与 D5 一致）
  - 每个 chunk 可以有自己的 assemblyDirs 覆盖（适用于游戏引擎特定 DLL）
- **watch_items**:
  - 配置与实际环境（CI/本地）的路径一致性
  - 新增 chunk 时默认配置是否足够

### Phase D2: Subject-Rooted Call-Graph Closure

- **调度模式**: sequential
- **goal**: 改造 Loader 的 closure 计算策略：从"加载所有 assembly 中所有方法"改为"只加载从 subject wrapper 实际可达的方法"，实现零配置的精确 closure
- **exit_criteria**: 所有 chunk 的 AOT IR methods ≈ 实际可达方法的 1.1x（不超过 10% 的 over-approximation），无需任何白名单
- **deliverables**:
  - LoaderStage 新增 `ComputeSubjectRootedClosure()` 算法
  - 入口：subject wrapper 的 IL instructions 中的 callee SubjectIds
  - 传播：从 callee 的 IL instructions 递归收集更多 callee
  - 终止：已处理的 method / 接口方法 / P/Invoke / 已知 runtime service
  - 与现有 Loader 的 `--full-closure` 模式并存（通过 flag 切换）
  - 处理反射调用（Type.GetMethod / Invoke）——以 metadata 注册替代 codegen
  - 处理泛型特化——从 GenericInstantiationDemandGraph 提取
- **dependencies**: D1
- **resolved_decisions**:
  - 算法是 over-approximation（保守地多包含），不是 exact call graph
  - 接口方法引用所有已知实现（而非推测运行时 dispatch）
  - 反射调用：只包含 metadata 注册，不生成 C++ 代码
- **watch_items**:
  - 复杂泛型场景（多层嵌套、协变、逆变）是否被正确包含
  - 反射调用遗漏导致的运行时 TypeLoadException

### Phase Wiki: 方案记录

- **调度模式**: sequential
- **goal**: 将 closure 精度重构的设计、配置、运行机制写入 wiki
- **exit_criteria**: wiki 文档通过 review
- **deliverables**:
  - wiki 文档：closure-architecture.md
    - --full-closure vs subject-rooted 模式对比
    - assemblyDirs 配置说明
    - Loader closure 计算算法说明
    - Subject-Rooted Closure 算法设计
  - wiki 更新记录
- **dependencies**: D2（但可 D5/D1 完成后就开始写）
- **watch_items**: 无

---

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|-----------|----------|----------------|-----------------|
| T-D5-1 | D5 | ready | main | 修复 CodegenOrchestrator 的 assembly-dir | - | - | `src/managed/Chaos.IL2CPP.Generator/src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/BridgeAotCompiler.cs`, `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Codegen/` | 10min |
| T-D5-2 | D5 | ready | main | 统一 BuildGenericDemandLookup 去重 | T-D5-1 | - | `src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/BridgeAotCompiler.cs`, `src/managed/Chaos.IL2CPP.Generator/AotCoreIrLowering.cs` | 10min |
| T-D5-3 | D5 | ready | main | 验证 numerics chunk build+fact 通过 | T-D5-2 | - | `testing/foundation-dll/System.Private.CoreLib/chunks/numerics/` | 10min |
| T-D1-1 | D1 | planned | main | pipeline-config.yaml 增加 assemblyDirs 配置 | T-D5-3 | - | `testing/foundation-dll/config/pipeline-config.yaml`, `testing/foundation-dll/verification/orchestration/` | 2h |
| T-D1-2 | D1 | planned | main | CodegenOrchestrator 读取 assemblyDirs 配置 | T-D1-1 | - | `src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Codegen/` | 1h |
| T-D1-3 | D1 | planned | main | 22 个 chunk 适配新配置 + 验证 | T-D1-2 | - | `testing/foundation-dll/` | 4h |
| T-D2-1 | D2 | planned | main | Loader 新增 Subject-Rooted Closure 算法 | T-D1-3 | - | `src/managed/Chaos.IL2CPP.Loader/` | 2w |
| T-D2-2 | D2 | planned | main | 反射调用处理 | T-D2-1 | - | `src/managed/Chaos.IL2CPP.Loader/` | 3d |
| T-D2-3 | D2 | planned | main | 泛型特化处理 | T-D2-1 | - | `src/managed/Chaos.IL2CPP.Loader/` | 3d |
| T-D2-4 | D2 | planned | main | 全量 chunk 验证 + bug fix | T-D2-2, T-D2-3 | - | `testing/foundation-dll/` | 5d |
| T-W-1 | Wiki | planned | main | closure-architecture.md 方案文档 | T-D1-3 (可提前) | - | `wiki/` | 1d |

---

## 依赖图

```
T-D5-1 → T-D5-2 → T-D5-3
                      ↓
                    T-D1-1 → T-D1-2 → T-D1-3
                                        ↓
                                      T-D2-1 → T-D2-2 → T-D2-4
                                        → T-D2-3 ↗
                                        ↓
                                      T-W-1 (可提早在 T-D1-3 后)
```

---

## 风险

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|---------|
| D5 缩小 assembly-dir 后某些 chunk 依赖的 assembly 不在 runtime 目录 | 中 | 高（link error） | D1 引入配置化后解决；D5 阶段通过完整 pipeline 验证 |
| Subject-Rooted Closure 遗漏反射调用的方法 | 中 | 高（运行时 crash） | 反射调用走 metadata-only 路径，不排除；保留 --full-closure 作为 fallback |
| 泛型特化闭包计算不完整 | 中 | 中 | 从 GenericInstantiationDemandGraph 提取已知特化列表 |
| 某些 subject wrapper 通过 delegate 间接调用 | 低 | 中 | delegate 创建点的 IL 包含 target method 的 ldftn 指令，可被 callee 收集到 |
| 关闭 D5 但没更新其他 21 个 chunk 的 metadata cache | 中 | 低 | build 阶段自动重建 |

---

## 备选路径

1. **只用 D5，放弃 D1/D2** ——适合短期缓解，但 200+DLL 游戏引擎场景无法扩展
2. **跳过 D5/D1，直接做 D2** ——Subject-Rooted Closure 本身就解决了 assembly-dir 问题，但实施周期 2-4 周太长，期间 numerics 不可用
3. **只用 D1 不做 D2** ——配置化 assemblyDirs 加上自动化推导（从 chunk 命名空间 → assembly 列表），可覆盖 90% 场景，实施成本 2-3 天。但遇到非标准 assembly 布局时仍需手动配置
4. **D2 换方案：Assembly Metadata 标注** ——用 `[RuntimeAssembly]` attribute 标记程序集类型，成本极高（需要改所有 runtime assembly 源码）

---

## 当前建议推进顺序

```
D5（今天立即做，修复 numerics 阻塞）
  → D1（本周完成，建立配置框架）
    → Wiki（D1 完成后开始写，与 D2 并行）
      → D2（长期，精益 closure 算法）
```

## 子任务执行策略

- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `recommended_next_child`: T-D5-1
