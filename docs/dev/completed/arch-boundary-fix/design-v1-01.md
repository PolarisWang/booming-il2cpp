# 架构边界修复 & AI 合规体系

> 设计文档 v1.0 — 2026-06-12
> 
> 来源: `testing/foundation-dll/verification/` 管线四层架构风险评估报告

---

## 1. 架构总览

### 1.1 修复后四层架构

```
┌────────────────────────────────────────────────────────────┐
│ Layer 4: Python Pipeline (testing/foundation-dll/verify/)  │
│ 职责: 编排、数据流、测试配置、CI 门禁                       │
│ 产出: verification-report.json, dashboard.json              │
│ 红线: 不得 write_text .cpp/.h，CMakeLists.txt 仅限白名单   │
├────────────────────────────────────────────────────────────┤
│ Layer 3: TPG (TestProjectGenerator, C#)                    │
│ 职责: 测试项目脚手架(Scriban)、cmake 构建                  │
│ 产出: CMakeLists.txt, entry.cpp/h, dispatch.cpp,           │
│       runtime-entry.cpp, chaos-config.cmake                 │
│ 红线: 不得修改 codegen 产出的 .generated.cpp/.h           │
├────────────────────────────────────────────────────────────┤
│ Layer 2: Codegen (Chaos.IL2CPP.Generator, C#)              │
│ 职责: IL→C++ 翻译、类型声明、方法体、external runtime tbl │
│ 产出: native-aot.generated.cpp/.h, code-registration.json  │
│ 红线: 所有 C++ 产出必须自包含、语法正确、不依赖后处理      │
├────────────────────────────────────────────────────────────┤
│ Layer 1: Native Runtime (C++)                               │
│ 职责: GC, dispatch, interpreter, JIT                       │
├────────────────────────────────────────────────────────────┤
│ 支撑层: ATG (AutoTestGenerator, C#)                         │
│ 职责: DLL 扫描、C# 测试代码生成 (--generate-wrappers)      │
│         patch-mode 生成                                    │
├────────────────────────────────────────────────────────────┤
│ 支撑层: SDK (Chaos.TestFramework.Sdk, C# -> NuGet)          │
│ 职责: Assert stubs, runtime_stubs/, profile_globals.cpp     │
└────────────────────────────────────────────────────────────┘
```

### 1.2 修复后数据流

```
[修复前]
Python ──write_text──▶ entry_stubs.cpp          ❌ L4 生成 C++
Python ──write_text──▶ runtime-entry.cpp patch   ❌ L4 修补 C++
Python ──f-string───▶ CombinedSubjects.cs        ⚠️ L4 生成 C# wrappers
Hephaestus ◀──[input DLL hash only]──▶ cache hit ⚠️ 忽略 codegen 变更

[修复后]
TPG Scriban ──render──▶ runtime-entry.cpp        ✅ L3 模板
SDK runtime_stubs/ ──▶ Assert_Reset/Complete      ✅ 公共源
Codegen emitter ──emit──▶ external runtime tbl    ✅ L2 完整
ATG ──generate-wrappers──▶ .cs wrappers          ✅ 正式工具
Hephaestus ◀──[... + codegen_hash + template_hash]──▶ cache ✅ 版本感知
```

---

## 2. 执行批次规划

### 2.1 Batch A: C++ 修补迁移（P0）

**目标**: 消除 Python 层生成/修补 C++ 代码的行为

#### A-1: Assert stubs → SDK runtime_stubs/

| 项 | 值 |
|---|-----|
| 当前状态 | Python `build.py:840-848` 写入 `entry_stubs.cpp` |
| 目标状态 | `chaos-sdk/runtime_stubs/entry_stubs.cpp` 作为 SDK 公共源 |
| 涉及文件 | 新建 `chaos-sdk/runtime_stubs/entry_stubs.cpp` + `SdkEmitter.cs` |
| 改动文件 | `build.py` (删除 L840-848)、`SdkEmitter.cs` (新增 `CopyRuntimeStubSources`)、`TestProject.CMakeLists.txt.scriban` (引用新 stubs) |

