# 27 — 外部运行时表 Null 检查架构限制

> 来源：`MEMORY.md > external-runtime-null-check-architecture-limit.md`

## 问题

System.Runtime.Serialization.Formatters (6 个失败) + System.Collections.Immutable (10 个失败) 共 16 个跨程序集调用 failures。全部根因：非主程序集方法不在 linkedWorld.Methods 中 → callvirt → ExternalRuntime → CHAOS_IL2CPP_FAIL()。

## 已尝试路径

| 修复 | 结果 | 原因 |
|------|------|------|
| assemblyDirs + DLL 放入 subjects 目录 | ❌ | 引入 FSharp 传递依赖，C++ 类型重定义 |
| YAML parser 修复 | ✅ | 配置正确解析，但 assemblyDirs 不能用 |
| EmitLinearCall null check | ✅ +3 fix | 修复了使用 call 的 instance 方法 |
| EmitExternalRuntimeTableDispatch callvirt null check | ❌ | 代码路径未到达 — 走 devirtualization 路径 |
| internalAssemblyNames 修复 | ✅ | 需要 Forms 方法在 linkedWorld.Methods 才能生效 |
| DirectNativeSymbol 存根 (18 个) | ✅ | 对 runtime-interop 类有效 |
| 修改 catch-all stub 返回 0 | ❌ | 测试需要 NullRefException，不是返回值 |

## 架构限制

`LoadStage.LoadMultiple()` 在 `ProjectCrossAssemblyMethodInstantiations` 后可能丢弃附加程序集的方法。`LinkerStage.Link()` 的 `ComputeReachableClosure` early return 在 `FullAssemblyClosure=true` 时不包含所有 loadedAssembly 的方法。

## 修复方向

1. 在 LoaderStage 确认投影后 Forms 方法是否仍存在
2. 修改 Linker 的 ComputeReachableClosure 确保 FullAssemblyClosure=true 时包含所有 loadedAssembly 的方法
3. 如果不存在，修改 LoaderStage 确保附加程序集方法在投影后保留

## TRIGGER

涉及跨程序集调用失败、callvirt 分派到未注册方法、或 linkedWorld.Methods 不包含非主程序集方法时。
