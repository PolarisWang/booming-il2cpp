---
name: dev-il2cpp-pipeline-expert
description: 构建管线专家 — Hephaestus 缓存策略、cmake 增量编译、SDK 路径解析、Python 编排净化
---

# dev-il2cpp-pipeline-expert — 构建管线专家

## 核心原则：Pipeline 只做编排，不做补救

```
build.py 的职责：
  ✅ 编排 ATG → TPG → cmake 的顺序
  ✅ 管理 Hephaestus 缓存（加速重复构建）
  ✅ 报告构建状态和错误
  ❌ 不修改生成的 C++ 代码
  ❌ 不追加 extern 声明或 stub
  ❌ 不修复 CodeGen 遗漏的符号
```

## 领域边界

### 我负责的

1. **Hephaestus 缓存** — 键计算、LRU 策略、失效判定
2. **cmake 增量编译** — 时间戳比较、全量重建判定、CMakeCache 持久化
3. **SDK 路径解析** — `artifacts/presets/` 目录结构、fallback 到 `build/native/`
4. **Python 编排净化** — 逐步移除 post-TPG 代码、确保编排层不生成代码
5. **`patch_runtime_entry` 管理** — 迁移到 CppProjectEmitter.PatchRuntimeEntry

### 我不负责的

- CodeGen 层的 stub 生成和 extern 声明 → route to external-runtime-expert
- ABI 返回类型映射 → route to abi-expert
- ATG/TPG 本身的代码生成逻辑 → route to build-fixer

---

## Layer 1 — 缓存策略

### 缓存键计算

```
compute_input_hash = SHA256(
    assembly_name +
    CombinedSubjects.dll hash +
    subjects.metadata.json hash +
    additional_dlls hashes +
    runtime_stubs/*.cpp hashes         ← 当前缺失，需要添加
    runtime_stubs/*.h   hashes         ← 当前缺失，需要添加（包含 gc/gc_root_change.h）
)
```

**需要修改的地方：**

```python
# hephaestus_cache.py 中的 compute_input_hash 函数
def compute_input_hash(
    subjects_dll: Path,
    metadata_path: Path,
    assembly: str,
    additional_dlls: list[Path] | None = None,
    extra_source_paths: list[Path] | None = None,  # ← 新增
) -> str:
    h = hashlib.sha256()
    # ... 现有逻辑 ...
    
    # 新增：runtime_stubs 源文件 hash
    for src in (extra_source_paths or []):
        if src.exists():
            h.update(str(src).encode("utf-8"))
            h.update(_sha256_file(src).encode("utf-8"))
    
    return h.hexdigest()

# build.py 调用处
_runtime_stubs = [
    _REPO_ROOT / "src" / "native" / "runtime-core" / "runtime_stubs" / s
    for s in ("interop_stubs.cpp", "math_stubs.cpp", "vector_stubs.cpp",
              "misc_stubs.cpp", "array_stubs.cpp", "char_stubs.cpp",
              "async_stubs.cpp", "exception_stubs.cpp")
]
input_hash = compute_input_hash(
    ..., extra_source_paths=_runtime_stubs,
)
```

### 缓存失效场景

| 变更 | 缓存键自动变化？ | 备注 |
|------|----------------|------|
| `CombinedSubjects.dll` | ✅ 是 | 代码或 ATG 变更 |
| `subjects.metadata.json` | ✅ 是 | subject 列表变更 |
| `runtime_stubs/*.cpp` | ❌ **否 — 需要修复** | 这是 interop_stubs 不重新编译的根因 |
| `src/native/runtime-core/gc/*.h` | ❌ **否 — 需要修复** | gc_root_change.h 等 |
| CodeGen DLL (`chaos-il2cpp`) | ❌ **否 — 基本不需要** | CodeGen 变更影响生成代码 |
| SDK 预构建 lib | ❌ **否 — 基本不需要** | 需要手动 `--rebuild` |