执行步骤:
1. 在 `SdkEmitter.cs` 中将 `entry_stubs.cpp` 注册为标准 runtime stub 源文件
2. TPG CMake 模板添加 `${CHAOS_SDK_DIR}/runtime_stubs/entry_stubs.cpp` 引用
3. 删除 `build.py` 中的 `entry_stubs.cpp` 生成代码

#### A-2: SEH fix + CHAOS_FACT_CHECK macro → TPG Scriban 模板

| 项 | 值 |
|---|-----|
| 当前状态 | `runtime_entry_patcher.py` 做文本替换修补 |
| 目标状态 | `runtime-entry.cpp.scriban` 直接输出正确的宏定义和 SEH 处理 |
| 涉及文件 | `runtime-entry.cpp.scriban` |
| 改动文件 | `runtime-entry.cpp.scriban`（修改模板）、`runtime_entry_patcher.py`（删除）、`build.py`（删除 patch_runtime_entry 调用） |

执行步骤:
1. 修改 `runtime-entry.cpp.scriban`：将 CHAOS_FACT_CHECK 改为无 try 版本
2. 修改 JitVehHandler 逻辑：使用 CONTINUE_SEARCH 替代 RIP+=3
3. 在模板中直接加入 assert stubs 的 forward declaration
4. 删除 `runtime_entry_patcher.py` 全部内容
5. 删除 `build.py` 中对 `patch_runtime_entry()` 的调用

#### A-3: Interop stub registration → Codegen emitter

| 项 | 值 |
|---|-----|
| 当前状态 | Python `build.py:890-974` 在 runtime-entry.cpp 注入 extern 声明 + 注册循环 |
| 目标状态 | Codegen 的 `ExternalRuntimeFnTableEmitter` 扫描所有 external runtime subjects，对 nullptr entry 自动生成 stub |
| 涉及文件 | `NativeAotLoweringPlanner.*.cs`（新增 stub generation） |
| 改动文件 | Codegen emitter（新增方法）、`build.py`（删除 ~80 行修补代码） |

执行步骤:
1. 在 Codegen 中定位 `kChaosExternalRuntimeFnTable` 的发射逻辑
2. 新增扫描 pass：检测哪些 subjects 的 entry 为 nullptr
3. 对这些 subjects，自动生成简单的 return-0 / return-default stub
4. 注册到 external runtime table
5. 验证 Marshal.GetLastPInvokeError 等特殊方法被覆盖
6. 删除 `build.py` 中的 interop stub 注册修补代码

#### A-4: C# wrapper 生成 → ATG --generate-wrappers

| 项 | 值 |
|---|-----|
| 当前状态 | Python `build.py:104-234` `_compile_custom_subjects()` 用 f-string 生成 C# wrappers |
| 目标状态 | ATG 新增 `--generate-wrappers` CLI 命令，Python 仅负责调用 |
| 涉及文件 | ATG `Program.cs`（新命令）、`TestEmitter.cs`（新方法） |
| 改动文件 | `AutoTestGenerator/Program.cs`、`TestEmitter.cs`、`build.py`（简化调用） |

执行步骤:
1. ATG 新增命令行模式 `--generate-wrappers`:
   - 接收参数：`--custom-cs-files <list>`、`--output <dir>`、`--slug <name>`
   - 生成 `CombinedSubjects.cs` + `CombinedSubjects.csproj`
2. `build.py` 删除 `_compile_custom_subjects`，改为调用 ATG
3. Python 只负责组装参数和检查返回值

#### A-5: Codegen flat layout copy hack 修复

| 项 | 值 |
|---|-----|
| 当前状态 | `build.py:875-886` Python 将 codegen 产出从 `codegen/generated/` copy 到 `subjects/` |
| 目标状态 | TPG 的 Emit() 步骤正确完成所有文件放置 |
| 改动文件 | `CppProjectEmitter.cs` 或 `CodegenOrchestrator.cs` |

