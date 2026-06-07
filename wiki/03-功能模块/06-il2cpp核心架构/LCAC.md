# LCAC: Layered Cross-Assembly Compilation

## 概述

LCAC 是替代 BridgeAOT 的工业化跨程序集调用方案。采用三层分层架构，兼顾极致性能、编译效率和稳定性。

## 架构

```
                    性能                     编译开销                    适用场景
Gold Direct Link    直接调用 (0 cycles)      仅热点方法 (5%)              运行时热点
Hephaestus Lib      直接/LTCG 内联           一次编译，全量复用         核心公共 DLL
Demeter Table       O(1) 间接 (+0.75ns)      0 (纯 extern 声明)         冷门/动态调用
```

### 层 1: Gold Direct Link (热点直接编译)

**原理**: 根据 profiling 数据识别热点跨 DLL 调用，在编译时嵌入目标方法体。

```
if (_emitDirectCall)
    CallSymbol(args);    // 直接 C++ 函数调用
else
    kTable[idx](args);   // Demeter Table 间接调用
```

**适用**: 运行时 95%+ 的指令执行时间集中在 <10% 的方法中。

### 层 2: Hephaestus Lib (共享缓存库)

**原理**: 核心 DLL 编译一次，产物存为 `.lib` 库，所有 chunk 共享链接。

```
CI 构建:
  CoreLib.dll → IL2CPP codegen → CoreLib.lib (预编译)
  CoreLib.lib → SDK 仓库 (版本缓存)

每 chunk 构建:
  subjects.cpp + CoreLib.lib → cmake --build
```

**缓存版本管理**:
- manifest.json 记录每个 DLL 的 commit hash
- ABI 兼容性编译时校验
- 仅缓存经常变更的核心 DLL

### 层 3: Demeter Table (外部调度表)

**原理**: 基于现有的 `kChaosExternalRuntimeFnTable`，所有未被层 1/2 覆盖的跨 DLL 调用走 O(1) 间接调度。

```cpp
// codegen 生成（EmitExternalRuntimeTableDispatch）
reinterpret_cast<FnType>(kChaosExternalRuntimeFnTable[idx])(args);
```

**兜底保障**:
- 启动时由 `ChaosResolveExternalRuntimeFnTable` / 解释器填充
- 未填充条目为 nullptr → 安全保护（可设 stub）
- 所有外部调用自动获得 stub 生成（`CHAOS_IL2CPP_FAIL()`）

## 与 BridgeAOT 对比

| 指标 | BridgeAOT | LCAC |
|------|-----------|------|
| 稳定性 | ❌ IL 解码异常 → 堆损坏 | ✅ 无异常路径 |
| 增量编译 | ❌ 不支持 | ✅ 完全支持 |
| 200+ DLL | ❌ 无法线性扩展 | ✅ 线性 |
| 运行时性能 | 🟢 直接调用 | 🟢 95%+ 直接调用，<5% O(1) 间接 |
| 维护成本 | 🔴 极高 | 🟢 极低 |
| 编译时间 | ~560s/chunk | ~300s/chunk (Demeter) → 可优化 |

## 实施状态

| Phase | 内容 | 状态 |
|-------|------|------|
| Phase 1 | 禁用 BridgeAOT + Demeter Table 补全 | ✅ 已完成 |
| Phase 2 | 清理 bridge 残留代码 | ✅ 已完成 |
| Phase 3 | Hephaestus Lib 缓存库 | ✅ 已完成 |
| Phase 4 | Gold Direct Link 热点编译 | ✅ 已完成 |
| Phase 5 | Wiki 文档更新 | ✅ 已完成 |

## 相关文档

- [Closure 精度架构](01-翻译管线/19-closure-precision-architecture.md) — D5/D1 assemblyDirs 配置
- [32-byte ABI 修复](01-翻译管线/20-32byte-abi-fix.md) — Vector<T> pass-by-reference 及 opcode 归一化

## Phase 3: Hephaestus Lib 实现

### hot_cache.py

`verification/stages/hephaestus_cache.py` 提供基于输入哈希的构建缓存：

