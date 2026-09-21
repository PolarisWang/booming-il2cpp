# dispatch_overhead 性能根因调查 — STATUS（已结案：非缺陷）

> 起始：2026-09-11 | 结案：2026-09-11 | 关联：ENG-34914 报告中发现的 P0 性能问题

## 结论：这不是 codegen 缺陷

**`dispatch_overhead` 标记的 1597 个方法中，SIMD intrinsic 部分（~1346 个）实际上已经被正确 inline 展开。**
之前的"inline shape 前缀不匹配导致全部落入解释器"判断是**错误的**，源自被陈旧二进制污染的对比实验。

## 真实成因

`ChaosExternalRuntimeFallback` 的软件 SIMD 实现（`vector_fixed_templates.h` 的 `VectorFixedHypot` 等）
逐元素调用 `std::hypot` 等标量函数，而 .NET 8 使用硬件 SIMD 指令（`vhypotpd` / `vhaddpd`）。
**软件仿真 vs 硬件指令相差 50-200x 是物理预期，不是 bug。**

Sassafras 注释 `interop_stubs.cpp:684` 已说明：
> Real hardware-intrinsic SIMD execution is tracked by codegen Track B

即：硬件 SIMD 是**已知的未实现工作项（Track B）**，不是缺陷。

## 关键教训（重要）

### 🔴 陈旧 TPG 二进制会污染一切 codegen 实验

`src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Release/net8.0/Chaos.IL2CPP.Generator.dll`
捆绑的是**构建时的** Generator 副本。修改 Generator 源码后，**必须重建 TPG** 才能生效：

```bash
dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Chaos.IL2CPP.Tools.TestProjectGenerator.csproj -c Release
```

这与 memory 中 `chaos-il2cpp-tool-reinstall-required` 记录一致。

### 🔴 生成的代码分布在多个 page 文件中

`native-aot.generated.cpp` **不是全部**。inline 表达式实际在
`native-aot.generated.page3.cpp` 等分页文件中（`page-0006.cpp` ... `page-NNNN.cpp`）。

**只 grep 主文件会得到 0 结果，从而误判为"没有生成"。** 正确做法：

```bash
grep -l "vector_fixed" artifacts/<run>/codegen/generated/*.cpp   # 用 -l 遍历所有分页
```

## 已证伪假设（勿重复）

### ❌ 假设 A：TypeDisplayNamePrefix 缺少命名空间

**曾做**：将 `RuntimeHelperShapeRegistry.CoreStubs.Part2.S3.cs` 中 30 处
`TypeDisplayNamePrefix: "Vector128"` 改为 `"System.Runtime.Intrinsics.Vector128"`（及 Vector64/256/512）

**A/B 实测结果**：

| 前缀形式 | 含 `vector_fixed` 的生成文件数 |
|---------|------------------------------|
| `"Vector128"`（原始） | **18 / 20** ✅ |
| `"System.Runtime.Intrinsics.Vector128"`（"修复"） | **0 / 20** ❌ |

**证伪依据**：`GetTypeDisplayNameFromSubjectId`（`RuntimeHelperShapeRegistry.CoreStubs.Part1.cs:110-116`）
对 `"System.Runtime.Intrinsics/Vector128::Hypot:..."` 返回的是 **`"Vector128"`**（去掉 assembly 前缀 + `/` 后）
→ 原始裸前缀才是正确的匹配形式，"修复"反而会破坏匹配。

**状态**：已全部 `git checkout` 回退，工作树干净。

### ❌ 假设 B：inline shape 匹配失败导致解释器路径

**曾判**：调试输出显示 resolver 返回 OK，但主文件 grep 不到 VectorFixed → 误判为"下游覆盖"

**证伪**：实际生成在 page3.cpp（134 处 `vector_fixed`），主文件确实没有——是**搜索方法错误**，不是代码问题。

## 复现验证方法（已执行且通过）

```bash
# 1. 重建 TPG（关键，否则用旧 DLL）
dotnet build src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/Chaos.IL2CPP.Tools.TestProjectGenerator.csproj -c Release

# 2. 跑 codegen
dotnet src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Release/net8.0/Chaos.IL2CPP.Tools.TestProjectGenerator.dll \
  generate-dll \
  --dll tests/e2e/translation/System.Private.CoreLib/chunks/runtime-intrinsics/managed/CombinedSubjects.dll \
  --metadata tests/e2e/translation/System.Private.CoreLib/chunks/runtime-intrinsics/managed/subjects/subjects.metadata.json \
  --output artifacts/ri-check

# 3. 验证 inline 生效（必须遍历所有分页）
grep -l "vector_fixed" artifacts/ri-check/codegen/generated/*.cpp | wc -l   # 预期 ~18
grep -c "vector_fixed" artifacts/ri-check/codegen/generated/native-aot.generated.page3.cpp  # 预期 >100
```

**实测结果**：18/20 文件含 `vector_fixed`，page3.cpp 有 134 处。

## 后续建议

若要真正缩小 SIMD 性能差距，需要的是 **Track B（硬件 SIMD 代码生成）**，而非修 inline shape。
这是既有的、已识别的工作项。建议：

1. 把本结论同步到 ENG-34914 报告（修正"dispatch_overhead 是唯一根因"的表述）
2. Track B 单独立项，不要在 dispatch_overhead 上继续投入

## 已清理

- 所有临时实验产物：`artifacts/{ri-verify,ri-verify2,ri-dbg,ab-before,ab-after}` 已删除
- 所有源码改动已 `git checkout` 回退，工作树干净
