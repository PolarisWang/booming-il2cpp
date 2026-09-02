# 发布流程 与 foundation-dll 流程 — 深度对比与统一 Generic 机制分析

> 日期：2026-09-01
> 域：发布流程 + 翻译路径 + 构建 + CI/测试 + 集成
> 状态：architectural 分析

---

## 1. 两个流程的本质

### 1.1 foundation-dll（内部测试/验证流程）

**本质**：一个 **stage-DAG 驱动的验证引擎**，对「多个 subject（方法）× 多个 family（assembly/chunk）× 多个 stage」做矩阵化验证。

```
引擎入口：tests/e2e/verification/chunk_pipeline.py

输入：
  --assembly System.Private.CoreLib
  --all-chunks | --chunk <slug>
  --stages build,fact,hotupdate,coverage-audit,benchmark,...
  --mode standard|strict
  --native-config check|profile|ship

核心机制（可复用点）：
  1. STAGE_DEPS: dict[str, list[str]]       ← stage 依赖 DAG（toposort）
  2. runners:    dict[str, Callable]        ← stage → 可调用函数注册表
  3. ChunkContext                            ← 运行上下文（装配、目录根、模式、run_id）
  4. StageResult                             ← 每 stage 的结果（status/summary/details/errors）
  5. UnifiedReport                           ← 汇总报告（chunk/assembly/stages/coverage/dashboard）
  6. pipeline-config.yaml                    ← 声明式配置（timeouts/mode/chunks/assemblyDirs）

执行循环：
  for chunk in chunks:
      ctx = ChunkContext(...)
      for stage in topological_order(STAGE_DEPS):
          sr = runners[stage](ctx, stages_result)
          stages_result[stage] = sr.to_dict()
      overall_status = aggregate(sr.status)
```

**代码生成工具（重要）**：foundation-dll 的 build stage 用 **ATG + TPG**（AutoTestGenerator → subjects.dll → TestProjectGenerator → entry.exe），**不是** publish 的 `ConvertService.Convert()`。两者都是 IL→C++，但是**姊妹路径**：
- ATG = 为每个方法生成 test subject（带断言），TPG 生成测试 harness
- ConvertService = 生产级 codegen（closure + AOT emitter）

**架构评分**：⭐⭐⭐⭐⭐ — 正确的 **generic 管道引擎**（stage DAG + runner 注册表 + 统一 context + 统一 result + 声明式配置）。

### 1.2 发布流程（外部产品发布）

**本质**：一个 **线性的 bash 编排脚本**，产生分发的 artifacts。

```
入口：scripts/release.sh 0.1.1 --publish

步骤（硬编码线性，非 DAG）：
  1/9  branch    创建 release/0.1.x
  2/9  bump      release_bump.sh → 三源版本号
  3/9  notes     generate-release-notes.sh
  4/9  sdk       build_presets.py → 14 个 .lib
  5/9  nupkg     build-tool-package.sh → dotnet tool 包
  6/9  checksums generate-checksums.sh
  7/9  sbom      generate-sbom.sh
  8/9  hygiene   chaos_hygiene.py --ci
  9/9  github    gh release create + upload
```

子工具（`generate-*.sh`）是**独立的 bash 脚本**，各自 `source release-config.sh` 取常量，没有共享的依赖关系/结果模型。

**架构评分**：⭐⭐⭐ — 功能正确，但**每个步骤是孤立的 bash 黑盒**，没有声明式描述、没有依赖 DAG、没有统一的 stage result 聚合、没有可查询的 pipeline 状态。

### 1.3 dotnet tool publish（PublishController，managed）

**本质**：一个 **C# 方法内的 5 步硬编码流程**。

```
PublishController.Run():
  1/5  build managed (dotnet build csproj → dll)
  2/5  detect entry point
  3/5  ConvertService.Convert (IL→C++ codegen + SDK emit)
  4/5  EmitAppProject (app_main.cpp + CMakeLists)
  5/5  BuildService.ConfigureAndBuild (cmake → entry.exe)
```

