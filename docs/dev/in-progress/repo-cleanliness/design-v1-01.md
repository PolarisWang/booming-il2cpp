# 设计文档：构建产物统一落位 artifacts/ 方案

> 2026-08-28 · 域：构建/测试管线/仓库治理
> 状态：draft（待审批）

---

## 1. 背景与目标

### 问题

构建产物散落三处，`tests/` 因此达到 84G：

| 产物系统 | 当前路径 | 体积 | 是否受跟踪 |
|----------|---------|------|-----------|
| CMake 预设编译 | `artifacts/presets/*` | 5.7G | gitignore ✅ |
| 原生测试编译 | `build/native/` + `build/asan-native/` | 1.17G | gitignore ✅ |
| Chunk 管线构建 | `tests/e2e/translation/*/chunks/` + `.hephaestus-cache/` | ~80G | gitignore ✅ |

**核心矛盾**：`tests/e2e/translation/` 是**源文件目录**（受跟踪的 `config/`、`crypto-refs/`、`.metadata.json`、`ci-run.sh`），但构建产物（`chunks/*/native/`、`chunks/*/managed/.autogen/`、`chunks/*/managed/combined/`、`chunks/*/results/`、`.hephaestus-cache/`）被混入其中，导致 `tests/` 体积失控。

### 目标

1. 把未跟踪构建产物从 `tests/e2e/translation/` 迁出到 `artifacts/` 下
2. `tests/` 只保留源文件（受跟踪内容），体积从 84G 降到 ~200M
3. 不改 20+ stage 文件，只动 `ChunkContext` 封装层
4. `artifacts/` 功能上成为「构建产物唯一出口」

---

## 2. 架构设计

### 核心思路：按生命周期拆 source_dir / build_dir 双根

当前 `ChunkContext` 只有一个 `chunk_dir`，指向 `foundation_dir / "chunks" / <slug>`，且所有产物和源文件在此路径下混合。

```
当前（混合）：
  tests/e2e/translation/System.Private.CoreLib/
    config/            ← source
    _dll/              ← source（manifest）
    chunks/global-ns/
      managed/          ← source + build 混合
        subjects/       ← source (.cs, .csproj)
        .autogen/       ← build（probe/obj/bin）
        combined/       ← build
      native/           ← build
      results/          ← build
    .hephaestus-cache/   ← build
```

```
目标（分离）：
  tests/e2e/translation/System.Private.CoreLib/
    config/            ← source（不变）
    _dll/              ← source（不变）
    chunks/global-ns/
      managed/subjects/ ← source（不变）

  artifacts/foundation-dll/System.Private.CoreLib/
    chunks/global-ns/
      managed/
        .autogen/       ← build（迁出）
        combined/       ← build（迁出）
      native/           ← build（迁出）
      results/          ← build（迁出）
    .hephaestus-cache/   ← build（迁出）
```

### 关键设计决策

**只迁未跟踪产物，不迁源文件。** 受跟踪的 `config/`、`_dll/`、`ci-run.sh`、`crypto-refs/`、`.metadata.json`、`chunks/*/managed/subjects/*.cs`、`chunks/*/managed/subjects/*.csproj` 留在原处。

这意味着 `source_dir` 和 `build_dir` 共享相同的 `chunks/<slug>/` 路径结构，但 `managed/subjects/` 是源文件，`managed/.autogen/` 和 `managed/combined/` 是构建产物。所以 `chunk_dir` 不再指向单一物理路径，而是根由 `source_dir` 决定，子路径按属性路由到 `build_dir`。

---

## 3. 改动方案

### 文件改动范围

| 文件 | 改动量 | 风险 |
|------|--------|------|
| `orchestration/context.py` | ~新增 10 行，改 8 个属性 | 低 |
| `chunk_pipeline.py`（构造处） | ~改 5 行，传入 `build_dir` | 低 |
| `nightly_build.py`（构造处） | ~改 5 行，同上 | 低 |
| `stages/build.py` L827（`.autogen` 路径） | 改 1 行 | 低 |
| `stages/hotupdate_chunk.py` L377（`.autogen.patch` 路径） | 改 1 行 | 低 |
| `.gitignore` | 替换 4 条规则 | 低 |
| 其余 15+ stage 文件 | **0 改动** | 无 |

### chore: `context.py` 核心改动

