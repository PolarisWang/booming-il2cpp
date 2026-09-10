# .patchdata 工具链

## 概述

`.patchdata` 是 Chaos.IL2CPP 热更新的核心二进制格式，封装了补丁程序集的 PE 元数据和 AotCoreIr IR，供运行时 `PatchLoader` 加载执行。

工具链提供三个核心 CLI 命令：

| 命令 | 功能 | 职责归属 |
|------|------|----------|
| `emit-patch-data` | 从 DLL 生成 .patchdata 二进制（需手动提供 AotCoreIr） | il2cpp 运行时核心 |
| `emit-patch-data dump` | 解析/查看 .patchdata 内容 | il2cpp 运行时核心 |
| `emit-patch-data full` | 从 DLL 一步生成 .patchdata（自动运行闭包管线生成 AotCoreIr） | il2cpp 运行时核心 |

**原则**：只将与 il2cpp 运行时直接相关的操作纳入 CLI。测试编排（入口点生成、IR 精简、验证运行）保留在 Python 脚本层。

---

## emit-patch-data — 生成（手动提供 IR）

### 用法

```
chaos-il2cpp emit-patch-data <patch-dll-path> <output-patchdata-path> [--aot-core-ir <path>]
```

### 参数

| 参数 | 必填 | 说明 |
|------|------|------|
| `patch-dll-path` | 是 | 补丁 DLL 路径（`dotnet build` 输出的 Library） |
| `output-patchdata-path` | 是 | 输出的 .patchdata 文件路径 |
| `--aot-core-ir` | 否 | 嵌入的 AotCoreIr JSON 路径（对应补丁的 aot-core-ir.json） |

### 流程

1. 读取 DLL 的 PE 元数据（`System.Reflection.Metadata`）
2. 两遍扫描：
   - **第 1 遍**：收集 StringHeap / BlobHeap / UserStringHeap
   - **第 2 遍**：构建 6 个元数据表（AssemblyRef、TypeRef、TypeDef、FieldDef、MethodDef、MemberRef）
3. 读取方法体原始 IL 字节（重建 ECMA 335 方法头）
4. 可选的 AotCoreIr JSON 嵌入（按方法名匹配）
5. 序列化为 .patchdata v2 二进制

### 端到端示例

```bash
# 1. 生成入口点 DLL（由新管线 verification pipeline 自动完成）
#    旧: python build/toolchains/run/testing/foundation_dll/family_entrypoint_generator.py ...
#    新: 由 testing/foundation-dll/verification/stages/codegen.py 自动编排
#    手动触发:
python -m verification.entry_points.cli <family-slug> --skip <stages>

# 2. 生成 .patchdata
chaos-il2cpp emit-patch-data \
    testing/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/entrypoint-patch/build-output/ConvertCharPatchEntry.dll \
    testing/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/patch/patchdata/convert-char.patchdata \
    --aot-core-ir testing/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/entrypoint-patch/closure-sp/analysis/aot-core-ir.json

# 3. 验证生成的 .patchdata
chaos-il2cpp emit-patch-data dump \
    testing/foundation-dll/System.Private.CoreLib/convert-char/il2cpp_dist/patch/patchdata/convert-char.patchdata
```

---

## emit-patch-data full — 一步生成（自动运行闭包管线）

### 用法

```
chaos-il2cpp emit-patch-data full <patch-dll-path> [output-patchdata-path]
```

### 参数

| 参数 | 必填 | 说明 |
|------|------|------|
| `patch-dll-path` | 是 | 补丁 DLL 路径（`dotnet build` 输出的 Library） |
| `output-patchdata-path` | 否 | 输出的 .patchdata 文件路径（默认将 `.dll` 替换为 `.patchdata`） |

### 流程

1. **闭包管线**：将 DLL 输入 `PipelinePlan`（Loader → SemanticWorld → Linker → MetadataWriter → CodeGen），生成包含 IR 的完整闭包分析结果
2. **AotCoreIr 序列化**：从管线结果中提取 `AotCoreIr` 对象并序列化为 JSON
3. **PatchDataExtractor**：与 `emit-patch-data` 相同的两遍扫描 + 方法体提取 + IR 嵌入
4. **清理**：删除临时管线输出目录

### 端到端示例

```bash
# 一步生成 .patchdata（无需手动准备 AotCoreIr）
chaos-il2cpp emit-patch-data full \
    managed/patch/build-output/ConvertCharPatchEntry.dll \
    dist/patch/patchdata/convert-char.patchdata

# 验证
chaos-il2cpp emit-patch-data dump \
    dist/patch/patchdata/convert-char.patchdata
```

### 与 emit-patch-data 的区别

| 方面 | `emit-patch-data` | `emit-patch-data full` |
|------|-------------------|------------------------|
| 输入 | DLL + 可选 AotCoreIr JSON | 仅 DLL |
| IR 来源 | 外部提供（`--aot-core-ir`） | 自动运行闭包管线生成 |
| 耗时 | 秒级（无管线开销） | 分钟级（运行完整 5 阶段管线） |
| 适用场景 | 调试/CI 中已有 AotCoreIr | 开发/集成中使用 DLL 即可 |
| 依赖 | 需管线产物（aot-core-ir.json） | 无外部依赖 |