**架构评分**：⭐⭐⭐ — 流程正确，但 **5 步在单个方法里硬编码**，无 stage 抽象。

---

## 2. 核心洞察：三者其实在共享「翻译 → 构建 → 运行」原语

| 层 | foundation-dll | 发布流程 | publish |
|----|---------------|---------|---------|
| **依赖 DAG** | ✅ STAGE_DEPS + toposort | ❌ 线性 bash | ❌ 线性 C# 方法 |
| **stage 注册表** | ✅ runners dict | ❌ 独立脚本 | ❌ switch/if |
| **统一上下文** | ✅ ChunkContext | ❌ 各自环境变量 | ❌ 局部变量 |
| **统一结果** | ✅ StageResult + UnifiedReport | ❌ stdout 解析 | ❌ 返回码 |
| **声明式配置** | ✅ pipeline-config.yaml | ⚠️ release-config.sh 常量 | ❌ 命令行 arg |
| **核心原语** | 翻译(CombineSubj)→构建(TPG→exe)→验证 | 构建 SDK(presets)→打包→签名 | 翻译(Convert)→构建(cmake→exe) |

**关键**：三者共享的底层原语是完全相同的 —— **一个 .NET 程序集 → IL→C++ codegen → native 链接 → 可执行文件**。差异只在**外壳**：

- foundation-dll 的外壳 = 多 subject × 多 stage × 聚合报告（验证）
- 发布流水线 的外壳 = 单 SDK × 打包产物 × 签名分发（产品）
- publish 的外壳 = 单应用 × 入口点 × 用户级 exe

**所以"统一到同一套 generic 机制"是完全可行的 —— 通用机制就是 foundation-dll 的 stage-DAG 引擎。**

---

## 3. 深度分析：为什么 foundation-dll 引擎是统一的基础

### 3.1 它已经泛化了「对任意目标跑任意步骤」

foundation-dll 引擎的关键设计让它天然可复用于发布：

1. **stage 是可插入函数**：`runners = {"build": run_build, "fact": run_fact_chunk, ...}`。要加一个发布 stage，只需 `{"checksum": run_checksum, "sbom": run_sbom, "release": run_release}`。

2. **依赖是显式 DAG**：`STAGE_DEPS` 让 stage 能声明"我依赖谁"，引擎自动 toposort。发布流程的 `branch→bump→sdk→nupkg→checksum→sbom→release` 天然是一个线性 DAG，`release→(checksum, sbom, nupkg)` 是扇出。

3. **context 承载所有共享状态**：`ChunkContext` 已经有 `native_config`、`assembly_dirs`、`run_id`、`platform`、`git_commit`。发布需要的 `version`、`sdk_dir`、`output_dir` 只是再加几个字段。

4. **result 统一聚合**：`StageResult.status ∈ {passed, failed, warning, error}` + `UnifiedReport`。发布流程现在的 stdout 返回码可以换成这个统一结果模型，CI 可以 grep 到结构化 JSON。

5. **声明式配置**：`pipeline-config.yaml`。发布可以有一个 `release-plan.yaml` 声明 stages + families + platform matrix。

### 3.2 foundation-dll 已经"发布"了每个 chunk 的原生 exe

`build.py` 的 `entry.exe` 就是「一个 subject 集合被 publish 成原生可执行文件」——**这和 `chaos-il2cpp publish` 的产物是同一种东西**。foundation-dll 在 CI 里已经把 System.Private.CoreLib 分 chunk 全部 publish 过、跑过 fact、benchmark 过。

**所以：foundation-dll 的 build stage ≈ chaos-il2cpp publish（对 subject 集合）；差异只是 entry 形态（subject dispatch harness vs 用户 Main）。**

---

## 4. 统一方案设计

### 4.1 目标架构：一个通用 Stage 引擎，三种 pipeline