---

### 2.2 Batch B: 缓存架构升级（P0）

#### B-1: Hephaestus cache key 版本化

| 项 | 值 |
|---|-----|
| 当前状态 | cache key = `assembly/chunk_slug/platform/input_hash[:16]` |
| 目标状态 | cache key = `assembly/chunk_slug/platform/input_hash[:12]/codegen_ver[:8]/tpl_hash[:8]/rt_hash[:8]` |
| 改动文件 | `hephaestus_cache.py` |

实现细节:
```python
def compute_key(self, input_hash, assembly, chunk_slug):
    # 新增三层版本戳
    codegen_ver = self._compute_codegen_version()   # sha256 of managed/Chaos.IL2CPP.Generator/
    tpl_hash = self._compute_template_hash()         # sha256 of all .scriban files
    rt_hash = self._compute_runtime_hash()            # sha256 of native/runtime-core headers
    return (
        f"{assembly}/{chunk_slug}/{sys.platform}/"
        f"{input_hash[:12]}/{codegen_ver[:8]}/{tpl_hash[:8]}/{rt_hash[:8]}"
    )

def _compute_codegen_version(self):
    """SHA-256 of all .cs files in Chaos.IL2CPP.Generator."""
    gen_dir = self._repo_root / "src" / "managed" / "Chaos.IL2CPP.Generator"
    return self._hash_files(gen_dir.rglob("*.cs"))

def _compute_template_hash(self):
    """SHA-256 of all Scriban templates."""
    tpl_dir = self._repo_root / "src" / "tools" / "Chaos.IL2CPP.Tools.TestProjectGenerator" / "Templates"
    return self._hash_files(tpl_dir.rglob("*.scriban"))
```

逃生门: `pipeline-config.yaml` 中增加:
```yaml
cache:
  invalidate_all: false    # 设为 true 强制全量失效
  version: "1"             # 手动 bump 使所有旧缓存失效
```

#### B-2: 缓存合约文件

在 `native/` 目录构建完成后写入 `.cache-contract.json`:

```json
{
  "generated_by": {
    "input_dll_hash": "abc123def456",
    "codegen_version": "def789ab",
    "template_hash": "ghi012cd",
    "runtime_hash": "jkl345ef"
  },
  "generated_at": "2026-06-12T10:00:00Z",
  "cache_key": "System.Private.CoreLib/system/win32/abc123def/def789ab/ghi012cd/jkl345ef"
}
```

---

### 2.3 Batch C: 工程债务清理（P1-P2）

#### C-1: 提取共享 tool_helpers 模块

| 项 | 值 |
|---|-----|
| 当前状态 | `_detect_tfm`、`_ensure_tool_built`、`_tool_dll` 在 build.py 和 hotupdate_chunk.py 中重复 |
| 目标状态 | 统一在 `testing/_pipeline/tool_helpers.py` 中 |
| 改动文件 | 新建 `_pipeline/tool_helpers.py`，修改 `build.py`、`hotupdate_chunk.py` |

#### C-2: 清理 `[Obsolete]` 模板代码

| 项 | 值 |
|---|-----|
| 当前状态 | `BuildSystemTemplateCatalog.cs` + `CmakeGenerator.cs` 标记 `[Obsolete]` 但编译 |
| 目标状态 | 确认无消费方后删除；或有消费方则拷贝到消费方项目 |
| 改动文件 | Codegen `BuildSystem/` 目录 |

#### C-3: 清理 TPG server mode 死代码

| 项 | 值 |
|---|-----|
| 当前状态 | `TPG Program.cs` 中的 `server` 命令未被 chunk pipeline 使用 |
| 目标状态 | 删除 server mode |
| 改动文件 | `Program.cs` |

#### C-4: 消除 CSProj 重复写入