---

## emit-patch-data dump — 解析/查看

### 用法

```
chaos-il2cpp emit-patch-data dump <patchdata-path>
```

### 输出内容

| 输出段 | 说明 |
|--------|------|
| Header | magic、version、file size、header_size |
| Heaps | String/Blob/UserString 各堆的大小和偏移 |
| Metadata tables | 7 张元数据表（AssemblyRef/TypeRef/TypeDef/FieldDef/MethodDef/MemberRef/StandaloneSig）的行数和大小 |
| Method bodies | 方法体总览（有多少方法有 body） |
| AotCoreIr section | 嵌入的 IR 方法数、JSON 大小、方法名列表 |
| Register IR section | v2+ 寄存器 IR 段状态（空/非空） |
| String heap sample | 前 20 个字符串（快速了解元数据内容） |
| TypeDef entries | 所有 TypeDef 类型的 token 和完整名称 |
| MethodDef entries | 所有 MethodDef 方法的 token、名称、body 大小 |

### 用途

- **开发调试**：确认 .patchdata 文件和预期一致
- **审计验证**：检查版本、方法数、IR 嵌入状态
- **问题排查**：快速查看元数据表是否完整，AotCoreIr 是否嵌入

---

## .patchdata 二进制格式 (v2)

### 文件布局

```
PatchDataHeader (124 bytes)
StringHeap       ── ECMA #Strings 格式（以 null 分隔的 UTF-8 字符串）
BlobHeap         ── ECMA #Blobs 格式（长度前缀的二进制数据）
UserStringHeap   ── ECMA #US 格式（长度前缀的 UTF-16）
AssemblyRefEntry[]
TypeRefEntry[]
TypeDefEntry[]
FieldDefEntry[]
MethodDefEntry[]
MemberRefEntry[]
StandaloneSigEntry[]
BodyDataSection  ── 原始 IL 字节码（MethodDefEntry.body_offset 索引）
AotCoreIrSection ── uint32_t 索引[count] + null 结尾 JSON 字符串
RegIrSection     ── uint32_t 索引[count] + 寄存器 IR 块（v2+ 预留）
```

### PatchDataHeader

定义在 `contracts/native/v0/patch_data.h`，124 字节：

- `magic` — 0x50415854 ("PADT")
- `version` — 2（v2 新增 reg_ir 段）
- `header_size` — 124（v1: 112, v2: 124）
- 各堆偏移量和大小（string/heap/body/ir）
- 各表偏移量和行数
- AotCoreIr 段偏移/大小/方法数（O(1) JSON 查找）
- Register IR 段偏移/大小/方法数（v2+，空时运行时回退到 JSON）

### 表条目标结构

| 条目 | 大小 | 关键字段 |
|------|------|----------|
| AssemblyRefEntry | 8B | name_offset, token |
| TypeRefEntry | 16B | type_name_offset, namespace_offset, resolution_scope_token, token |
| TypeDefEntry | 22B | type_name_offset, namespace_offset, enclosing_type_token, extends_token, token, flags |
| FieldDefEntry | 18B | name_offset, signature_offset, declaring_type_token, token, flags |
| MethodDefEntry | 28B | name_offset, signature_offset, body_offset, body_size, declaring_type_token, token, impl_flags, flags |
| MemberRefEntry | 16B | name_offset, signature_offset, parent_token, token |
| StandaloneSigEntry | 8B | signature_offset, token |

### 实现文件

| 角色 | 路径 |
|------|------|
| 格式定义（C 头文件） | `contracts/native/v0/patch_data.h` |
| C# 序列化器 | `src/managed/Chaos.IL2CPP.Generator/PatchDataExtractor.cs` |
| C# 解析器（dump） | `src/managed/Chaos.IL2CPP.Generator/PatchDataDumper.cs` |
| CLI 入口点 | `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs` (`emit-patch-data` 命令) |
| 运行时加载器（C++） | `src/native/runtime-core/patch_loader.h` / `patch_loader.cpp` |
| 热更新调度注册表 | `src/native/runtime-core/hotpatch_table.h` / `hotpatch_table.cpp` |

---

## 调试工作流

### 验证 .patchdata 完整性

```bash
# 文件大小合理
ls -la *.patchdata

# dump 检查
chaos-il2cpp emit-patch-data dump foo.patchdata

# 确认内容
# - Version == 2
# - BodyData 有方法体（非 zero）
# - AotCoreIr section 非空（有嵌入的 IR）
# - TypeDef/MethodDef 数量符合预期
```

### 常见问题

| 现象 | 原因 | 检查 |
|------|------|------|
| 运行时 `PatchData_TotalSize` 警告 | 文件被截断或 section 偏移计算不对 | dump 看各段 offset+size 是否越界 |
| AotCoreIr section 为空 | `--aot-core-ir` 路径错误或方法名不匹配 | dump 看 ir_count 是否为 0 |
| RegIr section 为空 | 正常（v2 新增，运行时回退 JSON） | dump 看 reg_ir_count == 0 |
| MethodDef 有 0-body 方法 | 补丁 DLL 中存在 stub-only 方法 | dump 对比 bodyCount vs total Count |