- **缓存键**: SHA-256(subjects DLL + metadata JSON + 依赖 DLL)
- **缓存位置**: `<foundation-dll>/.hephaestus-cache/` + `manifest.json`
- **集成点**: `build.py` 在 TPG codegen 前检查缓存命中
- **流程**: Cache hit 跳过 500s codegen，直接恢复 entry.exe
- **LRU 淘汰**: 默认保留最近 50 个缓存条目

```
manifest.json:
  - version: 1
  - max_entries: 50
  - entries:
      - cache_key: System.Collections/global-ns/abc123...
      - input_hash: SHA-256 of inputs
      - duration_ms: 123456
      - entry_exe_size: 1048576
```

### 缓存命中逻辑

1. `compute_input_hash()` 计算统一哈希
2. `cache.is_cache_hit()` 检查 manifest 中是否有效
3. Cache hit: `cache.restore_to()` 复制 build 输出
4. Cache miss: 全量构建 cache.store() 存入缓存

### CLI 管理

```
python -m verification.chunk_pipeline hephaestus stats
python -m verification.chunk_pipeline hephaestus clear
python -m verification.chunk_pipeline hephaestus invalidate System.Collections
python -m verification.chunk_pipeline hephaestus prune --max-entries 50
```

### CMakeLists 支持

当设置 `CHAOS_HEPHAESTUS_DIR` CMake 变量时，自动链接缓存目录中的 `.lib` / `.a` 文件。

## Phase 4: Gold Direct Link 实现

### 原理

Gold Direct Link 将 PGO 识别出的热点跨 DLL 调用从 Demeter Table 间接调度升级为直接 C++ 函数调用:

```
Demeter Table (冷):  kChaosExternalRuntimeFnTable[idx](args)  — O(1) 间接调用
Gold Direct (热):    TargetMethod(args)                        — 直接函数调用（可内联）
```

### 实现

1. **`_goldDirectCallCache`**: `NativeAotLoweringPlanner` 中的 `HashSet<string>`，记录应使用直接调用的 callee SubjectId
2. **`LoadGoldDirectCallProfile()`**: 从 JSON 文件加载 PGO 热点方法列表
3. **`TryResolveDirectInvocationTarget()` 修改**: 当 callee 在 gold cache 中时，设置 `DirectNativeSymbol` 为目标 C++ 函数符号
4. **`EmitExternalRuntimeTableDispatch()`**: 已有 DirectNativeSymbol 支持 → 自动发射直接调用

### PGO 数据流

```
1. 运行基准测试/功能测试
2. entry.exe --dump-gold-profile  >  gold-profile.json
3. codegen --gold-profile gold-profile.json  (下次编译)
4. 热点方法 → 直接 C++ 调用
```

### CLI

```
codegen --gold-profile gold-profile.json
# 将热点 SubjectIds 列表传递给 codegen
# 匹配的 callee 使用直接 C++ 函数调用
```

### 已存在的 DirectNativeSymbol 支持

`ExternalRuntimeHelperDefinition.DirectNativeSymbol` 已广泛用于运行时存根:
- Array 操作: `ChaosArrayClear_Inline`, `ChaosArrayCopy_Unsafe_Inline`, `ChaosArraySort_Inline`
- 枚举操作: `ChaosEnumIsDefined`, `ChaosEnumGetName`, `ChaosEnumParse`
- Guid 操作: `ChaosGuidNewGuid`, `ChaosGuidToString`, `ChaosGuidParse`
- 全球化: `ChaosTextInfoToLower`, `ChaosTextInfoToUpper`
- 互操作: `ChaosMarshalGetLastPInvokeError`, `ChaosMarshalGetExceptionCode`

Gold Direct Link 将相同的直接调用机制扩展到 **任意跨 DLL 方法**。

### 运行时 PGO 工具

`entry.exe --dump-gold-profile` 命令列出所有外部运行时表条目:
- 输出 `kChaosExternalRuntimeSubjects[]` 中每个 SubjectId
- 可作为 `--gold-profile` 的输入（手动筛选热点方法后）
- 未来：添加调用计数跟踪（`kChaosExternalRuntimeCallCount[]`）支持阈值过滤
