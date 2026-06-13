# STATUS — Profile Build Fix: entry.exe --profile AOT GC data

## 元信息

```yaml
phase: planned
task_id: profile-build-fix
created: 2026-06-08
source: foundation-dll profile pipeline 评估 → C2712/C2713 fix → --profile segfault
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: false
```

## 问题

`entry.exe --profile` 在 check build 下 segfault，无法采集 AOT 侧 GC pause / alloc bytes / fast path rate 数据。需要 profile build 才能使 profile_stats 的真实采集函数生效。

## 修复步骤

### Step 1: 修复 TPG 编译 —— CS8820 error

```csharp
// src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.cs:4147
// error CS8820: A static anonymous function cannot contain a reference to 'sym'.
// 修复: 移除 lambda 的 static 关键字，或捕获 sym 为非 static
// 当前: static anonymous function 引用局部变量 sym
```

**文件**: `RuntimeHelperShapeRegistry.cs` line 4147
**问题**: C# 编译器更新后禁止 static lambda 引用局部变量 `sym`
**修复**: 将 lambda 前的 `static` 移除，或将 `sym` 作为参数传入

### Step 2: 重建 TPG DLL

```bash
cd src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator
dotnet build --nologo -v q
```

确认 `bin/Debug/net8.0/Chaos.IL2CPP.Tools.TestProjectGenerator.dll` 包含：
- `SdkEmitter.cs` 修改（vector_fixed_templates.h 不再复制到 codegen/include/）
- `TestProject.CMakeLists.txt.scriban` 修改（eager_compile.cpp 条件编译）
- `TestProject.RuntimeEntry.cpp.scriban` 修改（FactDispatchWorker + RunProfileMode）

### Step 3: 构建 chaos_runtime_core.lib (profile build)

```bash
cd D:/agent/booming-il2cpp
cmake --preset profile
cmake --build artifacts/presets/profile --target chaos_runtime_core --config RelWithDebInfo
```

确保 `build_release_tmp/` 中的 lib 包含：
- `profile_stats.cpp` 编译（CHAOS_IL2CPP_PROFILE_ENABLED=1）
- `profile_globals.cpp` 编译（CHAOS_IL2CPP_PROFILE_ENABLED=1）

复制到 SDK 参考预设:
```bash
cp artifacts/presets/profile/src/native/runtime-core/RelWithDebInfo/chaos_runtime_core.lib \
   artifacts/presets/windows-x64-reference/src/native/runtime-core/RelWithDebInfo/
```

### Step 4: 构建 entry.exe (profile build)

```bash
cd testing/foundation-dll
python -m verification.chunk_pipeline \
  --assembly System.Private.CoreLib \
  --chunk buffers \
  --stages build \
  --native-config profile
```

验证 `entry.exe --profile` 输出 JSON:
```json
{"profile":[{"methodIndex":0,"gcPauseNs":1250000,"nurseryAllocBytes":4096, ...}]}
```

### Step 5: 运行完整 profile 管线

```bash
python -m verification.chunk_pipeline \
  --assembly System.Private.CoreLib \
  --chunk buffers \
  --stages build,fact,profile,managed_benchmark,benchmark_report \
  --native-config profile
```

验证:
- profile.json 包含 `gcPauseNs` / `nurseryAllocBytes` / `fastPathCount` / `slowPathCount`
- comparison.json 包含 `gcComparison` 节（chaosAotAllocBytes 不再为 null）
- gcComparison 输出 "Top-5 high allocation methods"

## 预期风险

| 风险 | 影响 | 缓解 |
|------|------|------|
| SdkEmitter 的 vector_fixed_templates.h 移除导致其他 chunk 构建失败 | 中 | 保留 header 在 src 目录，只阻止 copy 到 SDK include |
| profile build 下 entry.exe 链接不同 lib 版本导致 ABI 不兼容 | 高 | 确保 check/profile 使用同一套工具链版本 |
| dispatch segfault 在 profile build 下仍然存在 | 中 | __try/__except 已覆盖 kSubjectSlotMap + dispatch + Flush |
| profile build 耗时更长（`CHAOS_IL2CPP_PROFILE_ENABLED=1`的开销） | 低 | profile stage 默认不启用 |

## 完成定义

```yaml
exit_criteria:
  - entry.exe --profile 输出有效 JSON（非空 profile 数组）
  - comparison.json 的 gcComparison 节包含 chaosAotAllocBytes
  - profile.json + benchmark-history.jsonl 可被 profile_compare.py 消费
  - 全量 pipeline (build+fact+profile+managed_benchmark+benchmark_report) 通过
```
