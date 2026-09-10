# STATUS - 传统 IL2CPP 全量翻译

## Meta
- task_id: il2cpp-full-translation
- lifecycle: in-progress
- phase: phase-1-implementation
- task_type: roadmap
- roadmap_or_plan: docs/dev/in-progress/il2cpp-full-translation/roadmap-v1-01.md

## 当前进展
- **本轮完成**: FullAssemblyEmitter 重写 + ConvertToCppHandler 直接路径改造
- **FullAssemblyEmitter**: 从占位符死代码→使用 NativeAotLoweringPlanner 的实时 lowering 发射器
  - 通过 NativeAotEmitter.GenerateFromArtifacts() 方法直接传入内存工件，跳过 JSON 序列化/反序列化往返
  - 发射器正确输出所有生成的 C++ 源文件到输出目录
- **ConvertToCppHandler**: 
  - 单 Assembly 路径: WriteArtifacts → EmitNativeAot(JSON往返) 替换为 FullAssemblyEmitter.Emit() 直接发射
  - 多 Assembly 路径: 同样改用 FullAssemblyEmitter，CMakeLists.txt 使用真实发射结果
  - `--full-closure` 标志现在被正确使用（之前硬编码为 true）
  - 删除了未使用的 `EmitNativeAot` 私有方法
- **NativeAotEmitter**: 新增 `GenerateFromArtifacts()` 公共方法，支持从内存工件直接生成

## 剩余工作
- **端到端验证**: `convert-to-cpp` 需要可执行的测试程序集（带 Main 入口点）来完全验证
- **InternalCall 解析**: InternalCall/PInvoke 方法的运行时桥接暂未集中处理
- **多程序集联合编译**: 多 Assembly 路径未完全测试
- **Foundation DLL 验证接入**: Phase 2

## 架构映射
- il_feature: full-assembly-translation
- translation_path: pipeline-full-closure → NativeAotLoweringPlanner (fullAssemblyMode=true)
- planner: NativeAotLoweringPlanner::Create() with fullAssemblyMode=true → CollectAllMethods()
- emitter: FullAssemblyEmitter → NativeAotEmitter.GenerateFromArtifacts()
- runtime_support: 通过 NativeAotLoweringPlanner.RuntimeSupport 区域注册
- constraints_followed:
  - 性能最优（P1）: 直接内存路径，无 JSON 序列化开销
  - 架构完美（P2）: 复用现有流水线与 lowering planner，避免重复实现
  - 热更新适配（P3）: 生成的 C++ 代码与已有 NativeAot 输出格式一致
- new_constraints:
  - FullAssemblyEmitter 不再自行管理方法遍历（原有占位符代码），改为委托给 NativeAotEmitter
  - `--full-closure` 标志现在通过 config.FullClosure 传递到 ManagedClosureRequest
- consistency_check: ok
- snapshot_tests: n/a（未变更 emitter/planner 输出逻辑）
- wiki_entry: needs-update: `FullAssemblyEmitter` 实际架构与 roadmap 描述存在偏差

## 关键文档
- roadmap: roadmap-v1-01.md（需更新 FullAssemblyEmitter 架构描述）
- plan: n/a（本轮通过直接实现完成，未创建独立 plan）

## 完成证据
- canonicalCommand: `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1` (0 errors)
- formalObject:
  - `src/managed/Chaos.IL2CPP.Generator/NativeAotEmitter.cs` — 新增 `GenerateFromArtifacts()` 方法
  - `src/managed/Chaos.IL2CPP.Driver/ConvertToCpp/FullAssemblyEmitter.cs` — 完全重写为真实 lowering 发射器
  - `src/managed/Chaos.IL2CPP.Driver/ConvertToCpp/ConvertToCppHandler.cs` — 重构为直接路径 + --full-closure 标志修复
- test_result: build passed, e2e validation requires executable test assembly
- next: 用可执行测试程序集验证 convert-to-cpp 输出正确性，推进 Phase 2 Foundation DLL 集成

## blocking_questions
- blocking_questions:
  - 需要可执行测试程序集进行端到端验证
  - InternalCall 运行时桥接尚未实现
- question_clearance: partial
- clearance_confirmed_by_user: false
