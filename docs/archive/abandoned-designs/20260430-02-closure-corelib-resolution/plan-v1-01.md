# CoreLib 闭包补齐：让 System.Private.CoreLib 进入 foundation-dll pipeline

> **面向执行 Agent：** 使用 dev:subagent-driven-development 或 dev:executing-plans 执行。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 修复 foundation-dll pipeline 中 emit-native-aot 阶段对所有跨程序集调用（如 `System.Array::Copy`）抛出 "unresolved call target" 的问题。

**根因：** `batch_native_aot_runner.py` 的 manifest 只包含 entrypoint DLL，Linker 的 reachability BFS 无法跨入 System.Private.CoreLib，导致 _methodsBySubjectId 中缺失被调用方法。

**架构：** manifest 中加入 System.Private.CoreLib.dll 作为依赖程序集，Linker 自然 trace 跨程序集调用，_methodsBySubjectId 包含 CoreLib 方法后 codegen 可正常推导 native symbol。

**技术栈：** Python（batch_native_aot_runner.py）+ .NET SDK（System.Private.CoreLib 路径解析）

**架构审核模式：** critical

**结构告警重点：** batch_native_aot_runner.py 中新增的 CoreLib 路径解析逻辑应保持独立可测，不耦合到 family 循环中。

**权责图审核主题：** 确认 codegen 层（emit-native-aot 阶段）不做任何改动——职责分工上 CoreLib 闭包补齐是 pipeline 配置层的责任，不是 codegen 层的责任。

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: pipeline-infrastructure
- capabilityItem: foundation-dll closure completeness
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: []

**设计文档：** n/a（brainstorming 已确认方案 A）

**问题清零来源：** direct user confirmation（方案 A）

**计划来源：** direct-plan

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

**全局优先级约束：**
- 优先级 1（性能最优）：CoreLib 方法体被 codegen 正常 emit —— 生成的 C++ 直接调用符号，无额外间接层
- 优先级 2（方案完美性）：架构上 pipeline 配置层（Python）负责闭包范围，codegen 层（C#）零改动
- 优先级 3（HotUpdate 支持）：CoreLib 方法出现在 IR 后 hotupdate 路径也自然受益
- 冲突时低优先级让位于高优先级

---

## 步骤

### Step 1: 读取 entry DLL 的 TFM

`batch_native_aot_runner.py` 的 `_run_convert()` 中，在构建 manifest 前新增 TFM 探测。

读取 entry DLL 同级目录的 `.runtimeconfig.json`，从 `runtimeOptions.tfm` 字段获取目标框架名（如 `net10.0`）。该信息用于定位匹配的 System.Private.CoreLib.dll。

如果 runtimeconfig 不存在（异常路径），fallback 到 `net10.0`。

### Step 2: 根据 TFM 定位 CoreLib 路径

新增函数 `_resolve_corelib_path(tfm: str) -> str`：

```python
# .NET SDK 的 CoreLib 位置规则：
#   Windows: C:/Program Files/dotnet/shared/Microsoft.NETCore.App/<version>/System.Private.CoreLib.dll
#   TFM "net10.0" → 在 Microsoft.NETCore.App 目录下找版本匹配的 dll

def _resolve_corelib_path(tfm: str) -> str:
    # 从 TFM "net10.0" 提取主版本 "10"
    major = re.search(r"net(\d+)", tfm).group(1)
    base = "C:/Program Files/dotnet/shared/Microsoft.NETCore.App"
    # 找对应主版本的最新子版本
    versions = sorted([
        d for d in os.listdir(base)
        if d.startswith(f"{major}.")
    ])
    if not versions:
        raise RuntimeError(f"No .NET Core {major} runtime found in {base}")
    latest = versions[-1]
    return os.path.join(base, latest, "System.Private.CoreLib.dll")
```

### Step 3: 修改 manifest

`_run_convert()` 中原 manifest：

```python
manifest = {
    "source": {
        "type": "managed-dlls",
        "assemblies": [dll_path],
        "entry": entry_point_subject_id,
    }
}
```

改为：

```python
corlib_path = _resolve_corelib_path(tfm)
manifest = {
    "source": {
        "type": "managed-dlls",
        "assemblies": [dll_path, corlib_path],
        "entry": entry_point_subject_id,
    }
}
```

### Step 4: 构建并运行验证

```bash
dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj

python build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py
```

验证预期：
- 所有 31 个 family 的 `convert` 阶段通过（CoreLib trace 后方法数增多，但仍成功）
- `emit-native-aot` 不再因 "unresolved call target" 失败
- 生成的 `.cpp` 文件中包含 `chaos_load_indirect` / `chaos_store_indirect` 等模板调用

### Step 5: 代码审查

- 确认 codegen 层（`InvocationPlanning.cs`、`AotCoreIrLowering.cs`、`MethodEmission.cs`）零改动
- 确认 CoreLib 路径解析只在 entry DLL 构建成功后执行（不依赖 SDK-only 信息）
- 确认异常路径（runtimeconfig 不存在 / CoreLib 未找到）有合理 fallback 或明确报错

---

## 关键文件

| 文件 | 操作 |
|------|------|
| `build/toolchains/run/testing/foundation_dll/batch_native_aot_runner.py` | 新增 `_resolve_corelib_path()`、修改 `_run_convert()` manifest |

## 验证方式

1. `batch_native_aot_runner.py` 全部 31 family 通过（预期：0 passed → 31 passed）
2. codegen 生成的 `.cpp` 文件非空（之前因 throw 无输出）
3. 仅 `batch_native_aot_runner.py` 被修改

## 不纳入范围

- codegen C# 层不做任何改动
- 不对 `trim_aot_core_ir.py` 做改动（它已正确 follow callee）
- 不对 Driver 端做改动（`managed-dlls` 已正确处理多程序集）
- 不修改 `batch_hotupdate_runner.py`（后续可根据相同模式适配）