| 项 | 值 |
|---|-----|
| 当前状态 | `build.py` L620-658 有两段完全相同的 CSProj write_text |
| 目标状态 | 只保留一段 |

#### C-5: 清理 .autogen/ 缓存目录

| 项 | 值 |
|---|-----|
| 当前状态 | `.autogen/{slug}/` 与 `chunks/{slug}/managed/` 路径重复 |
| 目标状态 | 统一到 `chunks/{slug}/managed/`，删除 `.autogen/` |

#### C-6: CodegenOrchestrator 归属明确

| 项 | 值 |
|---|-----|
| 当前状态 | `CodegenOrchestrator.cs` 在 TPG 项目中 |
| 目标状态 | 在 Codegen 项目中新增 `Orchestration/` 目录，迁移 |

---

### 2.4 Batch D: AI 合规体系（全链路）

#### D-1: Pre-commit Hook — 边界违规检测

在 `.githooks/pre-commit` 中添加:

```bash
#!/bin/bash
# 检查 Python 文件是否 write_text .cpp/.h 文件
# 允许 BOUNDARY_OVERRIDE 标注

STAGED_PY=$(git diff --cached --name-only --diff-filter=ACM '*.py')
violations=0

for f in $STAGED_PY; do
    # 检查是否包含 write_text 指向 .cpp/.h
    if grep -nP '\.write_text\(.*\.(cpp|h)' "$f" | grep -v 'BOUNDARY_OVERRIDE'; then
        echo "❌ 边界违规: $f 中的 write_text 写入 .cpp/.h 文件"
        echo "   如确实需要，在 write_text 前一行加 # BOUNDARY_OVERRIDE: <ticket-link>"
        violations=$((violations + 1))
    fi
done

[ $violations -eq 0 ] || exit 1
```

#### D-2: CI Pipeline — 四层边界检查

在 `testing/foundation-dll/verification/preflight/` 新增 `check_layer_boundaries.py`:

```python
def check_layer_boundaries():
    """Pipeline 启动前的架构边界完整性检查."""
    violations = []
    
    # 1. Python 不得 write_text .cpp/.h
    for py_file in stages_dir.rglob("*.py"):
        content = py_file.read_text()
        if re.search(r'\.write_text\(.*\.(cpp|h)', content):
            if 'BOUNDARY_OVERRIDE' not in get_previous_line(content, match):
                violations.append(f"{py_file}: Python writes C++")
    
    # 2. TPG 不得修改 .generated.cpp
    # 3. Codegen 的 .generated.cpp 必须自包含（不依赖后处理）
    # 4. ATG 不得生成 C++
    
    return violations  # 非空则 fail
```

#### D-3: CLAUDE.md 架构层规则

在 CLAUDE.md 中新增《四层架构合规规范》章节（见下文第 4 节）

#### D-4: Code Review Checklist 条目

PR template 中新增架构边界检查项:
- [ ] Python 层没有新产生 `write_text` 写入 .cpp/.h 的调用
- [ ] 如果必须违规，已标注 `BOUNDARY_OVERRIDE` + ticket link
- [ ] Codegen 产出的 `.generated.cpp` 是自包含的
- [ ] Scriban 模板是 C++ 脚手架的唯一生成源

---

## 3. 执行计划与工作量估算

