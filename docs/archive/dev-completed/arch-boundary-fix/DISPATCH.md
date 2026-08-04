# DISPATCH.md — 调度总纲

> 架构边界修复 & AI 合规体系
> dispatch_model: hybrid

---

## 批处理定义

### batch-a1: Scriban 模板修复流

| 字段 | 值 |
|------|-----|
| mode | sequential |
| description | Assert stubs → SDK runtime_stubs/ → SEH fix + MACRO → Scriban |
| wait_for | — |
| merge_after | all-completed |
| conflict_check | 两个 task 都修改 Scriban 模板，必须串行 |

**Task 列表**:
| id | conflict_scope | wait_for |
|----|---------------|----------|
| A-1 | chaos-sdk/runtime_stubs/ + TPG/Templates/runtime-entry.cpp.scriban | — |
| A-2 | TPG/Templates/runtime-entry.cpp.scriban + 删除 runtime_entry_patcher.py + 删除 build.py patching | A-1 |

### batch-a3: Codegen + ATG 修复流

| 字段 | 值 |
|------|-----|
| mode | sequential |
| description | Interop stub → Codegen emitter → C# wrapper → ATG → Flat layout fix |
| wait_for | — |
| merge_after | all-completed |

**Task 列表**:
| id | conflict_scope | wait_for |
|----|---------------|----------|
| A-3 | managed/Chaos.IL2CPP.Generator/ + build.py (删除 interop patching) | — |
| A-4 | tools/AutoTestGenerator/ + build.py (删除 _compile_custom_subjects) | — |
| A-5 | TPG/Emission/ + build.py (删除 flat layout copy) | A-3 |

### batch-b: 缓存架构流

| 字段 | 值 |
|------|-----|
| mode | sequential |
| description | Cache key 版本化 + 合约文件 |
| wait_for | — |
| merge_after | all-completed |

**Task 列表**:
| id | conflict_scope | wait_for |
|----|---------------|----------|
| B-1 | verification/stages/hephaestus_cache.py | — |
| B-2 | verification/stages/hephaestus_cache.py + build.py (写入合约) | B-1 |

### batch-c: 工程债务清理流

| 字段 | 值 |
|------|-----|
| mode | parallel |
| description | 6 个独立清理任务 |
| wait_for | Phase 1 completed |
| merge_after | all-completed |
| conflict_check | 每个 task 修改不同文件，无冲突 |

**Task 列表**:
| id | conflict_scope | wait_for |
|----|---------------|----------|
| C-1 | _pipeline/tool_helpers.py + build.py + hotupdate_chunk.py | — |
| C-2 | Codegen/BuildSystem/ | — |
| C-3 | tools/TestProjectGenerator/Program.cs | — |
| C-4 | verification/stages/build.py | — |
| C-5 | .autogen/ 目录 + verification/ | — |
| C-6 | Codegen/Orchestration/ | — |

### batch-d: AI 合规体系流

| 字段 | 值 |
|------|-----|
| mode | hybrid (D-2 启动后 D-1/D-3/D-4 可并行) |
| description | Hook + CI + CLAUDE.md + PR template + 验收 |
| wait_for | Phase 1+2 completed |
| merge_after | all-completed |

**Task 列表**:
| id | conflict_scope | wait_for |
|----|---------------|----------|
| D-1 | .githooks/pre-commit | — |
| D-2 | verification/preflight/check_layer_boundaries.py | — |
| D-3 | CLAUDE.md | D-1, D-2 结构定稿后 |
| D-4 | .github/PULL_REQUEST_TEMPLATE.md | — |
| E-1 | (无文件，仅跑 pipeline) | D-1~D-4 |

## 合并协议

1. batch 内所有 task 完成后，读取各 task STATUS.md
2. 汇总到父 STATUS.md 的 `## 调度状态`
3. 检查下一 batch 的 `wait_for` 是否满足
4. 如果满足，自动推进到下一 phase

## 跨批次文件冲突检查矩阵

| task | 写入路径 | 冲突 task |
|------|---------|-----------|
| A-1 | chaos-sdk/*, TPG/Templates/*.scriban | A-2 (Scriban 同区域) |
| A-2 | TPG/Templates/*.scriban | A-1 (串行解决) |
| A-3 | managed/Chaos.IL2CPP.Generator/*, build.py | — |
| A-4 | tools/AutoTestGenerator/*, build.py | — |
| A-5 | TPG/Emission/*, build.py | A-3 (build.py 同区域, 串行解决) |
| B-1 | hephaestus_cache.py | B-2 (串行解决) |
| C-1 | _pipeline/*, build.py, hotupdate_chunk.py | — |
| C-2 | Codegen/BuildSystem/* | — |
| C-3 | Program.cs | — |
| C-4 | build.py | C-1 (同区域, 需注意) |
| C-5 | .autogen/*, verification/* | — |
| C-6 | Codegen/Orchestration/* | — |
| D-1~D-4 | .githooks/*, CLAUDE.md, PR template | — |