```
                 ┌─────────────────────────────────────────────┐
                 │  GenericStageEngine (foundation-dll 演化)     │
                 │  /tests/e2e/verification/orchestration/       │
                 │  - STAGE_DEPS (DAG)                          │
                 │  - runners (stage 注册表)                     │
                 │  - ChunkContext → PipelineContext (泛化)      │
                 │  - StageResult + PipelineReport              │
                 │  - pipeline-config.yaml → plan.yaml          │
                 └─────────────────────────────────────────────┘
                          │            │            │
              ┌───────────▼─┐  ┌───────▼────┐  ┌────▼─────────┐
              │ 验证 Pipeline│  │发布Pipeline│  │用户publish   │
              │  foundation- │  │ release    │  │ chaos-il2cpp │
              │  dll         │  │            │  │              │
              │ build,fact,  │  │ bump,sdk,  │  │ convert,     │
              │ hotupdate,   │  │ nupkg,     │  │ emit, build  │
              │ benchmark,...│  │ checksum,  │  │              │
              │              │  │ sbom,      │  │              │
              │              │  │ release    │  │              │
              └──────────────┘  └────────────┘  └──────────────┘
                 stage 集合:      stage 集合:      stage 集合:
                 build,fact,      bump,sdk,        convert,emit,
                 hotupdate,       nupkg,checksum,  buildnative,
                 coverage,        sbom,hygiene,    run,verify
                 benchmark,       github-release
                 aggregate,
                 reporting
```

### 4.2 具体改造步骤

#### 步骤 A：泛化 engine 数据模型（改 `context.py`）

把 `ChunkContext` 泛化为 `PipelineContext`，或给 `ChunkContext` 加可选发布字段，保持向后兼容：

```python
# 现状 ChunkContext 已有字段已覆盖 90%
# 增加（可选，发布专用）：
@dataclass
class ChunkContext:  # 或新增 ReleaseContext
    version: str = ""            # 发布版本 0.1.1
    sdk_dir: Path | None = None  # artifacts/release/<ver>
    nupkg_path: Path | None = None
    release_notes: Path | None = None
    artifacts_root: Path | None = None
```

`StageResult` 和 `UnifiedReport` **不需要改** —— 它们已经通用（stage/status/details）。

#### 步骤 B：把发布步骤写成 stage（新建 `verification/stages/release/*.py`）

每个发布子工具变成一个 stage 函数，签名 `fn(ctx, stages_result) -> StageResult`：

```python
# verification/stages/release/bump.py
def run_bump(ctx, stages_result) -> StageResult:
    # 调 scripts/release_bump.sh version --tag
    ...

# verification/stages/release/sdk_build.py
def run_sdk_build(ctx, stages_result) -> StageResult:
    # 调 build_presets.py
    ...

# verification/stages/release/nupkg.py
def run_nupkg(ctx, stages_result) -> StageResult:
    # 调 build-tool-package.sh
    ...

# verification/stages/release/checksum.py / sbom.py / github_release.py
# 同理，每个都是 (ctx, stages_result) -> StageResult
```

这些是**薄包装**：逻辑仍调现有 `scripts/*.sh`，但被纳入统一 stage 框架 → 有依赖 DAG、有统一 result、有 config。

#### 步骤 C：新建 `release_pipeline.py` 或复用 `chunk_pipeline.py` 的 runner

一个发布 DAG：

```python
# verification/release_pipeline.py
STAGE_DEPS = {
    "bump":       [],
    "branch":     ["bump"],
    "sdk":        ["branch"],
    "nupkg":      ["sdk"],
    "checksum":   ["sdk", "nupkg"],   # checksum 依赖 sdk+nupkg 都产出
    "sbom":       ["sdk", "nupkg"],
    "hygiene":    ["checksum", "sbom"],   # 门禁在产物后
    "github":     ["hygiene", "checksum", "sbom"],
}
runners = {"bump": run_bump, "sdk": run_sdk_build, ...}
```