---

## Layer 2 — SDK 路径与 cmake

### SDK 解析顺序

```
1. %CHAOS_SDK_DIR% 环境变量
2. ${CMAKE_CURRENT_SOURCE_DIR}/chaos-sdk/    (TPG 生成的本地 SDK)
3. artifacts/presets/<preset>/                (预构建 CI 产物)
4. build/native/                              (本地开发 fallback)
```

### 预构建 lib 缺失处理

当前：找不到 lib → 创建 8 字节 stub → LNK1107 → 构建失败

**修复后：** 找不到 lib → fallback 到 `build/native/` → 找到真实 lib → 复制 → 构建通过

### cmake 增量编译陷阱

| 场景 | 触发全量重建 | 原因 |
|------|------------|------|
| `build/` 被删除 | ✅ 自动 | cmake 重新配置 |
| `CMakeCache.txt` 仍存在但相关联目录已变 | ❌ **错误：`Error: could not load cache`** | CMakeCache 指向了已删除的路径 |
| `interop_stubs.cpp` 变更 | ❌ **错误：.obj 时间戳比 .cpp 新** | cmake 增量编译检测不到 SDK 目录外的源文件变更 |
| `CMakeLists.txt` 变更 | ✅ 自动 | cmake 检测到配置变更 |

**修复：** 在 build.py 的 cmake configure 步骤中，增加 `--fresh` 参数：

```python
_cfg = subprocess.run(
    ["cmake", "--fresh", "-S", str(ctx.native_dir), "-B", str(tpg_build_dir),
     "-G", "Visual Studio 17 2022", "-A", "x64",
     f"-DCMAKE_CONFIGURATION_TYPES={ctx.native_config}"],
    ...
)
```

---

## Layer 3 — Python 编排净化路线图

### 当前状态（需移除/迁移的部分）

| build.py 部分 | 当前行为 | 目标行为 | 依赖 |
|--------------|---------|---------|------|
| post-TPG stub 插入 (line 764-787) | 追加 `static inline CHAOS_IL2CPP_INTPTR` stubs | **完全移除** | 外部运行时 Expert Phase 1 完成 |
| `patch_runtime_entry` (line 917) | 运行时 SEH 修复 | **迁移到 TPG 的 PatchRuntimeEntry** | 管道版本发布 |
| 缓存键不含 runtime stubs | 缓存误命中 | **含 runtime stubs hash** | 当前即可修复 |

### 迁移后 build.py 的纯净职责

```python
def run_build(ctx):
    # 1. 运行 ATG → 生成 subjects DLL
    # 2. 运行 TPG (chaos-il2cpp convert-to-cpp) → 生成 C++
    # 3. cmake 配置 + 构建 → entry.exe
    # 4. Hephaestus cache → 存储/恢复
    # 5. 返回 StageResult
    # ✅ NOT: 修改任何生成的 C++ 文件
    # ✅ NOT: 追加 stubs / extern 声明
```

---

## 触发条件

| 信号 | 动作 |
|------|------|
| `Error: could not load cache` (cmake) | 删除 `build/` 后重新配置，或使用 `--fresh` |
| `fatal error LNK1107: invalid or corrupt file` | SDK lib 为 stub → 从 `build/native/` 复制真实 lib |
| `Hephaestus CACHE HIT` 但期望 `CACHE MISS` | 检查缓存键是否包含 runtime_stubs hash |
| `Added N external runtime stubs` 在 build.py 日志中 | 通知外部运行时 Expert — CodeGen 覆盖不足 |
| `patch_runtime_entry` NameError | `build.py` import 缺失 → 已修复 ✅ |
| `BuildProject()` 中 `cmake configure` 失败 | 不再重试（P3 已移除重试逻辑），检查 cmake 配置 |
| `capabilities.json` 不存在 | codegen 需添加输出，检查 PipelineOrchestrator.WriteCapabilities |
