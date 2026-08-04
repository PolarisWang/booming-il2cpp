# Bridge 重构设计方案 — B1+B3 → C

## 1. C2733 根因定位

```
Subject 方法: call Deserialize<JsonDocument>(stream, options)
             → CollectExternalRuntimeDispatch 注册到表 (index=42)
             → ResolveDirectInvocationTarget 查到表 index=42
             → 用 table[index] 调用 → 无 stub 生成 ✅

Bridge 方法: call Deserialize<JsonDocument>(stream, options)
             → CollectExternalRuntimeDispatch 注册到表 (index=42，已存在)
             → ResolveDirectInvocationTarget 查到表 index=42
             → 用 table[index] 调用 → 无 stub 生成 ✅
             BUT: 如果 DispatchKindCode = None
             → ResolveDirectInvocationTarget 走不同路径
             → TryCreateExternalRuntimeHelperDefinition 生成 stub ❌
             → C2733（重复的 chaos_stub_definition_*）
```

**关键结论**：bridge 方法加到 `aotCoreIr.Methods` 后，只要它们的指令的 `DispatchKindCode` 被正确设为 `ExternalRuntime`，`CollectExternalRuntimeDispatchEntries` 会注册所有 callee 到运行时表，`ResolveDirectInvocationTarget` 走查表路径 → **不生成重复 stub → 无 C2733**。之前 C2733 是因为 dispatch kind 为 `None`，走了不同代码路径。

## 2. 实施步骤

### Step A: CompileAndIntegrate（恢复 + 修复）

修改 `BridgeAotCompiler.CompileBridgedMethods` → `CompileAndIntegrate`：
- Phase 1: 编译所有 bridge 方法，建立 redirectMap
- Phase 2: 对每个 bridge 方法的指令做 dispatch patching
  - callee 在 redirectMap 中 → `DispatchKindCode = Direct`
  - callee 不在 redirectMap 中 → `DispatchKindCode = ExternalRuntime`
- Phase 3: 把 bridge 方法加入 `aotCoreIr.Methods`

这样主管线自然处理 bridge 方法：callee 走 ExternalRuntime 查表，bridge-to-bridge 走 Direct。

### Step B: Scheme C — 回填 subject 指令

bridge 方法编译完成后，扫描所有 subject 方法，对指向 bridge callee 的 ExternalRuntime 指令做回填：
- `DispatchKindCode: ExternalRuntime → Direct`
- `TargetSymbol: chaos_external_X → chaos_bridge_X`

这样 subject→bridge 的调用链变成 Direct dispatch，彻底消除 ExternalRuntime 间接层。

Subject 方法里调用未 bridge 编译的 callee 仍然走 ExternalRuntime（保持不变）。

### Step C: 代码清理

删除/简化以下文件：
- `BridgeMethodBodyEmitter.cs` — 删除（独立发射器不再需要）
- `BridgeCompilationResult.cs` — 删除（不再需要）
- `FullAssemblyEmitter.GenerateBridgeRedirectCpp()` — 删除
- `FullAssemblyEmitter.GenerateBridgeRegisterCpp()` — 删除
- `chaos_runtime_host.h` — 恢复 FillExternalRuntimeStubs 原始状态
- `scriptan CMakeLists.txt` — 保持 bridge redirect 排除（不再生成这些文件）
- `chaos_stub_patch_loader.cpp` — 恢复原始状态

## 3. 文件变更清单

| 文件 | 改动 |
|:----|:-----|
| `BridgeAotCompiler.cs` | `CompileBridgedMethods` → `CompileAndIntegrate`（+dispatch patching） |
| `CodeGenStage.cs` | 调用 `CompileAndIntegrate`，bridge 方法合入 aotCoreIr |
| `IlBytecodeDecoder.cs` | 保持现有 type token 解析（合入后通过主管线使用） |
| `ConvertToCppHandler.cs` | 传递 BridgeRedirectMap |
| `FullAssemblyEmitter.cs` | 删除 bridge redirect/register 生成；Scheme C 回填逻辑 |
| `ManagedClosureArtifactModels.cs` | 保留 BridgeRedirectMap，删除 BridgeCompiledMethods |
| `chaos_runtime_host.h` | 恢复原始 FillExternalRuntimeStubs |
| `CppProjectEmitter.cs` | 删除 bridge redirect 文件删除逻辑 |
| `TestProject.CMakeLists.txt.scriban` | 删除 bridge redirect 排除（已由 CppProjectEmitter 处理） |
| `BridgeMethodBodyEmitter.cs` | **删除** |
| `BridgeCompilationResult.cs` | **删除** |

## 4. 三优先级权衡

| 优先级 | 影响 |
|:------|:-----|
| **P1 性能最优** | Scheme C 消除 ExternalRuntime → Direct dispatch，最高性能 |
| **P2 方案完美性** | 单一发射器，架构干净，无维护债务 |
| **P3 HotUpdate** | 保持现状（bridge 方法未涉及 hotupdate） |

## 5. 风险评估

| 风险 | 概率 | 影响 | 缓解 |
|:----|:----:|:----:|:-----|
| C2733 复发 | 低 | 高 | Step A 后立即验证 |
| bridge 方法导致主管线 emitter 崩溃 | 中 | 高 | 逐步添加（先加 10 个） |
| Scheme C 回填破坏 subject 调用链 | 中 | 中 | 验证所有 fact 通过 |
| bridge 方法数减少 | 低 | 低 | 从 241 回退到主管线支持 |