```python
@dataclass
class ChunkContext:
    slug: str
    assembly: str
    chunk_dir: Path          # ← 语义不变，仍指向 source 侧
    foundation_dir: Path     # ← 语义不变，仍指向 source 侧
    build_dir: Path = None   # ← 新增，指向 artifacts/foundation-dll/<assembly>/chunks/<slug>

    @property
    def managed_dir(self) -> Path:
        # managed_dir 内的 subjects/ 是源文件，留在 source 侧
        return self.chunk_dir / "managed" / "subjects"

    @property
    def native_dir(self) -> Path:
        # native/ 是构建产物（entry.exe），迁到 build_dir
        return (self.build_dir or self.chunk_dir) / "native"

    @property
    def results_dir(self) -> Path:
        # results/ 是构建产物（report），迁到 build_dir
        return (self.build_dir or self.chunk_dir) / "results"

    # 其余属性（subjects_dll_path, subjects_metadata_path, entry_exe_path,
    # entry_jit_exe_path, patch_data_path, patch_subjects_dll_path）
    # 继承自 managed_dir / native_dir 的变更，自动跟随——0 改动
```

### 关键属性路由汇总

| 属性 | 当前路径根 | 改后路径根 | 性质 |
|------|-----------|-----------|------|
| `chunk_dir` | `source/chunks/<slug>` | 不变（source 侧） | source |
| `managed_dir` | `chunk_dir/managed/subjects` | 不变 | source |
| `native_dir` | `chunk_dir/native` | `build_dir/native` | **build** |
| `results_dir` | `chunk_dir/results` | `build_dir/results` | **build** |
| `foundation_dir` | `source/<assembly>` | 不变 | source |
| `dll_manifest_dir` | `foundation_dir/_dll` | 不变 | source |
| `.autogen` 路径 | `foundation_dir/chunks/<slug>/managed/.autogen` | `build_dir/managed/.autogen` | **build** |
| `.autogen.patch` 路径 | `foundation_dir/chunks/<slug>/managed/.autogen.patch` | `build_dir/managed/.autogen.patch` | **build** |

### 构建处的改动

所有 `ChunkContext` 的构造处（`chunk_pipeline.py`、`nightly_build.py`）增加一行：

```python
build_dir = work_dir / "chunks" / slug  # tests/e2e/translation/<family>/chunks/<slug>
# ↓
source_dir = work_dir / "chunks" / slug
build_dir = artifacts_dir / assembly / "chunks" / slug
ctx = ChunkContext(
    slug=slug, assembly=assembly,
    chunk_dir=source_dir,      # source 侧
    build_dir=build_dir,       # 产物侧
    foundation_dir=work_dir,
)
```

其中 `artifacts_dir` 由 `artifacts/foundation-dll/` 或 `CHAOS_ARTIFACT_DLL` env 覆盖。

### `.gitignore` 规则迁移

```
# 旧（在 tests/e2e/translation/ 下生效）
tests/e2e/translation/**/.hephaestus-cache/
tests/e2e/translation/**/chunks/
tests/e2e/translation/**/_dll/
tests/e2e/translation/**/results/

# 新（在 artifacts/foundation-dll/ 下生效）
artifacts/foundation-dll/**
```

**旧规则保留一个过渡期**，待旧路径磁盘残留清理后再移除。

---

## 4. 风险分析

### R1：`build_dir = None` 的 fallback 行为

`build_dir` 默认 `None`，`native_dir` 等属性在 `build_dir` 为 `None` 时 fallback 到 `chunk_dir`（旧行为）。这保证了：
- 任何未传 `build_dir` 的构造调用（如单测）自动兼容旧行为
- 不破坏现有测试

**缓解**：文档中声明 `build_dir` 是 pipeline 必填项，测试中用 `chunk_dir` 作为 fallback 即可。

### R2：`.autogen` 和 `.autogen.patch` 的硬编码路径

`build.py` L827 和 `hotupdate_chunk.py` L377 硬编码了 `ctx.foundation_dir / "chunks" / ctx.slug / "managed" / ".autogen"`，没有走 `ctx.native_dir` 或 `ctx.managed_dir` 属性。这两处需要单独改：

```python
# 旧（build.py L827）
auto_output = ctx.foundation_dir / "chunks" / ctx.slug / "managed" / ".autogen"
# 新
auto_output = ctx.managed_dir.parent / ".autogen"  # inherit from managed_dir's location
```