这就是 `chunk_pipeline.py` 的 stage_names/runners/STAGE_DEPS 循环**一字不差**复用 —— 只换 stage 集合。

#### 步骤 D：把 publish（managed C#）也纳入统一 `translate+build+run` 原语

`PublishController.cs` 的 5 步本质上正是 `build.py`（translate + build native）+ `fact_chunk.py`（run + verify）。最干净的统一：

- **在 engine 侧**：`run_build` 已经做了「subjects.dll → TPG → entry.exe」。`chaos-il2cpp publish` 对一个单应用做同样的事。
- **让 publish 复用 engine**：publish 其实可以调用 `run_build`（当输入是 subject 集合）或调用一个新的 `run_publish_app` stage（单应用）。
- **或者**：把 PublishController 的 5 步改为调用 engine 的 stage runner（C# 侧留个薄 CLI，Python 侧跑 stage）。成本高，可作 P2。

**推荐的低成本路径**：publish 不动，只把 **发布流程（release.sh）** 迁移到 engine，让它和 foundation-dll **共享 stage 基础设施**（入口、deps、result、config）。这已经满足用户"共同 generic 机制"的核心诉求，且不破坏 foundation-dll 现有运行。

---

## 5. 统一后的收益

| 维度 | 统一前 | 统一后 |
|------|--------|--------|
| **开发心智** | bash + C# + Python 三套流程 | 一套 stage 机制 + 三套 stage 集合 |
| **可测性** | release.sh 只能手跑，无单测 | 每个 stage 是 `(ctx, results)->StageResult`，可单测 |
| **可观测性** | stdout 返回码 | `PipelineReport` JSON + `stage.status` 聚合 |
| **可组合性** | 只能线性 | toposort + DAG 扇出/扇入 |
| **可配置性** | 环境变量/arg | `release-plan.yaml` 声明式 |
| **回归保护** | 无 | stage 依赖声明，缺依赖立即报错 |
| **CI 集成** | 每个 CI 重写一遍 | 统一 runner 入口 + 统一 report |

---

## 6. 风险与成本

| 风险 | 等级 | 缓解 |
|------|------|------|
| 改 engine 破坏 foundation-dll | 中 | **纯增量**：加 stage + 新 pipeline，不动现有 build/fact 逻辑 |
| 把 release.sh 从 bash 迁到 Python 成本 | 中 | stage 是薄包装调现有 `scripts/*.sh`，不是重写 |
| PublishController 统一成本高 | 高 | **P2 再做**；先发布+验证共享 engine |
| 双语言（Python engine / C# publish）割裂 | 低 | 单应用 publish 保留 C#，多 subject 验证走 engine |

---

## 7. 结论

**核心结论**：foundation-dll 引擎已经是成熟、正确的 **generic stage 管道**。发布流程（release.sh）和它**共享完全相同的一个底层原语**（.NET 程序集 → codegen → native exe），只是外层 stage 集合不同。

**推荐方案（分阶段）**：

- **Phase 1（推荐，P0）**：把 release.sh 的每个步骤改写成 engine 的 stage（薄包装），新建 `release_pipeline.py` + `release-plan.yaml`。让发布和 foundation-dll **共享 stage 基础设施**（DAG runner + context + StageResult + pipeline-config）。改动 <engine> 的纯增量，零破坏，约 1-2 天。
- **Phase 2（P1）**：统一 `build.py` 与 `PublishController` 的"translate+build+run"原语 —— 单应用 publish 复用 engine 的 build stage（作为 `run_publish_app`）。
- **Phase 3（P2）**：publish 的 5 步 C# 逻辑也迁移到 engine（大改，需专门设计）。

**一句话**：foundation-dll 的 stage-DAG 引擎就是那套 generic 机制，发布流程"接入"它而非"另起炉灶"，是成本最低、收益最清晰的统一路径。