```
Week 1                    Week 2                    Week 3
┌──────────────────┐      ┌──────────────────┐      ┌──────────────────┐
│ Batch A: C++     │      │ Batch C: 债务    │      │ Batch D: AI      │
│  修补迁移         │      │  清理            │      │  合规体系        │
│                  │      │                  │      │                  │
│ A-1 Assert stubs │ 并行 │ C-1 tool_helpers │ 并行 │ D-1 Pre-commit   │
│ A-2 SEH+MACRO    │  ⟷  │ C-2 Obsolete     │  ⟷  │ D-2 CI 边界检查   │
│ A-3 Interop stub │      │ C-3 Server mode  │      │ D-3 CLAUDE.md    │
│ A-4 ATG wrappers │      │ C-4 CSProj dup   │      │ D-4 PR template  │
│ A-5 Flat layout  │      │ C-5 .autogen     │      │                  │
│                  │      │ C-6 Orchestrator │      │                  │
├──────────────────┤      ├──────────────────┤      ├──────────────────┤
│ Batch B: 缓存    │      │ 验收测试         │      │ 文档更新         │
│  架构            │      │                  │      │                  │
│ B-1 Cache key    │      │ 全量 pipeline    │      │ STATUS.md        │
│ B-2 合约文件     │      │ 验证无回归       │      │ wiki 更新        │
└──────────────────┘      └──────────────────┘      └──────────────────┘
```

### 工作量汇总

| Batch | 任务数 | 预估人天 | 并行度 |
|-------|--------|---------|--------|
| A: C++ 修补迁移 | 5 | 5-7 天 | 部分串行 |
| B: 缓存架构 | 2 | 1-1.5 天 | 与 A 并行 |
| C: 工程债务 | 6 | 2-3 天 | 全并行 |
| D: AI 合规 | 4 | 2-3 天 | 半并行 |
| 验收 + 文档 | — | 1 天 | — |
| **总计** | **17** | **11-16 天** | **~3 周** |

---

## 4. AI 合规体系详设

### 4.1 三层门禁架构

```
┌──────────────────────────────────────────────────────┐
│ Layer 1: 开发期 (Hook)                                │
│  触发: Edit/Write 工具调用                            │
│  检查: 目标文件扩展名 + 当前 layer 的权限             │
│  响应: 拦截 + 提示正确的 layer                        │
│  绕过: BOUNDARY_OVERRIDE + ticket link                │
├──────────────────────────────────────────────────────┤
│ Layer 2: 提交期 (pre-commit)                          │
│  触发: git commit                                     │
│  检查: staged files 中的跨层写入                      │
│  响应: 拒绝提交 + 显示违规文件和行号                   │
│  绕过: --no-verify (记录到审计日志)                   │
├──────────────────────────────────────────────────────┤
│ Layer 3: CI 期 (pipeline preflight)                   │
│  触发: CI pipeline 启动                               │
│  检查: 全量扫描 + 历史对比                            │
│  响应: pipeline fail + 发送告警                       │
│  绕过: 无 (安全网，不可绕过)                          │
└──────────────────────────────────────────────────────┘
```

### 4.2 Layer Permission Matrix

| 操作 \ 目标文件 | .generated.cpp/.h | .cpp/.h (non-gen) | .cs | .scriban | .py | CMakeLists.txt | .csproj | .json |
|----------------|-------------------|-------------------|-----|----------|-----|----------------|---------|-------|
| **Codegen** 写入 | ✅ 职责 | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ | ✅ |
| **TPG** 写入 | ❌ 红线 | ✅ 模板 | ❌ | ✅ 创作 | ❌ | ✅ 模板 | ❌ | ✅ metadata |
| **Python** 写入 | ❌ 红线 | ❌ 红线 | ⚠️ 白名单 | ❌ | ✅ 创作 | ⚠️ 白名单 | ⚠️ 白名单 | ✅ |
| **ATG** 写入 | ❌ | ❌ | ✅ 职责 | ❌ | ❌ | ❌ | ✅ csproj | ✅ metadata |

> ⚠️ 白名单 = 已有明确 BOUNDARY_OVERRIDE 标注的可接受行为

### 4.3 AI Agent 开发规范

AI Agent 在执行任何文件写入操作前，必须:

```
1. 识别当前修改所属 layer:
   - src/managed/Chaos.IL2CPP.Generator/ → Codegen
   - src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/ → TPG
   - testing/foundation-dll/verification/ → Python
   - src/tools/Chaos.IL2CPP.Tools.AutoTestGenerator/ → ATG

2. 检查目标文件扩展名:
   - 如果是 .generated.cpp/.h → 只允许 Codegen layer 写入
   - 如果是 .cpp/.h (non-generated) → 只允许 TPG/Scriban 写入
   - 如果是 .py → 只允许 Python layer 写入
   - 如果是 .scriban → 只允许 TPG layer 写入

3. 如果跨层写入是必要的:
   - 在前一行添加 # BOUNDARY_OVERRIDE: https://github.com/.../issues/NNN
   - 在 commit message 中说明为什么不能通过正确 layer 解决
   - 在 PR 描述中标注

4. 如果写入的是 .cs (C# 测试代码):
   - 判断是否应通过 ATG --generate-wrappers 完成
   - 只有当 ATG 命令无法覆盖时才由 Python 直接写
```

### 4.4 新增 BOUNDARY_OVERRIDE 机制

所有需要跨层写入的地方必须标注:

```python
# BOUNDARY_OVERRIDE: https://github.com/.../issues/1234
# Reason: ATG --custom-only not yet implemented, see issue for migration plan
# Expires: 2026-07-15 (after Batch A-4 completes)
_stubs.write_text('...')
```

CI 检查:
- 过期 BOUNDARY_OVERRIDE → warning (不 fail)
- 无 ticket link → fail
- 过期超过 30 天 → fail

---

## 5. 风险评估

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|---------|
| Codegen emitter 修改引入新 bug | 中 | 高 | Batch A-3 必须有独立的 verifiation test，先在 worktree 中验证 |
| Scriban 模板变更被 linter revert | 低 | 中 | 修改模板前先确认 linter 规则，必要时更新 linter 白名单 |
| Cache key 版本化后冷启动时间增长 | 高 | 低 | 只在 codegen/TPG 变更时重构建，正常开发不受影响 |
| ATG --generate-wrappers 与 Python 版本行为不一致 | 中 | 中 | 迁移后删除 Python 代码版本，避免双重维护 |
| Hook/pre-commit 影响正常开发流程 | 低 | 低 | 提供 --no-verify 逃生门，但记录审计日志 |
| BOUNDARY_OVERRIDE 过期无人清理 | 中 | 低 | CI 检查过期 override，超过 30 天自动 fail |

---

## 6. 三优先级权衡

| 维度 | 评估 | 说明 |
|------|------|------|
| **P1 性能最优** | ✅ 不受影响 | 本计划不修改任何运行时 C++ 或 codegen 生成的 C++ 代码的热路径。修补的是构建期和模板层。 |
| **P2 方案完美性** | ✅ 显著提升 | 消除所有跨层修补，四层职责边界清晰。Codegen emitter 不再"带病运行"。 |
| **P3 热更适配** | ✅ 不受影响 | 本计划不涉及 hotupdate 路径。 |

**结论**: 本计划完全兼容三条优先级，无损性能，显著提升架构质量。

---

## 7. 验收标准

| 验收项 | 验证方式 |
|--------|---------|
| Python 不再 write_text 任何 .cpp/.h | git grep '\.write_text.*\.(cpp|h)' verification/ 返回 0 (或全部带 BOUNDARY_OVERRIDE) |
| runtime_entry_patcher.py 已删除 | 文件不存在 |
| entry_stubs.cpp 来自 SDK runtime_stubs/ | TPG CMake 模板引用 SDK 路径 |
| Cache key 包含 codegen/TPG 版本 | hephaestus_cache.py 中 cache key 格式验证 |
| ATG --generate-wrappers 可用 | ATG CLI 测试 |
| Pre-commit hook 安装并生效 | 测试跨层写入被拦截 |
| CI boundary check 作为 preflight 阶段 | CI pipeline 配置确认 |
| BOUNDARY_OVERRIDE 列表已维护 | git grep BOUNDARY_OVERRIDE 输出可控 |
| 全量 pipeline 无回归 | 所有 chunk build+fact 通过 |