但 `managed_dir` 指向 source 侧的 `subjects/`，而 `.autogen` 是 build 产物。所以正确的是：

```python
# 新
build_managed = (ctx.build_dir or ctx.chunk_dir) / "managed"
auto_output = build_managed / ".autogen"
```

### R3：`combined/` 目录路径

`build.py` L393 中 `output_dir = subjects_dll.parent.parent / "combined"` 等价于 `chunk_dir / "managed" / "combined"`。`subjects_dll_path` 属性指向 `managed_dir / "CombinedSubjects.dll"`，`managed_dir` 指向 source 侧。但 `combined/` 是构建产物（ATG 生成的 `.cs` + `.csproj`，然后编译成 `.dll`），应该走 build 侧。

这个路径由 `subjects_dll.parent.parent` 间接推导，受 `subjects_dll_path` 属性控制。不改 `subjects_dll_path` 指向 `build_dir` 的话，`combined/` 会产生在 source 侧。

**解法**：`managed_dir` 拆为 `source_managed_dir` 和 `build_managed_dir`，`subjects_dll_path` 指向 `build_managed_dir`：

```python
@property
def source_managed_dir(self) -> Path:
    return self.chunk_dir / "managed" / "subjects"

@property
def build_managed_dir(self) -> Path:
    return (self.build_dir or self.chunk_dir) / "managed"

@property
def managed_dir(self) -> Path:
    # 兼容旧的 managed_dir 引用——subjects/ 是源文件
    return self.source_managed_dir

@property
def subjects_dll_path(self) -> Path:
    # CombinedSubjects.dll 是构建产物（ATG 编译输出）
    return self.build_managed_dir / "CombinedSubjects.dll"
```

这样改后，`subjects_dll_path` 指向 `build_dir/managed/CombinedSubjects.dll`，而 `combined/` 作为 `subjects_dll.parent.parent / "combined"` 也自动指向 `build_dir/managed/combined/`。

### R4：`_dll/` 路径

`dll_manifest_dir` 指向 `foundation_dir / "_dll"`，这属于 source 侧（manifest 元数据 `namespace-partition.json` 被 git 跟踪）。**不改**。

### R5：`managed/subjects/` 下的 `.cs` 和 `.csproj` 是源文件

这些是 ATG 测试用例源文件，和 `chunks/*/managed/subjects/` 下的 `subjects.metadata.json` 等元数据文件一样，被 git 跟踪（153 json + 23 cpp）。**留在 source 侧**，不迁。

---

## 5. 验收口径

1. **`test_driver.py --group foundation-dll-smoke`** 通过（单 chunk 全流程：build → fact → coverage-audit）
2. **`chunk_pipeline.py --all-chunks --smoke 1 --skip-probe`** 通过，产物写入 `artifacts/foundation-dll/` 而非 `tests/e2e/translation/`
3. **`tests/e2e/translation/` 下无新的 `.dll`、`.exe`、`.obj`、`.hephaestus-cache` 产生**
4. **`git clean -nd`** 不报 `tests/e2e/translation/` 下有未跟踪新建文件
5. **`git ls-files`** 无变更（不修改任何受跟踪文件路径）

---

## 6. 分步执行计划

```
Step 2a: 改 context.py —— 新增 build_dir 字段 + 属性路由
  改动: context.py（~10 行新增）
  验证: 单测 chunk_pipeline --dry-run 属性值正确

Step 2b: 改 chunk_pipeline.py + nightly_build.py 构造处
  改动: 各传 build_dir 参数
  验证: --smoke 1 跑通，产物落在 artifacts/ 下

Step 2c: 改 build.py L827 + hotupdate_chunk.py L377 的 .autogen 路径
  改动: 各 1 行
  验证: smoke 全流程通过

Step 2d: 清存量 + 换 gitignore
  改动: 删除旧缓存 + 更新 .gitignore
  验证: tests/ 体积从 84G → ~200M
```

---

## 7. 回滚方案

如果 smoke 管线失败：

1. **回退 `context.py`** 到 `build_dir=None` 默认值——所有 `build_dir` 属性自动 fallback 到 `chunk_dir`，行为完全恢复旧状
2. **不改 `.gitignore`** 旧规则保留了一个过渡期，所以旧路径的 gitignore 覆盖仍在
3. 分开修复：R2/R3 的 `.autogen` 和 `combined/` 路径问题可以单独调试，不影响其他 15+ stage